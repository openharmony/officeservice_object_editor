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
ErrCode ObjectEditorExtensionProxy::GetSnapshot(const std::string &documentId)
{
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write documentId fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::DoEdit(const std::string &documentId)
{
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write documentId fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_DO_EDIT), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified)
{
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (isEditing == nullptr || isModified == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "isEditing or isModified is null");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write documentId fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_EDITING_STATE), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    if (!reply.ReadBool(*isEditing)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read isEditing failed");
        return ERR_INVALID_DATA;
    }
    if (!reply.ReadBool(*isModified)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read isModified failed");
        return ERR_INVALID_DATA;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetExtensionEditStatus(bool &isEditing)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_EXTENSION_EDITING_STATUS), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    if (!reply.ReadBool(isEditing)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read isEditing failed");
        return ERR_INVALID_DATA;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::GetCapability(const std::string &documentId, uint32_t *bitmask)
{
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (bitmask == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "bitmask is null");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write documentId fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_CAPABILITY), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    if (!reply.ReadUint32(*bitmask)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read bitmask failed");
        return ERR_INVALID_DATA;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::Close(const std::string &documentId, bool &isAllObjectsRemoved,
    uint32_t callerTokenId)
{
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write documentId fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteUint32(callerTokenId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write callerTokenId fail");
        return ERR_INVALID_VALUE;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_CLOSE), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    if (!reply.ReadBool(isAllObjectsRemoved)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read isAllObjectsRemoved failed");
        return ERR_INVALID_DATA;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtensionProxy::Initial(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCallback, uint32_t callerTokenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!document || !data.WriteParcelable(document.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write document fail");
        return ERR_INVALID_DATA;
    }
    if (clientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "clientCallback is null");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteRemoteObject(clientCallback->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write clientCallback fail");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteUint32(callerTokenId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write callerTokenId fail");
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remote is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_INITIAL), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "send request failed");
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "reply failed");
        return errCode;
    }
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
