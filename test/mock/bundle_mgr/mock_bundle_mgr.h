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

#ifndef MOCK_BUNDLE_MGR_H
#define MOCK_BUNDLE_MGR_H

#include <gmock/gmock.h>
#include "bundle_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {

class MockBundleMgr : public IBundleMgr {
public:
    MOCK_METHOD(ErrCode, GetBundleInfoV9,
        (const std::string &bundleName, int32_t flags, BundleInfo &bundleInfo, int32_t userId), (override));
    MOCK_METHOD(bool, QueryExtensionAbilityInfos,
        (const ExtensionAbilityType &extensionType, const int32_t &userId,
         std::vector<ExtensionAbilityInfo> &extensionInfos), (override));
    MOCK_METHOD(sptr<IRemoteObject>, AsObject, (), (override));
};

} // namespace AppExecFwk
} // namespace OHOS

#endif // MOCK_BUNDLE_MGR_H