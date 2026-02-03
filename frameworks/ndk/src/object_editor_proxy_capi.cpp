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

#include <cstring>
#include <filesystem>

#include "image_source.h"

#include "iobject_editor_extension.h"
#include "iobject_editor_package.h"
#include "native_object_editor_types.h"
#include "object_editor_client.h"
#include "object_editor_client_callback.h"
#include "object_editor_config.h"
#include "system_utils.h"

using namespace OHOS::ObjectEditor;
namespace {
constexpr uint32_t MAX_STRING_LENGTH = 10000;

// LCOV_EXCL_START
std::unique_ptr<OHOS::Media::PixelMap> CreatePixelMap(const std::string &filePath,
    const std::string &formatHint, uint32_t &errCode)
{
    OHOS::Media::SourceOptions options;
    options.formatHint = formatHint;
    std::unique_ptr<OHOS::Media::ImageSource> imageSource =
        OHOS::Media::ImageSource::CreateImageSource(filePath, options, errCode);
    if (imageSource == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "fail:%{public}u", errCode);
        return nullptr;
    }
    OHOS::Media::DecodeOptions decodeOptions;
    return imageSource->CreatePixelMap(decodeOptions, errCode);
}

std::unique_ptr<OH_PixelMapNative> CreatePixelMapNative(const std::string &filePath,
    const std::string &formatHint, uint32_t &errCode)
{
    std::unique_ptr<OHOS::Media::PixelMap> pixelMap = CreatePixelMap(filePath, formatHint, errCode);
    if (pixelMap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "fail:%{public}u", errCode);
        return nullptr;
    }
    std::shared_ptr<OHOS::Media::PixelMap> pixelMapShared = std::move(pixelMap);
    return std::make_unique<OH_PixelMapNative>(pixelMapShared);
}

char *CopyToCString(const std::string &str)
{
    size_t size = str.size();
    if (size == 0 || size > MAX_STRING_LENGTH) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "size:%{public}u invalid", size);
        return nullptr;
    }
    char *cstr = new (std::nothrow) char[size + 1];
    if (cstr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "alloc failed");
        return nullptr;
    }
    if (memcpy_s(cstr, size + 1, str.c_str(), size) != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "memcpy_s failed");
        delete[] cstr;
        return nullptr;
    }
    cstr[size] = '\0';
    return cstr;
}

void FreeCString(char **cstrs, size_t len)
{
    if (cstrs == nullptr) {
        return;
    }
    for (size_t i = 0; i < len; i++) {
        if (cstrs[i] != nullptr) {
            delete[] cstrs[i];
            cstrs[i] = nullptr;
        }
    }
    delete[] cstrs;
    cstrs = nullptr;
}

char **SplitToCStrings(const std::string &str, char pattern, uint32_t &count)
{
    std::vector<std::string> substrs = SystemUtils::SplitString(str, pattern);
    size_t size = substrs.size();
    count = static_cast<uint32_t>(size);
    if (size == 0 || size > MAX_STRING_LENGTH) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "size:%{public}u invalid", size);
        return nullptr;
    }
    char **cstrs = new (std::nothrow) char *[size];
    if (cstrs == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "alloc failed");
        return nullptr;
    }
    for (size_t i = 0; i < size; i++) {
        char *subcstr = CopyToCString(substrs[i]);
        if (subcstr == nullptr) {
            FreeCString(cstrs, i);
            return nullptr;
        }
        cstrs[i] = subcstr;
    }
    return cstrs;
}
} // namespace

ContentEmbed_Format::~ContentEmbed_Format()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "~ContentEmbed_Format");
    if (fileNameExtensions == nullptr) {
        return;
    }
    for (uint32_t i = 0; i < fileNameExtensionsCount; i++) {
        if (fileNameExtensions[i] != nullptr) {
            delete[] fileNameExtensions[i];
            fileNameExtensions[i] = nullptr;
        }
    }
    delete[] fileNameExtensions;
    fileNameExtensions = nullptr;
    fileNameExtensionsCount = 0;
}

