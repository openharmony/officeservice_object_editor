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

int32_t ObjectEditorClientCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t ret = CallbackEnter(code);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnRemoteRequest, CallbackEnter failed, code = %{public}u, ret = %{public}d", code, ret);
        return ret;
    }
    ret = OnRemoteRequestInner(code, data, reply, option);
    ret = CallbackExit(code, ret);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnRemoteRequest, CallbackExit failed, code = %{public}u, ret = %{public}d", code, ret);
        return ret;
    }
    return ret;
}

int32_t ObjectEditorClientCallbackStub::OnRemoteRequestInner(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "OnRemoteRequestInner, invalid descriptor");
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IObjectEditorClientCallbackIpcCode>)
    {
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
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT,"call");
    std::unique_ptr<ObjectEditorDocument> document = data.ReadParcelable<ObjectEditorDocument>();
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnUpdate, document is null");
        return ERR_INVALID_DATA;
    }
    ErrCode ret = OnUpdate(document);
    if (!reply.WriteInt32(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnUpdate, OnUpdate failed, ret = %{public}d", ret);
        return ret;
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnError(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT,"call");
    ContentEmbed_ErrorCode errorCode = static_cast<ContentEmbed_ErrorCode>(data.ReadInt32());
    ErrCode ret = OnError(errorCode);
    if (!reply.WriteInt32(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnError, OnError failed, ret = %{public}d", ret);
        return ERR_INVALID_VALUE;
    }
    if(SUCCEEDED(ret)){
        if (!reply.WriteInt32(static_cast<int32_t>(errorCode))) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnError, OnError failed, ret = %{public}d", ret);
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnStopEdit(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT,"call");
    bool isModified = data.ReadBool();
    ErrCode ret = OnStopEdit(isModified);
    if (!reply.WriteInt32(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnStopEdit, OnStopEdit failed, ret = %{public}d", ret);
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorClientCallbackStub::HandleOnExtensionStopped(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT,"call");
    ErrCode ret = OnExtensionStopped();
    if (!reply.WriteInt32(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "HandleOnExtensionStopped, OnExtensionStopped failed, ret = %{public}d", ret);
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}
} // namespace ObjectEditor
} // namespace OHOS
