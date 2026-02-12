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

#include "fuzzer/FuzzedDataProvider.h"
#include "native_object_editor_types.h"
#include "object_editor_extension_proxy.h"
#include "oh_contentembed_createcontentembedinfo_fuzzer.h"

using namespace OHOS::ObjectEditor;

namespace {
constexpr float IS_CONFIG = 0.2;
}
namespace OHOS {
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    bool isConfig = (provider.ConsumeProbability<float>()) > IS_CONFIG;
    if (isConfig) {
        ObjectEditorConfig &config = ObjectEditorConfig::GetInstance();
        config.isSupportObjectEditor_.isLoaded = true;
        config.isSupportObjectEditor_.value = true;
    }
    ContentEmbed_Info *info = nullptr;
    if (provider.ConsumeBool()) {
        OH_ContentEmbed_CreateContentEmbedInfo(&info);
    } else {
        OH_ContentEmbed_CreateContentEmbedInfo(nullptr);
    }
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