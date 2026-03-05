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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_PROXY_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_PROXY_H

#include <iremote_proxy.h>
#include "iobject_editor_extension.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorExtensionProxy : public IRemoteProxy<IObjectEditorExtension> {
public:
    explicit ObjectEditorExtensionProxy(
        const sptr<IRemoteObject> &remote)
        : IRemoteProxy<IObjectEditorExtension>(remote)
    {}
    virtual ~ObjectEditorExtensionProxy()
    {}
    ErrCode GetSnapshot(const std::string &documentId) override;
    ErrCode DoEdit(const std::string &documentId) override;
    ErrCode GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified) override;
    ErrCode GetExtensionEditStatus(bool &isEditing) override;
    ErrCode GetCapability(const std::string &documentId, uint32_t *bitmask) override;
    ErrCode Close(const std::string &documentId, bool &isAllObjectsRemoved) override;
    ErrCode Initial(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCallback) override;
private:
    static inline BrokerDelegator<ObjectEditorExtensionProxy> delegator_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_EXTENSION_PROXY_H