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

#define private public
#include "object_editor_config.h"
#undef private

#include "native_object_editor_types.h"
#include "object_editor_extension_proxy.h"
#include "oh_contentembed_getcontentembedinfo_fuzzer.h"

using namespace OHOS::ObjectEditor;
namespace {
constexpr int MIN_DATA_SIZE = 10;
constexpr int RANDOM_NULL = 10;
}
namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (size > MIN_DATA_SIZE) {
        ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
        config.isSupportObjectEditor_.isLoaded = true;
        config.isSupportObjectEditor_.value = true;
    }
    int32_t localeSize = static_cast<int32_t>(size) + 1;
    char *locale = static_cast<char*>(malloc(sizeof(char) * localeSize));
    if (locale == nullptr) {
        return false;
    }
    if (memcpy_s(locale, localeSize, data, size) != EOK) {
        free(locale);
        return false;
    }
    locale[size] = 0;
    if (size % RANDOM_NULL != 0) {
        ContentEmbed_Info info;
        OH_ContentEmbed_GetContentEmbedInfo(locale, &info);
    } else {
        OH_ContentEmbed_GetContentEmbedInfo(locale, nullptr);
    }
    free(locale);
    return true;
}
}

// Fuzzer entry point
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    if (data == nullptr) {
        return 0;
    }
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}