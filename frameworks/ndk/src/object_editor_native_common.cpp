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

#include <map>

#include "object_editor_native_common.h"
#include "object_editor_err_code.h"

namespace OHOS {
namespace ObjectEditor {
const std::map<int32_t, ContentEmbed_ErrorCode> ERROR_CODE_MAP {
// ObjectEditorClientErrCode
    {ObjectEditorClientErrCode::CLIENT_OK, CE_ERR_OK},
    {ObjectEditorClientErrCode::CLIENT_INVALID_PARAMETER, CE_ERR_PARAM_INVALID},
    {ObjectEditorClientErrCode::CLIENT_UNKNOWN_OPERATE, CE_ERR_PARAM_INVALID},
    {ObjectEditorClientErrCode::CLIENT_GET_PATH_ERROR, CE_ERR_PARAM_INVALID},
    {ObjectEditorClientErrCode::CLIENT_PREPARE_FILES_ERROR, CE_ERR_PARAM_INVALID},
    {ObjectEditorClientErrCode::CLIENT_MEMORY_ALLOCATION_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorClientErrCode::CLIENT_COPY_FILE_FAILED, CE_ERR_DISK_FULL},
// ObjectEditorManagerErrCode
    {ObjectEditorManagerErrCode::SA_OK, CE_ERR_OK},
    {ObjectEditorManagerErrCode::SA_PERMISSION_DENIED, CE_PERMISSION_DENIED},
    {ObjectEditorManagerErrCode::SA_UNKNOWN_OPERATE, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_INVALID_PARAMETER, CE_ERR_PARAM_INVALID},
    {ObjectEditorManagerErrCode::SA_GRANT_PERMISSION_TO_SERVER_EXTENSION_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_PARCEL_ERR, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_REMOTE_ERR, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_ALLOC_FAIL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_ERR, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_QUERY_FAIL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_QUERY_EMPTY, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_INSERT_FAIL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_DELETE_FAIL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DB_PARSE_FAIL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DIVERSION_QUERY_EMPTY, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_UTD_QUERY_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_BMS_QUERY_DEFAULT_LAUNCHER_APP_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_FILE_EXT_NOT_SET_DEFAULT_LAUNCHER_APP, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_SUCCEED, CE_ERR_OK},
    {ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CONNECT_ABILITY_TIMEOUT, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CONNECT_PROXY_IS_NULL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CONNECT_EXTENSION_PROXY_IS_NULL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_MEMORY_ALLOCATION_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_EXTENSION_PARAM_INVALID, CE_ERR_PARAM_INVALID},
    {ObjectEditorManagerErrCode::SA_NOT_FOUND_ABILITY_BY_REMOTE_OBJECT, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_NOT_FOUND_CLIENT_REMOTE_OBJECT_BY_SERVER_REMOTE_OBJECT, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_ABILITY_CONNECTION_IS_NULL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_UNLOAD_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_TIMEOUT, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_SUCCEED, CE_ERR_OK},
    {ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_WRITE_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_SEND_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_EXTENSION_REMOTE_STUB_IS_NULL, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CHECK_START_EXTENSION_OK, CE_ERR_OK},
    {ObjectEditorManagerErrCode::SA_ERR_DEVICE_NOT_SUPPORT, CE_ERR_DEVICE_NOT_SUPPORTED},
    {ObjectEditorManagerErrCode::SA_START_UIABILITY_FAILED, CE_ERR_SYSTEM_ABNORMAL},
    {ObjectEditorManagerErrCode::SA_CONNECT_LIMIT_EXCEED, CE_ERR_CONNECT_LIMIT_EXCEED},
    {ObjectEditorManagerErrCode::SA_CHECK_CLIENT_FILE_VALID_FAILED, CE_ERR_FILE_NOT_GRANT},
// ObjectorEditorExtensionErrCode
    {ObjectorEditorExtensionErrCode::EXTENSION_CAPABILITY_NOT_SUPPORT, CE_ERR_EXTENSION_NOT_SUPPORT},
    {ObjectorEditorExtensionErrCode::EXTENSION_NULL_POINTER, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_PARAM_INVALID, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_DOCUMENT_NOT_FOUND, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_CALLBACK_NULL, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_DOCUMENT_ALREADY_INITIAL, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_MEMORY_ALLOCATION_FAILED, CE_ERR_EXTENSION_ERROR},
    {ObjectorEditorExtensionErrCode::EXTENSION_UNKNOWN_ERROR, CE_ERR_EXTENSION_ERROR},
// ObjectEditorDocumentErrCode
    {ObjectEditorDocumentErrCode::DOCUMENT_FLUSH_ERROR, CE_ERR_FILE_OPERATION_FAILED},
};

ContentEmbed_ErrorCode ConvertErrorToCode(int32_t code, ContentEmbed_ErrorCode defaultValue)
{
    auto iter = ERROR_CODE_MAP.find(code);
    if (iter != ERROR_CODE_MAP.end()) {
        return iter->second;
    }
    return defaultValue;
}
} // OHOS
} // ObjectEditor