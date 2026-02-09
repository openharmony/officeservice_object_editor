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

#include <cerrno>
#include <fstream>
#include <vector>

#include "appexecfwk_errors.h"
#include "common_func.h"
#include "cJSON.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "uri_permission_manager_client.h"
#include "utd_client.h"
#include "want.h"

#include "hilog_object_editor.h"
#include "object_editor_config.h"
#include "object_editor_err_code.h"
#include "object_editor_extension_proxy.h"
#include "object_editor_manager_database.h"
#include "object_editor_manager_system_ability.h"
#include "object_editor_manager_utils.h"
#include "object_editor_package.h"
#include "system_utils.h"
#include "user_mgr.h"

using namespace OHOS::AAFwk;

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
const std::string DIVISION_MAP_JSON_PATH = "/system/etc/office_service/object_editor_service/division_map.json";
constexpr int32_t ILLEGAL_REQUEST_CODE = -1;
}

REGISTER_SYSTEM_ABILITY_BY_ID(ObjectEditorManagerSystemAbility, OBJECT_EDITOR_SERVICE_SA_ID, true);
std::mutex ObjectEditorManagerSystemAbility::mutexTimer_;
std::condition_variable ObjectEditorManagerSystemAbility::cvTimer_;
bool ObjectEditorManagerSystemAbility::timerRunning_ = false;
bool ObjectEditorManagerSystemAbility::timerNotify_ = false;
std::mutex ObjectEditorManagerSystemAbility::connectMapMutex_;
std::map<sptr<IRemoteObject>, sptr<ObjectEditorConnection>> ObjectEditorManagerSystemAbility::connectMap_;

ObjectEditorManagerSystemAbility::ObjectEditorManagerSystemAbility(int32_t saId, bool isSystemService)
    : SystemAbility(saId, isSystemService)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "constructor");
    ReadDiversionJsonFile();
    ObjectEditorManagerDatabase::GetInstance().Init();
}

ObjectEditorManagerSystemAbility::~ObjectEditorManagerSystemAbility()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "destructor");
}

void ObjectEditorManagerSystemAbility::OnStart()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (state_ == SystemAbilityState::STATE_RUNNING) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "already running");
        return;
    }
    bool res = Publish(this);
    if (!res) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "Publish failed");
        return;
    } else {
        state_ = SystemAbilityState::STATE_RUNNING;
        ResetStopSATimer();
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "Publish success");
    }
}

void ObjectEditorManagerSystemAbility::OnStop()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (state_ != SystemAbilityState::STATE_RUNNING) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "not running");
        return;
    }
    state_ = SystemAbilityState::STATE_START;
}

int32_t ObjectEditorManagerSystemAbility::OnIdle(const SystemAbilityOnDemandReason &infoReason) override
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    return 0;
}

void ObjectEditorManagerSystemAbilityConnectionStatusCallback::OnConnectionStatusChanged(
    sptr<IRemoteObject> &remoteObject, const ObjectEditorConnectionStatus status)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "status:%{public}d", status);
    if (status != ObjectEditorConnectionStatus::STATUS_DISCONNECTED) {
        return;
    }
    std::lock_guard<std::mutex> lock(connectMapMutex_);
    connectMap_.erase(remoteObject);
}

bool ObjectEditorManagerSystemAbility::ConnectObjectEditorExtAbility(
    std::unique_ptr<ObjectEditorFormat> &format, sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "format is null");
        return false;
    }
    sptr<ObjectEditorConnection> connection = new (std::nothrow) ObjectEditorConnection();
    if (connection == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "connection is null");
        return false;
    }
    auto ret = connection->StartConnect(format->bundleName, format->abilityName,
        format->moduleName, remoteObject);
    if (ret != ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "Connect failed, ret = %{public}d", ret);
        return false;
    }
    std::lock_guard<std::mutex> lock(connectMapMutex_);
    connectionMap_[remoteObject] = connection;
    std::shared_ptr<ObjectEditorConnectionStatusCallback> callback =
        std::make_shared<ObjectEditorManagerSystemAbilityConnectionStatusCallback>();
    connection->RegisterConnectionStatusCallback(callback);
    return true;
}

