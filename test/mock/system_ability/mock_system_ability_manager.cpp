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

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "bundle_mgr_interface.h"
#include "mock_bundle_mgr.h"

namespace OHOS {

static sptr<IRemoteObject> g_mockBundleMgrRemoteObj = nullptr;
static sptr<ISystemAbilityManager> g_mockSaMgr = nullptr;
static bool g_saMgrNull = false;
static bool g_bundleMgrRemoteNull = false;

void SetMockSaMgrNull(bool isNull)
{
    g_saMgrNull = isNull;
}

void SetMockBundleMgrRemoteNull(bool isNull)
{
    g_bundleMgrRemoteNull = isNull;
}

void SetMockBundleMgrRemoteObj(const sptr<IRemoteObject> &obj)
{
    g_mockBundleMgrRemoteObj = obj;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    if (g_saMgrNull) {
        return nullptr;
    }
    return g_mockSaMgr;
}

class MockSystemAbilityManager : public ISystemAbilityManager {
public:
    MOCK_METHOD(sptr<IRemoteObject>, GetSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, CheckSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(int32_t, AddSystemAbility, (int32_t systemAbilityId, const std::string &deviceId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, GetSystemAbility, (int32_t systemAbilityId, const std::string &deviceId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, CheckSystemAbility, (int32_t systemAbilityId, const std::string &deviceId), (override));
    MOCK_METHOD(int32_t, RemoveSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(int32_t, SubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange> &listener), (override));
    MOCK_METHOD(int32_t, UnSubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange> &listener), (override));
    MOCK_METHOD(int32_t, AddSystemAbilityProcessInfo, (int32_t systemAbilityId, const std::string &processName, int32_t pid), (override));
};

static sptr<MockSystemAbilityManager> g_mockSaMgrInst = nullptr;

void InitMockSystemAbilityManager()
{
    g_mockSaMgrInst = sptr<MockSystemAbilityManager>::MakeSptr();
    g_mockSaMgr = g_mockSaMgrInst;
    ON_CALL(*g_mockSaMgrInst, GetSystemAbility(_))
        .WillByDefault(Return(g_bundleMgrRemoteNull ? nullptr : g_mockBundleMgrRemoteObj));
}

void ResetMockSystemAbilityManager()
{
    g_mockSaMgrInst = nullptr;
    g_mockSaMgr = nullptr;
    g_mockBundleMgrRemoteObj = nullptr;
    g_saMgrNull = false;
    g_bundleMgrRemoteNull = false;
}

} // namespace OHOS