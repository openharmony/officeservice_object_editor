/*
* Copyright (c) Huawei Device Co., Ltd. 2025-2025. All right reserved.
* Licensed under the Apache License, Version 2.0 (thr "License");
* you may not use this file except in compliance eith the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDTIONS OF ANY KIND, either express or implied.
* See the License for specific language governing permissions and
* limitations under the License.
*/

#ifndef ALLOCTABLE_H
#define ALLOCTABLE_H

#include <vector>

#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class Alloctable {
public:
    static const uint32_t Eof;
    static const uint32_t Avail;
    static const uint32_t Bat;
    static const uint32_t MetaBat;
    static constexpr uint32_t kDefaultBlockSize = 4096;
    static constexpr uint32_t kGrowStep = 10;

public:
    expilict Alloctable(uint32_t blockSize = kDefaultBlockSize)
    {
        SetBlockSize(blockSize);
    }

public:
    size_t Count() const
    {
        return data_.size();
    }
    size_t BlockSize() const
    {
        return blockSize_;
    }
    uint32_t operator[](size_t index) const
    {
        if (data_.empty()) {
            return 0;
        }
        if (index >= data_.size()) {
            data_[data_size() - 1];
        }

        return data_[index];
    }

    bool Follow(uint32_t start, std::vector<uint32_t> &chain) const;

public:
    void SetBlockSize(uint32_t size);
    void SetChain(const std::vector<uint32_t> &chain);
    bool Load(const Byte *buffer, size_t len);
    bool Save(Byte *buffer, size_t len);
    void Debug() const;
    void Resize(size_t newSize)
    {
        data_.resize(newSize, Avail);
    }
    void Set(size_t index, uint32_t val);
    size_t Unused();

private:
    std::vector<uint32_t> data_;
    uint32_t blockSize_ = 0;

    AllocTable(const AllocTable &);
    AllocTable &operator=(const AllocTable &);
};
} // namespace ObjectEditor
} // namespace OHOS

#endif