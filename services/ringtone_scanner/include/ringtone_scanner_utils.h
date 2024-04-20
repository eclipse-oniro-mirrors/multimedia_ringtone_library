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

#ifndef RINGTONE_SCANNER_UTILS_H
#define RINGTONE_SCANNER_UTILS_H

#include <string>
#include <sys/stat.h>

namespace OHOS {
namespace Media {
#define EXPORT __attribute__ ((visibility ("default")))
enum EXPORT ErrorCodes {
    ERR_FAIL = -1,
    ERR_SUCCESS,
    ERR_EMPTY_ARGS,
    ERR_NOT_ACCESSIBLE,
    ERR_INCORRECT_PATH,
    ERR_MEM_ALLOC_FAIL,
    ERR_MIMETYPE_NOTSUPPORT,
    ERR_SCAN_NOT_INIT
};

const int32_t FILE_ID_DEFAULT = 0;
constexpr int32_t MAX_BATCH_SIZE = 5;

constexpr int32_t UNKNOWN_ID = -1;

// Const for Metadata defaults
const int32_t TONE_ID_DEFAULT = 0;
const std::string DATA_DEFAULT = "";
const int64_t SIZE_DEFAULT = 0;
const std::string DISPLAY_NAME_DEFAULT = "";
const std::string TITLE_DEFAULT = "";
const int32_t METADATA_MEDIA_TYPE_DEFAULT = 2;
const int32_t TONE_TYPE_DEFAULT = 0;
const std::string MIME_TYPE_DEFAULT = "";
const int32_t SOURCE_TYPE_DEFAULT = 0;
const int64_t DATE_ADDED_DEFAULT = 0;
const int64_t DATE_MODIFIED_DEFAULT = 0;
const int64_t DATE_TAKEN_DEFAULT = 0;
const int32_t DURATION_DEFAULT = 0;
const int32_t SHOT_TONE_TYPE_DEFAULT = 0;
const int32_t SHOT_TONE_SOURCE_TYPE_DEFAULT = 2;
const int32_t NOTIFICATION_TONE_TYPE_DEFAULT = 0;
const int32_t NOTIFICATION_TONE_SOURCE_TYPE_DEFAULT = 2;
const int32_t RING_TONE_TYPE_DEFAULT = 0;
const int32_t RING_TONE_SOURCE_TYPE_DEFAULT = 2;
const int32_t ALARM_TONE_TYPE_DEFAULT = 0;
const int32_t ALARM_TONE_SOURCE_TYPE_DEFAULT = 2;

const std::string DEFAULT_AUDIO_MIME_TYPE = "audio/*";

static std::vector<std::string> EXTRACTOR_SUPPORTED_MIME = {
    DEFAULT_AUDIO_MIME_TYPE,
};

class RingtoneScannerUtils {
public:
    EXPORT RingtoneScannerUtils();
    EXPORT ~RingtoneScannerUtils();

    EXPORT static bool IsExists(const std::string &path);
    EXPORT static std::string GetFileNameFromUri(const std::string &path);
    EXPORT static std::string GetFileExtension(const std::string &path);
    EXPORT static std::string GetParentPath(const std::string &path);
    EXPORT static bool IsFileHidden(const std::string &path);

    EXPORT static bool IsDirectory(const std::string &path);
    EXPORT static bool IsRegularFile(const std::string &path);

    EXPORT static std::string GetFileTitle(const std::string &displayName);
    EXPORT static bool IsDirHiddenRecursive(const std::string &path);
    EXPORT static bool IsDirHidden(const std::string &path);
};
} // namespace Media
} // namespace OHOS

#endif // RINGTONE_SCANNER_UTILS_H
