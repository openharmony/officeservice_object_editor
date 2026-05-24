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
#include "mock_rdb_store.h"
#include "mock_bundle_mgr.h"
#include "mock_system_ability_manager.h"
#include "mock_access_token_kit.h"
#include "mock_ipc_skeleton.h"
#include "object_editor_manager_utils.h"
#include "object_editor_manager_resmgr.h"
#include "object_editor_permission_utils.h"
#include "system_utils.h"
#include "user_mgr.h"
#include "nlohmann/json.hpp"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerUtilsTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    sptr<AppExecFwk::MockBundleMgr> mockBundleMgr_;
};

void ObjectEditorManagerUtilsTest::SetUpTestCase() {}

void ObjectEditorManagerUtilsTest::TearDownTestCase() {}

void ObjectEditorManagerUtilsTest::SetUp()
{
    mockBundleMgr_ = sptr<AppExecFwk::MockBundleMgr>::MakeSptr();
    ASSERT_NE(mockBundleMgr_, nullptr);

    ON_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _)).WillByDefault(Return(ERR_OK));
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(ERR_OK));

    SetGetHapTokenInfoResult(ERR_OK);
    SetHapTokenInfoBundleName("com.test.bundle");
    SetCallingTokenId(100);

    logMsg.clear();
    LOG_SetCallback(MyLogCallback);

    ObjectEditorManagerResmgr::GetInstance().resMgrs_.clear();
}

void ObjectEditorManagerUtilsTest::TearDown()
{
    ObjectEditorManagerResmgr::GetInstance().resMgrs_.clear();
    ResetMockSystemAbilityManager();
}

namespace {

/**
 * @tc.name: GetBundleInfos_BundleMgrNull
 * @tc.desc: Test GetBundleInfos when bundleMgr is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleInfos_BundleMgrNull, TestSize.Level1)
{
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    std::map<std::string, AppExecFwk::BundleInfo> bundleInfos;
    GetBundleInfos(nullptr, extensionInfos, bundleInfos);
    EXPECT_TRUE(bundleInfos.empty());
}

/**
 * @tc.name: GetBundleInfos_GetBundleInfoFailed
 * @tc.desc: Test GetBundleInfos when GetBundleInfoV9 fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleInfos_GetBundleInfoFailed, TestSize.Level1)
{
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.bundle";
    extensionInfos.push_back(extInfo);

    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(-1));

    std::map<std::string, AppExecFwk::BundleInfo> bundleInfos;
    GetBundleInfos(mockBundleMgr_, extensionInfos, bundleInfos);
    EXPECT_TRUE(bundleInfos.empty());
}

/**
 * @tc.name: GetBundleInfos_DuplicateBundleName
 * @tc.desc: Test GetBundleInfos with duplicate bundle names (should skip second)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleInfos_DuplicateBundleName, TestSize.Level1)
{
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo1;
    extInfo1.bundleName = "com.test.bundle";
    extensionInfos.push_back(extInfo1);
    AppExecFwk::ExtensionAbilityInfo extInfo2;
    extInfo2.bundleName = "com.test.bundle";
    extensionInfos.push_back(extInfo2);

    AppExecFwk::BundleInfo bundleInfo;
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));

    std::map<std::string, AppExecFwk::BundleInfo> bundleInfos;
    GetBundleInfos(mockBundleMgr_, extensionInfos, bundleInfos);
    EXPECT_EQ(bundleInfos.size(), 1u);
}

/**
 * @tc.name: GetBundleInfos_Success
 * @tc.desc: Test GetBundleInfos succeeds with valid data
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleInfos_Success, TestSize.Level1)
{
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.bundle";
    extensionInfos.push_back(extInfo);

    AppExecFwk::BundleInfo bundleInfo;
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));

    std::map<std::string, AppExecFwk::BundleInfo> bundleInfos;
    GetBundleInfos(mockBundleMgr_, extensionInfos, bundleInfos);
}

/**
 * @tc.name: GetAppIdentifier_GetHapTokenInfoFailed
 * @tc.desc: Test GetAppIdentifier when GetHapTokenInfo fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetAppIdentifier_GetHapTokenInfoFailed, TestSize.Level1)
{
    SetGetHapTokenInfoResult(-1);
    std::string appIdentifier;
    bool ret = GetAppIdentifier(appIdentifier);
    EXPECT_FALSE(ret);
    SetGetHapTokenInfoResult(ERR_OK);
}

/**
 * @tc.name: GetAppIdentifier_BundleMgrNull
 * @tc.desc: Test GetAppIdentifier when GetBundleMgr returns null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetAppIdentifier_BundleMgrNull, TestSize.Level1)
{
    SetMockSaMgrNull(true);
    SetGetHapTokenInfoResult(ERR_OK);
    SetHapTokenInfoBundleName("com.test.bundle");
    std::string appIdentifier;
    bool ret = GetAppIdentifier(appIdentifier);
    EXPECT_FALSE(ret);
    SetMockSaMgrNull(false);
}

/**
 * @tc.name: GetAppIdentifier_GetBundleInfoFailed
 * @tc.desc: Test GetAppIdentifier when GetBundleInfo fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetAppIdentifier_GetBundleInfoFailed, TestSize.Level1)
{
    SetGetHapTokenInfoResult(ERR_OK);
    SetHapTokenInfoBundleName("com.test.bundle");

    InitMockSystemAbilityManager();
    sptr<AppExecFwk::MockBundleMgr> mockMgr = sptr<AppExecFwk::MockBundleMgr>::MakeSptr();
    SetMockBundleMgrRemoteObj(mockMgr->AsObject());
    EXPECT_CALL(*mockMgr, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(-1));

    std::string appIdentifier;
    bool ret = GetAppIdentifier(appIdentifier);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetAppIdentifier_AppIdentifierEmpty
 * @tc.desc: Test GetAppIdentifier when appIdentifier is empty in signatureInfo
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetAppIdentifier_AppIdentifierEmpty, TestSize.Level1)
{
    SetGetHapTokenInfoResult(ERR_OK);
    SetHapTokenInfoBundleName("com.test.bundle");

    InitMockSystemAbilityManager();
    sptr<AppExecFwk::MockBundleMgr> mockMgr = sptr<AppExecFwk::MockBundleMgr>::MakeSptr();
    SetMockBundleMgrRemoteObj(mockMgr->AsObject());

    AppExecFwk::BundleInfo bundleInfo;
    bundleInfo.signatureInfo.appIdentifier = "";
    EXPECT_CALL(*mockMgr, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));

    std::string appIdentifier;
    bool ret = GetAppIdentifier(appIdentifier);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: GetAppIdentifier_Success
 * @tc.desc: Test GetAppIdentifier succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetAppIdentifier_Success, TestSize.Level1)
{
    SetGetHapTokenInfoResult(ERR_OK);
    SetHapTokenInfoBundleName("com.test.bundle");

    InitMockSystemAbilityManager();
    sptr<AppExecFwk::MockBundleMgr> mockMgr = sptr<AppExecFwk::MockBundleMgr>::MakeSptr();
    SetMockBundleMgrRemoteObj(mockMgr->AsObject());

    AppExecFwk::BundleInfo bundleInfo;
    bundleInfo.signatureInfo.appIdentifier = "test_app_id";
    EXPECT_CALL(*mockMgr, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));

    std::string appIdentifier;
    bool ret = GetAppIdentifier(appIdentifier);
    EXPECT_TRUE(ret);
    EXPECT_EQ(appIdentifier, "test_app_id");
}

/**
 * @tc.name: GetBundleMgr_SaMgrNull
 * @tc.desc: Test GetBundleMgr when SystemAbilityManager is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleMgr_SaMgrNull, TestSize.Level1)
{
    SetMockSaMgrNull(true);
    auto result = GetBundleMgr();
    EXPECT_EQ(result, nullptr);
    SetMockSaMgrNull(false);
}

/**
 * @tc.name: GetBundleMgr_RemoteObjectNull
 * @tc.desc: Test GetBundleMgr when GetSystemAbility returns null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, GetBundleMgr_RemoteObjectNull, TestSize.Level1)
{
    InitMockSystemAbilityManager();
    SetMockBundleMgrRemoteNull(true);
    auto result = GetBundleMgr();
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: BuildObjectEditorFormat_ResMgrNull
 * @tc.desc: Test BuildObjectEditorFormat when CreateResourceManager returns null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, BuildObjectEditorFormat_ResMgrNull, TestSize.Level1)
{
    ObjectEditorFormat format;
    NativeRdb::RowEntity rowEntity;
    rowEntity.Set("oeid", NativeRdb::ValueObject("test_oeid"));
    rowEntity.Set("bundle_name", NativeRdb::ValueObject("com.test.nonexist"));
    rowEntity.Set("module_name", NativeRdb::ValueObject("module1"));
    rowEntity.Set("ability_name", NativeRdb::ValueObject("ability1"));
    rowEntity.Set("resource_path", NativeRdb::ValueObject(""));
    rowEntity.Set("hap_path", NativeRdb::ValueObject(""));
    rowEntity.Set("version", NativeRdb::ValueObject("1.0"));
    rowEntity.Set("name_id", NativeRdb::ValueObject(static_cast<int32_t>(1)));
    rowEntity.Set("description_id", NativeRdb::ValueObject(static_cast<int32_t>(1)));
    rowEntity.Set("file_exts", NativeRdb::ValueObject(".docx"));
    rowEntity.Set("icon_id", NativeRdb::ValueObject(static_cast<int32_t>(1)));
    rowEntity.Set("create_time", NativeRdb::ValueObject(static_cast<int64_t>(1000)));

    bool ret = BuildObjectEditorFormat(format, rowEntity, "zh");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: BuildValuesBuckets_ResMgrNull
 * @tc.desc: Test BuildValuesBuckets when CreateResourceManager returns null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, BuildValuesBuckets_ResMgrNull, TestSize.Level1)
{
    AppExecFwk::BundleInfo bundleInfo;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.nonexist";
    extInfo.moduleName = "module1";
    extInfo.resourcePath = "";
    extInfo.hapPath = "";
    std::vector<NativeRdb::ValuesBucket> buckets;
    bool ret = BuildValuesBuckets(buckets, bundleInfo, extInfo);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: BuildValuesBuckets_ReadConfigFileFailed_NoMetadata
 * @tc.desc: Test BuildValuesBuckets when extension has no content_embed_config metadata
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerUtilsTest, BuildValuesBuckets_ReadConfigFileFailed_NoMetadata, TestSize.Level1)
{
    AppExecFwk::BundleInfo bundleInfo;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.bundle";
    extInfo.moduleName = "module1";
    extInfo.resourcePath = "/data/test/res";
    extInfo.hapPath = "";
    extInfo.metadata.clear();
    std::vector<NativeRdb::ValuesBucket> buckets;
    bool ret = BuildValuesBuckets(buckets, bundleInfo, extInfo);
    EXPECT_FALSE(ret);
}

}

} // namespace ObjectEditor
} // namespace OHOS