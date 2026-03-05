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
 * @file content_embed_extension.h
 *
 * @brief Defines the content embed extension APIs.
 *
 * @library libcontent_embed_ndk.so
 * @kit ContentEmbedKit
 * @syscap SystemCapability.ContentEmbed.ObjectEditor
 * @since 24
 */

#ifndef OHOS_CONTENT_EMBED_EXTENSION_H
#define OHOS_CONTENT_EMBED_EXTENSION_H

#include <stdint.h>
#include <stdlib.h>
#include "content_embed_common.h"
#include "extension_ability.h"
#include "ability_runtime/context.h"
#include "ability_runtime/start_options.h"
#include "want.h"
#include "want_manager.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/**
 * @brief Define the ContentEmbed_Document structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Document ContentEmbed_Document;

/**
 * @brief Define the ContentEmbed_ExtensionContext structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_ExtensionContext ContentEmbed_ExtensionContext;

/**
 * @brief Define the ContentEmbed_ExtensionContextHandle type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_ExtensionContext* ContentEmbed_ExtensionContextHandle;

/**
 * @brief Define the ContentEmbed_ExtensionInstance structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_ExtensionInstance ContentEmbed_ExtensionInstance;

/**
 * @brief Define the ContentEmbed_ExtensionInstanceHandle type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_ExtensionInstance* ContentEmbed_ExtensionInstanceHandle;

/**
 * @brief Define the ContentEmbed_Object structure type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Object ContentEmbed_Object;

/**
 * @brief Define the ContentEmbed_ObjectHandle type.
 *
 * Provides methods for Content Embed Kit.
 *
 * @since 24
 */
typedef struct ContentEmbed_Object* ContentEmbed_ObjectHandle;

/**
 * @brief Get the extension context from the content embed extension instance.
 *
 * @param ceInstance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance.
 * @param ceContext Represents a pointer to an {@link ContentEmbed_ExtensionContext} instance that is a output param.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContentEmbedContext(ContentEmbed_ExtensionInstanceHandle ceInstance,
                                                                        ContentEmbed_ExtensionContextHandle *ceContext);

/**
 * @brief Get the ability runtime context from the content embed extension context.
 *
 * @param ceContext Represents a pointer to an {@link ContentEmbed_ExtensionContext} instance.
 * @param context Represents a pointer to an {@link AbilityRuntime_Context} instance that is a output param.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContext(ContentEmbed_ExtensionContextHandle ceContext,
                                                            AbilityRuntime_ContextHandle *context);

/**
 * @brief Get the content embed extension instance from base extension instance.
 *
 * @param baseExtensionInstance Represents a pointer to an {@link AbilityRuntime_ExtensionInstance} instance.
 * @param ceExtensionInstance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance
 *                            that is a output param.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetExtensionInstance(
    AbilityRuntime_ExtensionInstanceHandle baseExtensionInstance,
    ContentEmbed_ExtensionInstanceHandle *ceExtensionInstance);

/**
 * @brief Called when the content embed extension instance is created.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ExtensionInstance} through {@link
 * OH_ContentEmbed_Extension_RegisterOnCreateFunc} to complete the registration.
 *
 * @param instance Represents a pointer to an
 *                 {@link ContentEmbed_ExtensionInstance} instance which will be set in.
 * @param want Represents a pointer to an {@link AbilityBase_Want} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnCreateFunc)(
    ContentEmbed_ExtensionInstanceHandle instance, AbilityBase_Want *want);

/**
 * @brief Called when the content embed extension instance is destroyed.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ExtensionInstance} through {@link
 * OH_ContentEmbed_Extension_RegisterOnDestroyFunc} to complete the registration.
 *
 * @param instance Represents a pointer to an
 *                 {@link ContentEmbed_ExtensionInstance} instance which will be set in.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnDestroyFunc)(
    ContentEmbed_ExtensionInstanceHandle instance);

/**
 * @brief Called when the content embed extension instance is attached to a content embed object.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ExtensionInstance} through {@link
 * OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc} to complete the registration.
 *
 * @param instance Represents a pointer to an
 *                 {@link ContentEmbed_ExtensionInstance} instance which will be set in.
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnObjectAttachFunc)(
    ContentEmbed_ExtensionInstanceHandle instance, ContentEmbed_ObjectHandle object);

/**
 * @brief Called when the content embed extension instance is detached from a content embed object.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ExtensionInstance} through {@link
 * OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc} to complete the registration.
 *
 * @param instance Represents a pointer to an
 *                 {@link ContentEmbed_ExtensionInstance} instance which will be set in.
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnObjectDetachFunc)(
    ContentEmbed_ExtensionInstanceHandle instance, ContentEmbed_ObjectHandle object);

/**
 * @brief Called when the content embed object instance is written to data stream.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ObjectHandle} through {@link
 * OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc} to complete the registration.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnWriteToDataStreamFunc)(
    ContentEmbed_ObjectHandle object);

/**
 * @brief Called when the content embed object instance gets a snapshot.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ObjectHandle} through {@link
 * OH_ContentEmbed_Extension_RegisterOnGetSnapShotFunc} to complete the registration.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnGetSnapshotFunc)(
    ContentEmbed_ObjectHandle object);

/**
 * @brief Called when the content embed object instance edits.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ObjectHandle} through {@link
 * OH_ContentEmbed_Extension_RegisterOnDoEditFunc} to complete the registration.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnDoEditFunc)(
    ContentEmbed_ObjectHandle object);

/**
 * @brief Called when the content embed object instance gets the edit status.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ObjectHandle} through {@link
 * OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc} to complete the registration.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param isEditing Indicates that the content embed document is being edited.
 * @param isModified Indicates that the content embed document has been modified.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnGetEditStatusFunc)(
    ContentEmbed_ObjectHandle object, bool *isEditing, bool *isModified);

/**
 * @brief Called when the content embed object instance gets a capability.
 *
 * You need to implement this function, set it to {@link ContentEmbed_ObjectHandle} through {@link
 * OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc} to complete the registration.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param bitmask Indicates the capabilities possessed by a content embed object instance.
 * @since 24
 */
typedef void (*OH_ContentEmbed_Extension_OnGetCapabilityFunc)(
    ContentEmbed_ObjectHandle object, uint32_t *bitmask);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnCreateFunc} into
 *        {@link ContentEmbed_ExtensionInstance}.
 *
 * @param instance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance
 *                 which will be set function in.
 * @param onCreateFunc Represents function {@link OH_ContentEmbed_Extension_OnCreateFunc} which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnCreateFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnCreateFunc onCreateFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnDestroyFunc} into
 *        {@link ContentEmbed_ExtensionInstance}.
 *
 * @param instance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance
 *                 which will be set function in.
 * @param onDestroyFunc Represents function {@link OH_ContentEmbed_Extension_OnDestroyFunc} which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDestroyFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnDestroyFunc onDestroyFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnObjectAttachFunc} into
 *        {@link ContentEmbed_ExtensionInstance}.
 *
 * @param instance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance
 *                 which will be set function in.
 * @param onObjectAttachFunc Represents function {@link OH_ContentEmbed_Extension_OnObjectAttachFunc}
 *                           which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(
    ContentEmbed_ExtensionInstanceHandle instance,
    OH_ContentEmbed_Extension_OnObjectAttachFunc onObjectAttachFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnObjectDetachFunc} into
 *        {@link ContentEmbed_ExtensionInstance}.
 *
 * @param instance Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance
 *                 which will be set function in.
 * @param onObjectDetachFunc Represents function {@link OH_ContentEmbed_Extension_OnObjectDetachFunc}
 *                           which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(
    ContentEmbed_ExtensionInstanceHandle instance,
    OH_ContentEmbed_Extension_OnObjectDetachFunc onObjectDetachFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnWriteToDataStreamFunc} into
 *        {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance
 *                 which will be set function in.
 * @param onWriteToDataStreamFunc Represents function {@link OH_ContentEmbed_Extension_OnWriteToDataStreamFunc}
 *                                which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(
    ContentEmbed_ObjectHandle object,
    OH_ContentEmbed_Extension_OnWriteToDataStreamFunc onWriteToDataStreamFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnGetSnapshotFunc} into
 *        {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance
 *                 which will be set function in.
 * @param onGetSnapshotFunc Represents function {@link OH_ContentEmbed_Extension_OnGetSnapshotFunc}
 *                          which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(
    ContentEmbed_ObjectHandle object, OH_ContentEmbed_Extension_OnGetSnapshotFunc onGetSnapShotFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnDoEditFunc} into
 *        {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance
 *                 which will be set function in.
 * @param onDoEditFunc Represents function {@link OH_ContentEmbed_Extension_OnDoEditFunc} which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDoEditFunc(
    ContentEmbed_ObjectHandle object, OH_ContentEmbed_Extension_OnDoEditFunc onDoEditFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnGetEditStatusFunc} into
 *        {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance
 *                 which will be set function in.
 * @param onGetEditStatusFunc Represents function {@link OH_ContentEmbed_Extension_OnGetEditStatusFunc}
 *                            which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(
    ContentEmbed_ObjectHandle object, OH_ContentEmbed_Extension_OnGetEditStatusFunc onGetEditStatusFunc);

/**
 * @brief Register function {@link OH_ContentEmbed_Extension_OnGetCapabilityFunc} into
 *        {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance
 *                 which will be set function in.
 * @param onGetCapabilityFunc Represents function {@link OH_ContentEmbed_Extension_OnGetCapabilityFunc}
 *                            which will be set in.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc(
    ContentEmbed_ObjectHandle object, OH_ContentEmbed_Extension_OnGetCapabilityFunc onGetCapabilityFunc);

/**
 * @brief Get {@link ContentEmbed_Document} instance from {@link ContentEmbed_Object}.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param document Represents a pointer to an {@link ContentEmbed_Document} instance that is a output param.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContentEmbedDocument(
    ContentEmbed_ObjectHandle object, ContentEmbed_Document **document);

/**
 * @brief Invoke the OnUpdate callback registered by the client.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED} - client callback not registered.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_CLIENT_CALLBACK_FAILED} - the client callback fails.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnUpdate(
    ContentEmbed_ObjectHandle object);

/**
 * @brief Invoke the OnError callback registered by the client.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param code Indicates the error code.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED} - client callback not registered.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_CLIENT_CALLBACK_FAILED} - the client callback fails.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnError(
    ContentEmbed_ObjectHandle object, ContentEmbed_ErrorCode code);

/**
 * @brief Invoke the OnEditingFinished callback registered by the client.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param dataModified Indicates whether the document data has been modified.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED} - client callback not registered.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_CLIENT_CALLBACK_FAILED} - the client callback fails.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnEditingFinished(
    ContentEmbed_ObjectHandle object, bool dataModified);

/**
 * @brief Invoke the OnExtensionStoped callback registered by the client.
 *
 * @param handle Represents a pointer to an {@link ContentEmbed_ExtensionInstance} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED} - client callback not registered.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_CLIENT_CALLBACK_FAILED} - the client callback fails.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(
    ContentEmbed_ExtensionInstanceHandle handle);

/**
 * @brief Send the document snapshot to the client application.
 *
 * @param object Represents a pointer to an {@link ContentEmbed_Object} instance.
 * @param pixelMap Document snapshot.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IMAGE_PACKER_OPERATION_FAILED} - the image packer operation failed.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_SetSnapshot(
    ContentEmbed_ObjectHandle object, OH_PixelmapNative *pixelMap);

/**
 * @brief Starts self UIAbility.
 *
 * @param context Represents a pointer to an {@link ContentEmbed_ExtensionContext} instance.
 * @param want The arguments passed to start self UIAbility.
 * For detail, see {@link AbilityBase_Want}.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_SYSTEM_ABNORMAL} - the system service works abnormally.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextStartSelfUIAbility(
    ContentEmbed_ExtensionContextHandle context, AbilityBase_Want *want);

/**
 * @brief Starts self UIAbility with start options.
 *
 * @param context Represents a pointer to an {@link ContentEmbed_ExtensionContext} instance.
 * @param want The arguments passed to start self UIAbility.
 * For detail, see {@link AbilityBase_Want}.
 * @param options The start options passed to start self UIAbility.
 * For detail, see {@link AbilityRuntime_StartOptions}.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_SYSTEM_ABNORMAL} - the system service works abnormally.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(
    ContentEmbed_ExtensionContextHandle context,
    AbilityBase_Want *want, AbilityRuntime_StartOptions *options);

/**
 * @brief Terminates self UIAbility.
 *
 * @param context Represents a pointer to an {@link ContentEmbed_ExtensionContext} instance.
 * @return Returns a specific error code.
 *     {@link CE_ERR_OK} - success.
 *     {@link CE_ERR_PARAM_INVALID} - parameter check failed.
 *     {@link CE_ERR_NULL_POINTER} - unexpected null pointer.
 *     {@link CE_ERR_SYSTEM_ABNORMAL} - the system service works abnormally.
 *     {@link CE_ERR_DEVICE_NOT_SUPPORTED} - the device is not supported.
 *     {@link CE_ERR_IN_DLP_SANDBOX} - application is in dlp sandbox.
 * Specific error codes can be referenced {@link ContentEmbed_ErrorCode}.
 * @since 24
 */
ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextTerminateAbility(
    ContentEmbed_ExtensionContextHandle context);

#ifdef __cplusplus
}
#endif /* __cplusplus */
/** @} */
#endif // OHOS_CONTENT_EMBED_EXTENSION_H