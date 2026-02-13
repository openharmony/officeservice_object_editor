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

#ifndef MOCK_OBJECT_EDITOR_CLIENT_CALLBACK_H
#define MOCK_OBJECT_EDITOR_CLIENT_CALLBACK_H

#include <gmock/gmock.h>
#include "iobject_editor_client_callback.h"

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorClientCallback : public IObjectEditorClientCallback {
public:
    MOCK_METHOD(ErrCode, OnUpdate, (std::unique_ptr<ObjectEditorDocument>& document), (override));
    MOCK_METHOD(ErrCode, OnError, (ContentEmbed_ErrorCode error), (override));
    MOCK_METHOD(ErrCode, OnStopEdit, (bool dataModified), (override));
    MOCK_METHOD(ErrCode, OnExtensionStoped, (), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

} // namespace ObjectEditor
} // namespace OHOS
#endif  // MOCK_OBJECT_EDITOR_CLIENT_CALLBACK_H