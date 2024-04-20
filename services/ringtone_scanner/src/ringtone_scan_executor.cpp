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
}

/* race condition is avoided by the ability life cycle */
void RingtoneScanExecutor::Start()
{
    *stopFlag_ = false;
}

void RingtoneScanExecutor::Stop()
{
    *stopFlag_ = true;

    /* wait for async scan theads to stop */
    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime_));

    /* clear all tasks in the queue */
    std::lock_guard<std::mutex> lock(queueMtx_);
    std::queue<std::shared_ptr<RingtoneScannerObj>> emptyQueue;
    queue_.swap(emptyQueue);
}
} // namespace Media
} // namespace OHOS
