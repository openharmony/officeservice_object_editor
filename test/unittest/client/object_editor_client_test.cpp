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
#include "mock_object_editor_client.h"
#include "mock_object_editor_client_callback.h"
#include "mock_object_editor_manager.h"
#include "mock_object_editor_service.h"
#include "stub.h"
#include "iservice_registry.h"
#include "object_editor_manager_proxy.h"
#include "application_context.h"
#include "mock_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorClientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<ObjectEditorClient> client_;
    std::unique_ptr<ObjectEditorLoadCallback> oeCallback_;
    std::unique_ptr<ObjectEditorAbilityListener> listener_;
};

void ObjectEditorClientTest::SetUpTestCase()
{
}

void ObjectEditorClientTest::TearDownTestCase()
{
}

void ObjectEditorClientTest::SetUp()
{
    client_ = std::make_unique<ObjectEditorClient>();
    oeCallback_ = std::make_unique<ObjectEditorLoadCallback>();
    listener_ = std::make_unique<ObjectEditorAbilityListener>();
}

void ObjectEditorClientTest::TearDown()
{
    client_ = nullptr;
    oeCallback_ = nullptr;
    listener_ = nullptr;
}


namespace {

sptr<IObjectEditorManager> GetIObjectEditorManager_stub()
{
    return nullptr;
}

sptr<MockObjectEditorManager> GetMockProxy()
{
    static sptr<MockObjectEditorManager> mockSAProxy = sptr<MockObjectEditorManager>::MakeSptr();
    return mockSAProxy;
}

sptr<MockObjectEditorManager> MockGetIObjectEditorManager()
{
    static sptr<IObjectEditorManager> saProxy = GetMockProxy();
    return saProxy;
}

ErrCode MockPrepareFiles()
{
    return ObjectEditorClientErrCode::CLIENT_OK;
}

ErrCode MockPrepareFilesFail()
{
    return ObjectEditorClientErrCode::CLIENT_UNKNOW_OPERATE;
}

bool MockWaitLoadStateChange()
{
    return false;
}

sptr<ISystemAbilityManager> MockGetSystemAbilityManager()
{
    return nullptr;
}

std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> MockGetApplicationContext()
{
    return std::make_shared<OHOS::AbilityRuntime::ApplicationContext>();
}

/**
 * @tc.name SubscribeSystemAbility_001
 * @tc.desc 测试SubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, SubscribeSystemAbility_001, TestSize.Level1)
{
    client_->saStatusListener_ = sptr<MockISystemAbilityStatusChange>::MakeSptr();
    client_->SubscribeSystemAbility();
    EXPECT_NE(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name SubscribeSystemAbility_002
 * @tc.desc 测试SubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, SubscribeSystemAbility_002, TestSize.Level1)
{
    client_->saStatusListener_ = sptr<MockISystemAbilityStatusChange>::MakeSptr();
    Stub stub;
    stub.set(ADDR(SystemAbilityManagerClient, GetSystemAbilityManager), MockGetSystemAbilityManager);
    client_->SubscribeSystemAbility();
    EXPECT_NE(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name SubscribeSystemAbility_003
 * @tc.desc 测试SubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, SubscribeSystemAbility_003, TestSize.Level1)
{
    client_->SubscribeSystemAbility();
    EXPECT_NE(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name UnsubscribeSystemAbility_001
 * @tc.desc 测试UnsubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, UnsubscribeSystemAbility_001, TestSize.Level1)
{
    client_->saStatusListener_ = nullptr;
    client_->UnsubscribeSystemAbility();
    EXPECT_EQ(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name UnsubscribeSystemAbility_002
 * @tc.desc 测试UnsubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, UnsubscribeSystemAbility_002, TestSize.Level1)
{
    client_->saStatusListener_ = sptr<MockISystemAbilityStatusChange>::MakeSptr();
    client_->UnsubscribeSystemAbility();
    EXPECT_NE(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name UnsubscribeSystemAbility_003
 * @tc.desc 测试UnsubscribeSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, UnsubscribeSystemAbility_003, TestSize.Level1)
{
    client_->saStatusListener_ = sptr<MockISystemAbilityStatusChange>::MakeSptr();
    Stub stub;
    stub.set(ADDR(SystemAbilityManagerClient, GetSystemAbilityManager), MockGetSystemAbilityManager);
    client_->UnsubscribeSystemAbility();
    EXPECT_NE(client_->saStatusListener_, nullptr);
}

/**
 * @tc.name OnAddSystemAbility_001
 * @tc.desc 测试OnAddSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnAddSystemAbility_001, TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    const std::string deviceId;
    listener_->OnAddSystemAbility(systemAbilityId, deviceId);
}

/**
 * @tc.name OnAddSystemAbility_002
 * @tc.desc 测试OnAddSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnAddSystemAbility_002, TestSize.Level1)
{
    int32_t systemAbilityId = OBJECT_EDITOR_SERVICE_SA_ID;
    const std::string deviceId;
    listener_->OnAddSystemAbility(systemAbilityId, deviceId);
}

/**
 * @tc.name OnRemoveSystemAbility_001
 * @tc.desc 测试OnRemoveSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnRemoveSystemAbility_001, TestSize.Level1)
{
    int32_t systemAbilityId = 0;
    const std::string deviceId;
    listener_->OnRemoveSystemAbility(systemAbilityId, deviceId);
}

/**
 * @tc.name OnRemoveSystemAbility_002
 * @tc.desc 测试OnRemoveSystemAbility方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnRemoveSystemAbility_002, TestSize.Level1)
{
    int32_t systemAbilityId = OBJECT_EDITOR_SERVICE_SA_ID;
    const std::string deviceId;
    listener_->OnRemoveSystemAbility(systemAbilityId, deviceId);
}

/**
 * @tc.name GetObjectEditorProxy_001
 * @tc.desc 测试GetObjectEditorProxy方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorProxy_001, TestSize.Level1)
{
    const sptr<MockIRemoteObject> object;
    client_->oeSAProxy_ = sptr<MockObjectEditorManager>::MakeSptr();
    auto res = client_->GetObjectEditorProxy(object);
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetObjectEditorProxy_002
 * @tc.desc 测试GetObjectEditorProxy方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorProxy_002, TestSize.Level1)
{
    const sptr<MockIRemoteObject> object = nullptr;
    client_->oeSAProxy_ = nullptr;
    auto res = client_->GetObjectEditorProxy(object);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name InitLoadState_001
 * @tc.desc 测试InitLoadState方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, InitLoadState_001, TestSize.Level1)
{
    client_->InitLoadState();
    EXPECT_EQ(client_->loadState_, false);
}

/**
 * @tc.name WaitLoadStateChange_001
 * @tc.desc 测试WaitLoadStateChange方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, WaitLoadStateChange_001, TestSize.Level1)
{
    auto res = client_->WaitLoadStateChange();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name OnLoadSystemAbilitySuccess_001
 * @tc.desc 测试OnLoadSystemAbilitySuccess方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnLoadSystemAbilitySuccess_001, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallBack);
    int32_t systemAbilityId = 0;
    const sptr<IRemoteObject> object = nullptr;
    oeCallback_->OnLoadSystemAbilitySuccess(systemAbilityId, object);
    EXPECT_TRUE(logMsg.find("sa id invalid") != std::string::npos);
}

/**
 * @tc.name OnLoadSystemAbilitySuccess_002
 * @tc.desc 测试OnLoadSystemAbilitySuccess方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnLoadSystemAbilitySuccess_002, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallBack);
    int32_t systemAbilityId = OBJECT_EDITOR_SERVICE_SA_ID;
    const sptr<IRemoteObject> object = nullptr;
    oeCallback_->OnLoadSystemAbilitySuccess(systemAbilityId, object);
    EXPECT_TRUE(logMsg.find("object is null.") != std::string::npos);
}

/**
 * @tc.name OnLoadSystemAbilityFail_001
 * @tc.desc 测试OnLoadSystemAbilityFail方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnLoadSystemAbilityFail_001, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallBack);
    int32_t systemAbilityId = 0;
    oeCallback_->OnLoadSystemAbilityFail(systemAbilityId);
    EXPECT_TRUE(logMsg.find("sa id invalid") != std::string::npos);
}

/**
 * @tc.name OnLoadSystemAbilityFail_002
 * @tc.desc 测试OnLoadSystemAbilityFail方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnLoadSystemAbilityFail_002, TestSize.Level1)
{
    logMsg.clear();
    LOG_SetCallback(MyLogCallBack);
    int32_t systemAbilityId = OBJECT_EDITOR_SERVICE_SA_ID;
    oeCallback_->OnLoadSystemAbilityFail(systemAbilityId);
}

/**
 * @tc.name LoadSystemAbilitySuccess_001
 * @tc.desc 测试LoadSystemAbilitySuccess方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, LoadSystemAbilitySuccess_001, TestSize.Level1)
{
    const sptr<IRemoteObject> object = nullptr;
    client_->LoadSystemAbilitySuccess(object);
    EXPECT_EQ(client_->loadState_, true);
}

/**
 * @tc.name LoadSystemAbilityFail_001
 * @tc.desc 测试LoadSystemAbilityFail方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, LoadSystemAbilityFail_001, TestSize.Level1)
{
    client_->LoadSystemAbilityFail();
    EXPECT_EQ(client_->loadState_, false);
}

/**
 * @tc.name OnRemoteDied_001
 * @tc.desc 测试OnRemoteDied方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, OnRemoteDied_001, TestSize.Level1)
{
    const wptr<MockIRemoteObject> object = nullptr;
    client_->OnRemoteDied(object);
}

/**
 * @tc.name GetIObjectEditorManager_001
 * @tc.desc 测试GetIObjectEditorManager方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIObjectEditorManager_001, TestSize.Level1)
{
    client_->oeSAProxy_ = sptr<MockObjectEditorManager>::MakeSptr();
    auto res = client_->GetIObjectEditorManager();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetIObjectEditorManager_002
 * @tc.desc 测试GetIObjectEditorManager方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIObjectEditorManager_002, TestSize.Level1)
{
    client_->oeSAProxy_ = nullptr;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, WaitLoadStateChange), MockWaitLoadStateChange);
    auto res = client_->GetIObjectEditorManager();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetIObjectEditorManager_003
 * @tc.desc 测试GetIObjectEditorManager方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIObjectEditorManager_003, TestSize.Level1)
{
    client_->oeSAProxy_ = nullptr;
    auto res = client_->GetIObjectEditorManager();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetIObjectEditorManager_004
 * @tc.desc 测试GetIObjectEditorManager方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIObjectEditorManager_001, TestSize.Level1)
{
    client_->oeSAProxy_ = nullptr;
    Stub stub;
    stub.set(ADDR(SystemAbilityManagerClient, GetSystemAbilityManager), MockGetSystemAbilityManager);
    auto res = client_->GetIObjectEditorManager();
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name StartObjectEditorExtension_001
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_001, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::CLIENT_UNKNOW_OPERATE);
}

/**
 * @tc.name StartObjectEditorExtension_002
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_002, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    stub.set(ADDR(ObjectEditorClient, PrepareFiles), MockPrepareFiles);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::CLIENT_UNKNOW_OPERATE);
}

/**
 * @tc.name StartObjectEditorExtension_003
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_003, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    document->operateType = OperateType::CREATE_BY_FILE;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    stub.set(ADDR(ObjectEditorClient, PrepareFiles), MockPrepareFiles);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_004
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_004, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    document->operateType = OperateType::CREATE_BY_FILE;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, PrepareFiles), MockPrepareFiles);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::ERR_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_005
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_005, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    document->operateType = OperateType::CREATE_BY_FILE;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, PrepareFiles), MockPrepareFiles);
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, StartObjectEditorExtension(_, _, _, _))
        .WillOnce(Return(ObjectEditorClientErrCode::SA_INVALID_VALUE))
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::SA_INVALID_VALUE);
}

/**
 * @tc.name StartObjectEditorExtension_006
 * @tc.desc 测试StartObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StartObjectEditorExtension_006, TestSize.Level1)
{
    const sptr<IObjectEditorClientCallback> objectEditorCallback =
        sptr<MockObjectEditorClientCallback>::MakeSptr();
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    document->operateType = OperateType::CREATE_BY_FILE;
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, PrepareFiles), MockPrepareFiles);
    ErrCode ret = client_->StartObjectEditorExtension(
        objectEditorCallback, document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::ERR_OK);
}

/**
 * @tc.name GenRandomUuid_001
 * @tc.desc 测试GenRandomUuid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GenRandomUuid_001, TestSize.Level1)
{
    string ret = client_->GenRandomUuid();
    EXPECT_NE(ret, "");
}

/**
 * @tc.name StopObjectEditorExtension_001
 * @tc.desc 测试StopObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StopObjectEditorExtension_001, TestSize.Level1)
{
    sptr<IObjectEditorService> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    std::unique_ptr<ObjectEditorDocument> document;
    ErrCode ret = client_->StopObjectEditorExtension(
        document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StopObjectEditorExtension_002
 * @tc.desc 测试StopObjectEditorExtension方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, StopObjectEditorExtension_002, TestSize.Level1)
{
    sptr<IObjectEditorService> oeExtensionRemoteObject = sptr<MockObjectEditorService>::MakeSptr();
    bool isPackageExtension = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    std::unique_ptr<ObjectEditorDocument> document;
    ErrCode ret = client_->StopObjectEditorExtension(
        document, oeExtensionRemoteObject, isPackageExtension);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetIcon_001
 * @tc.desc 测试GetIcon方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIcon_001, TestSize.Level1)
{
    std::string hmid;
    std::string resFilePatch;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    ErrCode ret = client_->GetIcon(hmid, resFilePatch);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetIcon_002
 * @tc.desc 测试GetIcon方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIcon_002, TestSize.Level1)
{
    std::string hmid;
    std::string resFilePatch;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetIconByHmid(_, _))
        .WillOnce(Return(1))
    ErrCode ret = client_->GetIcon(hmid, resFilePatch);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name GetIcon_003
 * @tc.desc 测试GetIcon方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetIcon_003, TestSize.Level1)
{
    std::string hmid;
    std::string resFilePatch;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetIconByHmid(_, _))
        .WillOnce(Return(ERR_OK))
    ErrCode ret = client_->GetIcon(hmid, resFilePatch);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name GetFormatName_001
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetFormatName_001, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetFormatName_002
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetFormatName_002, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetFormatName(_, _, _))
        .WillOnce(Return(1))
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name GetFormatName_003
 * @tc.desc 测试GetFormatName方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetFormatName_003, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::string formatName;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetFormatName(_, _, _))
        .WillOnce(Return(ERR_OK))
    ErrCode ret = client_->GetFormatName(hmid, locale, formatName);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_001
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatByHmidAndLocale_001, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_002
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatByHmidAndLocale_002, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetObjectEditorFormatByHmidAndLocale(_, _, _))
        .WillOnce(Return(1))
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name GetObjectEditorFormatByHmidAndLocale_003
 * @tc.desc 测试GetObjectEditorFormatByHmidAndLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatByHmidAndLocale_003, TestSize.Level1)
{
    std::string hmid;
    std::string locale;
    std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetObjectEditorFormatByHmidAndLocale(_, _, _))
        .WillOnce(Return(ERR_OK))
    ErrCode ret = client_->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_001
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatsByLocale_001, TestSize.Level1)
{
    std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), GetIObjectEditorManager_stub);
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_002
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatsByLocale_002, TestSize.Level1)
{
    std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetObjectEditorFormatsByLocale(_, _))
        .WillOnce(Return(1))
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, 1);
}

/**
 * @tc.name GetObjectEditorFormatsByLocale_003
 * @tc.desc 测试GetObjectEditorFormatsByLocale方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, GetObjectEditorFormatsByLocale_003, TestSize.Level1)
{
    std::string locale;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    formats.push_back(std::make_unique<ObjectEditorFormat>());
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, GetIObjectEditorManager), MockGetIObjectEditorManager);
    sptr<MockObjectEditorManager> saProxy = GetMockProxy();
    EXPECT_CALL(*saProxy, GetObjectEditorFormatsByLocale(_, _))
        .WillOnce(Return(ERR_OK))
    ErrCode ret = client_->GetObjectEditorFormatsByLocale(locale, formats);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name PrepareFiles_001
 * @tc.desc 测试PrepareFiles方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, PrepareFiles_001, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    Stub stub;
    stub.set(ADDR(AbilityRuntime::ApplicationContext, GetAbilityManager), MockGetAbilityManager);
    ErrCode ret = client_->PrepareFiles(document);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::CLIENT_GET_PATH_ERROR);
}

/**
 * @tc.name PrepareFiles_002
 * @tc.desc 测试PrepareFiles方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorClientTest, PrepareFiles_002, TestSize.Level1)
{
    std::unique_ptr<ObjectEditorDocument> document = std::make_unique<ObjectEditorDocument>();
    ErrCode ret = client_->PrepareFiles(document);
    EXPECT_EQ(ret, ObjectEditorClientErrCode::CLIENT_INVALID_PARAMETER);
}

}
}
}