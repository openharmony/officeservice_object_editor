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

#include "mock_ability_manager_client.h"

namespace OHOS {
namespace {
bool g_isNullInstance = false;
}

namespace ObjectEditor {
void MockAbilityManagerClientUtil::SetIsNullInstance(bool flag)
{
    g_isNullInstance = flag;
}
}  // namespace ObjectEditor

namespace AAFwk {
using namespace ObjectEditor;
std::shared_ptr<AbilityManagerClient> AbilityManagerClient::instance_ = nullptr;

std::shared_ptr<AbilityManagerClient> AbilityManagerClient::GetInstance()
{
    if (g_isNullInstance) {
        return nullptr;
    }
    if (instance_) {
        return instance_;
    }
    instance_ = std::make_shared<AbilityManagerClient>();
    return instance_;
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, int requestCode, int32_t userId,
                                           uint64_t specifiedFullTokenId)
{
    if (CEAbilityManagerClient::client == nullptr) {
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    return CEAbilityManagerClient::client->StartAbility(want, requestCode, userId, specifiedFullTokenId);
}

ErrCode AbilityManagerClient::StartAbility(const AAFwk::Want &want, sptr<IRemoteObject> callerToken, int requestCode,
                                           int32_t userId, uint64_t specifiedFullTokenId)
{
    if (CEAbilityManagerClient::client == nullptr) {
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    return CEAbilityManagerClient::client->StartAbility(want, callerToken, requestCode, userId, specifiedFullTokenId);
}

ErrCode AbilityManagerClient::StartAbility(const Want &want, const StartOptions &startOptions,
                                           sptr<IRemoteObject> callerToken, int requestCode, int32_t userId)
{
    if (CEAbilityManagerClient::client == nullptr) {
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    return CEAbilityManagerClient::client->StartAbility(want, startOptions, callerToken, requestCode, userId);
}

ErrCode AbilityManagerClient::TerminateAbility(sptr<IRemoteObject> token, int resultCode, const AAFwk::Want *resultWant)
{
    if (CEAbilityManagerClient::client == nullptr) {
        return ABILITY_SERVICE_NOT_CONNECTED;
    }
    return CEAbilityManagerClient::client->TerminateAbility(token, resultCode, resultWant);
}
}  // namespace AAFwk
}  // namespace OHOS