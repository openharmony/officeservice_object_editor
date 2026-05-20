/*
 * Copyright (c) Huawei Device Co., Ltd. 2026-2026. All rights reserved.
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
#include "mock_ability_manager_client.h"
#define protected public
#define private public
#include "object_editor_config.h"
#include "object_editor_manager_system_ability.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerSystemAbilityTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<ObjectEditorManagerSystemAbility> sa_;
};

void ObjectEditorManagerSystemAbilityTest::SetUpTestCase() {}

void ObjectEditorManagerSystemAbilityTest::TearDownTestCase() {}

void ObjectEditorManagerSystemAbilityTest::SetUp()
{
    sa_ = sptr<ObjectEditorManagerSystemAbility>::MakeSptr();
    EXPECT_NE(sa_, nullptr);
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
    ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
    config.isSupportObjectEditor_.isLoaded = true;
    config.isSupportObjectEditor_.value = true;
}

void ObjectEditorManagerSystemAbilityTest::TearDown() {}

} // namespace ObjectEditor
} // namespace OHOS
