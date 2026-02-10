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

#include <algorithm>
#include <cstring>
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <limits>
#include <optional>
#include <securec.h>
#include <set>
#include <utility>
#include <filesystem>
#include <array>

#include "alloctable.h"
#include "dirtree.h"
#include "header.h"
#include "storage.h"
#include "stream.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

StorageIO::StorageIO(const char *filename)
{
    dtModified_ = false;
    Init();
    auto f = std::make_unique<std::fstream>(filename,
        std::ios::binary | std::ios::in | std::ios::out);
    if (!f || f->fail()) {
        SetError(ErrorCode::OpenFailed, "Failed to open file for read/write");
        return;
    }
    file_ = std::move(f);
    stream_ = file_.get();
    const bool loaded = Load();
    if (!loaded) {
        return;
    }
}

StorageIO::StorageIO(std::iostream *stream)
{
    dtModified_ = false;
    Init();
    if (!stream) {
        SetError(ErrorCode::OpenFailed, "Invalid input stream");
        return;
    }
    stream_ = stream;
    const bool loaded = Load();
    if (!loaded) {
        return;
    }
}

StorageIO::StorageIO(const std::string &hmid)
{
    dtModified_ = false;
    Init();
    if (!ConfigMinimalCd(hmid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::StorageIO - ConfigMinimalCd failed");
        return;
    }

    if (!SerializeToMemory()) {
        SetError(ErrorCode::BadOLE, "Failed to serialize the minimal in-memory OLE from HMID");
        return;
    }
    ClearError();
}

bool StorageIO::ConfigMinimalCd(const std::string &hmid)
{
    if (!dirtree_ || !header_ || !bbat_) {
        return false;
    }
    if (dirtree_) {
        dirtree_->Clear();
    }

    const auto clsidOpt = ParseHmidToClsid(hmid);
    if (!clsidOpt) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "StorageIO::ConfigMinimalCd - Invalid HMID");
        return false;
    }
    const CLSID &clsid = *clsidOpt;
    header_->SetFatSectorCount(1);
    header_->SetFirstDirSector(1);
    header_->SetFirstMiniFat(AllocTable::Eof);
    header_->SetMiniFatCount(0);
    header_->SetFirstDifat(AllocTable::Eof);
    header_->SetDifatCount(0);
    header_->SetDifatElem(0, DIFAT_ELEM_VALUE);

    if (auto *root = GetRootEntry()) {
        root->SetType(DIR_ENTRY_ROOT);
        root->SetName("Root Entry");
        root->SetNameLen(ROOT_ENTRY_NAME_LEN);
        root->SetPrev(DIR_ENTRY_END);
        root->SetNext(DIR_ENTRY_END);
        root->SetChild(DIR_ENTRY_END);
        root->SetStart(0);
        root->SetSize(kDefaultSectorSize);
        root->SetClsid(clsid, std::size(clsid));
    }

    bbat_->SetBlockSize(1 << header_->BigBlockShift());
    const uint32_t fatEntriesPerSector = kDefaultSectorSize / static_cast<uint32_t>(sizeof(uint32_t));
    bbat_->Resize(fatEntriesPerSector);
    bbat_->Set(INDEX_ZERO, AllocTable::Eof);
    bbat_->Set(INDEX_ONE, AllocTable::Eof);
    bbat_->Set(INDEX_TWO, AllocTable::Bat);

    fatSectors_.clear();
    fatSectors_.push_back(INDEX_TWO);
    difatSectors_.clear();
    protectedBlocksCache_.clear();
    protectedBlocksDirty_ = true;

    for (size_t i = 3; i < fatEntriesPerSector; i++) {
        bbat_->Set(i, AllocTable::Avail);
    }
    return true;
}

bool StorageIO::SerializeToMemory()
{
    if (!dirtree_ || !header_ || !bbat_) {
        return false;
    }
    const size_t sector = kHeaderSize;
    const size_t sectorCount = 4;
    std::vector<Byte> oleData(sector * sectorCount, 0);
    Byte *oleBytes = oleData.data();
    if (!header_->Save(oleBytes, sector)) {
        return false;
    }

    std::fill_n(oleData.data() + sector, sector, 0xFF);
    Byte *dirSector = oleData.data() + sector * 2;
    std::fill_n(dirSector, sector, 0x00);
    if (!dirtree_->Save(dirSector, sector)) {
        return false;
    }
    Byte *fatSector = oleData.data() + sector * 3;
    std::fill_n(fatSector, sector, 0xFF);
    if (!bbat_->Save(fatSector, sectorCount * bbat_->Count())) {
        return false;
    }

    memoryBuffer_ = std::make_unique<std::vector<uint8_t>>(std::move(oleData));
    if (!memoryBuffer_) {
        return false;
    }
    std::stringstream tempStream(std::string(reinterpret_cast<char *>(
        memoryBuffer_->data()), memoryBuffer_->size()),
        std::ios::in | std::ios::out | std::ios::binary);
    stream_ = &tempStream;
    bool loadResult = Load();
    stream_ = nullptr;
    return loadResult;
}

StorageIO::~StorageIO()
{
    Close();
}

void StorageIO::Init()
{
    auto newHeader = std::make_unique<Header>();
    if (newHeader == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::Init - Failed to create new header");
        return;
    }
    auto newDirTree = std::make_unique<DirTree>();
    auto newBbat = std::make_unique<AllocTable>(1 << newHeader->BigBlockShift());
    auto newSbat = std::make_unique<AllocTable>(1 << newHeader->SmallBlockShift());
    std::vector<uint32_t> newFatSectors{};
    std::vector<uint32_t> newDifatSectors{};
    std::set<uint32_t> newProtectedBlocksCache{};

    error_ = ErrorCode::Ok;
    errorMsg_.clear();
    isValid_ = true;
    file_.reset();
    stream_ = nullptr;
    flushWriteBuffer_ = nullptr;
    header_.swap(newHeader);
    dirtree_.swap(newDirTree);
    bbat_.swap(newBbat);
    sbat_.swap(newSbat);
    size_ = 0;
    fatSectors_.swap(newFatSectors);
    difatSectors_.swap(newDifatSectors);
    protectedBlocksCache_.swap(newProtectedBlocksCache);
    protectedBlocksDirty_ = true;
}

bool StorageIO::IsValid() const noexcept
{
    return isValid_;
}

bool StorageIO::IsDirty() const noexcept
{
    return dtModified_;
}

StorageIO::ErrorCode StorageIO::LastError() const noexcept
{
    return error_;
}

const char *StorageIO::ErrorMessage() const noexcept
{
    if (!errorMsg_.empty()) {
        return errorMsg_.c_str();
    }
    switch (error_) {
        case ErrorCode::Ok:
            return "No error";
        case ErrorCode::OpenFailed:
            return "Failed to open file";
        case ErrorCode::NotOLE:
            return "Not a valid OLE file";
        case ErrorCode::BadOLE:
            return "Corrupted OLE structure";
        case ErrorCode::IOError:
            return "I/O operation failed";
        case ErrorCode::AllocationFailed:
            return "Block allocation failed";
        case ErrorCode::InvalidOperation:
            return "Invalid operation";
        case ErrorCode::Corruption:
            return "Data corruption detected";
        default:
            return "Unknown error";
    }
}

void StorageIO::ClearError() noexcept
{
    error_ = ErrorCode::Ok;
    errorMsg_.clear();
    isValid_ = true;
}

void StorageIO::SetError(ErrorCode code, const std::string &message, bool invalidate)
{
    error_ = code;
    if (!message.empty()) {
        errorMsg_ = message;
    } else {
        errorMsg_.clear();
    }
    if (invalidate) {
        isValid_ = false;
    }
}

namespace {
bool IsValidReadParams(StorageIO *self, size_t len, uint8_t *buf, size_t *outRead)
{
    if (!outRead || (!buf && len > 0)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadRawCd - Invalid parameters");
        return false;
    }
    if (self->IsDirty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadRawCd - Storage is dirty. Flush before read.");
        return false;
    }

    if (!self->file_ || !self->stream_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadRawCd - ReadRawCd requires file-backed storage");
        return false;
    }
}
}

bool StorageIO::ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead)
{
    if (!IsValidReadParams(this, len, buf, outRead))
        return false;
    *outRead = 0;
    if (len == 0) {
        ClearError();
        return true;
    }
    const uint64_t fileSize = static_cast<uint64_t>(size_);
    const uint64_t offset64 = static_cast<uint64_t>(offset);
    if (offset64 >= fileSize) {
        ClearError();
        return true;
    }
    const uint64_t remaining = fileSize - offset64;
    const uint64_t wanted = static_cast<uint64_t>(len);
    const uint64_t toRead64 = std::min(wanted, remaining);
    const uint64_t maxStreamSize = static_cast<uint64_t>(
        std::numeric_limits<std::streamsize>::max());
    if (toRead64 > maxStreamSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "StorageIO::ReadRawCd - Read length overflow");
        return false;
    }
    file_->flush();
    if (auto *rdbuf = file_->rdbuf())
        rdbuf->pubsync();
    stream_->clear();
    stream_->seekg(static_cast<std::streamoff>(offset64), std::ios::beg);
    if (!stream_->good()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadRawCd - Failed to seek CD file");
        return false;
    }

    const std::streamsize toRead = static_cast<std::streamsize>(toRead64);
    stream_->read(reinterpret_cast<char *>(buf), toRead);
    const std::streamsize actualRead = stream_->gcount();
    if ((actualRead < toRead) && !stream_->eof()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadRawCd - Failed to read CD file");
        *outRead = static_cast<size_t>(actualRead);
        return false;
    }
    stream_->clear();
    *outRead = static_cast<size_t>(actualRead);
    ClearError();
    return true;
}

bool StorageIO::Load()
{
    if (!stream_) {
        SetError(ErrorCode::OpenFailed, "No input stream available");
        return false;
    }

    stream_->seekg(0, std::ios::end);
    size_ = stream_->tellg();
    const uint64_t fileSize = size_ >= 0 ? static_cast<uint64_t>(size_) : 0;

    if (!ValidateHeader(fileSize)) {
        return false;
    }
    const uint32_t sectorSize = BigBlockSize();
    std::vector<uint32_t> fatBlocks;
    if (!LoadDifatChain(sectorSize, fatBlocks)) {
        return false;
    }
    if (!LoadFatChain(sectorSize, fatBlocks)) {
        return false;
    }
    SectorIndex sbStart = 0;
    if (!LoadDirectoryTree(sbStart)) {
        return false;
    }
    if (!LoadMiniFat(sbStart)) {
        return false;
    }
    ClearError();
    return true;
}

