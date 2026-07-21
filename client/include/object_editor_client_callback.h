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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_H

#include <atomic>
#include "object_editor_client_callback_stub.h"
#include "native_object_editor_types.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorClientCallback : public ObjectEditorClientCallbackStub {
public:
    explicit ObjectEditorClientCallback(struct ContentEmbed_ExtensionProxy *proxy);
    ~ObjectEditorClientCallback();
    int32_t CallbackEnter(uint32_t code) override;
    int32_t CallbackExit(uint32_t code, int32_t result) override;

    ErrCode OnUpdate(std::unique_ptr<ObjectEditorDocument> &document) override;
    ErrCode OnError(ContentEmbed_ErrorCode error) override;
    ErrCode OnStopEdit(bool dataModified) override;
    ErrCode OnExtensionStopped() override;

    // Clear the raw proxy_ pointer to prevent use-after-free when
    // DestroyExtensionProxy frees the proxy struct before in-flight
    // OnExtensionStopped/OnUpdate IPC is dispatched. After calling this,
    // OnXxx methods will hit the proxy_ == nullptr guard and return safely.
    void ClearProxy() { proxy_ = nullptr; }

    // Dispatch flag: true while a user C callback (onUpdateFunc etc.) is on
    // the call stack. PrepareForDestroy checks this under callbackMutex_ to
    // reject DestroyExtensionProxy during dispatch (UAF/CFI guard).
    // Encapsulated here instead of in the struct to keep the dispatch
    // mechanism private to the client layer.
    bool IsDispatching() const { return isDispatching_.load(std::memory_order_acquire); }

private:
    struct ContentEmbed_ExtensionProxy *proxy_ {nullptr};
    std::atomic<bool> isDispatching_ { false };
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_H