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

#include "ringtone_utils.h"

#include <securec.h>
#include <sys/statvfs.h>

#include "parameter.h"
#include "directory_ex.h"
#include "dfx_const.h"
#include "rdb_sql_utils.h"
#include "ringtone_errno.h"
#include "ringtone_log.h"
#include "ringtone_type.h"
#include "ringtone_db_const.h"
#include "ringtone_rdb_callbacks.h"
#include "preferences_helper.h"
#include "rdb_store_config.h"

namespace OHOS {
namespace Media {
using namespace std;
const char RINGTONE_PARAMETER_SCANNER_COMPLETED_KEY[] = "ringtone.scanner.completed";
const int RINGTONE_PARAMETER_SCANNER_COMPLETED_TRUE = 1;
const int RINGTONE_PARAMETER_SCANNER_COMPLETED_FALSE = 0;
static const int32_t SYSPARA_SIZE = 128;
static const int32_t NO_NEED_SCANNER = 1;
static const int32_t RDB_AREA_EL2 = 2;
constexpr int32_t MAX_SIZE = 9;
const std::string DATA_PATH = "/data/storage/el2/base";
const int32_t VIDEO_FREE_SIZE_LIMIT_10 = 10;
const int64_t UNIT = 1000;
const int64_t STD_UNIT = 1024;
const int64_t THRESHOLD = 512;
const int64_t PERCENTAGE = 100;

std::string RingtoneUtils::ReplaceAll(std::string str, const std::string &oldValue, const std::string &newValue)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += newValue.length()) {
        if ((pos = str.find(oldValue, pos)) != std::string::npos) {
            str.replace(pos, oldValue.length(), newValue);
        } else {
            break;
        }
    }
    return str;
}

std::map<int, std::string> RingtoneUtils::GetDefaultSystemtoneInfo()
{
    map<int, string> defaultSystemtoneInfo;
    char paramValue[SYSPARA_SIZE] = {0};
    GetParameter(PARAM_RINGTONE_SETTING_RINGTONE, "", paramValue, SYSPARA_SIZE);
    if (strlen(paramValue) > 0) {
        defaultSystemtoneInfo.insert(make_pair(DEFAULT_RING_TYPE_SIM_CARD_1, string(paramValue)));
    }

    if (memset_s(paramValue, sizeof(paramValue), 0, sizeof(paramValue)) == 0) {
        GetParameter(PARAM_RINGTONE_SETTING_RINGTONE2, "", paramValue, SYSPARA_SIZE);
        if (strlen(paramValue) > 0) {
            defaultSystemtoneInfo.insert(make_pair(DEFAULT_RING_TYPE_SIM_CARD_2, string(paramValue)));
        }
    }

    if (memset_s(paramValue, sizeof(paramValue), 0, sizeof(paramValue)) == 0) {
        GetParameter(PARAM_RINGTONE_SETTING_SHOT, "", paramValue, SYSPARA_SIZE);
        if (strlen(paramValue) > 0) {
            defaultSystemtoneInfo.insert(make_pair(DEFAULT_SHOT_TYPE_SIM_CARD_1, string(paramValue)));
        }
    }

    if (memset_s(paramValue, sizeof(paramValue), 0, sizeof(paramValue)) == 0) {
        GetParameter(PARAM_RINGTONE_SETTING_SHOT2, "", paramValue, SYSPARA_SIZE);
        if (strlen(paramValue) > 0) {
            defaultSystemtoneInfo.insert(make_pair(DEFAULT_SHOT_TYPE_SIM_CARD_2, string(paramValue)));
        }
    }

    if (memset_s(paramValue, sizeof(paramValue), 0, sizeof(paramValue)) == 0) {
        GetParameter(PARAM_RINGTONE_SETTING_NOTIFICATIONTONE, "", paramValue, SYSPARA_SIZE);
        if (strlen(paramValue) > 0) {
            defaultSystemtoneInfo.insert(make_pair(DEFAULT_NOTIFICATION_TYPE, string(paramValue)));
        }
    }

    if (memset_s(paramValue, sizeof(paramValue), 0, sizeof(paramValue)) == 0) {
        GetParameter(PARAM_RINGTONE_SETTING_ALARM, "", paramValue, SYSPARA_SIZE);
        if (strlen(paramValue) > 0) {
            defaultSystemtoneInfo.insert(make_pair(DEFAULT_ALARM_TYPE, string(paramValue)));
        }
    }

    return defaultSystemtoneInfo;
}

