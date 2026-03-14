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

#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <tokenid_kit.h>

#include "hilog_object_editor.h"
#include "object_editor_permission_utils.h"

namespace OHOS {
namespace ObjectEditor {
bool ObjectEditorPermissionUtils::CheckCallingPermission(const std::string &permission)
{
    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission) !=
        AppExecFwk::Constants::PERMISSION_GRANTED) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "permission:%{public}s denied", permission.c_str());
        return false;
    }
    return true;
}

} // namespace ObjectEditor
} // namespace OHOS