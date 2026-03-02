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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_PROXY_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_PROXY_H

#include <iremote_proxy.h>
#include "iobject_editor_package.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorPackageProxy : public IRemoteProxy<IObjectEditorPackage> {
public:
    explicit ObjectEditorPackageProxy(
        const sptr<IRemoteObject> &remote)
         : IRemoteProxy<IObjectEditorPackage>(remote)
    {}
    virtual ~ObjectEditorPackageProxy()
    {}
    ErrCode RegisterClientCB(const sptr<IObjectEditorClientCallback> &callback) override;
    ErrCode GetSnapshot() override;
    ErrCode DoEdit() override;
    ErrCode GetEditStatus(bool *isEditing, bool *isModified) override;
    ErrCode GetCapability(uint32_t *capability) override;
    ErrCode Close() override;
    ErrCode Initial(std::unique_ptr<ObjectEditorDocument> document) override;
private:
    static inline BrokerDelegator<ObjectEditorPackageProxy> delegator_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_PROXY_H
