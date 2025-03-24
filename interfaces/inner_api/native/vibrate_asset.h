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

#ifndef VIBRATE_ASSET_H
#define VIBRATE_ASSET_H

#include <mutex>
#include <string>

namespace OHOS {
namespace Media {
#define EXPORT __attribute__ ((visibility ("default")))

class VibrateAsset {
public:
    EXPORT VibrateAsset();
    EXPORT virtual ~VibrateAsset() = default;

    EXPORT int32_t GetId() const;
    EXPORT void SetId(int32_t id);

    EXPORT int64_t GetSize() const;
    EXPORT void SetSize(int64_t size);

    EXPORT const std::string &GetPath() const;
    EXPORT void SetPath(const std::string &path);

    EXPORT const std::string &GetDisplayName() const;
    EXPORT void SetDisplayName(const std::string &displayName);

    EXPORT const std::string &GetTitle() const;
    EXPORT void SetTitle(const std::string &title);

    EXPORT const std::string &GetDisplayLanguage() const;
    EXPORT void SetDisplayLanguage(const std::string &displayLanguage);

    EXPORT int32_t GetVibrateType() const;
    EXPORT void SetVibrateType(int32_t type);

    EXPORT int32_t GetSourceType() const;
    EXPORT void SetSourceType(int32_t type);

    EXPORT int64_t GetDateAdded() const;
    EXPORT void SetDateAdded(int64_t dateAdded);

    EXPORT int64_t GetDateModified() const;
    EXPORT void SetDateModified(int64_t dateModified);

    EXPORT int64_t GetDateTaken() const;
    EXPORT void SetDateTaken(int64_t dateTaken);

    EXPORT void SetPlayMode(int32_t playMode);
    EXPORT int32_t GetPlayMode() const;

    EXPORT void SetScannerFlag(int32_t playMode);
    EXPORT int32_t GetScannerFlag() const;

    EXPORT std::unordered_map<std::string, std::variant<int32_t, int64_t, std::string, double>> &GetMemberMap();
    EXPORT std::variant<int32_t, int64_t, std::string, double> &GetMemberValue(const std::string &name);

private:
    const std::string &GetStrMember(const std::string &name) const;
    int32_t GetInt32Member(const std::string &name) const;
    int64_t GetInt64Member(const std::string &name) const;

    std::unordered_map<std::string, std::variant<int32_t, int64_t, std::string, double>> member_;
};
} // namespace Media
} // namespace OHOS

#endif  // VIBRATE_ASSET_H
