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

#include "image_packer.h"
#include "want_manager.h"

#include "content_embed_extension.h"
#include "native_object_editor_types.h"
#include "object_editor_config.h"
#include "object_editor_extension.h"

// LCOV_EXCL_START
using namespace OHOS::ObjectEditor;

#ifdef __cplusplus
extern "C" {
#endif

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnCreateFunc(ContentEmbed_ExtensionInstance *instance,
    OH_ContentEmbed_Extension_OnCreateFunc onCreateFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr || onCreateFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onCreateFunc = onCreateFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDestroyFunc(ContentEmbed_ExtensionInstance *instance,
    OH_ContentEmbed_Extension_OnDestroyFunc onDestroyFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr || onDestroyFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onDestroyFunc = onDestroyFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectAttachFunc(ContentEmbed_ExtensionInstance *instance,
    OH_ContentEmbed_Extension_OnObjectAttachFunc onObjectAttachFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr || onObjectAttachFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectAttachFunc = onObjectAttachFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnObjectDetachFunc(ContentEmbed_ExtensionInstance *instance,
    OH_ContentEmbed_Extension_OnObjectDetachFunc onObjectDetachFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (instance == nullptr || onObjectDetachFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    instance->onObjectDetachFunc = onObjectDetachFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnWriteToDataStreamFunc(ContentEmbed_Object *object,
    OH_ContentEmbed_Extension_OnWriteToDataStreamFunc onWriteToDataStreamFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || onWriteToDataStreamFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onWriteToDataStreamFunc = onWriteToDataStreamFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetSnapshotFunc(ContentEmbed_Object *object,
    OH_ContentEmbed_Extension_OnGetSnapshotFunc onGetSnapshotFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || onGetSnapshotFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onGetSnapshotFunc = onGetSnapshotFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnDoEditFunc(ContentEmbed_Object *object,
    OH_ContentEmbed_Extension_OnDoEditFunc onDoEditFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || onDoEditFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    object->onDoEditFunc = onDoEditFunc;
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_RegisterOnGetEditStatusFunc(ContentEmbed_Object *object,
    OH_ContentEmbed_Extension_OnGetEditStatusFunc onGetEditStatusFunc)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || onGetEditStatusFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
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

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_GetContext(ContentEmbed_ExtensionContextHandle ceContext,
    AbilityRuntime_ContextHandle *context)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (ceContext == nullptr || context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *context = static_cast<AbilityRuntime_ContextHandle *>(ceContext);
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
    if (baseInstance == nullptr || ceInstance == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
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
    ContentEmbed_Object object, ContentEmbed_Document **ceDocument)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    *ceDocument = (object->document).get();
    if (*ceDocument == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_NULL_POINTER;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnUpdate(ContentEmbed_Object object)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
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
    auto errCode = clientCb->onUpdate(ceDocument->oeDocumentInner);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnError(ContentEmbed_Object object,
    ContentEmbed_ErrorCode code)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
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
    auto errCode = clientCb->onError(code);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnEditingFinished(ContentEmbed_Object object,
    bool isModified)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    auto clientCb = object->clientCb;
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "clientCb is null");
        return CE_ERR_CLIENT_CALLBACK_NOT_REGISTERED;
    }

    auto errCode = clientCb->onStopEdit(isModified);
    if (errCode != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "failed: %{public}d", errCode);
        return CE_ERR_CLIENT_CALLBACK_FAILED;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_CallbackToOnExtensionStoped(
    ContentEmbed_ExtensionInstanceHandle instance)
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
    std::lock_guard<std::mutex> lock(instance->objectsMutex);
    for (const auto &[objectId, object] : instance->objects) {
        if (object == nullptr) {
            continue;
        }
        auto clientCb = object->clientCb;
        if (clientCb == nullptr) {
            OBJECT_EDITOR_LOGW(ObjectEditorDomain::CLIENT_NDK, "object %{public}s clientCb is null", objectId.c_str());
            continue;
        }
        auto errCode = clientCb->OnExtensionStoped(false);
        if (errCode != OHOS::ERR_OK) {
            OBJECT_EDITOR_LOGW(ObjectEditorDomain::CLIENT_NDK, "object %{public}s clientCb is null", objectId.c_str());
        }
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_SetSnapshot(ContentEmbed_Object object,
    OH_PixelMapNative *pixelMap)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (object == nullptr || pixelMap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    if (object->document == nullptr || object->document->oeDocumentInner == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "document is null");
        return CE_ERR_NULL_POINTER;
    }
    std::string snapshotPath = object->document->oeDocumentInner->GetSnapshotPath();
    OHOS::Media::ImagePacker imagePacker;
    OHOS::Media::PackOption packOption;
    packOption.format = "image/png";
    std::shared_ptr<OHOS::Media::PixelMap> pixelMapPtr = pixelMap->GetInnerPixelMap();
    if (pixelMapPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "pixelMap is null");
        return CE_ERR_NULL_POINTER;
    }
    if (imagePacker.StartPacking(snapshotPath, packOption)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "packing failed");
        return CE_ERR_IMAGE_PACKER_OPERATION_FAILED;
    }
    if (imagePacker.AddImage(*pixelMapPtr)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "add image failed");
        return CE_ERR_IMAGE_PACKER_OPERATION_FAILED;
    }
    int64_t packedSize = 0;
    if (imagePacker.FinishPacking(packedSize)) {
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
    if (context == nullptr || want == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorContextContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorContextContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    OHOS::AAFwk::Want innerWant;
    OHOS::AAFwk::CWantManager::TransformWant(*want, false, innerWant);
    auto ret = contextPtr->StartAbility(innerWant);
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "start ability failed");
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

ContentEmbed_ErrorCode OH_ContentEmbed_Extension_ContextStartSelfUIAbilityWithStartOptions(
    ContentEmbed_ExtensionContextHandle context, AbilityBase_Want *want,
    AbilityBase_StartOptions *options)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT_NDK, "in");
    auto supported = ObjectEditorConfig::GetInstance().CheckIsSupported();
    if (supported != CE_ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "not supported:%{public}d", supported);
        return supported;
    }
    if (context == nullptr || want == nullptr || startOptions == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "param is null");
        return CE_ERR_PARAM_INVALID;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorContextContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorContextContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    OHOS::AAFwk::Want innerWant;
    OHOS::AAFwk::CWantManager::TransformWant(*want, false, innerWant);
    auto ret = contextPtr->StartAbility(innerWant, options);
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "start ability failed");
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
        return CE_ERR_NULL_POINTER;
    }
    std::shared_ptr<OHOS::AbilityRuntime::ObjectEditorContextContext> contextPtr =
        std::static_pointer_cast<OHOS::AbilityRuntime::ObjectEditorContextContext>(context->context.lock());
    if (contextPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "context is null");
        return CE_ERR_NULL_POINTER;
    }
    auto ret = contextPtr->TerminateAbility();
    if (ret != OHOS::ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "terminate ability failed");
        return CE_ERR_SYSTEM_ABNORMAL;
    }
    return CE_ERR_OK;
}

#ifdef __cplusplus
}
#endif
// LCOV_EXCL_STOP