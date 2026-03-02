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

#include "object_editor_client_callback_stub.h"
#include "native_object_editor_types.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorClientCallback : public ObjectEditorClientCallbackStub {
public:
    explicit ObjectEditorClientCallback(struct ContentEmbed_ExtensionProxy *proxy);
    ~ObjectEditorClientCallback() = default;
    int32_t CallbackEnter(uint32_t code) override;
    int32_t CallbackExit(uint32_t code, int32_t result) override;

    ErrCode OnUpdate(std::unique_ptr<ObjectEditorDocument> &document) override;
    ErrCode OnError(ContentEmbed_ErrorCode error) override;
    ErrCode OnStopEdit(bool dataModified) override;
    ErrCode OnExtensionStopped() override;

private:
    struct ContentEmbed_ExtensionProxy *proxy_ {nullptr};
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_H