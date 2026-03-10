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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_H

#include <vector>
#include <string>
#include "single_instance.h"
#include "system_ability_status_change_stub.h"
#include "system_ability_load_callback_stub.h"
#include "object_editor_format.h"
#include "iobject_editor_manager.h"
#include "iobject_editor_service.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorLoadCallback : public SystemAbilityLoadCallbackStub {
public:
    ObjectEditorLoadCallback() = default;
    void OnLoadSystemAbilitySuccess(int32_t systemAbilityId, const sptr<IRemoteObject> &object);
    void OnLoadSystemAbilityFail(int32_t systemAbilityId);
};

class ObjectEditorAbilityListener : public SystemAbilityStatusChangeStub {
public:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
private:
    std::mutex mutex_;
};

class ObjectEditorClient {
    DECLARE_SINGLE_INSTANCE_BASE(ObjectEditorClient);

public:
    ErrCode StartObjectEditorExtension(
        std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IObjectEditorService> &oeExtensionRemoteObject,
        bool &isPackageExtension);
    ErrCode StopObjectEditorExtension(
        std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorService> &oeExtensionRemoteObject,
        const bool &isPackageExtension);

    ErrCode GetIcon(const std::string &hmid, std::string &resourceId);
    ErrCode GetFormatName(const std::string &hmid, const std::string &locale, std::string &formatName);
    ErrCode GetObjectEditorFormatByHmidAndLocale(const std::string &hmid, const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format);
    ErrCode GetObjectEditorFormatsByLocale(const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats);

    void LoadSystemAbilitySuccess(const sptr<IRemoteObject> &object);
    void LoadSystemAbilityFail();
    void SARegCleanUp();

private:
    class ObjectEditorSADeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        ObjectEditorSADeathRecipient() = default;
        ~ObjectEditorSADeathRecipient() override = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            ObjectEditorClient::GetInstance().OnRemoteDied(remote);
        }
    };
    void OnRemoteDied(const wptr<IRemoteObject> &remote);
    void SubscribeSystemAbility();
    void UnsubscribeSystemAbility();
    ~ObjectEditorClient();
    ObjectEditorClient() = default;
    sptr<IObjectEditorManager> GetIObjectEditorManager();
    sptr<IObjectEditorManager> GetObjectEditorProxy(const sptr<IRemoteObject> &object);
    void InitLoadState();
    bool WaitLoadStateChange();
    ErrCode PrepareFiles(const std::unique_ptr<ObjectEditorDocument> &document);
    ErrCode HandlePackage(const std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IObjectEditorService> &oeExtensionRemoteObject);
    std::string GenRandomUuid();

    std::mutex proxyMutex_;
    sptr<IObjectEditorManager> oeSAProxy_ { nullptr };
    sptr<IRemoteObject::DeathRecipient> deathRecipient_ = nullptr;
    std::condition_variable loadCond_;
    std::mutex loadMutex_;
    bool loadState_ = false;
    sptr<ISystemAbilityStatusChange> saStatusListener_ = nullptr;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_H
