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
#ifndef OHOS_RINGTONE_MIMETYPE_UTILS_H
#define OHOS_RINGTONE_MIMETYPE_UTILS_H

#include "nlohmann/json.hpp"
#include "ringtone_type.h"

namespace OHOS {
namespace Media {
#define EXPORT __attribute__ ((visibility ("default")))
class RingtoneMimeTypeUtils {
public:
    EXPORT RingtoneMimeTypeUtils(const RingtoneMimeTypeUtils&) = delete;
    EXPORT RingtoneMimeTypeUtils(RingtoneMimeTypeUtils&&) = delete;
    EXPORT RingtoneMimeTypeUtils& operator=(const RingtoneMimeTypeUtils&) = delete;
    EXPORT RingtoneMimeTypeUtils& operator=(RingtoneMimeTypeUtils&&) = delete;
    EXPORT static int32_t InitMimeTypeMap();
    EXPORT static std::string GetMimeTypeFromExtension(const std::string &extension);
    EXPORT static std::string GetMimeTypeFromExtension(const std::string &extension,
        const std::unordered_map<std::string, std::vector<std::string>> &mimeTypeMap);
    EXPORT static RingtoneMediaType GetMediaTypeFromMimeType(const std::string &mimeType);

private:
    static void CreateMapFromJson();

    static std::unordered_map<std::string, std::vector<std::string>> mediaJsonMap_;
};
} // namespace Media
} // namespace OHOS
#endif // OHOS_RINGTONE_MIMETYPE_UTILS_H
