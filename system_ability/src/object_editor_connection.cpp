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

#include "object_editor_connection.h"

#include <ipc_skeleton.h>
#include "object_editor_common.h"
#include "object_editor_extension_proxy.h"
#include "object_editor_event_manager.h"
#include "object_editor_manager_system_ability.h"
#include "user_mgr.h"
#include "caller_info.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
constexpr int32_t RETRY_TIMES = 1;
constexpr int32_t MAX_CHECK_REMOTE_EDITSTATUS_TIMES = 1;
constexpr int32_t CONNECT_TIMEOUT = 3;
}

std::string& ObjectEditorConnection::GetExtensionBundleName()
{
    return extensionBundleName_;
}

void ObjectEditorConnection::SetClientBundleName(const std::string &bundleName)
{
    clientBundleName_ = bundleName;
}

std::string& ObjectEditorConnection::GetClientBundleName()
{
    return clientBundleName_;
}

bool ObjectEditorConnection::IsExtensionAbilityMatch(const std::string &moduleName,
    const std::string &abilityName)
{
    return moduleName == extensionModuleName_ && abilityName == extensionAbilityName_;
}

void ObjectEditorConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s, ret: %{public}d",
        element.GetBundleName().c_str(), element.GetModuleName().c_str(),
        element.GetAbilityName().c_str(), resultCode);
    std::unique_lock<std::mutex> uniqueProxyLock(extensionProxyMutex_);
    extensionProxy_ = remoteObject;
    isConnectReady_ = true;
    connectCondition_.notify_all();
    ResetStopExtensionTimer();
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    if (callback_ != nullptr) {
        callback_->OnConnectionStatusChanged(extensionProxy_,
            ObjectEditorConnectionStatus::STATUS_CONNECTED);
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "callback is null");
    }
}

void ObjectEditorConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s, ret: %{public}d",
        element.GetBundleName().c_str(), element.GetModuleName().c_str(),
        element.GetAbilityName().c_str(), resultCode);
    std::unique_lock<std::mutex> uniqueProxyLock(extensionProxyMutex_);
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    if (callback_ != nullptr) {
        callback_->OnConnectionStatusChanged(extensionProxy_,
            ObjectEditorConnectionStatus::STATUS_DISCONNECTED);
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "callback is null");
    }
    extensionProxy_ = nullptr;
    isConnectReady_ = false;
    connectCondition_.notify_all();
}

ObjectEditorManagerErrCode ObjectEditorConnection::StartConnect(const std::string &bundleName,
    const std::string &abilityName, const std::string &moduleName, sptr<IRemoteObject> &remoteObject)
{
    auto errCode = DoConnect(bundleName, abilityName, moduleName, remoteObject);
    if (errCode == ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
        return errCode;
    }
    int32_t currRetryTimes = RETRY_TIMES;
    do {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "retry connect, times: %{public}d", currRetryTimes);
        errCode = DoConnect(bundleName, abilityName, moduleName, remoteObject);
        if (errCode == ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
            break;
        }
        currRetryTimes--;
    } while (currRetryTimes > 0);
    return errCode;
}

void ObjectEditorConnection::RegisterConnectionStatusCallback(
    const std::shared_ptr<IObjectEditorConnectionStatusCallback> callback)
{
    std::unique_lock<std::mutex> callbackLock(callbackMutex_);
    if (callback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "callback is null");
        return;
    }
    callback_ = callback;
}

void ObjectEditorConnection::ResetStopExtensionTimer()
{
    bool expected = false;
    if (timerRunning_.compare_exchange_strong(expected, true)) {
        if (timerThread_.joinable()) {
            timerStopFlag_.store(true);
            {
                std::unique_lock<std::mutex> lock(mutexTimerRunning_);
                timerNotify_.store(true);
                cvTimer_.notify_one();
            }
            timerThread_.join();
            timerStopFlag_.store(false);
        }
        timerThread_ = std::thread([this]() { TimerThreadStopExtension(); });
    } else {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        timerNotify_.store(true);
        cvTimer_.notify_one();
    }
}

