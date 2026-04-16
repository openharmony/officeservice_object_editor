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

#include "object_editor_manager_stub.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
int32_t ObjectEditorManagerStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int32_t ret = CallbackEnter(code);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "CallbackEnter ret: %{public}d, code: %{public}u",
            ret, code);
        return ret;
    }
    ret = OnRemoteRequestInner(code, data, reply, option);
    ret = CallbackExit(code, ret);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "CallbackExit ret: %{public}d, code: %{public}u",
            ret, code);
    }
    return ret;
}

int32_t ObjectEditorManagerStub::OnRemoteRequestInner(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "descriptor invalid");
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IObjectEditorManagerIpcCode>(code)) {
        case IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION:
            return HandleStartObjectEditorExtension(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION:
            return HandleGetOEidByFileExtension(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID:
            return HandleGetIconByOEid(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE:
            return HandleGetObjectEditorFormatByOEidAndLocale(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE:
            return HandleGetObjectEditorFormatsByLocale(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY:
            return HandleStartUIAbility(data, reply);
        case IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION:
            return HandleStopObjectEditorExtension(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleStartObjectEditorExtension(MessageParcel &data,
    MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::unique_ptr<ObjectEditorDocument> document =
        std::unique_ptr<ObjectEditorDocument>(data.ReadParcelable<ObjectEditorDocument>());
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "document is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> clientCallbackRemoteObject = data.ReadRemoteObject();
    if (clientCallbackRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "clientCallbackRemoteObject is nullptr");
        return ERR_INVALID_DATA;
    }
    const sptr<IObjectEditorClientCallback> clientCallback =
        iface_cast<IObjectEditorClientCallback>(clientCallbackRemoteObject);
    if (clientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "clientCallback is nullptr");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> oeExtensionRemoteObject = nullptr;
    bool isPackageExtension = false;
    ErrCode errCode = StartObjectEditorExtension(document, clientCallback,
        oeExtensionRemoteObject, isPackageExtension);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    if (!reply.WriteBool(isPackageExtension)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write isPackageExtension failed");
        return ERR_INVALID_DATA;
    }
    if (!isPackageExtension && !reply.WriteRemoteObject(oeExtensionRemoteObject)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write oeExtensionRemoteObject failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleStopObjectEditorExtension(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::string documentId = data.ReadString();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "documentId is empty");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> extensionRemoteObject = data.ReadRemoteObject();
    if (extensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "extensionRemoteObject is nullptr");
        return ERR_INVALID_DATA;
    }
    bool isPackageExtension = data.ReadBool();
    ErrCode errCode = StopObjectEditorExtension(documentId, extensionRemoteObject, isPackageExtension);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleGetOEidByFileExtension(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::string oeid = Str16ToStr8(data.ReadString16());
    std::string fileExtension;
    ErrCode errCode = GetOEidByFileExtension(oeid, fileExtension);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    if (!reply.WriteString16(Str8ToStr16(fileExtension))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write fileExtension failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleGetIconByOEid(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::string oeid = Str16ToStr8(data.ReadString16());
    std::string resourceId;
    ErrCode errCode = GetIconByOEid(oeid, resourceId);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    if (!reply.WriteString16(Str8ToStr16(resourceId))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write resourceId failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleGetObjectEditorFormatByOEidAndLocale(MessageParcel &data,
    MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::string oeid = Str16ToStr8(data.ReadString16());
    std::string locale = Str16ToStr8(data.ReadString16());
    std::unique_ptr<ObjectEditorFormat> format = nullptr;
    ErrCode errCode = GetObjectEditorFormatByOEidAndLocale(oeid, locale, format);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    if (!format || !reply.WriteParcelable(format.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write format failed");
        return ERR_INVALID_DATA;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleGetObjectEditorFormatsByLocale(MessageParcel &data,
    MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::string locale = Str16ToStr8(data.ReadString16());
    std::vector<std::unique_ptr<ObjectEditorFormat>> formats;
    ErrCode errCode = GetObjectEditorFormatsByLocale(locale, formats);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    if (formats.size() > static_cast<size_t>(VECTOR_MAX_SIZE)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "formats size exceed limit");
        return ERR_INVALID_DATA;
    }
    reply.WriteInt32(formats.size());
    for (auto &format : formats) {
        if (!reply.WriteParcelable(format.get())) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write format failed");
            return ERR_INVALID_DATA;
        }
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerStub::HandleStartUIAbility(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::SA, "call");
    std::unique_ptr<AAFwk::Want> want = std::unique_ptr<AAFwk::Want>(data.ReadParcelable<AAFwk::Want>());
    if (!want) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "want is nullptr");
        return ERR_INVALID_DATA;
    }
    int32_t clientPid = data.ReadInt32();
    if (clientPid <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "clientPid is invalid");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> extensionToken = data.ReadRemoteObject();
    if (extensionToken == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "extensionToken is nullptr");
        return ERR_INVALID_DATA;
    }
    ErrCode errCode = StartUIAbility(want, extensionToken, clientPid);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    if (!SUCCEEDED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "errCode is failed");
        return errCode;
    }
    return ERR_NONE;
}

// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
