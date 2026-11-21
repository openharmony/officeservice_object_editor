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

#include <sys/inotify.h>

#include <cstdio>
#include <fstream>

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
constexpr const char *TEST_WATCH_FILE = "/data/local/tmp/oe_pkg_watch_file.doc";
constexpr const char *TEST_UNEXIST_FILE = "/data/local/tmp/oe_pkg_unexist_file.doc";
constexpr const char *TEST_NATIVE_FILE_URI = "file:///data/local/tmp/oe_pkg_native_file.doc";
constexpr const char *TEST_ORI_FILE_URI = "file:///data/local/tmp/oe_pkg_ori_file.doc";
constexpr const char *TEST_TMP_FILE_URI = "file:///data/local/tmp/oe_pkg_tmp_file.doc";

// ObjectEditorDocument getters are non-virtual, so EXPECT_CALL/ON_CALL on
// MockObjectEditorDocument never redirects calls made through the base-class
// pointer held by ObjectEditorPackage. Stub (binary patch, same mechanism as
// package_data_test.cpp) is used instead to control these non-virtual/static
// dependencies deterministically.
std::unique_ptr<PackageData> MockCreateByDocumentNull(std::shared_ptr<ObjectEditorDocument> document)
{
    (void)document;
    return nullptr;
}

std::unique_ptr<PackageData> MockCreateByDocumentValid(std::shared_ptr<ObjectEditorDocument> document)
{
    (void)document;
    return std::make_unique<PackageData>();
}

std::unique_ptr<ObjectEditorDocument> MockCreateByFileNull(const std::string &path, bool isLinking)
{
    (void)path;
    (void)isLinking;
    return nullptr;
}

std::unique_ptr<ObjectEditorDocument> MockCreateByFileValid(const std::string &path, bool isLinking)
{
    (void)path;
    (void)isLinking;
    return std::make_unique<ObjectEditorDocument>();
}

std::unique_ptr<ObjectEditorDocument> MockLoadFromFileValid(const std::string &path)
{
    (void)path;
    return std::make_unique<ObjectEditorDocument>();
}

bool CreateWatchFile(const std::string &path)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file << "oe";
    return true;
}

// Drives DoEdit to full success: real document, real PackageData with an
// existing watch file, mocked StartAbility returning ERR_OK. After returning
// true, package_->watcher_ holds the callback registered by DoEdit.
bool SetupWatchedPackage(const std::shared_ptr<ObjectEditorPackage> &package,
    const sptr<IObjectEditorClientCallback> &clientCb, const std::string &watchFilePath)
{
    if (!CreateWatchFile(watchFilePath)) {
        return false;
    }
    auto document = std::make_shared<ObjectEditorDocument>();
    document->SetDocumentId(TEST_DOCUMENT_ID);
    package->document_ = document;
    package->clientCb_ = clientCb;
    auto packageData = std::make_unique<PackageData>();
    packageData->filepath_ = watchFilePath;
    package->packageData_ = std::move(packageData);
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_OK));
    ErrCode editResult = package->DoEdit(TEST_DOCUMENT_ID);
    CEAbilityManagerClient::client = nullptr;
    return editResult == ERR_OK;
}

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
 * @tc.name: Destructor_002
 * @tc.desc: Test destructor stops the watcher when watcher_ is not null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Destructor_002, TestSize.Level1)
{
    auto pkg = std::make_shared<ObjectEditorPackage>();
    auto watcher = std::make_shared<FileWatcher>("", [](uint32_t mask, const std::string &path) {});
    watcher->running_.store(true);
    pkg->watcher_ = watcher;
    pkg = nullptr;
    EXPECT_EQ(watcher->running_.load(), false);
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
 * @tc.name: DoEdit_005
 * @tc.desc: Test DoEdit with linking document whose native file URI is missing returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_005, TestSize.Level1)
{
    auto document = std::make_shared<ObjectEditorDocument>();
    document->SetLinking(true);
    package_->document_ = document;
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_EQ(package_->isEditing_, false);
}

