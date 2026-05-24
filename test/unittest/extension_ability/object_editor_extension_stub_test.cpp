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
#define protected public
#include "mock_hilog.h"
#include "mock_message_parcel.h"
#include "mock_object_editor_extension_stub.h"
#include "mock_object_editor_client_callback.h"
#include "object_editor_document.h"
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

ErrCode MockObjectEditorExtensionStub::GetSnapshot(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{public}s", documentId.c_str());
    return error;
}

ErrCode MockObjectEditorExtensionStub::DoEdit(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{public}s", documentId.c_str());
    return error;
}

ErrCode MockObjectEditorExtensionStub::GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{public}s", documentId.c_str());
    return error;
}

ErrCode MockObjectEditorExtensionStub::GetExtensionEditStatus(bool &isEditing)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "call");
    return error;
}

ErrCode MockObjectEditorExtensionStub::GetCapability(const std::string &documentId, uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{public}s", documentId.c_str());
    return error;
}

ErrCode MockObjectEditorExtensionStub::Close(const std::string &documentId, bool &isAllObjectsRemoved)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{public}s", documentId.c_str());
    return error;
}

ErrCode MockObjectEditorExtensionStub::Initial(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "call");
    return error;
}

int32_t MockObjectEditorExtensionStub::CallbackEnter([[maybe_unused]] uint32_t code)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "CallbackEnter code: %{public}u", code);
    return callbackEnterError;
}

int32_t MockObjectEditorExtensionStub::CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "CallbackExit code: %{public}u, result: %{public}d", code,
        result);
    return error;
}

class ObjectEditorExtensionStubTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<MockObjectEditorExtensionStub> stub_;
};

void ObjectEditorExtensionStubTest::SetUpTestCase() {}

void ObjectEditorExtensionStubTest::TearDownTestCase() {}

void ObjectEditorExtensionStubTest::SetUp()
{
    stub_ = sptr<MockObjectEditorExtensionStub>::MakeSptr();
    ASSERT_NE(stub_, nullptr);
    stub_->error = ERR_OK;
    stub_->callbackEnterError = ERR_OK;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockMessageParcel::ClearAllErrorFlag();
}

void ObjectEditorExtensionStubTest::TearDown() {}

