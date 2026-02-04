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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_PROXY_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_PROXY_H

#include "iremote_proxy.h"
#include "iobject_editor_manager.h"
#include "iobject_editor_client_callback.h"

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorManagerProxy : public IRemoteProxy<IObjectEditorManager> {
public:
    explicit ObjectEditorManagerProxy(const sptr<IRemoteObject> &remote):IRemoteProxy<IObjectEditorManager>(remote){}
    virtual~ObjectEditorManagerProxy(){};
    ErrCode StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &callback, sptr<IRemoteObject> &objectEditorProxy,bool &isPackageExtension) override;
    ErrCode StopObjectEditorExtension(const sptr<IObjectEditorService> &oeExtensionRemoteObject,
        const bool &isPackageExtension) override;
    ErrCode GetHmidByFileExtension(const std::string &hmid, std::string &fileExtension) override;
    ErrCode GetIconByHmid(const std::string &hmid, std::string &resourceId) override;
    ErrCode GetFormatName(const std::string &hmid, const std::string &locale, std::string &formatName) override;
    ErrCode GetObjectEditorFormatByHmidAndLocale(const std::string &hmid, const std::string &locale,
        ObjectEditorFormat &format) override;
    ErrCode GetObjectEditorFormatsByLocale(const std::string &locale,
        std::vector<ObjectEditorFormat> &formats) override;
    ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want) override;
private:
    static inline BrokerDelegator<ObjectEditorManagerProxy> delegator_;
    const int32_t MAX_READ_COUNT = INT32_MAX;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_PROXY_H
