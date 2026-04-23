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

#ifndef UTILS_H
#define UTILS_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>

#include "hilog_object_editor.h"
#include "object_editor_err_code.h"
#include "types.h"

namespace OHOS {
namespace ObjectEditor {

constexpr size_t BYTE_INDEX_0 = 0;
constexpr size_t BYTE_INDEX_1 = 1;
constexpr size_t BYTE_INDEX_2 = 2;
constexpr size_t BYTE_INDEX_3 = 3;

constexpr uint32_t BITS_PER_BYTE = 8;
constexpr uint32_t BYTE_MASK = 0xFFu;
constexpr uint32_t BYTE_SHIFT_1 = BITS_PER_BYTE;      // 8
constexpr uint32_t BYTE_SHIFT_2 = 2 * BITS_PER_BYTE;  // 16
constexpr uint32_t BYTE_SHIFT_3 = 3 * BITS_PER_BYTE;  // 24

constexpr size_t BUFFER_ENTRY_SIZE = 128;
constexpr uint32_t FULL_MASK = 0xFFFFFFFFU;
constexpr size_t BIT_MASK = 32;
constexpr size_t CREATION_TIME_LOW_OFFSET = 0x64;
constexpr size_t CREATION_TIME_HIGH_OFFSET = 0x68;
constexpr size_t MODIFIED_TIME_LOW_OFFSET = 0x6C;
constexpr size_t MODIFIED_TIME_HIGH_OFFSET = 0x70;
constexpr uint64_t WINDOWS_TICK = 10000000ULL;
constexpr uint64_t SEC_TO_UNIX_EPOCH = 11644473600ULL;
constexpr uint64_t NANOS_PER_SEC = 1000000000ULL;
constexpr uint32_t READ_U32_BUF_LEN = 4;
constexpr uint32_t READ_U16_BUF_LEN = 2;

inline uint64_t GetCurrentFileTime()
{
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t secs = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    uint64_t nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % NANOS_PER_SEC;
    // 检查系统时钟的分辨率
    auto period = std::chrono::high_resolution_clock::period();
    if (period.den != NANOS_PER_SEC) {
        OBJECT_EDITOR_LOGW(ObjectEditorDomain::CLIENT_NDK, "System clock resolution is not nanosecond-precise");
    }
    return (secs * SEC_TO_UNIX_EPOCH) * WINDOWS_TICK + (nanos / 100ULL);
}

inline uint32_t ReadUint32(const Byte *ptr, uint32_t size = READ_U32_BUF_LEN)
{
    if (size < READ_U32_BUF_LEN) {
        return 0;
    }
    return static_cast<uint32_t>(ptr[BYTE_INDEX_0]) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_1]) << BYTE_SHIFT_1) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_2]) << BYTE_SHIFT_2) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_3]) << BYTE_SHIFT_3);
}

inline void WriteUint32(Byte *ptr, uint32_t data, uint32_t size = READ_U32_BUF_LEN)
{
    if (size < READ_U32_BUF_LEN) {
        return;
    }
    ptr[BYTE_INDEX_0] = static_cast<Byte>(data & BYTE_MASK);
    ptr[BYTE_INDEX_1] = static_cast<Byte>((data >> BYTE_SHIFT_1) & BYTE_MASK);
    ptr[BYTE_INDEX_2] = static_cast<Byte>((data >> BYTE_SHIFT_2) & BYTE_MASK);
    ptr[BYTE_INDEX_3] = static_cast<Byte>((data >> BYTE_SHIFT_3) & BYTE_MASK);
}

// 读取/写入 16 位、32 位整数
inline uint16_t ReadUint16(const Byte *ptr, uint32_t size = READ_U16_BUF_LEN)
{
    if (size < READ_U16_BUF_LEN) {
        return 0;
    }
    return static_cast<uint16_t>(ptr[BYTE_INDEX_0]) |
           (static_cast<uint16_t>(ptr[BYTE_INDEX_1]) << BYTE_SHIFT_1);
}

inline void WriteUint16(Byte *ptr, uint16_t data, uint32_t size = READ_U16_BUF_LEN)
{
    if (size < READ_U16_BUF_LEN) {
        return;
    }
    ptr[BYTE_INDEX_0] = static_cast<Byte>(data & BYTE_MASK);
    ptr[BYTE_INDEX_1] = static_cast<Byte>((data >> BYTE_SHIFT_1) & BYTE_MASK);
}

inline constexpr int HexToNibble(char c) noexcept
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;  // 10 is the base of hexadecimal
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;  // 10 is the base of hexadecimal
    return -1;
}

namespace {
    constexpr uint32_t LENGTH = 32;
    constexpr uint32_t LENGTH_HYPHEN = 36;
}
std::optional<CLSID> ParseOEidToClsid(const std::string &uuid);

std::string FormatClsidToOEid(const CLSID &clsid);

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // UTILS_H