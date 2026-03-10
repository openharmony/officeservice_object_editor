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

#include "object_editor_manager_utils.h"
#include <algorithm>
#include <unordered_set>

#include "accesstoken_kit.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "nlohmann/json.hpp"
#include "system_ability_definition.h"

#include "hilog_object_editor.h"
#include "object_editor_manager_resmgr.h"
#include "system_utils.h"
#include "user_mgr.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
namespace {
const std::string CONFIG_FILE_KEY = "content_embed_config";
const std::string MEDIA_PREFIX = "$media:";
const std::string STRING_PREFIX = "$string:";
const std::string HMID = "hmid";
const std::string FILE_EXTS = "file_exts";
const std::string ICON_ID = "icon_id";
const std::string NAME_ID = "name_id";
const std::string DESCRIPTION_ID = "description_id";
const std::string BUNDLE_NAME = "bundle_name";
const std::string MODULE_NAME = "module_name";
const std::string ABILITY_NAME = "ability_name";
const std::string CREATE_TIME = "create_time";
const std::string RESOURCE_PATH = "resource_path";
const std::string HAP_PATH = "hap_path";
const std::string VERSION = "version";

std::shared_ptr<Media::PixelMap> GetIconPixelMap(Global::Resource::ResourceManager &resMgr,
    int iconId, const std::string &hapPath)
{
    if (iconId < 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "failed, invalid iconId: %{private}d", iconId);
        return nullptr;
    }
    Media::SourceOptions options;
    uint32_t errCode = 0;
    std::unique_ptr<Media::ImageSource> imageSource = nullptr;
    if (!hapPath.empty()) { // hap compressed
        std::unique_ptr<uint8_t[]> iconOut;
        size_t len = 0;
        if (resMgr.GetMediaDataById(iconId, len, iconOut) != Global::Resource::RState::SUCCESS) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get icon failed, iconId: %{private}u", iconId);
            return nullptr;
        }
        imageSource = Media::ImageSource::CreateImageSource(iconOut.get(), len, options, errCode);
    } else { // hap decompressed
        std::string iconPath;
        if (resMgr.GetMediaById(iconId, iconPath) != Global::Resource::RState::SUCCESS) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get icon failed, iconId: %{private}u", iconId);
            return nullptr;
        }
        imageSource = Media::ImageSource::CreateImageSource(iconPath, options, errCode);
    }
    if (errCode != 0 || imageSource == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "create image source failed:%{public}u", errCode);
        return nullptr;
    }
    Media::DecodeOptions decodeOptions;
    auto pixelMap = imageSource->CreatePixelMap(decodeOptions, errCode);
    if (errCode != 0 || pixelMap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "create pixel map failed:%{public}u", errCode);
        return nullptr;
    }
    return std::shared_ptr<Media::PixelMap>(pixelMap.release());
}

std::string GetConfigFileName(const AppExecFwk::ExtensionAbilityInfo &extensionInfo)
{
    std::string resName;
    for (const auto &config : extensionInfo.metadata) {
        if (config.name == CONFIG_FILE_KEY) {
            resName = config.resource;
            break;
        }
    }
    if (resName.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get config file name failed");
        return "";
    }
    return SystemUtils::GetSubstrByPrefix(resName, "$profile:");
}

bool ReadConfigFile(std::string &fileContent, Global::Resource::ResourceManager &resMgr,
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo)
{
    std::string configFileName = GetConfigFileName(extensionInfo);
    if (configFileName.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get config file name failed");
        return false;
    }
    const std::string &resPath = extensionInfo.hapPath.empty() ? extensionInfo.resourcePath : extensionInfo.hapPath;
    if (resPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get config file path failed");
        return false;
    }
    if (!extensionInfo.hapPath.empty()) { // hap compressed
        std::unique_ptr<uint8_t[]> fileContentPtr = nullptr;
        size_t len = 0;
        if (resMgr.GetProfileDataByName(configFileName.c_str(), len, fileContentPtr) !=
            Global::Resource::RState::SUCCESS) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "fail: get profile data failed");
            return false;
        }
        if (fileContentPtr == nullptr || len == 0) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file data");
            return false;
        }
        fileContent = std::string(fileContentPtr.get(), fileContentPtr.get() + len);
    } else { // hap decompressed
        std::string configFilePath;
        if (resMgr.GetProfileByName(configFileName.c_str(), configFilePath) !=
            Global::Resource::RState::SUCCESS) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get config file path failed");
            return false;
        }
        fileContent = SystemUtils::ReadFile(configFilePath);
    }
    return true;
}

bool GetConfigHmid(const nlohmann::json &json, std::string &hmid)
{
    auto it = json.find(HMID);
    if (it == json.end() || !it->is_string()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, no hmid");
        return false;
    }
    hmid = it.value();
    return true;
}

bool GetConfigFileExts(const nlohmann::json &json, std::string &fileExts)
{
    auto it = json.find(FILE_EXTS);
    if (it == json.end() || !it->is_string()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, no file_exts");
        return false;
    }
    fileExts = it.value();
    return true;
}

bool GetConfigIconId(const nlohmann::json &json, int &iconId)
{
    auto it = json.find("icon");
    if (it == json.end() || !it->is_string()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, no icon");
        return false;
    }
    if (!SystemUtils::GetIntByPrefix(it.value(), MEDIA_PREFIX, iconId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, icon is invalid");
        return false;
    }
    return true;
}

bool GetConfigNameId(const nlohmann::json &json, int &nameId)
{
    auto it = json.find("name");
    if (it == json.end() || !it->is_string()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, no name");
        return false;
    }
    if (!SystemUtils::GetIntByPrefix(it.value(), STRING_PREFIX, nameId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, name is invalid");
        return false;
    }
    return true;
}

bool GetConfigDescriptionId(const nlohmann::json &json, int &descriptionId)
{
    auto it = json.find("description");
    if (it == json.end() || !it->is_string()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, no description");
        return false;
    }
    if (!SystemUtils::GetIntByPrefix(it.value(), STRING_PREFIX, descriptionId)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, description is invalid");
        return false;
    }
    return true;
}

bool ParseConfigEntry(NativeRdb::ValuesBucket &bucket, const nlohmann::json &configEntry,
    Global::Resource::ResourceManager &resMgr, const AppExecFwk::BundleInfo &bundleInfo,
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo)
{
    std::string hmid;
    if (!GetConfigHmid(configEntry, hmid)) {
        return false;
    }
    std::string fileExts;
    if (!GetConfigFileExts(configEntry, fileExts)) {
        return false;
    }
    int nameId = -1;
    if (!GetConfigNameId(configEntry, nameId)) {
        return false;
    }
    int iconId = -1;
    if (!GetConfigIconId(configEntry, iconId)) {
        return false;
    }
    int descriptionId = 0;
    if (!GetConfigDescriptionId(configEntry, descriptionId)) {
        descriptionId = -1;
    }
    bucket.PutString(HMID, hmid);
    bucket.PutString(BUNDLE_NAME, extensionInfo.bundleName);
    bucket.PutString(MODULE_NAME, extensionInfo.moduleName);
    bucket.PutString(ABILITY_NAME, extensionInfo.name);
    bucket.PutString(RESOURCE_PATH, extensionInfo.resourcePath);
    bucket.PutString(HAP_PATH, extensionInfo.hapPath);
    bucket.PutString(VERSION, bundleInfo.versionName);
    bucket.PutInt(NAME_ID, nameId);
    bucket.PutInt(DESCRIPTION_ID, descriptionId);
    bucket.PutString(FILE_EXTS, fileExts);
    bucket.PutInt(ICON_ID, iconId);
    bucket.PutLong(CREATE_TIME, bundleInfo.updateTime);
    return true;
}

void ParseConfigFile(const std::string &fileContent, std::vector<NativeRdb::ValuesBucket> &buckets,
    Global::Resource::ResourceManager &resMgr, const AppExecFwk::BundleInfo &bundleInfo,
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo)
{
    if (fileContent.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, empty content");
        return;
    }
    nlohmann::json profileJson = nlohmann::json::parse(fileContent, nullptr, false);
    if (profileJson.is_discarded()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file, parse failed");
        return;
    }
    auto it = profileJson.find(CONFIG_FILE_KEY);
    if (it == profileJson.end() || it->is_discarded() || !it->is_array() || it->empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid config file");
        return;
    }
    size_t size = it->size();
    for (size_t i = 0; i < size; ++i) {
        const nlohmann::json &configEntry = it->at(i);
        if (!configEntry.is_object()) {
            continue;
        }
        NativeRdb::ValuesBucket bucket;
        if (ParseConfigEntry(bucket, configEntry, resMgr, bundleInfo, extensionInfo)) {
            buckets.emplace_back(std::move(bucket));
        }
    }
}
} // namespace

sptr<AppExecFwk::IBundleMgr> GetBundleMgr()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "in");
    sptr<ISystemAbilityManager> samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "samgr is null");
        return nullptr;
    }
    sptr<IRemoteObject> bundleMgrObj = samgr->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleMgrObj == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgrObj is null");
        return nullptr;
    }
    sptr<AppExecFwk::IBundleMgr> bundleMgr = iface_cast<AppExecFwk::IBundleMgr>(bundleMgrObj);
    if (bundleMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgr is null");
        return nullptr;
    }
    return bundleMgr;
}

bool GetAppIdentifier(std::string &appIdentifier)
{
    uint32_t callerToken = IPCSkeleton::GetCallingTokenID();
    Security::AccessToken::HapTokenInfo hapTokenInfo;
    ErrCode ret = Security::AccessToken::AccessTokenKit::GetHapTokenInfo(callerToken, hapTokenInfo);
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetHapTokenInfo failed");
        return false;
    }
    std::string bundleName = hapTokenInfo.bundleName;
    auto bundleMgr = GetBundleMgr();
    if (bundleMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgr is null");
        return false;
    }
    AppExecFwk::BundleInfo bundleInfo;
    ret = bundleMgr->GetBundleInfoV9(bundleName,
        static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_WITH_SIGNATURE_INFO),
        bundleInfo, UserMgr::GetInstance().GetUserId());
    if (ret != ERR_OK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetBundleInfo failed, bundleName: %{public}s",
            bundleName.c_str());
        return false;
    }
    if (bundleInfo.signatureInfo.appIdentifier.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "appIdentifier is empty");
        return false;
    }
    appIdentifier = bundleInfo.signatureInfo.appIdentifier;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "appIdentifier: %{private}s", appIdentifier.c_str());
    return true;
}

bool IsLegalCalling()
{
    return true;
}

void GetBundleInfos(const sptr<AppExecFwk::IBundleMgr> &bundleMgr,
    const std::vector<AppExecFwk::ExtensionAbilityInfo> &extensionInfos,
    std::map<std::string, AppExecFwk::BundleInfo> &bundleInfos)
{
    if (bundleMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "bundleMgr is null");
        return;
    }
    for (const auto &extensionInfo : extensionInfos) {
        if (bundleInfos.find(extensionInfo.bundleName) != bundleInfos.end()) {
            continue;
        }
        AppExecFwk::BundleInfo bundleInfo;
        ErrCode ret = bundleMgr->GetBundleInfoV9(extensionInfo.bundleName,
            static_cast<int32_t>(AppExecFwk::GetBundleInfoFlag::GET_BUNDLE_INFO_DEFAULT),
            bundleInfo, UserMgr::GetInstance().GetUserId());
        if (ret != ERR_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetBundleInfo failed, bundleName: %{public}s",
                extensionInfo.bundleName.c_str());
            continue;
        }
        bundleInfos.emplace(extensionInfo.bundleName, std::move(bundleInfo));
    }
}

bool BuildObjectEditorFormat(ObjectEditorFormat &format, NativeRdb::RowEntity &rowEntity,
    const std::string &locale)
{
    std::string resourcePath;
    std::string hapPath;
    int nameId = -1;
    int descriptionId = -1;
    int iconId = -1;

    rowEntity.Get(HMID).GetString(format.hmid);
    rowEntity.Get(BUNDLE_NAME).GetString(format.bundleName);
    rowEntity.Get(MODULE_NAME).GetString(format.moduleName);
    rowEntity.Get(ABILITY_NAME).GetString(format.abilityName);
    rowEntity.Get(RESOURCE_PATH).GetString(resourcePath);
    rowEntity.Get(HAP_PATH).GetString(hapPath);
    rowEntity.Get(VERSION).GetString(format.version);
    format.locale = locale;
    rowEntity.Get(NAME_ID).GetInt(nameId);
    rowEntity.Get(DESCRIPTION_ID).GetInt(descriptionId);
    rowEntity.Get(FILE_EXTS).GetString(format.fileExts);
    rowEntity.Get(ICON_ID).GetInt(iconId);
    rowEntity.Get(CREATE_TIME).GetLong(format.createTime);
    auto resMgr = ObjectEditorManagerResmgr::GetInstance().CreateResourceManager(
        format.bundleName, format.moduleName, locale, resourcePath, hapPath);
    if (resMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "resMgr is null");
        return false;
    }
    if (nameId < 0 || resMgr->GetStringById(nameId, format.formatName) != Global::Resource::RState::SUCCESS) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get nameId: %{private}d failed", nameId);
        return false;
    }
    if (descriptionId < 0 ||
        resMgr->GetStringById(descriptionId, format.description) != Global::Resource::RState::SUCCESS) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "get descriptionId: %{private}d failed", descriptionId);
        return false;
    }
    format.pIconPixelMap = GetIconPixelMap(*resMgr, iconId, hapPath);
    if (format.pIconPixelMap == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "GetIconPixelMap failed, iconId: %{private}d", iconId);
        return false;
    }
    return true;
}

bool BuildValuesBuckets(std::vector<NativeRdb::ValuesBucket> &buckets, const AppExecFwk::BundleInfo &bundleInfo,
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DATABASE, "enter");
    auto resMgr = ObjectEditorManagerResmgr::GetInstance().CreateResourceManager(extensionInfo);
    if (resMgr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "resMgr is null");
        return false;
    }
    std::string configFileContent;
    if (!ReadConfigFile(configFileContent, *resMgr, extensionInfo)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "read configFile failed");
        return false;
    }
    ParseConfigFile(configFileContent, buckets, *resMgr, bundleInfo, extensionInfo);
    return true;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS