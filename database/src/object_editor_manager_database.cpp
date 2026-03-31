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

#include <filesystem>
#include "nlohmann/json.hpp"

#include "hilog_object_editor.h"
#include "object_editor_manager_database.h"
#include "object_editor_manager_resmgr.h"
#include "system_utils.h"
#include "user_mgr.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
constexpr const char* APP_INDEX = "appIndex";
const char* TABLE_NAME = "object_editor_info";
const char* DB_DIR = "/data/service/el2/public/object_editor_service/database/";

std::vector<std::string> GetDefaultDbSql()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    std::vector<std::string> defaultSqlList;
    defaultSqlList.emplace_back("PRAGMA FOREIGN_KEYS = ON;");
    defaultSqlList.emplace_back("CREATE TABLE IF NOT EXISTS object_editor_info"
        "(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "hmid TEXT NOT NULL UNIQUE, "
        "bundle_name TEXT NOT NULL, "
        "module_name TEXT NOT NULL, "
        "ability_name TEXT NOT NULL, "
        "resource_path TEXT NOT NULL, "
        "hap_path TEXT NOT NULL, "
        "version TEXT NOT NULL, "
        "name_id INTEGER NOT NULL, "
        "description_id INTEGER NOT NULL, "
        "file_exts TEXT NOT NULL, "
        "icon_id INTEGER NOT NULL, "
        "create_time INTEGER NOT NULL);"
    );
    // 为hmid创建索引
    defaultSqlList.emplace_back("CREATE INDEX IF NOT EXISTS idx_hmid ON object_editor_info(hmid);");
    return defaultSqlList;
}

class ObjectEditorOpenCallback : public NativeRdb::RdbOpenCallback {
public:
    ObjectEditorOpenCallback() = default;
    ~ObjectEditorOpenCallback() = default;

    int32_t OnCreate(NativeRdb::RdbStore &rdbStore) override
    {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
        std::vector<std::string> defaultSqlList = GetDefaultDbSql();
        for (const auto &sql : defaultSqlList) {
            int ret = rdbStore.ExecuteSql(sql);
            if (ret != 0) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "ExecSql sql: %{private}s failed ret: %{public}d",
                    sql.c_str(), ret);
                return ret;
            }
        }
        return NativeRdb::E_OK;
    }

    int32_t OnUpgrade(NativeRdb::RdbStore &rdbStore, int32_t oldVersion, int32_t newVersion) override
    {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "oldVersion: %{public}d, newVersion: %{public}d",
            oldVersion, newVersion);
        return NativeRdb::E_OK;
    }
};
} // namespace

IMPLEMENT_SINGLE_INSTANCE(ObjectEditorManagerDatabase);

ObjectEditorManagerDatabase::ObjectEditorManagerDatabase()
    : store_(nullptr),
    dbDir_(DB_DIR + std::to_string(UserMgr::GetInstance().GetUserId())),
    dbPath_(dbDir_ + "/object_editor.db"),
    bundleMgr_(nullptr),
    subscriber_(nullptr)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "dbPath_: %{private}s", dbPath_.c_str());
}

ObjectEditorManagerDatabase::~ObjectEditorManagerDatabase()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    if (subscriber_ != nullptr) {
        EventFwk::CommonEventManager::UnSubscribeCommonEvent(subscriber_);
        subscriber_ = nullptr;
    }
}

void ObjectEditorManagerDatabase::Init()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    if (store_ != nullptr) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "duplicate init");
        return;
    }
    HandleOpenDb();
    if (!InitSubscriber()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "init subscriber fail");
        store_ = nullptr;
    }
}

