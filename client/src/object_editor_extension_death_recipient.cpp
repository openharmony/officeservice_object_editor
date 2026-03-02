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
    struct ContentEmbed_ExtensionProxy *proxy): proxy_(proxy)
{
}

void ObjectEditorExtensionDeathRecipient::OnRemoteDied(const OHOS::wptr<OHOS::IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGW(ObjectEditorDomain::CLIENT, "extension remote died");
    if (proxy_) {
        // 通知SA清理资源
        ErrCode ret = ObjectEditorClient::GetInstance().StopObjectEditorExtension(
            proxy_->objectEditorService, proxy_->isPackageExtension);
        if (ret != OHOS::ERR_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
                "StopObjectEditorExtension failed, ret = %{public}d", ret);
        }
        proxy_->onErrorFunc(proxy_, ContentEmbed_ErrorCode::CE_ERR_EXTENSION_ABNORMAL_EXIT);
    } else {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "client proxy is already nullptr");
    }
}

} // namespace ObjectEditor
} // namespace OHOS