int32_t RingtoneUtils::ChecMoveDb()
{
    auto ret = RingtoneUtils::CheckNeedScanner(COMMON_XML_EL1);
    if (ret == NO_NEED_SCANNER) {
        RINGTONE_INFO_LOG("no need to scanner el1");
        return NO_NEED_SCANNER;
    } else if (ret == E_ERR) {
        RINGTONE_INFO_LOG("open el1 xml error");
        return E_ERR;
    }

    ret = RingtoneUtils::CheckNeedScanner(DFX_COMMON_XML);
    if (ret == E_ERR) {
        RINGTONE_INFO_LOG("open el2 xml error");
        return E_ERR;
    } else if (ret == E_OK) {
        RINGTONE_INFO_LOG("need to scanner el2");
        return E_OK;
    }
    if (!RingtoneUtils::MoveEL2DBToEL1DB()) {
        RINGTONE_INFO_LOG("move error");
        return E_ERR;
    }
    RingtoneUtils::SetMoveEL2DBToEL1();
    return E_OK;
}

int32_t RingtoneUtils::CheckNeedScanner(const std::string &xmlFilePath)
{
    int32_t errCode;
    shared_ptr<NativePreferences::Preferences> prefs =
        NativePreferences::PreferencesHelper::GetPreferences(xmlFilePath, errCode);
    if (!prefs) {
        RINGTONE_ERR_LOG("get el1 preferences error: %{public}d", errCode);
        return E_ERR;
    }
    int isCompleted = prefs->GetInt(RINGTONE_PARAMETER_SCANNER_COMPLETED_KEY,
        RINGTONE_PARAMETER_SCANNER_COMPLETED_FALSE);
    if (!isCompleted) {
        return E_OK;
    }
    return NO_NEED_SCANNER;
}

bool RingtoneUtils::MoveEL2DBToEL1DB()
{
    int32_t errCode = 0;
    RingtoneDataCallBack rdbDataCallBack;
    NativeRdb::RdbStoreConfig config {""};
    config.SetName(RINGTONE_LIBRARY_DB_NAME);
    string realPath = NativeRdb::RdbSqlUtils::GetDefaultDatabasePath(RINGTONE_LIBRARY_DB_PATH,
        RINGTONE_LIBRARY_DB_NAME, errCode);
    config.SetPath(move(realPath));
    config.SetBundleName(RINGTONE_BUNDLE_NAME);
    config.SetArea(RDB_AREA_EL2);
    config.SetSecurityLevel(NativeRdb::SecurityLevel::S3);
    auto rdbStore = NativeRdb::RdbHelper::GetRdbStore(config, RINGTONE_RDB_VERSION,
        rdbDataCallBack, errCode);
    if (rdbStore == nullptr) {
        RINGTONE_ERR_LOG("GetRdbStore is failed , errCode=%{public}d", errCode);
        return false;
    }
    //el2 rdb success
    realPath = NativeRdb::RdbSqlUtils::GetDefaultDatabasePath(RINGTONE_LIBRARY_DB_PATH_EL1,
        RINGTONE_LIBRARY_DB_NAME, errCode);
    RINGTONE_ERR_LOG("rdb Backup, realPath = %{public}s", realPath.c_str());
    auto ret = rdbStore->Backup(realPath);
    RINGTONE_ERR_LOG("rdb Backup, ret = %{public}d", ret);
    return ret != E_OK ? false : true;
}

