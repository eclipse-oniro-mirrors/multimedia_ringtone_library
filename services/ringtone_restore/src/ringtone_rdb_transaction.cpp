/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ringtone_rdb_transaction.h"

#include "ringtone_log.h"

namespace OHOS::Media {
using namespace std;
constexpr int32_t E_HAS_DB_ERROR = -222;
constexpr int32_t E_OK = 0;
constexpr int RDB_TRANSACTION_WAIT_MS = 1000;
std::mutex RingtoneRdbTransaction::transactionMutex_;
std::condition_variable RingtoneRdbTransaction::transactionCV_;
std::atomic<bool> RingtoneRdbTransaction::isInTransaction_(false);
constexpr int32_t MAX_TRY_TIMES = 30;
constexpr int32_t TRANSACTION_WAIT_INTERVAL = 50; // in milliseconds.

RingtoneRdbTransaction::RingtoneRdbTransaction(
    const shared_ptr<OHOS::NativeRdb::RdbStore> &rdbStore) : rdbStore_(rdbStore) {}

RingtoneRdbTransaction::~RingtoneRdbTransaction()
{
    if (isStart && !isFinish) {
        TransactionRollback();
    }
}

int32_t RingtoneRdbTransaction::Start()
{
    if (isStart || isFinish) {
        return 0;
    }
    int32_t errCode = BeginTransaction();
    if (errCode == 0) {
        isStart = true;
    }
    return errCode;
}

void RingtoneRdbTransaction::Finish()
{
    if (!isStart) {
        return;
    }
    if (!isFinish) {
        int32_t ret = TransactionCommit();
        if (ret == 0) {
            isFinish = true;
        }
    }
}

void RingtoneRdbTransaction::WaitForTransactionRelease()
{
    // 修复 bug3（持锁长睡眠）：修复前 unique_lock 覆盖整个 BeginTransaction 函数体
    // （含重试循环里的 sleep_for(50ms) 与 rdbStore_->BeginTransaction()），导致
    // TransactionCommit / TransactionRollback 请求同一把锁时被阻塞最长 30×50ms=1500ms。
    // 这里只做一次短临界区的 CV 等待：wait_for 在等待期间会释放 transactionMutex_，
    // 超时或谓词满足后立即返回，不会把锁带出本函数。
    unique_lock<mutex> cvLock(transactionMutex_);
    if (isInTransaction_.load()) {
        transactionCV_.wait_for(cvLock, chrono::milliseconds(RDB_TRANSACTION_WAIT_MS),
            [this] () { return !(this->isInTransaction_.load()); });
    }
}

RingtoneRdbTransaction::BeginResult RingtoneRdbTransaction::TryBeginTransactionOnce(int32_t &curTryTime)
{
    // 锁外睡眠：DB 繁忙时不再持 transactionMutex_ 等待，避免阻塞其他线程清标志。
    if (rdbStore_->IsInTransaction()) {
        this_thread::sleep_for(chrono::milliseconds(TRANSACTION_WAIT_INTERVAL));
    }

    // 修复 bug2（TOCTOU）：把“读标志 → 判定 → 开事务 → 置标志”收敛到同一临界区。
    // 修复前 store(true) 与 TransactionCommit 里的 store(false) 分属不同临界区：
    // Commit 在锁外读标志、加锁后清标志，两步之间可被本函数的 store(true) 插入，
    // 随后被 Commit 的 store(false) 覆写，使本线程已开启的 DB 事务既不 Commit 也不
    // RollBack，泄漏在 RdbStoreImpl 事务栈上（该后果基于 RDB 内部实现推断）。
    // 现在 BeginTransaction 的“开事务+置 true”与 Commit/Rollback 的“判定+提交+置 false”
    // 互斥，标志与底层事务状态不会再出现不一致。
    lock_guard<mutex> lock(transactionMutex_);
    if (isInTransaction_.load() || rdbStore_->IsInTransaction()) {
        curTryTime++;
        RINGTONE_INFO_LOG("RdbStore is in transaction, try %{public}d times...", curTryTime);
        return BeginResult::RETRY;
    }

    int32_t errCode = rdbStore_->BeginTransaction();
    if (errCode == SQLITE3_DATABASE_LOCKER) {
        curTryTime++;
        RINGTONE_ERR_LOG("Sqlite database file is locked! try %{public}d times...", curTryTime);
        return BeginResult::RETRY;
    } else if (errCode != NativeRdb::E_OK) {
        RINGTONE_ERR_LOG("Start Transaction failed, errCode=%{public}d", errCode);
        isInTransaction_.store(false);
        transactionCV_.notify_one();
        return BeginResult::FAILED;
    }

    isInTransaction_.store(true);
    return BeginResult::SUCCESS;
}

int32_t RingtoneRdbTransaction::BeginTransaction()
{
    if (rdbStore_ == nullptr) {
        RINGTONE_ERR_LOG("Pointer rdbStore_ is nullptr. Maybe it didn't init successfully.");
        return E_HAS_DB_ERROR;
    }
    RINGTONE_INFO_LOG("Start transaction");

    // 先等待可能存在的在途事务释放（短临界区，见 WaitForTransactionRelease）。
    WaitForTransactionRelease();

    // 重试循环只负责编排：单次尝试的全部逻辑（锁外睡眠 + 锁内判定/开事务/置标志）
    // 已下沉到 TryBeginTransactionOnce，主函数保持精简。
    int curTryTime = 0;
    while (curTryTime < MAX_TRY_TIMES) {
        BeginResult result = TryBeginTransactionOnce(curTryTime);
        if (result == BeginResult::SUCCESS) {
            return E_OK;
        }
        if (result == BeginResult::FAILED) {
            return E_HAS_DB_ERROR;
        }
    }

    RINGTONE_ERR_LOG("RdbStore is still in transaction after try %{public}d times, abort.", MAX_TRY_TIMES);
    return E_HAS_DB_ERROR;
}

int32_t RingtoneRdbTransaction::TransactionCommit()
{
    if (rdbStore_ == nullptr) {
        return E_HAS_DB_ERROR;
    }
    RINGTONE_INFO_LOG("Try commit transaction");

    int32_t errCode = E_OK;
    {
        // 修复 bug2（TOCTOU）：原实现在锁外读 isInTransaction_ 判定，再加锁清标志，
        // 两步之间另一线程的 BeginTransaction 可 store(true)，随后被这里的 store(false)
        // 覆写，导致对方事务泄漏。现在“判定 → 提交 → 清标志 → 通知”全部在同一临界区内，
        // 复合操作具备原子性，不存在可插入的窗口。
        // 说明：rdbStore_->Commit() 在锁内执行，会短暂延长持锁时间；这是为保证
        // “底层事务状态”与“isInTransaction_ 标志”始终一致所必需的取舍。
        lock_guard<mutex> lock(transactionMutex_);
        if (!(isInTransaction_.load()) || !(rdbStore_->IsInTransaction())) {
            RINGTONE_ERR_LOG("no transaction now");
            return E_HAS_DB_ERROR;
        }

        errCode = rdbStore_->Commit();
        isInTransaction_.store(false);
        transactionCV_.notify_all();
    }
    if (errCode != NativeRdb::E_OK) {
        RINGTONE_ERR_LOG("commit failed, errCode=%{public}d", errCode);
        return E_HAS_DB_ERROR;
    }

    return E_OK;
}

int32_t RingtoneRdbTransaction::TransactionRollback()
{
    if (rdbStore_ == nullptr) {
        return E_HAS_DB_ERROR;
    }
    RINGTONE_INFO_LOG("Try rollback transaction");

    int32_t errCode = E_OK;
    {
        // 修复 bug2（TOCTOU）：与 TransactionCommit 同源问题。原实现的无锁读与加锁清标志
        // 分离，标志可能被并发 BeginTransaction 覆写，导致析构函数中的回滚读到 false 而
        // 早退，底层事务既不提交也不回滚。现统一收敛到同一临界区。
        lock_guard<mutex> lock(transactionMutex_);
        if (!(isInTransaction_.load()) || !(rdbStore_->IsInTransaction())) {
            RINGTONE_ERR_LOG("no transaction now");
            return E_HAS_DB_ERROR;
        }

        errCode = rdbStore_->RollBack();
        isInTransaction_.store(false);
        transactionCV_.notify_all();
    }
    if (errCode != NativeRdb::E_OK) {
        RINGTONE_ERR_LOG("rollback failed, errCode=%{public}d", errCode);
        return E_HAS_DB_ERROR;
    }

    return E_OK;
}
} // namespace OHOS::Media
