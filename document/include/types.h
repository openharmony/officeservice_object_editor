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

#ifndef TYPES_H
#define TYPES_H

#include <cstdint>

namespace OHOS {
namespace ObjectEditor {
namespace {
constexpr uint32_t FOUR_BYTE_SIZE = 4;

// Header info
constexpr uint16_t DEFAULT_MINOR_VERSION  = 0x003Eu;
constexpr uint16_t DEFAULT_MAJOR_VERSION  = 0x0003u;
constexpr uint16_t DEFAULT_BYTE_ORDER = 0xFFFEu;         // 默认为小端
constexpr uint16_t DEFAULT_SECTOR_SHIFT = 0x0009u;       // MAJOR_VERSION 3: 2^9 = 512 字节
constexpr uint16_t DEFAULT_MINI_SECTOR_SHIFT = 0x0006u;  // 2^6 = 64 字节
constexpr uint32_t MINI_STREAM_CUTOFF = 4096;            // Mini Stream 阈值
constexpr size_t HEADER_FIXED_SIZE = 0x4C;               // 固定字段长度
constexpr size_t HEADER_DIFAT_ARRAY_SIZE = 109;          // Header DIFAT 条目数
constexpr uint32_t CLSID_SIZE = 16;
constexpr uint32_t FILE_SIGNATURE_SIZE = 8;

// Header offset
constexpr uint32_t HEADER_CLSID_OFFSET = 0x08;
constexpr uint32_t HEADER_MINOR_VERSION_OFFSET = 0x18;
constexpr uint32_t HEADER_MAJOR_VERSION_OFFSET = 0x1A;
constexpr uint32_t HEADER_BYTE_ORDER_OFFSET = 0x1C;
constexpr uint32_t HEADER_SECTOR_SHIFT_OFFSET = 0x1E;
constexpr uint32_t HEADER_MINI_SECTOR_SHIFT_OFFSET = 0x20;
constexpr uint32_t HEADER_RESERVED_OFFSET = 0x22;
constexpr uint32_t HEADER_DIR_SECTOR_NUMBER_OFFSET = 0x28;
constexpr uint32_t HEADER_FAT_SECTOR_NUMBER_OFFSET = 0x2C;
constexpr uint32_t HEADER_FIRST_DIR_SECTOR_OFFSET = 0x30;
constexpr uint32_t HEADER_TRANSACTION_SIGNATURE_OFFSET = 0x34;
constexpr uint32_t HEADER_MINI_STREAM_CUTOFF_OFFSET = 0x38;
constexpr uint32_t HEADER_FIRST_MINI_FAT_SECTOR_OFFSET = 0x3C;
constexpr uint32_t HEADER_MINI_FAT_SECTOR_NUMBER_OFFSET = 0x40;
constexpr uint32_t HEADER_FIRST_DIFAT_SECTOR_OFFSET = 0x44;
constexpr uint32_t HEADER_DIFAT_SECTOR_NUMBER_OFFSET = 0x48;
constexpr uint32_t HEADER_DIFAT_ARRAY_OFFSET = 0x4C;

// Dir info
constexpr uint32_t DIR_ENTRY_END = static_cast<uint32_t>(-1);
}

// 固定宽度别名，用于 MS-CFB 底层结构与偏移计算。
using Byte = std::uint8_t;
using SectorIndex = std::uint32_t;
using StreamPos = uint64_t;
using BlockCount = std::uint32_t;
using CLSID = std::array<uint8_t, CLSID_SIZE>;

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // TYPES_H