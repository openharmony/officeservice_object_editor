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
#include <string_ex.h>

#include "content_embed_document.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "object_editor_document.h"
#include "pole.h"
#include "dirtree.h"

using namespace OHOS::ObjectEditor;
namespace {
constexpr char PATH_SEPARATOR = '/';
const std::string PATH_SEPARATOR_STR = "/";
constexpr int32_t MAX_NAME_LEN = 31;
constexpr int32_t OEID_LEN = 32;
constexpr int32_t OEID_MAX_LEN = 36;
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
    for (const char *p = name; *p != '\0'; ++p, ++len) {
        if (len > MAX_NAME_LEN) {
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

Storage *OH_ContentEmbed_Helper_GetRootStorage(const ContentEmbed_Storage *oeStorage)
{
    if (!oeStorage || !oeStorage->owner) {
        return nullptr;
    }
    return OH_ContentEmbed_Helper_GetRootStorage(oeStorage->owner);
}

ContentEmbed_ErrorCode CopyOEid(ContentEmbed_Storage *srcStorage, ContentEmbed_Storage *dstStorage)
{
    char oeid[128];
    size_t oeidLen = sizeof(oeid);
    auto err = OH_ContentEmbed_Storage_GetOEid(srcStorage, oeid, oeidLen);
    if (err != CE_ERR_OK) {
        return err;
    }
    err = OH_ContentEmbed_Storage_SetOEid(dstStorage, oeid, OEID_MAX_LEN);
    if (err != CE_ERR_OK) {
        return err;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode CopyStream(ContentEmbed_Storage *srcStorage, ContentEmbed_Storage *dstStorage, const char *name)
{
    ContentEmbed_Stream *srcStream = nullptr;
    ContentEmbed_Stream *dstStream = nullptr;
    ContentEmbed_ErrorCode err = OH_ContentEmbed_Storage_GetStream(srcStorage, name, &srcStream);
    if (err != CE_ERR_OK || srcStream == nullptr) {
        return err;
    }

    err = OH_ContentEmbed_Storage_CreateStream(dstStorage, name, &dstStream);
    if (err != CE_ERR_OK || dstStream == nullptr) {
        delete srcStream;
        srcStream = nullptr;
        return err;
    }
    
    const size_t BUFFER_SIZE = 4096;
    unsigned char *buffer = nullptr;
    size_t bytesRead = 0;
    size_t offset = 0;
    do {
        OH_ContentEmbed_Stream_Seek(srcStream, offset);
        err = OH_ContentEmbed_Stream_Read(srcStream, &buffer, BUFFER_SIZE, &bytesRead);
        if (err != CE_ERR_OK) {
            break;
        }
        if (bytesRead > 0) {
            size_t bytesWritten = 0;
            OH_ContentEmbed_Stream_Seek(dstStream, offset);
            err = OH_ContentEmbed_Stream_Write(dstStream, buffer, bytesRead, &bytesWritten);
            if (err != CE_ERR_OK || bytesWritten != bytesRead) {
                break;
            }
            offset += bytesRead;
        }
        if (buffer != nullptr) {
            delete[] buffer;
            buffer = nullptr;
        }
    } while (bytesRead > 0);

    if (buffer != nullptr) {
        delete[] buffer;
        buffer = nullptr;
    }
    delete srcStream;
    srcStream = nullptr;
    delete dstStream;
    dstStream = nullptr;
    return err;
}

ContentEmbed_ErrorCode CopyElement(ContentEmbed_Storage *srcStorage, ContentEmbed_Storage *dstStorage,
    ContentEmbed_StorageElements *elements, size_t index)
{
    ContentEmbed_StorageElement *element = nullptr;
    auto err = OH_ContentEmbed_StorageElements_GetElement(elements, index, &element);

    if (err != CE_ERR_OK) {
        return err;
    }
    
    char name[256];
    size_t nameLen = sizeof(name);
    err = OH_ContentEmbed_StorageElement_GetName(element, name, nameLen);
    if (err != CE_ERR_OK) {
        return err;
    }
    bool isStorage = false;
    err = OH_ContentEmbed_StorageElement_IsStorage(element, &isStorage);
    if (err != CE_ERR_OK) {
        return err;
    }

    if (isStorage) {
        ContentEmbed_Storage *srcSubStorage = nullptr;
        ContentEmbed_Storage *dstSubStorage = nullptr;
        err = OH_ContentEmbed_Storage_GetStorage(srcStorage, name, &srcSubStorage);
        if (err != CE_ERR_OK) {
            return err;
        }
        err = OH_ContentEmbed_Storage_CreateStorage(dstStorage, name, &dstSubStorage);
        if (err != CE_ERR_OK) {
            delete srcSubStorage;
            srcSubStorage = nullptr;
            return err;
        }
        err = OH_ContentEmbed_Storage_CopyTo(srcSubStorage, dstSubStorage);

        delete srcSubStorage;
        srcSubStorage = nullptr;
        delete dstSubStorage;
        dstSubStorage = nullptr;
    } else {
        bool isStream = false;
        err = OH_ContentEmbed_StorageElement_IsStream(element, &isStream);
        if (err == CE_ERR_OK && isStream) {
            err = CopyStream(srcStorage, dstStorage, name);
            if (err != CE_ERR_OK) {
                return err;
            }
        }
    }
    return err;
}


ContentEmbed_ErrorCode CopyElements(ContentEmbed_Storage *srcStorage, ContentEmbed_Storage *dstStorage)
{
    ContentEmbed_StorageElements *elements = nullptr;
    auto err = OH_ContentEmbed_StorageElements_Create(&elements);
    if (err != CE_ERR_OK) {
        return err;
    }
    err = OH_ContentEmbed_Storage_GetElements(srcStorage, elements);
    if (err != CE_ERR_OK) {
        OH_ContentEmbed_StorageElements_Destroy(elements);
        return err;
    }

    size_t count = 0;
    err = OH_ContentEmbed_StorageElements_GetCount(elements, &count);
    if (err != CE_ERR_OK) {
        OH_ContentEmbed_StorageElements_Destroy(elements);
        return err;
    }

    for (size_t i = 0; i < count; ++i) {
        err = CopyElement(srcStorage, dstStorage, elements, i);
        if (err != CE_ERR_OK) {
            break;
        }
    }

    OH_ContentEmbed_StorageElements_Destroy(elements);
    return err;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Helper_RequireStorageEntry(const ContentEmbed_Storage *handle,
    Storage *&storageOut)
{
    if (handle == nullptr) {
        return CE_ERR_NULL_POINTER;
    }
    storageOut = OH_ContentEmbed_Helper_GetRootStorage(handle);
    if (storageOut == nullptr) {
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
        handle->stream = nullptr;
        return CE_ERR_NULL_POINTER;
    }
    if (!handle->stream || handle->storage != storage) {
        Stream *stream = storage->GetStream(handle->path, false, true);
        if (stream == nullptr) {
            return CE_ERR_STREAM_OPERATION_FAILED;
        }
        StreamPos desired = handle->pos;
        const StreamPos size = stream->Size();
        if (desired > size) {
            desired = size;
        }
        stream->Seek(desired);
        if (stream->Fail()) {
            return CE_ERR_STREAM_OPERATION_FAILED;
        }
        handle->storage = storage;
        handle->stream = stream;
    }
    streamOut = handle->stream;
    handle->pos = streamOut->Tell();
    return CE_ERR_OK;
}
}

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
ContentEmbed_ErrorCode OH_ContentEmbed_CreateDocumentByOEid(const char *oeid, ContentEmbed_Document **document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    if (oeid == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "oeid is null");
        return CE_ERR_PARAM_INVALID;
    }
    const std::string oeidStr(oeid);
    if (!(oeidStr.size() == OEID_LEN || oeidStr.size() == OEID_MAX_LEN)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "oeid length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    auto doc = ObjectEditorDocument::CreateByOEid(oeidStr);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Document{oeid, {}, {}, {}, {}, {}, std::move(doc)};
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
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    if (srcFilePath == nullptr || length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string path(srcFilePath, length);
    auto doc = ObjectEditorDocument::CreateByFile(path, isLinking);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    if (isLinking && SystemUtils::IsAppSandboxPath(doc->GetOriFilePath())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "app sandbox path is not supported");
        return CE_ERR_INVALID_LINKING_PATH;
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
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    *document = nullptr;
    if (srcFilePath == nullptr || length <= 0 || !*(srcFilePath)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "srcFilePath is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string path(srcFilePath, length);
    auto doc = ObjectEditorDocument::LoadFromFile(path);
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create document failed");
        return CE_ERR_NULL_POINTER;
    }
    auto *wrapper = new (std::nothrow) ContentEmbed_Document{doc->GetOEid(), {}, {}, {}, {}, {}, std::move(doc)};
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
    if (document == nullptr || document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (buffer == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "buffer is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (readSize == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "readSize is null");
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

ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetOEid(const ContentEmbed_Document *document,
    char *oeid)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (oeid == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "oeid is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string oeidStr = document->oeid;
    if (strcpy_s(oeid, MAX_OEID_LENGTH, oeidStr.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy_s failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_IsLinking(const ContentEmbed_Document *document,
    bool *isLinking)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (isLinking == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "isLinking is null");
        return CE_ERR_PARAM_INVALID;
    }
    *isLinking = document->linking;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetNativeFilePath(const ContentEmbed_Document *document,
    char *nativeFilePath)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (nativeFilePath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "nativeFilePath is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::string filePathStr = document->oeDocumentInner->GetNativeFilePath();
    if (strcpy_s(nativeFilePath, MAX_PATH_LENGTH, filePathStr.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "copy path failed");
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
    if (storage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is null");
        return CE_ERR_PARAM_INVALID;
    }
    *storage = nullptr;
    if (document == nullptr || document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    };
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

ContentEmbed_ErrorCode OH_ContentEmbed_Document_Flush(const ContentEmbed_Document *document)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (document == nullptr || document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
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
    if (parentStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "parentStorage is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!OH_ContentEmbed_Helper_IsValidName(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "name is invalid");
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
        return CE_ERR_NULL_POINTER;
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
    if (parentStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "parentStorage is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!OH_ContentEmbed_Helper_IsValidName(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "name is invalid");
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
        return CE_ERR_NULL_POINTER;
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
    if (parentStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "parentStorage is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!OH_ContentEmbed_Helper_IsValidName(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "name is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storage = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(parentStorage, storage);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage entry failed");
        return ret;
    }
    const std::string targetPath = OH_ContentEmbed_Helper_JoinPath(parentStorage->name, name);
    if (!storage->GetStorage(targetPath, false)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get storage failed");
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
        return CE_ERR_NULL_POINTER;
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
    if (!parentStorage) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "parentStorage is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (!OH_ContentEmbed_Helper_IsValidName(name)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "name is invalid");
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

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_DeleteAllEntry(ContentEmbed_Storage *storage)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (!storage) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "storage is null");
        return CE_ERR_PARAM_INVALID;
    }
    Storage *storageInner = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(storage, storageInner);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require storage entry failed");
        return ret;
    }
    bool res = storageInner->EnterDirectory(storage->name);
    if (!res) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "EnterDirectory failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }

    std::vector<const OHOS::ObjectEditor::DirEntry *> result;
    storageInner->ListEntries(result);
    for (const auto entry : result) {
        if (!entry) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "entry is nullptr");
            return CE_ERR_STORAGE_OPERATION_FAILED;
        }
        if (!storageInner->GetEntry(entry->Name(), false)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "get entry failed");
            return CE_ERR_STORAGE_OPERATION_FAILED;
        }
        if (!storageInner->DeleteEntry(entry->Name())) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "delete entry failed");
            return CE_ERR_STORAGE_OPERATION_FAILED;
        }
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
    if (buffer == nullptr || num == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "buffer or num is null");
        return CE_ERR_PARAM_INVALID;
    }
    *buffer = nullptr;
    *num = 0;
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (length > static_cast<size_t>(std::numeric_limits<std::streamsize>::max()) || length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require stream failed");
        return ret;
    }
    *buffer = new unsigned char[length];
    const std::streamsize read = innerStream->Read(*buffer, static_cast<std::streamsize>(length));
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
        *num = 0;
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
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (buffer == nullptr || num == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "buffer or num is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (length == 0) {
        *num = 0;
        return CE_ERR_OK;
    }
    if (length > static_cast<size_t>(std::numeric_limits<std::streamsize>::max()) || length <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "length is invalid");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require stream failed");
        return ret;
    }
    const auto beforePos = innerStream->Tell();
    innerStream->Write(buffer, static_cast<std::streamsize>(length));
    const auto afterPos = innerStream->Tell();
    const size_t written = (afterPos >= beforePos) ? static_cast<size_t>(afterPos - beforePos) : 0U;
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
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (static_cast<uint64_t>(position) > std::numeric_limits<uint64_t>::max()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "position exceeds limit");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require stream failed");
        return ret;
    }
    const uint64_t size = innerStream->Size();
    if (position > size) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "position exceeds size:%{public}llu", size);
        return CE_ERR_STREAM_OPERATION_FAILED;
    }
    innerStream->Seek(static_cast<uint64_t>(position));
    stream->pos = static_cast<uint64_t>(position);
    if (innerStream->Fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream failed");
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
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (position == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "position is null");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require stream failed");
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
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (size == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "size is null");
        return CE_ERR_PARAM_INVALID;
    }
    Stream *innerStream = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStream(stream, innerStream);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require stream failed");
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
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "stream is null");
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
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete document;
    document = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetOEid(ContentEmbed_Storage *storage, char *oeid, size_t oeidSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storage == nullptr || storage->owner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    OHOS::ObjectEditor::Storage *root = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(storage, root);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require storage entry failed");
        return ret;
    }
    std::string oeidStr = storage->owner->oeid;
    if (oeidStr.size() >= oeidSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "oeid size exceeds limit");
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(oeid, oeidSize, oeidStr.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy oeid failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_SetOEid(ContentEmbed_Storage *storage, char *oeid, size_t oeidSize)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storage == nullptr || storage->owner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    OHOS::ObjectEditor::Storage *root = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(storage, root);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require storage entry failed");
        return ret;
    }
    std::string oeidStr(oeid, oeidSize);
    storage->owner->oeid = oeidStr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElements_Create(ContentEmbed_StorageElements **storageElements)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    *storageElements = new ContentEmbed_StorageElements();
    if (*storageElements == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "create storage elements failed");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElements_Destroy(ContentEmbed_StorageElements *storageElements)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElements == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    delete storageElements;
    storageElements = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetElements(const ContentEmbed_Storage *storage,
    ContentEmbed_StorageElements *storageElements)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storage == nullptr || storage->owner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    OHOS::ObjectEditor::Storage *root = nullptr;
    ContentEmbed_ErrorCode ret = OH_ContentEmbed_Helper_RequireStorageEntry(storage, root);
    if (ret != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "require storage entry failed");
        return ret;
    }
    if (!storage->owner->oeDocumentInner) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed to access document");
        return CE_ERR_NULL_POINTER;
    }
    OHOS::ObjectEditor::Storage *rootStorage = storage->owner->oeDocumentInner->GetRootStorage();
    if (!rootStorage) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "rootStorage is null");
        return CE_ERR_NULL_POINTER;
    }

    bool res = rootStorage->EnterDirectory(storage->name);
    if (!res) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "enter directory failed");
        return CE_ERR_STORAGE_OPERATION_FAILED;
    }
    std::vector<const OHOS::ObjectEditor::DirEntry *> result;
    rootStorage->ListEntries(result);
    storageElements->elements.clear();
    for (const auto entry : result) {
        auto element = std::make_unique<ContentEmbed_StorageElement>();
        element->name = entry->Name();
        element->isStorage = entry->IsDir();
        element->size = entry->Size();
        element->creationTime = entry->GetCreationTime();
        element->modifiedTime = entry->GetModifiedTime();
        storageElements->elements.push_back(std::move(element));
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElements_GetCount(
    const ContentEmbed_StorageElements *storageElements, size_t *count)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElements == nullptr || count == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *count = storageElements->elements.size();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElements_GetElement(const ContentEmbed_StorageElements *storageElements,
    size_t index, ContentEmbed_StorageElement **storageElement)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElements == nullptr || storageElement == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (index >= storageElements->elements.size() || index < 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "index exceeds limit");
        return CE_ERR_PARAM_INVALID;
    }
    *storageElement = storageElements->elements[index].get();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElement_GetName(
    const ContentEmbed_StorageElement *storageElement, char *name, size_t nameSize)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElement == nullptr || nameSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (strcpy_s(name, nameSize, storageElement->name.c_str()) != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "strcpy name failed");
        return CE_ERR_PARAM_INVALID;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElement_GetCTime(
    const ContentEmbed_StorageElement *storageElement, uint64_t *ctime)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElement == nullptr || ctime == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *ctime = storageElement->creationTime;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElement_GetMTime(
    const ContentEmbed_StorageElement *storageElement, uint64_t *mtime)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElement == nullptr || mtime == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *mtime = storageElement->modifiedTime;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElement_IsStorage(
    const ContentEmbed_StorageElement *storageElement, bool *isStorage)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElement == nullptr || isStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *isStorage = storageElement->isStorage;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_StorageElement_IsStream(
    const ContentEmbed_StorageElement *storageElement, bool *isStream)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (storageElement == nullptr || isStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *isStream = !(storageElement->isStorage);
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Storage_CopyTo(
    ContentEmbed_Storage *srcStorage, ContentEmbed_Storage *dstStorage)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (srcStorage == nullptr || dstStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto err = CopyOEid(srcStorage, dstStorage);
    if (err != CE_ERR_OK) {
        return err;
    }
    err = CopyElements(srcStorage, dstStorage);
    if (err != CE_ERR_OK) {
        return err;
    }
    return CE_ERR_OK;
}

// LCOV_EXCL_STOP
#ifdef __cplusplus
}
#endif /* __cplusplus */