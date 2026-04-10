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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_H

#include "extension_base.h"
#include "object_editor_extension_context.h"
#include "runtime.h"
#include "native_object_editor_types.h"
#include "display_manager.h"
#include "system_ability_status_change_stub.h"
#include "object_editor_extension_stub.h"

namespace OHOS {
namespace AbilityRuntime {

using namespace OHOS::ObjectEditor;
/**
 * @brief Basic ObjectEditor components.
 */
class ObjectEditorExtension
    : public ExtensionBase<ObjectEditorExtensionContext>, public ObjectEditorExtensionStub {
public:
    ObjectEditorExtension() = default;
    virtual ~ObjectEditorExtension();

    /**
     * @brief Create and init context.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     * @return The created context.
     */
    virtual std::shared_ptr<ObjectEditorExtensionContext> CreateAndInitContext(
        const std::shared_ptr<AbilityLocalRecord> &record, const std::shared_ptr<OHOSApplication> &application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;

    /**
     * @brief Init the extension.
     *
     * @param record the extension record.
     * @param application the application info.
     * @param handler the extension handler.
     * @param token the remote token.
     */
    virtual void Init(const std::shared_ptr<AbilityLocalRecord> &record,
                      const std::shared_ptr<OHOSApplication> &application, std::shared_ptr<AbilityHandler> &handler,
                      const sptr<IRemoteObject> &token) override;

    /**
     * @brief Create extension.
     *
     * @param runtime The runtime.
     * @return The ObjectEditorExtension instance.
     */
    static ObjectEditorExtension *Create(const std::unique_ptr<Runtime> &runtime);

    /**
     * @brief Called when this extension is started. You must override this function if you want to perform some
     *        initialization operations during extension startup.
     *
     * This function can be called only once in the entire lifecycle of an extension.
     * @param want Indicates the {@link Want} structure containing startup information about the extension.
     */
    virtual void OnStart(const AAFwk::Want &want) override;

    /**
     * @brief Called when this extension is connected for the first time.
     *
     * You can override this function to implement your own processing logic.
     *
     * @param want Indicates the {@link Want} structure containing connection information about the extension.
     *
     * @return Returns a pointer to the <b>sid</b> of the connected extension.
     */
    virtual sptr<IRemoteObject> OnConnect(const AAFwk::Want &want) override;

    /**
     * @brief Called when all abilities connected to this extension are disconnected.
     *
     * You can override this function to implement your own processing logic.
     *
     */
    virtual void OnDisconnect(const AAFwk::Want &want) override;

    /**
     * @brief Called back when Service is started.
     * This method can be called only by Service. You can use the StartAbility(ohos.aafwk.content.Want) method to start
     * Service. Then the system calls back the current method to use the transferred want parameter to execute its own
     * logic.
     *
     * @param want Indicates the want of Service to start.
     * @param restart Indicates the startup mode. The value true indicates that Service is restarted after being
     * destroyed, and the value false indicates a normal startup.
     * @param startId Indicates the number of times the Service extension has been started. The startId is
     * incremented by 1 every time the extension is started. For example, if the extension has been started
     * for six times, the value of startId is 6.
     */
    virtual void OnCommand(const AAFwk::Want &want, bool restart, int startId) override;

    /**
     * @brief Called when this extension enters the <b>STATE_STOP</b> state.
     *
     * The extension in the <b>STATE_STOP</b> is being destroyed.
     * You can override this function to implement your own processing logic.
     */
    virtual void OnStop() override;

    std::shared_ptr<struct ContentEmbed_ExtensionContext> GetCEContext() const;

    ErrCode GetSnapshot(const std::string &documentId) override;

    ErrCode DoEdit(const std::string &documentId) override;

    ErrCode GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified) override;

    ErrCode GetCapability(const std::string &documentId, uint32_t *bitmask) override;

    ErrCode Close(const std::string &documentId, bool &isAllObjectsRemoved) override;

    ErrCode GetExtensionEditStatus(bool &isEditing) override;

    ErrCode Initial(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCb) override;

    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;
protected:
    class ObjectEditorExtensionDisplayListener : public Rosen::DisplayManager::IDisplayListener {
    public:
        explicit ObjectEditorExtensionDisplayListener(const std::weak_ptr<ObjectEditorExtension> &extension)
        {
            objectEditorExtension_ = extension;
        }

        void OnCreate(Rosen::DisplayId displayId) override
        {
            auto objectEditorSptr = objectEditorExtension_.lock();
            if (objectEditorSptr != nullptr) {
                objectEditorSptr->OnCreate(displayId);
            }
        }

        void OnDestroy(Rosen::DisplayId displayId) override
        {
            auto objectEditorSptr = objectEditorExtension_.lock();
            if (objectEditorSptr != nullptr) {
                objectEditorSptr->OnDestroy(displayId);
            }
        }

        void OnChange(Rosen::DisplayId displayId) override
        {
            auto objectEditorSptr = objectEditorExtension_.lock();
            if (objectEditorSptr != nullptr) {
                objectEditorSptr->OnChange(displayId);
            }
        }

    private:
        std::weak_ptr<ObjectEditorExtension> objectEditorExtension_;
    };

    void OnCreate(Rosen::DisplayId displayId);
    void OnDestroy(Rosen::DisplayId displayId);
    void OnChange(Rosen::DisplayId displayId);

private:
    class SystemAbilityStatusChangeListener
        : public OHOS::SystemAbilityStatusChangeStub {
    public:
        SystemAbilityStatusChangeListener(sptr<ObjectEditorExtensionDisplayListener> displayListener)
            : listener_(displayListener){};
        virtual void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
        virtual void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override {}

    private:
        sptr<ObjectEditorExtensionDisplayListener> listener_ = nullptr;
    };

    void GetSrcPath(std::string &srcPath);
    void ListenWindowManager();
    ErrCode CreateObject(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCb);

    std::shared_ptr<AbilityHandler> handler_ = nullptr;
    std::shared_ptr<struct ContentEmbed_ExtensionInstance> ceInstance_ = nullptr;
    std::shared_ptr<struct ContentEmbed_ExtensionContext> ceContext_ = nullptr;
    sptr<ObjectEditorExtensionDisplayListener> displayListener_ = nullptr;
};

} // namespace AbilityRuntime
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_H