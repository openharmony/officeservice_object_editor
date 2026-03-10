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

#include "object_editor_event_manager.h"

namespace OHOS {
namespace ObjectEditor {
IMPLEMENT_SINGLE_INSTANCE(ObjectEditorEventManager);

void ObjectEditorEventManager::OnScreenLocked()
{
    isScreenLocked_.store(true);
}
void ObjectEditorEventManager::OnScreenUnlocked()
{
    isScreenLocked_.store(false);
}
bool ObjectEditorEventManager::CheckIsScreenLocked()
{
    return isScreenLocked_.load();
}

} // namespace ObjectEditor
} // namespace OHOS
