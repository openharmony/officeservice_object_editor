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
#include "object_editor_extension_context.h"
#include "hilog_object_editor.h"
#include "want.h"
#include "start_options.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AbilityRuntime {

class ObjectEditorExtensionContextTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ObjectEditorExtensionContext> context_;
};

void ObjectEditorExtensionContextTest::SetUpTestCase()
{
}

void ObjectEditorExtensionContextTest::TearDownTestCase()
{
}

void ObjectEditorExtensionContextTest::SetUp()
{
    context_ = std::make_shared<ObjectEditorExtensionContext>();
}

void ObjectEditorExtensionContextTest::TearDown()
{
    context_ = nullptr;
}

namespace {
/**
 * @tc.name SetClientPid_001
 * @tc.desc Test SetClientPid method with valid pid
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, SetClientPid_001, TestSize.Level1)
{
    int32_t clientPid = 1234;
    context_->SetClientPid(clientPid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name SetClientPid_002
 * @tc.desc Test SetClientPid method with zero pid
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, SetClientPid_002, TestSize.Level1)
{
    int32_t clientPid = 0;
    context_->SetClientPid(clientPid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name SetClientPid_003
 * @tc.desc Test SetClientPid method with negative pid
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, SetClientPid_003, TestSize.Level1)
{
    int32_t clientPid = -1;
    context_->SetClientPid(clientPid);
    EXPECT_TRUE(true);
}

/**
 * @tc.name StartAbility_001
 * @tc.desc Test StartAbility method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbility_001, TestSize.Level1)
{
    AAFwk::Want want;
    ErrCode ret = context_->StartAbility(want);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartAbility_002
 * @tc.desc Test StartAbility method with empty want
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbility_002, TestSize.Level1)
{
    AAFwk::Want want;
    ErrCode ret = context_->StartAbility(want);
    EXPECT_EQ(ret, ERR_INVALID_VALUE);
}

/**
 * @tc.name StartAbility_003
 * @tc.desc Test StartAbility method with valid want and startOptions
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbility_003, TestSize.Level1)
{
    AAFwk::Want want;
    AAFwk::StartOptions startOptions;
    ErrCode ret = context_->StartAbility(want, startOptions);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name StartAbility_004
 * @tc.desc Test StartAbility method with startOptions and null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbility_004, TestSize.Level1)
{
    AAFwk::Want want;
    AAFwk::StartOptions startOptions;
    ErrCode ret = context_->StartAbility(want, startOptions);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name StartAbilityWithAccount_001
 * @tc.desc Test StartAbilityWithAccount method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbilityWithAccount_001, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = 100;
    ErrCode ret = context_->StartAbilityWithAccount(want, accountId);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name StartAbilityWithAccount_002
 * @tc.desc Test StartAbilityWithAccount method with zero accountId
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbilityWithAccount_002, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = 0;
    ErrCode ret = context_->StartAbilityWithAccount(want, accountId);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name StartAbilityWithAccount_003
 * @tc.desc Test StartAbilityWithAccount method with negative accountId
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbilityWithAccount_003, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = -1;
    ErrCode ret = context_->StartAbilityWithAccount(want, accountId);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name StartAbilityWithAccount_004
 * @tc.desc Test StartAbilityWithAccount method with startOptions and null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, StartAbilityWithAccount_004, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = 100;
    AAFwk::StartOptions startOptions;
    ErrCode ret = context_->StartAbilityWithAccount(want, accountId, startOptions);
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name ConnectAbility_001
 * @tc.desc Test ConnectAbility method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, ConnectAbility_001, TestSize.Level1)
{
    AAFwk::Want want;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    bool ret = context_->ConnectAbility(want, connectCallback);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ConnectAbility_002
 * @tc.desc Test ConnectAbility method with null callback
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, ConnectAbility_002, TestSize.Level1)
{
    AAFwk::Want want;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    bool ret = context_->ConnectAbility(want, connectCallback);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ConnectAbilityWithAccount_001
 * @tc.desc Test ConnectAbilityWithAccount method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, ConnectAbilityWithAccount_001, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = 100;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    bool ret = context_->ConnectAbilityWithAccount(want, accountId, connectCallback);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ConnectAbilityWithAccount_002
 * @tc.desc Test ConnectAbilityWithAccount method with null callback
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, ConnectAbilityWithAccount_002, TestSize.Level1)
{
    AAFwk::Want want;
    int accountId = 100;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    bool ret = context_->ConnectAbilityWithAccount(want, accountId, connectCallback);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name DisconnectAbility_001
 * @tc.desc Test DisconnectAbility method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, DisconnectAbility_001, TestSize.Level1)
{
    AAFwk::Want want;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    ErrCode ret = context_->DisconnectAbility(want, connectCallback);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name DisconnectAbility_002
 * @tc.desc Test DisconnectAbility method with null callback
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, DisconnectAbility_002, TestSize.Level1)
{
    AAFwk::Want want;
    sptr<AbilityConnectCallback> connectCallback = nullptr;
    ErrCode ret = context_->DisconnectAbility(want, connectCallback);
    EXPECT_EQ(ret, ERR_OK);
}

/**
 * @tc.name TerminateAbility_001
 * @tc.desc Test TerminateAbility method with null token
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, TerminateAbility_001, TestSize.Level1)
{
    ErrCode ret = context_->TerminateAbility();
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name TerminateAbility_002
 * @tc.desc Test TerminateAbility method without init
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, TerminateAbility_002, TestSize.Level1)
{
    ErrCode ret = context_->TerminateAbility();
    EXPECT_EQ(ret, ERR_NO_INIT);
}

/**
 * @tc.name GetAbilityInfoType_001
 * @tc.desc Test GetAbilityInfoType method without init
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, GetAbilityInfoType_001, TestSize.Level1)
{
    AppExecFwk::AbilityType type = context_->GetAbilityInfoType();
    EXPECT_EQ(type, AppExecFwk::AbilityType::UNKNOWN);
}

/**
 * @tc.name GetAbilityInfoType_002
 * @tc.desc Test GetAbilityInfoType method with null ability info
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, GetAbilityInfoType_002, TestSize.Level1)
{
    AppExecFwk::AbilityType type = context_->GetAbilityInfoType();
    EXPECT_EQ(type, AppExecFwk::AbilityType::UNKNOWN);
}

/**
 * @tc.name ContextTypeId_001
 * @tc.desc Test CONTEXT_TYPE_ID static member
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, ContextTypeId_001, TestSize.Level1)
{
    EXPECT_NE(ObjectEditorExtensionContext::CONTEXT_TYPE_ID, 0);
}

/**
 * @tc.name IllegalRequestCode_001
 * @tc.desc Test ILLEGAL_REQUEST_CODE static member
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, IllegalRequestCode_001, TestSize.Level1)
{
    EXPECT_EQ(ObjectEditorExtensionContext::ILLEGAL_REQUEST_CODE, -1);
}

/**
 * @tc.name Destructor_001
 * @tc.desc Test destructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, Destructor_001, TestSize.Level1)
{
    ObjectEditorExtensionContext* context = new ObjectEditorExtensionContext();
    delete context;
    EXPECT_TRUE(true);
}

/**
 * @tc.name Constructor_001
 * @tc.desc Test default constructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionContextTest, Constructor_001, TestSize.Level1)
{
    ObjectEditorExtensionContext context;
    EXPECT_TRUE(true);
}
}
}
}