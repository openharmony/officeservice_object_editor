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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_ERR_CODE_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_ERR_CODE_H

#include <cstdint>

namespace OHOS {
namespace ObjectEditor {
enum ObjectEditorClientErrCode : int32_t {
    CLIENT_OK = 0,
    CLIENT_INVALID_PARAMETER,
    CLIENT_UNKNOWN_OPERATE,
    CLIENT_GET_PATH_ERROR,
    CLIENT_PREPARE_FILES_ERROR,
    CLIENT_MEMORY_ALLOCATION_FAILED,
    CLIENT_COPY_FILE_FAILED,
};

enum ObjectEditorManagerErrCode : int32_t {
    SA_OK = 0,
    SA_PERMISSION_DENIED,
    SA_UNKNOWN_OPERATE,
    SA_INVALID_PARAMETER,
    SA_GRANT_PERMISSION_TO_SERVER_EXTENSION_FAILED,
    SA_PARCEL_ERR,
    SA_REMOTE_ERR,
    SA_ALLOC_FAIL,
    SA_DB_ERR,
    SA_DB_QUERY_FAIL,
    SA_DB_QUERY_EMPTY,
    SA_DB_INSERT_FAIL,
    SA_DB_DELETE_FAIL,
    SA_DB_PARSE_FAIL,
    SA_DIVERSION_QUERY_EMPTY,
    SA_UTD_QUERY_FAILED,
    SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED,
    SA_FILE_EXT_NOT_SET_DEFAULT_LAUNCHER_APP,

    SA_CONNECT_ABILITY_SUCCEED,
    SA_CONNECT_ABILITY_FAILED,
    SA_CONNECT_ABILITY_TIMEOUT,
    SA_CONNECT_PROXY_IS_NULL,
    SA_CONNECT_EXTENSION_PROXY_IS_NULL,
    SA_MEMORY_ALLOCATION_FAILED,
    SA_EXTENSION_PARAM_INVALID,
    SA_NOT_FOUND_ABILITY_BY_REMOTE_OBJECT,
    SA_NOT_FOUND_CLIENT_REMOTE_OBJECT_BY_SERVER_REMOTE_OBJECT,
    SA_ABILITY_CONNECTION_IS_NULL,
    SA_DISCONNECT_ABILITY_FAILED,
    SA_UNLOAD_FAILED,
    SA_DISCONNECT_ABILITY_TIMEOUT,
    SA_DISCONNECT_ABILITY_SUCCEED,
    SA_EXTENSION_REMOTE_WRITE_FAILED,
    SA_EXTENSION_REMOTE_SEND_FAILED,
    SA_EXTENSION_REMOTE_STUB_IS_NULL,
    SA_CHECK_START_EXTENSION_OK,
    SA_ERR_DEVICE_NOT_SUPPORT,
    SA_START_UIABILITY_FAILED,
};

enum ObjectorEditorExtensionErrCode : int32_t {
    EXTENSION_OK = 0,
};

enum ObjectEditorDocumentErrCode : int32_t {
    DOCUMENT_OK = 0,
    DOCUMENT_FLUSH_ERROR,
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_MANAGER_ERR_CODE_H