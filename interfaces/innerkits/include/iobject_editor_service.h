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

#ifndef OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_SERVICE_H
#define OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_SERVICE_H

#include <cstdint>
#include <string_ex.h>
#include <iremote_object.h>
#include "iobject_editor_client_callback.h"
#include "object_editor_document.h"

namespace OHOS {
namespace ObjectEditor {
using namespace OHOS::ObjectEditor;

enum class IObjectEditorServiceIpcCode {
    COMMAND_GET_SNAPSHOT = MIN_TRANSACTION_ID,
    COMMAND_DO_EDIT,
    COMMAND_GET_EDITING_STATE,
    COMMAND_GET_EXTENSION_EDITING_STATUS,
    COMMAND_GET_CAPABILITY,
    COMMAND_CLOSE,
    COMMAND_INITIAL,
};

class IObjectEditorService : public virtual RefBase {
public:
    virtual ErrCode GetSnapshot(const std::string &documentId) = 0;

    virtual ErrCode DoEdit(const std::string &documentId) = 0;

    virtual ErrCode GetEditStatus(const std::string &documentId, bool *isEditing, bool *isModified) = 0;

    virtual ErrCode GetExtensionEditStatus(bool &isEditing) = 0;

    virtual ErrCode GetCapability(const std::string &documentId, uint32_t *bitmask) = 0;

    virtual ErrCode Close(const std::string &documentId, bool &isAllObjectsRemoved) = 0;

    virtual ErrCode Initial(std::unique_ptr<ObjectEditorDocument> document,
        const sptr<IObjectEditorClientCallback> &clientCb) = 0;

    sptr<IRemoteObject> GetRemoteObject() const
    {
        return remoteObject_;
    }

    void SetRemoteObject(const sptr<IRemoteObject> &remoteObject)
    {
        remoteObject_ = remoteObject;
    }
private:
    sptr<IRemoteObject> remoteObject_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_SERVICE_H
