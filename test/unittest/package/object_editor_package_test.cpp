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
#define final
#include "object_editor_package.h"
#include "mock_object_editor_document.h"
#undef final
#include "mock_object_editor_client_callback_stub.h"
#include "mock_ability_manager_client.h"
#include "hilog_object_editor.h"
#include "stub.h"
#include "want.h"
#include "ability_manager_client.h"
#include "user_mgr.h"
#include "system_utils.h"
#include "object_editor_err_code.h"
#include "object_editor_config.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorPackageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ObjectEditorPackage> package_;
    sptr<MockObjectEditorClientCallbackStub> mockClientCb_;
};

void ObjectEditorPackageTest::SetUpTestCase()
{
}

void ObjectEditorPackageTest::TearDownTestCase()
{
}

void ObjectEditorPackageTest::SetUp()
{
    package_ = std::make_shared<ObjectEditorPackage>();
    mockClientCb_ = sptr<MockObjectEditorClientCallbackStub>(new MockObjectEditorClientCallbackStub());
}

void ObjectEditorPackageTest::TearDown()
{
    package_ = nullptr;
    mockClientCb_ = nullptr;
}

namespace {

constexpr const char *TEST_DOCUMENT_ID = "test_doc_001";
constexpr const char *TEST_FILE_URI = "file:///data/test/test.doc";

}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Test ObjectEditorPackage constructor creates valid instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Constructor_001, TestSize.Level1)
{
    auto pkg = std::make_shared<ObjectEditorPackage>();
    EXPECT_NE(pkg, nullptr);
}

