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

#include "ringtone_file_utils_test.h"

#include <algorithm>

#include "ringtone_db_const.h"
#include "ringtone_errno.h"
#include "ringtone_file_utils.h"
#include "ringtone_log.h"
#include "ringtone_type.h"

using std::string;
using namespace testing::ext;

namespace OHOS {
namespace Media {
const string PATH = "ringtone/audio";
const string DEFAULT_STR = "";

void RingtoneFileUtilsTest::SetUpTestCase() {}
void RingtoneFileUtilsTest::TearDownTestCase() {}
void RingtoneFileUtilsTest::SetUp()
{
    system("mkdir /data/local/tmp/test");
}
void RingtoneFileUtilsTest::TearDown(void)
{
    system("rm -rf /data/local/tmp/test");
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_SplitByChar_Test_001, TestSize.Level0)
{
    const char SLASH_CHAR = '/';
    auto str = RingtoneFileUtils::SplitByChar(PATH, SLASH_CHAR);
    ASSERT_EQ(str, "audio");
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_SplitByChar_Test_002, TestSize.Level0)
{
    const char star = '*';
    auto str = RingtoneFileUtils::SplitByChar(PATH, star);
    ASSERT_EQ(str, DEFAULT_STR);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetExtensionFromPath_Test_001, TestSize.Level0)
{
    const string path = "ringtone/audio.mp3";
    const string extension = "mp3";
    auto str = RingtoneFileUtils::GetExtensionFromPath(path);
    ASSERT_EQ(str, extension);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetExtensionFromPath_Test_002, TestSize.Level0)
{
    auto str = RingtoneFileUtils::GetExtensionFromPath(PATH);
    ASSERT_EQ(str, DEFAULT_STR);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_OpenFile_Test_001, TestSize.Level0)
{
    const string path = "/storage/media/100";
    auto ret = RingtoneFileUtils::OpenFile(path, RINGTONE_FILEMODE_WRITEONLY);
    ASSERT_EQ(ret, E_INVALID_PATH);
    ret = RingtoneFileUtils::OpenFile(path, "v");
    ASSERT_EQ(ret, E_INVALID_PATH);
    const int32_t count = 5200;
    string filePath = string(count, 'a') + ".ogg";
    ret = RingtoneFileUtils::OpenFile(filePath, RINGTONE_FILEMODE_WRITEONLY);
    ASSERT_EQ(ret, E_ERR);
    filePath = "ringtoneFileUtils_OpenFile_Test_001.ogg";
    ret = RingtoneFileUtils::OpenFile(filePath, "z");
    ASSERT_EQ(ret, E_ERR);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_MilliSecond_Test_001, TestSize.Level0)
{
    struct timespec t;
    clock_gettime(CLOCK_REALTIME, &t);
    int64_t MilliSecondTime = RingtoneFileUtils::Timespec2Millisecond(t);
    ASSERT_EQ(MilliSecondTime, RingtoneFileUtils::UTCTimeMilliSeconds());
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_StartsWith_Test_001, TestSize.Level0)
{
    const std::string prefix = "audio";
    auto str = RingtoneFileUtils::StartsWith(PATH, prefix);
    ASSERT_EQ(str, false);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_StartsWith_Test_002, TestSize.Level0)
{
    const std::string prefix = "ringtone/audio";
    auto str = RingtoneFileUtils::StartsWith(PATH, prefix);
    ASSERT_EQ(str, true);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_IsFileExists_Test_001, TestSize.Level0)
{
    const string filePath = "/data/test/isfileexists_001";
    EXPECT_EQ(RingtoneFileUtils::IsFileExists(filePath), false);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CreateFile_Test_001, TestSize.Level0)
{
    const string filePath = "/data/local/tmp/test/createfile_001.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_SUCCESS);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CreateFile_Test_002, TestSize.Level0)
{
    string filePath = DEFAULT_STR;
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_VIOLATION_PARAMETERS);
    filePath = "/data/local/tmp/test/createfile_002.pcm";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_INVALID_PATH);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CreateFile_Test_003, TestSize.Level0)
{
    const string filePath = "/data/local/tmp/test/createfile_003.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_SUCCESS);
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_FILE_EXIST);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CreateFile_Test_004, TestSize.Level0)
{
    const string filePath = "/data/local/tmp/test/test/test/test/createfile_004.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_ERR);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetFileNameFromPath_Test_001, TestSize.Level0)
{
    string filePath = "/ringtoneFileUtils_GetFileNameFromPath_Test_001/createfile_001.ogg";
    auto ret = RingtoneFileUtils::GetFileNameFromPath(filePath);
    EXPECT_EQ(ret, "createfile_001.ogg");
    filePath = "/";
    ret = RingtoneFileUtils::GetFileNameFromPath(filePath);
    EXPECT_EQ(ret, "");
    filePath = "";
    ret = RingtoneFileUtils::GetFileNameFromPath(filePath);
    EXPECT_EQ(ret, "");
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetBaseNameFromPath_Test_001, TestSize.Level0)
{
    string filePath = "data/createfile_001.";
    auto ret = RingtoneFileUtils::GetBaseNameFromPath(filePath);
    EXPECT_EQ(ret, "createfile_001");
    filePath = "createfile_001./";
    ret = RingtoneFileUtils::GetBaseNameFromPath(filePath);
    EXPECT_EQ(ret, "");
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_IsSameFile_Test_001, TestSize.Level0)
{
    const string srcPath = "/data/local/tmp/test/createfile_Test_001.ogg";
    const string dstPath = "/data/local/tmp/test/createfile_Test_001.ogg";
    auto ret = RingtoneFileUtils::IsSameFile(srcPath, dstPath);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(RingtoneFileUtils::CreateFile(srcPath), E_SUCCESS);
    ret = RingtoneFileUtils::IsSameFile(srcPath, dstPath);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_IsSameFile_Test_002, TestSize.Level0)
{
    const string srcPath = "/data/local/tmp/test/createfile_Test_001.ogg";
    const string dstPath = "/data/local/tmp/test/createfile_001.ogg";
    auto ret = RingtoneFileUtils::IsSameFile(srcPath, dstPath);
    EXPECT_EQ(ret, false);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_RemoveDirectory_Test_001, TestSize.Level0)
{
    const string filePath = "/data/local/tmp/test/createfile_003.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(filePath), E_SUCCESS);
    auto ret = RingtoneFileUtils::RemoveDirectory(filePath);
    EXPECT_EQ(ret, E_SUCCESS);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_Mkdir_Test_001, TestSize.Level0)
{
    const string path = "ringtoneFileUtils_RemoveDirectory_Test_001";
    std::shared_ptr<int> errCodePtr;
    auto ret = RingtoneFileUtils::Mkdir(path);
    EXPECT_EQ(ret, true);
}
HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_Mkdir_Test_002, TestSize.Level0)
{
    const string path = "/data/local/tmp/test";
    std::shared_ptr<int> errCodePtr;
    auto ret = RingtoneFileUtils::Mkdir(path);
    EXPECT_EQ(ret, true);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_IsDirectory_Test_001, TestSize.Level0)
{
    const string path = "ringtoneFileUtils_RemoveDirectory_Test_001";
    std::shared_ptr<int> errCodePtr;
    auto ret = RingtoneFileUtils::IsDirectory(path, errCodePtr);
    EXPECT_EQ(ret, true);
    const string pathDir = "/data/ringtone/ringtoneFileUtils_RemoveDirectory_Test_001";
    ret = RingtoneFileUtils::IsDirectory(pathDir, errCodePtr);
    EXPECT_EQ(ret, false);
    std::shared_ptr<int> errCode = std::make_shared<int>(1);
    ret = RingtoneFileUtils::IsDirectory(pathDir, errCode);
    EXPECT_EQ(ret, false);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CreateDirectory_Test_001, TestSize.Level0)
{
    string dirPath = "/data/test/isdirempty_002";
    string subPath = dirPath + "/isdirempty_002";
    EXPECT_EQ(RingtoneFileUtils::CreateDirectory(subPath), true);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_MoveFile_Test_001, TestSize.Level0)
{
    string oldPath = "/data/local/tmp/test/movefile_001.ogg";
    string newPath = "/data/local/tmp/test/movefile_001_move.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(oldPath), E_SUCCESS);
    EXPECT_EQ(RingtoneFileUtils::MoveFile(oldPath, newPath), true);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_CopyFileUtil_Test_001, TestSize.Level0)
{
    string oldPath = "/data/local/tmp/test/movefile_001.ogg";
    string newPath = "/data/local/tmp/test/movefile_001_move.ogg";
    EXPECT_EQ(RingtoneFileUtils::CreateFile(oldPath), E_SUCCESS);
    EXPECT_EQ(RingtoneFileUtils::CopyFileUtil(oldPath, newPath), true);
    const string filePath("a", 5200);
    RingtoneFileUtils::CopyFileUtil(filePath, newPath);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetSandboxPath_Test_001, TestSize.Level0)
{
    string uri1 = "/data/app/el2/100/base/com.ohos.ringtonelibrary.ringtonelibrarydata/files/Ringtone";
    string uri2 = "/data/local/tmp/test";
    int32_t result = GetAppSandboxPathFromUri(uri1);
    EXPECT_EQ(result, E_OK);
    result = GetAppSandboxPathFromUri(uri2);
    EXPECT_EQ(result, E_INVALID_URI);
}


HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_OpenVibrateFile_Test_001, TestSize.Level0)
{
    string vibratePath1 = "vibrate/audio.json";
    string mode = "O_WR";
    EXPECT_EQ(RingtoneFileUtils::OpenVibrateFile(vibratePath1, mode), E_ERR);
    const string vibratePath2 = "ringtone/audio.mp3";
    EXPECT_EQ(RingtoneFileUtils::OpenVibrateFile(vibratePath2, mode), E_INVALID_PATH);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_OpenVibrateFile_Test_002, TestSize.Level0)
{
    string vibratePath1 = "";
    string mode1 = "O_RDONLY";
    EXPECT_EQ(RingtoneFileUtils::OpenVibrateFile(vibratePath1, mode1), E_ERR);
    string vibratePath2 = "vibrate/audio.json";
    string mode2 = "";
    EXPECT_EQ(RingtoneFileUtils::OpenVibrateFile(vibratePath2, mode2), E_ERR);
    string vibratePath3 = "vibrate/audio.json";
    string mode3 = "O_RDWR";
    EXPECT_EQ(RingtoneFileUtils::OpenVibrateFile(vibratePath3, mode3), E_ERR);
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_ParseFromUri_Test_002, TestSize.Level0)
{
    string path1 = "content://test";
    bool isTitle = false;
    string fileName1 = RingtoneFileUtils::GetFileNameFromPathOrUri(path1, isTitle);
    EXPECT_TRUE(fileName1.empty());
    string path2 = "content://title/test&data";
    string fileName2 = RingtoneFileUtils::GetFileNameFromPathOrUri(path2, isTitle);
    EXPECT_FALSE(fileName2.empty());
}

HWTEST_F(RingtoneFileUtilsTest, ringtoneFileUtils_GetFileExtension_Test_001, TestSize.Level0)
{
    string path1 = "ringtone/audio.mp3";
    string extension = "mp3";
    string str1 = RingtoneFileUtils::GetFileExtension(path1);
    EXPECT_EQ(str1, extension);
    string path2 = "ringtone/audio";
    string str2 = RingtoneFileUtils::GetFileExtension(path2);
    EXPECT_EQ(str2, "");
}

} // namespace Media
} // namespace OHOS
