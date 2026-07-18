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

#ifndef MOCK_OBJECT_EDITOR_EXTENSION_STUB_H
#define MOCK_OBJECT_EDITOR_EXTENSION_STUB_H

#include <gmock/gmock.h>
#include "object_editor_extension_stub.h"

namespace OHOS {
namespace ObjectEditor {

class MockObjectEditorExtensionStub : public ObjectEditorExtensionStub {
public:
    MockObjectEditorExtensionStub() = default;
    ~MockObjectEditorExtensionStub() = default;
    ErrCode GetSnapshot(const std::string &documentId) override;
    ErrCode DoEdit(const std::string &documentId) override;
    ErrCode GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified) override;
    ErrCode GetExtensionEditStatus(bool &isEditing) override;
    ErrCode GetCapability(const std::string &documentId, uint32_t *bitmask) override;
    ErrCode Close(const std::string &documentId, bool &isAllObjectsRemoved, uint32_t callerTokenId) override;
    ErrCode Initial(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCb, uint32_t callerTokenId) override;
    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;

    ErrCode callbackEnterError = ERR_OK;
    ErrCode error = ERR_OK;
    std::string testcaseName;
};

} // namespace ObjectEditor
} // namespace OHOS
#endif