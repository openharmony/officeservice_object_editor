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
#define private public
#define protected public
#include "mock_hilog.h"
#include "object_editor_manager_resmgr.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerResmgrTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    ObjectEditorManagerResmgr &resmgr_;
};

void ObjectEditorManagerResmgrTest::SetUpTestCase() {}

void ObjectEditorManagerResmgrTest::TearDownTestCase() {}

void ObjectEditorManagerResmgrTest::SetUp()
{
    resmgr_ = ObjectEditorManagerResmgr::GetInstance();
    resmgr_.resMgrs_.clear();
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
}

void ObjectEditorManagerResmgrTest::TearDown()
{
    resmgr_.resMgrs_.clear();
}

namespace {

/**
 * @tc.name: AddResourceManager_NewBundle
 * @tc.desc: Test AddResourceManager when bundle not exists in resMgrs_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, AddResourceManager_NewBundle, TestSize.Level1)
{
    auto mockResMgr = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr);
    EXPECT_NE(resmgr_.resMgrs_.find("com.test.bundle1"), resmgr_.resMgrs_.end());
    EXPECT_EQ(resmgr_.resMgrs_["com.test.bundle1"].size(), 1u);
}

/**
 * @tc.name: AddResourceManager_ExistingBundleNewModule
 * @tc.desc: Test AddResourceManager when bundle exists but module+locale is new
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, AddResourceManager_ExistingBundleNewModule, TestSize.Level1)
{
    auto mockResMgr1 = std::make_shared<Global::Resource::ResourceManager>();
    auto mockResMgr2 = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr1);
    resmgr_.AddResourceManager("com.test.bundle1", "module2", "zh", mockResMgr2);
    EXPECT_EQ(resmgr_.resMgrs_["com.test.bundle1"].size(), 2u);
}

/**
 * @tc.name: AddResourceManager_ExistingBundleExistingModule
 * @tc.desc: Test AddResourceManager when bundle and module+locale both exist
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, AddResourceManager_ExistingBundleExistingModule, TestSize.Level1)
{
    auto mockResMgr1 = std::make_shared<Global::Resource::ResourceManager>();
    auto mockResMgr2 = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr1);
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr2);
    EXPECT_EQ(resmgr_.resMgrs_["com.test.bundle1"].size(), 1u);
}

/**
 * @tc.name: GetResourceManager_BundleNotFound
 * @tc.desc: Test GetResourceManager when bundle not found
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, GetResourceManager_BundleNotFound, TestSize.Level1)
{
    auto result = resmgr_.GetResourceManager("com.test.nonexist", "module1", "zh");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetResourceManager_ModuleLocaleNotFound
 * @tc.desc: Test GetResourceManager when bundle found but module+locale not found
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, GetResourceManager_ModuleLocaleNotFound, TestSize.Level1)
{
    auto mockResMgr = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr);
    auto result = resmgr_.GetResourceManager("com.test.bundle1", "module2", "zh");
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetResourceManager_Found
 * @tc.desc: Test GetResourceManager when both bundle and module+locale found
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, GetResourceManager_Found, TestSize.Level1)
{
    auto mockResMgr = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr);
    auto result = resmgr_.GetResourceManager("com.test.bundle1", "module1", "zh");
    EXPECT_NE(result, nullptr);
    EXPECT_EQ(result, mockResMgr);
}

/**
 * @tc.name: RemoveBundle_Existing
 * @tc.desc: Test RemoveBundle when bundle exists in resMgrs_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, RemoveBundle_Existing, TestSize.Level1)
{
    auto mockResMgr = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr);
    EXPECT_NE(resmgr_.resMgrs_.find("com.test.bundle1"), resmgr_.resMgrs_.end());
    resmgr_.RemoveBundle("com.test.bundle1");
    EXPECT_EQ(resmgr_.resMgrs_.find("com.test.bundle1"), resmgr_.resMgrs_.end());
}

/**
 * @tc.name: RemoveBundle_NonExisting
 * @tc.desc: Test RemoveBundle when bundle not exists in resMgrs_
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, RemoveBundle_NonExisting, TestSize.Level1)
{
    resmgr_.RemoveBundle("com.test.nonexist");
    EXPECT_EQ(resmgr_.resMgrs_.find("com.test.nonexist"), resmgr_.resMgrs_.end());
}

/**
 * @tc.name: CreateResourceManager_ExistingResMgr
 * @tc.desc: Test CreateResourceManager returns existing ResourceManager when it already exists
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, CreateResourceManager_ExistingResMgr, TestSize.Level1)
{
    auto mockResMgr = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "en", mockResMgr);
    auto result = resmgr_.CreateResourceManager("com.test.bundle1", "module1", "en", "/res/path", "/hap/path");
    EXPECT_EQ(result, mockResMgr);
}

/**
 * @tc.name: RemoveBundle_MultipleModules
 * @tc.desc: Test RemoveBundle removes all module entries under a bundle
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerResmgrTest, RemoveBundle_MultipleModules, TestSize.Level1)
{
    auto mockResMgr1 = std::make_shared<Global::Resource::ResourceManager>();
    auto mockResMgr2 = std::make_shared<Global::Resource::ResourceManager>();
    resmgr_.AddResourceManager("com.test.bundle1", "module1", "zh", mockResMgr1);
    resmgr_.AddResourceManager("com.test.bundle1", "module2", "en", mockResMgr2);
    EXPECT_EQ(resmgr_.resMgrs_["com.test.bundle1"].size(), 2u);
    resmgr_.RemoveBundle("com.test.bundle1");
    EXPECT_EQ(resmgr_.resMgrs_.find("com.test.bundle1"), resmgr_.resMgrs_.end());
}

}

} // namespace ObjectEditor
} // namespace OHOS