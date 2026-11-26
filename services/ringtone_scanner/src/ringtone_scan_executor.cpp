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

#include "ringtone_scan_executor.h"

#include <thread>

namespace OHOS {
namespace Media {
const size_t MAX_THREAD = 4;
using namespace std;

int32_t RingtoneScanExecutor::Commit(std::shared_ptr<RingtoneScannerObj> scanner)
{
    lock_guard<mutex> lock(queueMtx_);

    queue_.push(move(scanner));

    if (activeThread_ < MAX_THREAD) {
        thread(&RingtoneScanExecutor::HandleScanExecution, this).detach();
        activeThread_++;
    }

    return 0;
}

void RingtoneScanExecutor::HandleScanExecution()
{
    shared_ptr<RingtoneScannerObj> scanner;
    {
        // 修复 bug1（data race）：isScanFinished 是普通 bool，必须与 Stop() 的读/等待
        // 处于同一临界区（queueMtx_）。修复前该写发生在锁外，与 Stop() 的锁内读之间
        // 不存在 happens-before 关系，ARM64 弱内存序下 Stop 可能读到陈旧的 false。
        std::lock_guard<std::mutex> lock(queueMtx_);
        isScanFinished = false;
    }
    while (true) {
        {
            std::lock_guard<std::mutex> lock(queueMtx_);
            if (queue_.empty()) {
                break;
            }

            scanner = std::move(queue_.front());
            activeThread_--;
            queue_.pop();
        }

        scanner->SetStopFlag(stopFlag_);
        (void)scanner->Scan();
    }
    {
        // 修复 bug1（丢失唤醒）：谓词变量 isScanFinished 的写必须与 Stop() 的 check-then-wait
        // 在同一锁内完成，否则会出现两种失败：
        //   1) 写与 notify 都在锁外，Stop 在 ARM64 上读到陈旧 false；
        //   2) notify 落在 Stop 的 check 与 wait 之间，通知被丢弃（无 predicate 的 wait 无法
        //      表达“条件已满足”），Stop 永久阻塞于 stopCond.wait()。
        // 持锁写谓词 + 持锁 notify 是 condition_variable 的标准正确用法。
        std::lock_guard<std::mutex> lock(queueMtx_);
        isScanFinished = true;
        stopCond.notify_one();
    }
}

/* race condition is avoided by the ability life cycle */
void RingtoneScanExecutor::Start()
{
    *stopFlag_ = false;
}

void RingtoneScanExecutor::Stop()
{
    *stopFlag_ = true;

    /* clear all tasks in the queue */
    std::unique_lock<std::mutex> lock(queueMtx_);

    // 修复 bug1（丢失唤醒）：原实现是 if (!isScanFinished) { stopCond.wait(lock); }，
    // 既无 predicate 也无法抵御虚假唤醒。改为 predicate wait 后，wait 在每次被唤醒时
    // 都会在锁内重新读取 isScanFinished，因此：
    //   1) worker 的 notify 即使早于 wait 进入，谓词已为 true，wait 立即返回；
    //   2) 虚假唤醒不会导致提前返回；
    //   3) 谓词变量在锁内被 worker 修改，读写之间有明确的 happens-before。
    stopCond.wait(lock, [this] { return isScanFinished; });

    std::queue<std::shared_ptr<RingtoneScannerObj>> emptyQueue;
    queue_.swap(emptyQueue);
}
} // namespace Media
} // namespace OHOS
