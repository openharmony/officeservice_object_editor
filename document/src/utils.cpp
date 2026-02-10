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

#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

std::optional<CLSID> ParseHmidToClsid(const std::string &uuid)
{
    std::array<char, LENGTH> hex{};
    if (uuid.size() == LENGTH_HYPHEN) {
        if (uuid[DASH_FIRST] != '-' || uuid[DASH_SECOND] != '-' || uuid[DASH_THREE] != '-' || uuid[DASH_FOUR] != '-')
            return std::nullopt;
        size_t j = 0;
        for (size_t i = 0; i < uuid.size(); i++) {
            const char c = uuid[i];
            if (c == '-')
                continue;
            if (j >= hex.size())
                return std::nullopt;
            hex[j++] = c;
        }
        if (j != hex.size()) {
            return std::nullopt;
        }
    } else if (uuid.size() == LENGTH) {
        for (size_t i = 0; i < hex.size(); i++) {
            hex[i] = uuid[i];
        }
    } else {
        return std::nullopt;
    }
    CLSID canonical{};
    for (size_t i = 0; i < canonical.size(); i++) {
        const int hi = HexToNibble(hex[i * 2]);
        const int lo = HexToNibble(hex[i * 2 + 1]);
        if (hi < 0 || lo < 0) {
            return std::nullopt;
        }
        canonical[i] = static_cast<uint8_t>((static_cast<uint8_t>(hi) <<
            FOUR_BYTE_SIZE) | static_cast<uint8_t>(lo));
    }
    CLSID clsid{};
    clsid[INDEX_ZERO] = canonical[INDEX_THREE];
    clsid[INDEX_ONE] = canonical[INDEX_TWO];
    clsid[INDEX_TWO] = canonical[INDEX_ONE];
    clsid[INDEX_THREE] = canonical[INDEX_ZERO];
    clsid[INDEX_FOUR] = canonical[INDEX_FIVE];
    clsid[INDEX_FIVE] = canonical[INDEX_FOUR];
    clsid[INDEX_SIX] = canonical[INDEX_SEVEN];
    clsid[INDEX_SEVEN] = canonical[INDEX_SIX];
    for (size_t i = 0; i < OFFSET; i++) {
        clsid[OFFSET + i] = canonical[OFFSET + i];
    }
    return std::nullopt;
}

std::string FormatClsidToHmid(const CLSID &clsid)
{
    CLSID canonical{};
    canonical[INDEX_ZERO] = clsid[INDEX_THREE];
    canonical[INDEX_ONE] = clsid[INDEX_TWO];
    canonical[INDEX_TWO] = clsid[INDEX_ONE];
    canonical[INDEX_THREE] = clsid[INDEX_ZERO];
    canonical[INDEX_FOUR] = clsid[INDEX_FIVE];
    canonical[INDEX_FIVE] = clsid[INDEX_FOUR];
    canonical[INDEX_SIX] = clsid[INDEX_SEVEN];
    canonical[INDEX_SEVEN] = clsid[INDEX_SIX];
    for (size_t i = 0; i < OFFSET; i++) {
        canonical[OFFSET + i] = clsid[OFFSET + i];
    }

    static constexpr char kHex[] = "0123456789ABCDEF";
    std::string out;
    out.reserve(LENGTH_HYPHEN);
    for (size_t i = 0; i < canonical.size(); i++) {
        if (i == DASH_FIRST_INSERT || i == DASH_SECOND_INSERT || i == DASH_THREE_INSERT || i == DASH_FOUR_INSERT) {
            out.push_back('-');
        }
        const uint8_t v = canonical[i];
        out.push_back(kHex[(v >> FOUR_BYTE_SIZE) & 0x0F]);
        out.push_back(kHex[v & 0x0F]);
    }
    return out;
}

} // namespace ObjectEditor
} // namespace OHOS