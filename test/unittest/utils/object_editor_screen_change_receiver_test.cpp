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
#include "object_editor_screen_change_receiver.h"
#include "object_editor_event_manager.h"
#include "common_event_support.h"
#include "want.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorScreenChangeReceiverTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::shared_ptr<ObjectEditorScreenChangeReceiver> receiver_;
};

void ObjectEditorScreenChangeReceiverTest::SetUpTestCase()
{
}

void ObjectEditorScreenChangeReceiverTest::TearDownTestCase()
{
}

void ObjectEditorScreenChangeReceiverTest::SetUp()
{
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    receiver_ = std::make_shared<ObjectEditorScreenChangeReceiver>(subscribeInfo);
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
}

void ObjectEditorScreenChangeReceiverTest::TearDown()
{
    receiver_ = nullptr;
}

namespace {
/**
 * @tc.name Constructor_001
 * @tc.desc Test constructor with valid subscribe info
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, Constructor_001, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto receiver = std::make_shared<ObjectEditorScreenChangeReceiver>(subscribeInfo);
    EXPECT_NE(receiver, nullptr);
}

/**
 * @tc.name OnReceiveEvent_001
 * @tc.desc Test OnReceiveEvent with screen locked event
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_001, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED);
    EventFwk::CommonEventData eventData(want);
    receiver_->OnReceiveEvent(eventData);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_002
 * @tc.desc Test OnReceiveEvent with screen unlocked event
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_002, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED);
    EventFwk::CommonEventData eventData(want);
    receiver_->OnReceiveEvent(eventData);
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_003
 * @tc.desc Test OnReceiveEvent with empty action
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_003, TestSize.Level1)
{
    AAFwk::Want want;
    EventFwk::CommonEventData eventData(want);
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    receiver_->OnReceiveEvent(eventData);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_004
 * @tc.desc Test OnReceiveEvent with unknown action
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_004, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction("unknown.action");
    EventFwk::CommonEventData eventData(want);
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    receiver_->OnReceiveEvent(eventData);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_005
 * @tc.desc Test OnReceiveEvent sequence lock then unlock
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_005, TestSize.Level1)
{
    AAFwk::Want want1;
    want1.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED);
    EventFwk::CommonEventData eventData1(want1);
    receiver_->OnReceiveEvent(eventData1);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
    AAFwk::Want want2;
    want2.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_UNLOCKED);
    EventFwk::CommonEventData eventData2(want2);
    receiver_->OnReceiveEvent(eventData2);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_006
 * @tc.desc Test OnReceiveEvent multiple lock events
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_006, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED);
    EventFwk::CommonEventData eventData(want);
    receiver_->OnReceiveEvent(eventData);
    receiver_->OnReceiveEvent(eventData);
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnReceiveEvent_007
 * @tc.desc Test OnReceiveEvent multiple unlock events
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, OnReceiveEvent_007, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED);
    EventFwk::CommonEventData eventData(want);
    receiver_->OnReceiveEvent(eventData);
    receiver_->OnReceiveEvent(eventData);
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name Destructor_001
 * @tc.desc Test destructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorScreenChangeReceiverTest, Destructor_001, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    ObjectEditorScreenChangeReceiver* receiver = new ObjectEditorScreenChangeReceiver(subscribeInfo);
    delete receiver;
    EXPECT_TRUE(true);
}
}
}
}