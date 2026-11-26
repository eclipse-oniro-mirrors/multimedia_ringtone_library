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

#ifndef RINGTONE_SCANNER_MANAGER_H
#define RINGTONE_SCANNER_MANAGER_H

#include <mutex>

#include "ringtone_scan_executor.h"

namespace OHOS {
namespace Media {
#define EXPORT __attribute__ ((visibility ("default")))
class RingtoneScannerManager final {
public:
    EXPORT static std::shared_ptr<RingtoneScannerManager> GetInstance();

    RingtoneScannerManager() = default;
    EXPORT virtual ~RingtoneScannerManager() = default;

    EXPORT void Start(bool isSync = true);
    EXPORT void Stop();

    EXPORT int32_t ScanFile(const std::string &path, const std::shared_ptr<IRingtoneScannerCallback> &callback);
    EXPORT int32_t ScanFileSync(const std::string &path, const std::shared_ptr<IRingtoneScannerCallback> &callback,
        bool isForceScan = false);
    EXPORT int32_t ScanDir(const std::string &path, const std::shared_ptr<IRingtoneScannerCallback> &callback);
    EXPORT int32_t ScanDirSync(const std::string &path, const std::shared_ptr<IRingtoneScannerCallback> &callback);
private:
    // 修复 bug4（data race UB）：原实现是双重检查锁定，外层 if (instance_ == nullptr)
    // 在锁外读普通 shared_ptr，与锁内 make_shared 的写之间没有 acquire/release 配对，
    // 属 C++ 标准定义的 data race UB（ARM64 弱内存序下可能读到部分构造的 shared_ptr）。
    // 改用 std::call_once：once_flag 内部带平台相关的同步原语，保证构造与发布对后续
    // 所有调用者可见，且并发首次调用只会构造一次。instance_ 不再需要外层无锁读。
    static std::shared_ptr<RingtoneScannerManager> instance_;
    static std::once_flag instanceOnceFlag_;

    RingtoneScanExecutor executor_;
};
} // namespace Media
} // namespace OHOS

#endif // RINGTONE_SCANNER_MANAGER_H