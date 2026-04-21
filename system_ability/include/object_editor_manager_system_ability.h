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
#include "object_editor_common.h"
#include "object_editor_connection.h"
#include "object_editor_client_death_recipient.h"
#include "iobject_editor_connection_status_callback.h"
#include "object_editor_screen_change_receiver.h"
namespace OHOS {
namespace ObjectEditor {

struct ContentEmbed_Diversion {
    std::string sourceOEid;
    std::string targetOEid;
    std::string minVersion;
};

enum class ServiceRunningState {
    STATE_NOT_START = 0,
    STATE_RUNNING = 1,
};

struct ExtensionStop {
    ExtensionStopReason reason;
    uint64_t timestamp;
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
    ErrCode StartObjectEditorExtension(
        std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject,
        bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(
        const std::string &documentId,
        const sptr<IRemoteObject> &oeExtensionRemoteObject,
        const bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(const sptr<IRemoteObject> &oeExtensionRemoteObject);
    ErrCode GetOEidByFileExtension(
        const std::string &oeid,
        std::string &fileExtension) override;
    ErrCode GetIconByOEid(
        const std::string &oeid,
        std::string &resourceId) override;
    ErrCode GetFormatName(
        const std::string &oeid,
        const std::string &locale,
        std::string &formatName) override;
    ErrCode GetObjectEditorFormatByOEidAndLocale(
        const std::string &oeid,
        const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format) override;
    ErrCode GetObjectEditorFormatsByLocale(
        const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) override;
    ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want,
        sptr<IRemoteObject> extensionToken, int32_t clientPid) override;
    ErrCode QueryExtensionStopReason(const sptr<IRemoteObject> &oeExtensionRemoteObject,
        ExtensionStopReason &stopReason) override;

    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;

    static std::mutex connectionMapMutex_;
    static std::map<sptr<IRemoteObject>, sptr<ObjectEditorConnection>> connectionMap_;

    static std::mutex extensionStopReasonMapMutex_;
    static std::map<sptr<IRemoteObject>, std::shared_ptr<ExtensionStop>> extensionStopReasonMap_;
    void RegisterExtensionStopReason(const sptr<IRemoteObject> &remoteObject, ExtensionStopReason reason);

protected:
    void OnStart() override;
    void OnStop() override;
    int32_t OnIdle(const SystemAbilityOnDemandReason &idleReason) override;

private:
    bool ConnectObjectEditorExtAbility(
        std::unique_ptr<ObjectEditorFormat> &format,
        sptr<IRemoteObject> &remoteObject,
        bool &isExceed);
    // 校验客户端APP是否有沙箱临时文件权限
    bool CheckClientFileValid(const ObjectEditorDocument &document);
    bool GrantClientFileUriPermissionToServerExtension(const ObjectEditorDocument &document,
        const std::string &targetBundleName);
    void ReadDiversionsJsonFile();
    ObjectEditorManagerErrCode GetTargetOEid(const std::string &sourceOEid,
        std::string &targetOEid, std::string &minVersion);
    ObjectEditorManagerErrCode HandleDefaultAppFormatPolicy(
        const std::string &fileExt,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats,
        std::unique_ptr<ObjectEditorFormat> &objectEditorFormat);
    ObjectEditorManagerErrCode GetDefaultAppBundleNameByFileExt(
        const std::string &fileExt, std::string &bundleName);
    ObjectEditorManagerErrCode CheckIsAllowStartExtension(const ObjectEditorDocument &document);
    ObjectEditorManagerErrCode HandleOperateHasOEid(const ObjectEditorDocument &document,
        std::unique_ptr<ObjectEditorFormat> &objectEditorFormat);
    ObjectEditorManagerErrCode GetObjectEditorFormat(const ObjectEditorDocument &document,
        std::unique_ptr<ObjectEditorFormat> &objectEditorFormat, bool &isPackageExtension);
    bool CheckCallingPermission(uint32_t code);
    void InitScreenChangedCommonEventSubscriber();
    void ResetScreenChangedCommonEventSubscriber();
    bool CheckConnectionLimit(const std::string &clientBundleName, std::unique_ptr<ObjectEditorFormat> &format,
        sptr<IRemoteObject> &remoteObject);
    std::string GetCallerBundleName();
    bool CheckRateLimitAdvanced();

    ServiceRunningState state_ = ServiceRunningState::STATE_NOT_START;
    std::shared_mutex diversionMapMutex_;
    std::map<std::string, ContentEmbed_Diversion> diversionMap_;

    void TimerThreadStopSA();
    void ResetStopSATimer();
    void TimerThreadCleanExtensionStopReason();
    std::mutex mutexCallback_;
    static std::mutex mutexTimer_;
    static std::condition_variable cvTimer_;
    static std::atomic<bool> timerRunning_;
    static std::atomic<bool> timerNotify_;
    static std::string permissionClient_;
    static std::mutex extensionStopCleanMutex_;
    static std::condition_variable cvExtensionStopClean_;
    static std::atomic<bool> extensionStopCleanRunning_;
    static std::atomic<bool> extensionStopCleanNotify_;

    std::shared_ptr<ObjectEditorScreenChangeReceiver> screenChangedReceiver_;

    std::atomic<uint32_t> requestCount_ = 0;
    std::atomic<uint64_t> windowStartMs_ = 0;
};

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_SYSTEM_ABILITY_H