void ObjectEditorManagerDatabase::HandleOpenDb()
{
    if (!(std::filesystem::exists(dbDir_) && std::filesystem::is_directory(dbDir_))) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "dbDir not exist");
        std::error_code ec;
        std::filesystem::create_directories(dbDir_, ec);
        if (ec) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "create_directories failed, ec: %{public}s",
                ec.message().c_str());
            return;
        }
    }
    bundleMgr_ = GetBundleMgr();
    if (bundleMgr_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get bundleMgr failed");
        return;
    }
    if (!OpenDb()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "open db failed");
        (void)DeleteDb();
        if (!OpenDb()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "open db failed after delete");
            return;
        }
    }
    ObjectEditorManagerErrCode errCode = RefreshDb();
    if (errCode != ObjectEditorManagerErrCode::SA_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "RefreshDb failed, errCode: %{public}d", errCode);
        store_ = nullptr;
    }
}

void ObjectEditorManagerDatabase::OnUserSwitched()
{
    dbDir_ = DB_DIR + std::to_string(UserMgr::GetInstance().GetUserId());
    dbPath_ = dbDir_ + "/object_editor.db";
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "dbPath_: %{private}s", dbPath_.c_str());
    HandleOpenDb();
}

bool ObjectEditorManagerDatabase::OpenDb()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    if (dbPath_.empty()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "dbPath is empty");
        return false;
    }
    const int32_t version = 1;
    NativeRdb::RdbStoreConfig config(dbPath_);
    config.SetServiceName("ObjectEditorService");
    config.SetSecurityLevel(NativeRdb::SecurityLevel::S2);
    config.SetAllowRebuild(true);
    config.SetTokenizer(NativeRdb::CUSTOM_TOKENIZER);
    ObjectEditorOpenCallback helper;
    int32_t errCode = NativeRdb::E_OK;
    store_ = NativeRdb::RdbHelper::GetRdbStore(config, version, helper, errCode);
    if (errCode != NativeRdb::E_OK || store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get store failed, errCode: %{public}d", errCode);
        store_ = nullptr;
        return false;
    }
    if (!CreateDefaultTable()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "create default table fail");
        store_ = nullptr;
        return false;
    }
    return true;
}

bool ObjectEditorManagerDatabase::DeleteDb()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    store_ = nullptr;
    int32_t errCode = NativeRdb::RdbHelper::DeleteRdbStore(dbPath_);
    if (errCode != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "delete db fail");
        return false;
    }
    return true;
}

bool ObjectEditorManagerDatabase::CreateDefaultTable()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    std::vector<std::string> defaultSqlList = GetDefaultDbSql();
    if (!ExecuteTransactionSql(defaultSqlList)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "ExecuteSqlList failed");
        return false;
    }
    return true;
}

bool ObjectEditorManagerDatabase::InitSubscriber()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscriber_ = std::make_shared<DbPackageSubscriber>(subscribeInfo);
    if (subscriber_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "subscriber_ create failed");
        return false;
    }
    auto ret = EventFwk::CommonEventManager::SubscribeCommonEvent(subscriber_);
    if (ret == false) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "SubscribeCommonEvent failed");
        subscriber_ = nullptr;
        return false;
    }
    return true;
}

bool ObjectEditorManagerDatabase::ExecuteTransactionSql(const std::vector<std::string> &sqlList)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    if (sqlList.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "sqlList is empty");
        return true;
    }
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return false;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BeginTransaction failed, errCode: %{public}d", ret);
        return false;
    }
    for (const auto &sql : sqlList) {
        ret = store_->ExecuteSql(sql);
        if (ret != NativeRdb::E_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "ExecuteSql failed, errCode: %{public}d", ret);
            store_->RollBack();
            return false;
        }
    }
    ret = store_->Commit();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "Commit failed, errCode: %{public}d", ret);
        store_->RollBack();
        return false;
    }
    return true;
}

void ObjectEditorManagerDatabase::AddBundle(const std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName: %{public}s", bundleName.c_str());
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return;
    }
    std::vector<NativeRdb::ValuesBucket> buckets;
    ObjectEditorManagerErrCode errCode = GetBundleInfoValuesBuckets(bundleName, buckets);
    if (errCode != ObjectEditorManagerErrCode::SA_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetBundleInfoValuesBuckets failed");
        return;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BeginTransaction failed, errCode: %{public}d", ret);
        return;
    }
    if (!DoInsert(buckets)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "DoInsert failed");
        store_->RollBack();
        return;
    }
    ret = store_->Commit();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "Commit failed, errCode: %{public}d", ret);
        store_->RollBack();
    }
}

