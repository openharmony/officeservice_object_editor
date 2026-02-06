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

#include <thread>
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
using namespace OHOS::AAExecFwk;
using namespace OHOS::ObjectEditor;

ObjectEditorExtension *ObjectEditorExtension::Create(
    const std::unique_ptr<Runtime> &runtime)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (runtime == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "runtime is null");
    }
    return new ObjectEditorExtension();
}

void ObjectEditorExtension::Init(const std::shared_ptr<AbilityLocalRecord> &record,
                                 const std::shared_ptr<OHOSApplication> &application,
                                 std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    ExtensionBase<ObjectEditorExtensionContext>::Init(record, application, handler, token);

    ceInstance_ = std::make_shared<struct ContentEmbed_ExtensionInstance>();
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create ceInstance");
        return;
    }
    ceInstance_->extension = weak_from_this(); // 基类extension

    ceContext_ = std::make_shared<ContentEmbed_ExtensionContext>();
    if (ceContext_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create ceContext");
        return;
    }
    auto context = GetContext();
    if (context) {
        ceContext_->context = context->weak_from_this();
    }
    document_ = std::make_shared<struct ContentEmbed_Document>();
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create document");
        return;
    }

    std::string srcPath;
    GetSrcPath(srcPath);
    if (srcPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to get srcPath");
        return;
    }

    if (Exttension::abilityInfo_ == nullptr) {
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
    handle_ = handler;
    ListenWindowManager();
}

std::shared_ptr<ObjectEditorExtensionContext> ObjectEditorExtension::CreateAndInitContext(
    const std::shared_ptr<AbilityLocalRecord> &record, const std::shared_ptr<OHOSApplication> &application,
    std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    std::shared_ptr<ObjectEditorExtensionContext> context =
        ExtensionBase<ObjectEditorExtensionContext>::CreateAndInitContext(record, application, handler, token);
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fail to create context");
        return nullptr;
    }
    std::unique_lock<std::mutex> lock(mutexExtensionContext_);
    extensionContext_ = context;
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
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get SaMgr!");
        return;
    }
    
    auto objectEditorExtension = std::static_pointer_cast<ObjectEditorExtension>(shared_from_this());
    displayListener_ = sptr<ObjectEditorExtensionDisplayListener>::MakeSptr(objectEditorExtension);
    if (displayListener_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to create display listener!");
        return;
    }
    auto listener = sptr<SystemAbilityStatusChangeListener>::MakeSptr(displayListener_);
    if (listener == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to create status change listener!");
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
        auto errCode = AAFwk::CWantManager::TransformToCWantWithoutElement(want, false, cwant);
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
        cwant.element = element;
        ceInstance_->onCreateFunc(ceInstance_.get(), &cwant);
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
        ceInstance_->onDestroyFunc(ceInstance_.get());
    }
}

void ObjectEditorExtension::TimerThreadStopExtension()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "enter TimerThreadStopExtension");
    while (true) {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "start wait 20s");
        auto waitResult = cvTimer_.wait_for(lock, std::chrono::seconds(EXTENSION_STOP_TIME_S),
                                            [this]() { return timerNotify_.load() == true; });
        if (!waitResult) {
            bool isEditing = false;
            bool isModified = false;
            GetEditStatus(&isEditing, &isModified);
            if (isEditing) {
                OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "server is editing");
                timerNotify_.store(false);
                continue;
            }
            std::unique_lock<std::mutex> lock(mutexExtensionContext_);
            if (extensionContext_ != nullptr) {
                extensionContext_->TerminateAbility();
            }
            break;
        }
        timerNotify_.store(false);
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "wait finish");
    }
    std::unique_lock<std::mutex> lock(mutexTimerRunning_);
    timerRunning_.store(false);
}

