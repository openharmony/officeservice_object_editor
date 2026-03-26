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
#include "mock_hilog.h"
#include "mock_ability_manager_client.h"
#define protected public
#define private public
#include "object_editor_config.h"
#include "object_editor_manager_system_ability.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerSystemAbilityTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<ObjectEditorManagerSystemAbility> sa_;
};

void ObjectEditorManagerSystemAbilityTest::SetUpTestCase() {}

void ObjectEditorManagerSystemAbilityTest::TearDownTestCase() {}

void ObjectEditorManagerSystemAbilityTest::SetUp()
{
    sa_ = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(sa_, nullptr);
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorManagerSystemAbilityTest::TearDown() {}

/**
 * @tc.name: OnStart_AlreadyRunning
 * @tc.desc: Test OnStart for already running case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStart_AlreadyRunning, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_RUNNING;
    sa_->OnStart();
    EXPECT_EQ(sa_->state_, ServiceRunningState::STATE_RUNNING);
    EXPECT_TRUE(logMsg.find("already running") != std::string::npos);
}

/**
 * @tc.name: OnStart_Success
 * @tc.desc: Test OnStart for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStart_Success, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    sa_->OnStart();
    EXPECT_EQ(sa_->state_, ServiceRunningState::STATE_RUNNING);
    EXPECT_TRUE(logMsg.find("Publish success") != std::string::npos);
}

/**
 * @tc.name: OnStop_NotRunning
 * @tc.desc: Test OnStop for not running case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStop_NotRunning, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    sa_->OnStop();
    EXPECT_TRUE(logMsg.find("not running") != std::string::npos);
}

/**
 * @tc.name: OnStop_Success
 * @tc.desc: Test OnStop for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStop_Success, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_RUNNING;
    sa_->OnStop();
    EXPECT_EQ(sa_->state_, ServiceRunningState::STATE_NOT_START);
}

/**
 * @tc.name: OnIdle_Success
 * @tc.desc: Test OnIdle for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnIdle_Success, TestSize.Level1)
{
    SystemAbilityOnDemandReason idleReason;
    int32_t ret = sa_->OnIdle(idleReason);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: StartObjectEditorExtension_document_null
 * @tc.desc: Test StartObjectEditorExtension for document null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_document_null, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorClientCallback> callback = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: StartObjectEditorExtension_callback_null
 * @tc.desc: Test StartObjectEditorExtension for callback null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_callback_null, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetDocumentId("test_doc_id");
    sptr<IObjectEditorClientCallback> callback = nullptr;
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: StopObjectEditorExtension_documentId_empty
 * @tc.desc: Test StopObjectEditorExtension for documentId empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_documentId_empty, TestSize.Level1)
{
    std::string documentId = "";
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: StopObjectEditorExtension_extension_null
 * @tc.desc: Test StopObjectEditorExtension for extension null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_extension_null, TestSize.Level1)
{
    std::string documentId = "test_doc_id";
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name: StopObjectEditorExtension_package_extension_success
 * @tc.desc: Test StopObjectEditorExtension for package extension success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_package_extension_success, TestSize.Level1)
{
    std::string documentId = "test_doc_id";
    sptr<IRemoteObject> remoteObject = sptr<IRemoteObject>(new MockIRemoteObject());
    bool isPackageExtension = true;
    ErrCode ret = sa_->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: GetHmidByFileExtension_hmid_empty
 * @tc.desc: Test GetHmidByFileExtension for hmid empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetHmidByFileExtension_hmid_empty, TestSize.Level1)
{
    std::string hmid = "";
    std::string fileExtension;
    ErrCode ret = sa_->GetHmidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetHmidByFileExtension_success
 * @tc.desc: Test GetHmidByFileExtension for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetHmidByFileExtension_success, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string fileExtension;
    ErrCode ret = sa_->GetHmidByFileExtension(hmid, fileExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: GetIconByHmid_hmid_empty
 * @tc.desc: Test GetIconByHmid for hmid empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetIconByHmid_hmid_empty, TestSize.Level1)
{
    std::string hmid = "";
    std::string resourceId;
    ErrCode ret = sa_->GetIconByHmid(hmid, resourceId);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetIconByHmid_success
 * @tc.desc: Test GetIconByHmid for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetIconByHmid_success, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string resourceId;
    ErrCode ret = sa_->GetIconByHmid(hmid, resourceId);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: GetFormatName_hmid_empty
 * @tc.desc: Test GetFormatName for hmid empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_hmid_empty, TestSize.Level1)
{
    std::string hmid = "";
    std::string locale = "zh-CN";
    std::string formatName;
    ErrCode ret = sa_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetFormatName_locale_empty
 * @tc.desc: Test GetFormatName for locale empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_locale_empty, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string locale = "";
    std::string formatName;
    ErrCode ret = sa_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetFormatName_success
 * @tc.desc: Test GetFormatName for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_success, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string locale = "zh-CN";
    std::string formatName;
    ErrCode ret = sa_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: GetObjectEditorFormatByHmidAndLocale_hmid_empty
 * @tc.desc: Test GetObjectEditorFormatByHmidAndLocale for hmid empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatByHmidAndLocale_hmid_empty, TestSize.Level1)
{
    std::string hmid = "";
    std::string locale = "zh-CN";
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    ErrCode ret = sa_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetObjectEditorFormatByHmidAndLocale_locale_empty
 * @tc.desc: Test GetObjectEditorFormatByHmidAndLocale for locale empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatByHmidAndLocale_locale_empty, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string locale = "";
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    ErrCode ret = sa_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetObjectEditorFormatByHmidAndLocale_success
 * @tc.desc: Test GetObjectEditorFormatByHmidAndLocale for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatByHmidAndLocale_success, TestSize.Level1)
{
    std::string hmid = "test_hmid";
    std::string locale = "zh-CN";
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    ErrCode ret = sa_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_locale_empty
 * @tc.desc: Test GetObjectEditorFormatsByLocale for locale empty case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatsByLocale_locale_empty, TestSize.Level1)
{
    std::string locale = "";
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    ErrCode ret = sa_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_success
 * @tc.desc: Test GetObjectEditorFormatsByLocale for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatsByLocale_success, TestSize.Level1)
{
    std::string locale = "zh-CN";
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    ErrCode ret = sa_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: StartUIAbility_want_null
 * @tc.desc: Test StartUIAbility for want null case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartUIAbility_want_null, TestSize.Level1)
{
    std::unique_ptr<AAFwk::Want> want = nullptr;
    ErrCode ret = sa_->StartUIAbility(want);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: StartUIAbility_success
 * @tc.desc: Test StartUIAbility for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartUIAbility_success, TestSize.Level1)
{
    auto want = std::make_unique<AAFwk::Want>();
    ErrCode ret = sa_->StartUIAbility(want);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: CallbackEnter_success
 * @tc.desc: Test CallbackEnter for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_success, TestSize.Level1)
{
    uint32_t code = 1001;
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name: CallbackExit_success
 * @tc.desc: Test CallbackExit for success case
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackExit_success, TestSize.Level1)
{
    uint32_t code = 1001;
    int32_t result = 0;
    int32_t ret = sa_->CallbackExit(code, result);
    EXPECT_EQ(ret, ERR_OK);
}

} // namespace ObjectEditor
} // namespace OHOS