void ObjectEditorConnection::TimerThreadStopExtension()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    while (!timerStopFlag_.load()) {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "start wait %{public}ds", EXTENSION_STOP_TIME_S);
        auto waitResult = cvTimer_.wait_for(lock, std::chrono::seconds(EXTENSION_STOP_TIME_S),
                                            [this]() { return timerNotify_.load() || timerStopFlag_.load(); });
        if (!waitResult) {
            bool isScreenLocked = ObjectEditorEventManager::GetInstance().CheckIsScreenLocked();
            if (isScreenLocked && curCheckEditStatusTimes_ >= MAX_CHECK_REMOTE_EDITSTATUS_TIMES) {
                timerNotify_.store(false);
                continue;
            }
            if (isScreenLocked && curCheckEditStatusTimes_ < MAX_CHECK_REMOTE_EDITSTATUS_TIMES) {
                curCheckEditStatusTimes_++;
            }
            if (!isScreenLocked) {
                curCheckEditStatusTimes_ = 0;
            }
            bool isEditing = CheckRemoteEditStatus();
            if (isEditing) {
                OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "server is editing");
                timerNotify_.store(false);
                continue;
            }
            ObjectEditorManagerSystemAbility::GetInstance().RegisterExtensionStopReason(
                extensionProxy_, ExtensionStopReason::SA_CLEAN_IDLE);
            StopConnect();
            break;
        }
        timerNotify_.store(false);
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "wait finish");
    }
    timerRunning_.store(false);
}

bool ObjectEditorConnection::CheckRemoteEditStatus()
{
    bool isEditing = false;
    std::unique_lock<std::mutex> uniqueProxyLock(extensionProxyMutex_);
    if (extensionProxy_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "extensionProxy is null");
        return isEditing;
    }
    auto objectEditorExtensionProxy = iface_cast<ObjectEditorExtensionProxy>(extensionProxy_);
    if (objectEditorExtensionProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "objectEditorExtensionProxy is null");
        return isEditing;
    }
    objectEditorExtensionProxy->GetExtensionEditStatus(isEditing);
    return isEditing;
}

ObjectEditorManagerErrCode ObjectEditorConnection::StopConnect()
{
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "abilityManagerClient is null");
        return ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED;
    }
    int32_t ret = abilityManagerClient->DisconnectAbility(this);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "DisconnectAbility result: %{public}d", ret);
    return ret == ERR_OK ? ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_SUCCEED :
        ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED;
}

ObjectEditorManagerErrCode ObjectEditorConnection::DoConnect(
    const std::string &bundleName, const std::string &abilityName,
    const std::string &moduleName, sptr<IRemoteObject> &remoteObject)
{
    std::unique_lock<std::mutex> uniqueConnectLock(extensionProxyMutex_);
    if (extensionProxy_ != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s is connected",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        remoteObject = extensionProxy_;
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED;
    }
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "abilityManagerClient is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    int32_t clientPid = IPCSkeleton::GetCallingPid();
    AAFwk::Want want;
    want.SetModuleName(moduleName);
    want.SetElementName(bundleName, abilityName);
    want.SetParam("clientPid", clientPid);
    auto indirectCallerInfo = std::make_shared<AAFwk::IndirectCallerInfo>();
    if (indirectCallerInfo == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "indirectCallerInfo is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    indirectCallerInfo->tokenId = IPCSkeleton::GetCallingTokenID();
    indirectCallerInfo->callerUid = IPCSkeleton::GetCallingUid();
    indirectCallerInfo->callerPid = IPCSkeleton::GetCallingPid();
    auto ret = abilityManagerClient->ConnectAbilityWithIndirectCallerInfo(want, this, nullptr,
        UserMgr::GetInstance().GetUserId(), AppExecFwk::ExtensionAbilityType::CONTENT_EMBED, indirectCallerInfo);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "ConnectExtensionAbility failed: %{public}d", ret);
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    isConnectReady_ = false;
    auto waitStatus = connectCondition_.wait_for(uniqueConnectLock, std::chrono::seconds(CONNECT_TIMEOUT),
        [this] { return isConnectReady_; });
    if (!waitStatus) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s connect timeout",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_TIMEOUT;
    }
    if (extensionProxy_ == nullptr) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s connect failed",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    extensionBundleName_ = bundleName;
    extensionAbilityName_ = abilityName;
    extensionModuleName_ = moduleName;
    remoteObject = extensionProxy_;
    return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED;
}

ObjectEditorConnection::~ObjectEditorConnection()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s", extensionBundleName_.c_str());
    if (timerThread_.joinable()) {
        timerStopFlag_.store(true);
        {
            std::unique_lock<std::mutex> lock(mutexTimer_);
            timerNotify_.store(true);
            cvTimer_.notify_one();
        }
        timerThread_.join();
    }
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
