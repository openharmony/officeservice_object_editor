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

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
constexpr int32_t DEFAULT_USER_ID = -1;
constexpr int32_t RETRY_TIMES = 1;
}

void ObjectEditorConnection::OnAbilityConnectDone(const AppExecFwk::ElementName &element,
    const sptr<IRemoteObject> &remoteObject, int32_t resultCode)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s, ret: %{public}d",
        element.GetBundleName().c_str(), element.GetModuleName().c_str(),
        element.GetAbilityName().c_str(), resultCode);
    std::unique_lock<std::mutex> lock(extensionProxyMutex_);
    extensionProxy_ = remoteObject;
    isConnectReady_ = true;
    connectCondition_.notify_all();
}

void ObjectEditorConnection::OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s, ret: %{public}d",
        element.GetBundleName().c_str(), element.GetModuleName().c_str(),
        element.GetAbilityName().c_str(), resultCode);
    std::unique_lock<std::mutex> lock(extensionProxyMutex_);
    extensionProxy_ = nullptr;
    isConnectReady_ = false;
    connectCondition_.notify_all();
}

ObjectEditorManagerErrCode ObjectEditorConnection::StartConnect(
    const std::string &bundleName, const std::string &abilityName,
    const std::string &moduleName, sptr<IRemoteObject> &remoteObject)
{
    auto errCode = DoConnect(bundleName, abilityName, moduleName, remoteObject);
    if (errCode == ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
        return errCode;
    }
    int32_t retryTimes = RETRY_TIMES;
    do {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "retry connect, times: %{public}d", retryTimes);
        errCode = DoConnect(bundleName, abilityName, moduleName, remoteObject);
        if (errCode == ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
            break;
        }
        retryTimes--;
    } while (retryTimes > 0);
    return errCode;
}

ObjectEditorManagerErrCode ObjectEditorConnection::StopConnect()
{
    auto abilityManagerClient = AppExecFwk::AbilityManagerClient::GetInstance();
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
    std::unique_lock<std::mutex> lock(extensionProxyMutex_);
    if (extensionProxy_ != nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s is connected",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED;
    }
    auto abilityManagerClient = AppExecFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "abilityManagerClient is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    AAFwk::Want want;
    want.SetModuleName(moduleName);
    want.SetElementName(bundleName, abilityName);
    int32_t ret = abilityManagerClient->ConnectExtensionAbility(want, this, DEFAULT_USER_ID);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "ConnectExtensionAbility failed: %{public}d", ret);
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    isConnectReady_ = false;
    auto waitStatus = connectCondition_.wait(lock, std::chrono::seconds(CONNECT_TIMEOUT),
        [this] { return isConnectReady_; });
    if (!waitStatus) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s connect timeout",
            bundleName.c_str(), moduleName.c_str(), abilityName.c_str());
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    remoteObject = extensionProxy_;
    return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED;
}

// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
