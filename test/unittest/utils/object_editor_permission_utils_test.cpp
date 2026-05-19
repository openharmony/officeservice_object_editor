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
#include "object_editor_permission_utils.h"
#include "hilog_object_editor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorPermissionUtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ObjectEditorPermissionUtilsTest::SetUpTestCase()
{
}

void ObjectEditorPermissionUtilsTest::TearDownTestCase()
{
}

void ObjectEditorPermissionUtilsTest::SetUp()
{
}

void ObjectEditorPermissionUtilsTest::TearDown()
{
}

namespace {
/**
 * @tc.name CheckCallingPermission_001
 * @tc.desc Test CheckCallingPermission with valid permission
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckCallingPermission_001, TestSize.Level1)
{
    std::string permission = "ohos.permission.REGISTER_OBJECTEDITOR_EXTENSION";
    bool ret = ObjectEditorPermissionUtils::CheckCallingPermission(permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckCallingPermission_002
 * @tc.desc Test CheckCallingPermission with empty permission
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckCallingPermission_002, TestSize.Level1)
{
    std::string permission = "";
    bool ret = ObjectEditorPermissionUtils::CheckCallingPermission(permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckCallingPermission_003
 * @tc.desc Test CheckCallingPermission with invalid permission
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckCallingPermission_003, TestSize.Level1)
{
    std::string permission = "invalid.permission";
    bool ret = ObjectEditorPermissionUtils::CheckCallingPermission(permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckCallingPermission_004
 * @tc.desc Test CheckCallingPermission with PERMISSION_SERVER constant
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckCallingPermission_004, TestSize.Level1)
{
    bool ret = ObjectEditorPermissionUtils::CheckCallingPermission(PERMISSION_SERVER);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_001
 * @tc.desc Test CheckRequestPermission with valid bundleName and permission
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_001, TestSize.Level1)
{
    std::string bundleName = "com.test.bundle";
    std::string permission = "ohos.permission.REGISTER_OBJECTEDITOR_EXTENSION";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_002
 * @tc.desc Test CheckRequestPermission with empty bundleName
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_002, TestSize.Level1)
{
    std::string bundleName = "";
    std::string permission = "ohos.permission.REGISTER_OBJECTEDITOR_EXTENSION";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_003
 * @tc.desc Test CheckRequestPermission with empty permission
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_003, TestSize.Level1)
{
    std::string bundleName = "com.test.bundle";
    std::string permission = "";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_004
 * @tc.desc Test CheckRequestPermission with both empty parameters
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_004, TestSize.Level1)
{
    std::string bundleName = "";
    std::string permission = "";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_005
 * @tc.desc Test CheckRequestPermission with PERMISSION_SERVER constant
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_005, TestSize.Level1)
{
    std::string bundleName = "com.test.bundle";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, PERMISSION_SERVER);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name CheckRequestPermission_006
 * @tc.desc Test CheckRequestPermission with different bundleName
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, CheckRequestPermission_006, TestSize.Level1)
{
    std::string bundleName = "different.bundle.name";
    std::string permission = "ohos.permission.test";
    bool ret = ObjectEditorPermissionUtils::CheckRequestPermission(bundleName, permission);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name PERMISSION_SERVERConstant_001
 * @tc.desc Test PERMISSION_SERVER constant value
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, PERMISSION_SERVERConstant_001, TestSize.Level1)
{
    EXPECT_STREQ(PERMISSION_SERVER, "ohos.permission.REGISTER_OBJECTEDITOR_EXTENSION");
}

/**
 * @tc.name PERMISSION_SERVERConstant_002
 * @tc.desc Test PERMISSION_SERVER constant not empty
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorPermissionUtilsTest, PERMISSION_SERVERConstant_002, TestSize.Level1)
{
    EXPECT_NE(PERMISSION_SERVER, nullptr);
    EXPECT_NE(strlen(PERMISSION_SERVER), 0);
}
}
}
}