bool ObjectEditorManagerSystemAbility::CheckClientFileValid(const ObjectEditorDocument &document)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "in");
    if (!document.GetTmpFileUri().has_value || document.GetSnapshotUri().empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "uri is empty");
        return false;
    }
    std::vector<std::string> uriVec;
    uriVec.push_back(document.GetTmpFileUri().value());
    uriVec.push_back(document.GetSnapshotUri());
    
    if (document.GetOperationType() == OperationType::CREATE_BY_FILE) {
        if (document.GetNativeFileUri().has_value()) {
            uriVec.push_back(document.GetNativeFileUri().value());
        }
        if (document.GetLinking() && document.GetOriFileUri().has_value()) {
            uriVec.push_back(document.GetOriFileUri().value());
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

bool ObjectEditorManagerSystemAbility::GrantClientFilePermissionToServerExtension(
    const ObjectEditorDocument &document, const std::string &targetBundleName)
{
    std::vector<Uri> uriList;
    if (document.GetTmpFileUri().has_value()) {
        uriList.push_back(document.GetTmpFileUri().value());
    }
    if (document.GetSnapshotUri().has_value()) {
        uriList.push_back(document.GetSnapshotUri().value());
    }
    if (document.GetOperationType() == OperationType::CREATE_BY_FILE) {
        if (document.GetNativeFileUri().has_value()) {
            uriList.push_back(document.GetNativeFileUri().value());
        }
        if (document.GetLinking() && document.GetOriFileUri().has_value()) {
            uriList.push_back(document.GetOriFileUri().value());
        }
    }
    auto readAndWritePermission = Want::FLAG_AUTH_READ_URI_PERMISSION |
        Want::FLAG_AUTH_WRITE_URI_PERMISSION;
    return UriPermissionManagerClient::GetInstance().GrantUriPermissionPrivilege(uriList,
        readAndWritePermission, targetBundleName, 0) == ERR_OK;
}

void ObjectEditorManagerSystemAbility::ReadDiversionJsonFile()
{
    std::ifstream file(DIVERSION_JSON_FILE_PATH);
    if (!file.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "open file failed");
        return;
    }
    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    file.close();
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
        std::unique_lock<std::mutex> lock(diversionMapMutex_);
        diversionMap_[diversion.sourceHmid] = diversion;
    }
    cJSON_Delete(json);
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::HandlePackage(
    std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension)
{
    sptr<ObjectEditorPackage> packageProxy = new(std::nothrow) ObjectEditorPackage();
    if (packageProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "create proxy failed");
        return ObjectEditorManagerErrCode::SA_MEMORY_ALLOCATION_FAILED;
    }
    packageProxy->RegisterClientCB(clientCallback);
    packageProxy->Initial(std::move(document), clientCallback);
    oeExtensionRemoteObject = packageProxy;
    isPackageExtension = true;
    connectMap_[packageProxy] = nullptr;
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::GetTargetHmid(
    const std::string &sourceHmid, std::string &targetHmid,
    std::string &minVersion)
{
    std::unique_lock<std::mutex> lock(diversionMapMutex_);
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
        for (const auto &format : formats) {
            if (format->bundleName == defaultAppBundleName) {
                objectEditorFormat = std::move(format);
                defaultAppFormatRegistered = true;
                OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "default app bundlename:%{public}s registered",
                    defaultAppBundleName.c_str());
                break;
            }
        }
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

    
ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::GetDefaultAppBundleNameByFileExt(
    const std::string &fileExt, std::string &bundleName)
{
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
    AppExecFwk::BundleInfo defaultAppInfo;
    errCode = defaultAppProxy->GetDefaultApplication(UserMgr::GetInstance()->GetUserId(), utdType,
        defaultAppInfo);
    if (errCode != ERR_OK || errCode != ERR_BUNDLE_MANAGER_DEFAULT_APP_NOT_EXIST) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get default app bundlename failed");
        return ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED;
    }
    if (defaultAppInfo.bundleName.empty()) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::SA, "default app bundlename is empty");
    } else {
        bundleName = defaultAppInfo.bundleName;
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "fileExt:%{public}s default app bundlename is %{public}s",
            fileExt.c_str(), bundleName.c_str());
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::CheckIsAllowStartExtension(
    const ObjectEditorDocument &document)
{
    if (document.GetOperation() == ObjectEditorOperation::UNKNOWN) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "unknown operation");
        return ObjectEditorManagerErrCode::SA_UNKNOWN_OPERATION;
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

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::HandleOperateHasHmid(
    const ObjectEditorDocument &document,
    std::unique_ptr<ObjectEditorFormat> &objectEditorFormat)
{
    objectEditorFormat = std::make_unique<ObjectEditorFormat>();
    auto errCode = ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatByHmid(document.GetHmid(),
        objectEditorFormat);
    if (errCode == ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY &&
        document.GetOperationType() == OperationType::EDIT) {
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

void ObjectEditorManagerSystemAbility::TimerThreadStopSA()
{
    while (true) {
        std::unique_lock<std::mutex> lock(mutexTimer_);
        auto waitResult = cvTimer_.wait_for(lock, std::chrono::seconds(SA_EXIT_TIME_S),
            [this]() { return timerNotify_ == true; });
        if (!waitResult && connectMap_.empty()) {
            auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
            if (sam == nullptr) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get system ability manager failed");
                break;
            }
            int32_t result = sam->UnloadSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID);
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "UnloadSystemAbility result:%{public}d", result);
            break;
        }
        timerNotify_ = true;
    }
    timerNotify_ = false;
}

