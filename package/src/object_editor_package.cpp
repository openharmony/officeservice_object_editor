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

#include <filesystem>
#include <fstream>

#include "want.h"
#include "ability_manager_client.h"

#include "hilog_object_editor.h"
#include "object_editor_config.h"
#include "object_editor_package.h"
#include "user_mgr.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
const std::string WANT_ACTION = "ohos.want.action.viewData";
const std::string SHOW_DEFAULT_PICKER = "ability.params.showDefaultPicker";
constexpr int32_t ILLEGAL_REQUEST_CODE = -1;
}
// LCOV_EXCL_START
namespace fs = std::filesystem;
ObjectEditorPackage::ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "constructor");
}

ObjectEditorPackage::~ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "destructor");
    if (watcher_ != nullptr) {
        watcher_->Stop();
    }
}

ErrCode ObjectEditorPackage::GetSnapshot(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    std::unique_ptr<ObjectEditorDocument> newDocument = nullptr;
    if (document_->GetOperateType() == OperateType::CREATE_BY_FILE) {
        newDocument = ObjectEditorDocument::CreateByFile(document_->GetOriFilePath());
        if (newDocument == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create document by file failed");
            return ERR_INVALID_VALUE;
        }
        newDocument->SetHmid(PACKAGE_HMID);
        if (document_->GetNativeFileUri().has_value()) {
            newDocument->SetNativeFileUri(document_->GetNativeFileUri().value());
        }
    } else if (document_->GetOperateType() == OperateType::EDIT) {
        newDocument = ObjectEditorDocument::CreateByHmid(PACKAGE_HMID);
        if (newDocument == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create document by hmid failed");
            return ERR_INVALID_VALUE;
        }
        newDocument->SetOperateType(OperateType::EDIT);
    }
    newDocument->SetDocumentId(document_->GetDocumentId());
    if (document_->GetTmpFileUri().has_value()) {
        newDocument->SetTmpFileUri(document_->GetTmpFileUri().value());
    }
    newDocument->SetSnapshotUri(document_->GetSnapshotUri());
    if (!isEditing_ && clientCb_ != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package update document");
        clientCb_->OnUpdate(newDocument);
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackage::DoEdit(const std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    if (packageData_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "packageData is null");
        return ERR_INVALID_VALUE;
    }
    watcher_ = std::make_shared<FileWatcher>(packageData_->GetFilePath(),
        [this](uint32_t mask, const std::string &filepath) {
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "file mask:%{public}d, path:%{private}s",
                mask, filepath.c_str());
            packageData_->SaveData();
            if (clientCb_ != nullptr) {
                clientCb_->OnStopEdit(true);
            }
        });
    if (watcher_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "watcher is null");
        return ERR_INVALID_VALUE;
    }
    if (!watcher_->Start()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "watcher start failed");
        return ERR_INVALID_VALUE;
    }
    AAFwk::Want want;
    want.SetAction(WANT_ACTION);
    want.SetUri(SystemUtils::GetUriFromPath(packageData_->GetFilePath()));
    int32_t flags = AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION | AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION;
    want.SetFlags(flags);
    want.SetParam(SHOW_DEFAULT_PICKER, true);
    auto callerToken = GetRemoteObject();
    if (callerToken == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "callerToken is null");
    }
    auto abilityManagerClient = AAFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "abilityManagerClient is null");
        return ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED;
    }
    auto ret = abilityManagerClient->StartAbility(want, callerToken, ILLEGAL_REQUEST_CODE,
        UserMgr::GetInstance().GetUserId());
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package StartAbility result: %{public}d", ret);
    if (ret == ERR_OK) {
        isEditing_ = true;
    }
    return ret;
}

ErrCode ObjectEditorPackage::GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    *isEditing = isEditing_;
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetExtensionEditStatus(bool &isEditing)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    isEditing = isEditing_;
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetCapability(const std::string &documentId, uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Close(const std::string &documentId, bool &isAllObjectsRemoved)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Initial(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document == nullptr || clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document or clientCb is null");
        return ERR_INVALID_VALUE;
    }
    document->RestoreStorage();
    document->FlushHmid();
    document_ = std::move(document);
    clientCb_ = clientCb;
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    if (document_->GetOperateType() == OperateType::CREATE_BY_FILE) {
        packageData_ = PackageData::CreateByDocument(document_);
    } else {
        packageData_ = PackageData::LoadFromDocument(document_);
    }
    if (packageData_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "packageData is null");
        return ERR_INVALID_VALUE;
    }
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
