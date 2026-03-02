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

#include "object_editor_extension_proxy.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
ErrCode ObjectEditorExtensionProxy::RegisterClientCB(
    const sptr<ObjectEditorClientCallback> &clientCallback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (clientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "clientCallback is null");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteRemoteObject(clientCallback->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteRemoteObject clientCallback failed");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_REGISTER_CLIENT_CB), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetSnapshot()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::DoEdit()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_DO_EDIT), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetEditStatus(bool *isEditing, bool *isModified)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (isEditing == nullptr || isModified == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "isEditing or isModified is null");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_EDITING_STATE), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    *isEditing = reply.ReadBool();
    *isModified = reply.ReadBool();
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetCapability(uint32_t *capability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (capability == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "capability is null");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_CAPABILITY), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    *capability = reply.ReadUInt32();
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::Close()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_CLOSE), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::Initial(std::unique_ptr<ObjectEditorDocument> document)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!document || !data.WriteParcelable(document.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteParcelable failed");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_INITIAL), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
