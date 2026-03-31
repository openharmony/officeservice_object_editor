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

#include <fstream>
#include <iostream>
#include <vector>
#include <cerrno>

#include <cJSON.h>
#include "accesstoken_kit.h"
#include "ipc_skeleton.h"

#include "system_ability_definition.h"
#include "uri_permission_manager_client.h"
#include "want.h"
#include "common_func.h"
#include "utd_client.h"

#include "appexecfwk_errors.h"
#include "iservice_registry.h"
#include "common_event_manager.h"
#include "common_event_support.h"

#include "hilog_object_editor.h"
#include "object_editor_config.h"
#include "object_editor_err_code.h"
#include "object_editor_extension_proxy.h"
#include "object_editor_manager_database.h"
#include "object_editor_manager_system_ability.h"
#include "object_editor_manager_utils.h"
#include "object_editor_permission_utils.h"
#include "system_utils.h"
#include "user_mgr.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
const std::string DIVERSION_MAP_JSON_PATH = "/system/etc/office_service/object_editor_service/diversion_map.json";
constexpr int32_t ILLEGAL_REQUEST_CODE = -1;
constexpr int32_t MAX_CONNECTION_COUNT = 2;
}

IMPLEMENT_SINGLE_INSTANCE(ObjectEditorManagerSystemAbility);
const bool REGISTER_RESULT =
    SystemAbility::MakeAndRegisterAbility(&ObjectEditorManagerSystemAbility::GetInstance());
std::atomic<bool> ObjectEditorManagerSystemAbility::timerRunning_{false};
std::atomic<bool> ObjectEditorManagerSystemAbility::timerNotify_{false};
std::mutex ObjectEditorManagerSystemAbility::mutexTimer_;
std::condition_variable ObjectEditorManagerSystemAbility::cvTimer_;
std::mutex ObjectEditorManagerSystemAbility::connectionMapMutex_;
std::map<sptr<IRemoteObject>, sptr<ObjectEditorConnection>> ObjectEditorManagerSystemAbility::connectionMap_;
std::string ObjectEditorManagerSystemAbility::permissionClient_("ohos.permission.CONNECT_OBJECTEDITOR_EXTENSION");

ObjectEditorManagerSystemAbility::ObjectEditorManagerSystemAbility()
    : SystemAbility(OBJECT_EDITOR_SERVICE_SA_ID, true)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "constructor");
    ReadDiversionsJsonFile();
    ObjectEditorManagerDatabase::GetInstance().Init();
    InitScreenChangedCommonEventSubscriber();
}

ObjectEditorManagerSystemAbility::~ObjectEditorManagerSystemAbility()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "deconstructor");
}

void ObjectEditorManagerSystemAbility::InitScreenChangedCommonEventSubscriber()
{
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED);
    matchingSkills.AddEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    screenChangedReceiver_ = std::make_shared<ObjectEditorScreenChangeReceiver>(subscribeInfo);
    auto ret = OHOS::EventFwk::CommonEventManager::SubscribeCommonEvent(screenChangedReceiver_);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "subscribe common event ret:%{public}d", ret);
}

void ObjectEditorManagerSystemAbility::ResetScreenChangedCommonEventSubscriber()
{
    if (screenChangedReceiver_ == nullptr) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "screenChangedReceiver_ is null");
        return;
    }
    auto ret = OHOS::EventFwk::CommonEventManager::UnSubscribeCommonEvent(screenChangedReceiver_);
    screenChangedReceiver_ = nullptr;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "unsubscribe common event ret:%{public}d", ret);
}

void ObjectEditorManagerSystemAbility::OnStart()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (state_ == ServiceRunningState::STATE_RUNNING) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "already running");
        return;
    }
    bool res = Publish(this);
    if (!res) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "Publish failed");
    } else {
        state_ = ServiceRunningState::STATE_RUNNING;
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "Publish success");
    }
}

void ObjectEditorManagerSystemAbility::OnStop()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    ResetScreenChangedCommonEventSubscriber();
    if (state_ != ServiceRunningState::STATE_RUNNING) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "not running");
        return;
    }
    state_ = ServiceRunningState::STATE_NOT_START;
}

int32_t ObjectEditorManagerSystemAbility::OnIdle(const SystemAbilityOnDemandReason &idleReason)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    return 0;
}

