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
#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_H

#include "object_editor_package_stub.h"
#include <cstdint>

namespace OHOS {
namespace ObjectEditor {
class ObjectEditorPackage : public ObjectEditorPackageStub {
public:
    ObjectEditorPackage();
    ~ObjectEditorPackage();

    ErrCode RegisterClientCB(const sptr<IObjectEditorPackageCallback> &clientCb) override;

    ErrCode GetSnapshot() override;

    ErrCode DoEdit() override;

    ErrCode GetEditStatus(bool isEditing, bool *isModified) override;

    ErrCode GetCapability(uint32_t &bitmask) override;

    ErrCode Close() override;

    ErrCode Initial(std::unique_ptr<IObjectEditorDocument> document) override;

    int32_t CallbackEnter([[maybe_unused]] uint32_t code) override;
    int32_t CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result) override;
};

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_PACKAGE_H