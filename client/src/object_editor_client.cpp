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

#include "object_editor_client.h"

#include <filesystem>
#include <fstream>
#include <random>
#include <sys/time.h>
#include "application_context.h"
#include "iservice_registry.h"
#include "system_utils.h"
#include "iobject_editor_extension.h"
#include "iobject_editor_package.h"

namespace OHOS {
namespace ObjectEditor {
constexpr int32_t UUID_ARRAY_LEN = 16;
constexpr int32_t UUID_START_POS_1 = 8;
constexpr int32_t UUID_START_POS_2 = 12;
constexpr int32_t UUID_START_POS_3 = 16;
constexpr int32_t UUID_START_POS_4 = 20;
constexpr int32_t UUID_HEAD_LEN = 8;
constexpr int32_t UUID_OTHER_LEN = 4;

IMPLEMENT_SINGLE_INSTANCE(ObjectEditorClient);

void ObjectEditorClient::SubscribeSystemAbility()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (saStatusListener_ != nullptr) {
        return;
    }
    saStatusListener_ = sptr<ObjectEditorAbilityListener>(new ObjectEditorAbilityListener());
    if (saStatusListener_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "saStatusListener_ is null");
        return;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sam is null");
        return;
    }
    int32_t ret = sam->SubscribeSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID, this);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "SubscribeSystemAbility failed, ret: %{public}d", ret);
    }
}

void ObjectEditorClient::UnsubscribeSystemAbility()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (saStatusListener_ == nullptr) {
        return;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sam is null");
        return;
    }
    int32_t ret = sam->UnsubscribeSystemAbility(static_cast<int32_t>(OBJECT_EDITOR_SERVICE_SA_ID), saStatusListener_);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "UnsubscribeSystemAbility failed, ret: %{public}d", ret);
    }
}

void ObjectEditorAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex> (this->mutex_);
    if (systemAbilityId == OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "OBJECT_EDITOR_SERVICE_SA_ID is added");
    }
}

void ObjectEditorAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex> (this->mutex_);
    if (systemAbilityId == OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "OBJECT_EDITOR_SERVICE_SA_ID is removed");
    }
}

sptr<IObjectEditorManager> ObjectEditorClient::GetObjectEditorProxy(const sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex> (this->mutex_);
    if (oeSAProxy_ != nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeSAProxy_ is null");
        return oeSAProxy_;
    }
    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new ObjectEditorDeathRecipient());
    if (deathRecipient_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, " deathRecipient_ is null");
        return nullptr;
    }
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remoteObject is null");
        return nullptr;
    }
    if (remoteObject->IsProxyObject() && !remoteObject->AddDeathRecipient(deathRecipient_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "AddDeathRecipient failed");
        return nullptr;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "GetObjectEditorProxy success");
    oeSAProxy_ = iface_cast<IObjectEditorManager>(remoteObject);
    if (oeSAProxy_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeSAProxy_ is null");
        return nullptr;
    }
    return oeSAProxy_;
}

sptr<IObjectEditorManager> ObjectEditorClient::GetIObjectEditorManager()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    SubscribeSystemAbility();
    if (oeSAProxy_ != nullptr) {
        return oeSAProxy_;
    }
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgrProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "samgrProxy is null");
        return oeSAProxy_;
    }
    auto remoteObject = samgrProxy->CheckSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID);
    if (remoteObject != nullptr) {
        return GetObjectEditorProxy(remoteObject);
    }
    sptr<ObjectEditorLoaderCallback> loaderCallback = new (std::nothrow)ObjectEditorLoaderCallback();
    if (loaderCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "loaderCallback is null");
        return nullptr;
    }
    InitLoadState();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "load SA: %{public}d", OBJECT_EDITOR_SERVICE_SA_ID);
    int32_t ret = samgrProxy->LoadSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID, loaderCallback);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "LoadSystemAbility failed, ret: %{public}d", ret);
        return nullptr;
    }
    if (!WaitLoadStateChange()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WaitLoadStateChange failed");
        return nullptr;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT,
        "Notify samgr load sa %{public}d success", OBJECT_EDITOR_SERVICE_SA_ID);
    return oeSAProxy_;
}

void ObjectEditorClient::InitLoadState()
{
    std::unique_lock<std::mutex> lock(loadMutex_);
    loadState_ = false;
}