void ObjectEditorManagerDatabase::RemoveBundle(const std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName: %{public}s", bundleName.c_str());
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BeginTransaction failed, errCode: %{public}d", ret);
        return;
    }
    if (!DoDeleteBundle(bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "DoDeleteBundle failed");
        store_->RollBack();
        return;
    }
    ret = store_->Commit();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "Commit failed, errCode: %{public}d", ret);
        store_->RollBack();
    }
}

void ObjectEditorManagerDatabase::UpdateBundle(const std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName: %{public}s", bundleName.c_str());
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return;
    }
    std::vector<NativeRdb::ValuesBucket> buckets;
    ObjectEditorManagerErrCode errCode = GetBundleInfoValuesBuckets(bundleName, buckets);
    if (errCode != ObjectEditorManagerErrCode::SA_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetBundleInfoValuesBuckets failed");
        return;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BeginTransaction failed, errCode: %{public}d", ret);
        return;
    }
    if (!DoDeleteBundle(bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "DoDeleteBundle failed");
        store_->RollBack();
        return;
    }
    if (!DoInsert(buckets)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "DoInsert failed");
        store_->RollBack();
        return;
    }
    ret = store_->Commit();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "Commit failed, errCode: %{public}d", ret);
        store_->RollBack();
    }
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetBundleInfoValuesBuckets(
    const std::string &bundleName, std::vector<NativeRdb::ValuesBucket> &buckets) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName: %{public}s", bundleName.c_str());
    if (bundleMgr_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgr is null");
        return ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL;
    }
    AppExecFwk::BundleInfo bundleInfo;
    ErrCode ret = bundleMgr_->GetBundleInfoV9(bundleName,
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_HAP_MODULE) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_EXTENSION_ABILITY) |
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_METADATA),
        bundleInfo, UserMgr::GetInstance().GetUserId());
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetBundleInfo failed, errCode: %{public}d", ret);
        return ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL;
    }
    for (auto it = bundleInfo.hapModuleInfos.begin(); it != bundleInfo.hapModuleInfos.end(); ++it) {
        for (auto it2 = it->extensionInfos.begin(); it2 != it->extensionInfos.end(); ++it2) {
            if (it2->type != AppExecFwk::ExtensionAbilityType::CONTENT_EMBED) {
                continue;
            }
            if (!BuildValuesBuckets(buckets, bundleInfo, *it2)) {
                OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "build buckets failed");
            }
        }
    }
    if (buckets.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "buckets is empty");
        return ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

bool ObjectEditorManagerDatabase::DoInsert(const std::vector<NativeRdb::ValuesBucket> &buckets)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "store is null");
        return false;
    }
    if (buckets.empty()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "buckets is empty");
        return true;
    }
    std::pair<int32_t, int64_t> result = store_->BatchInsert(TABLE_NAME, buckets,
        NativeRdb::ConflictResolution::ON_CONFLICT_IGNORE);
    if (result.first != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "failed: %{public}d", result.first);
        return false;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "inserted: %{public}lld", result.second);
    return true;
}