bool StorageIO::ValidateHeader(uint64_t fileSize)
{
    if (!ReadAndLoadHeader()) {
        return false;
    }
    uint32_t sectorSize = 0;
    if (!ValidateSectorSizes(sectorSize)) {
        return false;
    }
    if (!CheckClaimedTableSizes(fileSize, sectorSize)) {
        return false;
    }
    if (!header_) {
        return false;
    }
    if (!ValidateHeaderSectorIndex(header_->DirentStart(),
        sectorSize, "dirent_start", fileSize)) {
        return false;
    }
    if (!ValidateHeaderSectorIndex(header_->SbatStart(),
        sectorSize, "sbat_start", fileSize)) {
        return false;
    }
    if (!ValidateHeaderSectorIndex(header_->DifatStart(),
        sectorSize, "difatStart", fileSize)) {
        return false;
    }
    return true;
}

bool StorageIO::LoadFatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks)
{
    const uint32_t bufLen = static_cast<uint32_t>(fatBlocks.size()) * sectorSize;
    if (bufLen > 0) {
        std::vector<Byte> buffer(bufLen);
        const uint32_t bytesRead = LoadBigBlocks(fatBlocks, buffer.data(), bufLen);
        if (bytesRead != bufLen) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::LoadFatChain - Failed to read FAT sectors");
            return false;
        }
        if (bbat_) {
            bbat_->Load(buffer.data(), buffer.size());
        }
    }
    return true;
}

bool StorageIO::ReadDifatSector(uint32_t sectorSize, uint32_t difatSector,
    std::vector<uint8_t> &difatBuf)
{
    difatBuf.assign(sectorSize, 0xFF);
    const uint32_t bytesRead = LoadBigBlock(difatSector, difatBuf.data(), sectorSize);
    if (bytesRead != sectorSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadDifatSector - Failed to load DIFAT sector");
        return false;
    }
    if (difatSectors_.empty() || difatSectors_.back() != difatSector) {
        difatSectors_.push_back(difatSector);
    }
    return true;
}

bool StorageIO::ProcessDifatEntries(uint32_t entriesPerSector, uint32_t expectedFromDifat,
    uint32_t expectedMaxSectors, uint32_t &loadedFromDifat,
    const std::vector<uint8_t> &difatBuf, std::vector<uint32_t> &fatBlocks)
{
    const uint32_t difatEntries = entriesPerSector > 0 ? entriesPerSector - 1 : 0;
    for (uint32_t i = 0; i < difatEntries && loadedFromDifat < expectedFromDifat; ++i) {
        uint32_t entry = ReadUint32(difatBuf.data() + i * 4);
        if (entry == AllocTable::Avail) {
            continue;
        }
        if (expectedMaxSectors > 0 && entry >= expectedMaxSectors) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ProcessDifatEntries - DIFAT entry out of range");
            return false;
        }
        fatBlocks.push_back(entry);
        ++loadedFromDifat;
    }
    return true;
}

bool StorageIO::LoadDifatChainBody(uint32_t sectorSize, uint32_t expectedFromDifat,
    uint32_t expectedMaxSectors, std::vector<uint32_t> &fatBlocks)
{
    const uint32_t entriesPerSector = sectorSize / static_cast<uint32_t>(sizeof(uint32_t));
    if (!header_) {
        return false;
    }
    uint32_t difatSector = header_->DifatStart();
    uint32_t loadedFromDifat = 0;
    std::set<uint32_t> visited;
    while (difatSector != AllocTable::Eof && loadedFromDifat < expectedFromDifat) {
        if (expectedMaxSectors > 0 && difatSector >= expectedMaxSectors) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::LoadDifatChainBody - DIFAT sector out of range");
            return false;
        }
        if (!visited.insert(difatSector).second) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::LoadDifatChainBody - DIFAT chain loop detected");
            return false;
        }

        std::vector<uint8_t> difatBuf;
        if (!ReadDifatSector(sectorSize, difatSector, difatBuf)) {
            return false;
        }
        if (!ProcessDifatEntries(entriesPerSector, expectedFromDifat,
            expectedMaxSectors, loadedFromDifat, difatBuf, fatBlocks)) {
            return false;
        }
        uint32_t nextDifat = (entriesPerSector > 0)
            ? ReadUint32(difatBuf.data() + (entriesPerSector - 1) * 4)
            : AllocTable::Eof;
        if (nextDifat == difatSector) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::LoadDifatChainBody - DIFAT chain self-reference");
            return false;
        }
        if (expectedMaxSectors > 0 && nextDifat != AllocTable::Eof &&
            nextDifat >= expectedMaxSectors) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::LoadDifatChainBody - DIFAT chain next sector out of range");
            return false;
        }
        difatSector = nextDifat;
    }

    if (loadedFromDifat < expectedFromDifat) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadDifatChainBody - Incomplete DIFAT chain");
        return false;
    }
    return true;
}

bool StorageIO::LoadDifatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks)
{
    fatBlocks.clear();
    if (!header_) {
        return false;
    }
    const BlockCount numBat = header_->NumBat();
    const BlockCount headerFatCount = std::min<BlockCount>(numBat,
        static_cast<BlockCount>(HEADER_DIFAT_ARRAY_SIZE));
    fatBlocks.reserve(static_cast<size_t>(numBat));
    for (uint32_t i = 0; i < headerFatCount; ++i) {
        fatBlocks.push_back(header_->BbBlocks()[i]);
    }

    const uint64_t fileSize = size_ >= 0 ? static_cast<uint64_t>(size_) : 0;
    uint32_t expectedMaxSectors = 0;
    if (sectorSize > 0 && fileSize >= sectorSize) {
        const uint64_t maxBlocks64 = fileSize / sectorSize;
        if (maxBlocks64 > 0) {
            expectedMaxSectors = static_cast<uint32_t>(std::min<uint64_t>(maxBlocks64 - 1,
                std::numeric_limits<uint32_t>::max()));
        }
    }
    if (numBat > headerFatCount) {
        const uint32_t expectedFromDifat = numBat - headerFatCount;
        if (!LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors, fatBlocks)) {
            return false;
        }
    }
    fatSectors_ = fatBlocks;
    return true;
}

bool StorageIO::LoadDirectoryTree(SectorIndex &sbStart)
{
    std::vector<uint32_t> blocks;
    if (!bbat_ || !header_) {
        return false;
    }
    if (!bbat_->Follow(header_->DirentStart(), blocks)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadDirectoryTree - Failed to follow directory chain");
        return false;
    }

    const uint32_t bufLen = static_cast<uint32_t>(blocks.size()) *
        static_cast<uint32_t>(bbat_->BlockSize());
    std::vector<Byte> buffer(bufLen);
    const uint32_t bytesRead = LoadBigBlocks(blocks, buffer.data(), bufLen);
    if (bytesRead != bufLen) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadDirectoryTree - Failed to read directory sectors");
        return false;
    }
    if (!dirtree_) {
        return false;
    }
    bool dirtreeLoaded = dirtree_->Load(buffer.data(), buffer.size());
    if (!dirtreeLoaded) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadDirectoryTree - Failed to load directory tree");
        return false;
    }
    sbStart = ReadUint32(buffer.data() + 0x74);
    return true;
}

bool StorageIO::LoadMiniFat(SectorIndex sbStart)
{
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    if (!LoadMiniFatBlocks(blocks, buffer)) {
        return false;
    }
    if (!FollowMiniStream(sbStart)) {
        return false;
    }
    size_t highestUsed = 0;
    if (!ValidateMiniFatEntries(highestUsed)) {
        return false;
    }
    return ValidateMiniRootCoverage(highestUsed);
}

bool StorageIO::LoadMiniFatBlocks(std::vector<uint32_t> &blocks, std::vector<Byte> &buffer)
{
    if (!bbat_ || !header_) {
        return false;
    }
    if (!bbat_->Follow(header_->SbatStart(), blocks)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadMiniFatBlocks - Failed to follow mini FAT chain");
        return false;
    }
    const uint32_t bufLen = static_cast<uint32_t>(blocks.size()) * bbat_->BlockSize();
    if (bufLen == 0) {
        return true;
    }
    buffer.resize(bufLen);
    const uint32_t bytesRead = LoadBigBlocks(blocks, buffer.data(), bufLen);
    if (bytesRead != bufLen) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::LoadMiniFatBlocks - Failed to read mini FAT sectors");
        return false;
    }
    if (!sbat_) {
        return false;
    }
    sbat_->Load(buffer.data(), buffer.size());
    return true;
}

bool StorageIO::FollowMiniStream(SectorIndex sbStart)
{
    if (!bbat_) {
        return false;
    }
    if (!bbat_->Follow(sbStart, sbBlocks_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FollowMiniStream - Failed to follow mini FAT chain");
        return false;
    }
    return true;
}

bool StorageIO::ValidateMiniFatEntries(size_t &highestUsed)
{
    highestUsed = 0;
    if (!sbat_) {
        return false;
    }
    const size_t miniEntries = sbat_->Count();
    for (size_t i = 0; i < miniEntries; ++i) {
        const uint32_t entry = (*sbat_)[i];
        if (entry != AllocTable::Avail) {
            highestUsed = i + 1;
        }

        const bool isSpecial = (entry == AllocTable::Avail || entry == AllocTable::Eof ||
                                entry == AllocTable::Bat || entry == AllocTable::MetaBat);
        if (!isSpecial && entry >= miniEntries) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ValidateMiniFatEntries - Invalid mini FAT entry");
            return false;
        }
    }
    return true;
}

bool StorageIO::ValidateMiniRootCoverage(size_t highestUsed)
{
    if (highestUsed == 0) {
        return true;
    }
    const uint64_t requiredMiniBytes = static_cast<uint64_t>(highestUsed) * SmallBlockSize();
    const uint64_t bigBlockSz = BigBlockSize();
    if (bigBlockSz == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateMiniRootCoverage - Invalid big block size");
        return false;
    }
    const uint64_t requiredBigBlocks = (requiredMiniBytes + bigBlockSz - 1) / bigBlockSz;
    if (requiredBigBlocks > sbBlocks_.size()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateMiniRootCoverage - Mini FAT reference exceeds available big blocks");
        return false;
    }
    return true;
}

bool StorageIO::Create(const char *filename)
{
    auto f = std::make_unique<std::fstream>(filename, std::ios::binary | std::ios::out);
    if (!f || f->fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::Create - Failed to open file");
        return false;
    }
    ClearError();
    file_ = std::move(f);
    stream_ = file_.get();
    return true;
}

void StorageIO::Close()
{
    const bool flushed = Flush();
    if (!flushed) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::Close - Failed to flush file");
    }
    streams_.clear();
    if (file_) {
        file_->close();
        file_.reset();
        stream_ = nullptr;
    }
}