void ObjectEditorExtension::ResetStopExtensionTimer()
{
    bool expected = false;
    std::unique_lock<std::mutex> lock(mutexTimerRunning_);
    if (timerRunning_.compare_exchange_strong(expected, true)) {
        std::thread([this]() { TimerThreadStopExtension(); }).detach();
    } else {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        timerNotify_.store(true);
        cvTimer_.notify_one();
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
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
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
    if (!context) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get context!");
        return;
    }
    auto contextConfig = context->GetConfiguration();
    if (!contextConfig) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed to get context config!");
        return;
    }
}

std::shared_ptr<struct ContentEmbed_ExtensionContext> ObjectEditorExtension::GetCEContext() const
{
    return ceContext_;
}

std::shared_ptr<struct ContentEmbed_Document> ObjectEditorExtension::GetDocument() const
{
    return document_;
}

sptr<IObjectEditorClientCallback> ObjectEditorExtension::GetClientCb() const
{
    return clientCb_;
}

ErrCode ObjectEditorExtension::RegisterClientCB(const sptr<IObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    clientCb_ = clientCb;
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetSnapshot()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null!");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_->onGetSnapShotFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetSnapShotFunc is nullptr!");
        return ERR_INVALID_VALUE;
    }
    ceInstance_->onGetSnapShotFunc(ceInstance_.get());
    return ERR_OK;
}

ErrCode ObjectEditorExtension::DoEdit()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null!");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_->onDoEditFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onDoEditFunc is nullptr!");
        return ERR_INVALID_VALUE;
    }
    ceInstance_->onDoEditFunc(ceInstance_.get());
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetEditStatus(bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null!");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_->onGetEditStatusFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetEditStatusFunc is nullptr!");
        return ERR_INVALID_VALUE;
    }
    ceInstance_->onGetEditStatusFunc(ceInstance_.get(), isEditing, isModified);
    return ERR_OK;
}

ErrCode ObjectEditorExtension::GetCapability(uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (ceInstance_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is null!");
        return ERR_INVALID_VALUE;
    }
    if (ceInstance_->onGetCapabilityFunc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onGetCapabilityFunc is nullptr!");
        return ERR_INVALID_VALUE;
    }
    ceInstance_->onGetCapabilityFunc(ceInstance_.get(), bitmask);
    return ERR_OK;
}

ErrCode ObjectEditorExtension::Close()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    return ERR_OK;
}

bool CheckFilePermission(std::string fileName)
{
    if (fileName.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "fileName is empty!");
        return false;
    }
    return access(fileName.c_str(), R_OK | W_OK) == 0;
}

bool CheckFileValid(const std::unique_ptr<ObjectEditorDocument> &document)
{
    if (!document->GetTmpFileUri().has_value() || document->GetSnapshotUri().empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "tmpFileUri or snapshotUri is empty");
        return false;
    }
    if (!CheckFilePermission(document->GetTmpFilePath())) {
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

ErrCode ObjectEditorExtension::Initial(std::unique_ptr<ObjectEditorDocument> document)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    if (document == nullptr || document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "document is nullptr");
        return ERR_INVALID_VALUE;
    }
    if (!CheckFileValid(document)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "extension file invalid");
        return ERR_INVALID_VALUE;
    }
    document->RestoreStorage();
    document_->hmid = document->GetHmid();
    document->FlushHmid();
    document_->oeDocumentInner = std::move(document);
    if (document_->oeDocumentInner->GetOperateType() == OperateType::CREATE_BY_FILE) {
        if (ceInstance_ == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ceInstance is nullptr!");
            return ERR_INVALID_VALUE;
        }
        if (ceInstance_->onWriteToDataStreamFunc == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "onWriteToDataStreamFunc is nullptr!");
            return ERR_INVALID_VALUE;
        }
        ceInstance_->onWriteToDataStreamFunc(ceInstance_.get());
        return ERR_OK;
    }
    return ERR_OK;
}

int32_t ObjectEditorExtension::CallbackEnter([[maybe_unused]] uint32_t code)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    ResetStopExtensionTimer();
    return ERR_OK;
}

int32_t ObjectEditorExtension::CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    return result;
}
// LCOV_EXCL_STOP
} // namespace AbilityRuntime
} // namespace OHOS