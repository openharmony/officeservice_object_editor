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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_RESMGR_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_RESMGR_H

#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <tuple>
#include <utility>

#include "bundle_info.h"
#include "resource_manager.h"
#include "single_instance.h"

#include "object_editor_err_code.h"

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorManagerResmgr {
    DECLARE_SINGLE_INSTANCE(ObjectEditorManagerResmgr);

public:
    std::shared_ptr<Global::Resource::ResourceManager> CreateResourceManager(
        const std::string &bundleName, const std::string &moduleName, const std::string &locale,
        const std::string &resourcePath, const std::string &hapPath);
    std::shared_ptr<Global::Resource::ResourceManager> CreateResourceManager(
        const AppExecFwk::ExtensionAbilityInfo &extensionInfo, const std::string &locale = "");
    void RemoveBundle(const std::string &bundleName);

private:
    void AddResourceManager(const std::string &bundleName, const std::string &moduleName,
        const std::string &locale, std::shared_ptr<Global::Resource::ResourceManager> &resMgr);
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager(
        const std::string &bundleName, const std::string &moduleName, const std::string &locale) const;

    using BundleResMgr = std::map<std::tuple<std::string, std::string, std::string>,
        std::shared_ptr<Global::Resource::ResourceManager>>;
    std::mutex resMgrMutex_;
    BundleResMgr resMgrs_;
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_RESMGR_H