void ObjectEditorManagerSystemAbility::ReadDiversionsJsonFile()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    std::ifstream inFile(DIVERSION_MAP_JSON_PATH);
    if (!inFile.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "open file failed");
        return;
    }
    std::string content((std::istreambuf_iterator<char>(inFile)),
        std::istreambuf_iterator<char>());
    inFile.close();
    if (content.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "file content is empty");
        return;
    }
    cJSON *json = cJSON_Parse(content.c_str());
    if (json == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "parse json failed");
        return;
    }
    cJSON *diversionsArray = cJSON_GetObjectItemCaseSensitive(json, "diversions");
    if (diversionsArray == nullptr || !cJSON_IsArray(diversionsArray)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get diversions array failed");
        cJSON_Delete(json);
        return;
    }
    int32_t diversionsArraySize = cJSON_GetArraySize(diversionsArray);
    for (int32_t i = 0; i < diversionsArraySize; i++) {
        cJSON *diversionItem = cJSON_GetArrayItem(diversionsArray, i);
        if (diversionItem == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get diversion item failed");
            continue;
        }
        cJSON *sourceHmid = cJSON_GetObjectItemCaseSensitive(diversionItem, "sourceHmid");
        cJSON *targetHmid = cJSON_GetObjectItemCaseSensitive(diversionItem, "targetHmid");
        cJSON *minVersion = cJSON_GetObjectItemCaseSensitive(diversionItem, "minVersion");
        if (!cJSON_IsString(sourceHmid) || !cJSON_IsString(targetHmid) || !cJSON_IsString(minVersion)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get field failed");
            continue;
        }
        ContentEmbed_Diversion diversion;
        diversion.sourceHmid = sourceHmid->valuestring;
        diversion.targetHmid = targetHmid->valuestring;
        diversion.minVersion = minVersion->valuestring;
        std::unique_lock lock(diversionMapMutex_);
        diversionMap_[diversion.sourceHmid] = diversion;
    }
    cJSON_Delete(json);
}

void ObjectEditorManagerSystemAbility::TimerThreadStopSA()
{
    while (true) {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        auto waitResult = cvTimer_.wait_for(lock, std::chrono::seconds(SA_EXIT_TIME_S),
            [this]() { return timerNotify_.load(); });
        std::unique_lock<std::mutex> lockExtension(connectionMapMutex_);
        if (!waitResult && connectionMap_.empty()) {
            auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (sam == nullptr) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get system ability manager failed");
                break;
            }
            int32_t result = sam->UnloadSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID);
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "UnloadSystemAbility result:%{public}d", result);
            break;
        }
        timerNotify_.store(false);
    }
    timerRunning_.store(false);
}

int32_t ObjectEditorManagerSystemAbility::CallbackEnter([[maybe_unused]] uint32_t code)
{
    ResetStopSATimer();
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "not supported");
        return ObjectEditorManagerErrCode::SA_ERR_DEVICE_NOT_SUPPORT;
    }
    if (!CheckCallingPermission(code)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "permission:%{public}s denied", permissionClient_.c_str());
        return ObjectEditorManagerErrCode::SA_PERMISSION_DENIED;
    }
    return ERR_NONE;
}

bool ObjectEditorManagerSystemAbility::CheckCallingPermission(uint32_t code)
{
    switch (static_cast<IObjectEditorManagerIpcCode>(code)) {
        case IObjectEditorManagerIpcCode::COMMAND_START_OBJECT_EDITOR_EXTENSION: 
        case IObjectEditorManagerIpcCode::COMMAND_GET_HMID_BY_FILE_EXTENSION:
        case IObjectEditorManagerIpcCode::COMMAND_GET_ICON_BY_HMID:
        case IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_NAME:
        case IObjectEditorManagerIpcCode::COMMAND_GET_FORMAT_BY_HMID_AND_LOCALE:
        case IObjectEditorManagerIpcCode::COMMAND_GET_FORMATS_BY_LOCALE:
        case IObjectEditorManagerIpcCode::COMMAND_STOP_OBJECT_EDITOR_EXTENSION: {
            return ObjectEditorPermissionUtils::CheckCallingPermission(permissionClient_);
        }
        case IObjectEditorManagerIpcCode::COMMAND_START_UI_ABILITY: {
            return true;
        }
        default: {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "unknown code:%{public}d", code);
            return false;
        }
    }
}

