/*
 * Copyright (c) Huawei Device Co., Ltd. 2026-2026. All rights reserved.
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
#include "object_editor_client_callback.h"
#include "object_editor_document.h"
#include "object_editor_manager_stub.h"
#include "object_editor_manager_system_ability.h"
#undef protected

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorManagerStub : public ObjectEditorManagerStub {
public:
    MockObjectEditorManagerStub() = default;
    ~MockObjectEditorManagerStub() = default;
    ErrCode StartObjectEditorExtension(
        std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject,
        bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(
        const std::string &documentId,
        const sptr<IRemoteObject> &oeExtensionRemoteObject,
        const bool &isPackageExtension) override;
    ErrCode GetOEidByFileExtension(
        const std::string &oeid,
        std::string &fileExtension) override;
    ErrCode GetIconByOEid(
        const std::string &oeid,
        std::string &resourceId) override;
    ErrCode GetFormatName(
        const std::string &oeid,
        const std::string &locale,
        std::string &formatName) override;
    ErrCode GetObjectEditorFormatByOEidAndLocale(
        const std::string &oeid,
        const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format) override;
    ErrCode GetObjectEditorFormatsByLocale(
        const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) override;
    ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want) override;
    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;

    ErrCode callbackEnterError = ERR_OK;
    ErrCode error = ERR_OK;
    std::string testcaseName;
    bool formatIsNull = false;
    uint32_t hookFormatsSize = 0;
};

class ObjectEditorManagerStubTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<MockObjectEditorManagerStub> stub_;
};

void ObjectEditorManagerStubTest::SetUpTestCase() {}

void ObjectEditorManagerStubTest::TearDownTestCase() {}

void ObjectEditorManagerStubTest::SetUp()
{
    stub_ = sptr<MockObjectEditorManagerStub>::MakeSptr();
    ASSERT_NE(stub_, nullptr);
    stub_->error = ERR_OK;
    stub_->callbackEnterError = ERR_OK;
    stub_->formatIsNull = false;
    stub_->hookFormatsSize = 0;
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    MockMessageParcel::ClearAllErrorFlag();
}

void ObjectEditorManagerStubTest::TearDown() {}

ErrCode MockObjectEditorManagerStub::StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "isPackageExtension: %{public}d", isPackageExtension);
    return error;
}

ErrCode MockObjectEditorManagerStub::StopObjectEditorExtension(
    const std::string &documentId,
    const sptr<IRemoteObject> &oeExtensionRemoteObject,
    const bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "isPackageExtension: %{public}d", isPackageExtension);
    return error;
}

ErrCode MockObjectEditorManagerStub::GetOEidByFileExtension(
    const std::string &oeid,
    std::string &fileExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "oeid: %{public}s", oeid.c_str());
    return error;
}

ErrCode MockObjectEditorManagerStub::GetIconByOEid(
    const std::string &oeid,
    std::string &resourceId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "oeid: %{public}s", oeid.c_str());
    return error;
}

ErrCode MockObjectEditorManagerStub::GetFormatName(
    const std::string &oeid,
    const std::string &locale,
    std::string &formatName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "oeid: %{public}s, locale: %{public}s", oeid.c_str(), locale.c_str());
    return error;
}

ErrCode MockObjectEditorManagerStub::GetObjectEditorFormatByOEidAndLocale(
    const std::string &oeid,
    const std::string &locale,
    std::unique_ptr<ObjectEditorFormat> &format)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "oeid: %{public}s, locale: %{public}s", oeid.c_str(), locale.c_str());
    return error;
}

ErrCode MockObjectEditorManagerStub::GetObjectEditorFormatsByLocale(
    const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "locale: %{public}s", locale.c_str());
    return error;
}

ErrCode MockObjectEditorManagerStub::StartUIAbility(const std::unique_ptr<AAFwk::Want> &want)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "want: %{public}s", want->ToString().c_str());
    return error;
}

int32_t MockObjectEditorManagerStub::CallbackEnter([[maybe_unused]] uint32_t code)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "CallbackEnter code: %{public}u", code);
    return callbackEnterError;
}

int32_t MockObjectEditorManagerStub::CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "CallbackExit code: %{public}u, result: %{public}d", code, result);
    return error;
}

/**
 * @tc.name: OnRemoteRequest_For_CallbackEnter
 * @tc.desc: Test OnRemoteRequest for CallbackEnter return error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequest_For_CallbackEnter, TestSize.Level1)
{
    uint32_t code = MIN_TRANSACTION_ID;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    stub_->callbackEnterError = ERR_TRANSACTION_FAILED;
    stub_->testcaseName = "OnRemoteRequest_For_CallbackEnter";
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
    EXPECT_TRUE(logMsg.find("CallbackEnter code") != std::string::npos);
}

/**
 * @tc.name: OnRemoteRequest_For_CallbackExit
 * @tc.desc: Test OnRemoteRequest for CallbackExit return error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequest_For_CallbackExit, TestSize.Level1)
{
    uint32_t code = MIN_TRANSACTION_ID;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    stub_->callbackEnterError = ERR_TRANSACTION_FAILED;
    stub_->testcaseName = "OnRemoteRequest_For_CallbackExit";
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
    EXPECT_TRUE(logMsg.find("CallbackExit code") != std::string::npos);
}

/**
 * @tc.name: OnRemoteRequest_For_Normal
 * @tc.desc: Test OnRemoteRequest for normal case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequest_For_Normal, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "OnRemoteRequest_For_Normal";
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: OnRemoteRequestInner_For_Descriptor_Not_Same
 * @tc.desc: Test OnRemoteRequestInner for descriptor not same case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequestInner_For_Descriptor_Not_Same, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "OnRemoteRequestInner_For_Descriptor_Not_Same";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor + u"_not_same");
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_TRANSACTION_FAILED);
}

/**
 * @tc.name: OnRemoteRequestInner_Success
 * @tc.desc: Test OnRemoteRequestInner for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequestInner_Success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "OnRemoteRequestInner_Success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: OnRemoteRequestInner_Code_Not_Exist
 * @tc.desc: Test OnRemoteRequestInner for code not exist case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, OnRemoteRequestInner_Code_Not_Exist, TestSize.Level1)
{
    uint32_t code = 0;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "OnRemoteRequestInner_Success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    int32_t ret = stub_->OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_document_is_null
 * @tc.desc: Test HandleStartObjectEditorExtension for document is null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_document_is_null, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_document_is_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("document is null") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_clientCallbackRemoteObject_is_null
 * @tc.desc: Test HandleStartObjectEditorExtension for clientCallbackRemoteObject is null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_clientCallbackRemoteObject_is_null,
    TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_clientCallbackRemoteObject_is_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(false);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("clientCallbackRemoteObject is null") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_clientCallback_is_null
 * @tc.desc: Test HandleStartObjectEditorExtension for clientCallback is null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_clientCallback_is_null, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_clientCallback_is_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("clientCallback is null") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_write_errCode_failed
 * @tc.desc: Test HandleStartObjectEditorExtension for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_errCode_is_failed
 * @tc.desc: Test HandleStartObjectEditorExtension for errCode is failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_errCode_is_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_errCode_is_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_isPackageExtension_failed
 * @tc.desc: Test HandleStartObjectEditorExtension for isPackageExtension failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_isPackageExtension_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_isPackageExtension_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    MockMessageParcel::SetWriteBoolErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write isPackageExtension failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_write_oeExtensionRemoteObject_failed
 * @tc.desc: Test HandleStartObjectEditorExtension for write oeExtensionRemoteObject failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_write_oeExtensionRemoteObject_failed,
    TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_write_oeExtensionRemoteObject_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write oeExtensionRemoteObject failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartObjectEditorExtension_success
 * @tc.desc: Test HandleStartObjectEditorExtension for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartObjectEditorExtension_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartObjectEditorExtension_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::AddInt32Cache(1);
    sptr<IObjectEditorManager> clientCb = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(clientCb->AsObject(), nullptr);
    data.WriteRemoteObject(clientCb->AsObject());
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleStopObjectEditorExtension_documentId_empty
 * @tc.desc: Test HandleStopObjectEditorExtension for documentId empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStopObjectEditorExtension_documentId_empty, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStopObjectEditorExtension_documentId_empty";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("documentId is empty") != std::string::npos);
}

/**
 * @tc.name: HandleStopObjectEditorExtension_extensionRemoteObject_null
 * @tc.desc: Test HandleStopObjectEditorExtension for extensionRemoteObject null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStopObjectEditorExtension_extensionRemoteObject_null, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStopObjectEditorExtension_extensionRemoteObject_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString("test_document_id");
    MockMessageParcel::SetWriteRemoteObjectErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("extensionRemoteObject is nullptr") != std::string::npos);
}

/**
 * @tc.name: HandleStopObjectEditorExtension_write_errCode_failed
 * @tc.desc: Test HandleStopObjectEditorExtension for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStopObjectEditorExtension_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStopObjectEditorExtension_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString("test_document_id");
    sptr<IObjectEditorManager> extension = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(extension->AsObject(), nullptr);
    data.WriteRemoteObject(extension->AsObject());
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleStopObjectEditorExtension_errCode_failed
 * @tc.desc: Test HandleStopObjectEditorExtension for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStopObjectEditorExtension_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStopObjectEditorExtension_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString("test_document_id");
    sptr<IObjectEditorManager> extension = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(extension->AsObject(), nullptr);
    data.WriteRemoteObject(extension->AsObject());
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleStopObjectEditorExtension_success
 * @tc.desc: Test HandleStopObjectEditorExtension for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStopObjectEditorExtension_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStopObjectEditorExtension_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString("test_document_id");
    sptr<IObjectEditorManager> extension = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(extension->AsObject(), nullptr);
    data.WriteRemoteObject(extension->AsObject());
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleGetOEidByFileExtension_write_errCode_failed
 * @tc.desc: Test HandleGetOEidByFileExtension for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetOEidByFileExtension_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetOEidByFileExtension_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetOEidByFileExtension_errCode_failed
 * @tc.desc: Test HandleGetOEidByFileExtension for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetOEidByFileExtension_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetOEidByFileExtension_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetOEidByFileExtension_write_fileExtension_failed
 * @tc.desc: Test HandleGetOEidByFileExtension for write fileExtension failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetOEidByFileExtension_write_fileExtension_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetOEidByFileExtension_write_fileExtension_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    MockMessageParcel::SetWriteString16ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write fileExtension failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetOEidByFileExtension_success
 * @tc.desc: Test HandleGetOEidByFileExtension for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetOEidByFileExtension_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetOEidByFileExtension_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleGetIconByOEid_write_errCode_failed
 * @tc.desc: Test HandleGetIconByOEid for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetIconByOEid_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetIconByOEid_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetIconByOEid_errCode_failed
 * @tc.desc: Test HandleGetIconByOEid for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetIconByOEid_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetIconByOEid_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetIconByOEid_write_resourceId_failed
 * @tc.desc: Test HandleGetIconByOEid for write resourceId failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetIconByOEid_write_resourceId_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetIconByOEid_write_resourceId_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    MockMessageParcel::SetWriteString16ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write resourceId failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetIconByOEid_success
 * @tc.desc: Test HandleGetIconByOEid for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetIconByOEid_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetIconByOEid_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleGetObjectEditorFormatByOEidAndLocale_write_errCode_failed
 * @tc.desc: Test HandleGetObjectEditorFormatByOEidAndLocale for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatByOEidAndLocale_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatByOEidAndLocale_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    data.WriteString16(Str8ToStr16("test_locale"));
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatByOEidAndLocale_errCode_failed
 * @tc.desc: Test HandleGetObjectEditorFormatByOEidAndLocale for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatByOEidAndLocale_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatByOEidAndLocale_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    data.WriteString16(Str8ToStr16("test_locale"));
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatByOEidAndLocale_format_null
 * @tc.desc: Test HandleGetObjectEditorFormatByOEidAndLocale for format null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatByOEidAndLocale_format_null, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatByOEidAndLocale_format_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    data.WriteString16(Str8ToStr16("test_locale"));
    stub_->formatIsNull = true;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write format failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatByOEidAndLocale_write_format_failed
 * @tc.desc: Test HandleGetObjectEditorFormatByOEidAndLocale for write format failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatByOEidAndLocale_write_format_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatByOEidAndLocale_write_format_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    data.WriteString16(Str8ToStr16("test_locale"));
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write format failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatByOEidAndLocale_success
 * @tc.desc: Test HandleGetObjectEditorFormatByOEidAndLocale for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatByOEidAndLocale_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatByOEidAndLocale_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_oeid"));
    data.WriteString16(Str8ToStr16("test_locale"));
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleGetObjectEditorFormatsByLocale_write_errCode_failed
 * @tc.desc: Test HandleGetObjectEditorFormatsByLocale for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatsByLocale_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatsByLocale_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_locale"));
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatsByLocale_errCode_failed
 * @tc.desc: Test HandleGetObjectEditorFormatsByLocale for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatsByLocale_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatsByLocale_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_locale"));
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatsByLocale_size_exceed_limit
 * @tc.desc: Test HandleGetObjectEditorFormatsByLocale for size exceed limit case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatsByLocale_size_exceed_limit, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatsByLocale_size_exceed_limit";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_locale"));
    stub_->hookFormatsSize = static_cast<uint32_t>(VECTOR_MAX_SIZE) + 1;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("formats size exceed limit") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatsByLocale_write_format_failed
 * @tc.desc: Test HandleGetObjectEditorFormatsByLocale for write format failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatsByLocale_write_format_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatsByLocale_write_format_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_locale"));
    MockMessageParcel::SetWriteParcelableErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("write format failed") != std::string::npos);
}

/**
 * @tc.name: HandleGetObjectEditorFormatsByLocale_success
 * @tc.desc: Test HandleGetObjectEditorFormatsByLocale for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleGetObjectEditorFormatsByLocale_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleGetObjectEditorFormatsByLocale_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    data.WriteString16(Str8ToStr16("test_locale"));
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: HandleStartUIAbility_want_null
 * @tc.desc: Test HandleStartUIAbility for want null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartUIAbility_want_null, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartUIAbility_want_null";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    MockMessageParcel::SetReadParcelableErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_DATA);
    EXPECT_TRUE(logMsg.find("want is nullptr") != std::string::npos);
}

/**
 * @tc.name: HandleStartUIAbility_write_errCode_failed
 * @tc.desc: Test HandleStartUIAbility for write errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartUIAbility_write_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartUIAbility_write_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    AAFwk::Want want;
    data.WriteParcelable(&want);
    MockMessageParcel::SetWriteInt32ErrorFlag(true);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("write errCode failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartUIAbility_errCode_failed
 * @tc.desc: Test HandleStartUIAbility for errCode failed case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartUIAbility_errCode_failed, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartUIAbility_errCode_failed";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    AAFwk::Want want;
    data.WriteParcelable(&want);
    stub_->error = ERR_INVALID_VALUE;
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
    EXPECT_TRUE(logMsg.find("errCode is failed") != std::string::npos);
}

/**
 * @tc.name: HandleStartUIAbility_success
 * @tc.desc: Test HandleStartUIAbility for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerStubTest, HandleStartUIAbility_success, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;;
    stub_->testcaseName = "HandleStartUIAbility_success";
    std::u16string descriptor = stub_->GetDescriptor();
    data.WriteInterfaceToken(descriptor);
    AAFwk::Want want;
    data.WriteParcelable(&want);
    int32_t ret = stub_->OnRemoteRequestInner(code, data, reply, option);
    EXPECT_EQ(ret, ERR_OK);
}
} // namespace ObjectEditor
} // namespace OHOS
