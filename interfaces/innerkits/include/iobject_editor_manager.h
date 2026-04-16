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

#ifndef OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_MANAGER_H
#define OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_MANAGER_H

#include <cstdint>
#include <iremote_broker.h>
#include <string_ex.h>
#include <vector>
#include "object_editor_document.h"
#include "object_editor_format.h"
#include "iobject_editor_client_callback.h"
#include "want.h"

namespace OHOS {
namespace ObjectEditor {
constexpr int OBJECT_EDITOR_SERVICE_SA_ID = 66528;
constexpr int32_t LOAD_TIMEOUT_MS = 5000;
constexpr int32_t SA_EXIT_TIME_S = 600;

enum class IObjectEditorManagerIpcCode {
    COMMAND_START_OBJECT_EDITOR_EXTENSION = MIN_TRANSACTION_ID,
    COMMAND_GET_OEID_BY_FILE_EXTENSION,
    COMMAND_GET_ICON_BY_OEID,
    COMMAND_GET_FORMAT_NAME,
    COMMAND_GET_FORMAT_BY_OEID_AND_LOCALE,
    COMMAND_GET_FORMATS_BY_LOCALE,
    COMMAND_STOP_OBJECT_EDITOR_EXTENSION,
    COMMAND_START_UI_ABILITY,
};

class IObjectEditorManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.ObjectEditor.IObjectEditorManager");

    virtual ErrCode StartObjectEditorExtension(std::unique_ptr<ObjectEditorDocument> &document,
                                               const sptr<IObjectEditorClientCallback> &objectEditorClientCallback,
                                               sptr<IRemoteObject> &oeExtensionRemoteObject,
                                               bool &isPackageExtension) = 0;

    virtual ErrCode StopObjectEditorExtension(const std::string &documentId,
                                              const sptr<IRemoteObject> &oeExtensionRemoteObject,
                                              const bool &isPackageExtension) = 0;

    virtual ErrCode GetOEidByFileExtension(const std::string &oeid, std::string &fileExtension) = 0;

    virtual ErrCode GetIconByOEid(const std::string &oeid, std::string &resourceId) = 0;

    virtual ErrCode GetFormatName(const std::string &oeid, const std::string &locale, std::string &InterName) = 0;

    virtual ErrCode GetObjectEditorFormatByOEidAndLocale(const std::string &oeid, const std::string &locale,
                                                         std::unique_ptr<ObjectEditorFormat> &format) = 0;

    virtual ErrCode GetObjectEditorFormatsByLocale(const std::string &locale,
                                                   std::vector<std::unique_ptr<ObjectEditorFormat>> &formats) = 0;

    virtual ErrCode StartUIAbility(const std::unique_ptr<AAFwk::Want> &want,
        sptr<IRemoteObject> extensionToken, int32_t clientPid) = 0;

protected:
    const int VECTOR_MAX_SIZE = 102400;
    const int LIST_MAX_SIZE = 102400;
    const int SET_MAX_SIZE = 102400;
    const int MAP_MAX_SIZE = 102400;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_IOBJECT_EDITOR_MANAGER_H
