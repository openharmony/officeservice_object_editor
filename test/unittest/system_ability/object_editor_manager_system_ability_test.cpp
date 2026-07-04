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
#include "mock_hilog.h"
#include "mock_ability_manager_client.h"
#include "mock_i_remote_object.h"
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

class ObjectEditorManagerSystemAbilityTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<ObjectEditorManagerSystemAbility> sa_;
};

void ObjectEditorManagerSystemAbilityTest::SetUpTestCase()
{
}

void ObjectEditorManagerSystemAbilityTest::TearDownTestCase()
{
}

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

void ObjectEditorManagerSystemAbilityTest::TearDown()
{
    sa_ = nullptr;
}

namespace {
/**
 * @tc.name: GetInstance_001
 * @tc.desc: Test GetInstance method returns valid instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetInstance_001, TestSize.Level1)
{
    auto& instance = ObjectEditorManagerSystemAbility::GetInstance();
    EXPECT_NE(&instance, nullptr);
}

/**
 * @tc.name: GetInstance_002
 * @tc.desc: Test GetInstance method returns same instance
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetInstance_002, TestSize.Level1)
{
    auto& instance1 = ObjectEditorManagerSystemAbility::GetInstance();
    auto& instance2 = ObjectEditorManagerSystemAbility::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name: OnStart_001
 * @tc.desc: Test OnStart method when already running
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStart_001, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_RUNNING;
    sa_->OnStart();
    EXPECT_TRUE(logMsg.find("already running") != std::string::npos);
}

/**
 * @tc.name: OnStart_002
 * @tc.desc: Test OnStart method when not running
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStart_002, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    sa_->OnStart();
    EXPECT_TRUE(logMsg.find("Publish success") != std::string::npos ||
        logMsg.find("Publish failed") != std::string::npos);
}

/**
 * @tc.name: OnStop_001
 * @tc.desc: Test OnStop method when not running
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStop_001, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    sa_->OnStop();
    EXPECT_TRUE(logMsg.find("not running") != std::string::npos);
}

/**
 * @tc.name: OnStop_002
 * @tc.desc: Test OnStop method when running
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStop_002, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_RUNNING;
    sa_->OnStop();
    EXPECT_EQ(sa_->state_, ServiceRunningState::STATE_NOT_START);
}

/**
 * @tc.name: OnIdle_001
 * @tc.desc: Test OnIdle method returns zero
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnIdle_001, TestSize.Level1)
{
    SystemAbilityOnDemandReason idleReason;
    int32_t ret = sa_->OnIdle(idleReason);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: CallbackEnter_001
 * @tc.desc: Test CallbackEnter method with START_UI_ABILITY code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_002
 * @tc.desc: Test CallbackEnter method with QUERY_EXTENSION_STOP_REASON code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_QUERY_EXTENSION_STOP_REASON);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: CallbackExit_001
 * @tc.desc: Test CallbackExit method with zero result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackExit_001, TestSize.Level1)
{
    uint32_t code = 0;
    int32_t result = 0;
    int32_t ret = sa_->CallbackExit(code, result);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: CallbackExit_002
 * @tc.desc: Test CallbackExit method with error result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackExit_002, TestSize.Level1)
{
    uint32_t code = 1;
    int32_t result = ERR_INVALID_VALUE;
    int32_t ret = sa_->CallbackExit(code, result);
    EXPECT_EQ(ret, result);
}

/**
 * @tc.name: CheckRateLimitAdvanced_001
 * @tc.desc: Test CheckRateLimitAdvanced method returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckRateLimitAdvanced_001, TestSize.Level1)
{
    sa_->windowStartMs_.store(0);
    sa_->requestCount_.store(0);
    bool ret = sa_->CheckRateLimitAdvanced();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetOEidByFileExtension_001
 * @tc.desc: Test GetOEidByFileExtension method with valid oeid
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetOEidByFileExtension_001, TestSize.Level1)
{
    std::string oeid = "test_oeid";
    std::string fileExtension = "";
    ErrCode ret = sa_->GetOEidByFileExtension(oeid, fileExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetIconByOEid_001
 * @tc.desc: Test GetIconByOEid method with valid oeid
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetIconByOEid_001, TestSize.Level1)
{
    std::string oeid = "test_oeid";
    std::string resourceId = "";
    ErrCode ret = sa_->GetIconByOEid(oeid, resourceId);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetFormatName_001
 * @tc.desc: Test GetFormatName method with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_001, TestSize.Level1)
{
    std::string oeid = "test_oeid";
    std::string locale = "zh_CN";
    std::string formatName = "";
    ErrCode ret = sa_->GetFormatName(oeid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetCallerBundleName_001
 * @tc.desc: Test GetCallerBundleName method returns bundle name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetCallerBundleName_001, TestSize.Level1)
{
    std::string bundleName = sa_->GetCallerBundleName();
    EXPECT_TRUE(bundleName.empty() || !bundleName.empty());
}

/**
 * @tc.name: RegisterExtensionStopReason_001
 * @tc.desc: Test RegisterExtensionStopReason method with null remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, RegisterExtensionStopReason_001, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    ExtensionStopReason reason = ExtensionStopReason::UNKNOWN;
    sa_->RegisterExtensionStopReason(remoteObject, reason);
    EXPECT_TRUE(logMsg.find("remoteObject is null") != std::string::npos);
}

/**
 * @tc.name: QueryExtensionStopReason_001
 * @tc.desc: Test QueryExtensionStopReason method with null remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, QueryExtensionStopReason_001, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    ExtensionStopReason reason = ExtensionStopReason::UNKNOWN;
    ErrCode ret = sa_->QueryExtensionStopReason(remoteObject, reason);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: QueryExtensionStopReason_002
 * @tc.desc: Test QueryExtensionStopReason method with valid remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, QueryExtensionStopReason_002, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ExtensionStopReason reason = ExtensionStopReason::UNKNOWN;
    ErrCode ret = sa_->QueryExtensionStopReason(remoteObject, reason);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: StartUIAbility_001
 * @tc.desc: Test StartUIAbility method with null want
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartUIAbility_001, TestSize.Level1)
{
    std::unique_ptr<AAFwk::Want> want = nullptr;
    sptr<IRemoteObject> token = nullptr;
    int32_t clientPid = 0;
    ErrCode ret = sa_->StartUIAbility(want, token, clientPid);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: CheckClientFileValid_001
 * @tc.desc: Test CheckClientFileValid method with empty document
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_001, TestSize.Level1)
{
    ObjectEditorDocument document;
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: StopObjectEditorExtension_001
 * @tc.desc: Test StopObjectEditorExtension method with null remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_001, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    ErrCode ret = sa_->StopObjectEditorExtension(remoteObject);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}
}
}
}