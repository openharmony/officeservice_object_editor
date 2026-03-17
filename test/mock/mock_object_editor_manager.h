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

#ifndef MOCK_OBJECT_EDITOR_MANAGER_H
#define MOCK_OBJECT_EDITOR_MANAGER_H

#include <gmock/gmock.h>
#include "iobject_editor_manager.h"

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorManager : public IObjectEditorManager {
public:
    MOCK_METHOD(ErrCode, StartObjectEditorExtension, (std::unique_ptr<ObjectEditorDocument> &document,
        const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject,
        bool &isPackageExtension), (override));
    MOCK_METHOD(ErrCode, StopObjectEditorExtension, (const std::string &documentId,
        const sptr<IRemoteObject> &oeExtensionRemoteObject,
        const bool &isPackageExtension), (override));
    MOCK_METHOD(ErrCode, GetHmidByFileExtension, (const std::string &hmid, std::string &fileExtension), (override));
    MOCK_METHOD(ErrCode, GetIconByHmid, (const std::string &hmid, std::string &resourceId), (override));
    MOCK_METHOD(ErrCode, GetFormatName, (const std::string &hmid,
        const std::string &locale, std::string &InterName), (override));
    MOCK_METHOD(ErrCode, GetObjectEditorFormatByHmidAndLocale, (const std::string &hmid, const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format), (override));
    MOCK_METHOD(ErrCode, GetObjectEditorFormatsByLocale, (const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats), (override));
    MOCK_METHOD(ErrCode, StartUIAbility, (const std::unique_ptr<AAFwk::Want> &want), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // MOCK_OBJECT_EDITOR_MANAGER_H