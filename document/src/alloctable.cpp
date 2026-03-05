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
#include <iomanip>
#include <sstream>

#include "alloctable.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
const uint32_t AllocTable::Avail = 0xffffffff;
const uint32_t AllocTable::Eof = 0xfffffffe;
const uint32_t AllocTable::Bat = 0xfffffffd;
const uint32_t AllocTable::MetaBat = 0xfffffffc;

void AllocTable::Set(size_t index, uint32_t value)
{
    if (index >= Count()) {
        Resize(index + 1);
    }
    data_[index] = value;
}
void AllocTable::SetBlockSize(uint32_t size)
{
    blockSize_ = size;
    data_.clear();
    Resize(DEFAULT_BLOCK_SIZE);
}
void AllocTable::SetChain(const std::vector<uint32_t> &chain)
{
    if (chain.empty()) {
        return;
    }
    const auto maxIt = std::max_element(chain.begin(), chain.end());
    if (*maxIt >= Count()) {
        Resize(*maxIt + 1);
    }

    size_t chainSize = chain.size() - 1;
    for (size_t i = 0; i < chainSize; i++) {
        Set(chain[i], chain[i + 1]);
    }
    Set(chain.back(), Eof);
}

bool AllocTable::Follow(uint32_t start, std::vector<uint32_t> &chain) const
{
    if (start == Eof || start == Avail || start == MetaBat || start == Bat) {
        return true;
    }
    if (start >= Count()) {
        return false;
    }

    uint32_t p = start;
    size_t loopControl = 0;
    while (p < Count()) {
        if (loopControl >= Count()) {
            return false;
        }
        chain.push_back(p);
        p = data_[p];
        ++loopControl;
    }

    if (p != Eof && p != Avail && p != Bat && p != MetaBat) {
        return false;
    }

    return true;
}

size_t AllocTable::Unused()
{
    for (size_t i = 0; i < data_.size(); i++) {
        if (data_[i] == Avail) {
            return i;
        }
    }

    size_t block = data_.size();
    Resize(data_.size() + kGrowStep);
    return block;
}

bool AllocTable::Load(const Byte *buffer, size_t len)
{
    if ((len % FOUR_BYTE_SIZE != 0) || buffer == nullptr) [[unlikely]] {
        return false;
    }
    Resize(len / FOUR_BYTE_SIZE);
    size_t count = Count();
    for (size_t i = 0; i < count; i++)
        Set(i, ReadUint32(buffer + i * FOUR_BYTE_SIZE));
    return true;
}

bool AllocTable::Save(Byte *buffer, size_t len)
{
    size_t count = Count();
    const size_t required = FOUR_BYTE_SIZE * count;
    if (len < required || buffer == nullptr) [[unlikely]] {
        return false;
    }

    for (size_t i = 0; i < count; i++) {
        WriteUint32(buffer + i * FOUR_BYTE_SIZE, data_[i]);
    }
    return true;
}

void AllocTable::Debug() const
{
    constexpr int32_t MINI_WIDTH = 4;
    std::ostringstream oss;
    oss << "==================== FAT ALLOCATION TABLE ====================" << std::endl;
    const size_t totalBlocks = data_.size();
    size_t usedBlocks = 0;
    size_t freeBlocks = 0;
    for (size_t i = 0; i < totalBlocks; ++i) {
        const uint32_t val = static_cast<uint32_t>(data_[i] & 0xFFFFFFFFUL);
        if (val == Avail) {
            ++freeBlocks;
        } else {
            ++usedBlocks;
        }
    }

    oss << "Total Blocks:   " << totalBlocks << std::endl;
    oss << "Used Blocks:    " << usedBlocks << std::endl;
    oss << "Free Blocks:    " << freeBlocks << std::endl;
    oss << "Block Size:     " << BlockSize() << " bytes" << std::endl;

    for (size_t i = 0; i < totalBlocks; ++i) {
        const uint32_t val = static_cast<uint32_t>(data_[i] & 0xFFFFFFFFUL);
        if (val == Avail) {
            continue;
        }
        oss << "0x" << std::hex << std::setw(MINI_WIDTH) << std::setfill('0') << i << std::dec << ": ";

        if (val == Eof) {
            oss << "[EOF]";
        } else if (val == Bat) {
            oss << "[FAT]";
        } else if (val == MetaBat) {
            oss << "[MetaFAT]";
        } else {
            oss << "-> 0x" << std::hex << std::setw(MINI_WIDTH) << std::setfill('0') << static_cast<uint64_t>(val)
                << std::dec;
        }
        oss << std::endl;
    }

    oss << "============================================================" << std::endl;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "AllocTable dump: %{public}s", oss.str().c_str());
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS