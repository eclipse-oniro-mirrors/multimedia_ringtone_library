/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ringtone_scanner_test.h"

#include <string>

#include "ringtone_errno.h"
#define private public
#include "ringtone_scanner.h"
#undef private
#include "ringtone_scanner_db.h"
#include "ringtone_scanner_utils.h"
#include "ringtone_unittest_utils.h"

using namespace std;
using namespace OHOS;
using namespace testing::ext;

namespace OHOS {
namespace Media {
void RingtoneScannerTest::SetUpTestCase() {}

void RingtoneScannerTest::TearDownTestCase() {}

// SetUp:Execute before each test case
void RingtoneScannerTest::SetUp() {}

void RingtoneScannerTest::TearDown(void) {}

HWTEST_F(RingtoneScannerTest, scanner_Scan_test_001, TestSize.Level0)
{
    const string path = "/storage/media";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(path, callback, RingtoneScannerObj::FILE);
    ringtoneScannerObj.Scan();
    RingtoneScannerObj ringtoneScannerObjThree(path, callback, RingtoneScannerObj::DIRECTORY);
    ringtoneScannerObjThree.Scan();
    shared_ptr<bool> flag = make_shared<bool>();
    ringtoneScannerObjThree.SetStopFlag(flag);
    RingtoneScannerObj ringtoneScannerObjTest(path, callback, RingtoneScannerObj::START);
    int32_t ret = ringtoneScannerObjTest.GetFileMetadata();
    EXPECT_NE(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_GetFileMetadata_test_001, TestSize.Level0)
{
    const string path = "/data";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(path, callback, RingtoneScannerObj::FILE);
    int32_t ret = ringtoneScannerObj.GetFileMetadata();
    EXPECT_NE(ret, E_OK);
    RingtoneScannerObj mediaScannerObjOne("", callback, RingtoneScannerObj::FILE);
    ret = mediaScannerObjOne.GetFileMetadata();
    EXPECT_EQ(ret, E_INVALID_ARGUMENTS);
}

HWTEST_F(RingtoneScannerTest, scanner_ScanFileInTraversal_test_001, TestSize.Level0)
{
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    const string parent = "ScanDirInternal";
    RingtoneScannerObj ringtoneScannerObj(ROOT_MEDIA_DIR, callback, RingtoneScannerObj::DIRECTORY);
    int32_t ret = ringtoneScannerObj.ScanFileInTraversal(ROOT_MEDIA_DIR);
    EXPECT_NE(ret, E_FILE_HIDDEN);
    const string path = "scanner_ScanDirInternal_test_001/.test";
    const string pathTest = "scanner_ScanDirInternal_test_001/test";
    ret = ringtoneScannerObj.ScanFileInTraversal(path);
    EXPECT_EQ(ret, E_FILE_HIDDEN);
    ret = ringtoneScannerObj.ScanFileInTraversal(pathTest);
    EXPECT_EQ(ret, E_SYSCALL);
    const string dirTest = "/storage/cloud/files";
    ret = ringtoneScannerObj.ScanFileInTraversal(ROOT_MEDIA_DIR);
    EXPECT_NE(ret, E_FILE_HIDDEN);
}

HWTEST_F(RingtoneScannerTest, scanner_WalkFileTree_test_001, TestSize.Level0)
{
    const string dir = "/storage/cloud/files";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    const int errorPath = 4096;
    const string path(errorPath, 'a');
    RingtoneScannerObj ringtoneScannerObj(dir, callback, RingtoneScannerObj::DIRECTORY);
    int32_t ret = ringtoneScannerObj.WalkFileTree(path);
    EXPECT_EQ(ret, ERR_INCORRECT_PATH);
    ret = ringtoneScannerObj.WalkFileTree("");
    EXPECT_EQ(ret, E_PERMISSION_DENIED);
}

HWTEST_F(RingtoneScannerTest, scanner_CleanupDirectory_test_001, TestSize.Level0)
{
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj("", callback, RingtoneScannerObj::DIRECTORY);
    int32_t ret = ringtoneScannerObj.CleanupDirectory();
    EXPECT_EQ(ret, E_OK);
    const string dirTest = "/storage/cloud/files";
    RingtoneScannerObj mediaScannerObjOne(dirTest, callback, RingtoneScannerObj::DIRECTORY);
    ret = mediaScannerObjOne.CleanupDirectory();
    EXPECT_EQ(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_AddToTransaction_test_001, TestSize.Level0)
{
    const string dir = "/storage/cloud/files";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(dir, callback, RingtoneScannerObj::FILE);
    ringtoneScannerObj.GetFileMetadata();
    int32_t ret = ringtoneScannerObj.AddToTransaction();
    EXPECT_EQ(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_CommitTransaction_test_001, TestSize.Level0)
{
    const string dir = "/storage/cloud/files";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(dir, callback, RingtoneScannerObj::FILE);
    ringtoneScannerObj.GetFileMetadata();
    int32_t ret = ringtoneScannerObj.CommitTransaction();
    EXPECT_EQ(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_InvokeCallback_test_001, TestSize.Level0)
{
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj("", callback, RingtoneScannerObj::FILE);
    int32_t ret = ringtoneScannerObj.InvokeCallback(0);
    EXPECT_EQ(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_ScanFileInternal_test_001, TestSize.Level0)
{
    const string path = "scanner_ScanFileInternal_test_001/.test";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(path, callback, RingtoneScannerObj::FILE);
    int32_t ret = ringtoneScannerObj.ScanFileInternal();
    EXPECT_EQ(ret, E_FILE_HIDDEN);
    const string pathOne = "/storage/cloud";
    RingtoneScannerObj mediaScannerObjOne(pathOne, callback, RingtoneScannerObj::FILE);
    ret = mediaScannerObjOne.ScanFileInternal();
    EXPECT_NE(ret, E_FILE_HIDDEN);
}

HWTEST_F(RingtoneScannerTest, scanner_ScanFile_test_001, TestSize.Level0)
{
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(ROOT_MEDIA_DIR, callback, RingtoneScannerObj::FILE);
    int32_t ret = ringtoneScannerObj.ScanFile();
    EXPECT_NE(ret, E_OK);
}

HWTEST_F(RingtoneScannerTest, scanner_Commit_test_001, TestSize.Level0)
{
    const string dir = "/storage";
    shared_ptr<IRingtoneScannerCallback> callback = nullptr;
    RingtoneScannerObj ringtoneScannerObj(dir, callback, RingtoneScannerObj::FILE);
    int32_t ret = ringtoneScannerObj.GetFileMetadata();
    EXPECT_EQ(ret, E_INVALID_ARGUMENTS);
    ret = ringtoneScannerObj.Commit();
    EXPECT_EQ(ret, E_OK);
}
} // namespace Media
} // namespace OHOS