uint32_t StorageIO::ReadBigBlocksFromMemory(const std::vector<uint32_t> &blocks,
    Byte *data, uint32_t maxlen)
{
    if (!memoryBuffer_ || !bbat_) {
        return 0;
    }
    const uint32_t blockSize32 = bbat_->BlockSize();
    const uint64_t blockSize = blockSize32;
    uint32_t bytes = 0;
    for (uint32_t i = 0; (i < blocks.size()) && (bytes < maxlen); ++i) {
        const uint32_t block = blocks[i];
        const uint64_t pos = BlockToOffset(block, blockSize);
        const size_t bufferSize = memoryBuffer_->size();
        if (pos >= bufferSize) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadBigBlocksFromMemory - Block offset exceeds memory buffer size");
            return bytes;
        }
        uint32_t p = std::min<uint32_t>(blockSize32, maxlen - bytes);
        if (pos + p > bufferSize) {
            p = static_cast<uint32_t>(bufferSize - pos);
        }
        if (memcpy_s(data + bytes, static_cast<size_t>(maxlen - bytes),
            memoryBuffer_->data() + static_cast<size_t>(pos), p) != EOK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadBigBlocksFromMemory - Failed to memcpy data from memory buffer");
            return 0;
        }
        bytes += p;
    }
    return bytes;
}

uint32_t StorageIO::ReadBigBlocksFromFile(const std::vector<uint32_t> &blocks,
    Byte *data, uint32_t maxlen)
{
    if (!stream_ || !stream_->good() || maxlen == 0 || blocks.empty() || !bbat_) {
        return 0;
    }
    const uint32_t blockSize32 = bbat_->BlockSize();
    const uint64_t blockSize = blockSize32;
    if (blockSize32 == 0) {
        return 0;
    }
    if (file_) {
        file_->flush();
        if (auto *buf = file_->rdbuf()) {
            buf->pubsync();
        }
    }
    const uint64_t maxStreamOff = static_cast<uint64_t>(std::numeric_limits<std::streamoff>::max());
    const uint64_t fileSize = size_ >= 0 ? static_cast<uint64_t>(size_) : 0;

    uint32_t bytes = 0;
    for (uint32_t i = 0; (i < blocks.size()) && (bytes < maxlen); ++i) {
        const uint32_t block = blocks[i];
        const uint64_t pos = BlockToOffset(block, blockSize);
        if (pos > maxStreamOff) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadBigBlocksFromFile - Block offset overflow");
            return bytes;
        }
        if (pos >= fileSize) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadBigBlocksFromFile - Block offset exceeds file size");
            return bytes;
        }
        uint32_t p = std::min<uint32_t>(blockSize32, maxlen - bytes);
        const uint64_t endPos = pos + p;
        if (endPos > fileSize) {
            p = static_cast<uint32_t>(fileSize - pos);
        }
        stream_->seekg(static_cast<std::streamoff>(pos));
        stream_->read(reinterpret_cast<char *>(data) + bytes, p);
        const std::streamsize actualRead = stream_->gcount();
        if ((actualRead < static_cast<std::streamsize>(p)) && !stream_->eof()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadBigBlocksFromFile - Failed to read big block");
            return bytes + static_cast<uint32_t>(actualRead);
        }
        stream_->clear();
        bytes += static_cast<uint32_t>(actualRead);
    }
    return bytes;
}

uint32_t StorageIO::LoadBigBlocks(const std::vector<uint32_t> &blocks,
    Byte *data, uint32_t maxlen)
{
    if (!data || maxlen == 0 || blocks.empty()) {
        return 0;
    }
    const uint32_t blockSize32 = bbat_ ? bbat_->BlockSize() : 0;
    if (blockSize32 == 0) {
        return 0;
    }
    if (memoryBuffer_) {
        return ReadBigBlocksFromMemory(blocks, data, maxlen);
    }
    return ReadBigBlocksFromFile(blocks, data, maxlen);
}

uint32_t StorageIO::LoadBigBlock(uint32_t block, Byte *data, uint32_t maxlen)
{
    if (!data) {
        return 0;
    }
    std::vector<uint32_t> blocks;
    blocks.resize(1);
    blocks[0] = block;
    return LoadBigBlocks(blocks, data, maxlen);
}

bool StorageIO::ValidateMiniBlockAccess(uint32_t block, uint64_t pos64,
    uint64_t miniStreamSize, uint32_t bigBlockSz, uint32_t &bbindex)
{
    if (pos64 >= miniStreamSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateMiniBlockAccess - mini block offset exceeds miniStreamSize");
        return false;
    }
    if (pos64 > std::numeric_limits<uint64_t>::max() - bigBlockSz || bigBlockSz == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateMiniBlockAccess - mini block offset exceeds uint32_t max");
        return false;
    }
    
    bbindex = static_cast<uint32_t>(pos64 / bigBlockSz);
    if (bbindex >= sbBlocks_.size()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateMiniBlockAccess - mini block resolves outside Root stream");
        return false;
    }
    return true;
}

bool StorageIO::ReadMiniBlock(uint32_t block, uint64_t pos64, uint64_t miniStreamSize,
    uint32_t bigBlockSz, uint32_t smallBlockSz, uint32_t remaining, Byte *dest, uint32_t &copied)
{
    copied = 0;
    uint32_t bbindex = 0;
    if (!ValidateMiniBlockAccess(block, pos64, miniStreamSize, bigBlockSz, bbindex)) {
        return false;
    }
    std::vector<Byte> buf(bigBlockSz);
    const uint32_t read = LoadBigBlock(sbBlocks_[bbindex], buf.data(), bigBlockSz);
    if (read != bigBlockSz || bigBlockSz == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniBlock - Failed to read mini stream backing block");
        return false;
    }

    uint32_t offset = static_cast<uint32_t>(pos64 % bigBlockSz);
    uint32_t p = std::min(remaining, bigBlockSz - offset);
    if (p > smallBlockSz) {
        p = smallBlockSz;
    }
    if (pos64 > std::numeric_limits<uint64_t>::max() - p) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniBlock - mini block offset exceeds uint32_t max");
        return false;
    }
    const uint64_t endPos = pos64 + p;
    if (endPos > miniStreamSize) {
        if (miniStreamSize < pos64) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::ReadMiniBlock - mini stream  truncated");
            return false;
        }
        p = static_cast<uint32_t>(miniStreamSize - pos64);
    }
    if (memcpy_s(dest, remaining, buf.data() + offset, p) != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniBlock - Failed to memcpy data from mini block");
        return false;
    }
    copied = p;
    return true;
}

uint32_t StorageIO::ReadMiniBlocks(const std::vector<uint32_t> &blocks, Byte *data, uint32_t maxlen)
{
    if (!data || maxlen == 0 || blocks.empty() || !bbat_ || !sbat_) {
        return 0;
    }
    const uint32_t bigBlockSz = bbat_->BlockSize();
    const uint32_t smallBlockSz = sbat_->BlockSize();
    if (bigBlockSz == 0 || smallBlockSz == 0) {
        return 0;
    }
    const DirEntry *root = GetRootEntry();
    uint64_t miniStreamSize = root ? root->Size() : 0;
    if (miniStreamSize == 0 && !sbBlocks_.empty()) {
        miniStreamSize = static_cast<uint64_t>(sbBlocks_.size()) * bigBlockSz;
    }
    uint32_t bytes = 0;
    for (uint32_t i = 0; (i < blocks.size()) && (bytes < maxlen); ++i) {
        const uint32_t block = blocks[i];
        const uint64_t pos64 = static_cast<uint64_t>(block) * smallBlockSz;
        uint32_t copied = 0;
        const uint32_t remaining = maxlen - bytes;
        if (!ReadMiniBlock(block, pos64, miniStreamSize, bigBlockSz,
            smallBlockSz, remaining, data + bytes, copied)) {
            return bytes;
        }
        bytes += copied;
    }
    return bytes;
}

uint32_t StorageIO::LoadSmallBlocks(const std::vector<uint32_t> &blocks, Byte *data, uint32_t maxlen)
{
    return ReadMiniBlocks(blocks, data, maxlen);
}

uint32_t StorageIO::LoadSmallBlock(uint32_t block, Byte *data, uint32_t maxlen)
{
    if (!data) {
        return 0;
    }
    std::vector<uint32_t> blocks;
    blocks.resize(1);
    blocks.assign(1, block);
    return LoadSmallBlocks(blocks, data, maxlen);
}

uint32_t StorageIO::SaveBlockToFile(uint64_t physicalOffset, const Byte *data, uint32_t len)
{
    if (!file_) {
        return 0;
    }
    const uint64_t maxStreamOff = static_cast<uint64_t>(std::numeric_limits<std::streamoff>::max());
    const std::streamoff offset = static_cast<std::streamoff>(physicalOffset);
    file_->clear();
    file_->seekp(offset);
    if (!file_->good()) {
        return 0;
    }
    file_->write(reinterpret_cast<const char *>(data), static_cast<std::streamsize>(len));
    if (!file_->good()) {
        return 0;
    }
    file_->flush();
    if (!file_->good()) {
        return 0;
    }
    if (auto *buf = file_->rdbuf()) {
        if (buf->pubsync() == -1) {
            return 0;
        }
    } else {
        return 0;
    }

    const auto endPos = file_->tellp();
    if (endPos == static_cast<std::streampos>(-1)) {
        return 0;
    }
    file_->seekg(endPos);
    if (!file_->good()) {
        return 0;
    }
    const uint64_t newEnd64 = physicalOffset + static_cast<uint64_t>(len);
    if (static_cast<std::streamoff>(newEnd64) > size_) {
        const uint64_t clamped = std::min<uint64_t>(newEnd64, maxStreamOff);
        size_ = static_cast<std::streamoff>(clamped);
    }
    return len;
}

uint32_t StorageIO::SaveBlockToBuffer(uint64_t physicalOffset, const Byte *data, uint32_t len,
    std::vector<uint8_t> &buffer)
{
    const uint64_t requiredSize = physicalOffset + static_cast<uint64_t>(len);
    if (requiredSize > buffer.max_size()) {
        return 0;
    }
    if (requiredSize > buffer.size()) {
        buffer.resize(static_cast<size_t>(requiredSize), 0);
    }
    const size_t offset = static_cast<size_t>(physicalOffset);
    auto ec = memcpy_s(buffer.data() + offset, buffer.size() - offset, data, len);
    if (ec != EOK) {
        return 0;
    }
    return len;
}

