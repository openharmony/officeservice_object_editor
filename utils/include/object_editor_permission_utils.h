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

#ifndef OHOS_OBJECT_EDITOR_PERMISSION_UTILS_H
#define OHOS_OBJECT_EDITOR_PERMISSION_UTILS_H

#include <string>

namespace OHOS {
namespace ObjectEditor {
constexpr const char* PERMISSION_SERVER = "ohos.permission.REGISTER_OBJECTEDITOR_EXTENSION";
class ObjectEditorPermissionUtils {
public:
    static bool CheckCallingPermission(const std::string &permission);
    static bool CheckRequestPermission(const std::string &bundleName, const std::string &permission);
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_PERMISSION_UTILS_H