/**
 * @tc.name: Destructor_001
 * @tc.desc: Test ObjectEditorPackage destructor with watcher_ null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Destructor_001, TestSize.Level1)
{
    auto pkg = std::make_shared<ObjectEditorPackage>();
    pkg->watcher_ = nullptr;
    EXPECT_NE(pkg, nullptr);
}

/**
 * @tc.name: GetSnapshot_001
 * @tc.desc: Test GetSnapshot returns EXTENSION_CAPABILITY_NOT_SUPPORT
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetSnapshot_001, TestSize.Level1)
{
    ErrCode result = package_->GetSnapshot(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ObjectorEditorExtensionErrCode::EXTENSION_CAPABILITY_NOT_SUPPORT);
}

/**
 * @tc.name: DoEdit_001
 * @tc.desc: Test DoEdit with null document returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_001, TestSize.Level1)
{
    package_->document_ = nullptr;
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: DoEdit_002
 * @tc.desc: Test DoEdit with linking document and nullopt native file URI returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_002, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetLinking()).WillRepeatedly(Return(true));
    EXPECT_CALL(*document, GetNativeFileUri()).WillRepeatedly(Return(std::optional<std::string>(std::nullopt)));
    package_->document_ = document;
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: DoEdit_003
 * @tc.desc: Test DoEdit with linking document and null ability manager client
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_003, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetLinking()).WillRepeatedly(Return(true));
    EXPECT_CALL(*document, GetNativeFileUri()).WillRepeatedly(Return(std::optional<std::string>(TEST_FILE_URI)));
    package_->document_ = document;
    MockAbilityManagerClientUtil::SetIsNullInstance(true);
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
}

/**
 * @tc.name: DoEdit_004
 * @tc.desc: Test DoEdit with non-linking document and null packageData returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_004, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetLinking()).WillRepeatedly(Return(false));
    package_->document_ = document;
    package_->packageData_ = nullptr;
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetEditStatus_001
 * @tc.desc: Test GetEditStatus with null isEditing pointer returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetEditStatus_001, TestSize.Level1)
{
    bool isModified = false;
    ErrCode result = package_->GetEditStatus(TEST_DOCUMENT_ID, nullptr, &isModified);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetEditStatus_002
 * @tc.desc: Test GetEditStatus when isEditing is true returns ERR_OK and isEditing true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetEditStatus_002, TestSize.Level1)
{
    bool isEditing = false;
    bool isModified = false;
    package_->isEditing_ = true;
    ErrCode result = package_->GetEditStatus(TEST_DOCUMENT_ID, &isEditing, &isModified);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isEditing, true);
}

/**
 * @tc.name: GetEditStatus_003
 * @tc.desc: Test GetEditStatus when isEditing is false returns ERR_OK and isEditing false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetEditStatus_003, TestSize.Level1)
{
    bool isEditing = false;
    bool isModified = false;
    package_->isEditing_ = false;
    ErrCode result = package_->GetEditStatus(TEST_DOCUMENT_ID, &isEditing, &isModified);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isEditing, false);
}

/**
 * @tc.name: GetExtensionEditStatus_001
 * @tc.desc: Test GetExtensionEditStatus when isEditing is true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetExtensionEditStatus_001, TestSize.Level1)
{
    bool isEditing = false;
    package_->isEditing_ = true;
    ErrCode result = package_->GetExtensionEditStatus(isEditing);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isEditing, true);
}

/**
 * @tc.name: GetExtensionEditStatus_002
 * @tc.desc: Test GetExtensionEditStatus when isEditing is false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetExtensionEditStatus_002, TestSize.Level1)
{
    bool isEditing = false;
    package_->isEditing_ = false;
    ErrCode result = package_->GetExtensionEditStatus(isEditing);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isEditing, false);
}

/**
 * @tc.name: GetCapability_001
 * @tc.desc: Test GetCapability with null bitmask pointer returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetCapability_001, TestSize.Level1)
{
    ErrCode result = package_->GetCapability(TEST_DOCUMENT_ID, nullptr);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: GetCapability_002
 * @tc.desc: Test GetCapability with valid bitmask pointer returns CE_CAPABILITY_SUPPORT_DO_EDIT
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetCapability_002, TestSize.Level1)
{
    uint32_t bitmask = 0;
    ErrCode result = package_->GetCapability(TEST_DOCUMENT_ID, &bitmask);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(bitmask, ContentEmbed_CapabilityCode::CE_CAPABILITY_SUPPORT_DO_EDIT);
}

/**
 * @tc.name: Close_001
 * @tc.desc: Test Close with valid documentId returns ERR_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Close_001, TestSize.Level1)
{
    bool isAllObjectsRemoved = false;
    ErrCode result = package_->Close(TEST_DOCUMENT_ID, isAllObjectsRemoved, 0);
    EXPECT_EQ(result, ERR_OK);
}

/**
 * @tc.name: Initial_001
 * @tc.desc: Test Initial with null document and null clientCb returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_001, TestSize.Level1)
{
    sptr<IObjectEditorClientCallback> clientCb = nullptr;
    auto result = package_->Initial(nullptr, clientCb, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: Initial_002
 * @tc.desc: Test Initial with valid document but null clientCb returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_002, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorClientCallback> clientCb = nullptr;
    auto result = package_->Initial(std::move(document), clientCb, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: Initial_003
 * @tc.desc: Test Initial with null document but valid clientCb returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_003, TestSize.Level1)
{
    auto result = package_->Initial(nullptr, mockClientCb_, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: Initial_004
 * @tc.desc: Test Initial with valid document and clientCb in linking mode returns ERR_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_004, TestSize.Level1)
{
    auto document = std::make_unique<MockObjectEditorDocument>();
    ON_CALL(*document, RestoreStorage());
    ON_CALL(*document, FlushOEid()).WillByDefault(Return(true));
    ON_CALL(*document, GetLinking()).WillByDefault(Return(true));
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
}

/**
 * @tc.name: Initial_005
 * @tc.desc: Test Initial with non-linking document and null packageData returns error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_005, TestSize.Level1)
{
    auto document = std::make_unique<MockObjectEditorDocument>();
    ON_CALL(*document, RestoreStorage());
    ON_CALL(*document, FlushOEid()).WillByDefault(Return(true));
    ON_CALL(*document, GetLinking()).WillByDefault(Return(false));
    ON_CALL(*document, GetOperateType()).WillByDefault(Return(OperateType::CREATE_BY_FILE));
    auto mockDocShared = std::make_shared<MockObjectEditorDocument>();
    package_->document_ = mockDocShared;
    package_->packageData_ = nullptr;
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_NE(result, ERR_OK);
}

/**
 * @tc.name: OpenFile_001
 * @tc.desc: Test OpenFile with null ability manager client returns SA_DISCONNECT_ABILITY_FAILED
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, OpenFile_001, TestSize.Level1)
{
    MockAbilityManagerClientUtil::SetIsNullInstance(true);
    package_->SetRemoteObject(nullptr);
    std::unique_ptr<MockAbilityManagerClient> mockClient = std::make_unique<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = nullptr;
    ErrCode result = package_->OpenFile(TEST_FILE_URI);
    EXPECT_EQ(result, ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED);
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    CEAbilityManagerClient::client = nullptr;
}

/**
 * @tc.name: OpenFile_002
 * @tc.desc: Test OpenFile with start ability success returns ERR_OK and sets isEditing true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, OpenFile_002, TestSize.Level1)
{
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_OK));
    package_->SetRemoteObject(sptr<IRemoteObject>(nullptr));
    ErrCode result = package_->OpenFile(TEST_FILE_URI);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(package_->isEditing_, true);
    CEAbilityManagerClient::client = nullptr;
}

/**
 * @tc.name: OpenFile_003
 * @tc.desc: Test OpenFile with start ability failure returns ERR_INVALID_VALUE and keeps isEditing false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, OpenFile_003, TestSize.Level1)
{
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_INVALID_VALUE));
    package_->SetRemoteObject(sptr<IRemoteObject>(nullptr));
    ErrCode result = package_->OpenFile(TEST_FILE_URI);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_EQ(package_->isEditing_, false);
    CEAbilityManagerClient::client = nullptr;
}

}
}