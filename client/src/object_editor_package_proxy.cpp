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

#include "object_editor_package_proxy.h"

namespace OHOS {
namespace ObjectEditor {

ErrCode ObjectEditorPackageProxy::RegisterClientCB(const sptr<IObjectEditorClientCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (callback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "callback is null");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteRemoteObject failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remoteObject = reply.ReadRemoteObject();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "ReadRemoteObject failed");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_REGISTER_CLIENT_CB), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "RegisterClientCB failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackageProxy::GetSnapshot()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "GetSnapshot failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackageProxy::DoEdit()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<int32_t>(IObjectEditorServiceIpcCode::COMMAND_DO_EDIT), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "DoEdit failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackageProxy::GetEditStatus(bool *isEditing, bool *isModified)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_EDIT_STATUS), data, reply, option);
    if(FAILED(ret)){
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "GetEditStatus failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackageProxy::GetCapability(uint32_t *capability)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_CAPABILITY), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {  
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "GetCapability failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackageProxy::Close()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_CLOSE), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {      
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "Close failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;  
}

ErrCode ObjectEditorPackageProxy::Initial(std::unique_ptr<ObjectEditorDocument> &document)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "remoteObject is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_INITIAL), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "Initial failed, errCode: %{public}d", errCode);
        return errCode;
    }
    return ERR_OK;  
}
} // namespace ObjectEditor
} // namespace OHOS