void ContentEmbed_Format::Build(std::unique_ptr<ObjectEditorFormat> &format)
{
    hmid = std::move(format->hmid);
    locale = std::move(format->locale);
    name = std::move(format->name);
    description = std::move(format->description);
    icon = std::move(format->icon);
    fileNameExtensions = SplitToCStrings(format->fileExts, ',', fileNameExtensionsCount);
}

vpid ContentEmbed_Info::Build(std::vector<std::unique_ptr<ObjectEditorFormat>> &oeFormats)
{
    for (auto &format : oeFormats) {
        std::unique_ptr<ContentEmbed_Format> embedFormat = std::make_unique<ContentEmbed_Format>();
        embedFormat->Build(format);
        formats.push_back(std::move(embedFormat));
    }
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ContentEmbed_ErrorCode OH_ContentEmbed_CreateContentEmbedInfo(ContentEmbed_Info **info)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (info == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "info is null");
        return CE_ERR_PARAM_INVALID;
    }
    *info = new (std::nothrow) ContentEmbed_Info();
    if (*info == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "alloc failed");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyContentEmbedInfo(ContentEmbed_Info *info)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (info == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "info is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete info;
    info = nullptr;
    return CE_ERR_OK;
}
// LCOV_EXCL_START

ContentEmbed_ErrorCode OH_ContentEmbed_GetContentEmbedInfo(const char *locale, ContentEmbed_Info *info)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (info == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "info is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!info->formats.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "info is not empty");
        return CE_ERR_OK;
    }
    std::string strLocale = locale == nullptr ? "" : std::string(locale);
    std::vector<std::unique_ptr<ObjectEditorFormat>> oeFormats;
    auto errCode = ObjectEditorClient::GetInstance().GetObjectEditorFormatsByLocale(strLocale, oeFormats);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "GetFormats failed: %{public}d", errCode);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
// LCOV_EXCL_START
    info->Build(oeFormats);
    return CE_ERR_OK;
// LCOV_EXCL_STOP
}

// LCOV_EXCL_START
ContentEmbed_ErrorCode OH_ContentEmbed_GetFormatCountFromInfo(ContentEmbed_Info *info, uint32_t *count)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (info == nullptr || count == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *count = info->formats.size();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_GetFormatFromInfo(const ContentEmbed_Info *info,
    uint32_t index, ContentEmbed_Format **format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (info == nullptr || format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "info or format is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (index >= info->formats.size()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "index:%{public}u exceed count:%{public}zu",
            index, info->formats.size());
        return CE_ERR_PARAM_INVALID;
    }
    *format = info->formats[index].get();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_CreateContentEmbedFormat(ContentEmbed_Format **format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "format is null");
        return CE_ERR_PARAM_INVALID;
    }
    *format = new (std::nothrow) ContentEmbed_Format();
    if (*format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "alloc failed");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyContentEmbedFormat(ContentEmbed_Format *format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "format is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete format;
    format = nullptr;
    return CE_ERR_OK;
}
// LCOV_EXCL_STOP

ContentEmbed_ErrorCode OH_ContentEmbed_GetContentEmbedFormatByHmidAndLocale(const char *hmid, const char *locale,
    ContentEmbed_Format *format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (hmid == nullptr || format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string strLocale = locale == nullptr ? "" : std::string(locale);
    std::unique_ptr<ObjectEditorFormat> oeFormat;
    auto errCode = ObjectEditorClient::GetInstance().GetObjectEditorFormatByHmidAndLocale(std::string(hmid),
        strLocale, oeFormat);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "GetFormat failed: %{public}d", errCode);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
// LCOV_EXCL_START
    format->Build(oeFormat);
    return CE_ERR_OK;
// LCOV_EXCL_STOP
}

