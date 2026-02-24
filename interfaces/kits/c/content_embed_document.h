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

/**
 * @addtogroup ContentEmbed
 * @{
 *
 * @file content_embed_document.h
 *
 * @brief Defines the content embed document APIs.
 *
 * @library libcontent_embed_ndk.so
 * @kit ContentEmbedKit
 * @syscap SystemCapability.ContentEmbed.ObjectEditor
 * @since 24
 */

#ifndef OHOS_CONTENT_EMBED_DOCUMENT_H
#define OHOS_CONTENT_EMBED_DOCUMENT_H

#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include "content_embed_common.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief The maximum length of a file path.
 *
 * @since 24
 */
#define MAX_PATH_LENGTH (1 * 1024)

/**
 * @brief Define the ContentEmbed_Document structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Document ContentEmbed_Document;

/**
 * @brief Define the ContentEmbed_Storage structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Storage ContentEmbed_Storage;

/**
 * @brief Define the ContentEmbed_Stream structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Stream ContentEmbed_Stream;

/**
 * @brief Create a new {@link ContentEmbed_Document} instance using the provided hmid.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyDocument} to avoid memory leaks.
 *
 * @param hmid Represents hmid value.
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance which will be created.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_CreateDocumentByHmid(
    const char *hmid, ContentEmbed_Document **document);

/**
 * @brief Create a new {@link ContentEmbed_Document} instance from the source file.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyDocument} to avoid memory leaks.
 *
 * @param srcFilePath Represents source file path.
 * @param length Represents source file path length.
 * @param isLinking Represents whether the source file is linked to the document.
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance which will be created.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_CreateDocumentByFile(
    const char *srcFilePath, size_t length, bool isLinking, ContentEmbed_Document **document);

/**
 * @brief Load an instance of {@link ContentEmbed_Document} from the source file.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyDocument} to avoid memory leaks.
 *
 * @param srcFilePath Represents source file path.
 * @param length Represents source file path length.
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance which will be loaded.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_LoadDocumentFromFile(
    const char *srcFilePath, size_t length, ContentEmbed_Document **document);

/**
 * @brief Reads a compound document from {@link ContentEmbed_Document} instance.
 *
 * @param buffer Represents the buffer data read from an {@link ContentEmbed_Document} instance.
 * @param length Represents the length of the buffer data.
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @param offset Represents pointer position of reading.
 * @param readSize Represents the length of the data actually read.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_Read(
    uint8_t *buffer, size_t length, ContentEmbed_Document *document, size_t offset, size_t *readSize);

/**
 * @brief Get hmid from {@link ContentEmbed_Document} instance.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @param hmid Output parameter represents the hmid value.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetHmid(
    const ContentEmbed_Document *document, char hmid[MAX_HMID_LENGTH]);

/**
 * @brief whether the source file is linked to the {@link ContentEmbed_Document} instance.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @param isLinking Output parameter represents whether the source file is linked to the document.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_IsLinking(
    const ContentEmbed_Document *document, bool *isLinking);

/**
 * @brief Get native file path from {@link ContentEmbed_Document} instance.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @param nativeFilePath Output parameter represents the path of the source file within
                         the {@link ContentEmbed_Document} on disk.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetNativeFilePath(
    const ContentEmbed_Document *document, char *nativeFilePath);

/**
 * @brief Get the {@link ContentEmbed_Storage} from a {@link ContentEmbed_Document} instance.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyStorage} to avoid memory leaks.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @param storage Output parameter represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_GetRootStorage(
    const ContentEmbed_Document *document, ContentEmbed_Storage **storage);

/**
 * @brief Flushes the content of the {@link ContentEmbed_Document} instance.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_FILE_OPERATION_FAILED} - the file operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Document_Flush(const ContentEmbed_Document *document);

/**
 * @brief Creates a new {@link ContentEmbed_Storage} instance.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyStorage} to avoid memory leaks.
 *
 * @param parentStorage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @param name Represents the name of an {@link ContentEmbed_Storage} instance which will be created.
 * @param childStorage Output parameter represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Storage_CreateStorage(
    const ContentEmbed_Storage *parentStorage, const char *name, ContentEmbed_Storage **childStorage);

/**
 * @brief Obtain the child {@link ContentEmbed_Storage} instance from its parent instance.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyStorage} to avoid memory leaks.
 *
 * @param parentStorage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @param name Represents the name of an {@link ContentEmbed_Storage} instance which will be obtained.
 * @param childStorage Output parameter represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetStorage(
    const ContentEmbed_Storage *parentStorage, const char *name, ContentEmbed_Storage **childStorage);

/**
 * @brief Create a new {@link ContentEmbed_Stream} instance.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyStream} to avoid memory leaks.
 *
 * @param parentStorage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @param name Represents the name of an {@link ContentEmbed_Stream} instance which will be created.
 * @param childStream Output parameter represents a pointer to an {@link ContentEmbed_Stream} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Storage_CreateStream(
    const ContentEmbed_Storage *parentStorage, const char *name, ContentEmbed_Stream **childStream);

/**
 * @brief Obtain the child {@link ContentEmbed_Stream} instance from the parent storage instance.
 * The caller is responsible for destroying the instance by calling
 * {@link OH_ContentEmbed_DestroyStream} to avoid memory leaks.
 *
 * @param parentStorage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @param name Represents the name of an {@link ContentEmbed_Stream} instance which will be obtained.
 * @param childStream Output parameter represents a pointer to an {@link ContentEmbed_Stream} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Storage_GetStream(
    const ContentEmbed_Storage *parentStorage, const char *name, ContentEmbed_Stream **childStream);

/**
 * @brief Delete a specific Storage or Stream directory entry in a directory tree.
 *
 * @param parentStorage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * @param name Represents the name of an entry which will be deleted.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STORAGE_OPERATION_FAILED} - the storage operation failed.
 *     {@link CE_ERR_FILE_OPERATION_FAILED} - the file operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Storage_DeleteEntry(
    ContentEmbed_Storage *parentStorage, const char *name);

/**
 * @brief Destroy an {@link ContentEmbed_Storage} instance and reclaims the memory occupied by it.
 *
 * @param storage Represents a pointer to an {@link ContentEmbed_Storage} instance.
 * After this call, the pointer becomes invalid and must not be used.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_DestroyStorage(ContentEmbed_Storage *storage);

/**
 * @brief Read the buffer data from an {@link ContentEmbed_Stream} instance.
 * The caller is responsible for destroying the buffer to avoid memory leaks.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance.
 * @param buffer Output parameter represents the buffer data read from an {@link ContentEmbed_Stream} instance.
 * @param length Represents the length of the buffer data.
 * @param num Output parameter represents the num of the buffer data that has been read.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Read(
    ContentEmbed_Stream *stream, unsigned char **buffer, size_t length, size_t *num);

/**
 * @brief Write the buffer data to an {@link ContentEmbed_Stream} instance.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance.
 * @param data Represents the data to write to an {@link ContentEmbed_Stream} instance.
 * @param length Represents the length of the data.
 * @param num Output parameter represents the num of the data that has been written.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Write(
    ContentEmbed_Stream *stream, const unsigned char *data, size_t length, size_t *num);

/**
 * @brief Sets the current read position of the {@link ContentEmbed_Stream} to the specified offset.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance that will be modified.
 * @param position The offset in bytes from the beginning of the stream to which the position will be set.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Stream_Seek(ContentEmbed_Stream *stream, size_t position);

/**
 * @brief Gets the current read position of the {@link ContentEmbed_Stream}.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance.
 * @param position Output parameter represents a pointer to a size_t variable
 *                 where the current read position will be stored.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_FILE_OPERATION_FAILED} - the file operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Stream_GetPosition(ContentEmbed_Stream *stream, size_t *position);

/**
 * @brief Retrieves the size of the data in the {@link ContentEmbed_Stream}.
 *
 * This function attempts to determine the total size of the data available in the provided stream.
 * If successful, the size is written to the memory location pointed to by the 'size' parameter.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance from which
 *               the size is to be retrieved.
 * @param size Output parameter represents a pointer to a size_t variable where the size of the stream will be stored.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_STREAM_OPERATION_FAILED} - the stream operation failed.
 *     {@link CE_ERR_FILE_OPERATION_FAILED} - the file operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Stream_GetSize(ContentEmbed_Stream *stream, size_t *size);

/**
 * @brief Destroys an {@link ContentEmbed_Stream} instance and reclaims the memory occupied by it.
 *
 * @param stream Represents a pointer to an {@link ContentEmbed_Stream} instance.
 * After this call, the pointer becomes invalid and must not be used.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_DestroyStream(ContentEmbed_Stream *stream);

/**
 * @brief Destroys an {@link ContentEmbed_Document} instance and reclaims the memory occupied by it.
 *
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance.
 * After this call, the pointer becomes invalid and must not be used.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_DestroyDocument(ContentEmbed_Document *document);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/** @} */
#endif // OHOS_CONTENT_EMBED_DOCUMENT_H