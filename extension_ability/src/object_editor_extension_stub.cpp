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

#include "object_editor_extension_stub.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
int32_t ObjectEditorExtensionStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t ret = CallbackEnter(code);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "CallbackEnter ret: %{public}d, code: %{public}u",
            ret, code);
        return ret;
    }
    ret = OnRemoteRequestInner(code, data, reply, option);
    ret = CallbackExit(code, ret);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "CallbackExit ret: %{public}d, code: %{public}u",
            ret, code);
    }
    return ret;
}

int32_t ObjectEditorExtensionStub::OnRemoteRequestInner(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "descriptor invalid");
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IObjectEditorServiceIpcCode>(code)) {
        case IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT:
            return HandleExtensionGetSnapshot(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_DO_EDIT:
            return HandleExtensionDoEdit(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_INITIAL:
            return HandleExtensionInitial(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_GET_CAPABILITY:
            return HandleExtensionGetCapability(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_GET_EDITING_STATE:
            return HandleExtensionGetEditStatus(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_CLOSE:
            return HandleExtensionClose(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_GET_EXTENSION_EDITING_STATUS:
            return HandleExtensionClose(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionGetSnapshot(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read documentId failed");
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = GetSnapshot(documentId);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionGetCapability(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read documentId failed");
        return ERR_INVALID_VALUE;
    }
    uint32_t bitmask = 0;
    ErrCode errCode = GetCapability(documentId, &bitmask);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteUint32(bitmask)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write bitmask failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionGetEditStatus(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read documentId failed");
        return ERR_INVALID_VALUE;
    }
    bool isEditing = false;
    bool isModified = false;
    ErrCode errCode = GetEditStatus(documentId, &isEditing, &isModified);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteBool(isEditing)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write isEditing failed");
        return ERR_INVALID_DATA;
    }
    if (!reply.WriteBool(isModified)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write isModified failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionGetExtensionEditStatus(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    bool isEditing = false;
    ErrCode errCode = GetExtensionEditStatus(isEditing);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteBool(isEditing)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write isEditing failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionDoEdit(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read documentId failed");
        return ERR_INVALID_VALUE;
    }
    ErrCode errCode = DoEdit(documentId);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionInitial(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::unique_ptr<ObjectEditorDocument> document =
        std::unique_ptr<ObjectEditorDocument>(data.ReadParcelable<ObjectEditorDocument>());
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "document is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<IObjectEditorClientCallback> clientCb = iface_cast<IObjectEditorClientCallback>(
        data.ReadRemoteObject());
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "clientCb is nullptr");
        return ERR_INVALID_DATA;
    }
    ErrCode errCode = Initial(std::move(document), clientCb);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorExtensionStub::HandleExtensionClose(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "read documentId failed");
        return ERR_INVALID_VALUE;
    }
    bool isAllObjectsRemoved = false;
    ErrCode errCode = Close(documentId, isAllObjectsRemoved);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!reply.WriteBool(isAllObjectsRemoved)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "write isAllObjectsRemoved failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