bool ObjectEditorManagerDatabase::DoDeleteBundle(const std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName: %{public}s", bundleName.c_str());
    ObjectEditorManagerResmgr::GetInstance().RemoveBundle(bundleName);
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return false;
    }
    NativeRdb::AbsRdbPredicates predicates(TABLE_NAME);
    predicates.EqualTo("bundle_name", NativeRdb::ValueObject(bundleName));
    int32_t rows = 0;
    int32_t ret = store_->Delete(rows, predicates);
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "failed: %{public}d", ret);
        return false;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "rows:%{public}d", rows);
    return true;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetObjectEditorFormatByHmid(
    const std::string &hmid, std::unique_ptr<ObjectEditorFormat> &format) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "hmid: %{public}s", hmid.c_str());
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    std::string sql = "SELECT hmid, bundle_name, module_name, ability_name from object_editor_info WHERE hmid = ?";
    ObjectEditorManagerErrCode errCode = QueryBySql(sql, resultSet, { hmid });
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "query failed");
        return errCode;
    }
    NativeRdb::RowEntity rowEntity;
    resultSet->GetRow(rowEntity);
    format = std::make_unique<ObjectEditorFormat>();
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "format is null");
        return ObjectEditorManagerErrCode::SA_ALLOC_FAIL;
    }
    rowEntity.Get("hmid").GetString(format->hmid);
    rowEntity.Get("bundle_name").GetString(format->bundleName);
    rowEntity.Get("module_name").GetString(format->moduleName);
    rowEntity.Get("ability_name").GetString(format->abilityName);
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetObjectEditorFormatByHmidAndMinVersion(
    const std::string &hmid, const std::string &minVersion, std::unique_ptr<ObjectEditorFormat> &format) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "hmid: %{public}s, minVersion: %{public}s",
        hmid.c_str(), minVersion.c_str());
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    std::string sql = "SELECT hmid, bundle_name, module_name, ability_name from object_editor_info" +
        " WHERE hmid = ? and min_version >= ?";
    ObjectEditorManagerErrCode errCode = QueryBySql(sql, resultSet, { hmid, minVersion });
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "query failed");
        return errCode;
    }

    NativeRdb::RowEntity rowEntity;
    resultSet->GetRow(rowEntity);
    format = std::make_unique<ObjectEditorFormat>();
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "format is null");
        return ObjectEditorManagerErrCode::SA_ALLOC_FAIL;
    }
    rowEntity.Get("hmid").GetString(format->hmid);
    rowEntity.Get("bundle_name").GetString(format->bundleName);
    rowEntity.Get("module_name").GetString(format->moduleName);
    rowEntity.Get("ability_name").GetString(format->abilityName);
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetObjectEditorFormatByHmidAndLocale(
    const std::string &hmid, const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "hmid: %{public}s, locale: %{public}s",
        hmid.c_str(), locale.c_str());
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    ObjectEditorManagerErrCode errCode = QueryBySql("SELECT hmid, bundle_name, module_name, resource_path,"
        " hap_path, file_exts, name_id, description_id, icon_id from object_editor_info WHERE hmid = ?",
        resultSet, { hmid });
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "query failed");
        return errCode;
    }
    NativeRdb::RowEntity rowEntity;
    resultSet->GetRow(rowEntity);
    format = std::make_unique<ObjectEditorFormat>();
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "format is null");
        return ObjectEditorManagerErrCode::SA_ALLOC_FAIL;
    }
    if (!BuildObjectEditorFormat(*format, rowEntity, locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BuildObjectEditorFormat failed");
        return ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetObjectEditorFormatsByLocale(
    const std::string &locale, std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "locale: %{public}s", locale.c_str());
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    ObjectEditorManagerErrCode errCode = QueryBySql("SELECT hmid, bundle_name, module_name, resource_path,"
        " hap_path, file_exts, name_id, description_id, icon_id from object_editor_info", resultSet);
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "query failed");
        return errCode;
    }
    for (int32_t rowResult = NativeRdb::E_OK; rowResult == NativeRdb::E_OK; rowResult = resultSet->GoToNextRow()) {
        NativeRdb::RowEntity rowEntity;
        resultSet->GetRow(rowEntity);
        std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
        if (format == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "format is null");
            formats.clear();
            return ObjectEditorManagerErrCode::SA_ALLOC_FAIL;
        }
        if (!BuildObjectEditorFormat(*format, rowEntity, locale)) {
            OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "BuildObjectEditorFormat failed");
            continue;
        }
        formats.emplace_back(std::move(format));
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::GetObjectEditorFormatsByFileExt(
    const std::string &fileExt, std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "fileExt:%{public}s", fileExt.c_str());
    if (!SystemUtils::IsValidFileExt(fileExt)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid file ext");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    ObjectEditorManagerErrCode errCode = QueryBySql("SELECT hmid, bundle_name, module_name, ability_name,"
        " file_exts from object_editor_info WHERE file_exts like ? ORDER BY create_time DESC",
        resultSet, { "%" + fileExt + "%" });
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "query failed");
        return errCode;
    }
    for (int32_t rowResult = NativeRdb::E_OK; rowResult == NativeRdb::E_OK; rowResult = resultSet->GoToNextRow()) {
        NativeRdb::RowEntity rowEntity;
        resultSet->GetRow(rowEntity);
        std::string fileExts;
        rowEntity.Get("file_exts").GetString(fileExts);
        if (!SystemUtils::FileExtsHasFileExt(fileExts, fileExt)) {
            continue;
        }
        std::unique_ptr<ObjectEditorFormat> format = std::make_unique<ObjectEditorFormat>();
        if (format == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "format is null");
            formats.clear();
            return ObjectEditorManagerErrCode::SA_ALLOC_FAIL;
        }
        rowEntity.Get("hmid").GetString(format->hmid);
        rowEntity.Get("bundle_name").GetString(format->bundleName);
        rowEntity.Get("module_name").GetString(format->moduleName);
        rowEntity.Get("ability_name").GetString(format->abilityName);
        formats.emplace_back(std::move(format));
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::QueryBySql(const std::string &sql,
    std::shared_ptr<NativeRdb::AbsSharedResultSet> &resultSet,
    const std::vector<NativeRdb::ValueObject> &whereArgs) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "sql: %{private}s", sql.c_str());
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return ObjectEditorManagerErrCode::SA_DB_ERR;
    }
    resultSet = store_->QuerySql(sql, whereArgs);
    if (resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "null resultSet");
        return ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL;
    }
    int32_t rowResult = resultSet->GoToFirstRow();
    if (rowResult != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GoToFirstRow failed");
        return ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

std::map<std::string, int64_t> ObjectEditorManagerDatabase::GetBundleNameAndCreateTime() const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    std::shared_ptr<NativeRdb::AbsSharedResultSet> resultSet = nullptr;
    std::string sql = "SELECT DISTINCT bundle_name, create_time from object_editor_info";
    ObjectEditorManagerErrCode errCode = QueryBySql(sql, resultSet);
    if (errCode != ObjectEditorManagerErrCode::SA_OK || resultSet == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "queryBySql failed");
        return {};
    }
    std::map<std::string, int64_t> map;
    for (int32_t rowResult = NativeRdb::E_OK; rowResult == NativeRdb::E_OK; rowResult = resultSet->GoToNextRow()) {
        NativeRdb::RowEntity rowEntity;
        resultSet->GetRow(rowEntity);
        std::string bundleName;
        rowEntity.Get("bundle_name").GetString(bundleName);
        int64_t createTime = 0;
        rowEntity.Get("create_time").GetLong(createTime);
        map.emplace(bundleName, createTime);
    }
    return map;
}

