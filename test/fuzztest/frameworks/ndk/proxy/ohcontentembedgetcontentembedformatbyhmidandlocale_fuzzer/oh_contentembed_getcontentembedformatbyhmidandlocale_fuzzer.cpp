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
#include "oh_contentembed_getcontentembedformatbyhmidandlocale_fuzzer.h"

using namespace OHOS::ObjectEditor;
namespace OHOS {
namespace {
constexpr int MIN_DATA_SIZE = 10;
constexpr int SPILT_NUM = 2;
constexpr int RANDOM_NULL = 10;
}

bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (size > MIN_DATA_SIZE) {
        ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
        config.isSupportObjectEditor_.isLoaded = true;
        config.isSupportObjectEditor_.value = true;
    }
    int32_t hmidSize = size / SPILT_NUM + 1;
    char *hmid = static_cast<char*>(malloc(sizeof(char) * hmidSize));
    if (hmid == nullptr) {
        return false;
    }
    if (memcpy_s(hmid, size / SPILT_NUM, data, size / SPILT_NUM) != EOK) {
        free(hmid);
        return false;
    }
    hmid[hmidSize - 1] = 0;
    int32_t offset = size - (size / SPILT_NUM);
    int32_t localeSize = static_cast<int32_t>(offset) + 1;
    char *locale = static_cast<char*>(malloc(sizeof(char) * localeSize));
    if (locale == nullptr) {
        free(hmid);
        return false;
    }
    if (memcpy_s(locale, localeSize, data + size / SPILT_NUM, offset) != EOK) {
        free(hmid);
        free(locale);
        return false;
    }
    locale[offset] = 0;
    ContentEmbed_Format format;
    if (size % RANDOM_NULL != 0) {
        OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(hmid, locale, &format);
    } else {
        OH_ContentEmbed_GetContentEmbedFormatByOEidAndLocale(hmid, locale, nullptr);
    }
    free(hmid);
    free(locale);
    return true;
}
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    if (data == nullptr) {
        return 0;
    }
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}