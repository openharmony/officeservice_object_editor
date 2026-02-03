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

#include "object_editor_client_callback_proxy.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START

ErrCode ObjectEditorClientCallbackProxy::onUpdate(std::unique_ptr<ObjectEditorDocument> &document)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(document.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write document failed");
        return ERR_INVALID_DATA;
    }
    sptr<IObjectEditorClientCallback> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest( static_cast<uint32_t>(IObjectEditorClientCallback::COMMAND_ON_UPDATE),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "reply failed");
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClientCallbackProxy::onError(ContentEmbed_ErrorCode error)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInt32(static_cast<int32_t>(error))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write error failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IObjectEditorClientCallback> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest( static_cast<uint32_t>(IObjectEditorClientCallback::COMMAND_ON_ERROR),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "reply failed");
        return errCode;
    }
    int32_t enumTmp = 0;
    if (!reply.ReadInt32(enumTmp)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "read enum failed");
        return ERR_INVALID_DATA;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClientCallbackProxy::onStopEdit(bool dataModified)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteBool(dataModified)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write dataModified failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IObjectEditorClientCallback> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorClientCallback::COMMAND_ON_STOP_EDIT),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "reply failed");
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClientCallbackProxy::OnExtensionStoped()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IObjectEditorClientCallback> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorClientCallback::COMMAND_ON_EXTENSION_STOPED),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "reply failed");
        return errCode;
    }
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
