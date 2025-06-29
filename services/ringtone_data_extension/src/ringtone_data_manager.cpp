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

#define MLOG_TAG "DataManager"

#include "ringtone_data_manager.h"

#include "directory_ex.h"
#include "rdb_utils.h"
#include "ringtone_fetch_result.h"
#include "ringtone_file_utils.h"
#include "ringtone_log.h"
#include "ringtone_mimetype_utils.h"
#include "ringtone_rdbstore.h"
#include "ringtone_scanner_manager.h"
#include "ringtone_tracer.h"
#include "ringtone_xcollie.h"
#include "ringtone_utils.h"

namespace OHOS {
namespace Media {
using namespace std;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AbilityRuntime;
using namespace OHOS::NativeRdb;
using namespace OHOS::DataShare;
using namespace OHOS::RdbDataShareAdapter;
shared_ptr<RingtoneDataManager> RingtoneDataManager::instance_ = nullptr;
mutex RingtoneDataManager::mutex_;
shared_ptr<RingtoneUnistore> g_uniStore = nullptr;

RingtoneDataManager::RingtoneDataManager(void)
{
}

RingtoneDataManager::~RingtoneDataManager(void)
{
}

shared_ptr<RingtoneDataManager> RingtoneDataManager::GetInstance()
{
    if (instance_ == nullptr) {
        lock_guard<mutex> lock(mutex_);

        if (instance_ == nullptr) {
            instance_ = make_shared<RingtoneDataManager>();
        }
    }
    return instance_;
}

int32_t RingtoneDataManager::Init(const shared_ptr<OHOS::AbilityRuntime::Context> &context)
{
    RINGTONE_DEBUG_LOG("start");
    lock_guard<shared_mutex> lock(mgrSharedMutex_);

    if (refCnt_.load() > 0) {
        RINGTONE_DEBUG_LOG("already initialized");
        refCnt_++;
        return E_OK;
    }
    context_ = context;
    if (g_uniStore == nullptr) {
        g_uniStore = RingtoneRdbStore::GetInstance(context_);
        if (g_uniStore == nullptr) {
            RINGTONE_ERR_LOG("RingtoneDataManager is not initialized");
            return E_DB_FAIL;
        }
    }
    RingtoneMimeTypeUtils::InitMimeTypeMap();
    refCnt_++;

    RINGTONE_DEBUG_LOG("end");
    return E_OK;
}

int32_t RingtoneDataManager::DeleteRingtoneRowById(int32_t toneId)
{
    const string DELETE_SELECTION = RINGTONE_COLUMN_TONE_ID + " = ? ";
    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    DataSharePredicates predicates;
    vector<string> selectionArgs = { to_string(toneId) };
    predicates.SetWhereClause(DELETE_SELECTION);
    predicates.SetWhereArgs(selectionArgs);

    return this->Delete(cmd, predicates, true);
}

int32_t RingtoneDataManager::Insert(RingtoneDataCommand &cmd, const DataShareValuesBucket &dataShareValue)
{
    RINGTONE_DEBUG_LOG("start");
    shared_lock<shared_mutex> sharedLock(mgrSharedMutex_);
    if (refCnt_.load() <= 0) {
        RINGTONE_ERR_LOG("RingtoneDataManager is not initialized");
        return E_FAIL;
    }
    if (g_uniStore == nullptr) {
        RINGTONE_ERR_LOG("rdb store is not initialized");
        return E_DB_FAIL;
    }

    ValuesBucket value = RdbUtils::ToValuesBucket(dataShareValue);
    if (value.IsEmpty()) {
        RINGTONE_ERR_LOG("RingtoneDataManager Insert: Input parameter is invalid");
        return E_INVALID_VALUES;
    }
    cmd.SetValueBucket(value);
    int32_t err = CheckVideoRingtoneSize(cmd, dataShareValue);
    CHECK_AND_RETURN_RET_LOG(err == E_SUCCESS, err,
        "Check video ringtone size failed, errCode: %{public}d", err);

    int64_t outRowId = E_HAS_DB_ERROR;
    auto retRdb = g_uniStore->Insert(cmd, outRowId);
    if (retRdb != NativeRdb::E_OK) {
        RINGTONE_ERR_LOG("Insert operation failed. Result %{public}d.", retRdb);
        return E_HAS_DB_ERROR;
    }
    if (cmd.GetTableName().compare(SIMCARD_SETTING_TABLE) == 0) {
        return static_cast<int32_t>(outRowId);
    }

    auto asset = GetRingtoneAssetFromId(to_string(outRowId));
    if (asset == nullptr) {
        int32_t deleteRet = DeleteRingtoneRowById(outRowId);
        RINGTONE_ERR_LOG("Failed to get RingtoneAsset, delete row %{public}d from db, return %{public}d",
            static_cast<int32_t>(outRowId), deleteRet);
        return E_INVALID_VALUES;
    }

    auto ret = RingtoneFileUtils::CreateFile(asset->GetPath());
    if (ret != E_SUCCESS) {
        int32_t deleteRet = DeleteRingtoneRowById(outRowId);
        RINGTONE_ERR_LOG("Failed to create file, delete row %{public}d from db, return %{public}d",
            static_cast<int32_t>(outRowId), deleteRet);
        return ret;
    }

    RINGTONE_DEBUG_LOG("end");
    return static_cast<int32_t>(outRowId);
}

int32_t RingtoneDataManager::DeleteFileFromResultSet(std::shared_ptr<NativeRdb::ResultSet> &resultSet)
{
    auto count = 0;
    if (resultSet == nullptr) {
        RINGTONE_ERR_LOG("resultSet is nullptr");
        return E_ERR;
    }
    auto ret = resultSet->GetRowCount(count);
    if (ret != NativeRdb::E_OK) {
        RINGTONE_ERR_LOG("get rdbstore failed");
        return E_HAS_DB_ERROR;
    }
    if (count == 0) {
        RINGTONE_ERR_LOG("have no files to delete");
        return E_SUCCESS;
    }

    shared_ptr<RingtoneFetchResult<RingtoneAsset>> fetchResult = make_shared<RingtoneFetchResult<RingtoneAsset>>();
    if (fetchResult == nullptr) {
        RINGTONE_ERR_LOG("Failed to obtain fetch file result");
        return E_ERR;
    }

    for (auto i = 0; i < count; i++) {
        auto asset = fetchResult->GetObjectFromRdb(resultSet, i);
        if (asset == nullptr) {
            RINGTONE_ERR_LOG("asset is nullptr");
            continue;
        }
        RingtoneFileUtils::DeleteFile(asset->GetPath());
    }

    return E_SUCCESS;
}

int32_t RingtoneDataManager::Delete(RingtoneDataCommand &cmd, const DataSharePredicates &predicates, bool onlyDb)
{
    RINGTONE_DEBUG_LOG("start");
    shared_lock<shared_mutex> sharedLock(mgrSharedMutex_);
    if (refCnt_.load() <= 0) {
        RINGTONE_DEBUG_LOG("RingtoneDataManager is not initialized");
        return E_FAIL;
    }
    if (g_uniStore == nullptr) {
        RINGTONE_ERR_LOG("rdb store is not initialized");
        return E_DB_FAIL;
    }

    NativeRdb::RdbPredicates rdbPredicate = RdbUtils::ToPredicates(predicates, cmd.GetTableName());
    cmd.GetAbsRdbPredicates()->SetWhereClause(rdbPredicate.GetWhereClause());
    cmd.GetAbsRdbPredicates()->SetWhereArgs(rdbPredicate.GetWhereArgs());

    if (cmd.GetTableName().compare(SIMCARD_SETTING_TABLE) == 0) {
        onlyDb = true;
    } else {
        vector<string> columns = {RINGTONE_COLUMN_TONE_ID, RINGTONE_COLUMN_DATA};
        auto absResultSet = g_uniStore->Query(cmd, columns);
        if (!onlyDb) {
            DeleteFileFromResultSet(absResultSet);
        }
    }

    int32_t deletedRows = E_HAS_DB_ERROR;
    int32_t ret = g_uniStore->Delete(cmd, deletedRows);
    if (ret != NativeRdb::E_OK || deletedRows <= 0) {
        RINGTONE_ERR_LOG("Delete operation failed. Result %{public}d.", ret);
        deletedRows = E_HAS_DB_ERROR;
    }

    RINGTONE_DEBUG_LOG("end");
    return deletedRows;
}

int32_t RingtoneDataManager::Update(RingtoneDataCommand &cmd, const DataShareValuesBucket &dataShareValue,
    const DataSharePredicates &predicates)
{
    RINGTONE_DEBUG_LOG("start");
    shared_lock<shared_mutex> sharedLock(mgrSharedMutex_);
    if (refCnt_.load() <= 0) {
        RINGTONE_DEBUG_LOG("RingtoneDataManager is not initialized");
        return E_FAIL;
    }
    if (g_uniStore == nullptr) {
        RINGTONE_ERR_LOG("rdb store is not initialized");
        return E_DB_FAIL;
    }

    ValuesBucket value = RdbUtils::ToValuesBucket(dataShareValue);
    if (value.IsEmpty()) {
        RINGTONE_ERR_LOG("RingtoneDataManager Update:Input parameter is invalid");
        return E_INVALID_VALUES;
    }

    cmd.SetValueBucket(value);
    cmd.SetDataSharePred(predicates);
    NativeRdb::RdbPredicates rdbPredicate = RdbUtils::ToPredicates(predicates, cmd.GetTableName());
    cmd.GetAbsRdbPredicates()->SetWhereClause(rdbPredicate.GetWhereClause());
    cmd.GetAbsRdbPredicates()->SetWhereArgs(rdbPredicate.GetWhereArgs());

    int32_t updatedRows = E_HAS_DB_ERROR;
    int32_t result = g_uniStore->Update(cmd, updatedRows);
    if (result != NativeRdb::E_OK || updatedRows <= 0) {
        RINGTONE_ERR_LOG("Update operation failed. Result %{public}d. Updated %{public}d", result, updatedRows);
        updatedRows = E_HAS_DB_ERROR;
    }

    RINGTONE_DEBUG_LOG("end");
    return updatedRows;
}

shared_ptr<ResultSetBridge> RingtoneDataManager::Query(RingtoneDataCommand &cmd,
    const vector<string> &columns, const DataSharePredicates &predicates, int &errCode)
{
    RINGTONE_DEBUG_LOG("start");
    shared_lock<shared_mutex> sharedLock(mgrSharedMutex_);
    if (refCnt_.load() <= 0) {
        RINGTONE_DEBUG_LOG("RingtoneDataManager is not initialized");
        return nullptr;
    }
    if (g_uniStore == nullptr) {
        RINGTONE_ERR_LOG("rdb store is not initialized");
        return nullptr;
    }

    RingtoneTracer tracer;
    tracer.Start("RingtoneDataManager::Query");
    cmd.SetDataSharePred(predicates);
    NativeRdb::RdbPredicates rdbPredicate = RdbUtils::ToPredicates(predicates, cmd.GetTableName());
    cmd.GetAbsRdbPredicates()->SetWhereClause(rdbPredicate.GetWhereClause());
    cmd.GetAbsRdbPredicates()->SetWhereArgs(rdbPredicate.GetWhereArgs());
    cmd.GetAbsRdbPredicates()->SetOrder(rdbPredicate.GetOrder());
    cmd.GetAbsRdbPredicates()->SetJoinConditions(rdbPredicate.GetJoinConditions());
    cmd.GetAbsRdbPredicates()->SetJoinTableNames(rdbPredicate.GetJoinTableNames());
    cmd.GetAbsRdbPredicates()->SetJoinTypes(rdbPredicate.GetJoinTypes());

    auto absResultSet = g_uniStore->Query(cmd, columns);

    RINGTONE_DEBUG_LOG("end");
    return RdbUtils::ToResultSetBridge(absResultSet);
}

int32_t RingtoneDataManager::OpenRingtoneFile(RingtoneDataCommand &cmd, const string &mode)
{
    RINGTONE_DEBUG_LOG("start");
    RingtoneTracer tracer;
    tracer.Start("RingtoneDataManager::OpenFile");
    string toneId = GetIdFromUri(const_cast<Uri &>(cmd.GetUri()), RINGTONE_URI_PATH);
    auto asset = GetRingtoneAssetFromId(toneId);
    if (asset == nullptr) {
        RINGTONE_ERR_LOG("Failed to get RingtoneAsset");
        return E_INVALID_VALUES;
    }
    if (access(asset->GetPath().c_str(), F_OK) != E_OK) {
        RINGTONE_ERR_LOG("File not exist: %{public}s", asset->GetPath().c_str());
        return E_ERR;
    }
    string absFilePath;
    RingtoneXCollie ringtoneXCollie("RingtoneFile PathToRealPath time out",
        [](void *) {
            RINGTONE_INFO_LOG("RingtoneFile PathToRealPath time out");
        });
    if (!PathToRealPath(asset->GetPath(), absFilePath)) {
        RINGTONE_ERR_LOG("Failed to get real path: %{private}s, err:%{public}d", asset->GetPath().c_str(), errno);
        ringtoneXCollie.CancelXCollieTimer();
        return E_ERR;
    }
    ringtoneXCollie.CancelXCollieTimer();

    RINGTONE_DEBUG_LOG("end");
    return RingtoneFileUtils::OpenFile(absFilePath, mode);
}

int32_t RingtoneDataManager::OpenVibrateFile(RingtoneDataCommand &cmd, const string &mode)
{
    RINGTONE_DEBUG_LOG("start");
    RingtoneTracer tracer;
    tracer.Start("RingtoneDataManager::OpenFile");
    string toneId = GetIdFromUri(const_cast<Uri &>(cmd.GetUri()), VIBRATE_URI_PATH);
    auto asset = GetVibrateAssetFromId(toneId);
    if (asset == nullptr) {
        RINGTONE_ERR_LOG("Failed to get VibrateAsset");
        return E_INVALID_VALUES;
    }

    string absFilePath;
    RingtoneXCollie ringtoneXCollie("VibrateFile PathToRealPath time out",
        [](void *) {
            RINGTONE_INFO_LOG("VibrateFile PathToRealPath time out");
        });
    if (!PathToRealPath(asset->GetPath(), absFilePath)) {
        RINGTONE_ERR_LOG("Failed to get real path: %{private}s", asset->GetPath().c_str());
        ringtoneXCollie.CancelXCollieTimer();
        return E_ERR;
    }
    ringtoneXCollie.CancelXCollieTimer();
    RINGTONE_DEBUG_LOG("end");
    return RingtoneFileUtils::OpenVibrateFile(absFilePath, mode);
}

int32_t RingtoneDataManager::OpenFile(RingtoneDataCommand &cmd, const string &mode)
{
    RINGTONE_INFO_LOG("openfile uri %{public}s", cmd.GetUri().ToString().c_str());
    RINGTONE_INFO_LOG("openfile table %{public}s", cmd.GetTableName().c_str());
    if (cmd.GetTableName() == RINGTONE_TABLE) {
        return OpenRingtoneFile(cmd, mode);
    } else {
        return OpenVibrateFile(cmd, mode);
    }
}

void RingtoneDataManager::SetOwner(const shared_ptr<RingtoneDataShareExtension> &datashareExternsion)
{
    extension_ = datashareExternsion;
}

shared_ptr<RingtoneDataShareExtension> RingtoneDataManager::GetOwner()
{
    return extension_;
}

string RingtoneDataManager::GetIdFromUri(Uri &uri, const std::string &uriPath)
{
    vector<string> segments;
    uri.GetPathSegments(segments);
    const int uriSegmentsCount = 3;
    const int toneIdSegmentNumber = 2;
    if (segments.size() != uriSegmentsCount || segments[1] != uriPath ||
        !RingtoneUtils::IsNumber(segments[toneIdSegmentNumber])) {
        return {};
    }
    return segments[toneIdSegmentNumber];
}

shared_ptr<RingtoneAsset> RingtoneDataManager::GetRingtoneAssetFromId(const string &id)
{
    if ((id.empty()) || (!RingtoneUtils::IsNumber(id))) {
        RINGTONE_ERR_LOG("Id for the path is incorrect: %{private}s", id.c_str());
        return nullptr;
    }
    Uri uri("");
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    cmd.GetAbsRdbPredicates()->EqualTo(RINGTONE_COLUMN_TONE_ID, id);

    auto resultSet = g_uniStore->Query(cmd, {RINGTONE_COLUMN_TONE_ID, RINGTONE_COLUMN_DATA});
    if (resultSet == nullptr) {
        RINGTONE_ERR_LOG("Failed to obtain file asset from database");
        return nullptr;
    }

    shared_ptr<RingtoneFetchResult<RingtoneAsset>> fetchResult = make_shared<RingtoneFetchResult<RingtoneAsset>>();
    if (fetchResult == nullptr) {
        RINGTONE_ERR_LOG("Failed to obtain fetch file result");
        return nullptr;
    }
    return fetchResult->GetObjectFromRdb(resultSet, 0);
}

std::shared_ptr<VibrateAsset> RingtoneDataManager::GetVibrateAssetFromId(const std::string &id)
{
    if ((id.empty()) || (!RingtoneUtils::IsNumber(id))) {
        RINGTONE_ERR_LOG("Id for the path is incorrect: %{private}s", id.c_str());
        return nullptr;
    }
    Uri uri("");
    RingtoneDataCommand cmd(uri, VIBRATE_TABLE, RingtoneOperationType::QUERY);
    cmd.GetAbsRdbPredicates()->EqualTo(VIBRATE_COLUMN_VIBRATE_ID, id);

    auto resultSet = g_uniStore->Query(cmd, {VIBRATE_COLUMN_VIBRATE_ID, VIBRATE_COLUMN_DATA});
    if (resultSet == nullptr) {
        RINGTONE_ERR_LOG("Failed to obtain file asset from database");
        return nullptr;
    }

    shared_ptr<RingtoneFetchResult<VibrateAsset>> fetchResult = make_shared<RingtoneFetchResult<VibrateAsset>>();
    if (fetchResult == nullptr) {
        RINGTONE_ERR_LOG("Failed to obtain fetch file result");
        return nullptr;
    }
    return fetchResult->GetObjectFromRdb(resultSet, 0);
}

void RingtoneDataManager::ClearRingtoneDataMgr()
{
    lock_guard<shared_mutex> lock(mgrSharedMutex_);

    refCnt_--;
    if (refCnt_.load() > 0) {
        RINGTONE_DEBUG_LOG("still other extension exist");
        return;
    }

    extension_ = nullptr;
}

int32_t RingtoneDataManager::CheckVideoRingtoneSize(RingtoneDataCommand &cmd, const DataShareValuesBucket &value)
{
    if (value.IsEmpty()) {
        RINGTONE_ERR_LOG("CheckVideoRingtoneSize failed, value is empty");
        return E_INVALID_VALUES;
    }
    if (cmd.GetTableName().compare(RINGTONE_TABLE) != 0) {
        return E_OK;
    }
    bool isValid = false;
    int32_t mediaType = value.Get(RINGTONE_COLUMN_MEDIA_TYPE, isValid);
    if (!isValid) {
        RINGTONE_ERR_LOG("CheckVideoRingtoneSize failed, media type is not valid");
        return E_INVALID_VALUES;
    }
    if (mediaType == RINGTONE_MEDIA_TYPE_VIDEO) {
        if (!RingtoneUtils::CheckRemainSpaceMeetCondition()) {
            RINGTONE_ERR_LOG("CheckVideoRingtoneSize failed, not enough space");
            return E_NOT_ENOUGH_ROM;
        }
        std::vector<std::string> columns = { "count(1) AS count" };
        Uri uri("");
        RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
        cmd.GetAbsRdbPredicates()->EqualTo(RINGTONE_COLUMN_MEDIA_TYPE, RINGTONE_MEDIA_TYPE_VIDEO);
        auto resultSet = g_uniStore->Query(cmd, columns);
        if (resultSet == nullptr || resultSet->GoToFirstRow() != NativeRdb::E_OK) {
            RINGTONE_ERR_LOG("Query video ringtone count failed");
            return E_INVALID_VALUES;
        }
        int isIndex = -1;
        int32_t num = 0;
        resultSet->GetColumnIndex("count", isIndex);
        resultSet->GetInt(isIndex, num);
        resultSet->Close();
        if (num >= RINGTONE_VIDEO_MAX_COUNT) {
            RINGTONE_ERR_LOG("CheckVideoRingtoneSize failed, video ringtone count exceeds limit");
            return E_VIDEOS_NUM_EXCEEDS_SPECIFICATION;
        }
    } else if (mediaType == RINGTONE_MEDIA_TYPE_AUDIO) {
        return E_OK;
    } else {
        RINGTONE_ERR_LOG("CheckVideoRingtoneSize failed, media type is not video or audio");
        return E_INVALID_VALUES;
    }
    return E_OK;
}
}  // namespace Media
}  // namespace OHOS
