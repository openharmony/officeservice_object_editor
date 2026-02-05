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
    // 依据 HMID（32 位十六进制）构造内存态 CD 存储
    explicit StorageIO(const std::string &hmid);
    ~StorageIO();

public:
    // @deprecated 改用 LastError()
    [[nodiscard]] int Result() const
    {
        return static_cast<int>(error_);
    }

    // 新版错误接口
    [[nodiscard]] bool IsValid() const noexcept;
    [[nodiscard]] ErrorCode LastError() const noexcept;
    [[nodiscard]] const char *ErrorMessage() const noexcept;
    void ClearError() noexcept;

    // 获取已解析的 Header；初始化失败时返回 nullptr
    [[nodiscard]] const Header *GetHeader() const
    {
        return header_ ? header_.get() : nullptr;
    }
    // 获取指定路径的 DirEntry；可按需创建
    [[nodiscard]] DirEntry *Entry(const std::string &path, bool create = false);
    // 获取只读 DirEntry
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
        return (sbat_) ? sbat_->BlockSize() : 0;
    }
    [[nodiscard]] uint32_t BigBlockSize() const
    {
        return (bbat_) ? bbat_->BlockSize() : 0;
    }
    [[nodiscard]] bool FollowSmallBlockTable(uint32_t start, std::vector<uint32_t> &chain) const
    {
        if (sbat_)
            return sbat_->Follow(start, chain);
        return false;
    }
    [[nodiscard]] bool FollowBigBlockTable(uint32_t start, std::vector<uint32_t> &chain) const
    {
        if (bbat_)
            return bbat_->Follow(start, chain);
        return false;
    }

    [[nodiscard]] const std::vector<uint32_t> &SbBlocks() const
    {
        return sbBlocks_;
    }

    // 访问 FAT 表条目
    [[nodiscard]] size_t BbatCount() const
    {
        return bbat_ ? bbat_->Count() : 0;
    }
    [[nodiscard]] uint32_t BbatAt(size_t index) const
    {
        return bbat_ ? (*bbat_)[index] : 0;
    }

    // 访问 MiniFAT 表条目
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
    // 获取 Root Entry（索引 0）
    [[nodiscard]] DirEntry *GetRootEntry();
    // 删除路径对应 Entry，并可由 Flush 持久化
    [[nodiscard]] bool DeleteEntry(const std::string &path);
    // 写入时动态扩展，按需分配块并更新 FAT/Entry
    [[nodiscard]] bool ExtendEntry(DirEntry *entry, uint64_t newSize);

    // MiniFAT 支持：扩展 Root Stream 容量以容纳 mini blocks
    [[nodiscard]] bool ExtendRootStream(uint32_t requiredMiniBlocks);
    // Flush 时持久化 MiniFAT
    [[nodiscard]] bool SaveMiniFat();
    // Flush 时持久化 DIFAT
    [[nodiscard]] bool SaveDifat();
    // DIFAT 保护块分配/缓存
    [[nodiscard]] uint32_t AllocateProtectedSector(uint32_t markerType);
    void RebuildProtectedBlocksCache();

    // 持久化 FAT
    [[nodiscard]] bool SaveFat();
    void EnsureFatCapacity();
    // 持久化所有修改（事务式）
    [[nodiscard]] bool Flush();

    // 内存态持久化：当 `memoryBuffer_` 存在时，将整段内存 CD 写入 `filename`。
    // 可选切换为文件模式：用新开的 fstream 替换内部 `stream_`。
    // 成功返回 true，失败（包含非内存模式下调用）返回 false。
    [[nodiscard]] bool SaveToFile(const char *filename, bool switchToFileMode = false);

    // 从底层 compound document 文件按偏移读取原始字节；仅支持 file-backed 模式。
    bool ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead);