void StorageIO::ListDirectory(std::list<std::string> &result) const
{
    if (!dirtree_) {
        return;
    }
    std::vector<const DirEntry *> entries;
    dirtree_->ListDirectory(entries);
    for (size_t i = 0; i < entries.size(); i++) {
        if (entries[i]) {
            result.push_back(entries[i]->Name());
        }
    }
}

void StorageIO::ListEntries(std::vector<const DirEntry *> &result) const
{
    if (!dirtree_) {
        return;
    }
    dirtree_->ListDirectory(result);
}

DirEntry *StorageIO::Entry(const std::string &path, bool create)
{
    DirEntry *result = dirtree_ ? dirtree_->Entry(path, create) : nullptr;
    if (create && result) {
        dtModified_ = true;
    }
    return result;
}

DirEntry *StorageIO::GetRootEntry()
{
    return dirtree_ ? dirtree_->GetEntryAt(0) : nullptr;
}

uint32_t StorageIO::SaveBlock(uint64_t physicalOffset, const Byte *data, uint32_t len)
{
    if (!data || len == 0) {
        return 0;
    }
    const uint64_t maxStreamOff = static_cast<uint64_t>(std::numeric_limits<std::streamoff>::max());
    if (physicalOffset > maxStreamOff) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveBlock: physicalOffset overflow");
        return 0;
    }
    if (file_) {
        return SaveBlockToFile(physicalOffset, data, len);
    }
    if (flushWriteBuffer_) {
        return SaveBlockToBuffer(physicalOffset, data, len, *flushWriteBuffer_);
    }
    if (memoryBuffer_) {
        const uint32_t written = SaveBlockToBuffer(physicalOffset, data, len, *memoryBuffer_);
        if (written > 0) {
            const uint64_t requiredSize = physicalOffset + static_cast<uint64_t>(written);
            const std::streamoff newEnd = static_cast<std::streamoff>(requiredSize);
            if (newEnd > size_) {
                size_ = newEnd;
            }
        }
        return written;
    }
    return 0;
}

bool StorageIO::WriteBlockChecked(uint64_t physicalOffset, const Byte *data,
    uint32_t len, const std::string &context)
{
    const uint32_t written = SaveBlock(physicalOffset, data, len);
    if (written != len) {
        if (error_ == ErrorCode::Ok) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::WriteBlockChecked - Failed to write block");
        }
        const std::string &errMessage = !errorMsg_.empty()
            ? errorMsg_
            : (context.empty() ? std::string("Write Failed") : context);
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::WriteBlockChecked - Write Failed");
        return false;
    }
    return true;
}

bool StorageIO::ValidateConvertInputs(DirEntry *entry, uint64_t newSize, uint64_t &oldSize, uint32_t &blockSize,
    uint32_t &threshold)
{
    if (!entry || !bbat_ || !sbat_ || !header_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateConvertInputs - Invalid inputs");
        return false;
    }
    oldSize = entry->Size();
    threshold = header_->Threshold();
    if (oldSize >= threshold) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateConvertInputs - Entry size exceeds threshold");
        return false;
    }
    if (newSize < threshold || newSize < oldSize) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateConvertInputs - New size is smaller than threshold");
        return false;
    }
    blockSize = BigBlockSize();
    if (blockSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateConvertInputs - Invalid block size");
        return false;
    }
    return true;
}

bool StorageIO::ReadMiniStream(DirEntry *entry, uint64_t oldSize, std::vector<uint32_t> &oldBlocks,
    std::vector<Byte> &smallBuffer)
{
    if (!entry) {
        return false;
    }
    if (!FollowSmallBlockTable(entry->Start(), oldBlocks)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniStream - Failed to follow small block table");
        return false;
    }
    if (oldSize > 0 && oldBlocks.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniStream - Failed to read old blocks");
        return false;
    }
    if (oldSize == 0) {
        return true;
    }
    smallBuffer.resize(static_cast<size_t>(oldSize));
    const uint32_t bytesRead = LoadSmallBlocks(oldBlocks, smallBuffer.data(), static_cast<uint32_t>(oldSize));
    if (bytesRead < static_cast<uint32_t>(oldSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadMiniStream - Failed to read old blocks");
        return false;
    }
    return true;
}

bool StorageIO::ComputeBlocksNeeded(uint64_t newSize, uint32_t blockSize, uint32_t &blocksNeeded)
{
    if (blockSize == 0) {
        return false;
    }
    uint64_t blocksNeeded64 = (newSize + blockSize - 1) / blockSize;
    if (blocksNeeded64 == 0) {
        blocksNeeded64 = 1;
    }
    if (blocksNeeded64 > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ComputeBlocksNeeded - blocksNeeded overflow");
        return false;
    }
    blocksNeeded = static_cast<uint32_t>(blocksNeeded64);
    return true;
}

bool StorageIO::AllocateBigBlocksForConvert(uint32_t blocksNeeded, std::vector<uint32_t> &newBlocks)
{
    if (!bbat_) {
        return false;
    }
    newBlocks.reserve(static_cast<size_t>(blocksNeeded));
    for (uint32_t i = 0; i < blocksNeeded; i++) {
        const uint32_t newBlock = static_cast<uint32_t>(bbat_->Unused());
        newBlocks.push_back(newBlock);
        bbat_->Set(newBlock, AllocTable::Eof);
    }

    if (newBlocks.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::AllocateBigBlocksForConvert - Failed to allocate big blocks");
        return false;
    }
    bbat_->SetChain(newBlocks);
    return true;
}

namespace {

std::vector<Byte> BuildBatchBuffer(uint64_t &batchStartIdx, uint64_t &batchEndIdx, uint64_t newSize,
    uint32_t blockSize, const std::vector<Byte> &smallBuffer, size_t &copied, uint32_t &batchWriteLen,
    uint64_t &lastWrittenIdx)
{
    std::vector<Byte> batchBuffer;
    for (size_t idx = batchStartIdx; idx <= batchEndIdx; ++idx) {
        const uint64_t offsetInStream = idx * static_cast<uint64_t>(blockSize);
        uint64_t remaining = newSize > offsetInStream ? newSize - offsetInStream : 0;
        const uint32_t chunk = static_cast<uint32_t>(std::min<uint64_t>(remaining, blockSize));
        if (chunk == 0) {
            exhausted = true;
            break;
        }
        const size_t blockOffset = (idx - batchStartIdx) * static_cast<size_t>(blockSize);
        const size_t required = blockOffset + static_cast<size_t>(chunk);
        if (required > batchBuffer.size()) {
            batchBuffer.resize(required, 0);
        }
        const size_t remainingOldData = copied < smallBuffer.size() ? smallBuffer.size() - copied : 0;
        const size_t dataToWrite = std::min(static_cast<size_t>(chunk), remainingOldData);
        if (dataToWrite > 0) {
            auto ec = memcpy_s(batchBuffer.data() + blockOffset, batchBuffer.size() - blockOffset,
                smallBuffer.data() + copied, dataToWrite);
            if (ec != EOK) {
                return false;
            }
            copied += dataToWrite;
        }
        batchWriteLen += static_cast<uint32_t>(required);
        lastWrittenIdx = idx;
    }
    return batchBuffer;
}
}

bool StorageIO::CopyDataToBigBlocks(const std::vector<uint32_t> &blocks, const std::vector<Byte> &smallBuffer,
    uint64_t newSize, uint32_t blockSize)
{
    size_t copied = 0;
    size_t batchStartIdx = 0;
    bool exhausted = false;
    while (batchStartIdx < blocks.size() && !exhausted) {
        size_t batchEndIdx = batchStartIdx;
        while (batchEndIdx + 1 < blocks.size() && blocks[batchEndIdx + 1] == blocks[batchEndIdx] + 1) {
            ++batchEndIdx;
        }
        const size_t contiguousCount = batchEndIdx - batchStartIdx + 1;
        if (blockSize == 0) {
            return false;
        }
        const size_t maxBatchBlocks = std::max<size_t>(1, std::numeric_limits<uint32_t>::max() / blockSize);
        const size_t batchCount = std::min<size_t>(contiguousCount, maxBatchBlocks);
        if (batchStartIdx + batchCount - 1 < batchEndIdx) {
            batchEndIdx = batchStartIdx + batchCount - 1;
        }
        std::vector<Byte> batchBuffer;
        batchBuffer.reserve(batchCount * static_cast<size_t>(blockSize));
        uint32_t batchWriteLen = 0;
        size_t lastWrittenIdx = batchStartIdx;

        batchBuffer = BuildBatchBuffer(batchStartIdx, batchEndIdx, newSize, blockSize, smallBuffer,
            copied, batchWriteLen, lastWrittenIdx);
        if (batchBuffer.empty()) {
            return false;
        }
        
        if (batchWriteLen > 0) {
            const uint64_t physicalOffset = BlockToOffset(blocks[batchStartIdx], blockSize);
            const std::string context = "converted blocks " + std::to_string(blocks[batchStartIdx]) + "-" +
                std::to_string(blocks[lastWrittenIdx]);
            if (!WriteBlockChecked(physicalOffset, batchBuffer.data(), batchWriteLen, context)) {
                return false;
            }
        }
        batchStartIdx = exhausted ? blocks.size() : batchEndIdx + 1;
    }
    return true;
}

void StorageIO::ReleaseMiniBlocks(const std::vector<uint32_t> &oldBlocks)
{
    if (!sbat_) {
        return;
    }
    for (uint32_t block : oldBlocks) {
        sbat_->Set(block, AllocTable::Avail);
    }
}

bool StorageIO::ConvertSmallToBig(DirEntry *entry, uint64_t newSize)
{
    uint64_t oldSize = 0;
    uint32_t blockSize = 0;
    uint32_t threshold = 0;

    if (!ValidateConvertInputs(entry, newSize, oldSize, blockSize, threshold)) {
        return false;
    }
    std::vector<uint32_t> oldBlocks;
    std::vector<Byte> smallBuffer;
    if (!ReadMiniStream(entry, oldSize, oldBlocks, smallBuffer)) {
        return false;
    }
    uint32_t blocksNeeded = 0;
    if (!ComputeBlocksNeeded(newSize, blockSize, blocksNeeded)) {
        return false;
    }
    std::vector<uint32_t> newBlocks;
    if (!AllocateBigBlocksForConvert(blocksNeeded, newBlocks)) {
        return false;
    }
    if (!bbat_) {
        return false;
    }
    auto releaseNewBlocks = [&]() {
        for (uint32_t block : newBlocks) {
            bbat_->Set(block, AllocTable::Avail);
        }
    };
    if (!CopyDataToBigBlocks(newBlocks, smallBuffer, newSize, blockSize)) {
        releaseNewBlocks();
        return false;
    }
    if (!entry) {
        return false;
    }
    ReleaseMiniBlocks(oldBlocks);
    entry->SetStart(newBlocks.front());
    entry->SetSize(newSize);
    dtModified_ = true;
    ClearError();
    return true;
}

bool StorageIO::ValidateFlushState(bool &memoryMode, size_t &blockSize)
{
    if (!IsValid()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateFlushState - Object is in invalid state");
        return false;
    }
    if (!header_ || !bbat_ || !dirtree_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateFlushState - Alloc tables are not initialized");
        return false;
    }
    memoryMode = (memoryBuffer_ != nullptr && file_ == nullptr);
    blockSize = static_cast<size_t>(BigBlockSize());
    if (blockSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateFlushState - Block size is 0");
        return false;
    }
    return true;
}

bool StorageIO::PrepareDirectoryBlocks(size_t blockSize, size_t dirEntries, std::vector<uint32_t> &blocks,
    size_t &neededBlocks)
{
    if (blockSize == 0) {
        return false;
    }
    const size_t neededBytes = dirEntries * kDirectoryEntrySize;
    neededBlocks = (neededBytes + blockSize - 1) / blockSize;
    if (neededBlocks == 0) {
        neededBlocks = 1;
    }
    if (!bbat_ || !header_) {
        return false;
    }
    if (!bbat_->Follow(header_->DirentStart(), blocks)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::PrepareDirectoryBlocks - Failed to follow dirent start");
        return false;
    }
    for (uint32_t block : blocks) {
        if ((*bbat_)[block] == AllocTable::Avail) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::PrepareDirectoryBlocks - Block is not available");
            return false;
        }
    }
    return true;
}

