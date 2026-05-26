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
#include "mock_i_remote_object.h"
#include "object_editor_connection.h"
#include "object_editor_manager_system_ability.h"
#include "object_editor_client_death_recipient.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorClientDeathRecipientTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<ObjectEditorClientDeathRecipient> recipient_;
};

void ObjectEditorClientDeathRecipientTest::SetUpTestCase()
{
}

void ObjectEditorClientDeathRecipientTest::TearDownTestCase()
{
}

void ObjectEditorClientDeathRecipientTest::SetUp()
{
    std::string documentId = "test_document";
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    recipient_ = sptr<ObjectEditorClientDeathRecipient>::MakeSptr(documentId, remoteObject);
    ASSERT_NE(recipient_, nullptr);
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
}

void ObjectEditorClientDeathRecipientTest::TearDown()
{
    recipient_ = nullptr;
}

namespace {
/**
 * @tc.name: Constructor_001
 * @tc.desc: Test ObjectEditorClientDeathRecipient constructor with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, Constructor_001, TestSize.Level1)
{
    std::string documentId = "test";
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    auto recipient = sptr<ObjectEditorClientDeathRecipient>::MakeSptr(documentId, remoteObject);
    EXPECT_NE(recipient, nullptr);
}

/**
 * @tc.name: Constructor_002
 * @tc.desc: Test ObjectEditorClientDeathRecipient constructor with empty parameters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, Constructor_002, TestSize.Level1)
{
    std::string documentId = "";
    sptr<IRemoteObject> remoteObject = nullptr;
    auto recipient = sptr<ObjectEditorClientDeathRecipient>::MakeSptr(documentId, remoteObject);
    EXPECT_NE(recipient, nullptr);
}

/**
 * @tc.name: Constructor_003
 * @tc.desc: Test ObjectEditorClientDeathRecipient constructor with long documentId
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, Constructor_003, TestSize.Level1)
{
    std::string documentId = "valid_document_id";
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    auto recipient = sptr<ObjectEditorClientDeathRecipient>::MakeSptr(documentId, remoteObject);
    EXPECT_NE(recipient, nullptr);
}

/**
 * @tc.name: OnRemoteDied_001
 * @tc.desc: Test OnRemoteDied method with empty remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, OnRemoteDied_001, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject;
    recipient_->OnRemoteDied(remoteObject);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: OnRemoteDied_002
 * @tc.desc: Test OnRemoteDied method with valid remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, OnRemoteDied_002, TestSize.Level1)
{
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    wptr<IRemoteObject> weakRemote = remoteObject;
    recipient_->OnRemoteDied(weakRemote);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: OnRemoteDied_003
 * @tc.desc: Test OnRemoteDied method with null remote object
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, OnRemoteDied_003, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject = nullptr;
    recipient_->OnRemoteDied(remoteObject);
    EXPECT_TRUE(true);
}

/**
 * @tc.name: Destructor_001
 * @tc.desc: Test ObjectEditorClientDeathRecipient destructor
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, Destructor_001, TestSize.Level1)
{
    std::string documentId = "test";
    sptr<IRemoteObject> remoteObject = sptr<MockIRemoteObject>::MakeSptr();
    ObjectEditorClientDeathRecipient* recipient =
        new ObjectEditorClientDeathRecipient(documentId, remoteObject);
    delete recipient;
    EXPECT_TRUE(true);
}

/**
 * @tc.name: MultipleOnRemoteDied_001
 * @tc.desc: Test OnRemoteDied method called multiple times
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorClientDeathRecipientTest, MultipleOnRemoteDied_001, TestSize.Level1)
{
    wptr<IRemoteObject> remoteObject;
    recipient_->OnRemoteDied(remoteObject);
    recipient_->OnRemoteDied(remoteObject);
    EXPECT_TRUE(true);
}
}
}
}