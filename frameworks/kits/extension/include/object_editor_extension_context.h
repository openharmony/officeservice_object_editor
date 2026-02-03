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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_CONTEXT_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_CONTEXT_H

#include "ability_connect_callback.h"
#include "connection_manager.h"
#include "extension_context.h"
#include "start_options.h"
#include "want.h"
#include "iobject_editor_manager.h"

namespace OHOS {
namespace AbilityRuntime {

using namespace OHOS::ObjectEditor;
/**
 * @brief context supply for ObjectEditor
 *
 */
class ObjectEditorExtensionContext : public ExtensionContext {
public:
    ObjectEditorExtensionContext() = default;
    virtual ~ObjectEditorExtensionContext() = default;

    /**
     * @brief Starts a new ability.
     * An ability uses this method to start a specific ability.
     * The system locates the target ability from installed abilities
     * based on the value of the want parameter and then starts it. You can
     * specify the ability to start using the want parameter.
     *
     * @param want Indicates the want containing information about the target
     * ability to start.
     *
     * @return errCode ERR_OK on success, others on failure.
     */
    ErrCode StartAbility(const AAFwk::Want &want) const;

    ErrCode StartAbility(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions) const;

    /**
     * @brief Connects the current ability to an ability.
     *
     * @param want Indicates the want containing information about the ability to
     * connect.
     *
     * @param connectCallback Indicates the callback object when the target ability is
     * connected.
     *
     * @return True means success and false means failure.
     */
    ErrCode ConnectAbility(const AAFwk::Want &want, const sptr<AbilityConnectCallback> &connectCallback) const;

    /**
     * @brief Starts a new ability.
     * An ability uses this method to start a specific ability.
     * The system locates the target ability from installed abilities
     * based on the value of the want parameter and then starts it. You can
     * specify the ability to start using the want parameter.
     *
     * @param want Indicates the want containing information about the target
     * ability to start.
     * @param accountId caller user.
     *
     * @return errCode ERR_OK on success, others on failure.
     */
    ErrCode StartAbilityWithAccount(const AAFwk::Want &want, int accountId) const;

    ErrCode StartAbilityWithAccount(const AAFwk::Want &want, int accountId,
        const AAFwk::StartOptions &startOptions) const;

    /**
     * @brief Connects the current ability to an ability.
     *
     * @param want Indicates the want containing information about the ability to
     * connect.
     *
     * @param accountId caller user.
     *
     * @param connectCallback Indicates the callback object when the target ability is
     * connected.
     *
     * @return True means success and false means failure.
     */
    ErrCode ConnectAbilityWithAccount(const AAFwk::Want &want, int accountId,
        const sptr<AbilityConnectCallback> &connectCallback) const;
    
    /**
     * @brief Disconnects the current ability from an ability.
     *
     * @param connectCallback Indicates the IAbilityConnection callback object passed by
     * connectAbility after the connection is set up. The IAbilityConnection
     * object uniquely identifies a connection between two abilities.
     *
     * @return True means success and false means failure.
     */
    ErrCode DisconnectAbility(const AAFwk::Want &want, const sptr<AbilityConnectCallback> &connectCallback) const;

    /**
     * @brief Destroys the current ability.
     *
     * @return errCode ERR_OK on success, others on failure.
     */
    ErrCode TerminateAbility();

    using SelfType = ObjectEditorExtensionContext;
    static const size_t CONTEXT_TYPE_ID;

protected:
    bool IsContext(size_t contextTypeId) override
    {
        return contextTypeId == CONTEXT_TYPE_ID || ExtensionContext::IsContext(contextTypeId);
    }

private:
    sptr<IObjectEditorManager> GetIObjectEditorManager() const;
    static int ILLEGAL_REQUEST_CODE;
    /**
     * @brief Get Current Ability Type.
     *
     * @return Current Ability Type.
     */
    OHOS::AppExecFwk::AbilityType GetAbilityInfoType() const;
};

} // namespace AbilityRuntime
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_CONTEXT_H