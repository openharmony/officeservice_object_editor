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
#include "object_editor_manager_database.h"
#include "object_editor_manager_resmgr.h"
#include "object_editor_config.h"
#include "object_editor_permission_utils.h"
#include "system_utils.h"
#undef protected
#undef private

using namespace testing;
using namespace testing::ext;

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
}

void ObjectEditorManagerDatabaseTest::TearDown()
{
    db_->store_ = nullptr;
    db_->bundleMgr_ = nullptr;
    db_->subscriber_ = nullptr;
    ObjectEditorManagerResmgr::GetInstance().resMgrs_.clear();
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
    EXPECT_EQ(ret, ObjectEditorManagerErrCode::SA_DB_ERR);
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

}

} // namespace ObjectEditor
} // namespace OHOS