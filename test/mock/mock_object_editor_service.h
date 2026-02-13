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

#ifndef MOCK_OBJECT_EDITOR_SERVICE_H
#define MOCK_OBJECT_EDITOR_SERVICE_H

#include <gmock/gmock.h>
#include "iobject_editor_service.h"

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorService : public IObjectEditorService {
public:
    MOCK_METHOD(ErrCode, RegisterClientCB, (
        const sptr<IObjectEditorClientCallback> &clientCb), (override));
    MOCK_METHOD(ErrCode, GetSnapshot, (), (override));
    MOCK_METHOD(ErrCode, DoEdit, (), (override));
    MOCK_METHOD(ErrCode, GetEditStatus, (bool *isEditing, bool *isModified), (override));
    MOCK_METHOD(ErrCode, GetCapability, (uint32_t *bitmask), (override));
    MOCK_METHOD(ErrCode, Close, (), (override));
    MOCK_METHOD(ErrCode, Initial, (std::unique_ptr<ObjectEditorDocument> document), (override));
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // MOCK_OBJECT_EDITOR_SERVICE_H
