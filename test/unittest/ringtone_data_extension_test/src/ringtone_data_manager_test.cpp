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
#define MLOG_TAG "FileExtUnitTest"

#include "ringtone_data_manager_test.h"

#include <iostream>

#include "ability_context_impl.h"
#include "datashare_helper.h"
#include "get_self_permissions.h"
#include "iservice_registry.h"
#define private public
#include "ringtone_data_manager.h"
#undef private
#include "ringtone_errno.h"
#include "ringtone_fetch_result.h"
#include "ringtone_file_utils.h"
#include "ringtone_log.h"
#include "ringtone_rdbstore.h"

using namespace std;
using namespace testing::ext;

namespace OHOS {
namespace Media {
const string RINGTONE_LIBRARY_PATH = "/storage/cloud/files/Ringtone";
const string VIBRATE_LIBRARY_PATH = "/storage/cloud/files/Vibrate";
const string MTP_FORMAT_OGG = ".ogg"; // OGG audio files
const string MTP_FORMAT_MP3 = ".mp3"; // MP3 audio files
const string VIBRATE_FORMAT_JSON = ".json";
const string TEST_INSERT_RINGTONE_LIBRARY = "test_insert_ringtone_library";
const string TEST_INSERT_VIBRATE_LIBRARY = "test_insert_ringtone_library";
const int TEST_RINGTONE_COLUMN_SIZE = 1022;
const string RAINNING = "rainning";
const int TEST_RINGTONE_COLUMN_TONE_TYPE = 2;
const int TEST_VIBRATE_COLUMN_VIBRATE_TYPE = 1;
const int TEST_VIBRATE_COLUMN_SOURCE_TYPE = 1;
const int TEST_SIMCARD_SETTING_MODE = 1;
const string MP3 = "mp3";
const string JSON = "json";
const string ERROR_RINGTONE_TABLE = "ThotoFiles";
const string SELECT_STR = " < ? ";

void RingtoneDataManagerUnitTest::SetUpTestCase()
{
    auto stageContext = std::make_shared<AbilityRuntime::ContextImpl>();
    auto abilityContextImpl = std::make_shared<OHOS::AbilityRuntime::AbilityContextImpl>();
    abilityContextImpl->SetStageContext(stageContext);
    auto ret = RingtoneDataManager::GetInstance()->Init(abilityContextImpl);
    CHECK_AND_RETURN_LOG(ret == E_OK, "InitMediaLibraryMgr failed, ret: %{public}d", ret);

    vector<string> perms;
    perms.push_back("ohos.permission.WRITE_RINGTONE");

    uint64_t tokenId = 0;
    RingtonePermissionUtilsUnitTest::SetAccessTokenPermission("RingtoneDataManagerUnitTest", perms, tokenId);
    ASSERT_TRUE(tokenId != 0);
}

void RingtoneDataManagerUnitTest::TearDownTestCase()
{
    system("rm -rf /storage/cloud/files/");
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    dataManager->ClearRingtoneDataMgr();
}

void RingtoneDataManagerUnitTest::SetUp(void)
{
    system("rm -rf /storage/cloud/files/");
    system("mkdir /storage/cloud/files");
    system("mkdir /storage/cloud/files/Ringtone");
}

void RingtoneDataManagerUnitTest::TearDown(void) {}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_001, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_001::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
        static_cast<string>(RINGTONE_LIBRARY_PATH + RINGTONE_SLASH_CHAR +
        TEST_INSERT_RINGTONE_LIBRARY + to_string(0) + MTP_FORMAT_OGG));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_DISPLAY_NAME, static_cast<string>(RAINNING) + MTP_FORMAT_OGG);
    valuesBucket.Put(RINGTONE_COLUMN_TITLE, static_cast<string>(RAINNING));
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_001::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_002::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
        static_cast<string>(RINGTONE_LIBRARY_PATH + RINGTONE_SLASH_CHAR +
        TEST_INSERT_RINGTONE_LIBRARY + to_string(1) + MTP_FORMAT_MP3));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_TONE_TYPE, static_cast<int>(TEST_RINGTONE_COLUMN_TONE_TYPE));
    valuesBucket.Put(RINGTONE_COLUMN_MIME_TYPE, MP3);
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ((retVal > 0), true);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_002::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_003::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA, RINGTONE_DEFAULT_STR);
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_TONE_TYPE, static_cast<int>(TEST_RINGTONE_COLUMN_TONE_TYPE));
    valuesBucket.Put(RINGTONE_COLUMN_MIME_TYPE, MP3);
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ(retVal, E_VIOLATION_PARAMETERS);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_003::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_004, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_004::Start");
    Uri uri(RINGTONE_PATH_URI);
    const string errorRingtoneLibraryPath = "/storage/cloud/files/Photo";
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
        static_cast<string>(errorRingtoneLibraryPath + RINGTONE_SLASH_CHAR +
        TEST_INSERT_RINGTONE_LIBRARY + to_string(0) + MTP_FORMAT_OGG));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_DISPLAY_NAME, static_cast<string>(RAINNING) + MTP_FORMAT_OGG);
    valuesBucket.Put(RINGTONE_COLUMN_TITLE, static_cast<string>(RAINNING));
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ(retVal, E_ERR);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_004::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_005, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_005::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ(retVal, E_INVALID_VALUES);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_005::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_006, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_006::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    dataManager->refCnt_ = 0;
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ(retVal, E_FAIL);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_006::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_007, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_007::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    auto stageContext = std::make_shared<AbilityRuntime::ContextImpl>();
    auto abilityContextImpl = std::make_shared<OHOS::AbilityRuntime::AbilityContextImpl>();
    ASSERT_NE(abilityContextImpl, nullptr);
    abilityContextImpl->SetStageContext(stageContext);
    auto result = dataManager->Init(abilityContextImpl);
    EXPECT_EQ(result, E_OK);
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
        static_cast<string>(RINGTONE_LIBRARY_PATH + RINGTONE_SLASH_CHAR +
        TEST_INSERT_RINGTONE_LIBRARY + to_string(1) + MTP_FORMAT_MP3));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_TONE_TYPE, static_cast<int>(TEST_RINGTONE_COLUMN_TONE_TYPE));
    valuesBucket.Put(RINGTONE_COLUMN_MIME_TYPE, MP3);
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    shared_ptr<RingtoneUnistore> uniStore = RingtoneRdbStore::GetInstance(abilityContextImpl);
    ASSERT_NE(uniStore, nullptr);
    uniStore->Stop();
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    uniStore->Init();
    RINGTONE_INFO_LOG("dataManager_Insert_Test_007::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Insert_Test_008, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Insert_Test_008::Start");
    Uri uri(SIMCARD_SETTING_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(SIMCARD_SETTING_COLUMN_MODE, static_cast<int>(TEST_SIMCARD_SETTING_MODE));
    RingtoneDataCommand cmd(uri, SIMCARD_SETTING_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_TRUE(retVal > 0);
    RINGTONE_INFO_LOG("dataManager_Insert_Test_008::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Test_001, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Test_001::Start");
    vector<string> columns;
    DataShare::DataSharePredicates predicates;
    const string specialStr = " <> ";
    const int index = 3;
    string prefix = RINGTONE_COLUMN_MEDIA_TYPE + specialStr + to_string(index);
    predicates.SetWhereClause(prefix);
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, predicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    EXPECT_EQ(results->GetCount() > 0, true);
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Test_001::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Test_002::Start");
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_TONE_TYPE, to_string(TEST_RINGTONE_COLUMN_TONE_TYPE));
    vector<string> columns;
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Test_002::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Test_003::Start");
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_MIME_TYPE, MP3);
    vector<string> columns = { { RINGTONE_COLUMN_TONE_ID }, { RINGTONE_COLUMN_DISPLAY_NAME },
        { RINGTONE_COLUMN_DATA }, { RINGTONE_COLUMN_SHOT_TONE_TYPE } };
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Test_003::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Test_004, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Test_004::Start");
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_ALARM_TONE_TYPE, to_string(1));
    vector<string> columns = { { RINGTONE_COLUMN_TONE_ID }, { RINGTONE_COLUMN_DISPLAY_NAME },
        { RINGTONE_COLUMN_DATE_ADDED }, { RINGTONE_COLUMN_SHOT_TONE_TYPE } };
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    EXPECT_EQ(results->GetCount() == 0, true);
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Test_004::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Test_005, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Test_005::Start");
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, ERROR_RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    vector<string> columns;
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    EXPECT_EQ(results->GetCount() == 0, true);
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Test_005::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Vibrate_Test_001, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_001::Start");
    vector<string> columns;
    DataShare::DataSharePredicates predicates;
    const string specialStr = " <> ";
    const int index = 1;
    string prefix = VIBRATE_COLUMN_VIBRATE_TYPE + specialStr + to_string(index);
    predicates.SetWhereClause(prefix);
    Uri uri(VIBRATE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, VIBRATE_TABLE, RingtoneOperationType::QUERY);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, predicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<VibrateAsset>>(move(resultSet));
    EXPECT_EQ(results->GetCount() >= 0, true);
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_001::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Vibrate_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_002::Start");
    Uri uri(VIBRATE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, VIBRATE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(VIBRATE_COLUMN_SOURCE_TYPE, to_string(TEST_VIBRATE_COLUMN_SOURCE_TYPE));
    vector<string> columns;
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<VibrateAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_002::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Query_Vibrate_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_003::Start");
    Uri uri(VIBRATE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, VIBRATE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(VIBRATE_COLUMN_VIBRATE_TYPE, to_string(TEST_VIBRATE_COLUMN_VIBRATE_TYPE));
    vector<string> columns = { { VIBRATE_COLUMN_VIBRATE_ID }, { VIBRATE_COLUMN_DISPLAY_NAME },
        { VIBRATE_COLUMN_DATA }, { VIBRATE_COLUMN_VIBRATE_TYPE } };
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<VibrateAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;
    RINGTONE_INFO_LOG("dataManager_Query_Vibrate_Test_003::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Update_Test_001, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Update_Test_001::Start");
    DataShare::DataSharePredicates predicates;

    DataShare::DataShareValuesBucket valuesBucketUpdate;
    valuesBucketUpdate.Put(RINGTONE_COLUMN_TONE_TYPE, 0);

    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::UPDATE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Update(cmd, valuesBucketUpdate, predicates);
    EXPECT_EQ((retVal > 0), true);
    RINGTONE_INFO_LOG("dataManager_Update_Test_001::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Update_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Update_Test_002::Start");
    vector<string> updateIds;
    updateIds.push_back(to_string(1));
    DataShare::DataSharePredicates predicates;

    DataShare::DataShareValuesBucket valuesBucketUpdate;

    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::UPDATE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Update(cmd, valuesBucketUpdate, predicates);
    EXPECT_EQ(retVal, E_INVALID_VALUES);
    RINGTONE_INFO_LOG("dataManager_Update_Test_002::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Update_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Update_Test_003::Start");
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + SELECT_STR);

    DataShare::DataShareValuesBucket valuesBucketUpdate;
    valuesBucketUpdate.Put(RINGTONE_COLUMN_SHOT_TONE_TYPE, 0);
    valuesBucketUpdate.Put(RINGTONE_COLUMN_SHOT_TONE_SOURCE_TYPE, 0);

    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::UPDATE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Update(cmd, valuesBucketUpdate, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Update_Test_003::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Update_Test_004, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Update_Test_004::Start");
    vector<string> updateIds;
    updateIds.push_back(to_string(1));
    DataShare::DataSharePredicates predicates;
    predicates.In(RINGTONE_COLUMN_TONE_ID, updateIds);

    DataShare::DataShareValuesBucket valuesBucketUpdate;

    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::UPDATE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Update(cmd, valuesBucketUpdate, predicates);
    EXPECT_EQ(retVal, E_INVALID_VALUES);
    RINGTONE_INFO_LOG("dataManager_Update_Test_004::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Update_Test_005, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Update_Test_005::Start");
    vector<string> updateIds;
    updateIds.push_back(to_string(1));
    DataShare::DataSharePredicates predicates;
    predicates.In(RINGTONE_COLUMN_TONE_ID, updateIds);

    DataShare::DataShareValuesBucket valuesBucketUpdate;
    valuesBucketUpdate.Put(RINGTONE_COLUMN_SHOT_TONE_TYPE, 0);
    valuesBucketUpdate.Put(RINGTONE_COLUMN_SHOT_TONE_SOURCE_TYPE, 0);

    Uri uri(RINGTONE_PATH_URI);
    RingtoneDataCommand cmd(uri, ERROR_RINGTONE_TABLE, RingtoneOperationType::UPDATE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Update(cmd, valuesBucketUpdate, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Update_Test_005::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_001, TestSize.Level0)
{
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataShareValuesBucket valuesBucket;
    valuesBucket.Put(RINGTONE_COLUMN_DATA,
        static_cast<string>(RINGTONE_LIBRARY_PATH + RINGTONE_SLASH_CHAR +
        TEST_INSERT_RINGTONE_LIBRARY + to_string(1) + MTP_FORMAT_MP3));
    valuesBucket.Put(RINGTONE_COLUMN_SIZE, static_cast<int64_t>(TEST_RINGTONE_COLUMN_SIZE));
    valuesBucket.Put(RINGTONE_COLUMN_TONE_TYPE, static_cast<int>(TEST_RINGTONE_COLUMN_TONE_TYPE));
    valuesBucket.Put(RINGTONE_COLUMN_MIME_TYPE, MP3);
    valuesBucket.Put(RINGTONE_COLUMN_MEDIA_TYPE, static_cast<int>(RINGTONE_MEDIA_TYPE_AUDIO));
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::INSERT);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Insert(cmd, valuesBucket);
    EXPECT_EQ((retVal > 0), true);

    int errCode = 0;
    RingtoneDataCommand cmdQuery(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_MIME_TYPE, MP3);
    vector<string> columns = { { RINGTONE_COLUMN_TONE_ID }, { RINGTONE_COLUMN_DATA } };
    auto queryResultSet = dataManager->Query(cmdQuery, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;

    unique_ptr<RingtoneAsset> ringtoneAsset = results->GetFirstObject();
    if (ringtoneAsset != nullptr && ringtoneAsset->GetPath() != RINGTONE_DEFAULT_STR) {
        string uriStr = RINGTONE_PATH_URI + RINGTONE_SLASH_CHAR + to_string(ringtoneAsset->GetId());
        Uri ofUri(uriStr);
        RingtoneDataCommand cmdOpen(ofUri, RINGTONE_TABLE, RingtoneOperationType::OPEN);
        auto fd = dataManager->OpenFile(cmdOpen, RINGTONE_FILEMODE_READWRITE);
        EXPECT_GE(fd, 0);
        cout << "OpenFile fd = " << fd << endl;
        if (fd >= 0) {
            close(fd);
        }
    }
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_002::Start");
    Uri uri(RINGTONE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(RINGTONE_COLUMN_MIME_TYPE, MP3);
    vector<string> columns = { { RINGTONE_COLUMN_TONE_ID }, { RINGTONE_COLUMN_DATA } };
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    EXPECT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<RingtoneAsset>>(move(resultSet));
    cout << "query count = " << to_string(results->GetCount()) << endl;

    unique_ptr<RingtoneAsset> ringtoneAsset = results->GetFirstObject();
    if (ringtoneAsset != nullptr && ringtoneAsset->GetPath() != RINGTONE_DEFAULT_STR) {
        string uriStr = RINGTONE_PATH_URI + RINGTONE_SLASH_CHAR + to_string(ringtoneAsset->GetId());
        Uri ofUri(uriStr);
        auto dataManager = RingtoneDataManager::GetInstance();
        EXPECT_NE(dataManager, nullptr);
        RingtoneDataCommand cmd(ofUri, ERROR_RINGTONE_TABLE, RingtoneOperationType::OPEN);
        auto fd = dataManager->OpenFile(cmd, RINGTONE_FILEMODE_READWRITE);
        EXPECT_LT(fd, 0);
        cout << "OpenFile fd = " << fd << endl;
        if (fd >= 0) {
            close(fd);
        }
    }
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_002::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_003::Start");
    string uriStr = RINGTONE_PATH_URI + RINGTONE_SLASH_CHAR;
    Uri ofUri(uriStr);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    RingtoneDataCommand cmd(ofUri, RINGTONE_TABLE, RingtoneOperationType::OPEN);
    auto fd = dataManager->OpenFile(cmd, RINGTONE_FILEMODE_READWRITE);
    EXPECT_LT(fd, 0);
    if (fd >= 0) {
        close(fd);
    }
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_003::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_004, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_004::Start");
    string uriStr;
    Uri ofUri(uriStr);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    RingtoneDataCommand cmd(ofUri, RINGTONE_TABLE, RingtoneOperationType::OPEN);
    auto fd = dataManager->OpenFile(cmd, RINGTONE_FILEMODE_READWRITE);
    EXPECT_LT(fd, 0);
    if (fd >= 0) {
        close(fd);
    }
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_004::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_005, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_005::Start");
    Uri ofUri(RINGTONE_DEFAULT_STR);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    RingtoneDataCommand cmd(ofUri, RINGTONE_TABLE, RingtoneOperationType::OPEN);
    auto fd = dataManager->OpenFile(cmd, RINGTONE_FILEMODE_READWRITE);
    EXPECT_LT(fd, 0);
    if (fd >= 0) {
        close(fd);
    }
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_005::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_OpenFile_Test_006, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_OpenFile_Test_006::Start");
    Uri uri(VIBRATE_PATH_URI);
    int errCode = 0;
    RingtoneDataCommand cmd(uri, VIBRATE_TABLE, RingtoneOperationType::QUERY);
    DataShare::DataSharePredicates queryPredicates;
    queryPredicates.EqualTo(VIBRATE_COLUMN_VIBRATE_TYPE, to_string(TEST_VIBRATE_COLUMN_VIBRATE_TYPE));
    vector<string> columns = { { VIBRATE_COLUMN_VIBRATE_ID }, { VIBRATE_COLUMN_DISPLAY_NAME },
        { VIBRATE_COLUMN_DATA }, { VIBRATE_COLUMN_VIBRATE_TYPE } };
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    auto queryResultSet = dataManager->Query(cmd, columns, queryPredicates, errCode);
    ASSERT_NE((queryResultSet == nullptr), true);
    shared_ptr<AbilityRuntime::DataShareResultSet> resultSet =
        make_shared<AbilityRuntime::DataShareResultSet>(queryResultSet);
    auto results = make_unique<RingtoneFetchResult<VibrateAsset>>(move(resultSet));
    ASSERT_NE(results, nullptr);
    cout << "query count = " << to_string(results->GetCount()) << endl;

    unique_ptr<VibrateAsset> vibrateAsset = results->GetFirstObject();
    if (vibrateAsset != nullptr && vibrateAsset->GetPath() != RINGTONE_DEFAULT_STR) {
        string uriStr = VIBRATE_PATH_URI + RINGTONE_SLASH_CHAR + to_string(vibrateAsset->GetId());
        Uri ofUri(uriStr);
        RingtoneDataCommand cmdOpen(ofUri, VIBRATE_TABLE, RingtoneOperationType::OPEN);
        dataManager->OpenFile(cmdOpen, RINGTONE_FILEMODE_READWRITE);
    }
}
  
HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_001, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_001::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataSharePredicates predicates;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ((retVal > 0), true);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_001::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_002, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_002::Start");
    Uri uri(RINGTONE_PATH_URI);
    const int32_t deleteId = 2; // delete id
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + SELECT_STR);
    predicates.SetWhereArgs({ to_string(deleteId) });
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_002::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_003, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_003::Start");
    Uri uri(RINGTONE_PATH_URI);
    const int32_t deleteId = 7; // delete id
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereArgs({ to_string(deleteId) });
    RingtoneDataCommand cmd(uri, ERROR_RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_003::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_004, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_004::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + SELECT_STR);
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_004::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_005, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_005::Start");
    Uri uri(RINGTONE_PATH_URI);
    int32_t deleteId = -1;
    DataShare::DataSharePredicates predicates;
    predicates.SetWhereClause(RINGTONE_COLUMN_TONE_ID + SELECT_STR);
    predicates.SetWhereArgs({ to_string(deleteId) });
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ(retVal, E_HAS_DB_ERROR);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_005::End");
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_006, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_006::Start");
    Uri uri(RINGTONE_PATH_URI);
    DataShare::DataSharePredicates predicates;
    RingtoneDataCommand cmd(uri, RINGTONE_TABLE, RingtoneOperationType::DELETE);
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    dataManager->ClearRingtoneDataMgr();
    dataManager->refCnt_ = 0;
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_EQ(retVal, E_FAIL);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_006::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_Delete_Test_007, TestSize.Level0)
{
    RINGTONE_INFO_LOG("dataManager_Delete_Test_007::Start");
    Uri uri(SIMCARD_SETTING_PATH_URI);
    auto stageContext = std::make_shared<AbilityRuntime::ContextImpl>();
    auto abilityContextImpl = std::make_shared<OHOS::AbilityRuntime::AbilityContextImpl>();
    ASSERT_NE(abilityContextImpl, nullptr);
    abilityContextImpl->SetStageContext(stageContext);
    auto dataManager = RingtoneDataManager::GetInstance();
    ASSERT_NE(dataManager, nullptr);
    auto result = dataManager->Init(abilityContextImpl);
    EXPECT_EQ(result, E_OK);
    DataShare::DataSharePredicates predicates;
    RingtoneDataCommand cmd(uri, SIMCARD_SETTING_TABLE, RingtoneOperationType::DELETE);
    auto retVal = dataManager->Delete(cmd, predicates);
    EXPECT_TRUE(retVal > 0);
    RINGTONE_INFO_LOG("dataManager_Delete_Test_007::retVal = %{public}d. End", retVal);
}

HWTEST_F(RingtoneDataManagerUnitTest, dataManager_GetOwner_Test_001, TestSize.Level0)
{
    auto dataManager = RingtoneDataManager::GetInstance();
    EXPECT_NE(dataManager, nullptr);
    shared_ptr<RingtoneDataShareExtension> datashareExternsion =  nullptr;
    dataManager->SetOwner(datashareExternsion);
    auto stageContext = std::make_shared<AbilityRuntime::ContextImpl>();
    auto abilityContextImpl = std::make_shared<OHOS::AbilityRuntime::AbilityContextImpl>();
    abilityContextImpl->SetStageContext(stageContext);
    auto result = RingtoneDataManager::GetInstance()->Init(abilityContextImpl);
    EXPECT_EQ(result, E_OK);
    dataManager->ClearRingtoneDataMgr();
    auto ret = dataManager->GetOwner();
    EXPECT_EQ(ret, nullptr);
}
} // namespace Media
} // namespace OHOS
