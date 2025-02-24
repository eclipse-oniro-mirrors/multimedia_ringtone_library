/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
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

#ifndef RINGTONE_XCOLLIE_H
#define RINGTONE_XCOLLIE_H

#include <string>
#include <map>
#include <mutex>

namespace OHOS {
namespace Media {
constexpr uint32_t TIME_OUT_SECONDS_DEFAULT = 3;
constexpr unsigned int RINGTONE_XCOLLIE_FLAG_LOG = (1 << 0);

class RingtoneXCollie {
public:
    explicit RingtoneXCollie(const std::string &tag, std::function<void(void *)> func = nullptr,
        uint32_t timeoutSeconds = TIME_OUT_SECONDS_DEFAULT, void *arg = nullptr,
        uint32_t flag = RINGTONE_XCOLLIE_FLAG_LOG);
    ~RingtoneXCollie();
    void CancelXCollieTimer();
private:
    int32_t id_ = 0;
    std::string tag_ = "";
    bool isCanceled_ = false;
};

} // namespace Media
} // namespace OHOS
#endif