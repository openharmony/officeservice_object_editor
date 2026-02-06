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

inline uint32_t ReadUint32(const Byte *ptr)
{
    return static_cast<uint32_t>(ptr[BYTE_INDEX_0]) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_1]) << BYTE_SHIFT_1) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_2]) << BYTE_SHIFT_2) |
           (static_cast<uint32_t>(ptr[BYTE_INDEX_3]) << BYTE_SHIFT_3);
}

inline void WriteUint32(Byte *ptr, uint32_t data)
{
    ptr[BYTE_INDEX_0] = static_cast<Byte>(data & BYTE_MASK);
    ptr[BYTE_INDEX_1] = static_cast<Byte>((data >> BYTE_SHIFT_1) & BYTE_MASK);
    ptr[BYTE_INDEX_2] = static_cast<Byte>((data >> BYTE_SHIFT_2) & BYTE_MASK);
    ptr[BYTE_INDEX_3] = static_cast<Byte>((data >> BYTE_SHIFT_3) & BYTE_MASK);
}

// 读取/写入 16 位、32 位整数
inline uint16_t ReadUint16(const Byte *ptr)
{
    return static_cast<uint16_t>(ptr[BYTE_INDEX_0]) |
           (static_cast<uint16_t>(ptr[BYTE_INDEX_1]) << BYTE_SHIFT_1);
}

inline void WriteUint16(Byte *ptr, uint16_t data)
{
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

// Parse UUID string into 16-byte CLSID in Windows GUID byte order:
// Data1/Data2/Data3 are little-endian; Data4 remains in network order.
// Accepts
// - "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX" (36 chars)
// - "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" (32 chars)
// Returns nullopt on failure.
namespace {
    constexpr uint32_t LENGTH = 32;
    constexpr uint32_t LENGTH_HYPHEN = 36;
}
std::optional<CLSID> ParseHmidToClsid(const std::string &uuid);

// Format 16-byte CLSID (Windows GUID byte order) as UUID string with hyphens.
std::string FormatClsidToHmid(const CLSID &clsid);

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // UTILS_H