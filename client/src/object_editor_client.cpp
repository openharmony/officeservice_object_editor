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
#include "object_editor_package.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
constexpr int32_t UUID_ARRAY_LEN = 16;
constexpr int32_t UUID_START_POS_1 = 8;
constexpr int32_t UUID_START_POS_2 = 12;
constexpr int32_t UUID_START_POS_3 = 16;
constexpr int32_t UUID_START_POS_4 = 20;
constexpr int32_t UUID_HEAD_LEN = 8;
constexpr int32_t UUID_OTHER_LEN = 4;
constexpr int32_t INDEX_15 = 15;
constexpr int32_t INDEX_14 = 14;
constexpr int32_t INDEX_13 = 13;
constexpr int32_t INDEX_12 = 12;
constexpr int32_t INDEX_10 = 10;
constexpr int32_t INDEX_9 = 9;
constexpr int32_t INDEX_8 = 8;
constexpr int32_t INDEX_7 = 7;
constexpr int32_t INDEX_6 = 6;
constexpr int32_t INDEX_5 = 5;
constexpr int32_t INDEX_4 = 4;
constexpr int32_t INDEX_3 = 3;
constexpr int32_t INDEX_2 = 2;
constexpr int32_t INDEX_1 = 1;
constexpr int32_t INDEX_0 = 0;
constexpr int32_t RIGHT_SHIFT_8 = 8;
constexpr int32_t RIGHT_SHIFT_16 = 16;
constexpr int32_t RIGHT_SHIFT_24 = 24;
constexpr int32_t RIGHT_SHIFT_32 = 32;
constexpr int32_t RIGHT_SHIFT_40 = 40;
constexpr int32_t RIGHT_SHIFT_48 = 48;
constexpr int32_t RIGHT_SHIFT_56 = 56;
}
namespace fs = std::filesystem;
// LCOV_EXCL_START
IMPLEMENT_SINGLE_INSTANCE(ObjectEditorClient);

void ObjectEditorClient::SubscribeSystemAbility()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (saStatusListener_ != nullptr) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "No duplicate subscribe");
        return;
    }
    saStatusListener_ = sptr<ObjectEditorAbilityListener>(new ObjectEditorAbilityListener());
    if (saStatusListener_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create failed");
        return;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sam is null");
        return;
    }
    int32_t result = sam->SubscribeSystemAbility(static_cast<int32_t>(OBJECT_EDITOR_SERVICE_SA_ID), saStatusListener_);
    if (result != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "subscribe failed:%{public}d", result);
    }
}

void ObjectEditorClient::UnsubscribeSystemAbility()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (saStatusListener_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "listener is null");
        return;
    }
    auto sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sam is null");
        return;
    }
    int32_t result = sam->UnSubscribeSystemAbility(
        static_cast<int32_t>(OBJECT_EDITOR_SERVICE_SA_ID), saStatusListener_);
    if (result == ERR_OK) {
        saStatusListener_ = nullptr;
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "unsubscribe failed:%{public}d", result);
    }
}

void ObjectEditorAbilityListener::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex>(this->mutex_);
    if (systemAbilityId == OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "sa is added");
    }
}

void ObjectEditorAbilityListener::OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::lock_guard<std::mutex>(this->mutex_);
    if (systemAbilityId == OBJECT_EDITOR_SERVICE_SA_ID) {
        ObjectEditorClient::GetInstance().SARegCleanUp();
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "sa is removed");
    }
}

