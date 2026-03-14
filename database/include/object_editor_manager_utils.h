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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_UTILS_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_UTILS_H

#include <map>
#include <string>
#include <utility>
#include <vector>

#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "extension_ability_info.h"
#include "image_source.h"
#include "rdb_store.h"

#include "object_editor_err_code.h"
#include "object_editor_format.h"

namespace OHOS {
namespace ObjectEditor {
sptr<AppExecFwk::IBundleMgr> GetBundleMgr();
void GetBundleInfos(const sptr<AppExecFwk::IBundleMgr> &bundleMgr,
    const std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos,
    std::map<std::string, AppExecFwk::BundleInfo> &bundleInfos);
bool BuildObjectEditorFormat(ObjectEditorFormat &format, NativeRdb::RowEntity &rowEntity,
    const std::string &locale);
bool BuildValuesBuckets(std::vector<NativeRdb::ValuesBucket> &buckets, const AppExecFwk::BundleInfo &bundleInfo,
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo);
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_UTILS_H