void StorageIO::BackupFlushState(FlushSnapshot &snap)
{
    if (header_ == nullptr) {
        return;
    }
    snap.headerBackup = *header_;
    snap.fatSectorsBackup = fatSectors_;
    snap.difatSectorsBackup = difatSectors_;
    snap.protectedBlocksCacheBackup = protectedBlocksCache_;
    snap.protectedBlocksDirtyBackup = protectedBlocksDirty_;
    snap.sbBlocksBackup = sbBlocks_;
    snap.sizeBackup = size_;
    snap.prevFlushWriteBuffer = flushWriteBuffer_;
    if (bbat_ == nullptr) {
        return;
    }
    snap.bbatSnapshot.reserve(bbat_->Count());
    for (size_t i = 0; i < bbat_->Count(); i++) {
        snap.bbatSnapshot.push_back((*bbat_)[i]);
    }
}

void StorageIO::RestoreFlushState(const FlushSnapshot &snap)
{
    flushWriteBuffer_ = snap.prevFlushWriteBuffer;
    if (bbat_) {
        bbat_->Resize(snap.bbatSnapshot.size());
        for (size_t i = 0; i < snap.bbatSnapshot.size(); i++) {
            bbat_->Set(i, snap.bbatSnapshot[i]);
        }
    }
    fatSectors_ = snap.fatSectorsBackup;
    difatSectors_ = snap.difatSectorsBackup;
    protectedBlocksCache_ = snap.protectedBlocksCacheBackup;
    protectedBlocksDirty_ = snap.protectedBlocksDirtyBackup;
    sbBlocks_ = snap.sbBlocksBackup;
    size_ = snap.sizeBackup;
    if (header_) {
        *header_ = snap.headerBackup;
    }
}

bool StorageIO::SetupStagingBuffer(bool memoryMode, FlushSnapshot &snap)
{
    if (memoryMode && memoryBuffer_) {
        snap.stagingMemory = *memoryBuffer_;
        flushWriteBuffer_ = &snap.stagingMemory;
    }
    return true;
}

bool StorageIO::ExecuteFlushSequence(std::vector<uint32_t> &blocks, size_t neededBlocks, size_t blockSize)
{
    if (!FlushDirectoryTree(blocks, neededBlocks, blockSize)) {
        return false;
    }
    if (!FlushMiniFat()) {
        return false;
    }
    if (!FlushFatChain()) {
        return false;
    }
    if (!FlushDifatChain()) {
        return false;
    }
    return FlushHeader();
}

bool StorageIO::FinalizeFlush(bool memoryMode, FlushSnapshot &snap, size_t blockSize)
{
    size_t maxUsedBlock = 0;
    if (bbat_ == nullptr) {
        return false;
    }
    for (size_t i = 0; i < bbat_->Count(); i++) {
        if ((*bbat_)[i] != AllocTable::Avail) {
            maxUsedBlock = i;
        }
    }
    const size_t computedSize = (maxUsedBlock + 2) * blockSize;
    const std::streamoff computedSizeOff = static_cast<std::streamoff>(
        std::min<size_t>(computedSize, static_cast<size_t>(std::numeric_limits<std::streamoff>::max())));

    if (file_) {
        file_->seekg(0, std::ios::end);
        size_ = file_->tellg();
    } else if (memoryMode && memoryBuffer_) {
        size_ = computedSizeOff;
        if (difatSectors_.empty() && snap.stagingMemory.size() != computedSize) {
            snap.stagingMemory.resize(computedSize, 0);
        }
        auto newBuffer  = std::make_unique<std::vector<uint8_t>>(std::move(snap.stagingMemory));
        memoryBuffer_.swap(newBuffer);
    } else if (memoryBuffer_) {
        size_ = computedSizeOff;
        if (difatSectors_.empty() && memoryBuffer_->size() != computedSize) {
            memoryBuffer_->resize(computedSize, 0);
        }
    }

    flushWriteBuffer_ = nullptr;
    return true;
}

bool StorageIO::ValidateDeletePreconditions(uint32_t &threshold, uint32_t &miniBlockSize, uint32_t &bigBlockSize)
{
    if (!IsValid()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateDeletePreconditions - Storage is not valid");
        return false;
    }
    if (!dirtree_ || !header_ || !bbat_ || !sbat_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateDeletePreconditions - Storage is not valid");
        return false;
    }
    threshold = header_->Threshold();
    miniBlockSize = SmallBlockSize();
    bigBlockSize = BigBlockSize();
    if (bigBlockSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateDeletePreconditions - Invalid block size");
        return false;
    }
    return true;
}

bool StorageIO::CollectDeleteTargets(const std::string &path, std::vector<DirEntry> &targets)
{
    if (!dirtree_) {
        return false;
    }
    if (dirtree_->CollectSubtreeEntries(path, targets)) {
        return true;
    }
    OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
        "StorageIO::CollectDeleteTargets: Invalid path");
    return false;
}

bool StorageIO::CollectChainForEntry(const DirEntry &entry, bool useBig, uint32_t miniblockSize,
    std::vector<uint32_t> &out)
{
    AllocTable *table = useBig ? bbat_.get() : sbat_.get();
    if (table == nullptr) {
        SetError(ErrorCode::InvalidOperation, "Cannot delete: storage not initialized", false);
        return false;
    }
    const uint32_t start = entry.Start();
    if (start == DIR_ENTRY_END || start == AllocTable::Eof || start == AllocTable::Avail ||
        start == AllocTable::Bat || start == AllocTable::MetaBat) {
        return true;
    }
    if (start >= table->Count()) {
        SetError(ErrorCode::Corruption, "Cannot delete: start block out of range");
        return false;
    }
    if ((*table)[start] == AllocTable::Avail) {
        return true;
    }
    if (!table->Follow(start, out)) {
        SetError(ErrorCode::Corruption, "Cannot delete: failed to follow allocation chain");
        return false;
    }
    if (out.empty()) {
        return true;
    }
    const uint32_t blockSize = useBig ? bigBlockSize : miniBlockSize;
    if (blockSize == 0) {
        SetError(ErrorCode::InvalidOperation, "Cannot delete: block size is 0", false);
        return false;
    }
    if (entry.Size() && (static_cast<uint64_t>(out.size()) * blockSize) < entry.Size()) {
        SetError(ErrorCode::Corruption, "Cannot delete: allocation chain is too short");
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::CollectChainForEntry - allocation chain is too short");
        return false;
    }
    if (useBig) {
        for (uint32_t block : out) {
            if (protectedBlocksCache_.count(block)) {
                SetError(ErrorCode::InvalidOperation, "Cannot delete: protected block", false);
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                    "StorageIO::CollectChainForEntry - protected block");
                return false;
            }
        }
    }
    return true;
}

bool StorageIO::BuildReleasePlan(const std::vector<DirEntry> &targets,
    uint32_t threshold, uint32_t miniblockSize, uint32_t bigBlockSize,
    std::vector<std::pair<bool, std::vector<uint32_t>>> &plan)
{
    plan.reserve(targets.size());
    for (const DirEntry &entry : targets) {
        if (!entry.IsFile() || entry.Size() == 0) {
            continue;
        }
        const bool useBig = !(entry.Size() < threshold);
        std::vector<uint32_t> blocks;
        if (!CollectChainForEntry(entry, useBig, miniBlockSize, bigBlockSize, blocks)) {
            return false;
        }
        if (!blocks.empty()) {
            plan.emplace_back(useBig, std::move(blocks));
        }
    }
    return true;
}

void StorageIO::ExecuteBlockRelease(const std::vector<std::pair<bool, std::vector<uint32_t>>> &plan)
{
    for (const auto &item : plan) {
        AllocTable *table = item.first ? bbat_.get() : sbat_.get();
        for (uint32_t block : item.second) {
            if (table) {
                table->Set(block, AllocTable::Avail);
            }
        }
    }
}

bool StorageIO::DeleteEntry(const std::string &path)
{
    uint32_t threshold = 0;
    uint32_t miniBlockSize = 0;
    uint32_t bigBlockSize = 0;
    if (!ValidateDeletePreconditions(threshold, miniBlockSize, bigBlockSize)) {
        return false;
    }
    std::vector<DirEntry> targets;
    if (!CollectDeleteTargets(path, targets)) {
        return false;
    }
    RebuildProtectedBlocksCache();

    std::vector<std::pair<bool, std::vector<uint32_t>>> plan;
    if (!BuildReleasePlan(targets, threshold, miniBlockSize, bigBlockSize, plan)) {
        return false;
    }
    if (dirtree_ == nullptr) {
        return false;
    }
    if (!dirtree_->DeleteEntry(path)) {
        SetError(ErrorCode::InvalidOperation, "Cannot delete: failed to delete entry", false);
        return false;
    }
    ExecuteBlockRelease(plan);
    dtModified_ = true;
    return Flush();
}

bool StorageIO::ValidateExtendPreconditions(DirEntry *entry)
{
    if (!IsValid()) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: storage not initialized", false);
        return false;
    }

    if (!entry || !bbat_ || !header_ || !sbat_) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: entry or alloc table is null", false);
        return false;
    }
    return true;
}

