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

#include "accesstoken_kit.h"
#include "hap_token_info.h"

namespace OHOS {
namespace Security {
namespace AccessToken {

static ErrCode g_getHapTokenInfoResult = ERR_OK;
static HapTokenInfo g_hapTokenInfo;

void SetGetHapTokenInfoResult(ErrCode result)
{
    g_getHapTokenInfoResult = result;
}

void SetHapTokenInfoBundleName(const std::string &bundleName)
{
    g_hapTokenInfo.bundleName = bundleName;
}

ErrCode AccessTokenKit::GetHapTokenInfo(uint32_t tokenId, HapTokenInfo &hapTokenInfo)
{
    if (g_getHapTokenInfoResult != ERR_OK) {
        return g_getHapTokenInfoResult;
    }
    hapTokenInfo = g_hapTokenInfo;
    return ERR_OK;
}

} // namespace AccessToken
} // namespace Security
} // namespace OHOS