void ObjectEditorManagerSystemAbility::ResetStopSATimer()
{
    std::unique_lock<std::mutex> lock(mutexCallback_);
    bool expected = false;
    if (timerRunning_.compare_exchange_strong(expected, true)) {
        std::thread([this]() { TimerThreadStopSA(); }).detach();
    } else {
        timerNotify_.store(true);
        cvTimer_.notify_one();
    }
}

int32_t ObjectEditorManagerSystemAbility::CallbackExit([[maybe_unused]] uint32_t code,
    [[maybe_unused]] int32_t result)
{
    return result;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::GetObjectEditorFormat(
    const ObjectEditorDocument &document,
    std::unique_ptr<ObjectEditorFormat> &objectEditorFormat,
    bool &isPackageExtension)
{
    if (document.GetHmid() == PACKAGE_HMID &&
        (document.GetOperateType() == OperateType::CREATE_BY_HMID ||
         document.GetOperateType() == OperateType::EDIT)) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "hmid is package");
        isPackageExtension = true;
        return ObjectEditorManagerErrCode::SA_OK;
    }
    auto errCode = ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY;
    if (document.GetOperateType() == OperateType::CREATE_BY_FILE && document.GetOriFileUri().has_value()) {
        std::vector<std::unique_ptr<ObjectEditorFormat>> objectEditorFormats;
        std::string fileExt = SystemUtils::GetFileSuffix(document.GetOriFileUri().value());
        if (!fileExt.empty()) {
            errCode = ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatsByFileExt(
                fileExt, objectEditorFormats);
        }
        if (errCode == ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY) {
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "use package extension");
            isPackageExtension = true;
            return ObjectEditorManagerErrCode::SA_OK;
        }
        if (errCode == ObjectEditorManagerErrCode::SA_OK && !objectEditorFormats.empty()) {
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "use default app");
            errCode = HandleDefaultAppFormatPolicy(fileExt, objectEditorFormats, objectEditorFormat);
        }
    } else {
        errCode = HandleOperateHasHmid(document, objectEditorFormat);
    }
    return errCode;
}

