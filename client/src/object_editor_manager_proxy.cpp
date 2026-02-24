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
ErrCode ObjectEditorManagerProxy::StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &callback, sptr<IRemoteObject> &objectEditorProxy, bool &isPackageExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (document == nullptr || callback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document or callback is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(document.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteParcelable document failed");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteRemoteObject callback failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    objectEditorProxy = reply.ReadRemoteObject();
    isPackageExtension = reply.ReadBool();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "StartObjectEditorExtension success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::StopObjectEditorExtension(const sptr<IRemoteObject> &oeExtensionRemoteObject,
    const bool &isPackageExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteRemoteObject(oeExtensionRemoteObject->AsObject())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteRemoteObject failed");
        return ERR_INVALID_DATA;
    }
    if (!data.WriteBool(isPackageExtension)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write isPackageExtension failed");
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "StopObjectEditorExtension success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetHmidByFileExtension(const std::string &hmid, std::string &fileExtension)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(hmid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write hmid failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_HMID_BY_FILE_EXTENSION), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    fileExtension = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetHmidByFileExtension success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetIconByHmid(const std::string &hmid, std::string &resFilePath)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(hmid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write hmid failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_ICON_BY_HMID), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    resFilePath = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetIconByHmid success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetFormatName(const std::string &hmid,
    const std::string &locale, std::string &formatName)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(hmid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write hmid failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(locale))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write locale failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<int32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_FORMAT_NAME), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    formatName = Str16ToStr8(reply.ReadString16());
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetFormatName success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetObjectEditorFormatByHmidAndLocale(const std::string &hmid,
    const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(hmid))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write hmid failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(locale))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write locale failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_FORMAT_BY_HMID_AND_LOCALE), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    format = std::unique_ptr<ObjectEditorFormat>(reply.ReadParcelable<ObjectEditorFormat>());
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadParcelable ObjectEditorFormat failed");
        return ERR_INVALID_VALUE;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetObjectEditorFormatByHmidAndLocale success");
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::GetObjectEditorFormatsByLocale(const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteString16(Str8ToStr16(locale))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Write locale failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorServiceIpcCode::COMMAND_GET_FORMATS_BY_LOCALE), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    int32_t count = reply.ReadInt32();
    if (count <= 0 || count > MAX_READ_COUNT) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 count failed");
        return ERR_INVALID_VALUE;
    }
    for (int32_t i = 0; i < count; i++) {
        std::unique_ptr<ObjectEditorFormat> format =
            std::unique_ptr<ObjectEditorFormat>(reply.ReadParcelable<ObjectEditorFormat>());
        if (format == nullptr) {
            formats.clear();
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadParcelable ObjectEditorFormat failed");
            return ERR_INVALID_VALUE;
        }
        formats.emplace_back(std::move(format));
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetObjectEditorFormatsByLocale success, count: %{public}d", count);
    return ERR_OK;
}

ErrCode ObjectEditorManagerProxy::StartUIAbility(const std::unique_ptr<AAFwk::Want> &want)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteInterfaceToken failed");
        return ERR_INVALID_VALUE;
    }
    if (!data.WriteParcelable(want.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WriteParcelable Want failed");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "Remote is null");
        return ERR_INVALID_DATA;
    }
    int32_t ret = remoteObject->SendRequest(
        static_cast<uint32_t>(IObjectEditorManager::Message::COMMAND_START_UI_ABILITY), data, reply, option);
    if (FAILED(ret)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SendRequest failed, ret: %{public}d", ret);
        return ret;
    }
    ErrCode errCode = reply.ReadInt32();
    if (FAILED(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "ReadInt32 failed, errCode: %{public}d", errCode);
        return errCode;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "StartUIAbility success");
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