void ObjectEditorManagerSystemAbility::ResetStopSATimer()
{
    std::unique_lock<std::mutex> lock(mutexCallback_);
    if (!timerRunning_) {
        timerRunning_ = true;
        std::thread timerThread([this]() { TimerThreadStopSA(); });
        timerThread.detach();
        return;
    }
    timerNotify_ = false;
    cvTimer_.notify_one();
}

int32_t ObjectEditorManagerSystemAbility::CallbackEnter([[maybe_unused]] uint32_t code)
{
    ResetStopSATimer();
    if (!ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    if (!IsLegalCalling()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "illegal calling");
        return ObjectEditorManagerErrCode::SA_PERMISSION_DENIED;
    }
    return ERR_NONE;
}

int32_t ObjectEditorManagerSystemAbility::CallbackExit([[maybe_unused]] uint32_t code,
    [[maybe_unused]] int32_t result)
{
    return result;
}

ObjectEditorManagerErrCode ObjectEditorManagerSystemAbility::StartObjectEditorExtensionByFile(
    const ObjectEditorDocument &document,
    const sptr<IObjectEditorClientCallback> &clientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension)
{
    if (document == nullptr || !document->GetOriFileUri().has_value()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "ori file uri is empty");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    std::vector<std::unique_ptr<ObjectEditorFormat>> objectEditorFormats;
    std::string fileExt = SystemUtils::GetFileSuffix(document->GetOriFileUri().value());
    auto errCode = ObjectEditorManagerDatabase::GetInstance().GetObjectEditorFormatsByFileExt(
        fileExt, objectEditorFormats);
    if (errCode == ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "use package extension");
        return HandlePackage(std::move(document), objectEditorClientCallback,
            oeExtensionRemoteObject, isPackageExtension);
    }
    if (errCode == ObjectEditorManagerErrCode::SA_OK && !objectEditorFormats.empty()) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "use default app");
        errCode = HandleDefaultAppFormatPolicy(fileExt, objectEditorFormats, objectEditorFormat);
    }
}

