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
#include "mock_access_token_verify.h"

namespace OHOS {
namespace Security {
namespace AccessToken {

static int g_verifyAccessTokenResult = MOCK_PERMISSION_DENIED;
static AccessTokenID g_getHapTokenIDResult = 0;

void SetVerifyAccessTokenResult(int result)
{
    g_verifyAccessTokenResult = result;
}

void SetGetHapTokenIDResult(uint32_t tokenID)
{
    g_getHapTokenIDResult = static_cast<AccessTokenID>(tokenID);
}

void ResetAccessTokenVerifyMock()
{
    g_verifyAccessTokenResult = MOCK_PERMISSION_DENIED;
    g_getHapTokenIDResult = 0;
}

int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string &permissionName)
{
    return g_verifyAccessTokenResult;
}

AccessTokenID AccessTokenKit::GetHapTokenID(int32_t userID, const std::string &bundleName, int32_t instIndex)
{
    return g_getHapTokenIDResult;
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
