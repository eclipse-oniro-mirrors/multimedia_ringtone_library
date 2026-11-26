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

#define MLOG_TAG "Scanner"

#include "ringtone_scanner_manager.h"

#include "directory_ex.h"
#include "ringtone_log.h"
#include "ringtone_scanner_utils.h"
#include "ringtone_tracer.h"

namespace OHOS {
namespace Media {
std::shared_ptr<RingtoneScannerManager> RingtoneScannerManager::instance_ = nullptr;
std::once_flag RingtoneScannerManager::instanceOnceFlag_;

std::shared_ptr<RingtoneScannerManager> RingtoneScannerManager::GetInstance()
{
    // 修复 bug4（data race UB）：原 DCL 实现在锁外读 instance_，与锁内 make_shared 的写
    // 之间没有 happens-before，C++ 标准判定为 data race UB；ARM64 弱内存序下调用方可能
    // 拿到部分构造的 shared_ptr（对象指针与控制块指针撕裂），解引用后崩溃或堆损坏。
    // std::call_once 保证：并发首次调用只有一个线程执行构造，其余线程阻塞至构造完成，
    // 且构造结果通过 once_flag 内部的同步原语对所有线程可见。构造完成后 call_once
    // 走快速路径，不再有锁竞争。
    std::call_once(instanceOnceFlag_, [] {
        instance_ = std::make_shared<RingtoneScannerManager>();
    });

    return instance_;
}

int32_t RingtoneScannerManager::ScanFile(const std::string &path,
    const std::shared_ptr<IRingtoneScannerCallback> &callback)
{
    RINGTONE_DEBUG_LOG("scan file %{private}s", path.c_str());

    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        RINGTONE_ERR_LOG("failed to get real path %{private}s, errno %{public}d", path.c_str(), errno);
        return E_INVALID_PATH;
    }

    if (!RingtoneScannerUtils::IsRegularFile(realPath)) {
        RINGTONE_ERR_LOG("the path %{private}s is not a regular file", realPath.c_str());
        return E_INVALID_PATH;
    }

    std::unique_ptr<RingtoneScannerObj> scanner =
        std::make_unique<RingtoneScannerObj>(realPath, callback, RingtoneScannerObj::FILE);
    executor_.Commit(move(scanner));

    return E_OK;
}

int32_t RingtoneScannerManager::ScanFileSync(const std::string &path,
    const std::shared_ptr<IRingtoneScannerCallback> &callback, bool isForceScan)
{
    RINGTONE_DEBUG_LOG("scan file %{private}s", path.c_str());

    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        RINGTONE_ERR_LOG("failed to get real path %{private}s, errno %{public}d", path.c_str(), errno);
        return E_INVALID_PATH;
    }

    if (!RingtoneScannerUtils::IsRegularFile(realPath)) {
        RINGTONE_ERR_LOG("the path %{private}s is not a regular file", realPath.c_str());
        return E_INVALID_PATH;
    }

    RingtoneScannerObj scanner = RingtoneScannerObj(realPath, callback, RingtoneScannerObj::FILE);
    if (isForceScan) {
        scanner.SetForceScan(true);
    }
    scanner.Scan();

    return E_OK;
}

int32_t RingtoneScannerManager::ScanDir(const std::string &path,
    const std::shared_ptr<IRingtoneScannerCallback> &callback)
{
    RINGTONE_DEBUG_LOG("scan dir %{private}s", path.c_str());

    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        RINGTONE_ERR_LOG("failed to get real path %{private}s, errno %{public}d", path.c_str(), errno);
        return E_INVALID_PATH;
    }

    if (!RingtoneScannerUtils::IsDirectory(realPath)) {
        RINGTONE_ERR_LOG("the path %{private}s is not a directory", realPath.c_str());
        return E_INVALID_PATH;
    }

    std::unique_ptr<RingtoneScannerObj> scanner = std::make_unique<RingtoneScannerObj>(realPath, callback,
        RingtoneScannerObj::DIRECTORY);
    executor_.Commit(move(scanner));

    return E_OK;
}

int32_t RingtoneScannerManager::ScanDirSync(const std::string &path,
    const std::shared_ptr<IRingtoneScannerCallback> &callback)
{
    RINGTONE_DEBUG_LOG("scan dir %{private}s", path.c_str());

    std::string realPath;
    if (!PathToRealPath(path, realPath)) {
        RINGTONE_ERR_LOG("failed to get real path %{private}s, errno %{public}d", path.c_str(), errno);
        return E_INVALID_PATH;
    }

    if (!RingtoneScannerUtils::IsDirectory(realPath)) {
        RINGTONE_ERR_LOG("the path %{private}s is not a directory", realPath.c_str());
        return E_INVALID_PATH;
    }

    RingtoneScannerObj scanner = RingtoneScannerObj(realPath, callback, RingtoneScannerObj::DIRECTORY);
    scanner.Scan();

    return E_OK;
}

void RingtoneScannerManager::Start(bool isSync)
{
    RingtoneTracer tracer;
    tracer.Start("Ringtone Scanner Start");
    if (isSync) {
        RINGTONE_WARN_LOG("scan start, isSync = %{public}d", isSync);
        RingtoneScannerObj scanner = RingtoneScannerObj(RingtoneScannerObj::START);
        scanner.Scan();
        RINGTONE_WARN_LOG("scan finished");
    } else {
        RINGTONE_WARN_LOG("scan start, isSync = %{public}d", isSync);
        executor_.Start();
        std::shared_ptr<RingtoneScannerObj> scanner = std::make_shared<RingtoneScannerObj>(RingtoneScannerObj::START);
        executor_.Commit(scanner);
        scanner->WaitFor();
        RINGTONE_WARN_LOG("scan finished");
    }
}

void RingtoneScannerManager::Stop()
{
    /* stop all working threads */
    this->executor_.Stop();
}
} // namespace Media
} // namespace OHOS
