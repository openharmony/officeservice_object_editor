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
#include "user_mgr.h"
#include "hilog_object_editor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class UserMgrTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UserMgrTest::SetUpTestCase()
{
}

void UserMgrTest::TearDownTestCase()
{
}

void UserMgrTest::SetUp()
{
}

void UserMgrTest::TearDown()
{
}

namespace {

/**
 * @tc.name UserMgr_001
 * @tc.desc Test UserMgr method
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_001, TestSize.Level1)
{
    UserMgr &instance1 = UserMgr::GetInstance();
    UserMgr &instance2 = UserMgr::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name UserMgr_002
 * @tc.desc Test GetUserId method
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_002, TestSize.Level1)
{
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_GE(userId, 100);
}

/**
 * @tc.name UserMgr_003
 * @tc.desc Test SetNewUserId method
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_003, TestSize.Level1)
{
    int32_t newUserId = 200;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_NE(userId, 200);
}

/**
 * @tc.name UserMgr_004
 * @tc.desc Test SetNewUserId method with small value
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_004, TestSize.Level1)
{
    int32_t newUserId = 50;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_NE(userId, 50);
}

/**
 * @tc.name UserMgr_005
 * @tc.desc Test SetNewUserId method with default value
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_005, TestSize.Level1)
{
    int32_t newUserId = 100;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_EQ(userId, 100);
}

/**
 * @tc.name UserMgr_006
 * @tc.desc Test GetUserId multiple calls
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_006, TestSize.Level1)
{
    int32_t originalUserId = UserMgr::GetInstance().GetUserId();
    UserMgr::GetInstance().SetNewUserId(99999);
    int32_t userIdAfterSet = UserMgr::GetInstance().GetUserId();
    EXPECT_EQ(originalUserId, userIdAfterSet);
}

/**
 * @tc.name UserMgr_007
 * @tc.desc Test SetNewUserId with large value
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_007, TestSize.Level1)
{
    int32_t newUserId = 100000;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_NE(userId, 100000);
}

/**
 * @tc.name UserMgr_008
 * @tc.desc Test SetNewUserId with zero value
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_008, TestSize.Level1)
{
    int32_t newUserId = 0;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_NE(userId, 0);
}

/**
 * @tc.name UserMgr_009
 * @tc.desc Test SetNewUserId with negative value
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_009, TestSize.Level1)
{
    int32_t newUserId = -1;
    UserMgr::GetInstance().SetNewUserId(newUserId);
    int32_t userId = UserMgr::GetInstance().GetUserId();
    EXPECT_NE(userId, -1);
}

/**
 * @tc.name UserMgr_010
 * @tc.desc Test GetCallingUserId method outside IPC context
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_010, TestSize.Level1)
{
    int32_t userId = UserMgr::GetInstance().GetCallingUserId();
    EXPECT_NE(userId, -1);
}

}
}
}