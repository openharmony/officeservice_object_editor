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

#include "bundle_constants.h"
#include "bundle_info.h"
#include "bundle_mgr_interface.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "hilog_object_editor.h"
#include "object_editor_bms_utils.h"
#include "user_mgr.h"

namespace OHOS {
namespace ObjectEditor {

bool ObjectEditorBmsUtils::QueryAbilityInfo(const AAFwk::Want &want, AppExecFwk::AbilityInfo &abilityInfo)
{
    auto bundleMgr = GetBundleMgr();
    if (bundleMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "bundleMgr is nullptr");
        return false;
    }
    auto abilityInfoFlag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION;
    bool ret = bundleMgr->QueryAbilityInfo(want, abilityInfo, abilityInfoFlag);
    return ret;
}

} // namespace OHOS
} // namespace ObjectEditor
