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

#include "object_editor_extension.h"

#include "ability_loader.h"
#include "configuration_utils.h"
#include "connection_manager.h"
#include "hilog_object_editor.h"
#include "object_editor_extension_context.h"
#include "runtime.h"
#include "system_ability_definition.h"
#include "object_editor_extension_ability_stub.h"
#include "iservice_registry.h"
#include "want_manager.h"
#include "native_runtime.h"

namespace OHOS {
namespace AbilityRuntime {
// LCOV_EXCL_START
using namespace OHOS::AppExecFwk;
using namespace OHOS::ObjectEditor;

ObjectEditorExtension *ObjectEditorExtension::Create(
    const std::unique_ptr<Runtime> &runtime)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    if (runtime == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "runtime is null");
    }
    return new ObjectEditorExtension();
}

void ObjectEditorExtension::Init(const std::shared_ptr<AbilityLocalRecord> &record,
                                 const std::shared_ptr<OHOSApplication> &application,
                                 std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    ExtensionBase<ObjectEditorExtensionContext>::Init(record, application, handler, token);

    ceInstance_ = std::make_shared<struct ContentEmbed_ExtensionInstance>();
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create ceInstance");
        return;
    }
    ceInstance_->extension = weak_from_this(); // 基类extension

    ceContext_ = std::make_shared<struct ContentEmbed_ExtensionContext>();
    if (ceContext_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create ceContext");
        return;
    }
    auto context = GetContext();
    if (context) {
        ceContext_->context = context->weak_from_this();
    }

    std::string srcPath;
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to get srcPath");
        return;
    }

    if (Extension::abilityInfo_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityInfo_ is null");
        return;
    }
    std::string bundleModuleName(Extension::abilityInfo_->bundleName);
    std::string moduleName(Extension::abilityInfo_->moduleName);
    bundleModuleName.append("/").append(moduleName);
    std::string src = srcPath.substr(srcPath.find_last_of("/") + 1);
    bool ret = NativeRuntime::LoadModule(bundleModuleName, src, abilityInfo_->name, *ceInstance_);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION,
                       "LoadModule, bundleModuleName: %{public}s, srcPath: %{public}s, "
                       "ret: %{public}d",
                       bundleModuleName.c_str(), srcPath.c_str(), ret);
    handler_ = handler;
    ListenWindowManager();
}

std::shared_ptr<ObjectEditorExtensionContext> ObjectEditorExtension::CreateAndInitContext(
    const std::shared_ptr<AbilityLocalRecord> &record, const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    std::shared_ptr<ObjectEditorExtensionContext> context =
        ExtensionBase<ObjectEditorExtensionContext>::CreateAndInitContext(record, application, handler, token);
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create context");
        return context;
    }
    return context;
}

void ObjectEditorExtension::GetSrcPath(std::string &srcPath)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    if (Extension::abilityInfo_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityInfo_ is null");
        return;
    }
    if (!Extension::abilityInfo_->srcEntrance.empty()) {
        srcPath.append(Extension::abilityInfo_->moduleName + "/");
        srcPath.append(Extension::abilityInfo_->srcEntrance);
    }
}

void ObjectEditorExtension::ListenWindowManager()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    auto abilityManager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (abilityManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get SaMgr");
        return;
    }

    auto objectEditorExtension = std::static_pointer_cast<ObjectEditorExtension>(shared_from_this());
    displayListener_ = sptr<ObjectEditorExtensionDisplayListener>::MakeSptr(objectEditorExtension);
    if (displayListener_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to create display listener");
        return;
    }
    auto listener = sptr<SystemAbilityStatusChangeListener>::MakeSptr(displayListener_);
    if (listener == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to create status change listener");
        return;
    }

    auto ret = abilityManager->SubscribeSystemAbility(WINDOW_MANAGER_SERVICE_ID, listener);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION,
            "failed to subscribe window manager sa!, ret: %{public}d", ret);
    }
}

void ObjectEditorExtension::SystemAbilityStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId,
                                                                                  const std::string &deviceId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "systemAbilityId: %{public}d, deviceId: %{public}s",
        systemAbilityId, deviceId.c_str());
    if (systemAbilityId == WINDOW_MANAGER_SERVICE_ID) {
        Rosen::DisplayManager::GetInstance().RegisterDisplayListener(listener_);
    }
}

