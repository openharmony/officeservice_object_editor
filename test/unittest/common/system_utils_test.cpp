/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
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

#include <cstdio>
#include <fcntl.h>
#include <gtest/gtest.h>
#include <fstream>
#include "hilog_object_editor.h"
#include "system_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {
namespace SystemUtils {
namespace {
std::string g_logMsg;

void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *msg)
{
    g_logMsg = msg;
}
}
class SystemUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SystemUtilsTest::SetUpTestCase()
{
}

void SystemUtilsTest::TearDownTestCase()
{
}

void SystemUtilsTest::SetUp()
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
}

void SystemUtilsTest::TearDown()
{
}

/**
 * @tc.name GetFileSize_001
 * @tc.desc 测试GetFileSize方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSize_001, TestSize.Level1)
{
    std::string filePath = "";
    uint64_t fileSize = GetFileSize(filePath);
    EXPECT_EQ(fileSize, 0);
}

/**
 * @tc.name GetFileSize_002
 * @tc.desc 测试GetFileSize方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSize_002, TestSize.Level1)
{
    std::string filePath = "test/../file";
    uint64_t fileSize = GetFileSize(filePath);
    EXPECT_EQ(fileSize, 0);
}

/**
 * @tc.name GetFileSize_003
 * @tc.desc 测试GetFileSize方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSize_003, TestSize.Level1)
{
    std::string filePath = "test/file";
    uint64_t fileSize = GetFileSize(filePath);
    EXPECT_EQ(fileSize, 0);
}

/**
 * @tc.name GetFileSize_004
 * @tc.desc 测试GetFileSize方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSize_004, TestSize.Level1)
{
    std::string filePath = "nonexistentfile";
    uint64_t fileSize = GetFileSize(filePath);
    EXPECT_EQ(fileSize, 0);
}

/**
 * @tc.name GetFileSize_005
 * @tc.desc 测试GetFileSize方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSize_005, TestSize.Level1)
{
    std::string filePath = "testfile.txt";
    const int content = 123;
    int fd = open(filePath.c_str(), O_WRONLY | O_CREAT, 0644);
    write(fd, &content, sizeof(content));
    close(fd);
    uint64_t fileSize = GetFileSize(filePath);
    EXPECT_EQ(fileSize, sizeof(content));
    unlink(filePath.c_str());
}

/**
 * @tc.name ReadFile_001
 * @tc.desc 测试ReadFile方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, ReadFile_001, TestSize.Level1)
{
    std::string filePath = "getfilestream_normal_success.txt";
    const std::string content = "1234567890";
    std::ofstream file(filePath);
    file << content;
    file.close();
    std::string readContent = ReadFile(filePath);
    EXPECT_NE(readContent, content);
    unlink(filePath.c_str());
}

/**
 * @tc.name ReadFile_002
 * @tc.desc 测试ReadFile方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, ReadFile_002, TestSize.Level1)
{
    std::string filePath = "nonexistentfile.txt";
    std::string readContent = ReadFile(filePath);
    EXPECT_EQ(readContent, "");
}

/**
 * @tc.name ReadFile_003
 * @tc.desc 测试ReadFile方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, ReadFile_003, TestSize.Level1)
{
    std::string filePath = "testfile.txt";
    int fd = open(filePath.c_str(), O_WRONLY | O_CREAT, 0644);
    close(fd);
    chmod(filePath.c_str(), 0444);
    std::string readContent = ReadFile(filePath);
    EXPECT_EQ(readContent, "");
    unlink(filePath.c_str());
}

/**
 * @tc.name SplitString_001
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_001, TestSize.Level1)
{
    const std::string str = "a,b,c";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "a");
    EXPECT_EQ(result[1], "b");
    EXPECT_EQ(result[2], "c");
}

/**
 * @tc.name SplitString_002
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_002, TestSize.Level1)
{
    const std::string str = "";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name SplitString_003
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_003, TestSize.Level1)
{
    const std::string str = "aaa";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], "aaa");
}

/**
 * @tc.name SplitString_004
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_004, TestSize.Level1)
{
    const std::string str = ",aaa,bbb";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "");
    EXPECT_EQ(result[1], "aaa");
    EXPECT_EQ(result[2], "bbb");
}

/**
 * @tc.name SplitString_005
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_005, TestSize.Level1)
{
    const std::string str = "aaa,bbb,";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], "aaa");
    EXPECT_EQ(result[1], "bbb");
}

/**
 * @tc.name SplitString_006
 * @tc.desc 测试SplitString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, SplitString_006, TestSize.Level1)
{
    const std::string str = "aaa,,bbb";
    const std::string delimiter = ",";
    std::vector<std::string> result = SplitString(str, delimiter);
    EXPECT_EQ(result.size(), 3);
    EXPECT_EQ(result[0], "aaa");
    EXPECT_EQ(result[1], "");
    EXPECT_EQ(result[2], "bbb");
}

/**
 * @tc.name TrimString_001
 * @tc.desc 测试TrimString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, TrimString_001, TestSize.Level1)
{
    std::string inputStr = "  aaa bbb  ";
    std::string trimmedStr = TrimString(inputStr);
    EXPECT_EQ(trimmedStr, "aaa bbb");
}

/**
 * @tc.name TrimString_002
 * @tc.desc 测试TrimString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, TrimString_002, TestSize.Level1)
{
    std::string inputStr = "    ";
    std::string trimmedStr = TrimString(inputStr);
    EXPECT_EQ(trimmedStr, "");
}

/**
 * @tc.name TrimString_003
 * @tc.desc 测试TrimString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, TrimString_003, TestSize.Level1)
{
    std::string inputStr = "";
    std::string trimmedStr = TrimString(inputStr);
    EXPECT_EQ(trimmedStr, "");
}

/**
 * @tc.name TrimString_004
 * @tc.desc 测试TrimString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, TrimString_004, TestSize.Level1)
{
    std::string inputStr = "HelloWorld";
    std::string trimmedStr = TrimString(inputStr);
    EXPECT_EQ(trimmedStr, "HelloWorld");
}

/**
 * @tc.name TrimString_005
 * @tc.desc 测试TrimString方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, TrimString_005, TestSize.Level1)
{
    std::string inputStr = "Hello  World";
    std::string trimmedStr = TrimString(inputStr);
    EXPECT_EQ(trimmedStr, "Hello  World");
}

/**
 * @tc.name StringToULong_001
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_001, TestSize.Level1)
{
    const char *input = nullptr;
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToULong_002
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_002, TestSize.Level1)
{
    const char *input = "";
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToULong_003
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_003, TestSize.Level1)
{
    const char *input = "123a";
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToULong_004
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_004, TestSize.Level1)
{
    const char *input = "1844692890384626432";
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToULong_005
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_005, TestSize.Level1)
{
    const char *input = "12345";
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, true);
    EXPECT_EQ(num, 12345);
}

/**
 * @tc.name StringToULong_006
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToULong_006, TestSize.Level1)
{
    const char *input = "0";
    unsigned long num = 0;
    bool result = StringToULong(input, num);
    EXPECT_EQ(result, true);
    EXPECT_EQ(num, 0);
}

/**
 * @tc.name StringToLong_001
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_001, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong(nullptr, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToLong_002
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_002, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong("", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToLong_003
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_003, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong("12345", num);
    EXPECT_EQ(result, true);
    EXPECT_EQ(num, 12345);
}

/**
 * @tc.name StringToLong_004
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_004, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong("9999999999999999999999", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToLong_005
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_005, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong("123abc", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToLong_006
 * @tc.desc 测试StringToULong方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToLong_006, TestSize.Level1)
{
    long num = 0;
    bool result = StringToLong("    ", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToFloat_001
 * @tc.desc 测试StringToFloat方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToFloat_001, TestSize.Level1)
{
    float num = 0.0f;
    bool result = StringToFloat(nullptr, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToFloat_002
 * @tc.desc 测试StringToFloat方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToFloat_002, TestSize.Level1)
{
    float num = 0.0f;
    bool result = StringToFloat("12a3", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToFloat_003
 * @tc.desc 测试StringToFloat方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToFloat_003, TestSize.Level1)
{
    float num = 0.0f;
    bool result = StringToFloat("1e400", num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToFloat_004
 * @tc.desc 测试StringToFloat方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToFloat_004, TestSize.Level1)
{
    float num = 0.0f;
    bool result = StringToFloat("123.45", num);
    EXPECT_EQ(result, true);
    EXPECT_NEAR(num, 123.45f, 0.001f);
}

/**
 * @tc.name StringToInt_001
 * @tc.desc 测试StringToInt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToInt_001, TestSize.Level1)
{
    const char *input = "12345";
    int num = 0;
    bool result = StringToInt(input, num);
    EXPECT_EQ(result, true);
    EXPECT_EQ(num, 12345);
}

/**
 * @tc.name StringToInt_002
 * @tc.desc 测试StringToInt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToInt_002, TestSize.Level1)
{
    const char *input = "abc";
    int num = 0;
    bool result = StringToInt(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToInt_003
 * @tc.desc 测试StringToInt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToInt_003, TestSize.Level1)
{
    const char *input = "2147483648";
    int num = 0;
    bool result = StringToInt(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name StringToInt_004
 * @tc.desc 测试StringToInt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, StringToInt_004, TestSize.Level1)
{
    const char *input = "nullptr";
    int num = 0;
    bool result = StringToInt(input, num);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name HasSQLWildcard_001
 * @tc.desc 测试HasSQLWildcard方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, HasSQLWildcard_001, TestSize.Level1)
{
    const std::string input = "abc%def";
    bool result = HasSQLWildcard(input);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name HasSQLWildcard_002
 * @tc.desc 测试HasSQLWildcard方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, HasSQLWildcard_002, TestSize.Level1)
{
    const std::string input = "abc_def";
    bool result = HasSQLWildcard(input);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name HasSQLWildcard_003
 * @tc.desc 测试HasSQLWildcard方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, HasSQLWildcard_003, TestSize.Level1)
{
    const std::string input = "abc%def_ghi";
    bool result = HasSQLWildcard(input);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name HasSQLWildcard_004
 * @tc.desc 测试HasSQLWildcard方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, HasSQLWildcard_004, TestSize.Level1)
{
    const std::string input = "abcdef";
    bool result = HasSQLWildcard(input);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FileExtsHasFileExt_001
 * @tc.desc 测试FileExtsHasFileExt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, FileExtsHasFileExt_001, TestSize.Level1)
{
    const std::string fileExts = "txt|docx|xlsx";
    bool result = FileExtsHasFileExt(fileExts, "docx");
    EXPECT_EQ(result, true);
}

/**
 * @tc.name FileExtsHasFileExt_002
 * @tc.desc 测试FileExtsHasFileExt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, FileExtsHasFileExt_002, TestSize.Level1)
{
    const std::string fileExts = "txt|docx|xlsx";
    bool result = FileExtsHasFileExt(fileExts, "pdf");
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FileExtsHasFileExt_003
 * @tc.desc 测试FileExtsHasFileExt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, FileExtsHasFileExt_003, TestSize.Level1)
{
    const std::string fileExts = "";
    bool result = FileExtsHasFileExt(fileExts, "txt");
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FileExtsHasFileExt_004
 * @tc.desc 测试FileExtsHasFileExt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, FileExtsHasFileExt_004, TestSize.Level1)
{
    const std::string fileExts = "txt|docx|xlsx";
    bool result = FileExtsHasFileExt(fileExts, "");
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FileExtsHasFileExt_005
 * @tc.desc 测试FileExtsHasFileExt方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, FileExtsHasFileExt_005, TestSize.Level1)
{
    const std::string fileExts = "txt||xlsx";
    bool result = FileExtsHasFileExt(fileExts, "txt");
    EXPECT_EQ(result, true);
}

/**
 * @tc.name UTCToBeijingTime_001
 * @tc.desc 测试UTCToBeijingTime方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, UTCToBeijingTime_001, TestSize.Level1)
{
    int64_t timestamp = 1625097600;
    int64_t beijingTimestamp = UTCToBeijingTime(timestamp);
    EXPECT_EQ(beijingTimestamp, "2021-07-01 08:00:00");
}

/**
 * @tc.name UTCToBeijingTime_002
 * @tc.desc 测试UTCToBeijingTime方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, UTCToBeijingTime_002, TestSize.Level1)
{
    int64_t timestamp = 1625097600 + 24 * 3600 - 1;
    int64_t beijingTimestamp = UTCToBeijingTime(timestamp);
    EXPECT_EQ(beijingTimestamp, "2021-07-02 07:59:59");
}

/**
 * @tc.name UTCToBeijingTime_003
 * @tc.desc 测试UTCToBeijingTime方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, UTCToBeijingTime_003, TestSize.Level1)
{
    int64_t timestamp = 1;
    int64_t beijingTimestamp = UTCToBeijingTime(timestamp);
    EXPECT_EQ(beijingTimestamp, "1969-12-32 07:59:59");
}

/**
 * @tc.name GetFileSuffix_001
 * @tc.desc 测试GetFileSuffix方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSuffix_001, TestSize.Level1)
{
    std::string fileName = "test.txt";
    std::string fileSuffix = GetFileSuffix(fileName);
    EXPECT_EQ(fileSuffix, ".txt");
}

/**
 * @tc.name GetFileSuffix_002
 * @tc.desc 测试GetFileSuffix方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSuffix_002, TestSize.Level1)
{
    std::string fileName = "test";
    std::string fileSuffix = GetFileSuffix(fileName);
    EXPECT_EQ(fileSuffix, "");
}

/**
 * @tc.name GetFileSuffix_003
 * @tc.desc 测试GetFileSuffix方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSuffix_003, TestSize.Level1)
{
    std::string fileName = "test.tar.gz";
    std::string fileSuffix = GetFileSuffix(fileName);
    EXPECT_EQ(fileSuffix, ".gz");
}

/**
 * @tc.name GetFileSuffix_004
 * @tc.desc 测试GetFileSuffix方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSuffix_004, TestSize.Level1)
{
    std::string fileName = "test.";
    std::string fileSuffix = GetFileSuffix(fileName);
    EXPECT_EQ(fileSuffix, ".");
}

/**
 * @tc.name GetFileSuffix_005
 * @tc.desc 测试GetFileSuffix方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetFileSuffix_005, TestSize.Level1)
{
    std::string fileName = "FILE.TXT";
    std::string fileSuffix = GetFileSuffix(fileName);
    EXPECT_EQ(fileSuffix, ".txt");
}

/**
 * @tc.name GetUriFromPath_001
 * @tc.desc 测试GetUriFromPath方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetUriFromPath_001, TestSize.Level1)
{
    std::string filePath = "/data/test.txt";
    std::string uri = GetUriFromPath(filePath);
    EXPECT_EQ(uri, "file:///data/test.txt");
}

/**
 * @tc.name GetUriFromPath_002
 * @tc.desc 测试GetUriFromPath方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetUriFromPath_002, TestSize.Level1)
{
    std::string filePath = "";
    std::string uri = GetUriFromPath(filePath);
    EXPECT_EQ(uri, "file://");
}

/**
 * @tc.name GetUriFromPath_003
 * @tc.desc 测试GetUriFromPath方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetUriFromPath_003, TestSize.Level1)
{
    std::string filePath = "test path with spaces 和中文";
    std::string uri = GetUriFromPath(filePath);
    EXPECT_EQ(uri, "file:///test%20path%20with%20spaces%20%E5%92%8C%E4%B8%AD%E6%96%87");
}

/**
 * @tc.name GetPathFromUri_001
 * @tc.desc 测试GetPathFromUri方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetPathFromUri_001, TestSize.Level1)
{
    std::string uri = "file:///data/test.txt";
    std::string filePath = GetPathFromUri(uri);
    EXPECT_EQ(filePath, "/data/test.txt");
}

/**
 * @tc.name GetPathFromUri_002
 * @tc.desc 测试GetPathFromUri方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetPathFromUri_002, TestSize.Level1)
{
    std::string uri = "";
    std::string filePath = GetPathFromUri(uri);
    EXPECT_EQ(filePath, "");
}

/**
 * @tc.name GetPathFromUri_003
 * @tc.desc 测试GetPathFromUri方法
 * @tc.type FUNC
 */
HWTEST_F(SystemUtilsTest, GetPathFromUri_003, TestSize.Level1)
{
    std::string uri = "invalid_uri";
    std::string filePath = GetPathFromUri(uri);
    EXPECT_EQ(filePath, "/invalid_uri");
}

}
}
}