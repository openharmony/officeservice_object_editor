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

#ifndef STREAM_H
#define STREAM_H

#include <fstream>
#include <limits>
#include <list>
#include <vector>

#include "dirtree.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class StorageIO;

class StreamImpl {
public:
    enum class State : unsigned int { Eof = 1, Bad = 2 };
    static const std::string nullPath;

public:
    StreamImpl(const StreamImpl &);
    StreamImpl(StorageIO *io, DirEntry *e);
    StreamImpl(StorageIO *io, const std::string &path);
    ~StreamImpl() = default;

public:
    const std::string &Path() const
    {
        return !path_.empty() ? path_ : (entry_ ? entry_->Name() : nullPath);
    }
    StreamPos Size() const
    {
        return (entry_) ? entry_->Size() : 0;
    }
    std::streamsize Tell() const
    {
        return pos_;
    }
    bool Fail() const
    {
        return ((state_ & static_cast<unsigned int>(State::Bad)) != 0);
    }
    bool Eof() const
    {
        return ((state_ & static_cast<unsigned int>(State::Eof)) != 0);
    }

public:
    void Seek(StreamPos pos)
    {
        if (!entry_)
            return;

        if (pos > entry_->Size()) {
            state_ |= static_cast<unsigned int>(State::Eof);
            pos_ = static_cast<std::streamsize>(entry_->Size());
            return;
        } else {
            state_ &= ~static_cast<unsigned int>(State::Eof);
        }

        const auto maxStreamPos = static_cast<StreamPos>(std::numeric_limits<std::streamsize>::max());
        if (pos > maxStreamPos) {
            state_ |= static_cast<unsigned int>(State::Bad);
            pos_ = std::numeric_limits<std::streamsize>::max();
            return;
        }

        pos_ = static_cast<std::streamsize>(pos);
    }

    int Getch();
    std::streamsize Read(Byte *data, std::streamsize maxlen);

    std::streamsize ReadBufferUntilNull(std::vector<Byte> &buffer);

    uint32_t Write(const Byte *data, uint32_t maxlen);

private:
    void Init();
    void RefreshBlocks();  // 扩容后刷新 FAT 块链，但不重置游标
    std::streamsize Read(size_t pos, Byte *data, std::streamsize maxlen);
    void UpdateCache();
    bool PrepareRead(size_t pos, Byte *buffer, std::streamsize maxlen, size_t &allowed, size_t &entrySizeT);
    std::streamsize ReadSmallBlocks(size_t pos, Byte *buffer, size_t allowed);
    std::streamsize ReadBigBlocks(size_t pos, Byte *buffer, size_t allowed, StreamPos entrySize);
    bool PrepareWrite(const Byte *data, uint32_t &maxlen);
    bool EnsureWriteCapacity(uint32_t &targetLen);
    uint32_t WriteMiniBlocks(const Byte *data, uint32_t targetLen, uint64_t smallBlockSize, uint64_t bigBlockSize);
    uint32_t WriteBigBlocks(const Byte *data, uint32_t targetLen, uint64_t bigBlockSize);

    StorageIO *io_ = nullptr;
    DirEntry *entry_ = nullptr;
    std::string path_;
    uint32_t cachedStart_{DIR_ENTRY_END};
    StreamPos cachedSize_{0};
    bool cachedUseBig_{false};
    std::vector<uint32_t> blocks_;
    std::streamsize pos_ = 0;  // 读写位置

    // 简单缓存，加速逐字节 Getch
    std::vector<Byte> cache_;
    std::streamsize cacheSize_;
    std::streamsize cachePos_;
    unsigned int state_ = 0;
    const size_t MAX_ALLOWED_2G = 2LL * 1024 * 1024 * 1024;

    StreamImpl();
    StreamImpl &operator=(const StreamImpl &);
};

}  // namespace ObjectEditor
}  // namespace OHOS

#endif  // STREAM_H