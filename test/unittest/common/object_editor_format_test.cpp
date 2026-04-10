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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iremote_broker.h>
#include "hilog_object_editor.h"
#include "mock_i_remote_object.h"
#include "mock_message_parcel.h"
#include "object_editor_format.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {
namespace {
std::string g_logMsg;

void MyLogCallback(const LogType type, const LogLevel level, const unsigned int domain, const char *tag,
    const char *msg)
{
    g_logMsg = msg;
}
int32_t stringErrorCount = -1;
}
class ObjectEditorFormatTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorFormatTest::SetUpTestCase()
{
}

void ObjectEditorFormatTest::TearDownTestCase()
{
}

void ObjectEditorFormatTest::SetUp()
{
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockMessageParcel::ClearAllErrorFlag();
}

void ObjectEditorFormatTest::TearDown()
{
}

namespace {

/**
 * @tc.name Marshalling_001
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_001, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.hmid = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_002
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_002, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.bundleName = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_003
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_003, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.moduleName = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_004
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_004, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.abilityName = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_005
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_005, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.version = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_006
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_006, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.locale = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_007
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_007, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.formatName = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_008
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_008, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.description = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_009
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_009, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetWriteStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetWriteStringErrorFlag(true);
    ObjectEditorFormat format;
    format.fileExts = "test";
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_010
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_010, TestSize.Level1)
{
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    ObjectEditorFormat format;
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_011
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_011, TestSize.Level1)
{
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    ObjectEditorFormat format;
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_012
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_012, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt64ErrorFlag(true);
    ObjectEditorFormat format;
    format.createTime = 1234567890;
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Marshalling_013
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Marshalling_013, TestSize.Level1)
{
    ObjectEditorFormat format;
    Parcel parcel;
    bool ret = format.Marshalling(parcel);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name Unmarshalling_001
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_001, TestSize.Level1)
{
    stringErrorCount = 0;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read hmid fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_002
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_002, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read bundleName fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_003
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_003, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read moduleName fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_004
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_004, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read abilityName fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_005
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_005, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read version fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_006
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_006, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name Unmarshalling_007
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_007, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read formatName fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_008
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_008, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read description fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_009
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_009, TestSize.Level1)
{
    stringErrorCount++;
    MockMessageParcel::SetReadStringErrorFlag(stringErrorCount);
    MockMessageParcel::SetReadStringErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read fileExts fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_010
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_010, TestSize.Level1)
{
    MockMessageParcel::SetReadInt64ErrorFlag(true);
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_EQ(ret, nullptr);
    EXPECT_TRUE(g_logMsg.find("read createTime fail") != string::npos);
}

/**
 * @tc.name Unmarshalling_011
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorFormatTest, Unmarshalling_011, TestSize.Level1)
{
    Parcel parcel;
    auto ret = format.Unmarshalling(parcel);
    EXPECT_NE(ret, nullptr);
}
}
}
}