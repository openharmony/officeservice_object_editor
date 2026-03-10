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

#include <cstring>
#include <iomanip>
#include <sstream>
#include <securec.h>
#include <iostream>

#include "header.h"
#include "alloctable.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
static const Byte g_cdMagic[] = {0xD0u, 0xCFu, 0x11u, 0xE0u, 0xA1u, 0xB1u, 0x1Au, 0xE1u};

Header::Header()
{
    minorVersion_ = DEFAULT_MINOR_VERSION;
    majorVersion_ = DEFAULT_MAJOR_VERSION;
    byteOrder_ = DEFAULT_BYTE_ORDER;
    bigBlockShift_ = DEFAULT_SECTOR_SHIFT;
    miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    dirSectorCount_ = 0;
    numBat_ = 0;
    direntStart_ = 0;
    transactionSignature_ = 0;
    threshold_ = MINI_STREAM_CUTOFF;
    sbatStart_ = 0;
    numSbat_ = 0;
    difatStart_ = 0;
    numDifat_ = 0;

    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; i++) {
        id_[i] = g_cdMagic[i];
    }
    clsid_.fill(0);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; i++) {
        bbBlocks_[i] = AllocTable::Avail;
    }
}

bool Header::Valid() const
{
    constexpr int32_t MAX_SECTOR_SHIFT_SIZE = 31;
    if (threshold_ != MINI_STREAM_CUTOFF || numBat_ == 0 || bigBlockShift_ <= DEFAULT_MINI_SECTOR_SHIFT ||
        bigBlockShift_ >= MAX_SECTOR_SHIFT_SIZE || miniBlockShift_ >= bigBlockShift_) {
        return false;
    }
    if (numBat_ > HEADER_DIFAT_ARRAY_SIZE) {
        const uint32_t sectorSize = 1u << bigBlockShift_;
        const uint32_t entriesPerSector = sectorSize / static_cast<uint32_t>(sizeof(uint32_t));
        const uint32_t difatEntries = entriesPerSector > 0 ? entriesPerSector - 1 : 0;
        if (difatEntries == 0) {
            return false;
        }
        const uint32_t extraNeeded = numBat_ - HEADER_DIFAT_ARRAY_SIZE;
        const uint32_t difatSectorsNeeded = (extraNeeded + difatEntries - 1) / difatEntries;
        if (numDifat_ < difatSectorsNeeded) {
            return false;
        }
    }
    if ((numBat_ < HEADER_DIFAT_ARRAY_SIZE) && (numDifat_ != 0)) {
        return false;
    }
    return true;
}

bool Header::IsCompoundDocument() const
{
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; i++) {
        if (id_[i] != g_cdMagic[i]) {
            return false;
        }
    }
    return true;
}

bool Header::Load(const Byte *buffer, size_t len)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    if (len < required || buffer == nullptr) [[unlikely]] {
        return false;
    }

    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; i++) {
        id_[i] = buffer[i];
    }

    if (memcpy_s(clsid_.data(), clsid_.size(), buffer + 0x08, clsid_.size()) != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Header::Load - memcpy_s failed");
        return false;
    }
    bigBlockShift_ = ReadUint16(buffer + HEADER_SECTOR_SHIFT_OFFSET);
    miniBlockShift_ = ReadUint16(buffer + HEADER_MINI_SECTOR_SHIFT_OFFSET);
    numBat_ = ReadUint32(buffer + HEADER_FAT_SECTOR_NUMBER_OFFSET);
    direntStart_ = ReadUint32(buffer + HEADER_FIRST_DIR_SECTOR_OFFSET);
    transactionSignature_ = ReadUint32(buffer + HEADER_TRANSACTION_SIGNATURE_OFFSET);
    threshold_ = ReadUint32(buffer + HEADER_MINI_STREAM_CUTOFF_OFFSET);
    if (threshold_ != MINI_STREAM_CUTOFF) {
        return false;
    }
    sbatStart_ = ReadUint32(buffer + HEADER_FIRST_MINI_FAT_SECTOR_OFFSET);
    numSbat_ = ReadUint32(buffer + HEADER_MINI_FAT_SECTOR_NUMBER_OFFSET);
    difatStart_ = ReadUint32(buffer + HEADER_FIRST_DIFAT_SECTOR_OFFSET);
    numDifat_ = ReadUint32(buffer + HEADER_DIFAT_SECTOR_NUMBER_OFFSET);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; i++) {
        bbBlocks_[i] = ReadUint32(buffer + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE);
    }
    return true;
}

