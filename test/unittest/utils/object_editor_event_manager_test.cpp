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
#include "object_editor_event_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorEventManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorEventManagerTest::SetUpTestCase()
{
}

void ObjectEditorEventManagerTest::TearDownTestCase()
{
}

void ObjectEditorEventManagerTest::SetUp()
{
}

void ObjectEditorEventManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name GetInstance_001
 * @tc.desc Test GetInstance method returns valid instance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, GetInstance_001, TestSize.Level1)
{
    auto& instance = ObjectEditorEventManager::GetInstance();
    EXPECT_NE(&instance, nullptr);
}

/**
 * @tc.name GetInstance_002
 * @tc.desc Test GetInstance method returns same instance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, GetInstance_002, TestSize.Level1)
{
    auto& instance1 = ObjectEditorEventManager::GetInstance();
    auto& instance2 = ObjectEditorEventManager::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name OnScreenLocked_001
 * @tc.desc Test OnScreenLocked method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, OnScreenLocked_001, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnScreenLocked_002
 * @tc.desc Test OnScreenLocked method multiple times
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, OnScreenLocked_002, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnScreenUnlocked_001
 * @tc.desc Test OnScreenUnlocked method
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, OnScreenUnlocked_001, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name OnScreenUnlocked_002
 * @tc.desc Test OnScreenUnlocked method multiple times
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, OnScreenUnlocked_002, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name CheckIsScreenLocked_001
 * @tc.desc Test CheckIsScreenLocked method default state
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, CheckIsScreenLocked_001, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    bool isLocked = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
    EXPECT_EQ(isLocked, false);
}

/**
 * @tc.name CheckIsScreenLocked_002
 * @tc.desc Test CheckIsScreenLocked method after locked
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, CheckIsScreenLocked_002, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    bool isLocked = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
    EXPECT_EQ(isLocked, true);
}

/**
 * @tc.name CheckIsScreenLocked_003
 * @tc.desc Test CheckIsScreenLocked method after unlocked
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, CheckIsScreenLocked_003, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    bool isLocked = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
    EXPECT_EQ(isLocked, false);
}

/**
 * @tc.name ScreenLockUnlocked_001
 * @tc.desc Test screen lock/unlock sequence
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, ScreenLockUnlocked_001, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    EXPECT_TRUE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name ScreenLockUnlocked_002
 * @tc.desc Test screen lock/unlock repeated sequence
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, ScreenLockUnlocked_002, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    EXPECT_FALSE(ObjectEditorEventManager::GetInstance().CheckIsScreenLocked());
}

/**
 * @tc.name CheckIsScreenLockedConcurrent_001
 * @tc.desc Test CheckIsScreenLocked method concurrent access
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorEventManagerTest, CheckIsScreenLockedConcurrent_001, TestSize.Level1)
{
    ObjectEditorEventManager::GetInstance().OnScreenLocked();
    bool result1 = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
    bool result2 = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
    EXPECT_EQ(result1, true);
    EXPECT_EQ(result2, true);
    EXPECT_EQ(result1, result2);
}
}
}
}