ErrCode ObjectEditorManagerSystemAbility::StartObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject,
    bool &isPackageExtension)
{
    if (document == nullptr || objectEditorClientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "invalid parameter");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    auto errCode = CheckIsAllowStartExtension(*document);
    if (errCode != ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK) {
        return errCode;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "operation:%{public}d",
        document->GetOperateType());
    std::unique_ptr<ObjectEditorFormat> objectEditorFormat;
    errCode = GetObjectEditorFormat(*document, objectEditorFormat, isPackageExtension);
    if (isPackageExtension) {
        return ObjectEditorManagerErrCode::SA_OK;
    }
    if (errCode != ObjectEditorManagerErrCode::SA_OK || objectEditorFormat == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get object editor format failed");
        return errCode;
    }
    if (document->GetOperateType() == OperateType::CREATE_BY_FILE) {
        document->SetHmid(objectEditorFormat->hmid);
    }
    if (!GrantClientFileUriPermissionToServerExtension(*document, objectEditorFormat->bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "grant client file permission to server extension failed");
        return ObjectEditorManagerErrCode::SA_GRANT_PERMISSION_TO_SERVER_EXTENSION_FAILED;
    }
    bool ret = ConnectObjectEditorExtAbility(objectEditorFormat, oeExtensionRemoteObject);
    if (!ret || oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "connect object editor extension ability failed");
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED;
    }
    auto objectEditorExtensionProxy = iface_cast<ObjectEditorExtensionProxy>(oeExtensionRemoteObject);
    if (objectEditorExtensionProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "object editor extension proxy is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_EXTENSION_PROXY_IS_NULL;
    }
    std::string documentId = document->GetDocumentId();
    ErrCode proxyResult = objectEditorExtensionProxy->Initial(
        std::move(document), objectEditorClientCallback);
    if (proxyResult != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "start object editor extension failed");
        return ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_SEND_FAILED;
    }
    sptr<IRemoteObject> clientRemote = objectEditorClientCallback->AsObject();
    auto clientDeathRecipient = sptr<ObjectEditorClientDeathRecipient>::MakeSptr(documentId,
        oeExtensionRemoteObject);
    if (clientRemote == nullptr ||
        !clientRemote->AddDeathRecipient(clientDeathRecipient)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "add client death recipient failed");
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::HandleOperateHasHmid(
    const ObjectEditorDocument &document,
    std::unique_ptr<ObjectEditorFormat> &objectEditorFormat)
{
    objectEditorFormat = std::make_unique<ObjectEditorFormat>();
    auto errCode = ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatByHmid(
        document.GetHmid(), objectEditorFormat);
    if (errCode == ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY &&
        document.GetOperateType() == OperateType::EDIT) {
        std::string targetHmid;
        std::string minVersion;
        errCode = GetTargetHmid(document.GetHmid(), targetHmid, minVersion);
        if (errCode == ObjectEditorManagerErrCode::SA_OK && !targetHmid.empty() &&
            !minVersion.empty()) {
            errCode = ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatByHmidAndMinVersion(
                targetHmid, minVersion, objectEditorFormat);
        }
    }
    return errCode;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::HandleDefaultAppFormatPolicy(
    const std::string &fileExt,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats,
    std::unique_ptr<ObjectEditorFormat> &objectEditorFormat)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "fileExt: %{public}s", fileExt.c_str());
    std::string defaultAppBundleName;
    auto errCode = GetDefaultAppBundleNameByFileExt(fileExt, defaultAppBundleName);
    bool defaultAppFormatRegistered = false;
    if (errCode == ObjectEditorManagerErrCode::SA_OK && !defaultAppBundleName.empty()) {
        for (auto &format : formats) {
            if (format->bundleName == defaultAppBundleName) {
                objectEditorFormat = std::move(format);
                defaultAppFormatRegistered = true;
                OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "default app bundlename:%{public}s registered",
                    defaultAppBundleName.c_str());
                break;
            }
        }
    }
    if (!defaultAppFormatRegistered || defaultAppBundleName.empty()) {
        objectEditorFormat = std::move(formats.front());
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::GetDefaultAppBundleNameByFileExt(
    const std::string &fileExt, std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "fileExt:%{public}s bundleName:%{public}s",
        fileExt.c_str(), bundleName.c_str());
    std::string utdType;
    ErrCode errCode = UDMF::UtdClient::GetInstance().GetUniformDataTypeByFilenameExtension(fileExt, utdType);
    if (errCode != ERR_OK || utdType.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get utd type failed");
        return ObjectEditorManagerErrCode::SA_UTD_QUERY_FAILED;
    }
    sptr<AppExecFwk::IDefaultApp> defaultAppProxy = AppExecFwk::CommonFunc::GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get default app proxy failed");
        return ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED;
    }
    AppExecFwk::BundleInfo bundleInfo;
    errCode = defaultAppProxy->GetDefaultApplication(
        UserMgr::GetInstance().GetUserId(), utdType, bundleInfo);
    if (errCode != ERR_OK && errCode != ERR_BUNDLE_MANAGER_DEFAULT_APP_NOT_EXIST) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get default app bundlename failed");
        return ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED;
    }
    if (bundleInfo.name.empty()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "default app bundlename is empty");
    } else {
        bundleName = bundleInfo.name;
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "fileExt:%{public}s default app bundlename is %{public}s",
            fileExt.c_str(), bundleName.c_str());
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::CheckIsAllowStartExtension(
    const ObjectEditorDocument &document)
{
    if (document.GetOperateType() == OperateType::UNKNOWN) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "unknown operation");
        return ObjectEditorManagerErrCode::SA_UNKNOWN_OPERATE;
    }
    if (!ObjectEditorManagerDatabase::GetInstance().Initted()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "db not initted");
        return ObjectEditorManagerErrCode::SA_DB_ERR;
    }
    if (!CheckClientFileValid(document)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "client file invalid");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    return ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::GetTargetHmid(const std::string &sourceHmid,
    std::string &targetHmid, std::string &minVersion)
{
    std::shared_lock lock(diversionMapMutex_);
    auto it = diversionMap_.find(sourceHmid);
    if (it == diversionMap_.end()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "source hmid %{public}s get diversion failed",
            sourceHmid.c_str());
        return ObjectEditorManagerErrCode::SA_OK;
    }
    targetHmid = it->second.targetHmid;
    minVersion = it->second.minVersion;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "source hmid %{public}s get target hmid %{public}s "
        "min version %{public}s",
        sourceHmid.c_str(), targetHmid.c_str(), minVersion.c_str());
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::StopObjectEditorExtension(
    const std::string &documentId,
    const sptr<IRemoteObject> &oeExtensionRemoteObject, const bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "invalid parameter");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    auto objectEditorExtensionProxy = iface_cast<ObjectEditorExtensionProxy>(oeExtensionRemoteObject);
    if (objectEditorExtensionProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "object editor extension proxy is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_EXTENSION_PROXY_IS_NULL;
    }
    bool isAllObjectsRemoved = false;
    objectEditorExtensionProxy->Close(documentId, isAllObjectsRemoved);
    if (!isAllObjectsRemoved) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "not all objects removed");
        return ObjectEditorManagerErrCode::SA_OK;
    }
    std::lock_guard<std::mutex> lock(connectionMapMutex_);
    auto it = connectionMap_.find(oeExtensionRemoteObject);
    if (it == connectionMap_.end()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "object editor extension proxy not found");
        return ObjectEditorManagerErrCode::SA_NOT_FOUND_ABILITY_BY_REMOTE_OBJECT;
    }
    auto connection = it->second;
    ErrCode errCode = ObjectEditorManagerErrCode::SA_OK;
    if (connection != nullptr && !isPackageExtension) {
        errCode = connection->StopConnect();
        errCode = errCode != ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_SUCCEED ?
            errCode : ObjectEditorManagerErrCode::SA_OK;
    }
    return errCode;
}