ErrCode ObjectEditorManagerSystemAbility::StartObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension)
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
        document->GetOperationType());
    if (document->GetHmid() == PACKAGE_HMID &&
        (document->GetOperationType() == OperationType::CREATE_BY_HMID ||
         document->GetOperationType() == OperationType::EDIT)) {
        return HandlePackage(std::move(document), objectEditorClientCallback,
            oeExtensionRemoteObject, isPackageExtension);
    }
    std::unique_ptr<ObjectEditorFormat> objectEditorFormat;
    if (document->GetOperationType() == OperationType::CREATE_BY_HMID) {
        errCode = StartObjectEditorExtensionByFile(document, objectEditorClientCallback,
            oeExtensionRemoteObject, isPackageExtension);
    } else {
        errCode = HandleOperateHasHmid(*document, objectEditorFormat);
    }
    if (errCode != ObjectEditorManagerErrCode::SA_OK || objectEditorFormat == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "get object editor format failed");
        return errCode;
    }
    if (document->GetOperationType() == OperationType::CREATE_BY_HMID) {
        document->SetHmid(objectEditorFormat->hmid);
    }
    if (!GrantClientFilePermissionToServerExtension(*document, objectEditorFormat->bundleName)) {
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
    ErrCode proxyResult = objectEditorExtensionProxy->Initial(
        std::move(document), objectEditorClientCallback);
    if (proxyResult != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "start object editor extension failed");
        return ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_SEND_FAILED;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::StopObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IRemoteObject> &oeExtensionRemoteObject, const bool &isPackageExtension)
{
    if (document == nullptr || oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "invalid parameter");
        return ObjectEditorManagerErrCode::SA_INVALID_PARAMETER;
    }
    auto objectEditorExtensionProxy = iface_cast<ObjectEditorExtensionProxy>(oeExtensionRemoteObject);
    if (objectEditorExtensionProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "object editor extension proxy is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_EXTENSION_PROXY_IS_NULL;
    }
    bool isAllObjectsRemoved = false;
    ErrCode proxyResult = objectEditorExtensionProxy->Close(std::move(document), isAllObjectsRemoved);
    if (proxyResult != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "stop object editor extension failed");
        return ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_SEND_FAILED;
    }
    if (!isAllObjectsRemoved) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "not all objects removed");
        return ObjectEditorManagerErrCode::SA_OK;
    }
    sptr<ObjectEditorConnection> objectEditorConnection = nullptr;
    {
        std::lock_guard<std::mutex> lock(connectMapMutex_);
        auto it = connectMap_.find(oeExtensionRemoteObject);
        if (it == connectMap_.end()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "object editor extension proxy not found");
            return ObjectEditorManagerErrCode::SA_OK;
        }
        objectEditorConnection = it->second;
    }
    ErrCode code = ObjectEditorManagerErrCode::SA_OK;
    if (objectEditorConnection != nullptr && !isPackageExtension) {
        code = objectEditorConnection->StopConnect();
        code = code != ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_SUCCEED ?
            code : ObjectEditorManagerErrCode::SA_OK;
    }
    return code;
}

ErrCode ObjectEditorManagerSystemAbility::GetHmidByFileExtension(const std::string &hmid,
    std::string &fileExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "not support");
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::GetIconByHmid(const std::string &hmid, std::string &resourceId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "not support");
    return ObjectEditorManagerErrCode::SA_OK;
}

ErrCode ObjectEditorManagerSystemAbility::GetFormatName(const std::string &hmid, const std::string &locale,
    std::string &formatName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::SA, "not support");
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
    auto abilityMgr = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "ability manager is null");
        return ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_MANAGER_IS_NULL;
    }
    OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "bundlename: %{public}s", want->GetBundle().c_str());
    ErrCode code = abilityMgr->StartAbility(*want, ILLEGAL_REQUEST_CODE,
        UserMgr::GetInstance().GetUserId());
    if (code != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::SA, "failed code: %{public}d", code);
        return ObjectEditorManagerErrCode::SA_START_UIABILITY_FAILED;
    }
    return ObjectEditorManagerErrCode::SA_OK;
}

// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
