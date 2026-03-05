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

#include "common_event_support.h"
#include "want.h"

#include "hilog_object_editor.h"
#include "object_editor_event_manager.h"
#include "object_editor_screen_change_receiver.h"

namespace OHOS {
namespace ObjectEditor {

void ObjectEditorScreenChangeReceiver::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    std::string action = eventData.GetWant().GetAction();
    if (action.empty()) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::COMMON, "action is empty");
        return;
    }
    if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_LOCKED) {
        ObjectEditorEventManager::GetInstance().OnScreenLocked();
    } else if (action == EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_UNLOCKED) {
        ObjectEditorEventManager::GetInstance().OnScreenUnlocked();
    }
}

} // namespace ObjectEditor
} // namespace OHOS