bool ObjectEditorClient::WaitLoadStateChange()
{
    std::unique_lock<std::mutex> lock(loadMutex_);
    auto wait = loadCond_.wait_for(lock, std::chrono::seconds(LOAD_TIMEOUT_MS),
        [this] { return loadState_ == true; });
    if (!wait) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "WaitLoadStateChange timeout");
        return false;
    }
    return true;
}

void ObjectEditorLoadCallback::OnLoadSystemAbilitySuccess(int32_t systemAbilityId,
    const sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (systemAbilityId != OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "systemAbilityId is not OBJECT_EDITOR_SERVICE_SA_ID");
        return;
    }
    if (remoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remoteObject is null");
        return;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT,
        "OnLoadSystemAbilitySuccess, systemAbilityId: %{public}d", systemAbilityId);
    ObjectEditorClient::GetInstance().LoadSystemAbilitySuccess(remoteObject);
}

void ObjectEditorLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (systemAbilityId != OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "systemAbilityId is not OBJECT_EDITOR_SERVICE_SA_ID");
        return;
    }
    OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
        "OnLoadSystemAbilityFailed, systemAbilityId: %{public}d", systemAbilityId);
    ObjectEditorClient::GetInstance().LoadSystemAbilityFail();
}

void ObjectEditorClient::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &remoteObject)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::unique_lock<std::mutex> lock(loadMutex_);
    loadState_ = true;
    GetObjectEditorProxy(remoteObject);
    loadCond_.notify_one();
}

void ObjectEditorClient::LoadSystemAbilityFail()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::unique_lock<std::mutex> lock(loadMutex_);
    loadState_ = false;
    loadCond_.notify_one();
}

void ObjectEditorClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (remote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "remote is null");
        return;
    }
    std::lock_guard<std::mutex> lock(proxyMutex_);
    if (oeSAProxy_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeSAProxy_ is null");
        return;
    }
    sptr<IRemoteObject> serviceRemote = oeSAProxy_->AsObject();
    if (serviceRemote != nullptr && serviceRemote == remote.promote()) {
        serviceRemote->RemoveDeathRecipient(deathRecipient_);
        oeSAProxy_ = nullptr;
        InitLoadState();
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "onRemoteDied");
    }
}

ErrCode ObjectEditorClient::StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &callback, sptr<IObjectEditorService> &oeExtensionRemoteObject,
    bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (document == nullptr || callback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document or callback is null");
        return ObjectEditorClientErrCode::CLIENT_INVALID_PARAMETER;
    }
    if (document->GetOperateType() == OperateType::UNKNOWN) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document operate type is unknown");
        return ObjectEditorClientErrCode::CLIENT_UNKNOWN_OPERATE;
    }
    ErrCode ret = PrepareFiles(document);
    if (ret != ObjectEditorClientErrCode::CLIENT_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "PrepareFiles failed, ret: %{public}d", ret);
        return ret;
    }
    sptr<IObjectEditorManager> oeManagerProxy = GetIObjectEditorManager();
    if (oeManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManagerProxy is null");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> oeExtensionRemote = nullptr;
    ret = oeManagerProxy->StartObjectEditorExtension(document, callback, oeExtensionRemote, isPackageExtension);
    if (ret != ERR_OK || oeExtensionRemote == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "StartObjectEditorExtension failed, ret: %{public}d", ret);
        return ret;
    }
    if (isPackageExtension) {
        oeExtensionRemoteObject = iface_cast<IObjectEditorPackage>(oeExtensionRemote);
    } else {
        oeExtensionRemoteObject = iface_cast<IObjectEditorExtension>(oeExtensionRemote);
    }
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
        return ERR_INVALID_OPERATION;
    }
    oeExtensionRemoteObject->SetRemoteObject(oeExtensionRemote);
    return ERR_OK;
}

std::string ObjectEditorClient::GenRandomUuid()
{
    struct timeval tv;
    struct timezone tz;
    struct tm randomTime;
    uint32_t randomNum = 0;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, RAND_MAX);
    std::array<uint8_t, UUID_ARRAY_LEN> uuid_ = {0x00};
    randomNum = static_cast<unsigned int>(dis(gen));
    gettimeofday(&tv, &tz);
    localtime_r(&tv.tv_sec, &randomTime);
    const int INDEX_15 = 15;
    const int INDEX_14 = 14;
    const int INDEX_13 = 13;
    const int INDEX_12 = 12;
    const int INDEX_10 = 10;
    const int INDEX_9 = 9;
    const int INDEX_8 = 8;
    const int INDEX_7 = 7;
    const int INDEX_6 = 6;
    const int INDEX_5 = 5;
    const int INDEX_4 = 4;
    const int INDEX_3 = 3;
    const int INDEX_2 = 2;
    const int INDEX_1 = 1;
    const int INDEX_0 = 0;
    uuid_[INDEX_15] = static_cast<uint8_t>(static_cast<uint64_t>(tv.tv_usec) & 0x00000000000000FF);
    uuid_[INDEX_14] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x000000000000FF00) >> 8);
    uuid_[INDEX_13] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x0000000000FF0000) >> 16);
    uuid_[INDEX_12] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x00000000FF000000) >> 24);
    uuid_[INDEX_10] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x000000FF00000000) >> 32);
    uuid_[INDEX_9] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x0000FF0000000000) >> 40);
    uuid_[INDEX_8] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x00FF000000000000) >> 48);
    uuid_[INDEX_7] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0xFF00000000000000) >> 56);
    uuid_[INDEX_6] = static_cast<uint8_t>((unsigned int)randomTime.tm_sec + static_cast<unsigned int>(randomNum) & 0xFF);
    uuid_[INDEX_5] = static_cast<uint8_t>(((unsigned int)randomTime.tm_min + (randomNum >> 8)) & 0xFF);
    uuid_[INDEX_4] = static_cast<uint8_t>(((unsigned int)randomTime.tm_hour + (randomNum >> 16)) & 0xFF);
    uuid_[INDEX_3] = static_cast<uint8_t>(((unsigned int)randomTime.tm_mday + (randomNum >> 24)) & 0xFF);
    uuid_[INDEX_2] = static_cast<uint8_t>((unsigned int)randomTime.tm_mon & 0xFF);
    uuid_[INDEX_1] = static_cast<uint8_t>((unsigned int)randomTime.tm_year & 0xFF);
    uuid_[INDEX_0] = static_cast<uint8_t>(((unsigned int)randomTime.tm_year & 0xFF00) >> 8);
    std::string tmp = "";
    std::string ret = "";
    static const char *hex = "0123456789ABCDEF";
    static const int NIBBLE_SHIFT = 4;
    static const int NIBBLE_MASK = 0xF;
    for (auto it = uuid_.begin(); it != uuid_.end(); it++) {
        tmp.push_back(hex[(*it >> NIBBLE_SHIFT) & NIBBLE_MASK]); // 右移4位
        tmp.push_back(hex[*it & NIBBLE_MASK]); // 取低4位
    }
    ret = tmp.substr(0, UUID_HEAD_LEN) + "-" +
    tmp.substr(UUID_START_POS_1, UUID_OTHER_LEN) + "-" +
    tmp.substr(UUID_START_POS_2, UUID_OTHER_LEN) + "-" +
    tmp.substr(UUID_START_POS_3, UUID_OTHER_LEN) + "-" +
    tmp.substr(UUID_START_POS_4);
    return ret;
}

ErrCode FlushDocument(const std::unique_ptr<ObjectEditorDocument> &document)
{
    if (document == nullptr || !document->Flush()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document flush failed");
        return ObjectEditorClientErrCode::CLENT_PREPARE_FILES_ERROR;
    }
    return ObjectEditorClientErrCode::CLIENT_OK;
}

ErrCode ObjectEditorClient::PrepareFiles(const std::unique_ptr<ObjectEditorDocument> &document)
{
    std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::Context::GetApplicationContext();
    if (context == nullptr || document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "context or document is null");
        return ObjectEditorClientErrCode::CLENT_INVALID_PARAMETER;
    }
    std::string fileDirPath = context->GetFilesDir();
    if (fileDirPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "fileDirPath is empty");
        return ObjectEditorClientErrCode::CLENT_GET_PATH_ERROR;
    }
    std::string sandboxPath = fileDirPath + "/" + GenRandomUuid();
    fs::path targetDirPath(sandboxPath);
    bool ret = fs::create_directories(targetDirPath);
    if (!ret) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create sandboxPath failed");
        return ObjectEditorClientErrCode::CLENT_UNKNOWN_OPERATE;
    }
    if (document->GetOperateType() == OperateType::CREATE_BY_FILE) {
        std::string sourceFilePath = document->GetOriFilePath();
        if (sourceFilePath.empty()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sourceFilePath is empty");
            return ObjectEditorClientErrCode::CLENT_GET_PATH_ERROR;
        }
        fs::path sourceFilePathPath(sourceFilePath);
        fs::path targetFilePathPath(targetDirPath.string() + "/" + sourceFilePathPath.filename().string());
        ret = fs::copy_file(sourceFilePathPath, targetFilePathPath);
        if (!ret) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "copy sourceFilePath to sandboxPath failed");
            return ObjectEditorClientErrCode::CLENT_UNKNOWN_OPERATE;
        }
        document->SetNativeFileUri(SystemUtils::GetUriFromPath(targetFilePathPath.string()));
    }
    std::unique_ptr<char, decltype(&free)> canonicalFilePath(realpath(sandboxPath.c_str(), nullptr), &free);
    if (canonicalFilePath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "realpath sandboxPath failed");
        return ObjectEditorClientErrCode::CLENT_PREPARE_FILES_ERROR;
    }
    std::string canonicalFilePathStr(canonicalFilePath.get());
    sandboxPath = canonicalFilePathStr;
    std::string snapshotFilePath = sandboxPath + "/snapshot.png";
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "snapshotFilePath is %{private}s", snapshotFilePath.c_str());
    std::ofstream snapshotFile(snapshotFilePath, std::ios::binary);
    if (!snapshotFile.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "open snapshotFilePath failed");
        return ObjectEditorClientErrCode::CLENT_PREPARE_FILES_ERROR;
    }
    snapshotFile.close();
    document->SetSnapshotUri(SystemUtils::GetUriFromPath(snapshotFilePath));
    document->SetTmpFileUri(SystemUtils::GetUriFromPath(sandboxPath + "/ole.bin"));
    return FlushDocument(document);
}

ErrCode ObjectEditorClient::StopObjectEditorExtension(const sptr<IObjectEditorService> &oeExtensionRemoteObject,
    const bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "in");
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
        return ERR_INVALID_VALUE;
    }
    sptr<IObjectEditorManager> oeManager = GetIObjectEditorManager();
    if (oeManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManager is null");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = oeExtensionRemoteObject->GetRemoteObject();
    ErrCode ret = oeManager->StopObjectEditorExtension(remoteObject, isPackageExtension);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "StopObjectEditorExtension failed, ret is %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetIcon(const std::string &hmid, std::string &resFilePath)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> oeManager = GetIObjectEditorManager();
    if (oeManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManager is null");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = oeManager->GetIconByHmid(hmid, resFilePath);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "GetIconByHmid failed, ret is %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetFormatName(const std::string &hmid,
    const std::string &locale, std::string &formatName)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> oeManager = GetIObjectEditorManager();
    if (oeManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManager is null");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = oeManager->GetFormatNameByHmid(hmid, formatName);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "GetFormatNameByHmid failed, ret is %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetObjectEditorFormatByHmidAndLocale(const std::string &hmid,
    const std::string &locale, std::unique_ptr<ObjectEditorFormat> &format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> oeManager = GetIObjectEditorManager();
    if (oeManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManager is null");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = oeManager->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
            "GetObjectEditorFormatByHmidAndLocale failed, ret is %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetObjectEditorFormatsByLocale(const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> oeManager = GetIObjectEditorManager();
    if (oeManager == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeManager is null");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = oeManager->GetObjectEditorFormatsByLocale(locale, formats);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT,
            "GetObjectEditorFormatsByLocale failed, ret is %{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

void ObjectEditorClient::SARegCleanUp()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex> lock(proxyMutex_);
    UnsubscribeSystemAbility();
    if (oeSAProxy_ == nullptr) {
        return;
    }
    sptr<IRemoteObject> remoteObject = oeSAProxy_->AsObject();
    if (remoteObject != nullptr && deathRecipient_ != nullptr) {
        remoteObject->RemoveDeathRecipient(deathRecipient_);
    }
    oeSAProxy_ = nullptr;
}

ObjectEditorClient::~ObjectEditorClient()
{
    SARegCleanUp();
}

} // namespace ObjectEditor
} // namespace OHOS
