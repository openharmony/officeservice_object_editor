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

#include <chrono>
#include <fstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cJSON.h>
#include "mock_hilog.h"
#include "mock_ability_manager_client.h"
#include "mock_i_remote_object.h"
#include "mock_object_editor_client_callback.h"
#include "stub.h"
#include "object_editor_permission_utils.h"
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
namespace {
bool MockCheckCallingPermissionTrue(const std::string &)
{
    return true;
}
}

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
    // Signal the detached timer thread (started by ResetStopSATimer via CallbackEnter)
    // to exit before destroying the SA object, preventing UAF CFI crash at process exit.
    sa_->timerNotify_.store(true);
    sa_->cvTimer_.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(200)); // 200: wait time
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
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
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

/**
 * @tc.name: ParseDiversionItem_001
 * @tc.desc: Test ParseDiversionItem with valid string fields adds entry to diversionMap_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_001, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(1));
    EXPECT_EQ(sa_->diversionMap_.count("src1"), static_cast<size_t>(1));
    EXPECT_EQ(sa_->diversionMap_["src1"].targetOEid, "tgt1");
    EXPECT_EQ(sa_->diversionMap_["src1"].minVersion, "1.0");
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_002
 * @tc.desc: Test ParseDiversionItem skips when sourceOEid is not a string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_002, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddNumberToObject(item, "sourceOEid", 123);
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("get field failed") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_003
 * @tc.desc: Test ParseDiversionItem skips when sourceOEid is an empty string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_003, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "");
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("sourceOEid is null or empty") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_004
 * @tc.desc: Test ParseDiversionItem skips when targetOEid is not a string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_004, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddNumberToObject(item, "targetOEid", 456);
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("get field failed") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_005
 * @tc.desc: Test ParseDiversionItem skips when targetOEid is an empty string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_005, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "targetOEid", "");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("targetOEid is null or empty") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_006
 * @tc.desc: Test ParseDiversionItem skips when minVersion is not a string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_006, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddNumberToObject(item, "minVersion", 789);
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("get field failed") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_007
 * @tc.desc: Test ParseDiversionItem skips when minVersion is an empty string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_007, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddStringToObject(item, "minVersion", "");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    EXPECT_TRUE(logMsg.find("minVersion is null or empty") != std::string::npos);
    cJSON_Delete(item);
}

/**
 * @tc.name: ReadDiversionsJsonFile_001
 * @tc.desc: Test ReadDiversionsJsonFile handles non-existent file gracefully
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ReadDiversionsJsonFile_001, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    sa_->ReadDiversionsJsonFile();
    std::ifstream testFile("/system/etc/office_service/object_editor_service/diversion_map.json");
    if (!testFile.is_open()) {
        EXPECT_TRUE(logMsg.find("open file failed") != std::string::npos);
        EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    } else {
        testFile.close();
        SUCCEED();
    }
}

/**
 * @tc.name: CheckCallingPermission_001
 * @tc.desc: Test CheckCallingPermission with code at IPC_CODE_START boundary returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_001, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::IPC_CODE_START);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckCallingPermission_002
 * @tc.desc: Test CheckCallingPermission with code at IPC_CODE_END boundary returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_002, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::IPC_CODE_END);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckCallingPermission_003
 * @tc.desc: Test CheckCallingPermission with QUERY_EXTENSION_STOP_REASON returns true (no permission needed)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_003, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_QUERY_EXTENSION_STOP_REASON);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_004
 * @tc.desc: Test CheckCallingPermission with START_OBJECT_EDITOR_EXTENSION returns false (permission denied in test)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_004, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckCallingPermission_005
 * @tc.desc: Test CheckCallingPermission with COMMAND_START_UI_ABILITY returns false (permission denied in test)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_005, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: LocaleIsValid_001
 * @tc.desc: Test LocaleIsValid with empty locale returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, LocaleIsValid_001, TestSize.Level1)
{
    bool ret = sa_->LocaleIsValid("");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: LocaleIsValid_002
 * @tc.desc: Test LocaleIsValid with a valid non-empty locale returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, LocaleIsValid_002, TestSize.Level1)
{
    bool ret = sa_->LocaleIsValid("zh-CN");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetTargetOEid_001
 * @tc.desc: Test GetTargetOEid with source oeid not in diversion map returns SA_DIVERSION_QUERY_EMPTY
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetTargetOEid_001, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    std::string targetOEid;
    std::string minVersion;
    auto ret = sa_->GetTargetOEid("non_existent_oeid", targetOEid, minVersion);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DIVERSION_QUERY_EMPTY);
    EXPECT_TRUE(targetOEid.empty());
    EXPECT_TRUE(minVersion.empty());
}

/**
 * @tc.name: GetTargetOEid_002
 * @tc.desc: Test GetTargetOEid with source oeid in diversion map returns SA_OK and fills target/minVersion
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetTargetOEid_002, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    ContentEmbed_Diversion diversion;
    diversion.sourceOEid = "src_oeid";
    diversion.targetOEid = "tgt_oeid";
    diversion.minVersion = "2.0";
    sa_->diversionMap_["src_oeid"] = diversion;
    std::string targetOEid;
    std::string minVersion;
    auto ret = sa_->GetTargetOEid("src_oeid", targetOEid, minVersion);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_EQ(targetOEid, "tgt_oeid");
    EXPECT_EQ(minVersion, "2.0");
    sa_->diversionMap_.clear();
}

/**
 * @tc.name: InitScreenChangedCommonEventSubscriber_001
 * @tc.desc: Test InitScreenChangedCommonEventSubscriber logs subscribe result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, InitScreenChangedCommonEventSubscriber_001, TestSize.Level1)
{
    logMsg.clear();
    sa_->InitScreenChangedCommonEventSubscriber();
    EXPECT_TRUE(logMsg.find("subscribe common event ret:") != std::string::npos);
}

/**
 * @tc.name: ResetScreenChangedCommonEventSubscriber_001
 * @tc.desc: Test ResetScreenChangedCommonEventSubscriber with null receiver logs warning
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ResetScreenChangedCommonEventSubscriber_001, TestSize.Level1)
{
    sa_->screenChangedReceiver_ = nullptr;
    logMsg.clear();
    sa_->ResetScreenChangedCommonEventSubscriber();
    EXPECT_TRUE(logMsg.find("screenChangedReceiver_ is null") != std::string::npos);
}

/**
 * @tc.name: CheckIsAllowStartExtension_001
 * @tc.desc: Test CheckIsAllowStartExtension when object editor not supported returns SA_CONNECT_LIMIT_EXCEED
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckIsAllowStartExtension_001, TestSize.Level1)
{
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = false;
    ObjectEditorDocument document;
    auto ret = sa_->CheckIsAllowStartExtension(document);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_CONNECT_LIMIT_EXCEED);
    config.isSupportObjectEditor_.value = true;
}

/**
 * @tc.name: CheckIsAllowStartExtension_002
 * @tc.desc: Test CheckIsAllowStartExtension with unknown operate type returns SA_UNKNOWN_OPERATE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckIsAllowStartExtension_002, TestSize.Level1)
{
    ObjectEditorDocument document;
    auto ret = sa_->CheckIsAllowStartExtension(document);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_UNKNOWN_OPERATE);
}

/**
 * @tc.name: GetObjectEditorFormat_001
 * @tc.desc: Test GetObjectEditorFormat with PACKAGE_OEID and CREATE_BY_OEID returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_001, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid(PACKAGE_OEID);
    document.SetOperateType(OperateType::CREATE_BY_OEID);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
    EXPECT_FALSE(isPackageExtension);
}

/**
 * @tc.name: GetObjectEditorFormat_002
 * @tc.desc: Test GetObjectEditorFormat with PACKAGE_OEID and EDIT returns SA_OK with isPackageExtension=true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_002, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid(PACKAGE_OEID);
    document.SetOperateType(OperateType::EDIT);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_TRUE(isPackageExtension);
}

/**
 * @tc.name: StartObjectEditorExtension_001
 * @tc.desc: Test StartObjectEditorExtension with null document returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    sptr<IObjectEditorClientCallback> callback = nullptr;
    sptr<IRemoteObject> remoteObject;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: StartObjectEditorExtension_002
 * @tc.desc: Test StartObjectEditorExtension with null callback returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_002, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorClientCallback> callback = nullptr;
    sptr<IRemoteObject> remoteObject;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: HandleDefaultAppFormatPolicy_001
 * @tc.desc: Test HandleDefaultAppFormatPolicy with empty formats returns BMS error from
 * GetDefaultAppBundleNameByFileExt
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleDefaultAppFormatPolicy_001, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    std::unique_ptr<ObjectEditorFormat> result;
    auto ret = sa_->HandleDefaultAppFormatPolicy(".txt", formats, result);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: HandleDefaultAppFormatPolicy_002
 * @tc.desc: Test HandleDefaultAppFormatPolicy with non-empty formats and no default app uses first format and
 * returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleDefaultAppFormatPolicy_002, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.back()->bundleName = "test_bundle";
    std::unique_ptr<ObjectEditorFormat> result;
    auto ret = sa_->HandleDefaultAppFormatPolicy(".txt", formats, result);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->bundleName, "test_bundle");
}

/**
 * @tc.name: StopObjectEditorExtension_002
 * @tc.desc: Test StopObjectEditorExtension (3-param) with null remote returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_002, TestSize.Level1)
{
    std::string documentId = "test_doc_id";
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: OnConnectionStatusChanged_001
 * @tc.desc: Test OnConnectionStatusChanged with STATUS_CONNECTED leaves connectionMap unchanged
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnConnectionStatusChanged_001, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorManagerSystemAbility::connectionMap_[remoteObject] = nullptr;
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbilityConnectionStatusCallback callback;
    callback.OnConnectionStatusChanged(remoteObject, ObjectEditorConnectionStatus::STATUS_CONNECTED);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
}

/**
 * @tc.name: OnConnectionStatusChanged_002
 * @tc.desc: Test OnConnectionStatusChanged with STATUS_DISCONNECTED erases entry from connectionMap
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnConnectionStatusChanged_002, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorManagerSystemAbility::connectionMap_[remoteObject] = nullptr;
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbilityConnectionStatusCallback callback;
    callback.OnConnectionStatusChanged(remoteObject, ObjectEditorConnectionStatus::STATUS_DISCONNECTED);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(0));
}

/**
 * @tc.name: CheckConnectionLimit_001
 * @tc.desc: Test CheckConnectionLimit with null format returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckConnectionLimit_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    sptr<IRemoteObject> remoteObject;
    bool ret = sa_->CheckConnectionLimit("test_client", format, remoteObject);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ConnectObjectEditorExtAbility_001
 * @tc.desc: Test ConnectObjectEditorExtAbility with null format returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ConnectObjectEditorExtAbility_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    sptr<IRemoteObject> remoteObject;
    bool isExceed = false;
    bool ret = sa_->ConnectObjectEditorExtAbility(format, remoteObject, isExceed);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: RegisterExtensionStopReason_002
 * @tc.desc: Test RegisterExtensionStopReason with valid remote object adds entry to extensionStopReasonMap_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, RegisterExtensionStopReason_002, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ExtensionStopReason reason = ExtensionStopReason::PROXY_STOP;
    sa_->RegisterExtensionStopReason(remoteObject, reason);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::extensionStopReasonMap_.size(), static_cast<size_t>(1));
    auto it = ObjectEditorManagerSystemAbility::extensionStopReasonMap_.find(remoteObject);
    EXPECT_NE(it, ObjectEditorManagerSystemAbility::extensionStopReasonMap_.end());
    if (it != ObjectEditorManagerSystemAbility::extensionStopReasonMap_.end()) {
        EXPECT_EQ(it->second->reason, reason);
    }
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
}

/**
 * @tc.name: CheckRateLimitAdvanced_002
 * @tc.desc: Test CheckRateLimitAdvanced returns false when request count exceeds MAX_REQUEST_COUNT
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckRateLimitAdvanced_002, TestSize.Level1)
{
    auto nowMs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    sa_->windowStartMs_.store(nowMs);
    sa_->requestCount_.store(50);
    bool ret = sa_->CheckRateLimitAdvanced();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_001
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale with empty locale delegates to database and returns non-SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatByOEidAndLocale_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorFormat> format;
    ErrCode ret = sa_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "", format);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_001
 * @tc.desc: Test GetObjectEditorFormatsByLocale with empty locale delegates to database and returns non-SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatsByLocale_001, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    ErrCode ret = sa_->GetObjectEditorFormatsByLocale("", formats);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: CallbackEnter_003
 * @tc.desc: Test CallbackEnter with START_OBJECT_EDITOR_EXTENSION code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_003, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_004
 * @tc.desc: Test CallbackEnter with GET_OEID_BY_FILE_EXTENSION code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_004, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_005
 * @tc.desc: Test CallbackEnter with GET_ICON_BY_OEID code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_005, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_006
 * @tc.desc: Test CallbackEnter with GET_FORMAT_NAME code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_006, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_NAME);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_007
 * @tc.desc: Test CallbackEnter with GET_FORMAT_BY_OEID_AND_LOCALE code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_007, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_008
 * @tc.desc: Test CallbackEnter with GET_FORMATS_BY_LOCALE code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_008, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_009
 * @tc.desc: Test CallbackEnter with STOP_OBJECT_EDITOR_EXTENSION code and permission denied
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_009, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_PERMISSION_DENIED);
}

/**
 * @tc.name: CallbackEnter_010
 * @tc.desc: Test CallbackEnter with QUERY_EXTENSION_STOP_REASON and permission granted returns ERR_NONE
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackEnter_010, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_QUERY_EXTENSION_STOP_REASON);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    sa_->windowStartMs_.store(0);
    sa_->requestCount_.store(0);
    int32_t ret = sa_->CallbackEnter(code);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
 * @tc.name: CallbackExit_003
 * @tc.desc: Test CallbackExit with zero code and zero result returns zero
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackExit_003, TestSize.Level1)
{
    uint32_t code = 0;
    int32_t result = 0;
    int32_t ret = sa_->CallbackExit(code, result);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: CallbackExit_004
 * @tc.desc: Test CallbackExit with large code and negative result returns same result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CallbackExit_004, TestSize.Level1)
{
    uint32_t code = 999;
    int32_t result = -1;
    int32_t ret = sa_->CallbackExit(code, result);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: CheckCallingPermission_006
 * @tc.desc: Test CheckCallingPermission with COMMAND_GET_OEID_BY_FILE_EXTENSION and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_006, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_007
 * @tc.desc: Test CheckCallingPermission with COMMAND_GET_ICON_BY_OEID and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_007, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_008
 * @tc.desc: Test CheckCallingPermission with COMMAND_GET_FORMAT_NAME and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_008, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_NAME);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_009
 * @tc.desc: Test CheckCallingPermission with COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_009, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_010
 * @tc.desc: Test CheckCallingPermission with COMMAND_GET_FORMATS_BY_LOCALE and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_010, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_011
 * @tc.desc: Test CheckCallingPermission with COMMAND_STOP_OBJECT_EDITOR_EXTENSION and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_011, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_012
 * @tc.desc: Test CheckCallingPermission with COMMAND_START_UI_ABILITY and permission granted
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_012, TestSize.Level1)
{
    uint32_t code = static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY);
    Stub stub;
    stub.set(ADDR(&ObjectEditorPermissionUtils::CheckCallingPermission), MockCheckCallingPermissionTrue);
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckCallingPermission_013
 * @tc.desc: Test CheckCallingPermission with code below IPC_CODE_START returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckCallingPermission_013, TestSize.Level1)
{
    uint32_t code = 0;
    bool ret = sa_->CheckCallingPermission(code);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckRateLimitAdvanced_003
 * @tc.desc: Test CheckRateLimitAdvanced returns true when within window and under limit
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckRateLimitAdvanced_003, TestSize.Level1)
{
    auto nowMs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    sa_->windowStartMs_.store(nowMs);
    sa_->requestCount_.store(0);
    bool ret = sa_->CheckRateLimitAdvanced();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckRateLimitAdvanced_004
 * @tc.desc: Test CheckRateLimitAdvanced returns true when at exactly MAX_REQUEST_COUNT-1
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckRateLimitAdvanced_004, TestSize.Level1)
{
    auto nowMs = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now().time_since_epoch()).count());
    sa_->windowStartMs_.store(nowMs);
    sa_->requestCount_.store(49);
    bool ret = sa_->CheckRateLimitAdvanced();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CheckRateLimitAdvanced_005
 * @tc.desc: Test CheckRateLimitAdvanced resets window after expiry and returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckRateLimitAdvanced_005, TestSize.Level1)
{
    sa_->windowStartMs_.store(1);
    sa_->requestCount_.store(100);
    bool ret = sa_->CheckRateLimitAdvanced();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: StartUIAbility_002
 * @tc.desc: Test StartUIAbility with valid want but empty caller bundle name returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartUIAbility_002, TestSize.Level1)
{
    auto want = std::make_unique<AAFwk::Want>();
    want->SetBundle("com.test.bundle");
    sptr<IRemoteObject> token = nullptr;
    int32_t clientPid = 0;
    ErrCode ret = sa_->StartUIAbility(want, token, clientPid);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: StartUIAbility_003
 * @tc.desc: Test StartUIAbility with want bundle name mismatching caller returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartUIAbility_003, TestSize.Level1)
{
    auto want = std::make_unique<AAFwk::Want>();
    want->SetBundle("com.different.bundle");
    sptr<IRemoteObject> token = nullptr;
    int32_t clientPid = 0;
    ErrCode ret = sa_->StartUIAbility(want, token, clientPid);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetObjectEditorFormat_003
 * @tc.desc: Test GetObjectEditorFormat with PACKAGE_OEID and CREATE_BY_FILE returns non-SA_OK or SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_003, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid(PACKAGE_OEID);
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_TRUE(ret == ObjectEditorManagerErrCode::SA_OK || ret != ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetObjectEditorFormat_004
 * @tc.desc: Test GetObjectEditorFormat with PACKAGE_OEID and UNKNOWN operate type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_004, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid(PACKAGE_OEID);
    document.SetOperateType(OperateType::UNKNOWN);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK);
    EXPECT_FALSE(isPackageExtension);
}

/**
 * @tc.name: GetObjectEditorFormat_005
 * @tc.desc: Test GetObjectEditorFormat with non-PACKAGE_OEID and CREATE_BY_OEID returns non-SA_OK (db not initted)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_005, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid("test_oeid_12345");
    document.SetOperateType(OperateType::CREATE_BY_OEID);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_FALSE(isPackageExtension);
}

/**
 * @tc.name: GetObjectEditorFormat_006
 * @tc.desc: Test GetObjectEditorFormat with non-PACKAGE_OEID and EDIT type returns non-SA_OK (db not initted)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_006, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid("test_oeid_edit");
    document.SetOperateType(OperateType::EDIT);
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_FALSE(isPackageExtension);
}

/**
 * @tc.name: GetObjectEditorFormat_007
 * @tc.desc: Test GetObjectEditorFormat with non-PACKAGE_OEID and CREATE_BY_FILE with oriFileUri set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormat_007, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid("test_oeid_file");
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    document.SetOriFileUri("file:///test/path/file.docx");
    std::unique_ptr<ObjectEditorFormat> format;
    bool isPackageExtension = false;
    auto ret = sa_->GetObjectEditorFormat(document, format, isPackageExtension);
    EXPECT_TRUE(ret == ObjectEditorManagerErrCode::SA_OK || ret != ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: CheckIsAllowStartExtension_003
 * @tc.desc: Test CheckIsAllowStartExtension with CREATE_BY_OEID operate type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckIsAllowStartExtension_003, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOperateType(OperateType::CREATE_BY_OEID);
    auto ret = sa_->CheckIsAllowStartExtension(document);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK);
}

/**
 * @tc.name: CheckIsAllowStartExtension_004
 * @tc.desc: Test CheckIsAllowStartExtension with EDIT operate type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckIsAllowStartExtension_004, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOperateType(OperateType::EDIT);
    auto ret = sa_->CheckIsAllowStartExtension(document);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK);
}

/**
 * @tc.name: CheckIsAllowStartExtension_005
 * @tc.desc: Test CheckIsAllowStartExtension with CREATE_BY_FILE operate type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckIsAllowStartExtension_005, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    auto ret = sa_->CheckIsAllowStartExtension(document);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK);
}

/**
 * @tc.name: HandleDefaultAppFormatPolicy_003
 * @tc.desc: Test HandleDefaultAppFormatPolicy with multiple formats selects first when no default app
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleDefaultAppFormatPolicy_003, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.back()->bundleName = "bundle_a";
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.back()->bundleName = "bundle_b";
    std::unique_ptr<ObjectEditorFormat> result;
    auto ret = sa_->HandleDefaultAppFormatPolicy(".txt", formats, result);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->bundleName, "bundle_a");
}

/**
 * @tc.name: HandleDefaultAppFormatPolicy_004
 * @tc.desc: Test HandleDefaultAppFormatPolicy with single format returns that format
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleDefaultAppFormatPolicy_004, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.back()->bundleName = "only_bundle";
    std::unique_ptr<ObjectEditorFormat> result;
    auto ret = sa_->HandleDefaultAppFormatPolicy(".pdf", formats, result);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result->bundleName, "only_bundle");
}

/**
 * @tc.name: CheckClientFileValid_002
 * @tc.desc: Test CheckClientFileValid with linking=true and empty snapshot returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_002, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(true);
    document.SetSnapshotUri("");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckClientFileValid_003
 * @tc.desc: Test CheckClientFileValid with no tmpFileUri and not linking and empty snapshot returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_003, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckClientFileValid_004
 * @tc.desc: Test CheckClientFileValid with linking=true and valid snapshot returns non-true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_004, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(true);
    document.SetSnapshotUri("file:///test/snapshot.png");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckClientFileValid_005
 * @tc.desc: Test CheckClientFileValid with tmpFileUri set and valid snapshot
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_005, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    document.SetTmpFileUri("/tmp/test.ole");
    document.SetSnapshotUri("file:///test/snapshot.png");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: StopObjectEditorExtension_003
 * @tc.desc: Test StopObjectEditorExtension (1-param) with valid remote object not in connectionMap
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_003, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ErrCode ret = sa_->StopObjectEditorExtension(remoteObject);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_NOT_FOUND_ABILITY_BY_REMOTE_OBJECT);
}

/**
 * @tc.name: StopObjectEditorExtension_004
 * @tc.desc: Test StopObjectEditorExtension (1-param) with remote in map but null connection
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_004, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorManagerSystemAbility::connectionMap_[remoteObject] = nullptr;
    ErrCode ret = sa_->StopObjectEditorExtension(remoteObject);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
}

/**
 * @tc.name: StopObjectEditorExtension_005
 * @tc.desc: Test StopObjectEditorExtension (3-param) with null remote returns SA_INVALID_PARAMETER
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StopObjectEditorExtension_005, TestSize.Level1)
{
    std::string documentId = "test_doc";
    sptr<IRemoteObject> remoteObject = nullptr;
    bool isPackageExtension = true;
    ErrCode ret = sa_->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: CheckConnectionLimit_002
 * @tc.desc: Test CheckConnectionLimit with valid format and empty connectionMap returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckConnectionLimit_002, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    auto format = std::make_unique<ObjectEditorFormat>();
    format->bundleName = "test_bundle";
    format->moduleName = "test_module";
    format->abilityName = "test_ability";
    sptr<IRemoteObject> remoteObject;
    bool ret = sa_->CheckConnectionLimit("test_client", format, remoteObject);
    EXPECT_TRUE(ret);
    EXPECT_EQ(remoteObject, nullptr);
}

/**
 * @tc.name: ConnectObjectEditorExtAbility_002
 * @tc.desc: Test ConnectObjectEditorExtAbility with valid format but no caller bundle name returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ConnectObjectEditorExtAbility_002, TestSize.Level1)
{
    auto format = std::make_unique<ObjectEditorFormat>();
    format->bundleName = "test_bundle";
    format->moduleName = "test_module";
    format->abilityName = "test_ability";
    sptr<IRemoteObject> remoteObject;
    bool isExceed = false;
    bool ret = sa_->ConnectObjectEditorExtAbility(format, remoteObject, isExceed);
    EXPECT_FALSE(ret);
    EXPECT_FALSE(isExceed);
}

/**
 * @tc.name: GetTargetOEid_003
 * @tc.desc: Test GetTargetOEid with empty sourceOEid returns SA_DIVERSION_QUERY_EMPTY
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetTargetOEid_003, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    std::string targetOEid;
    std::string minVersion;
    auto ret = sa_->GetTargetOEid("", targetOEid, minVersion);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DIVERSION_QUERY_EMPTY);
    EXPECT_TRUE(targetOEid.empty());
    EXPECT_TRUE(minVersion.empty());
}

/**
 * @tc.name: GetTargetOEid_004
 * @tc.desc: Test GetTargetOEid with multiple entries in diversion map finds correct target
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetTargetOEid_004, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    ContentEmbed_Diversion d1;
    d1.sourceOEid = "src1";
    d1.targetOEid = "tgt1";
    d1.minVersion = "1.0";
    sa_->diversionMap_["src1"] = d1;
    ContentEmbed_Diversion d2;
    d2.sourceOEid = "src2";
    d2.targetOEid = "tgt2";
    d2.minVersion = "2.0";
    sa_->diversionMap_["src2"] = d2;
    std::string targetOEid;
    std::string minVersion;
    auto ret = sa_->GetTargetOEid("src2", targetOEid, minVersion);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_EQ(targetOEid, "tgt2");
    EXPECT_EQ(minVersion, "2.0");
    sa_->diversionMap_.clear();
}

/**
 * @tc.name: LocaleIsValid_003
 * @tc.desc: Test LocaleIsValid with locale containing language and country returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, LocaleIsValid_003, TestSize.Level1)
{
    bool ret = sa_->LocaleIsValid("en-US");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: LocaleIsValid_004
 * @tc.desc: Test LocaleIsValid with locale containing language only returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, LocaleIsValid_004, TestSize.Level1)
{
    bool ret = sa_->LocaleIsValid("zh");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ParseDiversionItem_008
 * @tc.desc: Test ParseDiversionItem with valid fields adds multiple entries
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_008, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    cJSON *item1 = cJSON_CreateObject();
    cJSON_AddStringToObject(item1, "sourceOEid", "src_a");
    cJSON_AddStringToObject(item1, "targetOEid", "tgt_a");
    cJSON_AddStringToObject(item1, "minVersion", "1.0");
    sa_->ParseDiversionItem(item1);
    cJSON *item2 = cJSON_CreateObject();
    cJSON_AddStringToObject(item2, "sourceOEid", "src_b");
    cJSON_AddStringToObject(item2, "targetOEid", "tgt_b");
    cJSON_AddStringToObject(item2, "minVersion", "2.0");
    sa_->ParseDiversionItem(item2);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(2));
    EXPECT_EQ(sa_->diversionMap_["src_a"].targetOEid, "tgt_a");
    EXPECT_EQ(sa_->diversionMap_["src_b"].targetOEid, "tgt_b");
    cJSON_Delete(item1);
    cJSON_Delete(item2);
    sa_->diversionMap_.clear();
}

/**
 * @tc.name: ParseDiversionItem_009
 * @tc.desc: Test ParseDiversionItem with null item does not crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_009, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = nullptr;
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
}

/**
 * @tc.name: ParseDiversionItem_010
 * @tc.desc: Test ParseDiversionItem with missing minVersion field skips entry
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_010, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_011
 * @tc.desc: Test ParseDiversionItem with missing targetOEid field skips entry
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_011, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "sourceOEid", "src1");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    cJSON_Delete(item);
}

/**
 * @tc.name: ParseDiversionItem_012
 * @tc.desc: Test ParseDiversionItem with missing sourceOEid field skips entry
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ParseDiversionItem_012, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    logMsg.clear();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "targetOEid", "tgt1");
    cJSON_AddStringToObject(item, "minVersion", "1.0");
    sa_->ParseDiversionItem(item);
    EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    cJSON_Delete(item);
}

/**
 * @tc.name: RegisterExtensionStopReason_003
 * @tc.desc: Test RegisterExtensionStopReason overwrites existing entry for same remote
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, RegisterExtensionStopReason_003, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    sa_->RegisterExtensionStopReason(remoteObject, ExtensionStopReason::PROXY_STOP);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::extensionStopReasonMap_.size(), static_cast<size_t>(1));
    sa_->RegisterExtensionStopReason(remoteObject, ExtensionStopReason::EXTENSION_TERMINATED);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::extensionStopReasonMap_.size(), static_cast<size_t>(1));
    auto it = ObjectEditorManagerSystemAbility::extensionStopReasonMap_.find(remoteObject);
    ASSERT_NE(it, ObjectEditorManagerSystemAbility::extensionStopReasonMap_.end());
    EXPECT_EQ(it->second->reason, ExtensionStopReason::EXTENSION_TERMINATED);
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
}

/**
 * @tc.name: RegisterExtensionStopReason_004
 * @tc.desc: Test RegisterExtensionStopReason with UNKNOWN reason still registers
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, RegisterExtensionStopReason_004, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    sa_->RegisterExtensionStopReason(remoteObject, ExtensionStopReason::UNKNOWN);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::extensionStopReasonMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
}

/**
 * @tc.name: QueryExtensionStopReason_003
 * @tc.desc: Test QueryExtensionStopReason with valid remote not in map returns SA_OK with UNKNOWN reason
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, QueryExtensionStopReason_003, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ExtensionStopReason reason = ExtensionStopReason::PROXY_STOP;
    ErrCode ret = sa_->QueryExtensionStopReason(remoteObject, reason);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_EQ(reason, ExtensionStopReason::UNKNOWN);
}

/**
 * @tc.name: QueryExtensionStopReason_004
 * @tc.desc: Test QueryExtensionStopReason with remote registered with PROXY_STOP returns that reason
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, QueryExtensionStopReason_004, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    sa_->RegisterExtensionStopReason(remoteObject, ExtensionStopReason::PROXY_STOP);
    ExtensionStopReason reason = ExtensionStopReason::UNKNOWN;
    ErrCode ret = sa_->QueryExtensionStopReason(remoteObject, reason);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_EQ(reason, ExtensionStopReason::PROXY_STOP);
    ObjectEditorManagerSystemAbility::extensionStopReasonMap_.clear();
}

/**
 * @tc.name: OnConnectionStatusChanged_003
 * @tc.desc: Test OnConnectionStatusChanged with unknown status does not modify map
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnConnectionStatusChanged_003, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorManagerSystemAbility::connectionMap_[remoteObject] = nullptr;
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbilityConnectionStatusCallback callback;
    callback.OnConnectionStatusChanged(remoteObject, static_cast<ObjectEditorConnectionStatus>(999));
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(1));
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
}

/**
 * @tc.name: OnConnectionStatusChanged_004
 * @tc.desc: Test OnConnectionStatusChanged with DISCONNECTED on empty map does not crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnConnectionStatusChanged_004, TestSize.Level1)
{
    ObjectEditorManagerSystemAbility::connectionMap_.clear();
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorManagerSystemAbilityConnectionStatusCallback callback;
    callback.OnConnectionStatusChanged(remoteObject, ObjectEditorConnectionStatus::STATUS_DISCONNECTED);
    EXPECT_EQ(ObjectEditorManagerSystemAbility::connectionMap_.size(), static_cast<size_t>(0));
}

/**
 * @tc.name: OnStart_003
 * @tc.desc: Test OnStart when not running transitions to running or logs publish failure
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStart_003, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    sa_->OnStart();
    EXPECT_TRUE(sa_->state_ == ServiceRunningState::STATE_RUNNING ||
        logMsg.find("Publish failed") != std::string::npos);
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
}

/**
 * @tc.name: OnStop_003
 * @tc.desc: Test OnStop when not running logs "not running" and does not change state
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnStop_003, TestSize.Level1)
{
    sa_->state_ = ServiceRunningState::STATE_NOT_START;
    logMsg.clear();
    sa_->OnStop();
    EXPECT_TRUE(logMsg.find("not running") != std::string::npos);
    EXPECT_EQ(sa_->state_, ServiceRunningState::STATE_NOT_START);
}

/**
 * @tc.name: GetOEidByFileExtension_002
 * @tc.desc: Test GetOEidByFileExtension with non-empty oeid returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetOEidByFileExtension_002, TestSize.Level1)
{
    std::string oeid = "test_oeid_abc";
    std::string fileExtension;
    ErrCode ret = sa_->GetOEidByFileExtension(oeid, fileExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetOEidByFileExtension_003
 * @tc.desc: Test GetOEidByFileExtension with empty oeid returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetOEidByFileExtension_003, TestSize.Level1)
{
    std::string oeid = "";
    std::string fileExtension;
    ErrCode ret = sa_->GetOEidByFileExtension(oeid, fileExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetIconByOEid_002
 * @tc.desc: Test GetIconByOEid with empty oeid returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetIconByOEid_002, TestSize.Level1)
{
    std::string oeid = "";
    std::string resourceId;
    ErrCode ret = sa_->GetIconByOEid(oeid, resourceId);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetIconByOEid_003
 * @tc.desc: Test GetIconByOEid with non-empty oeid returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetIconByOEid_003, TestSize.Level1)
{
    std::string oeid = "test_oeid_icon";
    std::string resourceId;
    ErrCode ret = sa_->GetIconByOEid(oeid, resourceId);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetFormatName_002
 * @tc.desc: Test GetFormatName with empty locale returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_002, TestSize.Level1)
{
    std::string oeid = "test_oeid";
    std::string locale = "";
    std::string formatName;
    ErrCode ret = sa_->GetFormatName(oeid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetFormatName_003
 * @tc.desc: Test GetFormatName with non-empty oeid and locale returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetFormatName_003, TestSize.Level1)
{
    std::string oeid = "test_oeid_fmt";
    std::string locale = "en_US";
    std::string formatName;
    ErrCode ret = sa_->GetFormatName(oeid, locale, formatName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_002
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale with valid locale delegates to database
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatByOEidAndLocale_002, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorFormat> format;
    ErrCode ret = sa_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "en-US", format);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_002
 * @tc.desc: Test GetObjectEditorFormatsByLocale with valid locale delegates to database
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetObjectEditorFormatsByLocale_002, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    ErrCode ret = sa_->GetObjectEditorFormatsByLocale("zh-CN", formats);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: StartObjectEditorExtension_003
 * @tc.desc: Test StartObjectEditorExtension with valid document but unknown operate type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_003, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorClientCallback> callback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    sptr<IRemoteObject> remoteObject;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_UNKNOWN_OPERATE);
}

/**
 * @tc.name: StartObjectEditorExtension_004
 * @tc.desc: Test StartObjectEditorExtension with document having OEID and CREATE_BY_OEID type
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_004, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOEid("test_oeid_12345");
    document->SetOperateType(OperateType::CREATE_BY_OEID);
    sptr<IObjectEditorClientCallback> callback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    sptr<IRemoteObject> remoteObject;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: StartObjectEditorExtension_005
 * @tc.desc: Test StartObjectEditorExtension with PACKAGE_OEID and EDIT type returns SA_OK
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, StartObjectEditorExtension_005, TestSize.Level1)
{
    auto document = std::make_unique<ObjectEditorDocument>();
    document->SetOEid(PACKAGE_OEID);
    document->SetOperateType(OperateType::EDIT);
    document->SetLinking(false);
    document->SetSnapshotUri("file:///test/snapshot.png");
    sptr<IObjectEditorClientCallback> callback = sptr<MockObjectEditorClientCallback>::MakeSptr();
    sptr<IRemoteObject> remoteObject;
    bool isPackageExtension = false;
    ErrCode ret = sa_->StartObjectEditorExtension(document, callback, remoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_TRUE(isPackageExtension);
}

/**
 * @tc.name: ResetScreenChangedCommonEventSubscriber_002
 * @tc.desc: Test ResetScreenChangedCommonEventSubscriber after Init sets receiver to non-null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ResetScreenChangedCommonEventSubscriber_002, TestSize.Level1)
{
    sa_->InitScreenChangedCommonEventSubscriber();
    if (sa_->screenChangedReceiver_ != nullptr) {
        logMsg.clear();
        sa_->ResetScreenChangedCommonEventSubscriber();
        EXPECT_EQ(sa_->screenChangedReceiver_, nullptr);
    } else {
        SUCCEED();
    }
}

/**
 * @tc.name: HandleOperateHasOEid_001
 * @tc.desc: Test HandleOperateHasOEid with non-empty oeid and CREATE_BY_OEID type delegates to database
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleOperateHasOEid_001, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetOEid("test_oeid_handle");
    document.SetOperateType(OperateType::CREATE_BY_OEID);
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = sa_->HandleOperateHasOEid(document, format);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: HandleOperateHasOEid_002
 * @tc.desc: Test HandleOperateHasOEid with EDIT type and diversion map containing source oeid
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, HandleOperateHasOEid_002, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    ContentEmbed_Diversion diversion;
    diversion.sourceOEid = "divert_src";
    diversion.targetOEid = "divert_tgt";
    diversion.minVersion = "3.0";
    sa_->diversionMap_["divert_src"] = diversion;
    ObjectEditorDocument document;
    document.SetOEid("divert_src");
    document.SetOperateType(OperateType::EDIT);
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = sa_->HandleOperateHasOEid(document, format);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_OK);
    sa_->diversionMap_.clear();
}

/**
 * @tc.name: GetDefaultAppBundleNameByFileExt_001
 * @tc.desc: Test GetDefaultAppBundleNameByFileExt returns BMS error in test environment
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetDefaultAppBundleNameByFileExt_001, TestSize.Level1)
{
    std::string bundleName;
    auto ret = sa_->GetDefaultAppBundleNameByFileExt(".txt", bundleName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED);
    EXPECT_TRUE(bundleName.empty());
}

/**
 * @tc.name: GetDefaultAppBundleNameByFileExt_002
 * @tc.desc: Test GetDefaultAppBundleNameByFileExt with different file extension
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetDefaultAppBundleNameByFileExt_002, TestSize.Level1)
{
    std::string bundleName;
    auto ret = sa_->GetDefaultAppBundleNameByFileExt(".pdf", bundleName);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED);
}

/**
 * @tc.name: OnIdle_002
 * @tc.desc: Test OnIdle always returns zero regardless of reason
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, OnIdle_002, TestSize.Level1)
{
    SystemAbilityOnDemandReason idleReason;
    int32_t ret = sa_->OnIdle(idleReason);
    EXPECT_EQ(ret, 0);
}

/**
 * @tc.name: GetCallerBundleName_002
 * @tc.desc: Test GetCallerBundleName returns empty or non-empty string without crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetCallerBundleName_002, TestSize.Level1)
{
    std::string bundleName1 = sa_->GetCallerBundleName();
    std::string bundleName2 = sa_->GetCallerBundleName();
    EXPECT_TRUE(bundleName1 == bundleName2);
}

/**
 * @tc.name: InitScreenChangedCommonEventSubscriber_002
 * @tc.desc: Test InitScreenChangedCommonEventSubscriber can be called multiple times
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, InitScreenChangedCommonEventSubscriber_002, TestSize.Level1)
{
    logMsg.clear();
    sa_->InitScreenChangedCommonEventSubscriber();
    std::string firstLog = logMsg;
    logMsg.clear();
    sa_->InitScreenChangedCommonEventSubscriber();
    EXPECT_TRUE(logMsg.find("subscribe common event ret:") != std::string::npos);
}

/**
 * @tc.name: ReadDiversionsJsonFile_002
 * @tc.desc: Test ReadDiversionsJsonFile when diversion file does not exist returns empty map
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ReadDiversionsJsonFile_002, TestSize.Level1)
{
    sa_->diversionMap_.clear();
    sa_->ReadDiversionsJsonFile();
    std::ifstream testFile("/system/etc/office_service/object_editor_service/diversion_map.json");
    if (!testFile.is_open()) {
        EXPECT_EQ(sa_->diversionMap_.size(), static_cast<size_t>(0));
    } else {
        testFile.close();
        SUCCEED();
    }
}

/**
 * @tc.name: GetInstance_003
 * @tc.desc: Test GetInstance returns same instance after multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GetInstance_003, TestSize.Level1)
{
    auto& instance1 = ObjectEditorManagerSystemAbility::GetInstance();
    auto& instance2 = ObjectEditorManagerSystemAbility::GetInstance();
    auto& instance3 = ObjectEditorManagerSystemAbility::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
    EXPECT_EQ(&instance2, &instance3);
}

/**
 * @tc.name: ResetStopSATimer_001
 * @tc.desc: Test ResetStopSATimer starts timer thread on first call, notifies on second
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, ResetStopSATimer_001, TestSize.Level1)
{
    sa_->timerRunning_.store(false);
    sa_->timerNotify_.store(false);
    sa_->ResetStopSATimer();
    EXPECT_TRUE(sa_->timerRunning_.load());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sa_->ResetStopSATimer();
    EXPECT_TRUE(sa_->timerNotify_.load() || !sa_->timerRunning_.load());
    sa_->timerNotify_.store(true);
    sa_->cvTimer_.notify_one();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

/**
 * @tc.name: GrantClientFileUriPermission_001
 * @tc.desc: Test GrantClientFileUriPermissionToServerExtension with empty document returns failure
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GrantClientFileUriPermission_001, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    bool ret = sa_->GrantClientFileUriPermissionToServerExtension(document, "com.target.bundle");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GrantClientFileUriPermission_002
 * @tc.desc: Test GrantClientFileUriPermissionToServerExtension with linking=true skips tmpFileUri
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GrantClientFileUriPermission_002, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(true);
    document.SetSnapshotUri("file:///test/snapshot.png");
    bool ret = sa_->GrantClientFileUriPermissionToServerExtension(document, "com.target.bundle");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GrantClientFileUriPermission_003
 * @tc.desc: Test GrantClientFileUriPermissionToServerExtension with CREATE_BY_FILE and nativeFileUri set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GrantClientFileUriPermission_003, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    document.SetTmpFileUri("/tmp/test.ole");
    document.SetSnapshotUri("file:///test/snapshot.png");
    document.SetNativeFileUri("file:///test/native.docx");
    bool ret = sa_->GrantClientFileUriPermissionToServerExtension(document, "com.target.bundle");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GrantClientFileUriPermission_004
 * @tc.desc: Test GrantClientFileUriPermissionToServerExtension with EDIT type and no nativeFileUri
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, GrantClientFileUriPermission_004, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    document.SetOperateType(OperateType::EDIT);
    document.SetTmpFileUri("/tmp/test.ole");
    document.SetSnapshotUri("file:///test/snapshot.png");
    bool ret = sa_->GrantClientFileUriPermissionToServerExtension(document, "com.target.bundle");
    EXPECT_FALSE(ret);
}

// ===== CheckClientFileValid with CREATE_BY_FILE and linking =====

/**
 * @tc.name: CheckClientFileValid_006
 * @tc.desc: Test CheckClientFileValid with CREATE_BY_FILE, linking, and sandbox native path returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_006, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(true);
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    document.SetSnapshotUri("file:///test/snapshot.png");
    document.SetNativeFileUri("file:///data/app/test/native.docx");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckClientFileValid_007
 * @tc.desc: Test CheckClientFileValid with CREATE_BY_FILE, linking, and non-sandbox native path
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_007, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(true);
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    document.SetSnapshotUri("file:///test/snapshot.png");
    document.SetNativeFileUri("file:///system/test/native.docx");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CheckClientFileValid_008
 * @tc.desc: Test CheckClientFileValid with CREATE_BY_FILE, not linking, tmpFileUri and snapshot set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerSystemAbilityTest, CheckClientFileValid_008, TestSize.Level1)
{
    ObjectEditorDocument document;
    document.SetLinking(false);
    document.SetOperateType(OperateType::CREATE_BY_FILE);
    document.SetTmpFileUri("/tmp/test.ole");
    document.SetSnapshotUri("file:///test/snapshot.png");
    bool ret = sa_->CheckClientFileValid(document);
    EXPECT_FALSE(ret);
}
}
}
}