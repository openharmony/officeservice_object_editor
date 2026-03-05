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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_DATABASE_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_DATABASE_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "rdb_errno.h"
#include "rdb_helper.h"
#include "rdb_store.h"
#include "single_instance.h"

#include "object_editor_manager_utils.h"

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerDatabase {
    DECLARE_SINGLE_INSTANCE_BASE(ObjectEditorManagerDatabase);

public:
    void Init();
    bool Initted() const { return store_ != nullptr; }
    void AddBundle(const std::string &bundleName);
    void RemoveBundle(const std::string &bundleName);
    void UpdateBundle(const std::string &bundleName);

    ObjectEditorManagerErrCode GetObjectEditorFormatByHmid(const std::string &hmid,
        std::unique_ptr<ObjectEditorFormat> &format) const;
    ObjectEditorManagerErrCode GetObjectEditorFormatByHmidAndMinVersion(const std::string &hmid,
        const std::string &minVersion, std::unique_ptr<ObjectEditorFormat> &format) const;
    ObjectEditorManagerErrCode GetObjectEditorFormatByHmidAndLocale(const std::string &hmid,
        const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format) const;
    ObjectEditorManagerErrCode GetObjectEditorFormatsByLocale(const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) const;
    ObjectEditorManagerErrCode GetObjectEditorFormatsByFileExt(const std::string &fileExt,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) const;

private:
    ObjectEditorManagerDatabase();
    ~ObjectEditorManagerDatabase();
    bool OpenDb();
    bool DeleteDb();
    bool CreateDefaultTable();
    bool InitSubscriber();
    bool ExecuteTransactionSql(const std::vector<std::string> &sqlList);

    ObjectEditorManagerErrCode GetBundleInfoValuesBuckets(const std::string &bundleName,
        std::vector<NativeRdb::ValuesBucket> &buckets) const;
    bool DoInsert(const std::vector<NativeRdb::ValuesBucket> &buckets);
    bool DoDeleteBundle(const std::string &bundleName);
    ObjectEditorManagerErrCode QueryBySql(const std::string &sql,
        std::shared_ptr<NativeRdb::AbsSharedResultSet> &pResultSet,
        const std::vector<NativeRdb::ValueObject> &whereArgs = {}) const;
    std::map<std::string, int64_t> GetBundleNameAndCreateTime() const;
    void ParseExtensionInfos(const std::map<std::string, int64_t> &dbBundles,
        std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos,
        std::vector<NativeRdb::ValuesBucket> &buckets, std::set<std::string> &oldBundles) const;
    ObjectEditorManagerErrCode PrepareRefreshDb(std::vector<NativeRdb::ValuesBucket> &buckets,
        std::set<std::string> &oldBundles) const;
    ObjectEditorManagerErrCode RefreshDb();

private:
    class DbPackageSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        explicit DbPackageSubscriber(const EventFwk::CommonEventSubscribeInfo &subscribeInfo)
            : EventFwk::CommonEventSubscriber(subscribeInfo) {}
        ~DbPackageSubscriber() override = default;
        void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;
    };

    std::shared_ptr<OHOS::NativeRdb::RdbStore> store_;
    const std::string dbDir_;
    const std::string dbPath_;
    sptr<AppExecFwk::IBundleMgr> bundleMgr_;
    std::shared_ptr<DbPackageSubscriber> dbPackageSubscriber_;
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_DATABASE_H