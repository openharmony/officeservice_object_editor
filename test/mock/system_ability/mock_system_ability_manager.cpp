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
#include "iremote_object.h"
#include "if_system_ability_manager.h"

namespace OHOS {

static bool g_saMgrNull = false;
static bool g_bundleMgrRemoteNull = false;
static sptr<IRemoteObject> g_mockBundleMgrRemoteObj = nullptr;

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

class StubSystemAbilityManager : public ISystemAbilityManager {
public:
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    }

    std::vector<std::u16string> ListSystemAbilities(unsigned int dumpFlags) override
    {
        return {};
    }

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId) override
    {
        if (g_bundleMgrRemoteNull) {
            return nullptr;
        }
        return g_mockBundleMgrRemoteObj;
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId) override
    {
        return nullptr;
    }

    int32_t RemoveSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }

    int32_t SubscribeSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityStatusChange> &listener) override
    {
        return 0;
    }

    int32_t UnSubscribeSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityStatusChange> &listener) override
    {
        return 0;
    }

    sptr<IRemoteObject> GetSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override
    {
        return nullptr;
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override
    {
        return nullptr;
    }

    int32_t AddOnDemandSystemAbilityInfo(int32_t systemAbilityId,
        const std::u16string &localAbilityManagerName) override
    {
        return 0;
    }

    sptr<IRemoteObject> CheckSystemAbility(int32_t systemAbilityId, bool &isExist) override
    {
        isExist = false;
        return nullptr;
    }

    int32_t AddSystemAbility(int32_t systemAbilityId, const sptr<IRemoteObject> &ability,
        const SAExtraProp &extraProp) override
    {
        return 0;
    }

    int32_t AddSystemProcess(const std::u16string &procName,
        const sptr<IRemoteObject> &procObject) override
    {
        return 0;
    }

    sptr<IRemoteObject> LoadSystemAbility(int32_t systemAbilityId, int32_t timeout) override
    {
        return nullptr;
    }

    int32_t LoadSystemAbility(int32_t systemAbilityId,
        const sptr<ISystemAbilityLoadCallback> &callback) override
    {
        return 0;
    }

    int32_t LoadSystemAbility(int32_t systemAbilityId, const std::string &deviceId,
        const sptr<ISystemAbilityLoadCallback> &callback) override
    {
        return 0;
    }

    int32_t UnloadSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }

    int32_t CancelUnloadSystemAbility(int32_t systemAbilityId) override
    {
        return 0;
    }

    int32_t UnloadAllIdleSystemAbility() override
    {
        return 0;
    }

    int32_t GetSystemProcessInfo(int32_t systemAbilityId,
        SystemProcessInfo &systemProcessInfo) override
    {
        return 0;
    }

    int32_t GetRunningSystemProcess(std::list<SystemProcessInfo> &systemProcessInfos) override
    {
        return 0;
    }

    int32_t SubscribeSystemProcess(const sptr<ISystemProcessStatusChange> &listener) override
    {
        return 0;
    }

    int32_t SendStrategy(int32_t type, std::vector<int32_t> &systemAbilityIds,
        int32_t level, std::string &action) override
    {
        return 0;
    }

    int32_t UnSubscribeSystemProcess(const sptr<ISystemProcessStatusChange> &listener) override
    {
        return 0;
    }

    int32_t GetExtensionSaIds(const std::string &extension,
        std::vector<int32_t> &saIds) override
    {
        return 0;
    }

    int32_t GetExtensionRunningSaList(const std::string &extension,
        std::vector<sptr<IRemoteObject>> &saList) override
    {
        return 0;
    }

    int32_t GetRunningSaExtensionInfoList(const std::string &extension,
        std::vector<SaExtensionInfo> &infoList) override
    {
        return 0;
    }

    int32_t GetCommonEventExtraDataIdlist(int32_t saId,
        std::vector<int64_t> &extraDataIdList, const std::string &eventName) override
    {
        return 0;
    }

    int32_t GetOnDemandReasonExtraData(int64_t extraDataId,
        MessageParcel &extraDataParcel) override
    {
        return 0;
    }

    int32_t GetOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        std::vector<SystemAbilityOnDemandEvent> &abilityOnDemandEvents) override
    {
        return 0;
    }

    int32_t UpdateOnDemandPolicy(int32_t systemAbilityId, OnDemandPolicyType type,
        const std::vector<SystemAbilityOnDemandEvent> &abilityOnDemandEvents) override
    {
        return 0;
    }

    int32_t GetOnDemandSystemAbilityIds(std::vector<int32_t> &systemAbilityIds) override
    {
        return 0;
    }
};

static sptr<StubSystemAbilityManager> g_stubSaMgr = nullptr;

void InitMockSystemAbilityManager()
{
    g_stubSaMgr = sptr<StubSystemAbilityManager>::MakeSptr();
}

void ResetMockSystemAbilityManager()
{
    g_stubSaMgr = nullptr;
    g_mockBundleMgrRemoteObj = nullptr;
    g_saMgrNull = false;
    g_bundleMgrRemoteNull = false;
}

sptr<ISystemAbilityManager> SystemAbilityManagerClient::GetSystemAbilityManager()
{
    if (g_saMgrNull) {
        return nullptr;
    }
    return g_stubSaMgr;
}

} // namespace OHOS