bool RingtoneUtils::SetMoveEL2DBToEL1()
{
    int32_t errCode;
    shared_ptr<NativePreferences::Preferences> prefs =
        NativePreferences::PreferencesHelper::GetPreferences(COMMON_XML_EL1, errCode);
    if (!prefs) {
        RINGTONE_ERR_LOG("get el1 preferences error: %{public}d", errCode);
        return false;
    }
    prefs->PutInt(RINGTONE_PARAMETER_SCANNER_COMPLETED_KEY, RINGTONE_PARAMETER_SCANNER_COMPLETED_TRUE);
    prefs->FlushSync();
    return true;
}

bool RingtoneUtils::IsNumber(const std::string &str)
{
    if (str.empty()) {
        RINGTONE_ERR_LOG("Input is empty");
        return false;
    }

    if (str.size() > MAX_SIZE) {
        RINGTONE_ERR_LOG("input is too long");
        return false;
    }

    for (char const &c : str) {
        if (isdigit(c) == 0) {
            return false;
        }
    }
    return true;
}

static int64_t GetRoundSize(int64_t size)
{
    uint64_t val = 1;
    int64_t multple = UNIT;
    int64_t stdMultiple = STD_UNIT;
    while (static_cast<int64_t>(val) * stdMultiple < size) {
        val <<= 1;
        if (val > THRESHOLD) {
            val = 1;
            multple *= UNIT;
            stdMultiple *= STD_UNIT;
        }
    }
    return static_cast<int64_t>(val) * multple;
}

int64_t RingtoneUtils::GetTotalSize()
{
    struct statvfs diskInfo;
    int ret = statvfs(DATA_PATH.c_str(), &diskInfo);
    CHECK_AND_RETURN_RET_LOG(ret == 0, E_ERR, "Get total size failed, errno:%{public}d", errno);
    int64_t totalSize = static_cast<long long>(diskInfo.f_bsize) * static_cast<long long>(diskInfo.f_blocks);
    CHECK_AND_RETURN_RET_LOG(totalSize > 0, E_ERR, "Get total size failed, totalSize:%{public}" PRId64, totalSize);
    totalSize = GetRoundSize(totalSize);
    return totalSize;
}

int64_t RingtoneUtils::GetFreeSize()
{
    struct statvfs diskInfo;
    int ret = statvfs(DATA_PATH.c_str(), &diskInfo);
    CHECK_AND_RETURN_RET_LOG(ret == 0, E_ERR, "Get free size failed, errno:%{public}d", errno);
    int64_t freeSize = static_cast<int64_t>(diskInfo.f_bsize) * static_cast<int64_t>(diskInfo.f_bfree);
    return freeSize;
}

bool RingtoneUtils::CheckRemainSpaceMeetCondition()
{
    static int64_t totalSize = RingtoneUtils::GetTotalSize();
    if (totalSize <= 0) {
        totalSize = RingtoneUtils::GetTotalSize();
    }
    CHECK_AND_RETURN_RET_LOG(totalSize > 0, false, "Get total size failed, totalSize:%{public}" PRId64, totalSize);
    int64_t freeSize = RingtoneUtils::GetFreeSize();
    CHECK_AND_RETURN_RET_LOG(freeSize > 0, false, "Get free size failed, freeSize:%{public}" PRId64, freeSize);
    int32_t freeSizePercent = 0;
    if (totalSize != 0) {
        freeSizePercent = static_cast<int32_t>(freeSize * PERCENTAGE / totalSize);
    }
    CHECK_AND_RETURN_RET_LOG(freeSizePercent > VIDEO_FREE_SIZE_LIMIT_10, false,
        "Check free size failed, totalSize:%{public}" PRId64 ", freeSize:%{public}" PRId64, totalSize, freeSize);
    return true;
}
} // namespace Media
} // namespace OHOS
