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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONNECTION_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONNECTION_H

#include <thread>

#include "ability_connect_callback_stub.h"
#include "ability_manager_client.h"

#include "hilog_object_editor.h"
#include "iobject_editor_connection_status_callback.h"
#include "object_editor_err_code.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorConnection : public AAFwk::AbilityConnectionStub {
public:
    ObjectEditorConnection(){};
    ~ObjectEditorConnection();

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    ObjectEditorManagerErrCode StartConnect(const std::string &bundleName, const std::string &abilityName,
        const std::string &moduleName, sptr<IRemoteObject> &remoteObject);
    ObjectEditorManagerErrCode StopConnect();
    void RegisterConnectionStatusCallback(const std::shared_ptr<IObjectEditorConnectionStatusCallback> callback);
    std::string& GetExtensionBundleName();
    void SetClientBundleName(const std::string &bundleName);
    std::string& GetClientBundleName();
    bool IsExtensionAbilityMatch(const std::string &moduleName, const std::string &abilityName);

protected:
    const int8_t CONNECT_TIME_OUT = 3;

private:
    ObjectEditorManagerErrCode DoConnect(const std::string &bundleName, const std::string &abilityName,
        const std::string &moduleName, sptr<IRemoteObject> &remoteObject);

    std::string extensionBundleName_;
    std::string extensionAbilityName_;
    std::string extensionModuleName_;
    std::string clientBundleName_;
    std::mutex extensionProxyMutex_;
    sptr<IRemoteObject> extensionProxy_ = nullptr;
    std::condition_variable connectCondition_;
    bool isConnectReady_ = false;
    std::mutex connectionStatusCallbackMutex_;
    std::shared_ptr<IObjectEditorConnectionStatusCallback> connectionStatusCallback_ = nullptr;
    int32_t curCheckEditStatusTimes = 0;

    void TimerThreadStopExtension();
    void ResetStopExtensionTimer();
    bool CheckRemoteEditStatus();

    std::mutex mutexTimer_;
    std::mutex mutexTimerRunning_;
    std::condition_variable cvTimer_;
    std::atomic<bool> timerRunning_{false};
    std::atomic<bool> timerNotify_{false};
    std::thread timerThread_;
    std::atomic<bool> timerStopFlag_{false};
    static const int32_t EXTENSION_STOP_TIME_S = 20;
};

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONNECTION_H
