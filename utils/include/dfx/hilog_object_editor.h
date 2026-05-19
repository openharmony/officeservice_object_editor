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

#ifndef OHOS_OBJECT_EDITOR_HILOG_OBJECT_EDITOR_H
#define OHOS_OBJECT_EDITOR_HILOG_OBJECT_EDITOR_H

#include "hilog/log.h"

namespace OHOS {
namespace ObjectEditor {

static constexpr unsigned int OBJECT_EDITOR_HILOG_DOMAIN = 0xD005118;

// Set Domain ID, add new modules before UNDEFINED
enum class ObjectEditorDomain {
    COMMON,
    UTILS,
    SA,
    CLIENT,
    CLIENT_NDK,
    EXTENSION,
    EXTENSION_NDK,
    DOCUMENT,
    DATABASE,
    PACKAGE,
    UNDEFINED,
};

// Set Domain Tag, add new modules before OBJECT_EDITOR_MAKE_TAG(UNDEFINED)
#define OBJECT_EDITOR_MAKE_STR(name) #name
#define OBJECT_EDITOR_MAKE_TAG(name) OBJECT_EDITOR_MAKE_STR(CE_##name)
static constexpr const char* logTags[] = {
    OBJECT_EDITOR_MAKE_TAG(COMMON),
    OBJECT_EDITOR_MAKE_TAG(UTILS),
    OBJECT_EDITOR_MAKE_TAG(SA),
    OBJECT_EDITOR_MAKE_TAG(CLIENT),
    OBJECT_EDITOR_MAKE_TAG(CLIENT_NDK),
    OBJECT_EDITOR_MAKE_TAG(EXTENSION),
    OBJECT_EDITOR_MAKE_TAG(EXTENSION_NDK),
    OBJECT_EDITOR_MAKE_TAG(DOCUMENT),
    OBJECT_EDITOR_MAKE_TAG(DATABASE),
    OBJECT_EDITOR_MAKE_TAG(PACKAGE),
    OBJECT_EDITOR_MAKE_TAG(UNDEFINED),
};

static_assert(sizeof(logTags) / sizeof(logTags[0]) ==
    static_cast<size_t>(OHOS::ObjectEditor::ObjectEditorDomain::UNDEFINED) -
    static_cast<size_t>(OHOS::ObjectEditor::ObjectEditorDomain::COMMON) + 1,
    "logTags size not match ObjectEditorDomain size");

static inline const char* GetHiLogTag(OHOS::ObjectEditor::ObjectEditorDomain domain)
{
    size_t index = static_cast<size_t>(domain) -
        static_cast<size_t>(OHOS::ObjectEditor::ObjectEditorDomain::COMMON);
    if (index >= sizeof(logTags) / sizeof(logTags[0])) {
        return "UNDEFINED"; // Return default tag
    }
    return logTags[index];
}

static inline unsigned int GetHiLogDomain(OHOS::ObjectEditor::ObjectEditorDomain domain)
{
    return static_cast<unsigned int>(domain);
}

#define OBJECT_EDITOR_LOGE(domain, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_ERROR, \
    OHOS::ObjectEditor::OBJECT_EDITOR_HILOG_DOMAIN, \
    GetHiLogTag(domain), "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__)
#define OBJECT_EDITOR_LOGF(domain, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_FATAL, \
    OHOS::ObjectEditor::OBJECT_EDITOR_HILOG_DOMAIN, \
    GetHiLogTag(domain), "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__)
#define OBJECT_EDITOR_LOGW(domain, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_WARN, \
    OHOS::ObjectEditor::OBJECT_EDITOR_HILOG_DOMAIN, \
    GetHiLogTag(domain), "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__)
#define OBJECT_EDITOR_LOGI(domain, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_INFO, \
    OHOS::ObjectEditor::OBJECT_EDITOR_HILOG_DOMAIN, \
    GetHiLogTag(domain), "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__)
#define OBJECT_EDITOR_LOGD(domain, fmt, ...) HILOG_IMPL(LOG_CORE, LOG_DEBUG, \
    OHOS::ObjectEditor::OBJECT_EDITOR_HILOG_DOMAIN, \
    GetHiLogTag(domain), "[%{public}s]" fmt, __FUNCTION__, ##__VA_ARGS__)

} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_HILOG_OBJECT_EDITOR_H
