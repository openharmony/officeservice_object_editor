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

#ifndef STORAGE_H
#define STORAGE_H

#include <fstream>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "alloctable.h"
#include "dirtree.h"
#include "header.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class StreamImpl;

class StorageIO {
public:
    enum class ErrorCode : uint8_t {
        Ok = 0,
        OpenFailed = 1,
        NotOLE = 2,
        BadOLE = 3,
        IOError = 4,
        AllocationFailed = 5,
        InvalidOperation = 6,
        Corruption = 7,
        Unknown = 0xFF
    };

public:
    explicit StorageIO(const char *filename);
    explicit StorageIO(std::iostream *stream);
    // Construct memory-mode CD storage based on OEID (32-bit hex)
    explicit StorageIO(const std::string &oeid);
    ~StorageIO();

public:
    // @deprecated Use LastError() instead
    [[nodiscard]] int Result() const
    {
        return static_cast<int>(error_);
    }

    // New error interface
    [[nodiscard]] bool IsValid() const noexcept;
    [[nodiscard]] ErrorCode LastError() const noexcept;
    [[nodiscard]] const char *ErrorMessage() const noexcept;
    void ClearError() noexcept;

    // Get parsed Header; returns nullptr if initialization failed
    [[nodiscard]] const Header *GetHeader() const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        return header_ ? header_.get() : nullptr;
    }
    // Get DirEntry for specified path; can create if needed
    [[nodiscard]] DirEntry *Entry(const std::string &path, bool create = false);
    // Get read-only DirEntry
    [[nodiscard]] const DirEntry *Entry(const std::string &path) const
    {
        return dirtree_ ? dirtree_->Entry(path) : nullptr;
    }
    void Path(std::string &result) const
    {
        if (!dirtree_) {
            return;
        }
        dirtree_->Path(result);
    }
    void ListDirectory(std::list<std::string> &) const;
    void ListEntries(std::vector<const DirEntry *> &result) const;
    [[nodiscard]] uint32_t SmallBlockSize() const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        return (sbat_) ? sbat_->BlockSize() : 0;
    }
    [[nodiscard]] uint32_t BigBlockSize() const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        return (bbat_) ? bbat_->BlockSize() : 0;
    }
    [[nodiscard]] bool FollowSmallBlockTable(uint32_t start, std::vector<uint32_t> &chain) const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        if (sbat_)
            return sbat_->Follow(start, chain);
        return false;
    }
    [[nodiscard]] bool FollowBigBlockTable(uint32_t start, std::vector<uint32_t> &chain) const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        if (bbat_)
            return bbat_->Follow(start, chain);
        return false;
    }

    [[nodiscard]] const std::vector<uint32_t> &SbBlocks() const
    {
        std::lock_guard<std::recursive_mutex> lock(ioMutex_);
        return sbBlocks_;
    }

    // Access FAT table entries
    [[nodiscard]] size_t BbatCount() const
    {
        return bbat_ ? bbat_->Count() : 0;
    }
    [[nodiscard]] uint32_t BbatAt(size_t index) const
    {
        return bbat_ ? (*bbat_)[index] : 0;
    }

    // Access MiniFAT table entries
    [[nodiscard]] size_t SbatCount() const
    {
        return sbat_ ? sbat_->Count() : 0;
    }
    [[nodiscard]] uint32_t SbatAt(size_t index) const
    {
        return sbat_ ? (*sbat_)[index] : 0;
    }

    void GetEntryChildrens(size_t index, std::vector<size_t> &result)
    {
        if (!dirtree_) {
            return;
        }
        dirtree_->Children(index, result);
    }

    void Debug()
    {
        if (!dirtree_) {
            return;
        }
        dirtree_->Debug();
    }

    void Children(size_t index, std::vector<size_t> &result) const
    {
        if (dirtree_) {
            dirtree_->Children(index, result);
        }
    }

    bool IsDirty() const noexcept;

