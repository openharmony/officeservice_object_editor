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

#include "object_editor_config.h"
#include "object_editor_package.h"
#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
ObjectEditorPackage::ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "constructor.");
}

ObjectEditorPackage::~ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "destructor.");
}

ErrCode ObjectEditorPackage::RegisterClientCB(const sptr<ObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "in");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetSnapshot()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (!OHOS::ObjectEditor::ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "device not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackage::DoEdit()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetEditStatus(bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (!OHOS::ObjectEditor::ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "device not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetCapability(uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (!OHOS::ObjectEditor::ObjectEditorConfig::GetInstance().IsSupportObjectEditor()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::CLIENT_NDK, "device not supported");
        return CE_ERR_DEVICE_NOT_SUPPORTED;
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Close()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Initial(std::unique_ptr<ObjectEditorDocument> document)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

int32_t ObjectEditorPackage::CallbackEnter([[maybe_unused]] uint32_t code)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

int32_t ObjectEditorPackage::CallbackExit([[maybe_unused]] uint32_t code, [[maybe_unused]] int32_t result)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