ErrCode ObjectEditorManagerSystemAbility::GetHmidByFileExtension(const std::string &hmid,
    std::string &fileExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::GetIconByHmid(const std::string &hmid, std::string &resourceId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::GetFormatName(const std::string &hmid, const std::string &locale,
    std::string &formatName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::GetObjectEditorFormatByHmidAndLocale(const std::string &hmid,
    const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "hmid: %{public}s, locale: %{public}s",
        hmid.c_str(), locale.c_str());
    return ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatByHmidAndLocale(hmid,
        locale, format);
}

ErrCode ObjectEditorManagerSystemAbility::GetObjectEditorFormatsByLocale(const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "locale: %{public}s", locale.c_str());
    return ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatsByLocale(locale, formats);
}

ErrCode ObjectEditorManagerSystemAbility::StartUIAbility(const std::unique_ptr<AAFwk::Want> &want)
{
    if (want == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "want is null");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "ability manager client is null");
        return ObjectEditorManagerErrCode::SA_START_UIABILITY_FAILED;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "bundlename: %{public}s", want->GetBundle().c_str());
    ErrCode err = abilityManagerClient->StartAbility(*want, ILLEGAL_REQUEST_CODE,
        UserMgr::GetInstance().GetUserId());
    if (err != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "failed:%{public}d", err);
        return ObjectEditorManagerErrCode::SA_START_UIABILITY_FAILED;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

bool ObjectEditorManagerSystemAbility::CheckClientFileValid(const ObjectEditorDocument &document)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if ((!document.GetTmpFileUri().has_value() && !document.GetLinking()) ||
        document.GetSnapshotUri().empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "uri is empty");
        return false;
    }
    std::vector<std::string> uriVec;
    if (!document.GetLinking()) {
        uriVec.push_back(document.GetTmpFileUri().value());
    }
    uriVec.push_back(document.GetSnapshotUri());
    if (document.GetOperateType() == OperateType::CREATE_BY_FILE) {
        if (document.GetNativeFileUri().has_value()) {
            uriVec.push_back(document.GetNativeFileUri().value());
        }
    }
    auto tokenId = IPCSkeleton::GetCallingTokenID();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "tokenId = %{private}d", tokenId);
    auto readAndWritePermission = Want::FLAG_AUTH_READ_URI_PERMISSION |
        Want::FLAG_AUTH_WRITE_URI_PERMISSION;
    std::vector<bool> uriVecHasPermission = UriPermissionManagerClient::GetInstance()
        .CheckUriAuthorization(uriVec, readAndWritePermission, tokenId);

    auto it = std::find_if(uriVecHasPermission.begin(), uriVecHasPermission.end(),
        [](bool hasPermission) { return !hasPermission; });
    return it == uriVecHasPermission.end();
}

bool ObjectEditorManagerSystemAbility::GrantClientFileUriPermissionToServerExtension(
    const ObjectEditorDocument &document, const std::string &targetBundleName)
{
    std::vector<Uri> uriList;
    if (!document.GetTmpFileUri().has_value() && !document.GetLinking()) {
        uriList.push_back(Uri(document.GetTmpFileUri().value()));
    }
    if (!document.GetSnapshotUri().empty()) {
        uriList.push_back(Uri(document.GetSnapshotUri()));
    }
    if (document.GetOperateType() == OperateType::CREATE_BY_FILE) {
        if (document.GetNativeFileUri().has_value()) {
            uriList.push_back(Uri(document.GetNativeFileUri().value()));
        }
    }
    auto readAndWritePermission = Want::FLAG_AUTH_READ_URI_PERMISSION |
        Want::FLAG_AUTH_WRITE_URI_PERMISSION;
    return UriPermissionManagerClient::GetInstance().GrantUriPermissionPrivileged(uriList,
        readAndWritePermission, targetBundleName, 0) == ERR_OK;
}

void ObjectEditorManagerSystemAbilityConnectionStatusCallback::OnConnectionStatusChanged(
    sptr<IRemoteObject> &remoteObject, const ObjectEditorConnectionStatus status)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "status:%{public}d", status);
    if (status != ObjectEditorConnectionStatus::STATUS_DISCONNECTED) {
        return;
    }
    std::lock_guard<std::mutex> lock(ObjectEditorManagerSystemAbility::connectionMapMutex_);
    ObjectEditorManagerSystemAbility::connectionMap_.erase(remoteObject);
}

bool ObjectEditorManagerSystemAbility::CheckConnectionLimit(const std::string &clientBundleName,
    std::unique_ptr<ObjectEditorFormat> &format, sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "clientBundleName:%{public}s", clientBundleName.c_str());
    std::lock_guard<std::mutex> lock(connectionMapMutex_);
    int32_t count = 0;
    for (const auto &pair : connectionMap_) {
        if (pair.second == nullptr) {
            continue;
        }
        if (pair.second->GetExtensionBundleName() != format->bundleName) {
            count++;
            if (count >= MAX_CONNECTION_COUNT) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "max connection count reached");
                return false;
            }
        } else if (pair.second->IsExtensionAbilityMatch(format->moduleName, format->abilityName)) {
            if (pair.second->GetClientBundleName() != clientBundleName) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "extension client bundle name %{public}s not match",
                    pair.second->GetClientBundleName().c_str());
                return false;
            }
            remoteObject = pair.first;
            return true;
        }
    }
    return true;
}

std::string ObjectEditorManagerSystemAbility::GetCallerBundleName()
{
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    ErrCode ret = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetHapTokenInfo failed");
        return "";
    }
    return hapTokenInfo.bundleName;
}

bool ObjectEditorManagerSystemAbility::ConnectObjectEditorExtAbility(
    std::unique_ptr<ObjectEditorFormat> &format, sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "format is null");
        return false;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "%{public}s/%{public}s/%{public}s",
        format->bundleName.c_str(), format->moduleName.c_str(), format->abilityName.c_str());
    std::string clientBundleName = GetCallerBundleName();
    if (clientBundleName.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "client bundle name is empty");
        return false;
    }
    if (!CheckConnectionLimit(clientBundleName, format, remoteObject)) {
        return false;
    }
    if (remoteObject != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "ability already connect");
        return true;
    }
    sptr<ObjectEditorConnection> connection = sptr<ObjectEditorConnection>::MakeSptr();
    if (connection == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "connection is null");
        return false;
    }
    std::shared_ptr<ObjectEditorManagerSystemAbilityConnectionStatusCallback> callback =
        std::make_shared<ObjectEditorManagerSystemAbilityConnectionStatusCallback>();
    connection->RegisterConnectionStatusCallback(callback);
    connection->SetClientBundleName(clientBundleName);
    auto ret = connection->StartConnect(format->bundleName, format->abilityName,
        format->moduleName, remoteObject);
    if (ret != ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "Connect failed, ret = %{public}d", ret);
        return false;
    }
    std::lock_guard<std::mutex> lock(connectionMapMutex_);
    connectionMap_[remoteObject] = connection;
    return true;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