public:
    [[nodiscard]] bool Create(const char *filename);
    [[nodiscard]] bool EnterDirectory(const std::string &directory)
    {
        if (!dirtree_) {
            return false;
        }
        return dirtree_->EnterDirectory(directory);
    }
    void LeaveDirectory()
    {
        if (!dirtree_) {
            return;
        }
        dirtree_->LeaveDirectory();
    }
    [[nodiscard]] uint32_t LoadSmallBlock(uint32_t block, Byte *buffer, uint32_t maxlen);
    [[nodiscard]] uint32_t LoadBigBlock(uint32_t block, Byte *buffer, uint32_t maxlen);
    [[nodiscard]] uint32_t SaveBlock(uint64_t physicalOffset, const Byte *buffer, uint32_t maxlen);
    // Get Root Entry (index 0)
    [[nodiscard]] DirEntry *GetRootEntry();
    // Delete Entry for path, can be persisted by Flush
    [[nodiscard]] bool DeleteEntry(const std::string &path);
    // Dynamically expand on write, allocate blocks as needed and update FAT/Entry
    [[nodiscard]] bool ExtendEntry(DirEntry *entry, uint64_t newSize);

    // MiniFAT support: expand Root Stream capacity to hold mini blocks
    [[nodiscard]] bool ExtendRootStream(uint32_t requiredMiniBlocks);
    // Persist MiniFAT on Flush
    [[nodiscard]] bool SaveMiniFat();
    // Persist DIFAT on Flush
    [[nodiscard]] bool SaveDifat();
    // DIFAT protected block allocation/cache
    [[nodiscard]] uint32_t AllocateProtectedSector(uint32_t markerType);
    void RebuildProtectedBlocksCache();

    // Persist FAT
    [[nodiscard]] bool SaveFat();
    void EnsureFatCapacity();
    // Persist all modifications (transactional)
    [[nodiscard]] bool Flush();

    // Memory-mode persistence: when `memoryBuffer_` exists, write entire memory CD to `filename`.
    // Optionally switch to file mode: replace internal `stream_` with newly opened fstream.
    // Returns true on success, false on failure (including calling in non-memory mode).
    [[nodiscard]] bool SaveToFile(const char *filename, bool switchToFileMode = false);

    // Read raw bytes from underlying compound document file by offset; only supports file-backed mode.
    bool ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead);

private:
    // Default constants (avoid magic numbers)
    static constexpr uint32_t kDefaultSectorSize = 512;  // Standard CFB Sector
    static constexpr uint32_t kHeaderSize = 512;         // Fixed CD Header size
    static constexpr size_t kDirectoryEntrySize = 128;   // Serialized DirEntry size

    // Calculate physical offset of Sector/Block (including 512-byte Header)
    static inline uint64_t BlockToOffset(uint32_t blockIndex, uint64_t blockSize)
    {
        return (static_cast<uint64_t>(blockIndex) + 1) * blockSize;
    }

    void Init();
    [[nodiscard]] bool Load();
    void Close();

    // Unified error code/message setting
    void SetError(ErrorCode code, const std::string &message = "", bool invalidate = true);

    // Construct minimal CD structure (for OEID memory mode)
    bool ConfigMinimalCd(const std::string &oeid);
    // Serialize Header/DirTree/AllocTable to memory stream
    [[nodiscard]] bool SerializeToMemory();
    // Validate Header declarations and layout (MS-CFB offsets/thresholds)
    [[nodiscard]] bool ValidateHeader(uint64_t fileSize);
    [[nodiscard]] bool ReadAndLoadHeader();
    [[nodiscard]] bool ValidateSectorSizes(uint32_t &sectorSize);
    [[nodiscard]] bool CheckClaimedTableSizes(uint64_t fileSize, uint32_t sectorSize);
    [[nodiscard]] bool ValidateHeaderSectorIndex(uint32_t sectorIdx, uint32_t sectorSize, const char *name,
                                                 uint64_t fileSize);
    // Collect FAT Sectors declared directly in Header
    [[nodiscard]] bool LoadFatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks);
    // Read DIFAT chain, complete FAT list and deserialize FAT
    [[nodiscard]] bool LoadDifatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks);
    // Load directory stream, return Root Stream starting Sector
    [[nodiscard]] bool LoadDirectoryTree(SectorIndex &sbStart);
    // Load MiniFAT and validate mini stream range
    [[nodiscard]] bool LoadMiniFat(SectorIndex sbStart);
    // Write back Header
    [[nodiscard]] bool FlushHeader();
    // Write back FAT
    [[nodiscard]] bool FlushFatChain();
    // Write back DIFAT
    [[nodiscard]] bool FlushDifatChain();
    // Write back Directory
    [[nodiscard]] bool FlushDirectoryTree(std::vector<uint32_t> &blocks, size_t neededBlocks, size_t blockSize);
    // Write back MiniFAT
    [[nodiscard]] bool FlushMiniFat();

    [[nodiscard]] uint32_t LoadSmallBlocks(const std::vector<uint32_t> &blocks, Byte *buffer, uint32_t maxlen);
    [[nodiscard]] uint32_t LoadBigBlocks(const std::vector<uint32_t> &blocks, Byte *buffer, uint32_t maxlen);
    [[nodiscard]] bool ConvertSmallToBig(DirEntry *entry, uint64_t newSize);
    [[nodiscard]] bool WriteBlockChecked(uint64_t physicalOffset, const Byte *buffer, uint32_t maxlen,
                                         const std::string &context);
    [[nodiscard]] uint32_t ReadBigBlocksFromMemory(
        const std::vector<uint32_t> &blocks, Byte *data, uint32_t maxlen);
    [[nodiscard]] uint32_t ReadBigBlocksFromFile(
        const std::vector<uint32_t> &blocks, Byte *data, uint32_t maxlen);
    [[nodiscard]] uint32_t ReadMiniBlocks(const std::vector<uint32_t> &blocks,
                                          Byte *data, uint32_t maxlen);
    [[nodiscard]] bool ValidateMiniBlockAccess(uint32_t block, uint64_t pos64,
                                               uint64_t miniStreamSize,
                                               uint32_t bigBlockSz,
                                               uint32_t &bbindex);
    [[nodiscard]] bool ReadMiniBlock(uint32_t block, uint64_t pos64,
                                     uint64_t miniStreamSize,
                                     uint32_t bigBlockSz, uint32_t smallBlockSz,
                                     uint32_t remaining, Byte *dest,
                                     uint32_t &copied);
    [[nodiscard]] bool LoadMiniFatBlocks(std::vector<uint32_t> &blocks,
                                         std::vector<Byte> &buffer);
    [[nodiscard]] bool FollowMiniStream(SectorIndex sbStart);
    [[nodiscard]] bool ValidateMiniFatEntries(size_t &highestUsed);
    [[nodiscard]] bool ValidateMiniRootCoverage(size_t highestUsed);
    [[nodiscard]] uint32_t SaveBlockToFile(uint64_t physicalOffset,
                                           const Byte *data, uint32_t len);
    [[nodiscard]] uint32_t SaveBlockToBuffer(uint64_t physicalOffset,
                                             const Byte *data, uint32_t len,
                                             std::vector<uint8_t> &buffer);
    [[nodiscard]] uint32_t BuildMiniFatBuffer(std::vector<uint8_t> &buffer,
                                              uint32_t &neededBlocks);
    [[nodiscard]] bool WriteMiniFatBlocks(const std::vector<uint8_t> &buffer,
                                          uint32_t neededBlocks,
                                          uint32_t entriesPerSector);
    [[nodiscard]] bool AddMiniFatSectors(uint32_t neededBlocks,
                                         uint32_t entriesPerSector);
    [[nodiscard]] bool ValidateChainEntry(uint32_t start, AllocTable *table,
                                          const DirEntry &entry, bool useBig,
                                          uint32_t miniBlockSize,
                                          uint32_t bigBlockSize);
    [[nodiscard]] bool CollectChainBlocks(const DirEntry &entry, bool useBig,
                                          uint32_t miniBlockSize,
                                          uint32_t bigBlockSize,
                                          std::vector<uint32_t> &out);
    [[nodiscard]] bool ValidateChainLength(const DirEntry &entry, bool useBig,
                                           uint32_t miniBlockSize,
                                           uint32_t bigBlockSize,
                                           const std::vector<uint32_t> &out);
    [[nodiscard]] bool ValidateConvertInputs(DirEntry *entry, uint64_t newSize,
                                             uint64_t &oldSize,
                                             uint32_t &blockSize,
                                             uint32_t &threshold);
    [[nodiscard]] bool ReadMiniStream(DirEntry *entry, uint64_t oldSize,
                                      std::vector<uint32_t> &oldBlocks,
                                      std::vector<Byte> &smallBuffer);
    [[nodiscard]] bool ComputeBlocksNeeded(uint64_t newSize, uint32_t blockSize,
                                           uint32_t &blocksNeeded);
    [[nodiscard]] bool AllocateBigBlocksForConvert(
        uint32_t blocksNeeded, std::vector<uint32_t> &newBlocks);
    [[nodiscard]] bool CopyDataToBigBlocks(const std::vector<uint32_t> &blocks,
                                           const std::vector<Byte> &src,
                                           uint64_t newSize,
                                           uint32_t blockSize);
    [[nodiscard]] bool CopyBatchToBlocks(size_t batchStartIdx, size_t batchEndIdx,
                                         const std::vector<uint32_t> &blocks,
                                         const std::vector<Byte> &src,
                                         uint64_t newSize, uint32_t blockSize,
                                         size_t &copied);
    [[nodiscard]] bool WriteBatch(const std::vector<uint32_t> &blocks,
                                  size_t batchStartIdx, size_t lastWrittenIdx,
                                  const std::vector<Byte> &batchBuffer,
                                  uint32_t batchWriteLen);
    void ReleaseMiniBlocks(const std::vector<uint32_t> &oldBlocks);
    [[nodiscard]] bool LoadDifatChainBody(uint32_t sectorSize,
                                          uint32_t expectedFromDifat,
                                          uint32_t expectedMaxSectors,
                                          std::vector<uint32_t> &fatBlocks);
    [[nodiscard]] bool ReadDifatSector(uint32_t sectorSize, uint32_t difatSector,
                                       std::vector<uint8_t> &difatBuf);
    [[nodiscard]] bool ProcessDifatEntries(uint32_t entriesPerSector,
                                           uint32_t expectedFromDifat,
                                           uint32_t expectedMaxSectors,
                                           uint32_t &loadedFromDifat,
                                           const std::vector<uint8_t> &difatBuf,
                                           std::vector<uint32_t> &fatBlocks);
    [[nodiscard]] bool ValidateFlushState(bool &memoryMode, size_t &blockSize);
    [[nodiscard]] bool PrepareDirectoryBlocks(size_t blockSize, size_t dirEntries,
                                              std::vector<uint32_t> &blocks,
                                              size_t &neededBlocks);
    struct FlushSnapshot {
        Header headerBackup;
        std::vector<uint32_t> fatSectorsBackup;
        std::vector<uint32_t> difatSectorsBackup;
        std::set<uint32_t> protectedBlocksCacheBackup;
        bool protectedBlocksDirtyBackup = false;
        std::vector<uint32_t> sbBlocksBackup;
        std::vector<uint32_t> bbatSnapshot;
        std::streamoff sizeBackup = 0;
        std::vector<uint8_t> stagingMemory;
        std::vector<uint8_t> *prevFlushWriteBuffer = nullptr;
    };
    void BackupFlushState(FlushSnapshot &snap);
    void RestoreFlushState(const FlushSnapshot &snap);
    [[nodiscard]] bool SetupStagingBuffer(bool memoryMode, FlushSnapshot &snap);
    [[nodiscard]] bool ExecuteFlushSequence(std::vector<uint32_t> &blocks, size_t neededBlocks, size_t blockSize);
    [[nodiscard]] bool FinalizeFlush(bool memoryMode, FlushSnapshot &snap, size_t blockSize);
    [[nodiscard]] bool ValidateDeletePreconditions(uint32_t &threshold, uint32_t &miniBlockSize,
        uint32_t &bigBlockSize);
    [[nodiscard]] bool CollectDeleteTargets(const std::string &path, std::vector<DirEntry> &targets);
    [[nodiscard]] bool BuildReleasePlan(const std::vector<DirEntry> &targets, uint32_t threshold,
        uint32_t miniBlockSize, uint32_t bigBlockSize, std::vector<std::pair<bool, std::vector<uint32_t>>> &plan);
    [[nodiscard]] bool CollectChainForEntry(const DirEntry &entry, bool useBig, uint32_t miniBlockSize,
        uint32_t bigBlockSize, std::vector<uint32_t> &out);
    void ExecuteBlockRelease(const std::vector<std::pair<bool, std::vector<uint32_t>>> &plan);
    [[nodiscard]] bool ValidateExtendPreconditions(DirEntry *entry);
    [[nodiscard]] bool NeedsBigBlocks(uint64_t size) const;
    [[nodiscard]] bool ValidateAndFetchExistingChain(DirEntry *entry, bool useBig,
        std::vector<uint32_t> &existingBlocks);
    [[nodiscard]] bool ExtendSameTypeChain(DirEntry *entry, uint64_t oldSize, uint64_t newSize, bool useBig);
    [[nodiscard]] bool AllocateFreshChain(DirEntry *entry, uint64_t newSize, bool useBig);
    [[nodiscard]] bool EnsureRootForMiniGrowth(uint32_t requiredMiniBlocks, const std::string &logContext);
    [[nodiscard]] bool AdvertiseFATSectorInDIFAT(uint32_t fatSector, uint32_t current, uint32_t difatEntries);
    bool IsValidReadParams(size_t offset, uint8_t *buf, size_t len, size_t *outRead);
    bool ExtendChainWithZeros(std::vector<uint32_t> &chain, uint32_t &neededBigBlocks, uint32_t &bigBlocks);
    void AddBlocksFromChain(uint32_t start);
    bool ExtendAndFetchSbatChain(std::vector<uint32_t> &chain, uint32_t &neededBlocks);
    bool WriteBufferToFile(const std::string &filename);

    // Ownership model:
    // - stream_: non-owning observer pointer, points to current active file stream; empty in memory mode
    // - file_: owns fstream handle via unique_ptr
    // - memoryBuffer_: owns memory-mode CD buffer via unique_ptr
    std::iostream *stream_ = nullptr;
    std::unique_ptr<std::fstream> file_;
    std::unique_ptr<std::vector<uint8_t>> memoryBuffer_;  // Memory-mode CD buffer
    // Optional write redirect buffer during Flush transaction phase
    std::vector<uint8_t> *flushWriteBuffer_ = nullptr;
    std::streamoff size_ = 0;                         // Logical storage size
    ErrorCode error_ = ErrorCode::Ok;                 // Replaces legacy _result
    std::string errorMsg_;                            // Human-readable error message
    bool isValid_ = false;                            // Object validity flag
    std::list<std::unique_ptr<StreamImpl>> streams_;  // Currently open Streams
    std::vector<uint32_t> sbBlocks_;                  // Big block list for mini stream

    std::unique_ptr<Header> header_;    // Header ownership
    std::unique_ptr<DirTree> dirtree_;  // Directory tree ownership
    std::unique_ptr<AllocTable> bbat_;  // Big block FAT table (ownership)
    std::unique_ptr<AllocTable> sbat_;  // Small block MiniFAT table (ownership)
    bool dtModified_ = false;
    // DIFAT unified architecture
    std::vector<uint32_t> fatSectors_;         // All FAT block numbers (authoritative source)
    std::vector<uint32_t> difatSectors_;       // All DIFAT block numbers
    std::set<uint32_t> protectedBlocksCache_;  // Protected block cache (avoid repeated construction)
    bool protectedBlocksDirty_ = true;         // Cache invalidation flag
    uint32_t writeBufferSize_ = 0;             // Buffered bytes count
    static constexpr uint32_t MAX_BUFFER_SIZE = 16 * 1024 * 1024;  // 16M buffer

    // Disable copy and assignment
    StorageIO(const StorageIO &) = delete;
    StorageIO &operator=(const StorageIO &) = delete;

    mutable std::recursive_mutex ioMutex_;
};

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // STORAGE_H