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

#include "user_mgr.h"

#include <string>
#include <vector>
#include "os_account_manager.h"

#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {

namespace {
constexpr int32_t RETRY_INTERVAL_SECOND = 1;
constexpr int32_t MAX_RETRY = 4;
constexpr int32_t DEFAULT_USER_ID = 100;
// LCOV_EXCL_START
std::vector<int32_t> GetAllActiveUserIds()
{
    ErrCode result = -1;
    std::vector<int32_t> osAccountIds;
    int retry = 0;
    while ((result != ERR_OK || osAccountIds.empty()) && retry < MAX_RETRY) {
        result = AccountSA::OsAccountManager::QueryActiveOsAccountIds(osAccountIds);
        if (result != ERR_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON,
                "Query active os account ids failed, retry: %{public}d", retry);
            sleep(RETRY_INTERVAL_SECOND);
            retry++;
            continue;
        }
        if (osAccountIds.empty()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON,
                "Query active os account ids failed, retry: %{public}d", retry);
            sleep(RETRY_INTERVAL_SECOND);
            retry++;
            continue;
        }
    }
    return osAccountIds;
}

int32_t GetCurrentUserId()
{
    std::vector<int32_t> osAccountIds = GetAllActiveUserIds();
    if (osAccountIds.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Get all active user ids failed");
        return DEFAULT_USER_ID;
    }
    int32_t currentOsAccount = osAccountIds[0];
    if (currentOsAccount <= DEFAULT_USER_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Current os account id is invalid");
        return DEFAULT_USER_ID;
    }
    return currentOsAccount;
}
} // namespace

IMPLEMENT_SINGLE_INSTANCE(UserMgr);

UserMgr::UserMgr()
{
    userId_ = GetCurrentUserId();
}

void UserMgr::SetNewUserId(int32_t newUserId)
{
    std::unique_lock<std::shared_mutex> lock(mtx_);
    userId_ = newUserId;
}

int32_t UserMgr::GetUserId()
{
    std::shared_lock<std::shared_mutex> lock(mtx_);
    userId_ = GetCurrentUserId();
    return userId_;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
