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

#ifndef MOCK_OBJECT_EDITOR_CLIENT_H
#define MOCK_OBJECT_EDITOR_CLIENT_H

#include <gmock/gmock.h>
#include "object_editor_client.h"

namespace OHOS {
namespace ObjectEditor {

class MockIRemoteObject : public IRemoteObject {
public:
    MOCK_METHOD(int32_t, SendRequest, (uint32_t, MessageParcel&, MessageParcel&, MessageOption&), (override));
    MOCK_METHOD(int32_t, GetObjectRefCount, (), (override));
    MOCK_METHOD(int, Dump, (int fd, const std::vector<std::u16string>& args), (override));
    MOCK_METHOD(bool, AddDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
    MOCK_METHOD(bool, RemoveDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
};

class MockISystemAbilityStatusChange : public ISystemAbilityStatusChange {
public:
    MOCK_METHOD(void, OnAddSystemAbility, (int32_t systemAbilityId, const std::string &deviceId), (override));
    MOCK_METHOD(void, OnRemoveSystemAbility, (int32_t systemAbilityId, const std::string &deviceId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

}
}
#endif