void ObjectEditorManagerDatabase::ParseExtensionInfos(const std::map<std::string, int64_t> &dbBundles,
    std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos,
    std::vector<NativeRdb::ValuesBucket> &buckets, std::set<std::string> &oldBundles) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    std::map<std::string, AppExecFwk::BundleInfo> bundleInfos;
    GetBundleInfos(bundleMgr_, extensionInfos, bundleInfos);
    for (const auto &extensionInfo : extensionInfos) {
        auto it = bundleInfos.find(extensionInfo.bundleName);
        if (it == bundleInfos.end()) {
            continue;
        }
        AppExecFwk::BundleInfo &bundleInfo = it->second;
        auto it2 = dbBundles.find(extensionInfo.bundleName);
        if (it2 != dbBundles.end()) {
            if (bundleInfo.updateTime <= it2->second) {
                continue; // bundle没有更新过，直接skip
            } else {
                oldBundles.emplace(extensionInfo.bundleName); // bundle比数据库更新，删除旧数据
            }
        }
        if (!BuildValuesBuckets(buckets, bundleInfo, extensionInfo)) {
            OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "BuildValuesBuckets bundle:%{public}s failed",
                extensionInfo.bundleName.c_str());
        }
    }
    for (const auto &[bundleName, createTime] : dbBundles) {
        if (bundleInfos.find(bundleName) == bundleInfos.end()) {
            oldBundles.emplace(bundleName);
        }
    }
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::PrepareRefreshDb(
    std::vector<NativeRdb::ValuesBucket> &buckets, std::set<std::string> &oldBundles) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    if (bundleMgr_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgr is null");
        return ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL;
    }
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    (void)bundleMgr_->QueryExtensionAbilityInfos(AppExecFwk::ExtensionAbilityType::CONTENT_EMBED,
        UserMgr::GetInstance().GetUserId(), extensionInfos);
    if (extensionInfos.empty()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE, "extensionInfos is empty");
    }
    std::map<std::string, int64_t> dbBundles = GetBundleNameAndCreateTime();
    ParseExtensionInfos(dbBundles, extensionInfos, buckets, oldBundles);
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerDatabase::RefreshDb()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    if (store_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "store is null");
        return ObjectEditorManagerErrCode::SA_DB_ERR;
    }
    std::vector<NativeRdb::ValuesBucket> buckets;
    std::set<std::string> oldBundles;
    ObjectEditorManagerErrCode errCode = PrepareRefreshDb(buckets, oldBundles);
    if (errCode != ObjectEditorManagerErrCode::SA_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "PrepareRefreshDb failed");
        return errCode;
    }
    int32_t ret = store_->BeginTransaction();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "BeginTransaction failed");
        return ObjectEditorManagerErrCode::SA_DB_ERR;
    }
    for (const auto &bundleName : oldBundles) {
        if (!DoDeleteBundle(bundleName)) {
            store_->RollBack();
            return ObjectEditorManagerErrCode::SA_DB_DELETE_FAIL;
        }
    }
    if (!DoInsert(buckets)) {
        store_->RollBack();
        return ObjectEditorManagerErrCode::SA_DB_INSERT_FAIL;
    }
    ret = store_->Commit();
    if (ret != NativeRdb::E_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "commit failed, errCode:%{public}d", ret);
        store_->RollBack();
        return ObjectEditorManagerErrCode::SA_DB_ERR;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

void ObjectEditorManagerDatabase::DbPackageSubscriber::OnReceiveEvent(
    const EventFwk::CommonEventData &eventData)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "in");
    const AAFwk::Want &want = eventData.GetWant();
    const std::string &action = want.GetAction();
    const std::string &bundleName = want.GetElement().GetBundleName();
    int32_t appIndex = want.GetIntParam(APP_INDEX, 0);
    if (appIndex > 0) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::DATABASE,
            "ignore bundleName:%{public}s, appIndex:%{public}d", bundleName.c_str(), appIndex);
        return;
    }
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_ADDED) {
        ObjectEditorManagerDatabase::GetInstance().AddBundle(bundleName);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REMOVED ||
        action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_FULLY_REMOVED) {
        ObjectEditorManagerDatabase::GetInstance().RemoveBundle(bundleName);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_REPLACED ||
        action == EventFwk::CommonEventSupport::COMMON_EVENT_PACKAGE_CHANGED) {
        ObjectEditorManagerDatabase::GetInstance().UpdateBundle(bundleName);
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_USER_SWITCHED) {
        ObjectEditorManagerDatabase::GetInstance().OnUserSwitched();
    }
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS