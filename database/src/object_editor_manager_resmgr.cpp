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

#include "hilog_object_editor.h"
#include "object_editor_manager_resmgr.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
IMPLEMENT_SINGLE_INSTANCE(ObjectEditorManagerResmgr);

std::shared_ptr<Global::Resource::ResourceManager> ObjectEditorManagerResmgr::CreateResourceManager(
    const std::string &bundleName, const std::string &moduleName, const std::string &locale,
    const std::string &resourcePath, const std::string &hapPath)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName:%{public}s moduleName:%{public}s locale:%{public}s",
        bundleName.c_str(), moduleName.c_str(), locale.c_str());
    std::lock_guard<std::mutex> lock(resMgrMutex_);
    auto ptr = GetResourceManager(bundleName, moduleName, locale);
    if (ptr != nullptr) {
        return ptr;
    }
    std::unique_ptr<Global::Resource::ResConfig> resConfig(Global::Resource::CreateResConfig());
    if (!resConfig) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "CreateResConfig failed");
        return nullptr;
    }
    if (!locale.empty()) {
        Global::Resource::RState state = resConfig->SetLocaleInfo(locale.c_str());
        if (state != Global::Resource::RState::SUCCESS) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "SetLocaleInfo failed");
            return nullptr;
        }
    }
    auto resMgr = Global::Resource::CreateResourceManager(bundleName, moduleName, hapPath, {}, *resConfig);
    if (!resMgr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "CreateResourceManager failed");
        return nullptr;
    }
    resMgr->UpdateResConfig(*resConfig);

    const std::string &loadPath = hapPath.empty() ? resourcePath : hapPath;
    if (loadPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "invalid app resource");
        return nullptr;
    }
    if (!resMgr->AddResource(loadPath.c_str())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DATABASE, "AddResource failed");
        return nullptr;
    }
    AddResourceManager(bundleName, moduleName, locale, resMgr);
    return resMgr;
}

std::shared_ptr<Global::Resource::ResourceManager> ObjectEditorManagerResmgr::CreateResourceManager(
    const AppExecFwk::ExtensionAbilityInfo &extensionInfo, const std::string &locale)
{
    return CreateResourceManager(extensionInfo.bundleName, extensionInfo.moduleName, locale,
        extensionInfo.resourcePath, extensionInfo.hapPath);
}

void ObjectEditorManagerResmgr::RemoveBundle(const std::string &bundleName)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DATABASE, "bundleName:%{public}s", bundleName.c_str());
    std::lock_guard<std::mutex> lock(resMgrMutex_);
    resMgrs_.erase(bundleName);
}

void ObjectEditorManagerResmgr::AddResourceManager(const std::string &bundleName, const std::string &moduleName,
    const std::string &locale, std::shared_ptr<Global::Resource::ResourceManager> resMgr)
{
    auto bundleResMgr = resMgrs_.find(bundleName);
    if (bundleResMgr == resMgrs_.end()) {
        resMgrs_.emplace(bundleName, BundleResMgr{{std::make_tuple(moduleName, locale), resMgr}});
        return;
    }
    BundleResMgr &bundleResMgr = bundleResMgr->second;
    auto it = bundleResMgr.find(std::make_tuple(moduleName, locale));
    if (it == bundleResMgr.end()) {
        bundleResMgr.emplace(std::make_tuple(moduleName, locale), resMgr);
    }
}

std::shared_ptr<Global::Resource::ResourceManager> ObjectEditorManagerResmgr::GetResourceManager(
    const std::string &bundleName, const std::string &moduleName, const std::string &locale)
{
    auto bundleResMgr = resMgrs_.find(bundleName);
    if (bundleResMgr == resMgrs_.end()) {
        return nullptr;
    }
    BundleResMgr &bundleResMgr = bundleResMgr->second;
    auto it = bundleResMgr.find(std::make_tuple(moduleName, locale));
    if (it == bundleResMgr.end()) {
        return nullptr;
    }
    return it->second;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS