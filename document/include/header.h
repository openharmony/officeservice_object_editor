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

#ifndef HEADR_H
#define HEADR_H

#include <array>
#include <cstddef>
#include <cstdint>

#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class Header {
public:
    Header();
    const Byte *Id() const
    {
        return id_;
    }

    uint16_t BigBlockShift() const
    {
        return bigBlockShift_;
    }
    uint16_t SmallBlockShift() const
    {
        return miniBlockShift_;
    }
    BlockCount NumBat() const
    {
        return numBat_;
    }
    SectorIndex DirentStart() const
    {
        return direntStart_;
    }
    uint32_t Threshold() const
    {
        return threshold_;
    }
    SectorIndex SbatStart() const
    {
        return sbatStart_;
    }
    BlockCount NumSbat() const
    {
        return numSbat_;
    }
    SectorIndex DifatStart() const
    {
        return difatStart_;
    }
    BlockCount NumDifat() const
    {
        return numDifat_;
    }
    const CLSID &Clsid() const
    {
        return clsid_;
    }
    const SectorIndex *BbBlocks() const
    {
        return bbBlocks_;
    }
    bool Valid() const;
    bool IsCompoundDocument() const;
public:
    bool Load(const Byte *buffer, size_t len);
    bool Save(Byte *buffer, size_t len);
    void Debug();

    void SetFatSectorCount(BlockCount value)
    {
        numBat_ = value;
    }
    void SetFirstDirSector(SectorIndex value)
    {
        direntStart_ = value;
    }
    void SetFirstMiniFat(SectorIndex value)
    {
        sbatStart_ = value;
    }
    void SetMiniFatCount(BlockCount value)
    {
        numSbat_ = value;
    }
    void SetFirstDifat(SectorIndex value)
    {
        difatStart_ = value;
    }
    void SetDifatCount(BlockCount value)
    {
        numDifat_ = value;
    }
    void SetDifatElem(size_t index, SectorIndex value)
    {
        if (index < HEADER_DIFAT_ARRAY_SIZE) {
            bbBlocks_[index] = value;
        }
    }

private:
    Byte id[FILE_SIGNATURE_SIZE]{};
    CLSID clsid_{};
    uint16_t minorVersion_ = 0;
    uint16_t majorVersion_ = 0;
    uint16_t byteOrder_ = 0;
    uint16_t bigBlockShift_ = 0;
    uint16_t miniBlockShift_ = 0;
    BlockCount dirSectorCount_ = 0;
    BlockCount numBat_ = 0;
    SectorIndex direntStart_ = 0;
    uint32_t transactionSignature_ = 0;
    uint32_t threshold_ = 0;
    SectorIndex sbatStart_ = 0;
    BlockCount numSbat_ = 0;
    SectorIndex difatStart_ = 0;
    BlockCount numDifat_ = 0;
    SectorIndex bbBlocks_[HEADER_DIFAT_ARRAY_SIZE]{};
};
} // namespace ObjectEditor
} // namespace OHOS

#endif