private:
    // 默认常量（避免魔法数）
    static constexpr uint32_t kDefaultSectorSize = 512;  // 标准 CFB Sector
    static constexpr uint32_t kHeaderSize = 512;         // 固定 CD Header 大小
    static constexpr size_t kDirectoryEntrySize = 128;   // 序列化 DirEntry 大小

    // 计算 Sector/Block 的物理偏移（含 512 字节 Header）
    static inline uint64_t BlockToOffset(uint32_t blockIndex, uint64_t blockSize)
    {
        return (static_cast<uint64_t>(blockIndex) + 1) * blockSize;
    }

    void Init();
    [[nodiscard]] bool Load();
    void Close();

    // 统一设置错误码/信息
    void SetError(ErrorCode code, const std::string &message = "", bool invalidate = true);

    // 构造最小 CD 结构（用于 HMID 内存态）
    bool ConfigMinimalCd(const std::string &hmid);
    // 序列化 Header/DirTree/AllocTable 到内存流
    [[nodiscard]] bool SerializeToMemory();
    // 校验 Header 声明与布局（MS-CFB 偏移/阈值）
    [[nodiscard]] bool ValidateHeader(uint64_t fileSize);
    [[nodiscard]] bool ReadAndLoadHeader();
    [[nodiscard]] bool ValidateSectorSizes(uint32_t &sectorSize);
    [[nodiscard]] bool CheckClaimedTableSizes(uint64_t fileSize, uint32_t sectorSize);
    [[nodiscard]] bool ValidateHeaderSectorIndex(uint32_t sectorIdx, uint32_t sectorSize, const char *name,
                                                 uint64_t fileSize);
    // 收集 Header 直接声明的 FAT Sectors
    [[nodiscard]] bool LoadFatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks);
    // 读取 DIFAT 链，补全 FAT 列表并反序列化 FAT
    [[nodiscard]] bool LoadDifatChain(uint32_t sectorSize, std::vector<uint32_t> &fatBlocks);
    // 加载目录流，返回 Root Stream 起始 Sector
    [[nodiscard]] bool LoadDirectoryTree(SectorIndex &sbStart);
    // 加载 MiniFAT 并校验 mini stream 范围
    [[nodiscard]] bool LoadMiniFat(SectorIndex sbStart);
    // 写回 Header
    [[nodiscard]] bool FlushHeader();
    // 写回 FAT
    [[nodiscard]] bool FlushFatChain();
    // 写回 DIFAT
    [[nodiscard]] bool FlushDifatChain();
    // 写回 Directory
    [[nodiscard]] bool FlushDirectoryTree(std::vector<uint32_t> &blocks, size_t neededBlocks, size_t blockSize);
    // 写回 MiniFAT
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

    // 所有权模型：
    // - stream_：非拥有型观察指针，指向当前活跃的文件流；内存模式下为空
    // - file_：通过 unique_ptr 拥有 fstream 句柄
    // - memoryBuffer_：通过 unique_ptr 拥有内存态 CD 缓冲
    std::iostream *stream_ = nullptr;
    std::unique_ptr<std::fstream> file_;
    std::unique_ptr<std::vector<uint8_t>> memoryBuffer_;  // 内存态 CD 缓冲
    // Flush 事务阶段可选的写入重定向缓冲
    std::vector<uint8_t> *flushWriteBuffer_ = nullptr;
    std::streamoff size_ = 0;                         // 逻辑存储大小
    ErrorCode error_ = ErrorCode::Ok;                 // 取代旧版 _result
    std::string errorMsg_;                            // 人类可读的错误信息
    bool isValid_ = false;                            // 对象有效性标记
    std::list<std::unique_ptr<StreamImpl>> streams_;  // 当前打开的 Stream
    std::vector<uint32_t> sbBlocks_;                  // mini stream 的大块列表

    std::unique_ptr<Header> header_;    // Header 所有权
    std::unique_ptr<DirTree> dirtree_;  // 目录树所有权
    std::unique_ptr<AllocTable> bbat_;  // 大块 FAT 表（所有权）
    std::unique_ptr<AllocTable> sbat_;  // 小块 MiniFAT 表（所有权）
    bool dtModified_ = false;
    // DIFAT 统一架构
    std::vector<uint32_t> fatSectors_;         // 所有 FAT block 编号（权威来源）
    std::vector<uint32_t> difatSectors_;       // 所有 DIFAT block 编号
    std::set<uint32_t> protectedBlocksCache_;  // 保护块缓存（避免重复构建）
    bool protectedBlocksDirty_ = true;         // 缓存失效标志

    // 禁止拷贝与赋值
    StorageIO(const StorageIO &) = delete;
    StorageIO &operator=(const StorageIO &) = delete;
};

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // STORAGE_H