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

#include "object_editor_extension_death_recipient.h"
#include "object_editor_client.h"

namespace OHOS {
namespace ObjectEditor {
ObjectEditorExtensionDeathRecipient::ObjectEditorExtensionDeathRecipient(
    struct ContentEmbed_ExtensionProxy *proxy) : proxy_(proxy)
{
}

ObjectEditorExtensionDeathRecipient::~ObjectEditorExtensionDeathRecipient()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "destructor");
}

void ObjectEditorExtensionDeathRecipient::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGW(ObjectEditorDomain::CLIENT, "extension remote died");
    if (proxy_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy is null");
        return;
    }
    proxy_->onErrorFunc(proxy_, ContentEmbed_ErrorCode::CE_ERR_EXTENSION_ABNORMAL_EXIT);
}

} // namespace ObjectEditor
} // namespace OHOS
