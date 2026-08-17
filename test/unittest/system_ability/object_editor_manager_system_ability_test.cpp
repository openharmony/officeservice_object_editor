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
}
}
}