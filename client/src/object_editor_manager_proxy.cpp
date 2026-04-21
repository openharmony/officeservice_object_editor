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

#include "object_editor_manager_proxy.h"

#include <memory>
#include <vector>

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
ErrCode ObjectEditorManagerProxy::StartObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject,
    bool &isPackageExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (document == nullptr || objectEditorClientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document or objectEditorClientCallback is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(document.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write document fail");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteRemoteObject(objectEditorClientCallback->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write remote object fail");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    isPackageExtension = reply.ReadBool();
    if (!isPackageExtension) {
        oeExtensionRemoteObject = reply.ReadRemoteObject();
    }
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "succeed");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::StopObjectEditorExtension(
    const std::string &documentId,
    const sptr<IRemoteObject> &oeExtensionRemoteObject,
    const bool &isPackageExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString(documentId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write documentId failed");
        return ERR_INVALID_VALUE;
    }
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(oeExtensionRemoteObject)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write remote object fail");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteBool(isPackageExtension)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write isPackageExtension failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "succeed");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetOEidByFileExtension(
    const std::string &oeid,
    std::string &fileExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(oeid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write oeid failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_OEID_BY_FILE_EXTENSION),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    fileExtension = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetIconByOEid(
    const std::string &oeid,
    std::string &resFilePath)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(oeid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write oeid failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_OEID),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    resFilePath = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetFormatName(
    const std::string &oeid,
    const std::string &locale,
    std::string &formatName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(oeid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write oeid failed");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteString(locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write locale failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_NAME),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    formatName = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetObjectEditorFormatByOEidAndLocale(const std::string &oeid,
    const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(oeid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write oeid failed");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteString16(Str8ToStr16(locale))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write locale failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    format = std::unique_ptr<ObjectEditorFormat>(reply.ReadParcelable<ObjectEditorFormat>());
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadParcelable ObjectEditorFormat failed");
        return ERR_INVALID_VALUE;
    }
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetObjectEditorFormatsByLocale(const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteString16(Str8ToStr16(locale))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write locale failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_VALUE;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    int32_t count = reply.ReadInt32();
    if (count < 0 || count > MAX_READ_COUNT) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 count failed");
        return ERR_INVALID_DATA;
    }
    for (int32_t i = 0; i < count; ++i) {
        std::unique_ptr<ObjectEditorFormat> format =
            std::unique_ptr<ObjectEditorFormat>(reply.ReadParcelable<ObjectEditorFormat>());
        if (format == nullptr) {
            formats.clear();
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadParcelable ObjectEditorFormat failed");
            return ERR_INVALID_VALUE;
        }
        formats.emplace_back(std::move(format));
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "count: %{public}d", count);
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::StartUIAbility(const std::unique_ptr<AAFwk::Want> &want,
    sptr<IRemoteObject> extensionToken, int32_t clientPid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (want == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "want is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(want.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write want failed");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteInt32(clientPid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write clientPid failed");
        return ERR_INVALID_DATA;
    }
    if (extensionToken == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "extensionToken is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(extensionToken)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write extensionToken failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY), data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "succeed");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::QueryExtensionStopReason(const sptr<IRemoteObject> &oeExtensionRemoteObject,
    ExtensionStopReason &stopReason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write descriptor fail");
        return ERR_INVALID_VALUE;
    }
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(oeExtensionRemoteObject)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "write oeExtensionRemoteObject failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "null remote");
        return ERR_INVALID_DATA;
    }
    int32_t result = remote->SendRequest(
        static_cast<uint32_t>(IObjectEditorManagerIpcCode::COMMAND_QUERY_EXTENSION_STOP_REASON),
        data, reply, option);
    if (FAILED(result)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, result: %{public}d", result);
        return result;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed");
        return errCode;
    }
    stopReason = static_cast<ExtensionStopReason>(reply.ReadInt32());
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "succeed");
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
