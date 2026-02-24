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

#include "object_editor_package_stub.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
int32_t ObjectEditorPackageStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    int32_t ret = CallbackEnter(code);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "CallbackEnter ret:%{public}d", ret);
        return ret;
    }
    ret = OnRemoteRequestInner(code, data, reply, option);
    ret = CallbackExit(code, ret);
    if (ret != ERR_NONE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "CallbackExit ret:%{public}d", ret);
        return ret;
    }
    return ret;
}

int32_t ObjectEditorPackageStub::OnRemoteRequestInner(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                      MessageOption &option)
{
    std::u16string localDescriptor = GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (localDescriptor != remoteDescriptor) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "descriptor invalid");
        return ERR_TRANSACTION_FAILED;
    }
    switch (static_cast<IObjectEditorServiceIpcCode>(code)) {
        case IObjectEditorServiceIpcCode::COMMAND_REGISTER_CLIENT_CB:
            return HandlePackageRegisterClientCallback(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_GET_SNAPSHOT:
            return HandlePackageGetSnapshot(data, reply);
        case IObjectEditorServiceIpcCode::COMMAND_DO_EDIT:
            return HandlePackageDoEdit(data, reply);
        default:
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return ERR_TRANSACTION_FAILED;
}

int32_t ObjectEditorPackageStub::HandlePackageRegisterClientCallback(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::PACKAGE, "call");
    sptr<IObjectEditorClientCallback> clientCb = iface_cast<IObjectEditorClientCallback>(data.ReadRemoteObject());
    if (clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "clientCb is nullptr");
        return ERR_INVALID_DATA;
    }
    ErrCode errCode = RegisterClientCB(clientCb);
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorPackageStub::HandlePackageGetSnapshot(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::PACKAGE, "call");
    ErrCode errCode = GetSnapshot();
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}

int32_t ObjectEditorPackageStub::HandlePackageDoEdit(MessageParcel &data, MessageParcel &reply)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::PACKAGE, "call");
    ErrCode errCode = DoEdit();
    if (!reply.WriteInt32(errCode)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write errCode failed");
        return ERR_INVALID_VALUE;
    }
    return ERR_NONE;
}
// LCOV_EXCL_END
} // namespace ObjectEditor
} // namespace OHOS