void TransformToCWantElement(AbilityBase_Element &element, const AAFwk::Want &want)
{
    element.bundleName = new char[want.GetElement().GetBundleName().length() + 1];
    if (strcpy_s(element.bundleName, want.GetElement().GetBundleName().length() + 1,
                 want.GetElement().GetBundleName().c_str()) != 0) {
        delete[] element.bundleName;
        element.bundleName = nullptr;
        return;
    }
    element.moduleName = new char[want.GetElement().GetModuleName().length() + 1];
    if (strcpy_s(element.moduleName, want.GetElement().GetModuleName().length() + 1,
                 want.GetElement().GetModuleName().c_str()) != 0) {
        delete[] element.bundleName;
        delete[] element.moduleName;
        element.bundleName = nullptr;
        element.moduleName = nullptr;
        return;
    }
    element.abilityName = new char[want.GetElement().GetAbilityName().length() + 1];
    if (strcpy_s(element.abilityName, want.GetElement().GetAbilityName().length() + 1,
                 want.GetElement().GetAbilityName().c_str()) != 0) {
        delete[] element.bundleName;
        delete[] element.moduleName;
        delete[] element.abilityName;
        element.bundleName = nullptr;
        element.moduleName = nullptr;
        element.abilityName = nullptr;
        return;
    }
}

void DestroyCWantElement(AbilityBase_Element &element)
{
    if (element.bundleName != nullptr) {
        delete[] element.bundleName;
        element.bundleName = nullptr;
    }
    if (element.moduleName != nullptr) {
        delete[] element.moduleName;
        element.moduleName = nullptr;
    }
    if (element.abilityName != nullptr) {
        delete[] element.abilityName;
        element.abilityName = nullptr;
    }
}

void ObjectEditorExtension::OnStart(const AAFwk::Want &want)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ != nullptr && ceInstance_->onCreateFunc != nullptr) {
        AbilityBase_Want cWant;
        auto errCode = AAFwk::CWantManager::TransformToCWantWithoutElement(want, false, cWant);
        if (errCode != ABILITY_BASE_ERROR_CODE_NO_ERROR) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION,
                "failed to transform want to c want!, errCode: %{public}d", errCode);
            return;
        }
        AbilityBase_Element element;
        element.moduleName = nullptr;
        element.abilityName = nullptr;
        element.bundleName = nullptr;
        TransformToCWantElement(element, want);
        cWant.element = element;
        ceInstance_->onCreateFunc(ceInstance_.get(), &cWant);
        DestroyCWantElement(element);
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance_ is null or onCreateFunc is null!");
    }
}

void ObjectEditorExtension::OnStop()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    auto context = GetContext();
    if (!context) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get context!");
        return;
    }
    /* callback onDestroy */
    bool ret = ConnectionManager::GetInstance().DisconnectCaller(context->GetToken());
    if (ret) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION,
                           "the object editor extension connection is not disconnected.");
    }
    if (ceInstance_ != nullptr && ceInstance_->onDestroyFunc != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "call onDestroyFunc");
        ceInstance_->onDestroyFunc(ceInstance_.get());
    }
}

sptr<IRemoteObject> ObjectEditorExtension::OnConnect(const AAFwk::Want &want)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    Extension::OnConnect(want);
    auto remoteObj = this;
    if (remoteObj == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "remoteObj is null!");
        return nullptr;
    }
    return remoteObj;
}

void ObjectEditorExtension::OnDisconnect(const AAFwk::Want &want)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    Extension::OnDisconnect(want);
}

void ObjectEditorExtension::OnCommand(const AAFwk::Want &want, bool restart, int startId)
{
    Extension::OnCommand(want, restart, startId);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION,
        "start restart: %{public}d, startId: %{public}d", restart, startId);
}

void ObjectEditorExtension::OnCreate(Rosen::DisplayId displayId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "enter");
}

void ObjectEditorExtension::OnDestroy(Rosen::DisplayId displayId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "exit");
}

void ObjectEditorExtension::OnChange(Rosen::DisplayId displayId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "displayId: %{public}" PRIu64 "", displayId);
    auto context = GetContext();
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get context!");
        return;
    }
    auto contextConfig = context->GetConfiguration();
    if (contextConfig == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get context config!");
        return;
    }
}

