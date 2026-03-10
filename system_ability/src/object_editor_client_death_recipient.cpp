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

#include "object_editor_client_death_recipient.h"
#include "object_editor_manager_system_ability.h"

#include "object_editor_connection.h"

namespace OHOS {
namespace ObjectEditor {

void ObjectEditorClientDeathRecipient::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "documentId:%{public}s", documentId_.c_str());
    ObjectEditorManagerSystemAbility::GetInstance().StopObjectEditorExtension(documentId_,
        oeExtensionRemoteObject_, false);
}

} // namespace ObjectEditor
} // namespace OHOS
