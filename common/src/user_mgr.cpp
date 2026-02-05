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

std::vector<int32_t> GetAllActiveUserIds()
{
    ErrCode result = -1;
    std::vector<int32_t> userIds;
    int retry = 0;
    while ((result != ERR_OK || userIds.empty()) && retry < MAX_RETRY) {
        result = AccountSA::OsAccountManager::QueryActiveOsAccountIds(userIds);
        if (result != ERR_OK) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Query active os account ids failed, retry: %{public}d", retry);
            sleep(RETRY_INTERVAL_SECOND);
            retry++;
            continue;
        }
        if (userIds.empty()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Query active os account ids failed, retry: %{public}d", retry);
            sleep(RETRY_INTERVAL_SECOND);
            retry++;
            continue;
        }
    }
    std::string osAccountIdsStr = "";
    for (auto userId : userIds) {
        osAccountIdsStr.append(std::to_string(userId) + ",");
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::COMMON, "GetAllActiveUserIds success");
    return userIds;
}

int32_t GetCurrentUserId()
{
    std::vector<int32_t> userIds = GetAllActiveUserIds();
    if (userIds.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Get all active user ids failed");
        return DEFAULT_USER_ID;
    }
    int32_t currentUserId = userIds[0];
    if (currentUserId <= DEFAULT_USER_ID) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Current user id is invalid");
        return DEFAULT_USER_ID;
    }
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::COMMON, "GetCurrentUserId success");
    return currentUserId;
}
} // namespace

UserMgr::UserMgr()
{
    userId_ = GetCurrentUserId();
}

UserMgr &UserMgr::GetInstance()
{
    static UserMgr instance;
    return instance;
}

void UserMgr::SetNewUserId(int32_t userId)
{
    std::unique_lock<std::mutex> lock(mtx_);
    userId_ = newUserId;
}

int32_t UserMgr::GetUserId()
{
    std::shared_lock<std::shared_mutex> lock(mtx_);
    return userId_;
}

} // namespace ObjectEditor
} // namespace OHOS
