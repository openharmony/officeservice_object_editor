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

#include <ipc_skeleton.h>
#include <accesstoken_kit.h>
#include <tokenid_kit.h>
#include <bundle_constants.h>
#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "object_editor_permission_utils.h"
#include "hilog_object_editor.h"
#include "user_mgr.h"
#include "object_editor_bms_utils.h"

namespace OHOS {
namespace ObjectEditor {
bool ObjectEditorPermissionUtils::CheckCallingPermission(const std::string &permission)
{
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission) !=
        AppExecFwk::Constants::PERMISSION_GRANTED) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "permission:%{public}s denied!", permission.c_str());
        return false;
    }
    return true;
}

bool ObjectEditorPermissionUtils::CheckRequestPermission(const std::string &bundleName, const std::string &permission)
{
    auto bundleMgr = ObjectEditorBmsUtils::GetBundleMgr();
    if (bundleMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "null bundleMgr");
        return false;
    }
    AppExecFwk::BundleInfo bundleInfo;
    ErrCode errCode = bundleMgr->GetBundleInfoV9(bundleName,
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_REQUESTED_PERMISSION),
        bundleInfo, UserMgr::GetInstance().GetUserId());
    if (errCode != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "GetBundleInfoV9 fail,bundleName:%{public}s,errCode:%{public}d",
            bundleName.c_str(), errCode);
        return false;
    }
    std::vector<std::string> permissions = bundleInfo.reqPermissions;
    if (std::find(permissions.begin(), permissions.end(), permission) != permissions.end()) {
        return true;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::COMMON,
        "permission denied, bundleName:%{public}s register", bundleName.c_str());
    return false;
}

} // namespace ObjectEditor
} // namespace OHOS