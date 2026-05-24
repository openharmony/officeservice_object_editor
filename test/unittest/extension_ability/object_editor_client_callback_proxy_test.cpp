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
#include "object_editor_client_callback_proxy.h"
#include "object_editor_document.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorClientCallbackProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<MockIRemoteObject> remoteObject_;
    sptr<ObjectEditorClientCallbackProxy> proxy_;
};

void ObjectEditorClientCallbackProxyTest::SetUpTestCase()
{
}

void ObjectEditorClientCallbackProxyTest::TearDownTestCase()
{
}

void ObjectEditorClientCallbackProxyTest::SetUp()
{
    remoteObject_ = sptr<MockIRemoteObject>::MakeSptr();
    ASSERT_NE(remoteObject_, nullptr);
    proxy_ = sptr<ObjectEditorClientCallbackProxy>::MakeSptr(remoteObject_);
    ASSERT_NE(proxy_, nullptr);
    MockMessageParcel::ClearAllErrorFlag();
}

void ObjectEditorClientCallbackProxyTest::TearDown()
{
}

namespace {

/**
 * @tc.name: OnUpdate_DocumentNull
 * @tc.desc: Test OnUpdate with null document
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_DocumentNull, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnUpdate_WriteInterfaceTokenFailed
 * @tc.desc: Test OnUpdate when WriteInterfaceToken fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_WriteInterfaceTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnUpdate_WriteParcelableFailed
 * @tc.desc: Test OnUpdate when WriteParcelable fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_WriteParcelableFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnUpdate_RemoteNull
 * @tc.desc: Test OnUpdate when remote object is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_RemoteNull, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorClientCallbackProxy>::MakeSptr(nullptr);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy->OnUpdate(document);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnUpdate_SendRequestFailed
 * @tc.desc: Test OnUpdate when SendRequest fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_SendRequestFailed, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: OnUpdate_ReadErrCodeFailed
 * @tc.desc: Test OnUpdate when ReadInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_ReadErrCodeFailed, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name: OnUpdate_Success
 * @tc.desc: Test OnUpdate with valid document successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnUpdate_Success, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = proxy_->OnUpdate(document);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnError_WriteInterfaceTokenFailed
 * @tc.desc: Test OnError when WriteInterfaceToken fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_WriteInterfaceTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy_->OnError(error);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnError_WriteInt32Failed
 * @tc.desc: Test OnError when WriteInt32 for error code fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_WriteInt32Failed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy_->OnError(error);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnError_RemoteNull
 * @tc.desc: Test OnError when remote object is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_RemoteNull, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorClientCallbackProxy>::MakeSptr(nullptr);
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy->OnError(error);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnError_SendRequestFailed
 * @tc.desc: Test OnError when SendRequest fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_SendRequestFailed, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy_->OnError(error);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: OnError_ReadErrCodeFailed
 * @tc.desc: Test OnError when ReadInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_ReadErrCodeFailed, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy_->OnError(error);
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name: OnError_Success
 * @tc.desc: Test OnError with valid error code successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnError_Success, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ContentEmbed_ErrorCode error = CE_ERR_OK;
    ErrCode ret = proxy_->OnError(error);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnStopEdit_WriteInterfaceTokenFailed
 * @tc.desc: Test OnStopEdit when WriteInterfaceToken fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_WriteInterfaceTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ErrCode ret = proxy_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnStopEdit_WriteBoolFailed
 * @tc.desc: Test OnStopEdit when WriteBool fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_WriteBoolFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    ErrCode ret = proxy_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnStopEdit_RemoteNull
 * @tc.desc: Test OnStopEdit when remote object is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_RemoteNull, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorClientCallbackProxy>::MakeSptr(nullptr);
    ErrCode ret = proxy->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnStopEdit_SendRequestFailed
 * @tc.desc: Test OnStopEdit when SendRequest fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_SendRequestFailed, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->OnStopEdit(false);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: OnStopEdit_ReadErrCodeFailed
 * @tc.desc: Test OnStopEdit when ReadInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_ReadErrCodeFailed, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ErrCode ret = proxy_->OnStopEdit(false);
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name: OnStopEdit_Success
 * @tc.desc: Test OnStopEdit with valid parameter successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnStopEdit_Success, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->OnStopEdit(false);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnExtensionStopped_WriteInterfaceTokenFailed
 * @tc.desc: Test OnExtensionStopped when WriteInterfaceToken fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnExtensionStopped_WriteInterfaceTokenFailed, TestSize.Level1)
{
    MockMessageParcel::SetWriteInterfaceTokenErrorFlag(true);
    ErrCode ret = proxy_->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnExtensionStopped_RemoteNull
 * @tc.desc: Test OnExtensionStopped when remote object is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnExtensionStopped_RemoteNull, TestSize.Level1)
{
    auto proxy = sptr<ObjectEditorClientCallbackProxy>::MakeSptr(nullptr);
    ErrCode ret = proxy->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: OnExtensionStopped_SendRequestFailed
 * @tc.desc: Test OnExtensionStopped when SendRequest fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnExtensionStopped_SendRequestFailed, TestSize.Level1)
{
    int32_t result = -1;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->OnExtensionStopped();
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: OnExtensionStopped_ReadErrCodeFailed
 * @tc.desc: Test OnExtensionStopped when ReadInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnExtensionStopped_ReadErrCodeFailed, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    MockMessageParcel::SetReadInt32ErrorFlag(true);
    ErrCode errcode = 1;
    MockMessageParcel::AddInt32Cache(errcode);
    ErrCode ret = proxy_->OnExtensionStopped();
    EXPECT_EQ(ret, errcode);
}

/**
 * @tc.name: OnExtensionStopped_Success
 * @tc.desc: Test OnExtensionStopped successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientCallbackProxyTest, OnExtensionStopped_Success, TestSize.Level1)
{
    int32_t result = 0;
    EXPECT_CALL(*remoteObject_, SendRequest(_, _, _, _))
        .Times(1)
        .WillOnce(Return(result));
    ErrCode ret = proxy_->OnExtensionStopped();
    EXPECT_EQ(ret, ERR_OK);
}

}
} // namespace ObjectEditor
} // namespace OHOS