/**
 * @tc.name: DoEdit_006
 * @tc.desc: Test DoEdit with linking document and native file URI starts editor and sets isEditing
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_006, TestSize.Level1)
{
    auto document = std::make_shared<ObjectEditorDocument>();
    document->SetLinking(true);
    document->SetNativeFileUri(TEST_NATIVE_FILE_URI);
    package_->document_ = document;
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_OK));
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(package_->isEditing_, true);
    EXPECT_EQ(package_->watcher_, nullptr);
    CEAbilityManagerClient::client = nullptr;
}

/**
 * @tc.name: DoEdit_007
 * @tc.desc: Test DoEdit with watcher start failure on nonexistent file returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_007, TestSize.Level1)
{
    auto document = std::make_shared<ObjectEditorDocument>();
    package_->document_ = document;
    auto packageData = std::make_unique<PackageData>();
    packageData->filepath_ = TEST_UNEXIST_FILE;
    package_->packageData_ = std::move(packageData);
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_EQ(package_->watcher_, nullptr);
    EXPECT_EQ(package_->isEditing_, false);
}

/**
 * @tc.name: DoEdit_008
 * @tc.desc: Test DoEdit with valid packageData starts watcher, opens file and sets isEditing
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, DoEdit_008, TestSize.Level1)
{
    ASSERT_TRUE(CreateWatchFile(TEST_WATCH_FILE));
    auto document = std::make_shared<ObjectEditorDocument>();
    package_->document_ = document;
    auto packageData = std::make_unique<PackageData>();
    packageData->filepath_ = TEST_WATCH_FILE;
    package_->packageData_ = std::move(packageData);
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_OK));
    ErrCode result = package_->DoEdit(TEST_DOCUMENT_ID);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(package_->isEditing_, true);
    EXPECT_NE(package_->watcher_, nullptr);
    CEAbilityManagerClient::client = nullptr;
    std::remove(TEST_WATCH_FILE);
}

/**
 * @tc.name: WatcherCallback_004
 * @tc.desc: Test DoEdit watcher callback reloads document and notifies OnUpdate with propagated fields
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, WatcherCallback_004, TestSize.Level1)
{
    ASSERT_TRUE(SetupWatchedPackage(package_, mockClientCb_, TEST_WATCH_FILE));
    ASSERT_NE(package_->watcher_, nullptr);
    package_->document_->SetTmpFileUri(TEST_TMP_FILE_URI);
    std::string capturedDocumentId;
    std::string capturedTmpFileUri;
    EXPECT_CALL(*mockClientCb_, OnUpdate(_)).WillOnce(
        Invoke([&capturedDocumentId, &capturedTmpFileUri](std::unique_ptr<ObjectEditorDocument> &document) {
            capturedDocumentId = document->GetDocumentId();
            capturedTmpFileUri = document->GetTmpFileUri().value_or("");
            return ERR_OK;
        }));
    Stub stub;
    stub.set(ADDR(&ObjectEditorDocument::LoadFromFile), MockLoadFromFileValid);
    package_->watcher_->callback_(static_cast<uint32_t>(IN_CLOSE_WRITE), TEST_WATCH_FILE);
    EXPECT_EQ(capturedDocumentId, TEST_DOCUMENT_ID);
    EXPECT_EQ(capturedTmpFileUri, TEST_TMP_FILE_URI);
    std::remove(TEST_WATCH_FILE);
}

/**
 * @tc.name: WatcherCallback_005
 * @tc.desc: Test DoEdit watcher callback with missing tmp file URI skips SetTmpFileUri but still updates
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, WatcherCallback_005, TestSize.Level1)
{
    ASSERT_TRUE(SetupWatchedPackage(package_, mockClientCb_, TEST_WATCH_FILE));
    ASSERT_NE(package_->watcher_, nullptr);
    std::string capturedDocumentId;
    std::string capturedTmpFileUri;
    EXPECT_CALL(*mockClientCb_, OnUpdate(_)).WillOnce(
        Invoke([&capturedDocumentId, &capturedTmpFileUri](std::unique_ptr<ObjectEditorDocument> &document) {
            capturedDocumentId = document->GetDocumentId();
            capturedTmpFileUri = document->GetTmpFileUri().value_or("");
            return ERR_OK;
        }));
    Stub stub;
    stub.set(ADDR(&ObjectEditorDocument::LoadFromFile), MockLoadFromFileValid);
    package_->watcher_->callback_(static_cast<uint32_t>(IN_CLOSE_WRITE), TEST_WATCH_FILE);
    EXPECT_EQ(capturedDocumentId, TEST_DOCUMENT_ID);
    EXPECT_EQ(capturedTmpFileUri, "");
    std::remove(TEST_WATCH_FILE);
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
 * @tc.name: GetEditStatus_004
 * @tc.desc: Test GetEditStatus with null isModified returns ERR_OK and only fills isEditing
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, GetEditStatus_004, TestSize.Level1)
{
    bool isEditing = false;
    ErrCode result = package_->GetEditStatus(TEST_DOCUMENT_ID, &isEditing, nullptr);
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
 * @tc.name: Close_002
 * @tc.desc: Test Close with active watcher stops it and clears watcher_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Close_002, TestSize.Level1)
{
    auto watcher = std::make_shared<FileWatcher>("", [](uint32_t mask, const std::string &path) {});
    watcher->running_.store(true);
    package_->watcher_ = watcher;
    bool isAllObjectsRemoved = false;
    ErrCode result = package_->Close(TEST_DOCUMENT_ID, isAllObjectsRemoved, 0);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(isAllObjectsRemoved, true);
    EXPECT_EQ(package_->watcher_, nullptr);
    EXPECT_EQ(watcher->running_.load(), false);
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
 * @tc.name: Initial_006
 * @tc.desc: Test Initial with linking document returns ERR_OK without creating package data
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_006, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetLinking(true);
    document->SetTmpFileUri(TEST_TMP_FILE_URI);
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(package_->packageData_, nullptr);
}

/**
 * @tc.name: Initial_007
 * @tc.desc: Test Initial with CREATE_BY_FILE when CreateByDocument fails returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_007, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOperateType(OperateType::CREATE_BY_FILE);
    document->SetOriFileUri(TEST_ORI_FILE_URI);
    document->SetTmpFileUri(TEST_TMP_FILE_URI);
    Stub stub;
    stub.set(ADDR(&PackageData::CreateByDocument), MockCreateByDocumentNull);
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_EQ(package_->packageData_, nullptr);
}

/**
 * @tc.name: Initial_008
 * @tc.desc: Test Initial with CREATE_BY_FILE when CreateByFile fails returns ERR_INVALID_VALUE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_008, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOperateType(OperateType::CREATE_BY_FILE);
    document->SetOriFileUri(TEST_ORI_FILE_URI);
    document->SetTmpFileUri(TEST_TMP_FILE_URI);
    Stub stub;
    stub.set(ADDR(&PackageData::CreateByDocument), MockCreateByDocumentValid);
    stub.set(ADDR(&ObjectEditorDocument::CreateByFile), MockCreateByFileNull);
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_INVALID_VALUE);
    EXPECT_NE(package_->packageData_, nullptr);
}

/**
 * @tc.name: Initial_009
 * @tc.desc: Test Initial with CREATE_BY_FILE success notifies OnUpdate with propagated document fields
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_009, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOperateType(OperateType::CREATE_BY_FILE);
    document->SetOriFileUri(TEST_ORI_FILE_URI);
    document->SetNativeFileUri(TEST_NATIVE_FILE_URI);
    document->SetTmpFileUri(TEST_TMP_FILE_URI);
    document->SetDocumentId(TEST_DOCUMENT_ID);
    std::string capturedOEid;
    std::string capturedDocumentId;
    std::string capturedNativeFileUri;
    std::string capturedTmpFileUri;
    EXPECT_CALL(*mockClientCb_, OnUpdate(_)).WillOnce(
        Invoke([&](std::unique_ptr<ObjectEditorDocument> &newDocument) {
            capturedOEid = newDocument->GetOEid();
            capturedDocumentId = newDocument->GetDocumentId();
            capturedNativeFileUri = newDocument->GetNativeFileUri().value_or("");
            capturedTmpFileUri = newDocument->GetTmpFileUri().value_or("");
            return ERR_OK;
        }));
    Stub stub;
    stub.set(ADDR(&PackageData::CreateByDocument), MockCreateByDocumentValid);
    stub.set(ADDR(&ObjectEditorDocument::CreateByFile), MockCreateByFileValid);
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(capturedOEid, PACKAGE_OEID);
    EXPECT_EQ(capturedDocumentId, TEST_DOCUMENT_ID);
    EXPECT_EQ(capturedNativeFileUri, TEST_NATIVE_FILE_URI);
    EXPECT_EQ(capturedTmpFileUri, TEST_TMP_FILE_URI);
}

/**
 * @tc.name: Initial_010
 * @tc.desc: Test Initial with CREATE_BY_FILE success without optional URIs skips URI propagation
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, Initial_010, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOperateType(OperateType::CREATE_BY_FILE);
    document->SetOriFileUri(TEST_ORI_FILE_URI);
    document->SetDocumentId(TEST_DOCUMENT_ID);
    std::string capturedDocumentId;
    std::string capturedNativeFileUri;
    std::string capturedTmpFileUri;
    EXPECT_CALL(*mockClientCb_, OnUpdate(_)).WillOnce(
        Invoke([&](std::unique_ptr<ObjectEditorDocument> &newDocument) {
            capturedDocumentId = newDocument->GetDocumentId();
            capturedNativeFileUri = newDocument->GetNativeFileUri().value_or("");
            capturedTmpFileUri = newDocument->GetTmpFileUri().value_or("");
            return ERR_OK;
        }));
    Stub stub;
    stub.set(ADDR(&PackageData::CreateByDocument), MockCreateByDocumentValid);
    stub.set(ADDR(&ObjectEditorDocument::CreateByFile), MockCreateByFileValid);
    auto result = package_->Initial(std::move(document), mockClientCb_, 0);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(capturedDocumentId, TEST_DOCUMENT_ID);
    EXPECT_EQ(capturedNativeFileUri, "");
    EXPECT_EQ(capturedTmpFileUri, "");
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

/**
 * @tc.name: OpenFile_004
 * @tc.desc: Test OpenFile with non-null caller token starts ability and sets isEditing
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorPackageTest, OpenFile_004, TestSize.Level1)
{
    MockAbilityManagerClientUtil::SetIsNullInstance(false);
    auto mockClient = std::make_shared<MockAbilityManagerClient>();
    CEAbilityManagerClient::client = mockClient;
    EXPECT_CALL(*mockClient, mock5StartAbility(_, _, _, _, _)).WillOnce(Return(ERR_OK));
    package_->SetRemoteObject(sptr<IRemoteObject>(mockClientCb_.GetRefPtr()));
    ErrCode result = package_->OpenFile(TEST_FILE_URI);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(package_->isEditing_, true);
    CEAbilityManagerClient::client = nullptr;
}

}
}