bool StorageIO::NeedsBigBlocks(uint64_t size) const
{
    return header_ && size >= header_->Threshold();
}

bool StorageIO::ValidateAndFetchExistingChain(DirEntry *entry, bool useBig,
    std::vector<uint32_t> &existingBlocks)
{
    if (entry == nullptr) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: entry is null", false);
        return false;
    }
    if (useBig) {
        if (!FollowBigBlockTable(entry->Start(), existingBlocks)) {
            SetError(ErrorCode::Corruption,
                "Cannot extend: failed to follow big block chain");
            return false;
        }
        return true;
    }
    if (!FollowSmallBlockTable(entry->Start(), existingBlocks)) {
        SetError(ErrorCode::Corruption,
            "Cannot extend: failed to follow small block chain");
        return false;
    }
    return true;
}

bool StorageIO::EnsureRootForMiniGrowth(uint32_t requiredMiniBlocks, const std:;string &logContext)
{
    if (ExtendRootStream(requiredMiniBlocks)) {
        return true;
    }
    SetError(ErrorCode::Corruption, "Cannot extend root stream for mini growth");
    OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
        "StorageIO::EnsureRootForMiniGrowth: failed to extend root stream for mini growth");
    return false;
}

bool StorageIO::ExtendSameTypeChain(DirEntry *entry, uint64_t oldSize, uint32_t newSize, bool useBig)
{
    const uint32_t blockSize = useBig ? BigBlockSize() : SmallBlockSize();
    AllocTable *allocTable = useBig ? bbat_.get() : sbat_.get();
    if (blockSize == 0 || allocTable == nullptr) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: alloc table is null", false);
        return false;
    }
    const uint64_t blockSize64 = blockSize;
    const uint64_t oldBlocksNeeded64 = (oldSize + blockSize64 - 1) / blockSize64;
    const uint64_t newBlocksNeeded64 = (newSize + blockSize64 - 1) / blockSize64;
    const uint64_t maxBlocks = static_cast<uint64_t>(std::numeric_limits<uint32_t>::max());
    if (oldBlocksNeeded64 > maxBlocks || newBlocksNeeded64 > maxBlocks) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: requested size exceeds FAT addressing range");
        return false;
    }
    const uint32_t oldBlocksNeeded = static_cast<uint32_t>(oldBlocksNeeded64);
    const uint32_t newBlocksNeeded = static_cast<uint32_t>(newBlocksNeeded64);
    if (entry == nullptr) {
        return false;
    }
    if (newBlocksNeeded <= oldBlocksNeeded) {
        entry->SetSize(newSize);
        dtModified_ = true;
        ClearError();
        return true;
    }

    std::vector<uint32_t> existingBlocks;
    if (!ValidateAndFetchExistingChain(entry, useBig, existingBlocks)) {
        return false;
    }
    if (existingBlocks.empty()) {
        SetError(ErrorCode::Corruption, "Cannot extend: existing block chain is empty");
        return false;
    }
    if (existingBlocks.size() < oldBlocksNeeded) {
        SetError(ErrorCode::Corruption, "Cannot extend: existing block chain is shorter than expected (expected " +
            std::to_string(oldBlocksNeeded) + ", got " + std::to_string(existingBlocks.size()) + ")");
        return false;
    }
    const uint32_t additionalBlocks = newBlocksNeeded - oldBlocksNeeded;

    if (!useBig) {
        uint64_t highestMiniBlock = 0;
        if (sbat_ == nullptr) {
            return false;
        }
        for (size_t i = 0; i < sbat_->Count(); ++i) {
            if ((*sbat_)[i] != AllocTable::Avail) {
                highestMiniBlock = i + 1;
            }
        }
        highestMiniBlock += additionalBlocks;
        if (highestMiniBlock > std::numeric_limits<uint32_t>::max()) {
            SetError(ErrorCode::InvalidOperation, "Cannot extend: block count overflow");
            return false;
        }
        if (!EnsureRootForMiniGrowth(static_cast<uint32_t>(highestMiniBlock),
            "Failed to extend root stream when growing small stream")) {
            return false;
        }
    }

    for (uint32_t i = 0; i < additionalBlocks; ++i) {
        const uint32_t newBlock = allocTable->Unused();
        allocTable->Set(existingBlocks.back(), newBlock);
        allocTable->Set(newBlock, AllocTable::Eof);
        existingBlocks.push_back(newBlock);
    }
    entry->SetSize(newSize);
    dtModified_ = true;
    ClearError();
    return true;
}

bool StorageIO::AllocateFreshChain(DirEntry *entry, uint64_t newSize, bool useBig)
{
    const uint32_t blockSize = useBig ? BigBlockSize() : SmallBlockSize();
    AllocTable *allocTable = useBig ? bbat_.get() : sbat_.get();
    if (blockSize == 0 || allocTable == nullptr) {
        SetError(ErrorCode::InvalidOperation, "Cannot extend: invalid block size", false);
        return false;
    }
    uint64_t blocksNeeded64 = (newSize + blockSize - 1) / blockSize;
    if (blocksNeeded64 == 0)
        blocksNeeded64 = 1;
    if (blocksNeeded64 > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) {
        SetError(ErrorCode::InvalidOperation, "Cannot allocate: block count overflow");
        return false;
    }
    const uint32_t blocksNeeded = static_cast<uint32_t>(blocksNeeded64);

    if (!useBig) {
        uint64_t totalMiniBlocks = 0;
        for (size_t i = 0; i < sbat_->Count(); ++i)
            if ((*sbat_)[i] != AllocTable::Avail)
                totalMiniBlocks = i + 1;
        totalMiniBlocks += blocksNeeded;
        if (totalMiniBlocks > std::numeric_limits<uint32_t>::max()) {
            SetError(ErrorCode::InvalidOperation, "Cannot extend: block count overflow");
            return false;
        }
        if (!EnsureRootForMiniGrowth(static_cast<uint32_t>(totalMiniBlocks),
            "Failed to extend root stream for " + std::to_string(blocksNeeded) + " mini blocks")) {
            return false;
        }
    }
    std::vector<uint32_t> blocks;
    blocks.reserve(blocksNeeded);
    for (uint32_t i = 0; i < blocksNeeded; ++i) {
        const uint32_t block = allocTable->Unused();
        blocks.push_back(block);
        allocTable->Set(block, AllocTable::Eof);
    }
    allocTable->SetChain(blocks);
    if (blocks.empty()) {
        SetError(ErrorCode::AllocationFailed, "Cannot allocate: no more free blocks");
        return false;
    }
    if (entry == nullptr)
        return false;
    entry->SetStart(blocks[0]);
    entry->SetSize(newSize);
    dtModified_ = true;
    ClearError();
    return true;
}

bool StorageIO::ExtendEntry(DirEntry *entry, uint64_t newSize)
{
    if (!ValidateExtendPreconditions(entry)) {
        return false;
    }
    const uint64_t oldSize = entry->Size();
    const bool isFirstAllocation = (oldSize == 0);
    const bool oldUseBigBlocks = NeedsBigBlocks(oldSize);
    const bool newUseBigBlocks = NeedsBigBlocks(newSize);
    if (!isFirstAllocation && (oldUseBigBlocks != newUseBigBlocks)) {
        if (!oldUseBigBlocks && newUseBigBlocks) {
            const bool converted = ConvertSmallToBig(entry, newSize);
            if (converted) {
                ClearError();
            }
            return converted;
        }
        SetError(ErrorCode::InvalidOperation, "Cannot extend: cannot switch between big and small blocks", false);
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "BIG to SMALL is not supported");
        return false;
    }
    if (!isFirstAllocation)
        return ExtendSameTypeChain(entry, oldSize, newSize, newUseBigBlocks);
    return AllocateFreshChain(entry, newSize, newUseBigBlocks);
}

namespace {

void AddBlocksFromChain(StorageIO *self, uint32_t start)
{
    std::vector<uint32_t> chain;
    if (self->bbat_->Follow(start, chain)) {
        for (uint32_t block : chain) {
            self->protectedBlocksCache_.insert(block);
        }
    }
}

}

void StorageIO::RebuildProtectedBlocksCache()
{
    if (!protectedBlocksDirty_)
        return;
    std::set<uint32_t> carry = protectedBlocksCache_;
    protectedBlocksCache_.clear();
    if (!bbat_) {
        protectedBlocksDirty_ = false;
        return;
    }

    if (header_) {
        AddBlocksFromChain(this, header_->DirentStart());
    }
    
    for (uint32_t fatSector : fatSectors_) {
        protectedBlocksCache_.insert(fatSector);
    }

    for (uint32_t difatSector : difatSectors_) {
        protectedBlocksCache_.insert(difatSector);
    }

    if (header_ && header_->SbatStart() != AllocTable::Eof) {
        AddBlocksFromChain(this, header_->SbatStart());
    }

    if (header_) {
        DirEntry *root = GetRootEntry();
        if (root && root->Start() != DIR_ENTRY_END) {
            AddBlocksFromChain(this, root->Start());
        }
    }

    protectedBlocksCache_.insert(carry.begin(), carry.end());
    protectedBlocksDirty_ = false;
}

uint32_t StorageIO::AllocateProtectedSector(uint32_t markerType)
{
    if (!bbat_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "BBAT is not initialized");
        return AllocTable::Eof;
    }

    RebuildProtectedBlocksCache();
    std::vector<std::pair<uint32_t, uint32_t>> tempMarks;
    uint32_t candidate = static_cast<uint32_t>(bbat_->Unused());
    while (protectedBlocksCache_.count(candidate) > 0) {
        tempMarks.emplace_back(candidate, (*bbat_)[candidate]);
        bbat_->Set(candidate, markerType);
        candidate = static_cast<uint32_t>(bbat_->Unused());
    }
    for (const auto &[block, originalVal] : tempMarks) {
        bbat_->Set(block, originalVal);
    }
    return candidate;
}

namespace {
bool AdvertiseFATSectorInDIFAT(StorageIO *storage, uint32_t fatSector, uint32_t current, uint32_t difatEntries)
{
    if (difatEntries == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "DIFAT entries per sector is 0");
        return false;
    }
    if (current < HEADER_DIFAT_ARRAY_SIZE) {
        storage->header_->SetDifatElem(current, static_cast<uint32_t>(fatSector));
        return true;
    }
    const uint32_t difatIndex = current - HEADER_DIFAT_ARRAY_SIZE;
    const uint32_t difatSectorIndex = difatIndex / difatEntries;
    while (storage->difatSectors_.size() <= difatSectorIndex) {
        uint32_t newDifat = AllocateProtectedSector(AllocTable::MetaBat);
        if (newDifat == AllocTable::Eof) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to allocate new DIFAT sector");
            return false;
        }
        storage->difatSectors_.push_back(newDifat);
        storage->bbat_->Set(newDifat, AllocTable::MetaBat);
        storage->protectedBlocksDirty_ = true;

        if (storage->difatSectors_.size() == 1) {
            storage->header_->SetFirstDifat(newDifat);
        }
    }
    return true;
}
}

void StorageIO::EnsureFatCapacity()
{
    if (!bbat_ || !header_) {
        return;
    }

    const uint32_t sectorSize = BigBlockSize();
    if (sectorSize == 0) {
        return;
    }
    const uint32_t entriesPerSector = sectorSize / static_cast<uint32_t>(sizeof(uint32_t));
    if (entriesPerSector == 0) {
        return;
    }
    const uint32_t difatEntries = entriesPerSector > 0 ? static_cast<uint32_t>(entriesPerSector - 1) : 0;
    while (true) {
        const uint32_t entryCount = static_cast<uint32_t>(bbat_->Count());
        uint32_t required = (entryCount + entriesPerSector - 1) / entriesPerSector;
        if (required == 0) {
            required = 1;
        }
        if (required <= fatSectors_.size()) {
            break;
        }

        const uint32_t current = static_cast<uint32_t>(fatSectors_.size());
        uint32_t newFat = AllocateProtectedSector(AllocTable::Bat);
        if (newFat == AllocTable::Eof) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to allocate new FAT sector");
            return;
        }
        fatSectors_.push_back(newFat);
        bbat_->Set(newFat, AllocTable::Bat);
        protectedBlocksDirty_ = true;

        if (!AdvertiseFATSectorInDIFAT(this, newFat, current, difatEntries)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to advertise new FAT sector in DIFAT");
            return;
        }
    }
    header_->SetFatSectorCount(static_cast<uint32_t>(fatSectors_.size()));
    header_->SetDifatCount(static_cast<uint32_t>(difatSectors_.size()));
}

namespace {

bool ExtendChainWithZeros(StorageIO *self, std::vector<uint32_t> &chain,
    uint32_t &neededBigBlocks, uint32_t &bigBlocks)
{
    while (chain.size() < neededBigBlocks) {
        uint32_t newBlock = self->bbat_->Unused();
        if (!chain.empty()) {
            self->bbat_->Set(chain.back(), newBlock);
        }
        self->bbat_->Set(newBlock, AllocTable::Eof);
        chain.push_back(newBlock);
        self->protectedBlocksDirty_ = true;

        std::vector<Byte> zeroBuf(static_cast<size_t>(bigBlockSize), 0);
        const uint64_t physicalOffset = self->BlockToOffset(newBlock, bigBlockSize);
        const std::string context = "Root stream block " + std::to_string(newBlock);
        if (!self->WriteBlockChecked(physicalOffset, zeroBuf.data(), bigBlockSize, context)) {
            if (!chain.empty()) {
                chain.pop_back();
            }
            self->bbat_->Set(newBlock, AllocTable::Avail);
            if (!chain.empty()) {
                self->bbat_->Set(chain.back(), AllocTable::Eof);
            }
            return false;
        }
    }
    return true;
}
}

bool StorageIO::ExtendRootStream(uint32_t requiredMiniBlocks)
{
    if (!bbat_ || !header_)
        return false;
    DirEntry *root = GetRootEntry();
    if (!root)
        return false;
    
    const uint32_t bigBlockSize = BigBlockSize();
    const uint32_t smallBlockSize = SmallBlockSize();
    if (smallBlockSize == 0 || bigBlockSize == 0)
        return false;
    const uint32_t miniBlocksPerBigBlock = bigBlockSize / smallBlockSize;
    if (miniBlocksPerBigBlock == 0) {
        SetError(ErrorCode::InvalidOperation, "Big block size is not divisible by small block size", false);
        return false;
    }
    const uint64_t neededBigBlocks64 = (static_cast<uint64_t>(requiredMiniBlocks) +
        static_cast<uint64_t>(miniBlocksPerBigBlock) - 1) / static_cast<uint64_t>(miniBlocksPerBigBlock);
    if (neededBigBlocks64 > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) {
        SetError(ErrorCode::InvalidOperation, "Required big blocks count exceeds uint32_t max value");
        return false;
    }
    const uint32_t neededBigBlocks = static_cast<uint32_t>(neededBigBlocks64);
    std::vector<uint32_t> chain;
    const uint32_t rootStart = root->Start();
    if (rootStart != DIR_ENTRY_END && rootStart != AllocTable::Eof) {
        if (!bbat_->Follow(root->Start(), chain))
            return false;
    }

    if (chain.size() < neededBigBlocks) {
        if (!ExtendChainWithZeros(this, chain, neededBigBlocks, bigBlockSize))
            return false;
    }

    if ((rootStart == DIR_ENTRY_END || rootStart == AllocTable::Eof) && !chain.empty())
        root->SetStart(chain.front());

    const uint64_t miniStreamSize = static_cast<uint64_t>(requiredMiniBlocks) *
        static_cast<uint64_t>(smallBlockSize);
    root->SetSize(miniStreamSize);
    sbBlocks_ = chain;
    dtModified_ = true;
    return true;
}

namespace {
bool ExtendAndFetchSbatChain(StorageIO *self, std::vector<uint32_t> &chain, uint32_t &neededBlocks)
{
    uint32_t currentStart = self->header_->SbatStart();
    if (currentStart != AllocTable::Eof && currentStart != AllocTable::Avail) {
        if (!self->bbat_->Follow(currentStart, chain)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to follow SBAT chain");
            return false;
        }
    }
    while (chain.size() < neededBlocks) {
        uint32_t newBlock = static_cast<uint32_t>(self->bbat_->Unused());
        if (!chain.empty()) {
            self->bbat_->Set(chain.back(), newBlock);
        }
        self->bbat_->Set(newBlock, AllocTable::Eof);
        chain.push_back(newBlock);
        self->protectedBlocksDirty_ = true;
    }
    if (chain.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to extend SBAT chain");
        return false;
    }
    return true;
}
}

bool StorageIO::SaveMiniFat()
{
    if (!sbat_ || !bbat_ || !header_)
        return false;
    size_t highestUsed = 0;
    for (size_t i = 0; i < sbat_->Count(); ++i) {
        if ((*sbat_)[i] != AllocTable::Avail) {
            highestUsed = i + 1;
        }
    }
    if (highestUsed == 0) {
        header_->SetFirstMiniFat(AllocTable::Eof);
        header_->SetMiniFatCount(0);
        protectedBlocksDirty_ = true;
        return true;
    }
    
    const size_t entryCount = sbat_->Count();
    const uint32_t blockSize = BigBlockSize();
    if (blockSize == 0) {
        return false;
    }
    const size_t bytesNeeded = entryCount * sizeof(uint32_t);
    uint32_t neededBlocks = static_cast<uint32_t>((bytesNeeded + blockSize - 1) / blockSize);
    if (neededBlocks == 0) {
        neededBlocks = 1;
    }
    std::vector<uint32_t> chain;

    if (!ExtendAndFetchSbatChain(this, chain, neededBlocks)) {
        return false;
    }
    const size_t buflen = chain.size() * blockSize;
    std::vector<Byte> buffer(buflen, 0xFF);
    if (!sbat_->Save(buffer.data(), buflen)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Failed to save MiniFAT");
        return false;
    }
    for (size_t i = 0; i < chain.size(); ++i) {
        const uint32_t block = chain[i];
        const uint64_t physicalOffset = BlockToOffset(block, blockSize);
        const std::string context = "MiniFAT sector " + std::to_string(block);
        if (!WriteBlockChecked(physicalOffset, buffer.data() + (i * blockSize), blockSize, context)) {
            return false;
        }
    }
    header_->SetFirstMiniFat(chain.front());
    header_->SetMiniFatCount(static_cast<BlockCount>(chain.size()));
    return true;
}

bool StorageIO::SaveDifat()
{
    if (!header_) {
        return false;
    }
    const uint32_t sectorSize = BigBlockSize();
    if (sectorSize == 0) {
        return false;
    }
    const uint32_t entriesPerSector = sectorSize / static_cast<uint32_t>(sizeof(uint32_t));
    if (entriesPerSector == 0) {
        return false;
    }
    const uint32_t difatEntries = entriesPerSector > 0 ? static_cast<uint32_t>(entriesPerSector - 1) : 0;
    const size_t headerCount  = std::min<size_t>(fatSectors_.size(), HEADER_DIFAT_ARRAY_SIZE);
    for (size_t i = 0; i < headerCount; ++i) {
        header_->SetDifatElem(i, static_cast<uint32_t>(fatSectors_[i]));
    }
    for (size_t i = headerCount; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        header_->SetDifatElem(i, AllocTable::Avail);
    }
    if (!difatSectors_.empty()) {
        for (size_t chainIdx = 0; chainIdx < difatSectors_.size(); ++chainIdx) {
            const uint32_t difatSector = difatSectors_[chainIdx];
            std::vector<Byte> buf(static_cast<size_t>(sectorSize), 0xFF);
            const size_t baseFatIndex = HEADER_DIFAT_ARRAY_SIZE + static_cast<size_t>(chainIdx * difatEntries);
            for (uint32_t i = 0; i < difatEntries && baseFatIndex + i < fatSectors_.size(); ++i) {
                WriteUint32(buf.data() + (i * FOUR_BYTE_SIZE), fatSectors_[baseFatIndex + i]);
            }
            uint32_t nextDifatValue = (chainIdx + 1 < difatSectors_.size())
                ? difatSectors_[chainIdx + 1]
                : AllocTable::Eof;
            WriteUint32(buf.data() + ((entriesPerSector - 1) * FOUR_BYTE_SIZE), nextDifatValue);
            const uint64_t physicalOffset = BlockToOffset(difatSector, sectorSize);
            const std::string context = "DIFAT sector " + std::to_string(difatSector);
            if (!WriteBlockChecked(physicalOffset, buf.data(), sectorSize, context)) {
                return false;
            }
        }
        header_->SetFirstDifat(difatSectors_[0]);
        header_->SetDifatCount(static_cast<BlockCount>(difatSectors_.size()));
    } else {
        header_->SetFirstDifat(AllocTable::Eof);
        header_->SetDifatCount(0);
    }
    return true;
}

bool StorageIO::SaveFat()
{
    if (!bbat_ || !header_) {
        return false;
    }
    if (fatSectors_.empty()) {
        return true;
    }
    const uint32_t blockSize = BigBlockSize();
    if (blockSize == 0) {
        SetError(ErrorCode::InvalidOperation, "Invalid block size");
        return false;
    }
    const uint32_t buflen = static_cast<uint32_t>(fatSectors_.size()) * blockSize;
    std::vector<Byte> buffer(buflen);
    if (!bbat_->Save(buffer.data(), buflen)) {
        SetError(ErrorCode::IOError, "Failed to save FAT");
        return false;
    }
    for (size_t i = 0; i < fatSectors_.size(); ++i) {
        const uint32_t sector = fatSectors_[i];
        const uint64_t physicalOffset = BlockToOffset(sector, blockSize);
        const std::string context = "FAT sector " + std::to_string(sector);
        if (!WriteBlockChecked(physicalOffset, buffer.data() + (i * blockSize), blockSize, context)) {
            return false;
        }
    }
    return true;
}

bool StorageIO::Flush()
{
    bool memoryMode = false;
    size_t blockSize = 0;
    if (!ValidateFlushState(memoryMode, blockSize)) {
        return false;
    }
    if (!dtModified_) {
        ClearError();
        return true;
    }
    std::vector<uint32_t> blocks;
    size_t neededBlocks = 0;
    if (dirtree_ && !PrepareDirectoryBlocks(blockSize, dirtree_->EntryCount(), blocks, neededBlocks)) {
        return false;
    }
    FlushSnapshot snap;
    BackupFlushState(snap);
    if (!SetupStagingBuffer(memoryMode, snap)) {
        RestoreFlushState(snap);
        return false;
    }

    auto fail = [&]() {
        RestoreFlushState(snap);
        dtModified_ = true;
        return false;
    };
    if (!ExecuteFlushSequence(blocks, neededBlocks, blockSize)) {
        return fail();
    }
    if (!FinalizeFlush(memoryMode, snap, blockSize)) {
        return fail();
    }
    dtModified_ = false;
    ClearError();
    return true;
}

bool StorageIO::FlushDirectoryTree(std::vector<uint32_t> &blocks, size_t neededBlocks, size_t blockSize)
{
    while (blocks.size() < neededBlocks) {
        if (bbat_ == nullptr) {
            return false;
        }
        uint32_t newBlock = static_cast<uint32_t>(bbat_->Unused());
        if (!blocks.empty()) {
            bbat_->Set(blocks.back(), newBlock);
        }
        bbat_->Set(newBlock, AllocTable::Eof);
        blocks.push_back(newBlock);
    }
    if (header_ && !blocks.empty() && header_->DirentStart() != blocks.front()) {
        header_->SetFirstDirSector(blocks.front());
    }
    const size_t bufflen = blocks.size() * blockSize;
    if (bufflen == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushDirectoryTree: bufflen is 0");
        return false;
    }

    std::vector<Byte> buffer(bufflen);
    if (memset_s(buffer.data(), buffer.size(), 0, buffer.size()) != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushDirectoryTree - memset_s failed");
        return false;
    }
    if (dirtree_ == nullptr) {
        return false;
    }
    if (!dirtree_->Save(buffer.data(), bufflen)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushDirectoryTree - Save failed");
        return false;
    }
    for (size_t ndx = 0; ndx < blocks.size(); ++ndx) {
        const uint64_t physicalOffset = BlockToOffset(blocks[ndx], blockSize);
        const std::string context = "directory sector " + std::to_string(blocks[ndx]);
        if (!WriteBlockChecked(physicalOffset, buffer.data() + (ndx * blockSize), static_cast<uint32_t>(blockSize),
            context)) {
            return false;
        }
    }
    return true;
}

bool StorageIO::FlushMiniFat()
{
    if (!SaveMiniFat()) {
        if (error_ == ErrorCode::Ok) {
            SetError(ErrorCode::IOError, "Failed to save mini FAT");
        }
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushMiniFat: SaveMiniFat failed");
        return false;
    }
    return true;
}

bool StorageIO::FlushFatChain()
{
    EnsureFatCapacity();
    if (!SaveFat()) {
        if (error_ == ErrorCode::Ok) {
            SetError(ErrorCode::IOError, "Failed to save FAT");
        }
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushFatChain: SaveFat failed");
        return false;
    }
    return true;
}

bool StorageIO::FlushDifatChain()
{
    if (!SaveDifat()) {
        if (error_ == ErrorCode::Ok) {
            SetError(ErrorCode::IOError, "Failed to save Difat");
        }
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushDifatChain: SaveDifat failed");
        return false;
    }
    return true;
}

bool StorageIO::FlushHeader()
{
    Byte headerBuf[kHeaderSize];
    if (!header_) {
        return false;
    }
    if (header_->Save(headerBuf, kHeaderSize)) {
        if (!WriteBlockChecked(0, headerBuf, kHeaderSize, "storage header")) {
            return false;
        }
    } else {
        SetError(ErrorCode::IOError, "Failed to save header");
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::FlushHeader: Save failed");
        return false;
    }
    return true;
}

namespace {
std::string NormalizeFilePath(const std::string &filename)
{
    char canonicalDirPath[PATH_MAX + 1] = {0x00};
    std::filesystem::path path(filename);
    std::string directory = path.parent_path().string() + "/";
    std::string basename = path.filename().string();
    if (realpath(directory.c_str(), canonicalDirPath) == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveToFile: realpath failed");
        return "";
    }
    size_t len = strlen(canonicalDirPath);
    if (len >= PATH_MAX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "Storage::SaveToFile: canonicalDirPath too long");
        return "";
    }
    canonicalDirPath[PATH_MAX] = '\0';
    std::string canonicalFileName = std::string(canonicalDirPath) + "/" + basename;
    return canonicalFileName;
}

bool WriteBufferToFile(const std::string &filename, StorageIO *self)
{
    std::ofstream outFile(filename.c_str(), std::ios::binary | std::ios::trunc | std::ios::out);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DOCUMENT,
        "StorageIO::SaveToFile: filename: %{private}%", filename.c_str());
    if (!outFile || outFile.fail()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveToFile: open file failed");
        return false;
    }
    const size_t usedSize = self->size_ > 0 ? static_cast<size_t>(self->size_) : 0;
    const size_t writeSize = std::min(usedSize, self->memoryBuffer_->size());
    outFile.write(reinterpret_cast<const char *>(self->memoryBuffer_->data()), writeSize);
    if (!outFile.good()) {
        SetError(ErrorCode::IOError, "Failed to write file");
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveToFile: write file failed");
        return false;
    }
    outFile.flush();
    if (!outFile.good()) {
        SetError(ErrorCode::IOError, "Failed to flush file");
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveToFile: flush file failed");
        return false;
    }
    return true;
}
}

bool StorageIO::SaveToFile(const char *filename, bool switchToFileMode)
{
    if (!memoryBuffer_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::SaveToFile: memoryBuffer_ is null");
        return false;
    }
    if (!Flush()) {
        return false;
    }
    
    std::string canonicalFileName = NormalizeFilePath(filename);
    if (canonicalFileName.empty()) {
        return false;
    }

    if (!WriteBufferToFile(canonicalFileName, this)) {
        return false;
    }
    
    if (switchToFileMode) {
        if (file_) {
            file_->close();
            file_.reset();
            stream_ = nullptr;
        }

        auto f =
            std::make_unique<std::fstream>(canonicalFileName.c_str(),
            std::ios::binary | std::ios::in | std::ios::out);
        if (!f || f->fail()) {
            SetError(ErrorCode::OpenFailed, std::string("Failed to open file: ")+ filename);
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "StorageIO::SaveToFile: open file failed");
            return false;
        }
        file_ = std::move(f);
        stream_ = file_.get();
        file_->seekg(0, std::ios::end);
        size_ = file_->tellg();
        file_->seekg(0, std::ios::beg);
        memoryBuffer_.reset();
    }
    return true;
}

bool StorageIO::ReadAndLoadHeader()
{
    std::array<Byte, kHeaderSize> headerBuffer{};
    if (stream_ == nullptr || !header_) {
        return false;
    }
    stream_->seekg(0);
    stream_->read(reinterpret_cast<char *>(headerBuffer.data()), headerBuffer.size());
    if (!header_->Load(headerBuffer.data(), headerBuffer.size())) {
        SetError(ErrorCode::BadOLE, "Failed to load header");
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ReadAndLoadHeader: load header failed");
        return false;
    }
    if (!header_->IsCompoundDocument()) {
        SetError(ErrorCode::NotOLE, "Not a valid OLE file signature");
        return false;
    }
    if (!header_->Valid()) {
        SetError(ErrorCode::BadOLE, "OLE header failed validation");
        return false;
    }
    return true;
}

bool StorageIO::ValidateSectorSizes(uint32_t &sectorSize)
{
    if (bbat_ == nullptr || sbat_ == nullptr || header_ == nullptr) {
        return false;
    }
    bbat_->SetBlockSize(1 << header_->BigBlockShift());
    sbat_->SetBlockSize(1 << header_->SmallBlockShift());
    difatSectors_.clear();
    fatSectors_.clear();
    protectedBlocksCache_.clear();
    protectedBlocksDirty_ = true;
    sectorSize = BigBlockSize();
    if (sectorSize == 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "StorageIO::ValidateSectorSizes: Invalid sector size");
        SetError(ErrorCode::BadOLE, "Invalid sector size");
        return false;
    }
    return true;
}

bool StorageIO::CheckClaimedTableSizes(uint64_t fileSize, uint32_t sectorSize)
{
    if (header_ == nullptr) {
        return false;
    }
    const uint64_t claimedFatBytes = static_cast<uint64_t>(header_->NumBat()) * sectorSize;
    const uint64_t claimedDifatBytes = static_cast<uint64_t>(header_->NumDifat()) * sectorSize;
    if (claimedFatBytes > fileSize) {
        SetError(ErrorCode::BadOLE, "FAT size exceeds file size");
        return false;
    }
    if (claimedDifatBytes > fileSize) {
        SetError(ErrorCode::BadOLE, "DIFAT size exceeds file size");
        return false;
    }
    return true;
}

bool StorageIO::ValidateHeaderSectorIndex(uint32_t sectorIdx, uint32_t sectorSize,
    const char *name, uint64_t fileSize)
{
    if (sectorIdx == AllocTable::Eof || sectorIdx == AllocTable::Avail) {
        return true;
    }
    const uint64_t sectorOffset = BlockToOffset(sectorIdx, sectorSize);
    if (sectorOffset >= fileSize) {
        SetError(ErrorCode::BadOLE, "Header references sector index exceeds file size");
        return false;
    }
    return true;
}

} // namespace ObjectEditor
} // namespace OHOS