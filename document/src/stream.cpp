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
#include <cstdlib>
#include <iostream>
#include <limits>
#include <securec.h>

#include "alloctable.h"
#include "dirtree.h"
#include "header.h"
#include "storage.h"
#include "stream.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
    constexpr int EOF_FLAG = static_cast<unsigned int>(StreamImpl::State::Eof);
    constexpr int BAD_FLAG = static_cast<unsigned int>(StreamImpl::State::Bad);
}

const std::string StreamImpl::nullPath;

StreamImpl::StreamImpl(const StreamImpl &stream)
    : io_(stream.io_), entry_(stream.entry_), path_(stream.path_), cachedStart_(stream.cachedStart_),
      cachedSize_(stream.cachedSize_), cachedUseBig_(stream.cachedUseBig_), blocks_(stream.blocks_), pos_(stream.pos_),
      cache_(stream.cache_), cacheSize_(stream.cacheSize_), cachePos_(stream.cachePos_), state_(stream.state_)
{
}

StreamImpl::StreamImpl(StorageIO *io, DirEntry *e) : io_(io), entry_(e), pos_(0), cacheSize_(0), cachePos_(0), state_(0)
{
    Init();
}

StreamImpl::StreamImpl(StorageIO *io, const std::string &path)
    : io_(io), entry_(io->Entry(path)), path_(path), pos_(0), cacheSize_(0), cachePos_(0), state_(0)
{
    Init();
}

void StreamImpl::Init()
{
    pos_ = 0;
    cachePos_ = 0;
    cacheSize_ = CACHE_SIZE;
    cache_.assign(cacheSize_, 0);
    blocks_.clear();
    cachedStart_ = DIR_ENTRY_END;
    cachedUseBig_ = false;
    cachedSize_ = 0;
    if (!entry_) {
        return;
    }
    cachedStart_ = entry_->Start();
    cachedSize_ = entry_->Size();
    if (!io_) {
        return;
    }
    cachedUseBig_ = io_->GetHeader() && cachedSize_ >= io_->GetHeader()->Threshold();
    if (cachedSize_ == 0) {
        return;
    }
    if (cachedUseBig_) {
        if (!io_->FollowBigBlockTable(cachedStart_, blocks_)) {
            state_ |= BAD_FLAG;
        }
    } else if (!io_->FollowSmallBlockTable(cachedStart_, blocks_)) {
        state_ |= BAD_FLAG;
    }
}

bool StreamImpl::PrepareRead(size_t pos, Byte *buffer, std::streamsize maxlen, size_t &allowed, size_t &entrySizeT)
{
    if (!entry_ || !buffer || maxlen <= 0) [[unlikely]]
    {
        return false;
    }

    const StreamPos entrySize = entry_->Size();
    entrySizeT = entrySize > static_cast<StreamPos>(std::numeric_limits<size_t>::max()) ?
        std::numeric_limits<size_t>::max() : static_cast<size_t>(entrySize);
    if (entrySize > 0) {
        if (!io_) {
            return false;
        }
        const bool useBig = io_->GetHeader() && entrySize >= io_->GetHeader()->Threshold();
        if (blocks_.empty() || cachedStart_ != entry_->Start() ||
            cachedSize_ != entrySize || cachedUseBig_ != useBig) {
            RefreshBlocks();
        }
    }

    if (pos >= entrySizeT) {
        state_ |= EOF_FLAG;
        return false;
    }

    allowed = static_cast<size_t>(maxlen);
    const size_t available = entrySizeT - pos;
    if (allowed > available) {
        allowed = available;
        state_ |= EOF_FLAG;
    } else {
        state_ &= ~EOF_FLAG;
    }
    return allowed > 0;
}

std::streamsize StreamImpl::ReadSmallBlocks(size_t pos, Byte *buffer, size_t allowed)
{
    if (!io_ || io_->SmallBlockSize() == 0) {
        return 0;
    }
    size_t index = pos / io_->SmallBlockSize();
    if (index >= blocks_.size()) {
        return 0;
    }

    std::vector<Byte> buf(io_->SmallBlockSize());
    size_t offset = pos % io_->SmallBlockSize();
    std::streamsize totalbytes = 0;
    if (allowed > MAX_ALLOWED_2G) {
        return 0;
    }
    while (totalbytes < static_cast<std::streamsize>(allowed)) {
        if (index >= blocks_.size()) {
            if (totalbytes < static_cast<std::streamsize>(allowed)) {
                state_ |= EOF_FLAG;
            }
            break;
        }
        const uint32_t read =  io_->LoadSmallBlock(blocks_[index], buf.data(), io_->SmallBlockSize());
        if (read != io_->SmallBlockSize()) {
            state_ |= BAD_FLAG;
            break;
        }
        std::streamsize count = static_cast<std::streamsize>(io_->SmallBlockSize() - offset);
        const std::streamsize remaining = static_cast<std::streamsize>(allowed) - totalbytes;
        if (count > remaining) {
            count = remaining;
        }
        const size_t destRemaining = allowed - static_cast<size_t>(totalbytes);
        if (memcpy_s(buffer + static_cast<size_t>(totalbytes), destRemaining, buf.data() + offset,
            static_cast<size_t>(count)) != EOK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "StreamImpl::ReadSmallBlocks memcpy_s failed")
            return 0;
        }
        totalbytes += count;
        offset = 0;
        index++;
    }
    return totalbytes;
}

std::streamsize StreamImpl::ReadBigBlocks(size_t pos, Byte *buffer, size_t allowed, StreamPos entrySize)
{
    if (!io_ || io_->BigBlockSize() == 0) {
        return 0;
    }
    size_t index = pos / io_->BigBlockSize();
    if (index >= blocks_.size()) {
        return 0;
    }
    if (allowed > MAX_ALLOWED_2G) {
        return 0;
    }
    std::vector<Byte> buf(io_->BigBlockSize());
    size_t offset = pos % io_->BigBlockSize();
    std::streamsize totalbytes = 0;
    while (totalbytes < static_cast<std::streamsize>(allowed)) {
        if (index >= blocks_.size()) {
            if (totalbytes < static_cast<std::streamsize>(allowed) &&
                pos + static_cast<size_t>(totalbytes) < entrySize) {
                state_ |= BAD_FLAG;
            }
            break;
        }
        const uint32_t read =  io_->LoadBigBlock(blocks_[index], buf.data(), io_->BigBlockSize());
        if (read == 0) {
            if (pos + static_cast<size_t>(totalbytes) < entrySize) {
                state_ |= BAD_FLAG;
            }
            break;
        }
        std::streamsize available = static_cast<std::streamsize>(read) - static_cast<std::streamsize>(offset);
        std::streamsize count = (available < static_cast<std::streamsize>(allowed) - totalbytes) ?
            available : static_cast<std::streamsize>(allowed) - totalbytes;
        if (count > 0) {
            const size_t destRemaining = allowed - static_cast<size_t>(totalbytes);
            if (memcpy_s(buffer + static_cast<size_t>(totalbytes), destRemaining, buf.data() + offset,
                static_cast<size_t>(count)) != EOK) {
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "StreamImpl::ReadBigBlocks memcpy_s failed")
                return 0;
            }
            totalbytes += count;
        }
        if (read < io_->BigBlockSize()) {
            break;
        }
        index++;
        offset = 0;
    }
    return totalbytes;
}

void StreamImpl::RefreshBlocks()
{
    blocks_.clear();
    cachedStart_ = DIR_ENTRY_END;
    cachedSize_ = 0;
    cachedUseBig_ = false;
    if (!entry_) {
        return;
    }
    cachedStart_ = entry_->Start();
    cachedSize_ = entry_->Size();
    if (!io_) {
        return;
    }
    cachedUseBig_ = io_->GetHeader() && cachedSize_ >= io_->GetHeader()->Threshold();
    if (cachedSize_ == 0) {
        return;
    }
    if (cachedUseBig_) {
        bool success = io_->FollowBigBlockTable(cachedStart_, blocks_);
        if (!success) {
            state_ = BAD_FLAG;
        }
    } else {
        bool success = io_->FollowSmallBlockTable(cachedStart_, blocks_);
        if (!success) {
            state_ = BAD_FLAG;
        }
    }
}

