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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_SYSTEM_ABILITY_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_SYSTEM_ABILITY_H

#include <map>
#include <mutex>
#include "iremote_object.h"
#include "system_ability.h"
#include "single_instance.h"

#include "object_editor_manager_stub.h"
#include "object_editor_document.h"
#include "object_editor_format.h"
#include "object_editor_connection.h"
#include "object_editor_client_death_recipient.h"
#include "iobject_editor_connection_status_callback.h"

namespace OHOS {
namespace ObjectEditor {

struct ContentEmbed_Diversion {
    std::string sourceHmid;
    std::string targetHmid;
    std::string minVersion;
};

enum class ServiceRunningState {
    STATE_NOT_START = 0,
    STATE_RUNNING = 1,
};

class ObjectEditorManagerSystemAbilityConnectionStatusCallback : public IObjectEditorConnectionStatusCallback {
public:
    ObjectEditorManagerSystemAbilityConnectionStatusCallback() = default;
    virtual ~ObjectEditorManagerSystemAbilityConnectionStatusCallback() = default;
    void OnConnectionStatusChanged(sptr<IRemoteObject> &remoteObject,
        const ObjectEditorConnectionStatus status) override;
};

class ObjectEditorManagerSystemAbility : public SystemAbility, public ObjectEditorManagerStub {
    DECLARE_SYSTEM_ABILITY(ObjectEditorManagerSystemAbility);
    DECLARE_SINGLE_INSTANCE_BASE(ObjectEditorManagerSystemAbility);
public:
    ObjectEditorManagerSystemAbility();
    ~ObjectEditorManagerSystemAbility();

    ErrCode StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
                                       const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
                                       sptr<IRemoteObject> &oeExtensionRemoteObject,
                                       bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
                                      const sptr<IRemoteObject> &oeExtensionRemoteObject,
                                      const bool &isPackageExtension) override;
    ErrCode GetHmidByFileExtension(
        const std::string &hmid,
        std::string &fileExtension) override;
    ErrCode GetIconByHmid(
        const std::string &hmid,
        std::string &resourceId) override;
    ErrCode GetFormatName(
        const std::string &hmid,
        const std::string &locale,
        std::string &formatName) override;
    ErrCode GetObjectEditorFormatByHmidAndLocale(
        const std::string &hmid,
        const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format) override;
    ErrCode GetObjectEditorFormatsByLocale(
        const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) override;
    ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want) override;
    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;

    static std::mutex connectionMapMutex_;
    static std::map<sptr<IRemoteObject>, sptr<ObjectEditorConnection>> connectionMap_;

protected:
    void OnStart() override;
    void OnStop() override;
    int32_t OnIdle(const SystemAbilityOnDemandReason &idleReason) override;

private:
    bool ConnectObjectEditorExtAbility(
        std::unique_ptr<ObjectEditorFormat> &format,
        sptr<IRemoteObject> &remoteObject);
    // 校验客户端APP是否有沙箱临时文件权限
    bool CheckClientFileValid(const ObjectEditorDocument &document);
    bool GrantClientFileUriPermissionToServerExtension(const ObjectEditorDocument &document,
        const std::string &targetBundleName);
    void ReadDiversionsJsonFile();
    ObjectEditorManagerErrCode HandlePackage(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension);
    ObjectEditorManagerErrCode GetTargetHmid(const std::string &sourceHmid,
        std::string &targetHmid, std::string &minVersion);
    ObjectEditorManagerErrCode HandleDefaultAppFormatPolicy(const std::string &fileExt,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats,
        std::unique_ptr<ObjectEditorFormat> &objectEditorFormat);
    ObjectEditorManagerErrCode GetDefaultAppBundleNameByFileExt(const std::string &fileExt,
        std::string &bundleName);
    ObjectEditorManagerErrCode CheckIsAllowStartExtension(const ObjectEditorDocument &document);
    ObjectEditorManagerErrCode HandleOperateHasHmid(const ObjectEditorDocument &document,
        std::unique_ptr<ObjectEditorFormat> &objectEditorFormat);
    ObjectEditorManagerErrCode StartObjectEditorExtensionByFile(const ObjectEditorDocument &document,
        const sptr<IObjectEditorClientCallback> &clientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject, bool &isPackageExtension);

    void TimerThreadStopSA();
    void ResetStopSATimer();
    
    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    std::shared_mutex diversionMapMutex_;
    std::map<std::string, ContentEmbed_Diversion> diversionMap_;

    std::mutex mutexCallback_;
    static std::mutex mutexTimer_;
    static std::condition_variable cvTimer_;
    static bool timerRunning_;
    static bool timerNotify_;
};

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_SYSTEM_ABILITY_H
