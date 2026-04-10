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
#include "object_editor_config.h"
#include "parameters.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {
namespace {
constexpr const char *IS_SUPPORT_OBJECT_EDITOR = "const.object_editor.object_editor_enable";
}
class ObjectEditorConfigTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    ObjectEditorConfig config_;
};

void ObjectEditorConfigTest::SetUpTestCase()
{
}

void ObjectEditorConfigTest::TearDownTestCase()
{
}

void ObjectEditorConfigTest::SetUp()
{
    config_.isSupportObjectEditor_.isLoaded = false;
}

void ObjectEditorConfigTest::TearDown()
{
}

namespace {

/**
 * @tc.name ObjectEditorConfig_001
 * @tc.desc 测试IsSupportObjectEditor方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorConfigTest, ObjectEditorConfig_001, TestSize.Level1)
{
    bool expect = system::GetBoolParameter(IS_SUPPORT_OBJECT_EDITOR, false);
    EXPECT_EQ(config_.IsSupportObjectEditor(), expect);
    EXPECT_TRUE(config_.isSupportObjectEditor_.isLoaded);
    EXPECT_EQ(config_.IsSupportObjectEditor_.value, expect);
}

/**
 * @tc.name ObjectEditorConfig_002
 * @tc.desc 测试IsSupportObjectEditor方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorConfigTest, ObjectEditorConfig_002, TestSize.Level1)
{
    config_.isSupportObjectEditor_.isLoaded = true;
    config_.isSupportObjectEditor_.value = true;
    EXPECT_TRUE(config_.IsSupportObjectEditor());
}

}
}
}