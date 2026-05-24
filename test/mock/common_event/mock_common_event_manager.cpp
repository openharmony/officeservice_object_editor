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

#include "common_event_manager.h"
#include "common_event_subscriber.h"

namespace OHOS {
namespace EventFwk {

static bool g_subscribeCommonEventResult = true;
static bool g_unsubscribeCommonEventResult = true;

void SetSubscribeCommonEventResult(bool result)
{
    g_subscribeCommonEventResult = result;
}

void SetUnsubscribeCommonEventResult(bool result)
{
    g_unsubscribeCommonEventResult = result;
}

bool CommonEventManager::SubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_subscribeCommonEventResult;
}

bool CommonEventManager::UnSubscribeCommonEvent(const std::shared_ptr<CommonEventSubscriber> &subscriber)
{
    return g_unsubscribeCommonEventResult;
}

} // namespace EventFwk
} // namespace OHOS