std::shared_ptr<struct ContentEmbed_ExtensionContext> ObjectEditorExtension::GetCEContext() const
{
    return ceContext_;
}

ErrCode ObjectEditorExtension::GetSnapshot(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{private}s", documentId.c_str());
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
    auto iter = ceInstance_->objects.find(documentId);
    if (iter == ceInstance_->objects.end()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId not found");
        return ERR_INVALID_VALUE;
    }
    if (iter->second == nullptr || iter->second->onGetSnapshotFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetSnapshotFunc is nullptr");
        return ERR_INVALID_VALUE;
    }
    uint32_t bitmask = 0;
    iter->second->onGetCapabilityFunc(iter->second.get(), &bitmask);
    if ((bitmask & CE_CAPABILITY_SUPPORT_SNAPSHOT) == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "capability not support");
        return ObjectorEditorExtensionErrCode::EXTENSION_CAPABILITY_NOT_SUPPORT;
    }
    iter->second->onGetSnapshotFunc(iter->second.get());
    return ERR_OK;
}

ErrCode ObjectEditorExtension::DoEdit(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{private}s", documentId.c_str());
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
    auto iter = ceInstance_->objects.find(documentId);
    if (iter == ceInstance_->objects.end()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId not found");
        return ERR_INVALID_VALUE;
    }
    if (iter->second == nullptr || iter->second->onDoEditFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onDoEditFunc is nullptr");
        return ERR_INVALID_VALUE;
    }
    uint32_t bitmask = 0;
    iter->second->onGetCapabilityFunc(iter->second.get(), &bitmask);
    if ((bitmask & CE_CAPABILITY_SUPPORT_DO_EDIT) == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "capability not support");
        return ObjectorEditorExtensionErrCode::EXTENSION_CAPABILITY_NOT_SUPPORT;
    }
    iter->second->onDoEditFunc(iter->second.get());
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{private}s", documentId.c_str());
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    if (isEditing == nullptr || isModified == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "isEditing or isModified is null");
        return ERR_INVALID_VALUE;
    }
    {
        std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
        auto iter = ceInstance_->objects.find(documentId);
        if (iter == ceInstance_->objects.end()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId not found");
            return ERR_INVALID_VALUE;
        }
        if (iter->second == nullptr || iter->second->onGetEditStatusFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetEditStatusFunc is nullptr");
            return ERR_INVALID_VALUE;
        }
        iter->second->onGetEditStatusFunc(iter->second.get(), isEditing, isModified);
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "isEditing:%{public}d, isModified:%{public}d",
        *isEditing, *isModified);
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetExtensionEditStatus(bool &isEditing)
{
    isEditing = false;
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
    for (const auto &iter : ceInstance_->objects) {
        if (iter.second == nullptr || iter.second->onGetEditStatusFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetEditStatusFunc is nullptr");
            continue;
        }
        bool objectIsEditing = false;
        bool objectIsModified = false;
        iter.second->onGetEditStatusFunc(iter.second.get(), &objectIsEditing, &objectIsModified);
        if (objectIsEditing) {
            isEditing = true;
            break;
        }
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetCapability(const std::string &documentId, uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    if (bitmask == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "bitmask is null");
        return ERR_INVALID_VALUE;
    }
    {
        std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
        auto iter = ceInstance_->objects.find(documentId);
        if (iter == ceInstance_->objects.end()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId not found");
            return ERR_INVALID_VALUE;
        }
        if (iter->second == nullptr || iter->second->onGetCapabilityFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetCapabilityFunc is nullptr");
            return ERR_INVALID_VALUE;
        }
        iter->second->onGetCapabilityFunc(iter->second.get(), bitmask);
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "bitmask: %{public}u", *bitmask);
    return ERR_OK;
}

ErrCode ObjectEditorExtension::Close(const std::string &documentId, bool &isAllObjectsRemoved)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "documentId: %{private}s", documentId.c_str());
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null");
        return ERR_INVALID_VALUE;
    }
    std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
    isAllObjectsRemoved = ceInstance_->objects.size() == 0;
    auto iter = ceInstance_->objects.find(documentId);
    if (iter == ceInstance_->objects.end()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId not found");
        return ERR_INVALID_VALUE;
    }
    if (iter->second == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "object is nullptr");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = ERR_INVALID_VALUE;
    if (ceInstance_->onObjectDetachFunc != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "call onObjectDetachFunc");
        ceInstance_->onObjectDetachFunc(ceInstance_.get(), iter->second.get());
        ret = ERR_OK;
    }
    ceInstance_->objects.erase(iter);
    isAllObjectsRemoved = ceInstance_->objects.size() == 0;
    return ret;
}

bool CheckFilePermission(std::string filename)
{
    if (filename.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "filename is empty");
        return false;
    }
    return access(filename.c_str(), R_OK | W_OK) == 0;
}

bool CheckFileValid(const std::unique_ptr<ObjectEditorDocument> &document)
{
    if ((!document->GetTmpFileUri().has_value() && !document->GetLinking()) ||
        document->GetSnapshotUri().empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "tmpFileUri or snapshotUri is empty");
        return false;
    }
    if (!document->GetLinking() && !CheckFilePermission(document->GetTmpFilePath())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "tmpFilePath permission denied");
        return false;
    }
    if (!CheckFilePermission(document->GetSnapshotPath())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "snapshotPath permission denied");
        return false;
    }
    // 以现有文件方式插入需要校验nativeFileUri
    if (document->GetOperateType() == OperateType::CREATE_BY_FILE) {
        if (!CheckFilePermission(document->GetNativeFilePath())) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "nativeFilePath permission denied");
            return false;
        }
        // 如果同时是linking的场景，需要多校验原始文件
        if (document->GetLinking() && !CheckFilePermission(document->GetOriFilePath())) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "oriFilePath permission denied");
            return false;
        }
    }
    return true;
}

ErrCode ObjectEditorExtension::CreateObject(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCb)
{
    if (document == nullptr || ceInstance_ == nullptr) {
        return ERR_INVALID_VALUE;
    }
    std::string documentId = document->GetDocumentId();
    {
        std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
        auto iter = ceInstance_->objects.find(documentId);
        if (iter != ceInstance_->objects.end()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "documentId already initial");
            return ERR_INVALID_VALUE;
        }
    }
    std::unique_ptr<ContentEmbed_Object> object = std::make_unique<struct ContentEmbed_Object>();
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "create object failed");
        return ERR_INVALID_VALUE;
    }
    object->document = std::make_unique<struct ContentEmbed_Document>();
    if (object->document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "create document failed");
        return ERR_INVALID_VALUE;
    }
    object->ceInstance = ceInstance_;
    object->document->oeid = document->GetOEid();
    object->document->linking = document->GetLinking();
    object->document->oeDocumentInner = std::move(document);
    object->clientCb = clientCb;
    std::lock_guard<std::mutex> lock(ceInstance_->objectsMutex);
    ceInstance_->objects.insert({documentId, std::move(object)});
    auto iter = ceInstance_->objects.find(documentId);
    ceInstance_->onObjectAttachFunc(ceInstance_.get(), iter->second.get());
    if (iter->second->document->oeDocumentInner->GetOperateType() == OperateType::CREATE_BY_FILE) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "create document by file");
        if (iter->second->document->linking) {
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "create document by file linking");
            return ERR_OK;
        }
        if (iter->second->onWriteToDataStreamFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onWriteToDataStreamFunc is nullptr");
            ceInstance_->objects.erase(iter);
            return ERR_INVALID_VALUE;
        }
        iter->second->onWriteToDataStreamFunc(iter->second.get());
        return ERR_OK;
    }
    return ERR_OK;
}

ErrCode ObjectEditorExtension::Initial(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "extension");
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "document is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "clientCb is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!CheckFileValid(document)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "extension file invalid");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_->onObjectAttachFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onObjectAttachFunc is nullptr");
        return ERR_INVALID_VALUE;
    }
    document->RestoreStorage();
    document->FlushOEid();
    return CreateObject(std::move(document), clientCb);
}

int32_t ObjectEditorExtension::CallbackEnter([[maybe_unused]] uint32_t code)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "extension");
    return ERR_OK;
}

int32_t ObjectEditorExtension::CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "extension");
    return result;
}

ObjectEditorExtension::~ObjectEditorExtension()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "destructor");
}
// LCOV_EXCL_STOP
} // namespace AbilityRuntime
} // namespace OHOS