bool Header::Save(Byte *buffer, size_t len)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    if (len < required || buffer == nullptr) [[unlikely]] {
        return false;
    }

    auto ec = memset_s(buffer, len, 0, HEADER_FIXED_SIZE);
    if (ec != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Header::Save - memset_s failed");
        return false;
    }
    ec = memcpy_s(buffer, len, g_cdMagic, FILE_SIGNATURE_SIZE);
    if (ec != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Header::Save - magic data memcpy_s failed");
        return false;
    }
    ec = memcpy_s(buffer + FILE_SIGNATURE_SIZE, len - FILE_SIGNATURE_SIZE, clsid_.data(), clsid_.size());
    if (ec != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Header::Save - signature memcpy_s failed");
        return false;
    }

    WriteUint16(buffer + HEADER_MINOR_VERSION_OFFSET, DEFAULT_MINOR_VERSION);
    WriteUint16(buffer + HEADER_MAJOR_VERSION_OFFSET, DEFAULT_MAJOR_VERSION);
    WriteUint16(buffer + HEADER_BYTE_ORDER_OFFSET, DEFAULT_BYTE_ORDER);
    WriteUint16(buffer + HEADER_SECTOR_SHIFT_OFFSET, bigBlockShift_);
    WriteUint16(buffer + HEADER_MINI_SECTOR_SHIFT_OFFSET, miniBlockShift_);
    WriteUint32(buffer + HEADER_TRANSACTION_SIGNATURE_OFFSET, transactionSignature_);
    WriteUint32(buffer + HEADER_FAT_SECTOR_NUMBER_OFFSET, numBat_);
    WriteUint32(buffer + HEADER_FIRST_DIR_SECTOR_OFFSET, direntStart_);
    threshold_ = MINI_STREAM_CUTOFF;
    WriteUint32(buffer + HEADER_MINI_STREAM_CUTOFF_OFFSET, threshold_);
    WriteUint32(buffer + HEADER_FIRST_MINI_FAT_SECTOR_OFFSET, sbatStart_);
    WriteUint32(buffer + HEADER_MINI_FAT_SECTOR_NUMBER_OFFSET, numSbat_);
    WriteUint32(buffer + HEADER_FIRST_DIFAT_SECTOR_OFFSET, difatStart_);
    WriteUint32(buffer + HEADER_DIFAT_SECTOR_NUMBER_OFFSET, numDifat_);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; i++) {
        WriteUint32(buffer + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE, bbBlocks_[i]);
    }
    return true;
}

void Header::Debug()
{
    const uint32_t ENDOFCHAIN = AllocTable::Eof;
    constexpr uint32_t HEX_WIDTH = 8;
    std::ostringstream oss;
    auto fmtHex8 = [&oss](uint32_t v) {
        oss << "0x" << std::hex << std::setw(HEX_WIDTH) << std::setfill('0') << std::nouppercase << v;
        return oss.str();
    };
    auto sectorOrNone = [&](uint32_t v) {
        return v == ENDOFCHAIN ? std::string("[NONE]") : fmtHex8(v);
    };
    auto indexOrNoneDec = [&](uint32_t v) {
        return v == ENDOFCHAIN ? std::string("[NONE]") : std::to_string(v);
    };
    constexpr uint32_t PRINT_WIDTH = 16;
    oss << "==================== OLE HEADER ====================" << std::endl;
    oss << std::left << std::setw(PRINT_WIDTH) << "Block Shift:" << std::right << bigBlockShift_ << " (2^"
        << bigBlockShift_ << " = " << (1u << bigBlockShift_) << " bytes)" << std::endl;
    oss << std::left << std::setw(PRINT_WIDTH) << "Mini Shift:" << std::right << miniBlockShift_ << " (2^"
        << miniBlockShift_ << " = " << (1u << miniBlockShift_) << " bytes)" << std::endl;
    oss << std::left << std::setw(PRINT_WIDTH) << "FAT Blocks:" << std::right << numBat_ << std::endl;
    oss << std::left << std::setw(PRINT_WIDTH) << "Dir Start:" << std::right << indexOrNoneDec(direntStart_)
        << std::endl;
    oss << std::left << std::setw(PRINT_WIDTH) << "Threshold:" << std::right << threshold_ << " bytes"
        << std::endl;

    oss << std::left << std::setw(PRINT_WIDTH) << "MiniFAT Start:" << std::right << sectorOrNone(sbatStart_)
        << std::endl;
    if (numSbat_ != 0) {
        oss << std::left << std::setw(PRINT_WIDTH) << "MiniFAT Count:" << std::right << numSbat_ << std::endl;
    }
    oss << std::left << std::setw(PRINT_WIDTH) << "DIFAT Start:" << std::right << sectorOrNone(difatStart_)
        << std::endl;
    if (numDifat_ != 0) {
        oss << std::left << std::setw(PRINT_WIDTH) << "DIFAT Count:" << std::right << numDifat_ << std::endl;
    }

    uint32_t s = (numBat_ <= HEADER_DIFAT_ARRAY_SIZE) ? numBat_ : HEADER_DIFAT_ARRAY_SIZE;
    oss << std::left << std::setw(PRINT_WIDTH) << (std::string("BAT Blocks (first ") + std::to_string(s) + "): ");
    if (s == 0) {
        oss << "";
    } else {
        for (uint32_t i = 0; i < s; i++) {
            if (i) {
                oss << ' ';
            }
            oss << bbBlocks_[i];
        }
    }
    oss << std::endl;
    oss << "====================================================" << std::endl;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "dirtree dump: %{public}s", oss.str().c_str());
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS