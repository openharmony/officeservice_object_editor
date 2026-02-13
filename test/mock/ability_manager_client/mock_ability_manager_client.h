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

#ifndef MOCK_MESSAGE_PARCEL_H
#define MOCK_MESSAGE_PARCEL_H

#include <gmock/gmock.h>
#include "ability_manager_client.h"
#include "iservice_registry.h"

namespace OHOS {
namespace ObjectEditor {
constexpr int32_t DEFAULT_INVAL_VALUE = -1;

class CEAbilityManagerClient {
public:
    virtual ~CEAbilityManagerClient() = default;
    virtual ErrCode StartAbility(const AAFwk::Want &want, int requestCode = DEFAULT_INVAL_VALUE,
                                 int32_t userId = DEFAULT_INVAL_VALUE, uint64_t specifiedFullTokenId = 0) = 0;
    virtual ErrCode StartAbility(const AAFwk::Want &want, sptr<IRemoteObject> callerToken,
                                 int requestCode = DEFAULT_INVAL_VALUE, int32_t userId = DEFAULT_INVAL_VALUE,
                                 uint64_t specifiedFullTokenId = 0) = 0;
    virtual ErrCode StartAbility(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions,
                                 sptr<IRemoteObject> callerToken, int requestCode = DEFAULT_INVAL_VALUE,
                                 int32_t userId = DEFAULT_INVAL_VALUE) = 0;
    virtual ErrCode TerminateAbility(sptr<IRemoteObject> token, int resultCode, const AAFwk::Want *resultWant) = 0;
    static inline std::shared_ptr<CEAbilityManagerClient> client = nullptr;
};

class MockAbilityManagerClient : public CEAbilityManagerClient {
public:
    MockAbilityManagerClient() = default;
    ~MockAbilityManagerClient() override = default;
    MOCK_METHOD4(StartAbility,
                 ErrCode(const AAFwk::Want &want, int requestCode, int32_t userId, uint64_t specifiedFullTokenId));
    MOCK_METHOD5(StartAbility, ErrCode(const AAFwk::Want &want, sptr<IRemoteObject> callerToken, int requestCode,
                                       int32_t userId, uint64_t specifiedFullTokenId));
    MOCK_METHOD5(StartAbility, ErrCode(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions,
                                       sptr<IRemoteObject> callerToken, int requestCode, int32_t userId));
    MOCK_METHOD3(TerminateAbility, ErrCode(sptr<IRemoteObject> token, int resultCode, const AAFwk::Want *resultWant));
};

class MockAbilityManagerClientUtil {
public:
    static void SetIsNullInstance(bool flag);
};
}
}  // namespace OHOS
#endif
