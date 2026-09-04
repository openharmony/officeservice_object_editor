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
#include "mock_common_event_manager.h"
#include "mock_system_ability_manager.h"
#include "mock_access_token_kit.h"
#include "mock_access_token_verify.h"
#include "object_editor_manager_database.h"
#include "object_editor_manager_resmgr.h"
#include "object_editor_config.h"
#include "object_editor_permission_utils.h"
#include "system_utils.h"
#include "want.h"
#include "user_mgr.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;
using OHOS::Security::AccessToken::ResetAccessTokenVerifyMock;
using OHOS::Security::AccessToken::SetGetHapTokenIDResult;
using OHOS::Security::AccessToken::SetVerifyAccessTokenResult;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerDatabaseTest : public ::testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    ObjectEditorManagerDatabase *db_;
    std::shared_ptr<NativeRdb::MockRdbStore> mockStore_;
    sptr<AppExecFwk::MockBundleMgr> mockBundleMgr_;
    int32_t currentUserId_;
    int32_t mismatchUserId_;
};

void ObjectEditorManagerDatabaseTest::SetUpTestCase() {}

void ObjectEditorManagerDatabaseTest::TearDownTestCase() {}

void ObjectEditorManagerDatabaseTest::SetUp()
{
    db_ = &ObjectEditorManagerDatabase::GetInstance();
    db_->store_ = nullptr;
    db_->bundleMgr_ = nullptr;
    db_->subscriber_ = nullptr;
    db_->dbDir_ = "/data/service/el2/public/object_editor_service/database/100";
    db_->dbPath_ = db_->dbDir_ + "/object_editor.db";

    mockStore_ = std::make_shared<NativeRdb::MockRdbStore>();
    mockBundleMgr_ = sptr<AppExecFwk::MockBundleMgr>::MakeSptr();

    logMsg.clear();
    LOG_SetCallback(MyLogCallback);

    ::OHOS::EventFwk::SetSubscribeCommonEventResult(true);
    ::OHOS::EventFwk::SetUnsubscribeCommonEventResult(true);

    ON_CALL(*mockStore_, BeginTransaction()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, Commit()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, RollBack()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, ExecuteSql(_, _)).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillByDefault(Return(std::make_pair(NativeRdb::E_OK, static_cast<int64_t>(1))));
    ON_CALL(*mockStore_, Delete(_, _)).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _)).WillByDefault(Return(ERR_OK));
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(true));

    currentUserId_ = UserMgr::GetInstance().GetUserId();
    mismatchUserId_ = currentUserId_ + 1;
}

void ObjectEditorManagerDatabaseTest::TearDown()
{
    db_->store_ = nullptr;
    db_->bundleMgr_ = nullptr;
    db_->subscriber_ = nullptr;
    ObjectEditorManagerResmgr::GetInstance().resMgrs_.clear();
    ResetAccessTokenVerifyMock();
}

namespace {

/**
 * @tc.name: Init_DuplicateInit
 * @tc.desc: Test Init when store_ is already set (duplicate init)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Init_DuplicateInit, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->Init();
    EXPECT_NE(db_->store_, nullptr);
}

/**
 * @tc.name: Init_SubscriberFail
 * @tc.desc: Test Init when InitSubscriber fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Init_SubscriberFail, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    ::OHOS::EventFwk::SetSubscribeCommonEventResult(false);
    db_->Init();
    EXPECT_NE(db_->store_, nullptr);
}

/**
 * @tc.name: ExecuteTransactionSql_EmptyList
 * @tc.desc: Test ExecuteTransactionSql with empty sql list returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_EmptyList, TestSize.Level1)
{
    db_->store_ = mockStore_;
    std::vector<std::string> emptyList;
    bool ret = db_->ExecuteTransactionSql(emptyList);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_StoreNull
 * @tc.desc: Test ExecuteTransactionSql when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_BeginTransactionFailed
 * @tc.desc: Test ExecuteTransactionSql when BeginTransaction fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_BeginTransactionFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(-1));
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_ExecuteSqlFailed
 * @tc.desc: Test ExecuteTransactionSql when ExecuteSql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_ExecuteSqlFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, RollBack()).WillOnce(Return(NativeRdb::E_OK));
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_CommitFailed
 * @tc.desc: Test ExecuteTransactionSql when Commit fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_CommitFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, RollBack()).WillOnce(Return(NativeRdb::E_OK));
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_Success
 * @tc.desc: Test ExecuteTransactionSql succeeds with valid sql list
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillRepeatedly(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;", "CREATE TABLE test (id INTEGER);"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: QueryBySql_StoreNull
 * @tc.desc: Test QueryBySql when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, QueryBySql_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    auto ret = db_->QueryBySql("SELECT * FROM test", resultSet);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: QueryBySql_ResultSetNull
 * @tc.desc: Test QueryBySql when QuerySql returns null resultSet
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, QueryBySql_ResultSetNull, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(nullptr));
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    auto ret = db_->QueryBySql("SELECT * FROM test", resultSet);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL);
}

/**
 * @tc.name: QueryBySql_GoToFirstRowFailed
 * @tc.desc: Test QueryBySql when GoToFirstRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, QueryBySql_GoToFirstRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(-1));
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    auto ret = db_->QueryBySql("SELECT * FROM test", resultSet);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: QueryBySql_Success
 * @tc.desc: Test QueryBySql succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, QueryBySql_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet;
    auto ret = db_->QueryBySql("SELECT * FROM test", resultSet);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(resultSet, nullptr);
}

/**
 * @tc.name: DoInsert_StoreNull
 * @tc.desc: Test DoInsert when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::vector<NativeRdb::ValuesBucket> buckets;
    NativeRdb::ValuesBucket bucket;
    bucket.PutString("oeid", "test_oeid");
    buckets.push_back(bucket);
    bool ret = db_->DoInsert(buckets);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DoInsert_BucketsEmpty
 * @tc.desc: Test DoInsert with empty buckets returns true
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_BucketsEmpty, TestSize.Level1)
{
    db_->store_ = mockStore_;
    std::vector<NativeRdb::ValuesBucket> emptyBuckets;
    bool ret = db_->DoInsert(emptyBuckets);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: DoInsert_BatchInsertFailed
 * @tc.desc: Test DoInsert when BatchInsert fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_BatchInsertFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillOnce(Return(std::make_pair(-1, static_cast<int64_t>(0))));
    std::vector<NativeRdb::ValuesBucket> buckets;
    NativeRdb::ValuesBucket bucket;
    bucket.PutString("oeid", "test_oeid");
    buckets.push_back(bucket);
    bool ret = db_->DoInsert(buckets);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DoInsert_Success
 * @tc.desc: Test DoInsert succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillOnce(Return(std::make_pair(NativeRdb::E_OK, static_cast<int64_t>(1))));
    std::vector<NativeRdb::ValuesBucket> buckets;
    NativeRdb::ValuesBucket bucket;
    bucket.PutString("oeid", "test_oeid");
    buckets.push_back(bucket);
    bool ret = db_->DoInsert(buckets);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: DoDeleteBundle_StoreNull
 * @tc.desc: Test DoDeleteBundle when store is null after HasRegisteredOEFormat
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoDeleteBundle_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    bool ret = db_->DoDeleteBundle("com.test.bundle");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DoDeleteBundle_DeleteFailed
 * @tc.desc: Test DoDeleteBundle when Delete fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoDeleteBundle_DeleteFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(-1));
    bool ret = db_->DoDeleteBundle("com.test.bundle");
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DoDeleteBundle_Success
 * @tc.desc: Test DoDeleteBundle succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoDeleteBundle_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    bool ret = db_->DoDeleteBundle("com.test.bundle");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: AddBundle_StoreNull
 * @tc.desc: Test AddBundle when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, AddBundle_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    db_->AddBundle("com.test.bundle");
}

/**
 * @tc.name: RemoveBundle_StoreNull
 * @tc.desc: Test RemoveBundle when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: RemoveBundle_BeginTransactionFailed
 * @tc.desc: Test RemoveBundle when BeginTransaction fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_BeginTransactionFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(-1));
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: RemoveBundle_DoDeleteFailed
 * @tc.desc: Test RemoveBundle when DoDeleteBundle fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_DoDeleteFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, RollBack()).WillOnce(Return(NativeRdb::E_OK));
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: RemoveBundle_CommitFailed
 * @tc.desc: Test RemoveBundle when Commit fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_CommitFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, RollBack()).WillRepeatedly(Return(NativeRdb::E_OK));
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: RemoveBundle_Success
 * @tc.desc: Test RemoveBundle succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(nullptr));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: UpdateBundle_StoreNull
 * @tc.desc: Test UpdateBundle when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, UpdateBundle_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    db_->UpdateBundle("com.test.bundle");
}

/**
 * @tc.name: GetObjectEditorFormatByOEid_QueryFailed
 * @tc.desc: Test GetObjectEditorFormatByOEid when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEid_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEid("test_oeid", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndMinVersion_QueryFailed
 * @tc.desc: Test GetObjectEditorFormatByOEidAndMinVersion when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndMinVersion_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndMinVersion("test_oeid", "1.0", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_NotSupported
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale when IsSupportObjectEditor returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndLocale_NotSupported, TestSize.Level1)
{
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = false;
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "zh", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_NotSupported
 * @tc.desc: Test GetObjectEditorFormatsByLocale when IsSupportObjectEditor returns false
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByLocale_NotSupported, TestSize.Level1)
{
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = false;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByLocale("zh", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_QueryFailed
 * @tc.desc: Test GetObjectEditorFormatsByLocale when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByLocale_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByLocale("zh", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_InvalidFileExt
 * @tc.desc: Test GetObjectEditorFormatsByFileExt with invalid file extension
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_InvalidFileExt, TestSize.Level1)
{
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt("", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_INVALID_PARAMETER);
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_QueryFailed
 * @tc.desc: Test GetObjectEditorFormatsByFileExt when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt(".docx", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: GetBundleInfoValuesBuckets_BundleMgrNull
 * @tc.desc: Test GetBundleInfoValuesBuckets when bundleMgr is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleInfoValuesBuckets_BundleMgrNull, TestSize.Level1)
{
    db_->bundleMgr_ = nullptr;
    std::vector<NativeRdb::ValuesBucket> buckets;
    auto ret = db_->GetBundleInfoValuesBuckets("com.test.bundle", buckets);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL);
}

/**
 * @tc.name: GetBundleInfoValuesBuckets_GetBundleInfoFailed
 * @tc.desc: Test GetBundleInfoValuesBuckets when GetBundleInfoV9 fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleInfoValuesBuckets_GetBundleInfoFailed, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _)).WillOnce(Return(-1));
    std::vector<NativeRdb::ValuesBucket> buckets;
    auto ret = db_->GetBundleInfoValuesBuckets("com.test.bundle", buckets);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL);
}

/**
 * @tc.name: GetBundleInfoValuesBuckets_NoContentEmbedExtension
 * @tc.desc: Test GetBundleInfoValuesBuckets when bundle has no CONTENT_EMBED extension
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleInfoValuesBuckets_NoContentEmbedExtension, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    AppExecFwk::BundleInfo bundleInfo;
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));
    std::vector<NativeRdb::ValuesBucket> buckets;
    auto ret = db_->GetBundleInfoValuesBuckets("com.test.bundle", buckets);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: GetBundleNameAndCreateTime_QueryFailed
 * @tc.desc: Test GetBundleNameAndCreateTime when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleNameAndCreateTime_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    auto map = db_->GetBundleNameAndCreateTime();
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: GetBundleNameAndCreateTime_Success
 * @tc.desc: Test GetBundleNameAndCreateTime succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleNameAndCreateTime_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    auto map = db_->GetBundleNameAndCreateTime();
}

/**
 * @tc.name: PrepareRefreshDb_BundleMgrNull
 * @tc.desc: Test PrepareRefreshDb when bundleMgr is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, PrepareRefreshDb_BundleMgrNull, TestSize.Level1)
{
    db_->bundleMgr_ = nullptr;
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    auto ret = db_->PrepareRefreshDb(buckets, oldBundles);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL);
}

/**
 * @tc.name: RefreshDb_StoreNull
 * @tc.desc: Test RefreshDb when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    auto ret = db_->RefreshDb();
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: RefreshDb_PrepareFailed
 * @tc.desc: Test RefreshDb when PrepareRefreshDb fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_PrepareFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = nullptr;
    auto ret = db_->RefreshDb();
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL);
}

/**
 * @tc.name: RefreshDb_BeginTransactionFailed
 * @tc.desc: Test RefreshDb when BeginTransaction fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_BeginTransactionFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(-1));
    auto ret = db_->RefreshDb();
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: RefreshDb_DeleteOldBundleFailed
 * @tc.desc: Test RefreshDb when DoDeleteBundle fails for old bundle
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_DeleteOldBundleFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(true));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    ON_CALL(*mockStore_, BeginTransaction()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, Delete(_, _)).WillByDefault(Return(-1));
    ON_CALL(*mockStore_, RollBack()).WillByDefault(Return(NativeRdb::E_OK));
    db_->RefreshDb();
}

/**
 * @tc.name: RefreshDb_InsertFailed
 * @tc.desc: Test RefreshDb when DoInsert fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_InsertFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(true));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    ON_CALL(*mockStore_, BeginTransaction()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillByDefault(Return(std::make_pair(-1, static_cast<int64_t>(0))));
    ON_CALL(*mockStore_, RollBack()).WillByDefault(Return(NativeRdb::E_OK));
    db_->RefreshDb();
}

/**
 * @tc.name: RefreshDb_CommitFailed
 * @tc.desc: Test RefreshDb when Commit fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_CommitFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(true));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    ON_CALL(*mockStore_, BeginTransaction()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillByDefault(Return(std::make_pair(NativeRdb::E_OK, static_cast<int64_t>(0))));
    ON_CALL(*mockStore_, Commit()).WillByDefault(Return(-1));
    ON_CALL(*mockStore_, RollBack()).WillByDefault(Return(NativeRdb::E_OK));
    auto ret = db_->RefreshDb();
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
}

/**
 * @tc.name: HasRegisteredOEFormat_QueryFailed
 * @tc.desc: Test HasRegisteredOEFormat when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, HasRegisteredOEFormat_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::string oeid;
    bool ret = db_->HasRegisteredOEFormat("com.test.bundle", oeid);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HasRegisteredOEFormat_GetRowFailed
 * @tc.desc: Test HasRegisteredOEFormat when GetRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, HasRegisteredOEFormat_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::string oeid;
    bool ret = db_->HasRegisteredOEFormat("com.test.bundle", oeid);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: HasRegisteredOEFormat_EmptyOeid
 * @tc.desc: Test HasRegisteredOEFormat when oeid is empty
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, HasRegisteredOEFormat_EmptyOeid, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    std::string oeid;
    bool ret = db_->HasRegisteredOEFormat("com.test.bundle", oeid);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: ParseExtensionInfos_BundleNotInBundleInfos
 * @tc.desc: Test ParseExtensionInfos when extension bundle not found in bundleInfos
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ParseExtensionInfos_BundleNotInBundleInfos, TestSize.Level1)
{
    db_->bundleMgr_ = nullptr;
    std::map<std::string, int64_t> dbBundles;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.nonexist";
    extInfo.type = AppExecFwk::ExtensionAbilityType::CONTENT_EMBED;
    extensionInfos.push_back(extInfo);
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    db_->ParseExtensionInfos(dbBundles, extensionInfos, buckets, oldBundles);
    EXPECT_TRUE(buckets.empty());
}

/**
 * @tc.name: ParseExtensionInfos_BundleInDbNotInBundleInfos
 * @tc.desc: Test ParseExtensionInfos when db bundle not found in bundleInfos (should mark as old)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ParseExtensionInfos_BundleInDbNotInBundleInfos, TestSize.Level1)
{
    db_->bundleMgr_ = nullptr;
    std::map<std::string, int64_t> dbBundles;
    dbBundles["com.test.removed"] = 1000;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    db_->ParseExtensionInfos(dbBundles, extensionInfos, buckets, oldBundles);
    EXPECT_NE(oldBundles.find("com.test.removed"), oldBundles.end());
}

/**
 * @tc.name: Initted_StoreNull
 * @tc.desc: Test Initted returns false when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Initted_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    EXPECT_FALSE(db_->Initted());
}

/**
 * @tc.name: Initted_StoreSet
 * @tc.desc: Test Initted returns true when store is set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Initted_StoreSet, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_TRUE(db_->Initted());
}

/**
 * @tc.name: OpenDb_DbPathEmpty
 * @tc.desc: Test OpenDb when dbPath is empty
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OpenDb_DbPathEmpty, TestSize.Level1)
{
    db_->dbPath_ = "";
    db_->store_ = nullptr;
    bool ret = db_->OpenDb();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CreateDefaultTable_Success
 * @tc.desc: Test CreateDefaultTable succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, CreateDefaultTable_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillRepeatedly(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    bool ret = db_->CreateDefaultTable();
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CreateDefaultTable_ExecuteTransactionFailed
 * @tc.desc: Test CreateDefaultTable when ExecuteTransactionSql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, CreateDefaultTable_ExecuteTransactionFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(-1));
    bool ret = db_->CreateDefaultTable();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: OnReceiveEvent_PackageAdded_UserIdMismatch
 * @tc.desc: Test OnReceiveEvent ignores PACKAGE_ADDED when event user ID differs from current user
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageAdded_UserIdMismatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageAdded_UserIdMatch
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_ADDED when event user ID matches current user
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageAdded_UserIdMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageRemoved_UserIdMismatch
 * @tc.desc: Test OnReceiveEvent ignores PACKAGE_REMOVED when event user ID differs from current user
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageRemoved_UserIdMismatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageRemoved_UserIdMatch
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_REMOVED when event user ID matches current user
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageRemoved_UserIdMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageFullyRemoved_UserIdMismatch
 * @tc.desc: Test OnReceiveEvent ignores PACKAGE_FULLY_REMOVED when user ID differs
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageFullyRemoved_UserIdMismatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageFullyRemoved_UserIdMatch
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_FULLY_REMOVED when user ID matches
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageFullyRemoved_UserIdMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageReplaced_UserIdMismatch
 * @tc.desc: Test OnReceiveEvent ignores PACKAGE_REPLACED when user ID differs
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageReplaced_UserIdMismatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageReplaced_UserIdMatch
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_REPLACED when user ID matches
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageReplaced_UserIdMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageChanged_UserIdMismatch
 * @tc.desc: Test OnReceiveEvent ignores PACKAGE_CHANGED when user ID differs
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageChanged_UserIdMismatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageChanged_UserIdMatch
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_CHANGED when user ID matches
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageChanged_UserIdMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_UserSwitched_BypassUserIdCheck
 * @tc.desc: Test OnReceiveEvent always processes USER_SWITCHED even when user ID differs
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_UserSwitched_BypassUserIdCheck, TestSize.Level1)
{
    db_->store_ = mockStore_;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    want.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_UserSwitched_MatchingUserId
 * @tc.desc: Test OnReceiveEvent processes USER_SWITCHED when user ID matches
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_UserSwitched_MatchingUserId, TestSize.Level1)
{
    db_->store_ = mockStore_;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_AppIndexGtZero
 * @tc.desc: Test OnReceiveEvent ignores event when appIndex is greater than zero (clone app)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_AppIndexGtZero, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want.SetBundle("com.test.bundle");
    want.SetParam("appIndex", 1);
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore bundleName") != std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_UnknownAction
 * @tc.desc: Test OnReceiveEvent with unknown action does not crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_UnknownAction, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction("com.test.unknown.action");
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_EmptyAction
 * @tc.desc: Test OnReceiveEvent with empty action does not crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_EmptyAction, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageAdded_StoreNull
 * @tc.desc: Test OnReceiveEvent with PACKAGE_ADDED when store is null (AddBundle returns early)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageAdded_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageRemoved_StoreNull
 * @tc.desc: Test OnReceiveEvent with PACKAGE_REMOVED when store is null (RemoveBundle returns early)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageRemoved_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetBundle("com.test.bundle");
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_SubscriberConstructor
 * @tc.desc: Test constructing DbPackageSubscriber does not crash
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_SubscriberConstructor, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    EXPECT_NE(subscriber, nullptr);
}

/**
 * @tc.name: AddBundle_PermissionDenied
 * @tc.desc: Test AddBundle when store is set but permission check fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, AddBundle_PermissionDenied, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    db_->AddBundle("com.test.bundle");
    EXPECT_TRUE(logMsg.find("store is null") == std::string::npos);
}

/**
 * @tc.name: UpdateBundle_PermissionDenied
 * @tc.desc: Test UpdateBundle when store is set but permission check fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, UpdateBundle_PermissionDenied, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    db_->UpdateBundle("com.test.bundle");
    EXPECT_TRUE(logMsg.find("store is null") == std::string::npos);
}

/**
 * @tc.name: GetObjectEditorFormatByOEid_Success
 * @tc.desc: Test GetObjectEditorFormatByOEid succeeds with valid result set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEid_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEid("test_oeid", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(format, nullptr);
}

/**
 * @tc.name: GetObjectEditorFormatByOEid_GetRowFailed
 * @tc.desc: Test GetObjectEditorFormatByOEid when GetRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEid_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEid("test_oeid", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndMinVersion_Success
 * @tc.desc: Test GetObjectEditorFormatByOEidAndMinVersion succeeds with valid result set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndMinVersion_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndMinVersion("test_oeid", "1.0", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_NE(format, nullptr);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndMinVersion_GetRowFailed
 * @tc.desc: Test GetObjectEditorFormatByOEidAndMinVersion when GetRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndMinVersion_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndMinVersion("test_oeid", "1.0", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_QueryFailed
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale when QueryBySql fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndLocale_QueryFailed, TestSize.Level1)
{
    db_->store_ = nullptr;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "zh", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_GetRowFailed
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale when GetRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndLocale_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "zh", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_BuildFailed
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale when BuildObjectEditorFormat fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndLocale_BuildFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "zh", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_GetRowFailed
 * @tc.desc: Test GetObjectEditorFormatsByLocale when GetRow fails for a row
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByLocale_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByLocale("zh", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_AllBuildFail
 * @tc.desc: Test GetObjectEditorFormatsByLocale when BuildObjectEditorFormat fails for all rows
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByLocale_AllBuildFail, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(NativeRdb::E_OK));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByLocale("zh", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_TRUE(formats.empty());
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_Success
 * @tc.desc: Test GetObjectEditorFormatsByFileExt succeeds with matching file extension
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Invoke([](NativeRdb::RowEntity &rowEntity) -> int {
            rowEntity.Clear(5);
            rowEntity.Put("oeid", 0, NativeRdb::ValueObject("test_oeid"));
            rowEntity.Put("bundle_name", 1, NativeRdb::ValueObject("com.test.bundle"));
            rowEntity.Put("module_name", 2, NativeRdb::ValueObject("test_module"));
            rowEntity.Put("ability_name", 3, NativeRdb::ValueObject("TestAbility"));
            rowEntity.Put("file_exts", 4, NativeRdb::ValueObject(".docx"));
            return NativeRdb::E_OK;
        }));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt(".docx", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_EQ(formats.size(), 1u);
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_GetRowFailed
 * @tc.desc: Test GetObjectEditorFormatsByFileExt when GetRow fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt(".docx", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL);
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_FileExtsNoMatch
 * @tc.desc: Test GetObjectEditorFormatsByFileExt when file extension does not match
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_FileExtsNoMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Invoke([](NativeRdb::RowEntity &rowEntity) -> int {
            rowEntity.Clear(1);
            rowEntity.Put("file_exts", 0, NativeRdb::ValueObject(".pdf"));
            return NativeRdb::E_OK;
        }));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt(".docx", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
    EXPECT_TRUE(formats.empty());
}

/**
 * @tc.name: InitSubscriber_AlreadyInit
 * @tc.desc: Test InitSubscriber returns true when subscriber is already initialized
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, InitSubscriber_AlreadyInit, TestSize.Level1)
{
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    db_->subscriber_ = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    EXPECT_TRUE(db_->InitSubscriber());
}

/**
 * @tc.name: Init_NormalPath
 * @tc.desc: Test Init when store is null (HandleOpenDb fails, InitSubscriber succeeds)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Init_NormalPath, TestSize.Level1)
{
    db_->store_ = nullptr;
    db_->bundleMgr_ = nullptr;
    db_->subscriber_ = nullptr;
    ::OHOS::EventFwk::SetSubscribeCommonEventResult(true);
    db_->Init();
    EXPECT_EQ(db_->store_, nullptr);
}

/**
 * @tc.name: Init_SubscriberCreateFail
 * @tc.desc: Test Init when store is null and InitSubscriber fails
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Init_SubscriberCreateFail, TestSize.Level1)
{
    db_->store_ = nullptr;
    db_->bundleMgr_ = nullptr;
    db_->subscriber_ = nullptr;
    ::OHOS::EventFwk::SetSubscribeCommonEventResult(false);
    db_->Init();
    EXPECT_EQ(db_->store_, nullptr);
    EXPECT_EQ(db_->subscriber_, nullptr);
}

/**
 * @tc.name: OnUserSwitched_DbDirChanged
 * @tc.desc: Test OnUserSwitched updates dbDir_ and dbPath_ based on current user ID
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnUserSwitched_DbDirChanged, TestSize.Level1)
{
    db_->dbDir_ = "/some/initial/path";
    db_->dbPath_ = "/some/initial/path/object_editor.db";
    db_->store_ = mockStore_;
    db_->OnUserSwitched();
    std::string expectedDir = "/data/service/el2/public/object_editor_service/database/" +
        std::to_string(currentUserId_);
    EXPECT_EQ(db_->dbDir_, expectedDir);
    EXPECT_EQ(db_->dbPath_, expectedDir + "/object_editor.db");
}

/**
 * @tc.name: DeleteDb_StoreSet
 * @tc.desc: Test DeleteDb sets store_ to nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DeleteDb_StoreSet, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->DeleteDb();
    EXPECT_EQ(db_->store_, nullptr);
}

/**
 * @tc.name: HasRegisteredOEFormat_Success
 * @tc.desc: Test HasRegisteredOEFormat returns true when oeid is non-empty
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, HasRegisteredOEFormat_Success, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Invoke([](NativeRdb::RowEntity &rowEntity) -> int {
            rowEntity.Clear(1);
            rowEntity.Put("oeid", 0, NativeRdb::ValueObject("test_oeid_123"));
            return NativeRdb::E_OK;
        }));
    std::string oeid;
    bool ret = db_->HasRegisteredOEFormat("com.test.bundle", oeid);
    EXPECT_TRUE(ret);
    EXPECT_EQ(oeid, "test_oeid_123");
}

/**
 * @tc.name: GetBundleNameAndCreateTime_GetRowFailed
 * @tc.desc: Test GetBundleNameAndCreateTime when GetRow fails (continues without crash)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetBundleNameAndCreateTime_GetRowFailed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GoToNextRow()).WillOnce(Return(-1));
    EXPECT_CALL(*mockResultSet, GetRow(_)).WillOnce(Return(-1));
    auto map = db_->GetBundleNameAndCreateTime();
    EXPECT_TRUE(map.empty());
}

/**
 * @tc.name: ParseExtensionInfos_BundleUpdated
 * @tc.desc: Test ParseExtensionInfos when bundle is newer than database (marks as old)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ParseExtensionInfos_BundleUpdated, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    SetGetHapTokenIDResult(100); // non-zero token ID so CheckRequestPermission passes
    SetVerifyAccessTokenResult(0); // 0 means PERMISSION_GRANTED
    AppExecFwk::BundleInfo bundleInfo;
    bundleInfo.updateTime = 2000; // newer than database createTime (1000)
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));
    std::map<std::string, int64_t> dbBundles;
    dbBundles["com.test.bundle"] = 1000;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.bundle";
    extInfo.type = AppExecFwk::ExtensionAbilityType::CONTENT_EMBED;
    extensionInfos.push_back(extInfo);
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    db_->ParseExtensionInfos(dbBundles, extensionInfos, buckets, oldBundles);
    EXPECT_NE(oldBundles.find("com.test.bundle"), oldBundles.end());
}

/**
 * @tc.name: ParseExtensionInfos_BundleNotUpdated
 * @tc.desc: Test ParseExtensionInfos when bundle is not newer than database (skips)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ParseExtensionInfos_BundleNotUpdated, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    SetGetHapTokenIDResult(100); // non-zero token ID so CheckRequestPermission passes
    SetVerifyAccessTokenResult(0); // 0 means PERMISSION_GRANTED
    AppExecFwk::BundleInfo bundleInfo;
    bundleInfo.updateTime = 1000; // not newer than database createTime (2000)
    EXPECT_CALL(*mockBundleMgr_, GetBundleInfoV9(_, _, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(bundleInfo), Return(ERR_OK)));
    std::map<std::string, int64_t> dbBundles;
    dbBundles["com.test.bundle"] = 2000;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    AppExecFwk::ExtensionAbilityInfo extInfo;
    extInfo.bundleName = "com.test.bundle";
    extInfo.type = AppExecFwk::ExtensionAbilityType::CONTENT_EMBED;
    extensionInfos.push_back(extInfo);
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    db_->ParseExtensionInfos(dbBundles, extensionInfos, buckets, oldBundles);
    EXPECT_EQ(oldBundles.find("com.test.bundle"), oldBundles.end());
}

/**
 * @tc.name: PrepareRefreshDb_NoExtensions
 * @tc.desc: Test PrepareRefreshDb when no extensions are found
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, PrepareRefreshDb_NoExtensions, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    EXPECT_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _))
        .WillOnce(DoAll(SetArgReferee<2>(std::vector<AppExecFwk::ExtensionAbilityInfo>{}), Return(false)));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    db_->store_ = mockStore_;
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    auto ret = db_->PrepareRefreshDb(buckets, oldBundles);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: PrepareRefreshDb_QueryExtFailed
 * @tc.desc: Test PrepareRefreshDb when QueryExtensionAbilityInfos fails but continues
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, PrepareRefreshDb_QueryExtFailed, TestSize.Level1)
{
    db_->bundleMgr_ = mockBundleMgr_;
    EXPECT_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillOnce(Return(false));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    db_->store_ = mockStore_;
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    auto ret = db_->PrepareRefreshDb(buckets, oldBundles);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: RefreshDb_NoOldBundles
 * @tc.desc: Test RefreshDb when there are no old bundles to delete
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RefreshDb_NoOldBundles, TestSize.Level1)
{
    db_->store_ = mockStore_;
    db_->bundleMgr_ = mockBundleMgr_;
    ON_CALL(*mockBundleMgr_, QueryExtensionAbilityInfos(_, _, _)).WillByDefault(Return(false));
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    ON_CALL(*mockStore_, BeginTransaction()).WillByDefault(Return(NativeRdb::E_OK));
    ON_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillByDefault(Return(std::make_pair(NativeRdb::E_OK, static_cast<int64_t>(0))));
    ON_CALL(*mockStore_, Commit()).WillByDefault(Return(NativeRdb::E_OK));
    auto ret = db_->RefreshDb();
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_OK);
}

/**
 * @tc.name: DoInsert_MultipleBuckets
 * @tc.desc: Test DoInsert with multiple buckets succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_MultipleBuckets, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BatchInsert(_, _, _))
        .WillOnce(Return(std::make_pair(NativeRdb::E_OK, static_cast<int64_t>(3))));
    std::vector<NativeRdb::ValuesBucket> buckets;
    for (int i = 0; i < 3; i++) {
        NativeRdb::ValuesBucket bucket;
        bucket.PutString("oeid", "test_oeid_" + std::to_string(i));
        buckets.push_back(bucket);
    }
    bool ret = db_->DoInsert(buckets);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: ExecuteTransactionSql_SingleSqlSuccess
 * @tc.desc: Test ExecuteTransactionSql with a single SQL succeeds
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, ExecuteTransactionSql_SingleSqlSuccess, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    std::vector<std::string> sqlList = {"PRAGMA FOREIGN_KEYS = ON;"};
    bool ret = db_->ExecuteTransactionSql(sqlList);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: CreateDefaultTable_Failed
 * @tc.desc: Test CreateDefaultTable when ExecuteTransactionSql fails on commit
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, CreateDefaultTable_Failed, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, ExecuteSql(_, _)).WillRepeatedly(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(-1));
    EXPECT_CALL(*mockStore_, RollBack()).WillOnce(Return(NativeRdb::E_OK));
    bool ret = db_->CreateDefaultTable();
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: RemoveBundle_BeginTransactionSuccess
 * @tc.desc: Test RemoveBundle with full success path including HasRegisteredOEFormat
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, RemoveBundle_BeginTransactionSuccess, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Invoke([](NativeRdb::RowEntity &rowEntity) -> int {
            rowEntity.Clear(1);
            rowEntity.Put("oeid", 0, NativeRdb::ValueObject("registered_oeid"));
            return NativeRdb::E_OK;
        }));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    db_->RemoveBundle("com.test.bundle");
}

/**
 * @tc.name: DoDeleteBundle_WithRegisteredOE
 * @tc.desc: Test DoDeleteBundle when HasRegisteredOEFormat returns true (HiSysEventWrite path)
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoDeleteBundle_WithRegisteredOE, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockResultSet, GetRow(_))
        .WillOnce(Invoke([](NativeRdb::RowEntity &rowEntity) -> int {
            rowEntity.Clear(1);
            rowEntity.Put("oeid", 0, NativeRdb::ValueObject("registered_oeid"));
            return NativeRdb::E_OK;
        }));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    bool ret = db_->DoDeleteBundle("com.test.bundle");
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: GetObjectEditorFormatsByLocale_NotSupportedThenQueryFail
 * @tc.desc: Test GetObjectEditorFormatsByLocale when supported but store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByLocale_NotSupportedThenQueryFail, TestSize.Level1)
{
    db_->store_ = nullptr;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = true;
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByLocale("zh", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: GetObjectEditorFormatByOEidAndLocale_NotSupportedThenEnabled
 * @tc.desc: Test GetObjectEditorFormatByOEidAndLocale transitions from unsupported to supported
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatByOEidAndLocale_NotSupportedThenEnabled, TestSize.Level1)
{
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = true;
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.value = false;
    std::unique_ptr<ObjectEditorFormat> format;
    auto ret = db_->GetObjectEditorFormatByOEidAndLocale("test_oeid", "zh", format);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
    ObjectEditorConfig::GetInstance().isSupportObjectEditor_.isLoaded = false;
}

/**
 * @tc.name: OnReceiveEvent_PackageAdded_AppIndexZero
 * @tc.desc: Test OnReceiveEvent processes PACKAGE_ADDED when appIndex is zero
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageAdded_AppIndexZero, TestSize.Level1)
{
    db_->store_ = mockStore_;
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(0);
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    want.SetBundle("com.test.bundle");
    want.SetParam("appIndex", 0);
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore bundleName") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_PackageRemoved_EmptyBundleName
 * @tc.desc: Test OnReceiveEvent with PACKAGE_REMOVED and empty bundle name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_PackageRemoved_EmptyBundleName, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).WillOnce(Return(NativeRdb::E_OK));
    AAFwk::Want want;
    want.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData(want);
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    subscriber->OnReceiveEvent(eventData);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_MultipleEvents
 * @tc.desc: Test OnReceiveEvent processing multiple events in sequence
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_MultipleEvents, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(2).WillRepeatedly(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).Times(2).WillRepeatedly(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).Times(2).WillRepeatedly(Return(NativeRdb::E_OK));
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    AAFwk::Want want1;
    want1.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want1.SetBundle("com.test.bundle1");
    want1.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData1(want1);
    subscriber->OnReceiveEvent(eventData1);
    AAFwk::Want want2;
    want2.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    want2.SetBundle("com.test.bundle2");
    want2.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData2(want2);
    subscriber->OnReceiveEvent(eventData2);
    EXPECT_TRUE(logMsg.find("ignore event") == std::string::npos);
}

/**
 * @tc.name: OnReceiveEvent_MismatchThenMatch
 * @tc.desc: Test OnReceiveEvent first ignores mismatched event then processes matched event
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, OnReceiveEvent_MismatchThenMatch, TestSize.Level1)
{
    db_->store_ = mockStore_;
    ON_CALL(*mockStore_, QuerySql(_, _)).WillByDefault(Return(nullptr));
    EXPECT_CALL(*mockStore_, BeginTransaction()).Times(1).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Delete(_, _)).Times(1).WillOnce(Return(NativeRdb::E_OK));
    EXPECT_CALL(*mockStore_, Commit()).Times(1).WillOnce(Return(NativeRdb::E_OK));
    EventFwk::CommonEventSubscribeInfo subscribeInfo;
    auto subscriber = std::make_shared<ObjectEditorManagerDatabase::DbPackageSubscriber>(subscribeInfo);
    AAFwk::Want want1;
    want1.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want1.SetBundle("com.test.bundle");
    want1.SetParam("userId", mismatchUserId_);
    EventFwk::CommonEventData eventData1(want1);
    subscriber->OnReceiveEvent(eventData1);
    AAFwk::Want want2;
    want2.SetAction(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    want2.SetBundle("com.test.bundle");
    want2.SetParam("userId", currentUserId_);
    EventFwk::CommonEventData eventData2(want2);
    subscriber->OnReceiveEvent(eventData2);
}

/**
 * @tc.name: Initted_StoreNullThenSet
 * @tc.desc: Test Initted returns false when null then true when set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, Initted_StoreNullThenSet, TestSize.Level1)
{
    db_->store_ = nullptr;
    EXPECT_FALSE(db_->Initted());
    db_->store_ = mockStore_;
    EXPECT_TRUE(db_->Initted());
}

/**
 * @tc.name: GetObjectEditorFormatsByFileExt_EmptyResult
 * @tc.desc: Test GetObjectEditorFormatsByFileExt when query returns no results
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, GetObjectEditorFormatsByFileExt_EmptyResult, TestSize.Level1)
{
    db_->store_ = mockStore_;
    auto mockResultSet = std::make_shared<NativeRdb::MockAbsSharedResultSet>();
    EXPECT_CALL(*mockStore_, QuerySql(_, _)).WillOnce(Return(mockResultSet));
    EXPECT_CALL(*mockResultSet, GoToFirstRow()).WillOnce(Return(-1));
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    auto ret = db_->GetObjectEditorFormatsByFileExt(".docx", formats);
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY);
}

/**
 * @tc.name: HasRegisteredOEFormat_StoreNull
 * @tc.desc: Test HasRegisteredOEFormat when store is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, HasRegisteredOEFormat_StoreNull, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::string oeid;
    bool ret = db_->HasRegisteredOEFormat("com.test.bundle", oeid);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: DoInsert_StoreNullThenBuckets
 * @tc.desc: Test DoInsert when store is null then with empty buckets
 * @tc.type: FUNC
 */
HWTEST_F(ObjectEditorManagerDatabaseTest, DoInsert_StoreNullThenBuckets, TestSize.Level1)
{
    db_->store_ = nullptr;
    std::vector<NativeRdb::ValuesBucket> buckets;
    NativeRdb::ValuesBucket bucket;
    bucket.PutString("oeid", "test_oeid");
    buckets.push_back(bucket);
    bool ret = db_->DoInsert(buckets);
    EXPECT_FALSE(ret);
    db_->store_ = mockStore_;
    std::vector<NativeRdb::ValuesBucket> emptyBuckets;
    ret = db_->DoInsert(emptyBuckets);
    EXPECT_TRUE(ret);
}

}

} // namespace ObjectEditor
} // namespace OHOS