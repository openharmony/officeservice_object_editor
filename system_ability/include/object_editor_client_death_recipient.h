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
#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_DEATH_RECIPIENT_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_DEATH_RECIPIENT_H

#include "iremote_object.h"

#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorClientDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    ObjectEditorClientDeathRecipient(const std::string &documentId,
        const sptr<IRemoteObject> &oeExtensionRemoteObject) : documentId_(documentId),
        oeExtensionRemoteObject_(oeExtensionRemoteObject) {}
    virtual ~ObjectEditorClientDeathRecipient() = default;
    void OnRemoteDied(const wptr<IRemoteObject> &remote) override;
private:
    std::string documentId_;
    sptr<IRemoteObject> oeExtensionRemoteObject_ = nullptr;
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CLIENT_DEATH_RECIPIENT_H