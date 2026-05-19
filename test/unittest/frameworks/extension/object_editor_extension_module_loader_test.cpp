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
#include "object_editor_extension_module_loader.h"
#include "object_editor_extension.h"
#include "runtime.h"
#include "hilog_object_editor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace AbilityRuntime {

class ObjectEditorExtensionModuleLoaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    ObjectEditorExtensionModuleLoader* loader_;
};

void ObjectEditorExtensionModuleLoaderTest::SetUpTestCase()
{
}

void ObjectEditorExtensionModuleLoaderTest::TearDownTestCase()
{
}

void ObjectEditorExtensionModuleLoaderTest::SetUp()
{
    loader_ = &ObjectEditorExtensionModuleLoader::GetInstance();
}

void ObjectEditorExtensionModuleLoaderTest::TearDown()
{
    loader_ = nullptr;
}

namespace {
/**
 * @tc.name GetInstance_001
 * @tc.desc Test GetInstance method returns valid instance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetInstance_001, TestSize.Level1)
{
    auto& instance = ObjectEditorExtensionModuleLoader::GetInstance();
    EXPECT_NE(&instance, nullptr);
}

/**
 * @tc.name GetInstance_002
 * @tc.desc Test GetInstance method returns same instance
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetInstance_002, TestSize.Level1)
{
    auto& instance1 = ObjectEditorExtensionModuleLoader::GetInstance();
    auto& instance2 = ObjectEditorExtensionModuleLoader::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

/**
 * @tc.name Create_001
 * @tc.desc Test Create method with valid runtime
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, Create_001, TestSize.Level1)
{
    std::unique_ptr<Runtime> runtime = std::make_unique<Runtime>();
    Extension* extension = loader_->Create(runtime);
    EXPECT_NE(extension, nullptr);
    delete extension;
}

/**
 * @tc.name Create_002
 * @tc.desc Test Create method with null runtime
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, Create_002, TestSize.Level1)
{
    std::unique_ptr<Runtime> runtime = nullptr;
    Extension* extension = loader_->Create(runtime);
    EXPECT_NE(extension, nullptr);
    delete extension;
}

/**
 * @tc.name Create_003
 * @tc.desc Test Create method returns ObjectEditorExtension type
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, Create_003, TestSize.Level1)
{
    std::unique_ptr<Runtime> runtime = std::make_unique<Runtime>();
    Extension* extension = loader_->Create(runtime);
    EXPECT_NE(extension, nullptr);
    ObjectEditorExtension* oeExtension = dynamic_cast<ObjectEditorExtension*>(extension);
    EXPECT_NE(oeExtension, nullptr);
    delete extension;
}

/**
 * @tc.name GetParams_001
 * @tc.desc Test GetParams method returns valid map
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetParams_001, TestSize.Level1)
{
    std::map<std::string, std::string> params = loader_->GetParams();
    EXPECT_EQ(params.size(), 2);
}

/**
 * @tc.name GetParams_002
 * @tc.desc Test GetParams method contains type key
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetParams_002, TestSize.Level1)
{
    std::map<std::string, std::string> params = loader_->GetParams();
    EXPECT_TRUE(params.find("type") != params.end());
}

/**
 * @tc.name GetParams_003
 * @tc.desc Test GetParams method type value is correct
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetParams_003, TestSize.Level1)
{
    std::map<std::string, std::string> params = loader_->GetParams();
    EXPECT_EQ(params["type"], "511");
}

/**
 * @tc.name GetParams_004
 * @tc.desc Test GetParams method contains name key
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetParams_004, TestSize.Level1)
{
    std::map<std::string, std::string> params = loader_->GetParams();
    EXPECT_TRUE(params.find("name") != params.end());
}

/**
 * @tc.name GetParams_005
 * @tc.desc Test GetParams method name value is correct
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, GetParams_005, TestSize.Level1)
{
    std::map<std::string, std::string> params = loader_->GetParams();
    EXPECT_EQ(params["name"], "ObjectEditorExtension");
}

/**
 * @tc.name Constructor_001
 * @tc.desc Test default constructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, Constructor_001, TestSize.Level1)
{
    ObjectEditorExtensionModuleLoader loader;
    EXPECT_TRUE(true);
}

/**
 * @tc.name Destructor_001
 * @tc.desc Test destructor
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, Destructor_001, TestSize.Level1)
{
    ObjectEditorExtensionModuleLoader* loader = new ObjectEditorExtensionModuleLoader();
    delete loader;
    EXPECT_TRUE(true);
}

/**
 * @tc.name MultipleCreate_001
 * @tc.desc Test Create method multiple times
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorExtensionModuleLoaderTest, MultipleCreate_001, TestSize.Level1)
{
    std::unique_ptr<Runtime> runtime = std::make_unique<Runtime>();
    Extension* extension1 = loader_->Create(runtime);
    Extension* extension2 = loader_->Create(runtime);
    EXPECT_NE(extension1, nullptr);
    EXPECT_NE(extension2, nullptr);
    EXPECT_NE(extension1, extension2);
    delete extension1;
    delete extension2;
}
}
}
}