int StreamImpl::Getch()
{
    if (!entry_) {
        return 0;
    }
    if (pos_ > static_cast<std::streamsize>(entry_->Size())) {
        return -1;
    }
    if (!cacheSize_ || (pos_ < cachePos_) || (pos_ >= cachePos_ + cachedSize_)) {
        UpdateCache();
    }
    if (!cacheSize_) {
        return -1;
    }
    int data = cache_[pos_ - cachePos_];
    pos_++;
    return data;
}

std::streamsize StreamImpl::Read(size_t pos, Byte *buffer, std::streamsize maxlen)
{
    size_t allowed = 0;
    size_t entrySizeT = 0;
    if (!PrepareRead(pos, buffer, maxlen, allowed, entrySizeT)) {
        return 0;
    }
    const StreamPos entrySize = entry_->Size();
    if (!io_) {
        return 0;
    }
    if (io_->GetHeader() && entrySize < io_->GetHeader()->Threshold()) {
        return ReadSmallBlocks(pos, buffer, allowed);
    }
    return ReadBigBlocks(pos, buffer, allowed, entrySize);
}

std::streamsize StreamImpl::Read(Byte *data, std::streamsize maxlen)
{
    std::streamsize bytes = Read(Tell(), data, maxlen);
    pos_ += bytes;
    if (entry_ && pos_ >= 0 && static_cast<StreamPos>(pos_) == entry_->Size()) {
        state_ |= EOF_FLAG;
    }
    return bytes;
}

void StreamImpl::UpdateCache()
{
    if (!entry_) {
        return;
    }
    if (cache_.empty()) {
        return;
    }
    if (cacheSize_ == 0) {
        return;
    }
    cachePos_ = pos_ - (pos_ % cacheSize_);
    size_t bytes = static_cast<size_t>(cacheSize_);
    const StreamPos entrySize = entry_->Size();
    const StreamPos cacheStart = cachePos_ < 0 ? 0 : static_cast<StreamPos>(cachePos_);
    if (static_cast<size_t>(cacheStart) + bytes > static_cast<size_t>(entrySize)) {
        bytes = static_cast<size_t>(entrySize - cacheStart);
    }
    cacheSize_ = Read(static_cast<size_t>(cacheStart),
        cache_.data(), static_cast<std::streamsize>(bytes));
}

bool StreamImpl::PrepareWrite(const Byte *data, uint32_t &maxlen)
{
    if (!entry_ || !data || maxlen == 0) [[unlikely]]
    {
        return false;
    }

    const StreamPos entrySize = entry_->Size();
    if (entrySize > 0) {
        if (!io_) {
            return false;
        }
        const bool useBig = io_->GetHeader() && entrySize >= io_->GetHeader()->Threshold();
        if (blocks_.empty() || cachedStart_ != entry_->Start() ||
            cachedSize_ != entrySize || cachedUseBig_ != useBig) {
            RefreshBlocks();
        }
    }
    state_ &= ~EOF_FLAG;
    return true;
}

bool StreamImpl::EnsureWriteCapacity(uint32_t &targetLen)
{
    const StreamPos currentPos = pos_ < 0 ? 0 : static_cast<StreamPos>(pos_);
    const StreamPos desiredSize = currentPos + static_cast<StreamPos>(targetLen);
    if (desiredSize <= entry_->Size()) {
        return true;
    }
    if (!io_) {
        return false;
    }
    if (!io_->ExtendEntry(entry_, desiredSize)) {
        const StreamPos currentSize = entry_->Size();
        const StreamPos trimmed = (currentSize > currentPos) ? (currentSize - currentPos) : 0;
        targetLen = static_cast<uint32_t>(std::min<StreamPos>(trimmed,
            static_cast<StreamPos>(std::numeric_limits<uint32_t>::max())));
        state_ |= (EOF_FLAG | BAD_FLAG);
    } else {
        RefreshBlocks();
    }
    return targetLen > 0;
}

