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

#include "object_editor_client_callback_stub.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
int32_t ObjectEditorClientCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t ret = CallbackEnter(code);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
            "CallbackEnter failed, code = %{public}u, ret = %{public}d", code, ret);
        return ret;
    }
    ret = OnRemoteRequestInner(code, data, reply, option);
    ret = CallbackExit(code, ret);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
            "CallbackExit failed, code = %{public}u, ret = %{public}d", code, ret);
        return ret;
    }
    return ret;
}

int32_t ObjectEditorClientCallbackStub::OnRemoteRequestInner(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "descriptor invalid");
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IObjectEditorClientCallbackIpcCode>(code)) {
        case IObjectEditorClientCallbackIpcCode::COMMAND_ON_UPDATE:
            return HandleOnUpdate(data, reply);
        case IObjectEditorClientCallbackIpcCode::COMMAND_ON_ERROR:
            return HandleOnError(data, reply);
        case IObjectEditorClientCallbackIpcCode::COMMAND_ON_STOP_EDIT:
            return HandleOnStopEdit(data, reply);
        case IObjectEditorClientCallbackIpcCode::COMMAND_ON_EXTENSION_STOPPED:
            return HandleOnExtensionStopped(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_TRANSACTION_FAILED;
}

int32_t ObjectEditorClientCallbackStub::HandleOnUpdate(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "call");
    std::unique_ptr<ObjectEditorDocument> document = std::unique_ptr<ObjectEditorDocument>(
        data.ReadParcelable<ObjectEditorDocument>());
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document is null");
        return ERR_INVALID_DATA;
    }
    ErrCode errCode = OnUpdate(document);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnError(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "call");
    ContentEmbed_ErrorCode error = static_cast<ContentEmbed_ErrorCode>(data.ReadInt32());
    ErrCode errCode = OnError(error);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (SUCCEEDED(errCode)) {
        if (!reply.WriteInt32(static_cast<int32_t>(error))) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write error failed");
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnStopEdit(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "call");
    bool dataModified = data.ReadBool();
    ErrCode errCode = OnStopEdit(dataModified);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnExtensionStopped(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "call");
    ErrCode errCode = OnExtensionStopped();
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
