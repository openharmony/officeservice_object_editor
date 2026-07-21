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
#include "object_editor_client.h"
#include <memory>
#include <mutex>

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
// RAII guard for isDispatching_ flag. Sets flag to true on construction,
// resets to false on destruction (even if user callback throws/longjmps).
// Paired with ObjectEditorClient::PrepareForDestroy dispatch check.
// The flag is encapsulated in ObjectEditorClientCallback,
// to keep the dispatch mechanism private to the client layer.
class DispatchFlagGuard {
public:
    explicit DispatchFlagGuard(std::atomic<bool> &flag) : flag_(flag)
    {
        flag_.store(true, std::memory_order_release);
    }
    ~DispatchFlagGuard()
    {
        flag_.store(false, std::memory_order_release);
    }
private:
    std::atomic<bool> &flag_;
};
} // namespace

ObjectEditorClientCallback::ObjectEditorClientCallback(struct ContentEmbed_ExtensionProxy *proxy) : proxy_(proxy)
{
}

ObjectEditorClientCallback::~ObjectEditorClientCallback()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "destructor");
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
    struct ContentEmbed_ExtensionProxy *proxy = nullptr;
    OH_ContentEmbed_ClientCallbackOnUpdateFunc func = nullptr;
    ContentEmbed_Document *ceDoc = nullptr;
    std::unique_ptr<DispatchFlagGuard> guard;
    {
        auto lock = ObjectEditorClient::GetInstance().AcquireCallbackLock();
        proxy = proxy_;
        if (proxy == nullptr || proxy->ceDocument == nullptr || proxy->onUpdateFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy invalid");
            return ERR_INVALID_VALUE;
        }
        if (document == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document is null");
            return ERR_INVALID_VALUE;
        }
        func = proxy->onUpdateFunc;
        ceDoc = proxy->ceDocument;
        guard = std::make_unique<DispatchFlagGuard>(isDispatching_);
    }
    // Document operations outside the lock — isDispatching_=true keeps proxy
    // alive (PrepareForDestroy rejected). Avoids self-deadlock if
    // RestoreStorage/GetOEid indirectly call back into ObjectEditorClient.
    document->RestoreStorage();
    ceDoc->oeid = document->GetOEid();
    ceDoc->oeDocumentInner = std::move(document);
    func(proxy);
    return ERR_OK;
}

ErrCode ObjectEditorClientCallback::OnError(ContentEmbed_ErrorCode error)
{
    struct ContentEmbed_ExtensionProxy *proxy = nullptr;
    OH_ContentEmbed_ClientCallbackOnErrorFunc func = nullptr;
    std::unique_ptr<DispatchFlagGuard> guard;
    {
        auto lock = ObjectEditorClient::GetInstance().AcquireCallbackLock();
        proxy = proxy_;
        if (proxy == nullptr || proxy->onErrorFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy invalid");
            return ERR_INVALID_VALUE;
        }
        func = proxy->onErrorFunc;
        guard = std::make_unique<DispatchFlagGuard>(isDispatching_);
    }
    func(proxy, error);
    return ERR_OK;
}

ErrCode ObjectEditorClientCallback::OnStopEdit(bool dataModified)
{
    struct ContentEmbed_ExtensionProxy *proxy = nullptr;
    OH_ContentEmbed_ClientCallbackOnEditingFinishedFunc func = nullptr;
    ObjectEditorDocument *oeDocInner = nullptr;
    std::unique_ptr<DispatchFlagGuard> guard;
    {
        auto lock = ObjectEditorClient::GetInstance().AcquireCallbackLock();
        proxy = proxy_;
        if (proxy == nullptr || proxy->onEditingFinishedFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy invalid");
            return ERR_INVALID_VALUE;
        }
        if (proxy->ceDocument != nullptr && proxy->ceDocument->oeDocumentInner != nullptr) {
            oeDocInner = proxy->ceDocument->oeDocumentInner.get();
        }
        func = proxy->onEditingFinishedFunc;
        guard = std::make_unique<DispatchFlagGuard>(isDispatching_);
    }
    // RestoreStorage outside the lock — isDispatching_=true keeps proxy alive.
    // Avoids self-deadlock if RestoreStorage indirectly calls back into ObjectEditorClient.
    if (oeDocInner != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "restore storage");
        oeDocInner->RestoreStorage();
    }
    func(proxy, dataModified);
    return ERR_OK;
}

ErrCode ObjectEditorClientCallback::OnExtensionStopped()
{
    struct ContentEmbed_ExtensionProxy *proxy = nullptr;
    OH_ContentEmbed_ClientCallbackOnExtensionStoppedFunc func = nullptr;
    std::unique_ptr<DispatchFlagGuard> guard;
    {
        auto lock = ObjectEditorClient::GetInstance().AcquireCallbackLock();
        proxy = proxy_;
        if (proxy == nullptr || proxy->onExtensionStoppedFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy invalid");
            return ERR_INVALID_VALUE;
        }
        func = proxy->onExtensionStoppedFunc;
        guard = std::make_unique<DispatchFlagGuard>(isDispatching_);
    }
    // isDispatching_=true; PrepareForDestroy rejected (checks under same lock).
    func(proxy);
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS