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

#include <thread>
#include <gtest/gtest.h>
#include "user_mgr.h"

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

/**
 * @tc.name UserMgr_001
 * @tc.desc 测试UserMgr方法
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_001, TestSize.Level1)
{
    UserMgr &instance1 = UserMgr::GetInstance();
    UserMgr &instance2 = UserMgr::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name GetUserId_001
 * @tc.desc 测试GetUserId方法
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, GetUserId_001, TestSize.Level1)
{
    UserMgr mgr;
    int32_t ret = mgr.GetUserId();
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name SetNewUserId_001
 * @tc.desc 测试SetNewUserId方法
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, SetNewUserId_001, TestSize.Level1)
{
    UserMgr mgr = UserMgr::GetInstance();
    int32_t userId = 1000;
    mgr.SetNewUserId(userId);
    int32_t ret = mgr.GetUserId();
    EXPECT_EQ(ret, userId);
}

/**
 * @tc.name GetUserId_002
 * @tc.desc 测试GetUserId方法
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, GetUserId_002, TestSize.Level1)
{
    UserMgr mgr = UserMgr::GetInstance();
    int32_t ret = mgr.GetUserId();
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name UserMgr_Thread_001
 * @tc.desc 测试UserMgr线程安全
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_Thread_001, TestSize.Level1)
{
    std::thread t1([]() {
        UserMgr mgr = UserMgr::GetInstance();
        EXPECT_NE(mgr.GetUserId(), -1);
    });
    std::thread t2([]() {
        UserMgr mgr = UserMgr::GetInstance();
        EXPECT_NE(mgr.GetUserId(), -1);
    });
    t1.join();
    t2.join();
}

/**
 * @tc.name UserMgr_Thread_002
 * @tc.desc 测试UserMgr线程安全
 * @tc.type FUNC
 */
HWTEST_F(UserMgrTest, UserMgr_Thread_002, TestSize.Level1)
{
    UserMgr &mgr = UserMgr::GetInstance();
    int32_t newUserId = 5678;
    std::thread t1([&mgr, newUserId]() {
        mgr.SetNewUserId(newUserId);
    });
    std::thread t2([&mgr, newUserId]() {
        EXPECT_EQ(mgr.GetUserId(), newUserId);
    });
    t1.join();
    t2.join();
}

}
}