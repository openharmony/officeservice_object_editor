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

#include "object_editor_config.h"
#include "hilog_object_editor.h"
#include "parameters.h"
#include "single_instance.h"
#ifdef WITH_DLP
#include "dlp_permission_kit.h"
#include "dlp_permission.h"
#endif

namespace OHOS {
namespace ObjectEditor {
namespace {
constexpr const char *IS_SUPPORT_OBJECT_EDITOR = "const.object_editor.object_editor_enable";
}
IMPLEMENT_SINGLE_INSTANCE(ObjectEditorConfig);

bool ObjectEditorConfig::IsSupportObjectEditor()
{
    if (!isSupportObjectEditor_.isLoaded) {
        isSupportObjectEditor_.value = system::GetBoolParameter(IS_SUPPORT_OBJECT_EDITOR, false);
        isSupportObjectEditor_.isLoaded = true;
        OBJECT_EDITOR_HILOGI("IsSupportObjectEditor: %{public}d", isSupportObjectEditor_.value);
    }
    return isSupportObjectEditor_.value;
}

bool ObjectEditorConfig::CheckIsInDlp()
{
#ifdef WITH_DLP
    bool isInDlpSandbox = false;
    int32_t ret = OHOS::Security::DlpPermission::DlpPermissionKit::IsInDlpSandbox(isInDlpSandbox);
    if (ret != OHOS::Security::DlpPermission::DLP_OK) {
        OBJECT_EDITOR_HILOGE("CheckIsInDlpSandbox failed, ret: %{public}d", ret);
        return false;
    }
    if (isInDlpSandbox) {
        OBJECT_EDITOR_HILOGW("Application is in DLP sandbox");
    }
    return isInDlpSandbox;
#else
    return false;
#endif
}
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONFIG_H

