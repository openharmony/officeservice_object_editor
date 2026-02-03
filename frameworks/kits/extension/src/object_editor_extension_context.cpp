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

#include "object_editor_extension_context.h"
#include "ability_connection.h"
#include "ability_manager_client.h"
#include "hilog_object_editor.h"
#include "iservice_registry.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
const size_t ObjectEditorExtensionContext::CONTEXT_TYPE_ID(std::hash<const char *>{}("ObjectEditorExtensionContext"));
int ObjectEditorExtensionContext::ILLEGAL_REQUEST_CODE(-1);
using namespace OHOS::ObjectEditor;

ErrCode ObjectEditorExtensionContext::StartAbility(const AAFwk::Want &want) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in want");
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "get proxy failed");
        return ERR_INVALID_STATE;
    }
    std::unique_ptr<AAFwk::Want> innerWant = std::make_unique<AAFwk::Want>(want);
    ErrCode err = objectEditorManagerProxy->StartUIAbility(innerWant);
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "ret=%{public}d", err);
    if (err != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "StartAbility failed, err: %{public}d", err);
    }
    return err;
}

ErrCode ObjectEditorExtensionContext::StartAbility(
    const AAFwk::Want &want, const AAFwk::StartOptions &startOptions) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityManagerClient is nullptr");
        return ERR_NO_INIT;
    }
    ErrCode err = abilityManagerClient->StartAbility(want, startOptions, token_, ILLEGAL_REQUEST_CODE);
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "ret=%{public}d", err);
    if (err != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION,
            "StartAbility with startOptions failed, err: %{public}d", err);
    }
    return err;
}

ErrCode ObjectEditorExtensionContext::ConnectAbility(
    const AAFwk::Want &want, const sptr<AbilityConnectCallback> &connectCallback) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    ErrCode ret = ConnectionManager::GetInstance().ConnectAbility(token_, want, connectCallback);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "ConnectAbility failed, err: %{public}d", ret);
    }
    return ret;
}

ErrCode ObjectEditorExtensionContext::StartAbilityWithAccount(const AAFwk::Want &want, int accountId) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "accountId=%{public}d", accountId);
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityManagerClient is nullptr");
        return ERR_NO_INIT;
    }
    ErrCode err = abilityManagerClient->StartAbility(want, token_, ILLEGAL_REQUEST_CODE, accountId);
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "ret=%{public}d", err);
    if (err != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "failed, err: %{public}d", err);
    }
    return err;
}

ErrCode ObjectEditorExtensionContext::StartAbilityWithAccount(
    const AAFwk::Want &want, int accountId, const AAFwk::StartOptions &startOptions) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityManagerClient is nullptr");
        return ERR_NO_INIT;
    }
    ErrCode err = abilityManagerClient->StartAbility(want, startOptions, token_, ILLEGAL_REQUEST_CODE, accountId);
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "ret=%{public}d", err);
    if (err != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION,
            "StartAbility with startOptions failed, err: %{public}d", err);
    }
    return err;
}

bool ObjectEditorExtensionContext::ConnectAbilityWithAccount(
    const AAFwk::Want &want, int accountId, const sptr<AbilityConnectCallback> &connectCallback) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    ErrCode ret = ConnectionManager::GetInstance().ConnectAbilityWithAccount(token_, want, accountId, connectCallback);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "ConnectAbilityWithAccount ret: %{public}d", ret);
    return ret == ERR_OK;
}

ErrCode ObjectEditorExtensionContext::DisconnectAbility(
    const AAFwk::Want &want, const sptr<AbilityConnectCallback> &connectCallback) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::EXTENSION, "in");
    ErrCode ret = ConnectionManager::GetInstance().DisconnectAbility(token_, want.GetElement(), connectCallback);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "DisconnectAbility ret: %{public}d", ret);
    return ret;
}

ErrCode ObjectEditorExtensionContext::TerminateAbility() const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityManagerClient is nullptr");
        return ERR_NO_INIT;
    }
    ErrCode ret = abilityManagerClient->TerminateAbility(token_, ILLEGAL_REQUEST_CODE, nullptr);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "TerminateAbility ret: %{public}d", ret);
    return ret;
}

AppExecFwk::AbilityType ObjectEditorExtensionContext::GetAbilityInfoType() const
{
    std::shared_ptr<AppExecFwk::AbilityInfo> info = GetAbilityInfo();
    if (info == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "abilityInfo is nullptr");
        return AppExecFwk::AbilityType::UNKNOWN;
    }
    return info->type;
}

sptr<IObjectEditorManager> ObjectEditorExtensionContext::GetIObjectEditorManager() const
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "samgrProxy is nullptr");
        return nullptr;
    }
    sptr<IRemoteObject> object = samgrProxy->CheckSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID);
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::EXTENSION, "object is nullptr");
        return nullptr;
    }
    return iface_cast<IObjectEditorManager>(object);
}
// LCOV_EXCL_STOP
} // namespace AbilityRuntime
} // namespace OHOS
