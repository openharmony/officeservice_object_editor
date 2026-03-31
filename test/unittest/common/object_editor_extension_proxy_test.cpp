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
#include <iremote_object.h>
#include "hilog_object_editor.h"
#include "mock_i_remote_object.h"
#include "mock_message_parcel.h"
#include "mock_object_editor_client_callback.h"
#include "object_editor_extension_proxy.h"

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
}
class ObjectEditorExtensionProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<MockIRemoteObject> remoteObject_;
    sptr<ObjectEditorExtensionProxy> proxy_;
};

void ObjectEditorExtensionProxyTest::SetUpTestCase()
{
}

void ObjectEditorExtensionProxyTest::TearDownTestCase()
{
}

void ObjectEditorExtensionProxyTest::SetUp()
{
    remoteObject_ = sptr<MockIRemoteObject>::MakeSptr();
    proxy_ = sptr<ObjectEditorExtensionProxy>::MakeSptr(remoteObject_);
    EXPECT_TRUE(proxy_ != nullptr);
    EXPECT_NE(remoteObject_, nullptr);
    g_logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockMessageParcel::ClearAllErrorFlag();
}

void ObjectEditorExtensionProxyTest::TearDown()
{
}

namespace {

/**
 * @tc.name GetSnapshot_001
 * @tc.desc 测试GetSnapshot方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetSnapshot_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ErrCode ret = proxy_->GetSnapshot();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetSnapshot_002
 * @tc.desc 测试GetSnapshot方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetSnapshot_002, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    ErrCode ret = proxy->GetSnapshot();
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetSnapshot_003
 * @tc.desc 测试GetSnapshot方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetSnapshot_003, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->GetSnapshot();
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name GetSnapshot_004
 * @tc.desc 测试GetSnapshot方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetSnapshot_004, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ErrCode ret = proxy_->GetSnapshot();
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name GetSnapshot_005
 * @tc.desc 测试GetSnapshot方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetSnapshot_005, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->GetSnapshot();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name DoEdit_001
 * @tc.desc 测试DoEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, DoEdit_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ErrCode ret = proxy_->DoEdit();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name DoEdit_002
 * @tc.desc 测试DoEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, DoEdit_002, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    ErrCode ret = proxy->DoEdit();
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name DoEdit_003
 * @tc.desc 测试DoEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, DoEdit_003, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->DoEdit();
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name DoEdit_004
 * @tc.desc 测试DoEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, DoEdit_004, TestSize.Level1)
{
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ErrCode ret = proxy_->DoEdit();
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name DoEdit_005
 * @tc.desc 测试DoEdit方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, DoEdit_005, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->DoEdit();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name GetEditStatus_001
 * @tc.desc 测试GetEditStatus方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetEditStatus_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = proxy_->GetEditStatus(&isEditing, &isModified);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetEditStatus_002
 * @tc.desc 测试GetEditStatus方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetEditStatus_002, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = proxy->GetEditStatus(&isEditing, &isModified);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetEditStatus_003
 * @tc.desc 测试GetEditStatus方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetEditStatus_003, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = proxy_->GetEditStatus(&isEditing, &isModified);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name GetEditStatus_004
 * @tc.desc 测试GetEditStatus方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetEditStatus_004, TestSize.Level1)
{
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = proxy_->GetEditStatus(&isEditing, &isModified);
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name GetEditStatus_005
 * @tc.desc 测试GetEditStatus方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetEditStatus_005, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    bool isEditing = false;
    bool isModified = false;
    ErrCode ret = proxy_->GetEditStatus(&isEditing, &isModified);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name GetCapability_001
 * @tc.desc 测试GetCapability方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetCapability_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    uint32_t bitmask = 0;
    ErrCode ret = proxy_->GetCapability(&bitmask);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetCapability_002
 * @tc.desc 测试GetCapability方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetCapability_002, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    uint32_t bitmask = 0;
    ErrCode ret = proxy->GetCapability(&bitmask);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name GetCapability_003
 * @tc.desc 测试GetCapability方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetCapability_003, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    uint32_t bitmask = 0;
    ErrCode ret = proxy_->GetCapability(&bitmask);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name GetCapability_004
 * @tc.desc 测试GetCapability方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetCapability_004, TestSize.Level1)
{
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    uint32_t bitmask = 0;
    ErrCode ret = proxy_->GetCapability(&bitmask);
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name GetCapability_005
 * @tc.desc 测试GetCapability方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, GetCapability_005, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    uint32_t bitmask = 0;
    ErrCode ret = proxy_->GetCapability(&bitmask);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name Close_001
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Close_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ErrCode ret = proxy_->Close();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name Close_002
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Close_002, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    ErrCode ret = proxy->Close();
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name Close_003
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Close_003, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->Close();
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name Close_004
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Close_004, TestSize.Level1)
{
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ErrCode ret = proxy_->Close();
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name Close_005
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Close_005, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->Close();
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name Initial_001
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_001, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrorCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name Initial_002
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_002, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    ErrorCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name Initial_003
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_003, TestSize.Level1)
{
    MockMessageParcel::SetWriteParcelErrorFlag(true);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrorCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name Initial_004
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_004, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorExtensionProxy>::MakeSptr(nullptr);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrorCode ret = proxy->Initial(std::move(document));
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name Initial_005
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_005, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrorCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name Initial_006
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_006, TestSize.Level1)
{
    MockMessageParcel::SendReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name Initial_007
 * @tc.desc 测试Initial方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionProxyTest, Initial_007, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->Initial(std::move(document));
    EXPECT_EQ(ret, ERR_OK);
}

}
}
}