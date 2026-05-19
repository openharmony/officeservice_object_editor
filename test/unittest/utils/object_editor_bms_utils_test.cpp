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

#include "bundle_constants.h"
#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "want.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "object_editor_bms_utils.h"
#include "hilog_object_editor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorBmsUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorBmsUtilsTest::SetUpTestCase()
{
}

void ObjectEditorBmsUtilsTest::TearDownTestCase()
{
}

void ObjectEditorBmsUtilsTest::SetUp()
{
}

void ObjectEditorBmsUtilsTest::TearDown()
{
}

namespace {
/**
 * @tc.name GetBundleMgr_001
 * @tc.desc Test GetBundleMgr method returns null without system ability
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, GetBundleMgr_001, TestSize.Level1)
{
    auto bundleMgr = ObjectEditorBmsUtils::GetBundleMgr();
    EXPECT_EQ(bundleMgr, nullptr);
}

/**
 * @tc.name GetBundleMgr_002
 * @tc.desc Test GetBundleMgr method returns null multiple times
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, GetBundleMgr_002, TestSize.Level1)
{
    auto bundleMgr1 = ObjectEditorBmsUtils::GetBundleMgr();
    auto bundleMgr2 = ObjectEditorBmsUtils::GetBundleMgr();
    EXPECT_EQ(bundleMgr1, nullptr);
    EXPECT_EQ(bundleMgr2, nullptr);
}

/**
 * @tc.name QueryAbilityInfo_001
 * @tc.desc Test QueryAbilityInfo with valid want
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfo_001, TestSize.Level1)
{
    AAFwk::Want want;
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name QueryAbilityInfo_002
 * @tc.desc Test QueryAbilityInfo with empty want
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfo_002, TestSize.Level1)
{
    AAFwk::Want want;
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name QueryAbilityInfo_003
 * @tc.desc Test QueryAbilityInfo with want containing element
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfo_003, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetElementName("com.test.bundle", "TestAbility");
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name QueryAbilityInfo_004
 * @tc.desc Test QueryAbilityInfo with want containing action
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfo_004, TestSize.Level1)
{
    AAFwk::Want want;
    want.SetAction("action.test");
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name QueryAbilityInfo_005
 * @tc.desc Test QueryAbilityInfo with multiple parameters
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfo_005, TestSize.Level1)
{
    AAFwk::Want want1;
    AAFwk::Want want2;
    AppExecFwk::AbilityInfo abilityInfo1;
    AppExecFwk::AbilityInfo abilityInfo2;
    bool ret1 = ObjectEditorBmsUtils::QueryAbilityInfo(want1, abilityInfo1);
    bool ret2 = ObjectEditorBmsUtils::QueryAbilityInfo(want2, abilityInfo2);
    EXPECT_EQ(ret1, false);
    EXPECT_EQ(ret2, false);
}

/**
 * @tc.name QueryAbilityInfoMultiple_001
 * @tc.desc Test QueryAbilityInfo multiple calls
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorBmsUtilsTest, QueryAbilityInfoMultiple_001, TestSize.Level1)
{
    AAFwk::Want want;
    AppExecFwk::AbilityInfo abilityInfo;
    bool ret1 = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    bool ret2 = ObjectEditorBmsUtils::QueryAbilityInfo(want, abilityInfo);
    EXPECT_EQ(ret1, false);
    EXPECT_EQ(ret2, false);
}
}
}
}