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

#include "object_editor_extension_module_loader.h"
#include "hilog_object_editor.h"
#include "object_editor_extension.h"

namespace OHOS {
namespace AbilityRuntime {
// LCOV_EXCL_START
using namespace OHOS::ObjectEditor;
ObjectEditorExtensionModuleLoader::ObjectEditorExtensionModuleLoader() = default;
ObjectEditorExtensionModuleLoader::~ObjectEditorExtensionModuleLoader() = default;

Extension *ObjectEditorExtensionModuleLoader::Create(const std::unique_ptr<Runtime> &runtime) const
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::EXTENSION, "in");
    return ObjectEditorExtension::Create(runtime);
}

std::map<std::string, std::string> ObjectEditorExtensionModuleLoader::GetParams()
{
    std::map<std::string, std::string> params;
    params.insert(std::pair<std::string, std::string>("type", "600"));
    params.insert(std::pair<std::string, std::string>("name", "ObjectEditorExtension"));
    return params;
}

extern "C" __attribute__((visibility("default"))) void *OHOS_EXTENSION_GetExtensionModule()
{
    return &ObjectEditorExtensionModuleLoader::GetInstance();
}
// LCOV_EXCL_STOP
} // namespace AbilityRuntime
} // namespace OHOS