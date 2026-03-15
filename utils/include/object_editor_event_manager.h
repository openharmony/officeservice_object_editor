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

#ifndef OHOS_OBJECT_EDITOR_EVENT_MANAGER_H
#define OHOS_OBJECT_EDITOR_EVENT_MANAGER_H

#include <atomic>
#include <mutex>
#include <string>

#include "single_instance.h"

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorEventManager {
DECLARE_SINGLE_INSTANCE(ObjectEditorEventManager);
public:
    void OnScreenLocked();
    void OnScreenUnlocked();
    bool CheckIsScreenLocked();

private:
    std::atomic<bool> isScreenLocked_{false};
};
} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_EVENT_MANAGER_H