uint32_t StreamImpl::WriteMiniBlocks(const Byte *data, uint32_t targetLen,
    uint64_t smallBlockSize, uint64_t bigBlockSize)
{
    if (smallBlockSize == 0 || bigBlockSize == 0) {
        return 0;
    }
    if (!io_) {
        return 0;
    }
    std::vector<uint32_t> sbrootEntry = io_->SbBlocks();
    if (pos_ < 0) {
        return 0;
    }
    size_t index = static_cast<size_t>(pos_ / static_cast<std::streamsize>(smallBlockSize));
    if (index > sbrootEntry.size()) {
        return 0;
    }
    uint64_t offset = static_cast<uint64_t>(pos_ % static_cast<std::streamsize>(smallBlockSize));
    uint32_t count = 0;
    for (; ((index < blocks_.size()) && (count < targetLen)); index++) {
        const uint32_t minifatIndex = blocks_[index];
        const uint64_t sbindexOffset = minifatIndex % 8;
        const uint64_t position = static_cast<uint64_t>(minifatIndex) * smallBlockSize;
        const size_t bbindex = static_cast<size_t>(position / bigBlockSize);
        if (bbindex >= sbrootEntry.size()) {
            state_ |= BAD_FLAG;
            return count;
        }
        const uint32_t bbindice = sbrootEntry[bbindex];
        const uint64_t fisicalOffset  = (static_cast<uint64_t>(bbindice) * bigBlockSize) +
            bigBlockSize + (sbindexOffset * smallBlockSize) + offset;
        const uint64_t canwrite64 = smallBlockSize - offset;
        const uint32_t canwrite = static_cast<uint32_t>(std::min<uint64_t>(
            canwrite64, static_cast<uint64_t>(targetLen - count)));
        const uint32_t written = io_->SaveBlock(fisicalOffset, data + count, canwrite);
        if (written < canwrite) {
            state_ |= BAD_FLAG;
            return count;
        }
        count += written;
        offset = 0;
    }
    return count;
}

uint32_t StreamImpl::WriteBigBlocks(const Byte *data, uint32_t targetLen, uint64_t bigBlockSize)
{
    if (bigBlockSize == 0) {
        return 0;
    }
    size_t index = static_cast<size_t>(pos_ / static_cast<std::streamsize>(bigBlockSize));
    if (index > blocks_.size()) {
        return 0;
    }
    
    uint64_t offset = static_cast<uint64_t>(pos_ % static_cast<std::streamsize>(bigBlockSize));
    uint32_t count = 0;
    for (; ((index < blocks_.size()) && (count < targetLen)); index++) {
        const uint64_t fisicalOffset = (static_cast<uint64_t>(blocks_[index]) *
            bigBlockSize) + bigBlockSize + offset;
        const uint64_t canwrite64 = bigBlockSize - offset;
        const uint32_t canwrite = static_cast<uint32_t>(
            std::min<uint64_t>(canwrite64, static_cast<uint64_t>(targetLen - count)));
        if (!io_) {
            return 0;
        }
        const uint32_t written = io_->SaveBlock(fisicalOffset, data + count, canwrite);
        if (written < canwrite) {
            state_ |= BAD_FLAG;
            return count;
        }
        count += written;
        offset = 0;
    }
    return count;
}

uint32_t StreamImpl::Write(const Byte *data, uint32_t maxlen)
{
    if (!PrepareWrite(data, maxlen)) {
        return 0;
    }
    if (!EnsureWriteCapacity(maxlen)) {
        return 0;
    }
    if (!io_) {
        return 0;
    }
    const uint32_t targetLen = maxlen;
    const uint64_t bigBlockSize = io_->BigBlockSize();
    const uint64_t smallBlockSize = io_->SmallBlockSize();

    uint32_t count = 0;
    if (io_->GetHeader() && entry_->Size() < io_->GetHeader()->Threshold()) {
        count = WriteMiniBlocks(data, targetLen, smallBlockSize, bigBlockSize);
    } else {
        count = WriteBigBlocks(data, targetLen, bigBlockSize);
    }

    if (count > INT32_MAX) {
        return 0;
    }
    pos_ += static_cast<int32_t>(count);
    return count;
}
} // namespace ObjectEditor
} // namespace OHOS