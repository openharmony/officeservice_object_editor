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
#include <vector>

#include "parcel.h"
#include "mock_message_parcel.h"
#include "hilog_object_editor.h"
#include "iremote_object.h"
#include "message_parcel.h"

namespace OHOS {
namespace {
bool g_setWriteBoolErrorFlag = false;
bool g_setWriteInt32ErrorFlag = false;
bool g_setWriteInt64ErrorFlag = false;
bool g_setWriteUint32ErrorFlag = false;
bool g_setWriteUint64ErrorFlag = false;
bool g_setWriteFloatErrorFlag = false;
bool g_setWriteString16ErrorFlag = false;
bool g_setWriteStringErrorFlag = false;
bool g_setWriteParcelableErrorFlag = false;
bool g_setWriteInterfaceTokenErrorFlag = false;
bool g_setWriteRemoteObjectErrorFlag = false;
bool g_setReadRemoteObjectErrorFlag = false;
bool g_setReadBoolErrorFlag = false;
bool g_setReadUint32ErrorFlag = false;
bool g_setReadInt32ErrorFlag = false;
bool g_setReadUint64ErrorFlag = false;
bool g_setReadInt64ErrorFlag = false;
bool g_setReadFloatErrorFlag = false;
bool g_setReadStringVectorErrorFlag = false;
bool g_setReadStringErrorFlag = false;
bool g_setReadString16ErrorFlag = false;
std::vector<int32_t> g_int32Cache;
int32_t g_WriteInt32ErrorCount = 0;
int32_t g_WriteBoolErrorCount = 0;
int32_t g_WriteStringErrorCount = 0;
int32_t g_ReadStringErrorCount = 0;
int32_t g_WriteParcelableErrorCount = 0;
int32_t g_WriteUint32ErrorCount = 0;
int32_t g_WriteRemoteObjectErrorCount = 0;
const static std::string ERROR_FLAG = "error";
constexpr int32_t ERROR_INT = -1;
constexpr float ERROR_FLOAT = -1.0f;
constexpr int32_t ERROR_SIZE = 1;
std::u16string remoteDescriptor = u"";
sptr<IRemoteObject> g_remote = nullptr;
}  // namespace

namespace ObjectEditor {
void MockMessageParcel::ClearAllErrorFlag()
{
    g_setWriteBoolErrorFlag = false;
    g_setWriteInt32ErrorFlag = false;
    g_setWriteInt64ErrorFlag = false;
    g_setWriteUint32ErrorFlag = false;
    g_setWriteUint64ErrorFlag = false;
    g_setWriteFloatErrorFlag = false;
    g_setWriteString16ErrorFlag = false;
    g_setWriteStringErrorFlag = false;
    g_setWriteParcelableErrorFlag = false;
    g_setWriteInterfaceTokenErrorFlag = false;
    g_setWriteRemoteObjectErrorFlag = false;
    g_setReadRemoteObjectErrorFlag = false;
    g_setReadBoolErrorFlag = false;
    g_setReadUint32ErrorFlag = false;
    g_setReadInt32ErrorFlag = false;
    g_setReadUint64ErrorFlag = false;
    g_setReadInt64ErrorFlag = false;
    g_setReadFloatErrorFlag = false;
    g_setReadStringVectorErrorFlag = false;
    g_setReadStringErrorFlag = false;
    g_setReadString16ErrorFlag = false;
    g_int32Cache.clear();
    g_WriteInt32ErrorCount = 0;
    g_WriteBoolErrorCount = 0;
    g_WriteStringErrorCount = 0;
    g_ReadStringErrorCount = 0;
    g_WriteParcelableErrorCount = 0;
    g_WriteUint32ErrorCount = 0;
    g_WriteRemoteObjectErrorCount = 0;
}

void MockMessageParcel::SetWriteBoolErrorFlag(bool flag)
{
    g_setWriteBoolErrorFlag = flag;
}

void MockMessageParcel::SetWriteInt32ErrorFlag(bool flag)
{
    g_setWriteInt32ErrorFlag = flag;
}

void MockMessageParcel::SetWriteInt64ErrorFlag(bool flag)
{
    g_setWriteInt64ErrorFlag = flag;
}

void MockMessageParcel::SetWriteUint32ErrorFlag(bool flag)
{
    g_setWriteUint32ErrorFlag = flag;
}

void MockMessageParcel::SetWriteUint64ErrorFlag(bool flag)
{
    g_setWriteUint64ErrorFlag = flag;
}

void MockMessageParcel::SetWriteFloatErrorFlag(bool flag)
{
    g_setWriteFloatErrorFlag = flag;
}

void MockMessageParcel::SetWriteString16ErrorFlag(bool flag)
{
    g_setWriteString16ErrorFlag = flag;
}

void MockMessageParcel::SetWriteStringErrorFlag(bool flag)
{
    g_setWriteStringErrorFlag = flag;
}

void MockMessageParcel::SetWriteParcelableErrorFlag(bool flag)
{
    g_setWriteParcelableErrorFlag = flag;
}

void MockMessageParcel::SetWriteInterfaceTokenErrorFlag(bool flag)
{
    g_setWriteInterfaceTokenErrorFlag = flag;
}

void MockMessageParcel::SetWriteRemoteObjectErrorFlag(bool flag)
{
    g_setWriteRemoteObjectErrorFlag = flag;
}

void MockMessageParcel::SetReadRemoteObjectErrorFlag(bool flag)
{
    g_setReadRemoteObjectErrorFlag = flag;
}

void MockMessageParcel::SetReadBoolErrorFlag(bool flag)
{
    g_setReadBoolErrorFlag = flag;
}

void MockMessageParcel::SetReadUint32ErrorFlag(bool flag)
{
    g_setReadUint32ErrorFlag = flag;
}

void MockMessageParcel::SetReadInt32ErrorFlag(bool flag)
{
    g_setReadInt32ErrorFlag = flag;
}

void MockMessageParcel::SetReadUint64ErrorFlag(bool flag)
{
    g_setReadUint64ErrorFlag = flag;
}

void MockMessageParcel::SetReadInt64ErrorFlag(bool flag)
{
    g_setReadInt64ErrorFlag = flag;
}

void MockMessageParcel::SetReadFloatErrorFlag(bool flag)
{
    g_setReadFloatErrorFlag = flag;
}

void MockMessageParcel::SetReadStringVectorErrorFlag(bool flag)
{
    g_setReadStringVectorErrorFlag = flag;
}

void MockMessageParcel::SetReadStringErrorFlag(bool flag)
{
    g_setReadStringErrorFlag = flag;
}

void MockMessageParcel::AddInt32Cache(int32_t value)
{
    g_int32Cache.emplace_back(value);
}

void MockMessageParcel::SetWriteInt32ErrorCount(int count)
{
    g_WriteInt32ErrorCount = count;
}

void MockMessageParcel::SetWriteBoolErrorCount(int count)
{
    g_WriteBoolErrorCount = count;
}

void MockMessageParcel::SetWriteParcelableErrorCount(int count)
{
    g_WriteParcelableErrorCount = count;
}

void MockMessageParcel::SetWriteUint32ErrorCount(int count)
{
    g_WriteUint32ErrorCount = count;
}

void MockMessageParcel::SetWriteStringErrorCount(int32_t count)
{
    g_WriteStringErrorCount = count;
}

void MockMessageParcel::SetReadStringErrorCount(int32_t count)
{
    g_ReadStringErrorCount = count;
}

void MockMessageParcel::SetReadString16ErrorFlag(bool flag)
{
    g_setReadString16ErrorFlag = flag;
}

void MockMessageParcel::SetWriteRemoteObjectErrorCount(int32_t count)
{
    g_WriteRemoteObjectErrorCount = count;
}
}

bool MessageParcel::WriteInterfaceToken(std::u16string name)
{
    OBJECT_EDITOR_LOGI(ObjectEditor::ObjectEditorDomain::EXTENSION, "WriteInterfaceToken");
    if (g_setWriteInterfaceTokenErrorFlag) {
        return false;
    }
    remoteDescriptor = name;
    return true;
}

std::u16string MessageParcel::ReadInterfaceToken()
{
    OBJECT_EDITOR_LOGI(ObjectEditor::ObjectEditorDomain::EXTENSION, "ReadInterfaceToken");
    if (g_setWriteInterfaceTokenErrorFlag) {
        return u"";
    }
    return remoteDescriptor;
}

bool MessageParcel::WriteRemoteObject(const sptr<IRemoteObject> &object)
{
    if (g_setWriteRemoteObjectErrorFlag) {
        return false;
    }
    g_remote = object;
    return true;
}

sptr<IRemoteObject> MessageParcel::ReadRemoteObject()
{
    if (g_setReadRemoteObjectErrorFlag) {
        return nullptr;
    }
    return g_remote;
}

bool Parcel::WriteParcelable(const Parcelable *object)
{
    (void)object;
    if (g_setWriteParcelableErrorFlag && g_WriteParcelableErrorCount < 1) {
        return false;
    }
    if (g_WriteParcelableErrorCount > 0) {
        g_WriteParcelableErrorCount--;
    }
    return true;
}

bool Parcel::WriteBool(bool value)
{
    (void)value;
    if (g_setWriteBoolErrorFlag && g_WriteBoolErrorCount < 1) {
        return false;
    }
    if (g_WriteBoolErrorCount > 0) {
        g_WriteBoolErrorCount--;
    }
    return true;
}

bool Parcel::WriteInt32(int32_t value)
{
    (void)value;
    if ((g_setWriteInt32ErrorFlag || value == ERROR_INT) && g_WriteInt32ErrorCount < 1) {
        return false;
    }

    if (g_WriteInt32ErrorCount > 0) {
        g_WriteInt32ErrorCount--;
    }
    return true;
}

bool Parcel::WriteInt64(int64_t value)
{
    (void)value;
    if (g_setWriteInt64ErrorFlag || value == ERROR_INT) {
        return false;
    }
    return true;
}

bool Parcel::WriteUint32(uint32_t value)
{
    (void)value;
    if (g_setWriteUint32ErrorFlag && g_WriteUint32ErrorCount < 1) {
        return false;
    }
    if (g_WriteUint32ErrorCount > 0) {
        g_WriteUint32ErrorCount--;
    }
    return true;
}

bool Parcel::WriteUint64(uint64_t value)
{
    (void)value;
    if (g_setWriteUint64ErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::WriteFloat(float value)
{
    (void)value;
    if (g_setWriteFloatErrorFlag || value == ERROR_FLOAT) {
        return false;
    }
    return true;
}

bool Parcel::WriteString16(const std::u16string &value)
{
    (void)value;
    return !g_setWriteString16ErrorFlag;
}

bool Parcel::WriteString(const std::string &value)
{
    (void)value;
    if (g_setWriteStringErrorFlag && g_WriteStringErrorCount < 1) {
        return false;
    }
    if (g_WriteStringErrorCount > 0) {
        g_WriteStringErrorCount--;
    }
    return true;
}

bool Parcel::ReadBool(bool &val)
{
    if (g_setReadBoolErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::ReadUint32(uint32_t &value)
{
    if (g_setReadUint32ErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::ReadInt32(int32_t &value)
{
    if (g_setReadInt32ErrorFlag && g_int32Cache.size() == 0) {
        return false;
    }
    if (g_int32Cache.size() > 0) {
        value = g_int32Cache[0];
        g_int32Cache.erase(g_int32Cache.begin());
    }
    return true;
}

int32_t Parcel::ReadInt32()
{
    int32_t value = 0;
    if (g_setReadInt32ErrorFlag && g_int32Cache.size() == 0) {
        return value;
    }

    if (g_int32Cache.size() > 0) {
        value = g_int32Cache[0];
        g_int32Cache.erase(g_int32Cache.begin());
    }
    return value;
}

bool Parcel::ReadUint64(uint64_t &value)
{
    if (g_setReadUint64ErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::ReadInt64(int64_t &value)
{
    if (g_setReadInt64ErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::ReadFloat(float &value)
{
    if (g_setReadFloatErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::WriteStringVector(const std::vector<std::string> &val)
{
    (void)val;
    if (val.size() == ERROR_SIZE) {
        return false;
    }
    return true;
}

bool Parcel::ReadStringVector(std::vector<std::string> *val)
{
    if (g_setReadStringVectorErrorFlag) {
        return false;
    }
    return true;
}

bool Parcel::ReadString(std::string &val)
{
    (void)val;
    if (g_setReadStringErrorFlag && g_ReadStringErrorCount < 1) {
        return false;
    }
    if (g_ReadStringErrorCount > 0) {
        g_ReadStringErrorCount--;
    }
    return true;
}

const std::u16string Parcel::ReadString16()
{
    return u"";
}
}  // namespace OHOS
