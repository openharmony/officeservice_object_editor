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

#include <cstdint>
#include <cstring>
#include <string_ex.h>

#include "content_embed_document.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "object_editor_document.h"
#include "pole.h"

using namespace OHOS::ObjectEditor;
namespace {
constexpr char PATH_SEPARATOR = '/';
const std::string PATH_SEPARATOR_STR = "/";
constexpr int32_t MAX_NAME_LEN = 31;
constexpr int32_t HMID_LEN = 32;
constexpr int32_t HMID_MAX_LEN = 36;
// LCOV_EXCL_START
std::string OH_ContentEmbed_Helper_NormalizePath(const std::string &path)
{
    if (path.empty()) {
        return PATH_SEPARATOR_STR;
    }
    std::string normalizedPath = path;
    if (normalizedPath.front() != PATH_SEPARATOR) {
        normalizedPath.insert(0, PATH_SEPARATOR_STR);
    }
    while (normalizedPath.size() > 1 && normalizedPath.back() == PATH_SEPARATOR) {
        normalizedPath.pop_back();
    }
    return normalizedPath.empty() ? PATH_SEPARATOR_STR : normalizedPath;
}

bool OH_ContentEmbed_Helper_IsValidName(const char *name)
{
    if (name == nullptr || !(*name)) {
        return false;
    }
    size_t len = 0;
    for (const char *p = name; *p; ++p, ++len) {
        if (len >= MAX_NAME_LEN) {
            return false;
        }
        const char c = *p;
        if (c == PATH_SEPARATOR || c == '\\' || c == ':' || c == '!') {
            return false;
        }
    }
    return true;
}

std::string OH_ContentEmbed_Helper_JoinPath(const std::string &parent, const char *child)
{
    std::string base = OH_ContentEmbed_Helper_NormalizePath(parent);
    if (base == PATH_SEPARATOR_STR) {
        return base + child;
    }
    base.push_back(PATH_SEPARATOR);
    base.append(child);
    return base;
}

Storage *OH_ContentEmbed_Helper_GetRootStorage(const ContentEmbed_Document *oeDoc)
{
    if (!oeDoc || !oeDoc->oeDocumentInner) {
        return nullptr;
    }
    return oeDoc->oeDocumentInner->GetRootStorage();
}

Storage *OH_ContentEmbed_Helper_GetRootStorage(const ContentEmbed_Storage *oeStroage)
{
    if (!oeStroage || !oeStroage->owner) {
        return nullptr;
    }
    return OH_ContentEmbed_Helper_GetRootStorage(oeStroage->owner);
}

ContentEmbed_ErrorCode OH_ContentEmbed_Helper_RequireStorageEntry(const ContentEmbed_Storage *handle,
    Storage *&storageOut)
{
    if (handle == nullptr || storageOut == nullptr) {
        return CE_ERR_NULL_POINTER;
    }
    const std::string normalizedPath = OH_ContentEmbed_Helper_NormalizePath(handle->name);
    if (!storageOut->GetStorage(normalizedPath, false)) {
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Helper_RequireStream(ContentEmbed_Stream *handle,
    Stream *&streamOut)
{
    streamOut = nullptr;
    if (handle == nullptr || handle->owner == nullptr || handle->owner->oeDocumentInner == nullptr) {
        return CE_ERR_NULL_POINTER;
    }
    if (handle->path.empty()) {
        return CE_ERR_FILE_OPERATION_FAILED;
    }
    Storage *storage = OH_ContentEmbed_Helper_GetRootStorage(handle->owner);
    if (storage == nullptr) {
        return CE_ERR_NULL_POINTER;
    }
    if (!storage->GetEntry(handle->path, false)) {
        return CE_ERR_NULL_POINTER;
    }
    if (!handle->stream || handle->storage != storage) {
        Stream *stream = storage->GetStream(handle->path, false, true);
        if (stream == nullptr) {
            return CE_ERR_STREAM_OPERATION_FAILED;
        }
        StreamPos desired = handle->pos;
        const StreamPos size = stream->size();
        if (desired > size) {
            desired = size;
        }
        stream->Seek(desired);
        if (stream->Fail()) {
            return CE_ERR_STREAM_OPERATION_FAILED;
        }
        handle->stream = stream;
        handle->storage = storage;
    }
    streamOut = handle->stream;
    handle->pos = streamOut->Tell();
    return CE_ERR_OK;
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
ContentEmbed_ErrorCode OH_ContentEmbed_CreateDocumentByHmid(const char *hmid, ContentEmbed_Document **document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (hmid == nullptr || document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    const std::string hmidStr(hmid);
    if (!(hmidStr.size() == HMID_LEN || hmidStr.size() == HMID_MAX_LEN)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "hmid length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    auto doc = ObjectEditorDocument::CreateByHmid(hmidStr);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Document{hmid, {}, {}, {}, {}, {}, std::move(doc)};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document wrapper failed");
        return CE_ERR_NULL_POINTER;
    }
    *document = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_CreateDocumentByFile(const char *srcFilePath, size_t length,
    bool isLinking, ContentEmbed_Document **document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (srcFilePath == nullptr || document == nullptr || length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    std::string path(srcFilePath, length);
    auto doc = ObjectEditorDocument::CreateByFile(path, isLinking);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Document{{}, isLinking, {}, path, {}, {}, std::move(doc)};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document wrapper failed");
        return CE_ERR_NULL_POINTER;
    }
    *document = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_LoadDocumentFromFile(const char *srcFilePath, size_t length,
    ContentEmbed_Document **document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (srcFilePath == nullptr || document == nullptr || length <= 0 || !(*srcFilePath)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    std::string path(srcFilePath, length);
    auto doc = ObjectEditorDocument::LoadFromFile(path);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Document{doc->GetHmid(), {}, {}, {}, {}, {}, std::move(doc)};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document wrapper failed");
        return CE_ERR_NULL_POINTER;
    }
    *document = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_Read(uint8_t *buffer, size_t length,
    ContentEmbed_Document *document, size_t offset,
    size_t *readSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr ||
        buffer == nullptr || readSize == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *readSize = 0;
    if (length == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is 0");
        return CE_ERR_OK;
    }
    if (length > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is too large");
        return CE_ERR_PARAM_INVALID;
    }
    if (offset > static_cast<size_t>(std::numeric_limits<std::streamsize>::max())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "offset is too large");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = OH_ContentEmbed_Helper_GetRootStorage(document);
    if (storage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is null");
        return CE_ERR_NULL_POINTER;
    }
    if (storage->IsDirty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is dirty");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    if (!storage->ReadRawCd(offset, buffer, length, readSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "read raw cd failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetHmid(ContentEmbed_Document *document,
    char *hmid, size_t hmidSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr ||
        hmid == nullptr || hmidSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string hmidStr = document->hmid;
    if (hmidStr.size() >= hmidSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "hmid size is too small");
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(hmid, hmidSize, hmidStr.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_IsLinking(ContentEmbed_Document *document,
    bool *isLinking)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || isLinking == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *isLinking = document->linking;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetNativeFilePath(ContentEmbed_Document *document,
    char *nativeFilePath, size_t nativeFilePathSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || nativeFilePath == nullptr || nativeFilePathSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string filePathStr = document->oeDocumentInner->GetNativeFilePath();
    if (filePathStr.size() >= nativeFilePathSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "file path size is too small");
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(nativeFilePath, nativeFilePathSize, filePathStr.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetRootStorage(ContentEmbed_Document *document,
    ContentEmbed_Storage **storage)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr || rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *storage = nullptr;
    Storage *rootStorage = document->oeDocumentInner->GetRootStorage();
    if (rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "root storage is null");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    std::string path = PATH_SEPARATOR_STR;
    rootStorage->Path(path);
    path = OH_ContentEmbed_Helper_NormalizePath(path);
    auto *wrapper = new (std::nothrow) ContentEmbed_Storage{document, path};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage wrapper failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    *storage = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_Flush(ContentEmbed_Document *document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!document->oeDocumentInner->Flush()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "flush failed");
        return CE_ERR_FILE_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_CreateStorage(const ContentEmbed_Storage *parentStorage,
    const char *name, ContentEmbed_Storage **childStorage)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (childStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "child storage is null");
        return CE_ERR_PARAM_INVALID;
    }
    *childStorage = nullptr;
    if (parentStorage == nullptr || !OH_ContentEmbed_Helper_ValidatePath(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(parentStorage, storage);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage failed");
        return ret;
    }
    const std::string targetPath = OH_ContentEmbed_Helper_JoinPath(parentStorage->name, name);
    if (!storage->GetStorage(targetPath, false) && storage->GetEntry(targetPath, false)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is invalid");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    if (!storage->GetStorage(targetPath, true)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get storage failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Storage{parentStorage->owner,
        OH_ContentEmbed_Helper_NormalizePath(targetPath)};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage wrapper failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    *childStorage = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetStorage(const ContentEmbed_Storage *parentStorage,
    const char *name, ContentEmbed_Storage **childStorage)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (childStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "child storage is null");
        return CE_ERR_PARAM_INVALID;
    }
    *childStorage = nullptr;
    if (parentStorage == nullptr || !OH_ContentEmbed_Helper_IsValidName(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(parentStorage, storage);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage failed");
        return ret;
    }
    const std::string targetPath = OH_ContentEmbed_Helper_JoinPath(parentStorage->name, name);
    if (!storage->GetStorage(targetPath, false)) {
        if (storage->GetEntry(targetPath, false)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is invalid");
            return CE_ERR_STORAGE_OPERATION_FAILED;
        }
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Storage{parentStorage->owner,
        OH_ContentEmbed_Helper_NormalizePath(targetPath)};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage wrapper failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    *childStorage = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Helper_GetStreamInternal(ContentEmbed_Storage *parentStorage,
    const char *name, bool create, ContentEmbed_Stream **childStream)
{
    if (!childStream) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "child stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    *childStream = nullptr;
    if (!parentStorage || !OH_ContentEmbed_Helper_ValidatePath(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(parentStorage, storage);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage failed");
        return ret;
    }
    const std::string targetPath = OH_ContentEmbed_Helper_JoinPath(parentStorage->name, name);
    if (!storage->GetStream(targetPath, create)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get stream failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    Stream *stream = storage->GetStream(targetPath, create, true);
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get stream failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Stream{parentStorage->owner,
        targetPath, storage, stream, stream->Tell()};
    if (wrapper == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream wrapper failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    *childStream = wrapper;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_CreateStream(ContentEmbed_Storage *parentStorage,
    const char *name, ContentEmbed_Stream **childStream)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    return OH_ContentEmbed_Helper_GetStreamInternal(parentStorage, name, true, childStream);
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetStream(ContentEmbed_Storage *parentStorage,
    const char *name, ContentEmbed_Stream **childStream)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    return OH_ContentEmbed_Helper_GetStreamInternal(parentStorage, name, false, childStream);
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_DeleteEntry(ContentEmbed_Storage *parentStorage,
    const char *name)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!parentStorage || !OH_ContentEmbed_Helper_ValidatePath(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(parentStorage, storage);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage failed");
        return ret;
    }
    const std::string targetPath = OH_ContentEmbed_Helper_JoinPath(parentStorage->name, name);
    if (OH_ContentEmbed_Helper_NormalizePath(targetPath) == PATH_SEPARATOR_STR) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "delete entry failed");
        return CE_ERR_FILE_OPERATION_FAILED;
    }
    if (!storage->GetEntry(targetPath, false)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get entry failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    if (!storage->DeleteEntry(targetPath)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "delete entry failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyStorage(ContentEmbed_Storage *storage)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!storage) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete storage;
    storage = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Read(ContentEmbed_Stream *stream,
    unsigned char **buffer, size_t length, size_t *num)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream || !buffer || !num) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *buffer = nullptr;
    *num = 0;
    if (length > static_cast<size_t>(std::numeric_limits<std::streamsize>::max()) ||
        length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream failed");
        return ret;
    }
    *buffer = new unsigned char[length];
    const std::streamsize read = innerStream->read(*buffer, static_cast<std::streamsize>(length));
    if (read < 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "read stream failed");
        delete[] *buffer;
        *buffer = nullptr;
        return CE_ERR_STREAM_OPERATION_FAILED;
    }
    *num = static_cast<size_t>(read);
    stream->pos = innerStream->Tell();
    if (innerStream->Fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream fail");
        delete[] *buffer;
        *buffer = nullptr;
        return CE_ERR_STREAM_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Write(ContentEmbed_Stream *stream,
    const unsigned char *buffer, size_t length, size_t *num)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream || !buffer || !num) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (length == 0) {
        *num = 0;
        return CE_ERR_OK;
    }
    if (length > static_cast<size_t>(std::numeric_limits<std::streamsize>::max()) ||
        length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream failed");
        return ret;
    }
    const auto beforePos = innerStream->Tell();
    innerStream->Write(buffer, static_cast<std::streamsize>(length));
    const auto afterPos = innerStream->Tell();
    const size_t written = (afterPos >= beforePos) ? (afterPos - beforePos) : 0;
    stream->pos = afterPos;
    if (written != length || innerStream->Fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream fail");
        return CE_ERR_STREAM_OPERATION_FAILED;
    }
    *num = written;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Seek(ContentEmbed_Stream *stream, size_t position)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream failed");
        return ret;
    }
    const uint64_t size = innerStream->Size();
    if (position > size) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "position exceeds size:%{public}zu", size);
        return CE_ERR_STREAM_OPERATION_FAILED
    }
    innerStream->Seek(static_cast<uint64_t>(position));
    stream->pos = static_cast<uint64_t>(position);
    if (innerStream->Fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream fail");
        return CE_ERR_STREAM_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Stream_GetPosition(ContentEmbed_Stream *stream, size_t *position)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream || !position) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream failed");
        return ret;
    }
    stream->pos = innerStream->Tell();
    *position = static_cast<size_t>(stream->pos);
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Stream_GetSize(ContentEmbed_Stream *stream, size_t *size)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream || !size) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create stream failed");
        return ret;
    }
    *size = static_cast<size_t>(innerStream->Size());
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyStream(ContentEmbed_Stream *stream)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!stream) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete stream;
    stream = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_DestroyDocument(ContentEmbed_Document *document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!document) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete document;
    document = nullptr;
    return CE_ERR_OK;
}

// LCOV_EXCL_STOP
#ifdef __cplusplus
}
#endif /* __cplusplus */