namespace {

/**
 * @tc.name: OnRemoteRequest_CallbackEnterFailed
 * @tc.desc: Test OnRemoteRequest when CallbackEnter returns ERR_TRANSACTION_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequest_CallbackEnterFailed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    stub_->callbackEnterError = ERR_TRANSACTION_FAILED;
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnRemoteRequest_For_CallbackEnter
 * @tc.desc: Test OnRemoteRequest for CallbackEnter return error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequest_For_CallbackEnter, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    stub_->callbackEnterError = ERR_INVALID_VALUE;
    stub_->error = ERR_TRANSACTION_FAILED;
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: OnRemoteRequest_CallbackExitFailed
 * @tc.desc: Test OnRemoteRequest when stub returns ERR_TRANSACTION_FAILED for CallbackExit
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequest_CallbackExitFailed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    stub_->error = ERR_TRANSACTION_FAILED;
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnRemoteRequestInner_DescriptorInvalid
 * @tc.desc: Test OnRemoteRequestInner with invalid interface descriptor
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequestInner_DescriptorInvalid, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(u"com.example.InvalidDescriptor");
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnRemoteRequestInner_DescriptorValid
 * @tc.desc: Test OnRemoteRequestInner with valid interface descriptor
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequestInner_DescriptorValid, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ObjectEditorExtensionStub::GetDescriptor());
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequestInner_DefaultCode
 * @tc.desc: Test OnRemoteRequestInner with default code 0
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, OnRemoteRequestInner_DefaultCode, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    data.WriteInterfaceToken(ObjectEditorExtensionStub::GetDescriptor());
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_NE(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionGetSnapshot_DocumentIdEmpty
 * @tc.desc: Test HandleExtensionGetSnapshot with empty documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetSnapshot_DocumentIdEmpty, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionGetSnapshot(data, reply);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleExtensionGetSnapshot_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionGetSnapshot when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetSnapshot_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetSnapshot(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionGetSnapshot_Success
 * @tc.desc: Test HandleExtensionGetSnapshot with valid documentId successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetSnapshot_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    int32_t ret = stub_->HandleExtensionGetSnapshot(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionDoEdit_DocumentIdEmpty
 * @tc.desc: Test HandleExtensionDoEdit with empty documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionDoEdit_DocumentIdEmpty, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionDoEdit(data, reply);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleExtensionDoEdit_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionDoEdit when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionDoEdit_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionDoEdit(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionDoEdit_Success
 * @tc.desc: Test HandleExtensionDoEdit with valid documentId successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionDoEdit_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    int32_t ret = stub_->HandleExtensionDoEdit(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionGetCapability_DocumentIdEmpty
 * @tc.desc: Test HandleExtensionGetCapability with empty documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetCapability_DocumentIdEmpty, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionGetCapability(data, reply);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleExtensionGetCapability_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionGetCapability when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetCapability_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetCapability(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionGetCapability_WriteBitmaskFailed
 * @tc.desc: Test HandleExtensionGetCapability when WriteUint32 for bitmask fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetCapability_WriteBitmaskFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteUint32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetCapability(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionGetCapability_Success
 * @tc.desc: Test HandleExtensionGetCapability with valid documentId successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetCapability_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    int32_t ret = stub_->HandleExtensionGetCapability(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionGetEditStatus_DocumentIdEmpty
 * @tc.desc: Test HandleExtensionGetEditStatus with empty documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetEditStatus_DocumentIdEmpty, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionGetEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleExtensionGetEditStatus_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionGetEditStatus when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetEditStatus_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionGetEditStatus_WriteIsEditingFailed
 * @tc.desc: Test HandleExtensionGetEditStatus when WriteBool for isEditing fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetEditStatus_WriteIsEditingFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionGetEditStatus_WriteIsModifiedFailed
 * @tc.desc: Test HandleExtensionGetEditStatus when WriteBool for isModified fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetEditStatus_WriteIsModifiedFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteBoolErrorCount(1);
    int32_t ret = stub_->HandleExtensionGetEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionGetEditStatus_Success
 * @tc.desc: Test HandleExtensionGetEditStatus with valid documentId successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetEditStatus_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    int32_t ret = stub_->HandleExtensionGetEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionGetExtensionEditStatus_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionGetExtensionEditStatus when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetExtensionEditStatus_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetExtensionEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionGetExtensionEditStatus_WriteIsEditingFailed
 * @tc.desc: Test HandleExtensionGetExtensionEditStatus when WriteBool for isEditing fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetExtensionEditStatus_WriteIsEditingFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    int32_t ret = stub_->HandleExtensionGetExtensionEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionGetExtensionEditStatus_Success
 * @tc.desc: Test HandleExtensionGetExtensionEditStatus successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionGetExtensionEditStatus_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionGetExtensionEditStatus(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleExtensionInitial_DocumentNull
 * @tc.desc: Test HandleExtensionInitial with null document
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionInitial_DocumentNull, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionInitial(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionInitial_ClientCbNull
 * @tc.desc: Test HandleExtensionInitial with null client callback
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionInitial_ClientCbNull, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    int32_t ret = stub_->HandleExtensionInitial(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionInitial_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionInitial when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionInitial_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    sptr<IObjectEditorClientCallback> clientCb = sptr<MockObjectEditorClientCallback>::MakeSptr();
    data.WriteRemoteObject(clientCb->AsObject());
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionInitial(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionInitial_Success
 * @tc.desc: Test HandleExtensionInitial with valid document and client callback successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionInitial_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    auto document = std::make_unique<ObjectEditorDocument>();
    data.WriteParcelable(document.get());
    sptr<IObjectEditorClientCallback> clientCb = sptr<MockObjectEditorClientCallback>::MakeSptr();
    data.WriteRemoteObject(clientCb->AsObject());
    int32_t ret = stub_->HandleExtensionInitial(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
}

/**
 * @tc.name: HandleExtensionClose_DocumentIdEmpty
 * @tc.desc: Test HandleExtensionClose with empty documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionClose_DocumentIdEmpty, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    int32_t ret = stub_->HandleExtensionClose(data, reply);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleExtensionClose_WriteErrCodeFailed
 * @tc.desc: Test HandleExtensionClose when WriteInt32 for errCode fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionClose_WriteErrCodeFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->HandleExtensionClose(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionClose_WriteIsAllObjectsRemovedFailed
 * @tc.desc: Test HandleExtensionClose when WriteBool for isAllObjectsRemoved fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionClose_WriteIsAllObjectsRemovedFailed, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    int32_t ret = stub_->HandleExtensionClose(data, reply);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: HandleExtensionClose_Success
 * @tc.desc: Test HandleExtensionClose with valid documentId successfully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorExtensionStubTest, HandleExtensionClose_Success, TestSize.Level1)
{
    MessageParcel data;
    MessageParcel reply;
    data.WriteString("test_document_id");
    int32_t ret = stub_->HandleExtensionClose(data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

}
} // namespace ObjectEditor
} // namespace OHOS