// LCOV_EXCL_START
ContentEmbed_ErrorCode OH_ContentEmbed_GetHmidFromFormat(const ContentEmbed_Format *format, char *hmid,
    const int32_t hmidSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (format == nullptr || hmid == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (format->hmid.size() >= hmidSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "hmid size:%{public}zu exceed hmidSize:%{public}d",
            format->hmid.size(), hmidSize);
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(hmid, MAX_HMID_LENGTH, format->hmid.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_GetNameAndDescriptionFromFormat(const ContentEmbed_Format *format, char *name,
    const int32_t nameSize, char *description, const int32_t descriptionSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (format == nullptr || name == nullptr || description == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (format->name.size() >= nameSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "name size:%{public}zu exceed nameSize:%{public}d",
            format->name.size(), nameSize);
        return CE_ERR_PARAM_INVALID;
    }
    if (format->description.size() >= descriptionSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "description size:%{public}zu exceed descriptionSize:%{public}d",
            format->description.size(), descriptionSize);
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(name, MAX_NAME_LENGTH, format->name.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(description, MAX_DESCRIPTION_LENGTH, format->description.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_GetIconFromFormat(const ContentEmbed_Format *format,
    OHOS::Media::PixelMap **icon)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (format == nullptr || icon == nullptr || format->icon == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (*icon != nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "icon is not null");
        return CE_ERR_PARAM_INVALID;
    }
    *icon = new OH_PixelMapNative(format->icon);
    if (*icon == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "icon alloc failed");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

char **OH_ContentEmbed_GetFileNameExtensionsFromFormat(const ContentEmbed_Format *format,
    uint32_t *fileNameExtensionsCount)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return nullptr;
    }
    if (format == nullptr || fileNameExtensionsCount == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return nullptr;
    }
    if (format->fileNameExtensionsCount == 0 || format->fileNameExtensions == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "fileNameExtensions is null");
        return nullptr;
    }
    *fileNameExtensionsCount = format->fileNameExtensionsCount;
    return format->fileNameExtensions;
}

ContentEmbed_ErrorCode OH_ContentEmbed_CreateExtensionProxy(ContentEmbed_Document *ceDocument,
    ContentEmbed_ExtensionProxy **proxy)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (ceDocument == nullptr || proxy == nullptr) {    
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *proxy = new (std::nothrow) ContentEmbed_ExtensionProxy();
    if (*proxy == nullptr) {    
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "extensionProxy alloc failed");
        return CE_ERR_NULL_POINTER;
    }
    (*proxy)->ceDocument = ceDocument;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyExtensionProxy(ContentEmbed_ExtensionProxy *proxy)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "proxy is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (proxy->ceDocument != nullptr) {
        delete proxy->ceDocument;
        proxy->ceDocument = nullptr;
    }
    delete proxy;
    proxy = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_RegisterOnUpdateFunc(ContentEmbed_ExtensionProxy *proxy,
    OH_ContentEmbed_ClientCallbackOnUpdateFunc onUpdateFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || onUpdateFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    proxy->onUpdateFunc = onUpdateFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_RegisterOnErrorFunc(ContentEmbed_ExtensionProxy *proxy,
    OH_ContentEmbed_ClientCallbackOnErrorFunc onErrorFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || onErrorFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    proxy->onErrorFunc = onErrorFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_RegisterOnEditingFinishedFunc(ContentEmbed_ExtensionProxy *proxy,
    OH_ContentEmbed_ClientCallbackOnEditingFinishedFunc onEditingFinishedFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || onEditingFinishedFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    proxy->onEditingFinishedFunc = onEditingFinishedFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_RegisterOnExtensionStoppedFunc(ContentEmbed_ExtensionProxy *proxy,
    OH_ContentEmbed_ClientCallbackOnExtensionStoppedFunc onExtensionStoppedFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || onExtensionStoppedFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    proxy->onExtensionStoppedFunc = onExtensionStoppedFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_StartWork(ContentEmbed_ExtensionProxy *proxy)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "proxy is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (proxy->onUpdateFunc == nullptr || proxy->onErrorFunc == nullptr ||
        proxy->onEditingFinishedFunc == nullptr || proxy->onExtensionStoppedFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "callback is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto oeCallbackInner = new (std::nothrow) ObjectEditorClientCallback(proxy);
    if (oeCallbackInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "alloc failed");
        return CE_ERR_NULL_POINTER;
    }
    auto errCode = ObjectEditorClient::GetInstance().StartObjectEditorExtension(proxy->ceDocument->oeDocumentInner
        oeCallbackInner, proxy->oeExtensionRemoteObject, proxy->isPackageExtension);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "StartWork failed, errCode: %{public}d", errCode);
        delete oeCallbackInner;
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_DoEdit(ContentEmbed_ExtensionProxy *proxy)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->oeExtensionRemoteObject == nullptr ||
        proxy->ceDocument == nullptr || proxy->ceDocument->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "proxy is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    auto errCode = proxy->oeExtensionRemoteObject->DoEdit(proxy->ceDocument->oeDocumentInner->GetDocumentId());
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_EXTENSION_ERROR;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_GetEditStatus(ContentEmbed_ExtensionProxy *proxy,
    bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->oeExtensionRemoteObject == nullptr ||
        proxy->ceDocument == nullptr || proxy->ceDocument->oeDocumentInner == nullptr ||
        isEditing == nullptr || isModified == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto errCode = proxy->oeExtensionRemoteObject->GetEditStatus(
        proxy->ceDocument->oeDocumentInner->GetDocumentId(),
        isEditing, isModified);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_EXTENSION_ERROR;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_GetCapability(ContentEmbed_ExtensionProxy *proxy,
    uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->oeExtensionRemoteObject == nullptr ||
        proxy->ceDocument == nullptr || proxy->ceDocument->oeDocumentInner == nullptr ||
        capability == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto errCode = proxy->oeExtensionRemoteObject->GetCapability(
        proxy->ceDocument->oeDocumentInner->GetDocumentId(),
        bitmask);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_EXTENSION_ERROR;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_GetSnapShot(ContentEmbed_ExtensionProxy *proxy,
    OH_PixelMapNative **snapshot)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->oeExtensionRemoteObject == nullptr ||
        proxy->ceDocument == nullptr || proxy->ceDocument->oeDocumentInner == nullptr ||
        snapshot == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto errCode = proxy->oeExtensionRemoteObject->GetSnapShot(
        proxy->ceDocument->oeDocumentInner->GetDocumentId());
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_EXTENSION_ERROR;
    }
    std::string snapshotPath = proxy->ceDocument->oeDocumentInner->GetSnapshotPath();
    if (!std::filesystem::exists(snapshotPath) || std::filesystem::file_size(snapshotPath) == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "snapshotPath is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    uint32_t errCode2 = 0;
    auto pixelMap = CreatePixelMapNative(snapshotPath, "image/png", errCode2);
    if (pixelMap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create pixelmap fialed");
        return CE_ERR_NULL_POINTER;
    }
    *snapshot = pixelMap.get();
    pixelMap.release();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_StopWork(ContentEmbed_ExtensionProxy *proxy)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "proxy is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    auto errCode = ObjectEditorClient::GetInstance().StopObjectEditorExtension(proxy->ceDocument->oeDocumentInner,
        proxy->oeExtensionRemoteObject, proxy->isPackageExtension);
    if (errCode != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Proxy_GetDocument(ContentEmbed_ExtensionProxy *proxy,
    ContentEmbed_Document **ceDocument)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (ObjectEditorConfig::GetInstance().CheckIsInDlp()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported in dlp");
        return CE_ERR_IN_DLP_SANDBOX;
    }
    if (proxy == nullptr || proxy->ceDocument == nullptr || ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    *ceDocument = proxy->ceDocument;
    return CE_ERR_OK;
}
// LCOV_EXCL_STOP
#ifdef __cplusplus
}
#endif /* __cplusplus */