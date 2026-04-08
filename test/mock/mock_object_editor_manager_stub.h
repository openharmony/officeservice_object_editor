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

#ifndef MOCK_OBJECT_EDITOR_MANAGER_STUB_H
#define MOCK_OBJECT_EDITOR_MANAGER_STUB_H

#include <gmock/gmock.h>
#include "object_editor_manager_stub.h"

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorManagerStub : public ObjectEditorManagerStub {
public:
    MOCK_METHOD(int32_t, CallbackEnter, (uint32_t code), (override));
    MOCK_METHOD(int32_t, CallbackExit, (uint32_t code, int32_t result), (override));
    MOCK_METHOD(ErrCode, StartObjectEditorExtension, (std::unique_ptr<ObjectEditor::ObjectEditorDocument> &document,
        const sptr<ObjectEditor::IObjectEditorClientCallback> &objectEditorClientCallback,
        sptr<IRemoteObject> &oeExtensionRemoteObject,
        bool &isPackageExtension), (override));
    MOCK_METHOD(ErrCode, StopObjectEditorExtension, (const sptr<IRemoteObject> &oeExtensionRemoteObject,
        std::unique_ptr<ObjectEditorDocument> &document,
        const bool &isPackageExtension), (override));
    MOCK_METHOD(ErrCode, GetOEidByFileExtension, (const std::string &oeid, std::string &fileExtension), (override));
    MOCK_METHOD(ErrCode, GetIconByOEid, (const std::string &oeid, std::string &resourceId), (override));
    MOCK_METHOD(ErrCode, GetFormatName, (const std::string &oeid,
        const std::string &locale, std::string &InterName), (override));
    MOCK_METHOD(ErrCode, GetObjectEditorFormatByOEidAndLocale, (const std::string &oeid, const std::string &locale,
        std::unique_ptr<ObjectEditorFormat> &format), (override));
    MOCK_METHOD(ErrCode, GetObjectEditorFormatsByLocale, (const std::string &locale,
        std::vector<std::unique_ptr<ObjectEditorFormat>> &formats), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

}
}
#endif