sptr<IObjectEditorManager> ObjectEditorClient::GetObjectEditorProxy(const sptr<IRemoteObject> &object)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (oeSAProxy_ != nullptr) {
        return oeSAProxy_;
    }
    deathRecipient_ = sptr<IRemoteObject::DeathRecipient>(new ObjectEditorSADeathRecipient());
    if (deathRecipient_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "deathRecipient is null");
        return nullptr;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "object is null");
        return nullptr;
    }
    if (object->IsProxyObject() && !object->AddDeathRecipient(deathRecipient_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "AddDeathRecipient failed");
        return nullptr;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "get sa proxy succeed");
    oeSAProxy_ = iface_cast<IObjectEditorManager>(object);
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
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get samgrProxy fail");
        return oeSAProxy_;
    }
    sptr<IRemoteObject> object = samgrProxy->CheckSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID);
    if (object != nullptr) {
        return GetObjectEditorProxy(object);
    }
    sptr<ObjectEditorLoadCallback> oeLoadCallback = sptr<ObjectEditorLoadCallback>::MakeSptr();
    if (oeLoadCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "loadCallback is null");
        return nullptr;
    }
    InitLoadState();
    int32_t result = samgrProxy->LoadSystemAbility(OBJECT_EDITOR_SERVICE_SA_ID, oeLoadCallback);
    if (result != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "failed to sa error:%{public}d", result);
        return nullptr;
    }
    if (!WaitLoadStateChange()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "wait sa failed");
        return nullptr;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "load sa success");
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
    auto wait = loadCond_.wait_for(lock, std::chrono::milliseconds(LOAD_TIMEOUT_MS), [this] {
        return loadState_ == true;
    });
    if (!wait) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "timeout");
        return false;
    }
    return true;
}

void ObjectEditorLoadCallback::OnLoadSystemAbilitySuccess(
    int32_t systemAbilityId, const sptr<IRemoteObject> &object)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (systemAbilityId != OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sa id invalid");
        return;
    }
    if (object == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "object is null");
        ObjectEditorClient::GetInstance().LoadSystemAbilityFail();
        return;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "load sa success");
    ObjectEditorClient::GetInstance().LoadSystemAbilitySuccess(object);
}

void ObjectEditorLoadCallback::OnLoadSystemAbilityFail(int32_t systemAbilityId)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (systemAbilityId != OBJECT_EDITOR_SERVICE_SA_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sa id invalid");
        return;
    }
    ObjectEditorClient::GetInstance().LoadSystemAbilityFail();
}

void ObjectEditorClient::LoadSystemAbilitySuccess(const sptr<IRemoteObject> &object)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    std::unique_lock<std::mutex> lock(loadMutex_);
    loadState_ = true;
    GetObjectEditorProxy(object);
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
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "onRemoteDied");
    }
}

ErrCode ObjectEditorClient::StartObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IObjectEditorService> &oeExtensionRemoteObject,
    bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (document == nullptr || objectEditorClientCallback == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document or callback is null");
        return ObjectEditorClientErrCode::CLIENT_INVALID_PARAMETER;
    }
    if (document->GetOperateType() == OperateType::UNKNOWN) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "unknown operate");
        return ObjectEditorClientErrCode::CLIENT_UNKNOWN_OPERATE;
    }
    document->SetDocumentId(GenRandomUuid());
    ErrCode ret = PrepareFiles(document);
    if (ret != ObjectEditorClientErrCode::CLIENT_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "prepare files failed");
        return ret;
    }
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = nullptr;
    ret = objectEditorManagerProxy->StartObjectEditorExtension(document,
        objectEditorClientCallback, remoteObject, isPackageExtension);
    if (ret != ERR_OK || (!isPackageExtension && remoteObject == nullptr)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "failed:%{public}d", ret);
        return ret;
    }
    if (isPackageExtension) {
        HandlePackage(document, objectEditorClientCallback, oeExtensionRemoteObject);
    } else {
        oeExtensionRemoteObject = iface_cast<IObjectEditorExtension>(remoteObject);
        if (oeExtensionRemoteObject == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "oeExtensionRemoteObject is null");
            return ERR_INVALID_OPERATION;
        }
        oeExtensionRemoteObject->SetRemoteObject(remoteObject);
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::HandlePackage(
    const std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
    sptr<IObjectEditorService> &oeExtensionRemoteObject)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "in");
    std::unique_ptr<ObjectEditorDocument> newDocument = nullptr;
    if (document->GetOperateType() == OperateType::CREATE_BY_FILE) {
        newDocument = ObjectEditorDocument::CreateByFile(document->GetOriFilePath());
        if (newDocument == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create document by file failed");
            return ERR_INVALID_VALUE;
        }
        newDocument->SetHmid(PACKAGE_HMID);
        if (document->GetNativeFileUri().has_value()) {
            newDocument->SetNativeFileUri(document->GetNativeFileUri().value());
        }
    } else if (document->GetOperateType() == OperateType::EDIT) {
        newDocument = ObjectEditorDocument::CreateByHmid(PACKAGE_HMID);
        if (newDocument == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create document by hmid failed");
            return ERR_INVALID_VALUE;
        }
        newDocument->SetOperateType(OperateType::EDIT);
    }
    if (document->GetTmpFileUri().has_value()) {
        newDocument->SetTmpFileUri(document->GetTmpFileUri().value());
    }
    newDocument->SetSnapshotUri(document->GetSnapshotUri());
    sptr<ObjectEditorPackage> packageProxy = sptr<ObjectEditorPackage>::MakeSptr();
    if (packageProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create package proxy failed");
        return ERR_INVALID_VALUE;
    }
    packageProxy->Initial(std::move(newDocument), objectEditorClientCallback);
    oeExtensionRemoteObject = packageProxy;
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
    uuid_[INDEX_15] = static_cast<uint8_t>(static_cast<uint64_t>(tv.tv_usec) & 0x00000000000000FF);
    uuid_[INDEX_14] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x000000000000FF00) >> RIGHT_SHIFT_8);
    uuid_[INDEX_13] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x0000000000FF0000) >> RIGHT_SHIFT_16);
    uuid_[INDEX_12] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x00000000FF000000) >> RIGHT_SHIFT_24);
    uuid_[INDEX_10] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x000000FF00000000) >> RIGHT_SHIFT_32);
    uuid_[INDEX_9] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x0000FF0000000000) >> RIGHT_SHIFT_40);
    uuid_[INDEX_8] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0x00FF000000000000) >> RIGHT_SHIFT_48);
    uuid_[INDEX_7] = static_cast<uint8_t>((static_cast<uint64_t>(tv.tv_usec) & 0xFF00000000000000) >> RIGHT_SHIFT_56);
    uuid_[INDEX_6] = static_cast<uint8_t>((unsigned int)randomTime.tm_sec + static_cast<unsigned int>(randomNum) & 0xFF);
    uuid_[INDEX_5] = static_cast<uint8_t>(((unsigned int)randomTime.tm_min + (randomNum >> RIGHT_SHIFT_8)) & 0xFF);
    uuid_[INDEX_4] = static_cast<uint8_t>(((unsigned int)randomTime.tm_hour + (randomNum >> RIGHT_SHIFT_16)) & 0xFF);
    uuid_[INDEX_3] = static_cast<uint8_t>(((unsigned int)randomTime.tm_mday + (randomNum >> RIGHT_SHIFT_24)) & 0xFF);
    uuid_[INDEX_2] = static_cast<uint8_t>((unsigned int)randomTime.tm_mon & 0xFF);
    uuid_[INDEX_1] = static_cast<uint8_t>((unsigned int)randomTime.tm_year & 0xFF);
    uuid_[INDEX_0] = static_cast<uint8_t>(((unsigned int)randomTime.tm_year & 0xFF00) >> RIGHT_SHIFT_8);
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
        return ObjectEditorClientErrCode::CLIENT_PREPARE_FILES_ERROR;
    }
    return ObjectEditorClientErrCode::CLIENT_OK;
}

ErrCode ObjectEditorClient::PrepareFiles(const std::unique_ptr<ObjectEditorDocument> &document)
{
    std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::Context::GetApplicationContext();
    if (context == nullptr || document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "context or document is null");
        return ObjectEditorClientErrCode::CLIENT_INVALID_PARAMETER;
    }
    std::string fileDirPath = context->GetTempDir();
    if (fileDirPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "fileDirPath is empty");
        return ObjectEditorClientErrCode::CLIENT_GET_PATH_ERROR;
    }
    std::string sandboxPath = fileDirPath + '/' + GenRandomUuid();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "sandboxPath: %{private}s", sandboxPath.c_str());
    fs::path targetDirPath(sandboxPath);
    bool result = fs::create_directories(targetDirPath);
    if (!result) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "create sandboxPath failed");
        return ObjectEditorClientErrCode::CLIENT_UNKNOWN_OPERATE;
    }
    if (document->GetOperateType() == OperateType::CREATE_BY_FILE) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "handle copy file");
        std::string sourceFilePath = document->GetOriFilePath();
        if (sourceFilePath.empty()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "sourceFilePath is empty");
            return ObjectEditorClientErrCode::CLIENT_GET_PATH_ERROR;
        }
        fs::path sourceFilePathPath(sourceFilePath);
        fs::path targetFilePathPath = targetDirPath / sourceFilePathPath.filename().string();
        result = fs::copy_file(sourceFilePathPath, targetFilePathPath, fs::copy_options::overwrite_existing);
        if (!result) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "copy sourceFilePath to sandboxPath failed");
            return ObjectEditorClientErrCode::CLIENT_UNKNOWN_OPERATE;
        }
        document->SetNativeFileUri(SystemUtils::GetUriFromPath(targetFilePathPath.string()));
    }
    std::unique_ptr<char, decltype(&free)> canonicalFilePath(realpath(sandboxPath.c_str(), nullptr), &free);
    if (canonicalFilePath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "canonicalFilePath is null");
        return ObjectEditorClientErrCode::CLIENT_PREPARE_FILES_ERROR;
    }
    sandboxPath = canonicalFilePath.get();
    std::string snapshotFilePath = sandboxPath + "/snapshot.png";
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::CLIENT, "snapshotFilePath is %{private}s", snapshotFilePath.c_str());
    std::ofstream snapshotFile(snapshotFilePath, std::ios::binary);
    if (!snapshotFile.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "open snapshotFilePath failed");
        return ObjectEditorClientErrCode::CLIENT_PREPARE_FILES_ERROR;
    }
    snapshotFile.close();
    document->SetSnapshotUri(SystemUtils::GetUriFromPath(snapshotFilePath));
    document->SetTmpFileUri(SystemUtils::GetUriFromPath(sandboxPath + "/ole.bin"));
    return FlushDocument(document);
}

ErrCode ObjectEditorClient::StopObjectEditorExtension(
    std::unique_ptr<ObjectEditorDocument> &document,
    const sptr<IObjectEditorService> &oeExtensionRemoteObject,
    const bool &isPackageExtension)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "document is null");
        return ERR_INVALID_VALUE;
    }
    std::string documentId = document->GetDocumentId();
    if (documentId.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "documentId is not initialized");
        return ERR_INVALID_VALUE;
    }
    if (oeExtensionRemoteObject == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "extension is null");
        return ERR_INVALID_VALUE;
    }
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    sptr<IRemoteObject> remoteObject = oeExtensionRemoteObject->GetRemoteObject();
    ErrCode ret = objectEditorManagerProxy->StopObjectEditorExtension(documentId, remoteObject, isPackageExtension);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "proxy fail:%{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetIcon(const std::string &hmid, std::string &resFilePath)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = objectEditorManagerProxy->GetIconByHmid(hmid, resFilePath);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "GetIconByHmid failed:%{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetFormatName(const std::string &hmid,
    const std::string &locale, std::string &formatName)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = objectEditorManagerProxy->GetFormatName(hmid, locale, formatName);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "GetFormatName failed:%{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetObjectEditorFormatByHmidAndLocale(
    const std::string &hmid,
    const std::string &locale,
    std::unique_ptr<ObjectEditorFormat> &format)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = objectEditorManagerProxy->GetObjectEditorFormatByHmidAndLocale(hmid, locale, format);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "failed:%{public}d", ret);
        return ret;
    }
    return ERR_OK;
}

ErrCode ObjectEditorClient::GetObjectEditorFormatsByLocale(const std::string &locale,
    std::vector<std::unique_ptr<ObjectEditorFormat>> &formats)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::CLIENT, "in");
    sptr<IObjectEditorManager> objectEditorManagerProxy = GetIObjectEditorManager();
    if (objectEditorManagerProxy == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "get proxy fail");
        return ERR_INVALID_VALUE;
    }
    ErrCode ret = objectEditorManagerProxy->GetObjectEditorFormatsByLocale(locale, formats);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT, "failed:%{public}d", ret);
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
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
