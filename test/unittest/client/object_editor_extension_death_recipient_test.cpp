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
#include "object_editor_extension_death_recipient.h"
#include "mock_object_editor_service.h"
#include "mock_object_editor_client.h"
#include "mock_i_remote_object.h"
#include "mock_object_editor_manager_stub.h"
#include "object_editor_client.h"
#include "mock_hilog.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

static ContentEmbed_ErrorCode gLastError = CE_ERR_OK;

static void MockErrorFunc(ContentEmbed_ExtensionProxy *proxy, ContentEmbed_ErrorCode error)
{
    gLastError = error;
}

static ExtensionStopReason gStopReason = ExtensionStopReason::UNKNOWN;
static ErrCode gStopReasonRet = ERR_OK;

static void MockQueryExtensionStopReasonSaCleanIdle()
{
    gStopReason = ExtensionStopReason::SA_CLEAN_IDLE;
    gStopReasonRet = ERR_OK;
}

static void MockQueryExtensionStopReasonUnknown()
{
    gStopReason = ExtensionStopReason::UNKNOWN;
    gStopReasonRet = ERR_OK;
}

static void MockQueryExtensionStopReasonProxyStop()
{
    gStopReason = ExtensionStopReason::PROXY_STOP;
    gStopReasonRet = ERR_OK;
}

static void MockQueryExtensionStopReasonFail()
{
    gStopReason = ExtensionStopReason::UNKNOWN;
    gStopReasonRet = ERR_INVALID_VALUE;
}

class ObjectEditorExtensionDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<ObjectEditorExtensionDeathRecipient> recipient_;
    struct ContentEmbed_ExtensionProxy *proxy_;
};

void ObjectEditorExtensionDeathRecipientTest::SetUpTestCase()
{
}

void ObjectEditorExtensionDeathRecipientTest::TearDownTestCase()
{
}

void ObjectEditorExtensionDeathRecipientTest::SetUp()
{
    proxy_ = new ContentEmbed_ExtensionProxy();
    proxy_->objectEditorService = nullptr;
    proxy_->onErrorFunc = MockErrorFunc;
    proxy_->onUpdateFunc = nullptr;
    proxy_->onEditingFinishedFunc = nullptr;
    proxy_->onExtensionStoppedFunc = nullptr;
    proxy_->ceDocument = nullptr;
    proxy_->deathRecipient = nullptr;
    recipient_ = std::make_unique<ObjectEditorExtensionDeathRecipient>(proxy_);
    gLastError = CE_ERR_OK;
}

void ObjectEditorExtensionDeathRecipientTest::TearDown()
{
    if (proxy_ != nullptr) {
        if (proxy_->objectEditorService != nullptr) {
            proxy_->objectEditorService = nullptr;
        }
        delete proxy_;
        proxy_ = nullptr;
    }
    recipient_ = nullptr;
}

/**
 * @tc.name Constructor_001
 * @tc.desc Test constructor method with valid proxy
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, Constructor_001, TestSize.Level1)
{
    struct ContentEmbed_ExtensionProxy *localProxy = new ContentEmbed_ExtensionProxy();
    localProxy->objectEditorService = nullptr;
    localProxy->onErrorFunc = nullptr;
    auto localRecipient = std::make_unique<ObjectEditorExtensionDeathRecipient>(localProxy);
    EXPECT_NE(localRecipient, nullptr);
    delete localProxy;
}

/**
 * @tc.name Constructor_002
 * @tc.desc Test constructor method with nullptr proxy
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, Constructor_002, TestSize.Level1)
{
    auto localRecipient = std::make_unique<ObjectEditorExtensionDeathRecipient>(nullptr);
    EXPECT_NE(localRecipient, nullptr);
}

/**
 * @tc.name OnRemoteDied_001
 * @tc.desc Test OnRemoteDied method with nullptr proxy and nullptr remote
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_001, TestSize.Level1)
{
    recipient_->proxy_ = nullptr;
    const wptr<IRemoteObject> remote = nullptr;
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_OK);
}

/**
 * @tc.name OnRemoteDied_002
 * @tc.desc Test OnRemoteDied method with nullptr objectEditorService
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_002, TestSize.Level1)
{
    recipient_->proxy_ = proxy_;
    proxy_->objectEditorService = nullptr;
    proxy_->onErrorFunc = MockErrorFunc;
    const wptr<IRemoteObject> remote = nullptr;
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

/**
 * @tc.name OnRemoteDied_003
 * @tc.desc Test OnRemoteDied method with SA_CLEAN_IDLE stop reason
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_003, TestSize.Level1)
{
    recipient_->proxy_ = proxy_;
    proxy_->objectEditorService = sptr<MockObjectEditorService>::MakeSptr();
    proxy_->onErrorFunc = MockErrorFunc;
    const wptr<IRemoteObject> remote = nullptr;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, QueryExtensionStopReason), MockQueryExtensionStopReasonSaCleanIdle);
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

/**
 * @tc.name OnRemoteDied_004
 * @tc.desc Test OnRemoteDied method with UNKNOWN stop reason
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_004, TestSize.Level1)
{
    recipient_->proxy_ = proxy_;
    proxy_->objectEditorService = sptr<MockObjectEditorService>::MakeSptr();
    proxy_->onErrorFunc = MockErrorFunc;
    const wptr<IRemoteObject> remote = nullptr;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, QueryExtensionStopReason), MockQueryExtensionStopReasonUnknown);
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

/**
 * @tc.name OnRemoteDied_005
 * @tc.desc Test OnRemoteDied method with PROXY_STOP stop reason
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_005, TestSize.Level1)
{
    recipient_->proxy_ = proxy_;
    proxy_->objectEditorService = sptr<MockObjectEditorService>::MakeSptr();
    proxy_->onErrorFunc = MockErrorFunc;
    const wptr<IRemoteObject> remote = nullptr;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, QueryExtensionStopReason), MockQueryExtensionStopReasonProxyStop);
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

/**
 * @tc.name OnRemoteDied_006
 * @tc.desc Test OnRemoteDied method with failed QueryExtensionStopReason
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_006, TestSize.Level1)
{
    recipient_->proxy_ = proxy_;
    proxy_->objectEditorService = sptr<MockObjectEditorService>::MakeSptr();
    proxy_->onErrorFunc = MockErrorFunc;
    const wptr<IRemoteObject> remote = nullptr;
    Stub stub;
    stub.set(ADDR(ObjectEditorClient, QueryExtensionStopReason), MockQueryExtensionStopReasonFail);
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

/**
 * @tc.name OnRemoteDied_007
 * @tc.desc Test OnRemoteDied method with nullptr proxy and valid remote
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionDeathRecipientTest, OnRemoteDied_007, TestSize.Level1)
{
    recipient_->proxy_ = nullptr;
    const wptr<IRemoteObject> remote = sptr<MockIRemoteObject>::MakeSptr();
    recipient_->OnRemoteDied(remote);
    EXPECT_EQ(gLastError, CE_ERR_OK);
}

} // namespace ObjectEditor
} // namespace OHOS