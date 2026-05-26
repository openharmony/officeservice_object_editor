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
#include "object_editor_connection.h"
#include "mock_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorConnectionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    sptr<ObjectEditorConnection> connection_;
};

void ObjectEditorConnectionTest::SetUpTestCase()
{
}

void ObjectEditorConnectionTest::TearDownTestCase()
{
}

void ObjectEditorConnectionTest::SetUp()
{
    connection_ = sptr<ObjectEditorConnection>::MakeSptr();
    ASSERT_NE(connection_, nullptr);
    logMsg.clear();
    LOG_SetCallback(MyLogCallback);
}

void ObjectEditorConnectionTest::TearDown()
{
    connection_ = nullptr;
}

namespace {
/**
 * @tc.name: GetExtensionBundleName_001
 * @tc.desc: Test GetExtensionBundleName method returns empty string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, GetExtensionBundleName_001, TestSize.Level1)
{
    std::string& bundleName = connection_->GetExtensionBundleName();
    EXPECT_TRUE(bundleName.empty());
}

/**
 * @tc.name: SetClientBundleName_001
 * @tc.desc: Test SetClientBundleName method with valid bundle name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, SetClientBundleName_001, TestSize.Level1)
{
    std::string bundleName = "test_bundle";
    connection_->SetClientBundleName(bundleName);
    EXPECT_EQ(connection_->GetClientBundleName(), bundleName);
}

/**
 * @tc.name: SetClientBundleName_002
 * @tc.desc: Test SetClientBundleName method with empty bundle name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, SetClientBundleName_002, TestSize.Level1)
{
    std::string bundleName = "";
    connection_->SetClientBundleName(bundleName);
    EXPECT_EQ(connection_->GetClientBundleName(), bundleName);
}

/**
 * @tc.name: GetClientBundleName_001
 * @tc.desc: Test GetClientBundleName method returns empty string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, GetClientBundleName_001, TestSize.Level1)
{
    std::string& bundleName = connection_->GetClientBundleName();
    EXPECT_TRUE(bundleName.empty());
}

/**
 * @tc.name: GetClientBundleName_002
 * @tc.desc: Test GetClientBundleName method after setting bundle name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, GetClientBundleName_002, TestSize.Level1)
{
    connection_->SetClientBundleName("test");
    std::string& bundleName = connection_->GetClientBundleName();
    EXPECT_EQ(bundleName, "test");
}

/**
 * @tc.name: IsExtensionAbilityMatch_001
 * @tc.desc: Test IsExtensionAbilityMatch method with different names
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, IsExtensionAbilityMatch_001, TestSize.Level1)
{
    std::string moduleName = "test_module";
    std::string abilityName = "test_ability";
    bool ret = connection_->IsExtensionAbilityMatch(moduleName, abilityName);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: IsExtensionAbilityMatch_002
 * @tc.desc: Test IsExtensionAbilityMatch method with multiple names
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, IsExtensionAbilityMatch_002, TestSize.Level1)
{
    std::string moduleName1 = "module1";
    std::string abilityName1 = "ability1";
    std::string moduleName2 = "module2";
    std::string abilityName2 = "ability2";
    bool ret1 = connection_->IsExtensionAbilityMatch(moduleName1, abilityName1);
    bool ret2 = connection_->IsExtensionAbilityMatch(moduleName2, abilityName2);
    EXPECT_FALSE(ret1);
    EXPECT_FALSE(ret2);
}

/**
 * @tc.name: RegisterConnectionStatusCallback_001
 * @tc.desc: Test RegisterConnectionStatusCallback method with null callback
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, RegisterConnectionStatusCallback_001, TestSize.Level1)
{
    std::shared_ptr<IObjectEditorConnectionStatusCallback> callback = nullptr;
    connection_->RegisterConnectionStatusCallback(callback);
    EXPECT_TRUE(logMsg.find("callback is null") != std::string::npos);
}

/**
 * @tc.name: StopConnect_001
 * @tc.desc: Test StopConnect method returns failure
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, StopConnect_001, TestSize.Level1)
{
    ObjectEditorManagerErrCode ret = connection_->StopConnect();
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_SUCCEED);
}

/**
 * @tc.name: StartConnect_001
 * @tc.desc: Test StartConnect method with valid parameters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, StartConnect_001, TestSize.Level1)
{
    std::string bundleName = "test_bundle";
    std::string abilityName = "test_ability";
    std::string moduleName = "test_module";
    sptr<IRemoteObject> remoteObject = nullptr;
    ObjectEditorManagerErrCode ret = connection_->StartConnect(bundleName, abilityName, moduleName, remoteObject);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED);
}

/**
 * @tc.name: StartConnect_002
 * @tc.desc: Test StartConnect method with empty parameters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, StartConnect_002, TestSize.Level1)
{
    std::string bundleName = "";
    std::string abilityName = "";
    std::string moduleName = "";
    sptr<IRemoteObject> remoteObject = nullptr;
    ObjectEditorManagerErrCode ret = connection_->StartConnect(bundleName, abilityName, moduleName, remoteObject);
    EXPECT_NE(ret, ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED);
}

/**
 * @tc.name: Constructor_001
 * @tc.desc: Test ObjectEditorConnection constructor
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, Constructor_001, TestSize.Level1)
{
    sptr<ObjectEditorConnection> conn = sptr<ObjectEditorConnection>::MakeSptr();
    EXPECT_NE(conn, nullptr);
}

/**
 * @tc.name: Destructor_001
 * @tc.desc: Test ObjectEditorConnection destructor
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorConnectionTest, Destructor_001, TestSize.Level1)
{
    ObjectEditorConnection* conn = new ObjectEditorConnection();
    delete conn;
    EXPECT_TRUE(true);
}
}
}
}