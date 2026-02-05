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

#include "ability_connect_callback_stub.h"
#include "ability_manager_client.h"

#include "hilog_object_editor.h"
#include "object_editor_err_code.h"

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorConnection : public AAFwk::AbilityConnectCallbackStub {
public:
    ObjectEditorConnection();
    virtual ~ObjectEditorConnection() = default;

    void OnAbilityConnectDone(const AppExecFwk::ElementName &element,
        const sptr<IRemoteObject> &remoteObject, int32_t resultCode) override;
    void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int32_t resultCode) override;
    ObjectEditorManagerErrCode StartConnect(const std::string &bundleName, const std::string &abilityName,
        const std::string &moduleName, sptr<IRemoteObject> &remoteObject);
    ObjectEditorManagerErrCode StopConnect();

protected:
    const int8_t CONNECT_TIME_OUT = 3;

private:
    ObjectEditorManagerErrCode DoConnect(const std::string &bundleName, const std::string &abilityName,
        const std::string &moduleName, sptr<IRemoteObject> &remoteObject);

    std::mutex extensionProxyMutex_;
    sptr<IRemoteObject> extensionProxy_ = nullptr;
    std::condition_variable connectCondition_;
    bool isConnectReady_ = false;
};

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONNECTION_H
