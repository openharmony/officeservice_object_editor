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

#include "object_editor_client_callback.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
ObjectEditorClientCallback::ObjectEditorClientCallback(struct ContentEmbed_ExtensionProxy *proxy): proxy_(proxy)
{
}

int32_t ObjectEditorClientCallback::CallbackEnter(uint32_t code)
{
    return 0;
}

int32_t ObjectEditorClientCallback::CallbackExit(uint32_t code, int32_t result)
{
    return result;
}

ErrCode ObjectEditorClientCallback::OnUpdate(std::unique_ptr<ObjectEditorDocument> &document)
{
    if (proxy_ == nullptr ||
        proxy_->ceDocument == nullptr ||
        proxy_->onUpdateFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnUpdateEditResult, proxy_ or document is null");
        return ERR_INVALID_VALUE;
    }
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnUpdateEditResult, document is null");
        return ERR_INVALID_VALUE;
    }
    document->RestoreStorage();
    proxy_->ceDocument->hmid = document->GetHmid();
    proxy_->ceDocument->oeDocumentInner = std::move(document);
    proxy_->onUpdateFunc(proxy_);
    return ERR_OK;
}

ErrCode ObjectEditorClientCallback::OnError(ContentEmbed_ErrorCode error)
{
    if (proxy_ == nullptr || proxy_->onErrorFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnError, proxy_ or onErrorFunc is null");
        return ERR_INVALID_VALUE;
    }
    proxy_->onErrorFunc(proxy_, error);
    return ERR_OK;
}


ErrCode ObjectEditorClientCallback::OnStopEdit(bool isModified)
{
    if (proxy_ == nullptr || proxy_->onEditingFinishedFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnStopEdit, proxy_ or onStopEditFunc is null");
        return ERR_INVALID_VALUE;
    }
    if (proxy_->ceDocument != nullptr && proxy_->ceDocument->oeDocumentInner != nullptr) {
        proxy_->ceDocument->oeDocumentInner->RestoreStorage();
    }
    proxy_->onEditingFinishedFunc(proxy_, isModified);
    return ERR_OK;
}

ErrCode ObjectEditorClientCallback::OnExtensionStopped()
{
    if (proxy_ == nullptr || proxy_->onExtensionStoppedFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnExtensionStopped, proxy_ or onExtensionStoppedFunc is null");
        return ERR_INVALID_VALUE;
    }
    proxy_->onExtensionStoppedFunc(proxy_);
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS