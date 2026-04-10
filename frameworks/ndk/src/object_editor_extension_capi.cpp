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

#include <memory>
#include "native_object_editor_types.h"
#include "content_embed_extension.h"
#include "object_editor_extension.h"
#include "object_editor_config.h"

#include "want_manager.h"
#include "image_packer.h"
#include "start_options_impl.h"

// LCOV_EXCL_START
using namespace OHOS::ObjectEditor;

#ifdef __cplusplus
extern "C" {
#endif

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnCreateFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnCreateFunc onCreateFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onCreateFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onCreateFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onCreateFunc = onCreateFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDestroyFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnDestroyFunc onDestroyFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onDestroyFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onDestroyFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onDestroyFunc = onDestroyFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnObjectAttachFunc onObjectAttachFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onObjectAttachFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onObjectAttachFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectAttachFunc = onObjectAttachFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_UnRegisterOnObjectAttachFunc(
    ContentEmbed_ExtensionInstanceHandle instance)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectAttachFunc = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(
    ContentEmbed_ExtensionInstanceHandle instance, OH_ContentEmbed_Extension_OnObjectDetachFunc onObjectDetachFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onObjectDetachFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onObjectDetachFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectDetachFunc = onObjectDetachFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_UnRegisterOnObjectDetachFunc(
    ContentEmbed_ExtensionInstanceHandle instance)
{
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectDetachFunc = nullptr;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(
    ContentEmbed_ObjectHandle object,
    OH_ContentEmbed_Extension_OnWriteToDataStreamFunc onWriteToDataStreamFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onWriteToDataStreamFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onWriteToDataStreamFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onWriteToDataStreamFunc = onWriteToDataStreamFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(
    ContentEmbed_ObjectHandle object, OH_ContentEmbed_Extension_OnGetSnapshotFunc onGetSnapshotFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onGetSnapshotFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onGetSnapshotFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onGetSnapshotFunc = onGetSnapshotFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDoEditFunc(ContentEmbed_ObjectHandle object,
    OH_ContentEmbed_Extension_OnDoEditFunc onDoEditFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onDoEditFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onDoEditFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onDoEditFunc = onDoEditFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(ContentEmbed_ObjectHandle object,
    OH_ContentEmbed_Extension_OnGetEditStatusFunc onGetEditStatusFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (onGetEditStatusFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "onGetEditStatusFunc is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onGetEditStatusFunc = onGetEditStatusFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetCapabilityFunc(ContentEmbed_Object *object,
    OH_ContentEmbed_Extension_OnGetCapabilityFunc onGetCapabilityFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || onGetCapabilityFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onGetCapabilityFunc = onGetCapabilityFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContentEmbedContext(ContentEmbed_ExtensionInstanceHandle instance,
    ContentEmbed_ExtensionContextHandle *ceContext)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (ceContext == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "ceContext is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorExtension> extensionInstance =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorExtension>(instance->extension.lock());
    if (extensionInstance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "extensionInstance is null");
        return CE_ERR_NULL_POINTER;
    }
    *ceContext = (extensionInstance->GetCEContext()).get();
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContext(ContentEmbed_ExtensionContextHandle ceContext,
    AbilityRuntime_ContextHandle *context)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (ceContext == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "ceContext is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_PARAM_INVALID;
    }
    *context = static_cast<AbilityRuntime_ContextHandle>(ceContext);
    if (*context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetExtensionInstance(
    AbilityRuntime_ExtensionInstanceHandle baseInstance,
    ContentEmbed_ExtensionInstanceHandle *ceInstance)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (baseInstance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "baseInstance is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (ceInstance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "ceInstance is null");
        return CE_ERR_PARAM_INVALID;
    }
    *ceInstance = static_cast<ContentEmbed_ExtensionInstanceHandle>(baseInstance);
    if (*ceInstance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "ceInstance is null");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContentEmbedDocument(
    ContentEmbed_ObjectHandle object, ContentEmbed_Document **ceDocument)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "ceDocument is null");
        return CE_ERR_PARAM_INVALID;
    }
    *ceDocument = (object->document).get();
    if (*ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnUpdate(ContentEmbed_ObjectHandle object)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto clientCb = object->clientCb;
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "clientCb is null");
        return CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED;
    }
    auto ceDocument = (object->document).get();
    if (ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_NULL_POINTER;
    }
    auto errCode = clientCb->OnUpdate(ceDocument->oeDocumentInner);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnError(ContentEmbed_ObjectHandle object,
    ContentEmbed_ErrorCode code)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto clientCb = object->clientCb;
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "clientCb is null");
        return CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED;
    }
    auto errCode = clientCb->OnError(code);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnEditingFinished(ContentEmbed_ObjectHandle object,
    bool dataModified)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto clientCb = object->clientCb;
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "clientCb is null");
        return CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED;
    }

    auto errCode = clientCb->OnStopEdit(dataModified);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnExtensionStopped(
    ContentEmbed_ExtensionInstanceHandle instance)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "instance is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::lock_guard<std::mutex> lock(instance->objectsMutex);
    for (const auto &[objectId, object] : instance->objects) {
        if (object == nullptr) {
            continue;
        }
        auto clientCb = object->clientCb;
        if (clientCb == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object %{private}s clientCb is null", objectId.c_str());
            return CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED;
        }
        auto errCode = clientCb->OnExtensionStopped();
        if (errCode != OHOS::ERR_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object %{private}s clientCb is null", objectId.c_str());
            return CE_ERR_CLIENT_CALLBACK_FAILED;
        }
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_SetSnapshot(ContentEmbed_ObjectHandle object,
    OH_PixelmapNative *pixelmap)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (pixelmap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "pixelmap is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "object is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (object->document == nullptr || object->document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_NULL_POINTER;
    }
    std::string snapshotPath = object->document->oeDocumentInner->GetSnapshotPath();
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption option;
    option.format = "image/png";
    std::shared_ptr<OHOS::Media::PixelMap> pixelmapPtr = pixelmap->GetInnerPixelmap();
    if (pixelmapPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "pixelmap ptr is null");
        return CE_ERR_NULL_POINTER;
    }
    if (imagePacker.StartPacking(snapshotPath, option)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "packing failed");
        return CE_ERR_IMAGE_PACKER_OPERATION_FAILED;
    }
    if (imagePacker.AddImage(*pixelmapPtr)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "add image failed");
        return CE_ERR_IMAGE_PACKER_OPERATION_FAILED;
    }
    int64_t packedSize = 0;
    if (imagePacker.FinalizePacking(packedSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "finish packing failed");
        return CE_ERR_IMAGE_PACKER_OPERATION_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextStartSelfUIAbility(
    ContentEmbed_ExtensionContextHandle context, AbilityBase_Want *want)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (want == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "want is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorExtensionContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorExtensionContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    OHOS::AAFwk::Want innerWant;
    OHOS::AAFwk::CWantManager::TransformToWant(*want, false, innerWant);
    auto ret = contextPtr->StartAbility(innerWant);
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "start ability failed:%{public}d", ret);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(
    ContentEmbed_ExtensionContextHandle context, AbilityBase_Want *want,
    AbilityRuntime_StartOptions *options)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (want == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "want is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (options == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "options is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorExtensionContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorExtensionContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    OHOS::AAFwk::Want innerWant;
    OHOS::AAFwk::CWantManager::TransformToWant(*want, false, innerWant);
    OHOS::AAFWK::StartOptions startOptions = options->GetInnerStartOptions();
    auto ret = contextPtr->StartAbility(innerWant, startOptions);
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "start ability failed:%{public}d", ret);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextTerminateAbility(
    ContentEmbed_ExtensionContextHandle context)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorExtensionContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorExtensionContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    auto ret = contextPtr->TerminateAbility();
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "terminate ability failed:%{public}d", ret);
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

#ifdef __cplusplus
}
#endif
// LCOV_EXCL_STOP