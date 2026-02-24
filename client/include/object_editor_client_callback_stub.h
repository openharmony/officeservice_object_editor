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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_STUB_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_STUB_H

#include <iremote_stub.h>
#include "iobject_editor_client_callback.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorClientCallbackStub : public IRemoteStub<IObjectEditorClientCallback> {
public:
    ObjectEditorClientCallbackStub(bool serialInvokeFlag = false) : IRemoteStub(serialInvokeFlag){};
    ~ObjectEditorClientCallbackStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    virtual int32_t CallbackEnter([[maybe_unused]] uint32_t code);
    virtual int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] uint32_t result);
    int32_t OnRemoteRequestInner(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option);
private:
    int32_t HandleOnUpdate(MessageParcel &data, MessageParcel &reply);
    int32_t HandleOnError(MessageParcel &data, MessageParcel &reply);
    int32_t HandleOnStopEdit(MessageParcel &data, MessageParcel &reply);
    int32_t HandleOnExtensionStopped(MessageParcel &data, MessageParcel &reply);
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_CALLBACK_STUB_H
