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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONFIG_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONFIG_H

#include <mutex>
#include "single_instance.h"
#include "content_embed_common.h"

namespace OHOS {
namespace ObjectEditor {
template<typename T>
class DeviceConfiguration {
public:
    bool isLoaded = false;
    T value;
};

class ObjectEditorConfig {
DECLARE_SINGLE_INSTANCE(ObjectEditorConfig);
public:
    /**
     * @brief Check if the object editor is supported.
     *
     * @return true if the object editor is supported, false otherwise.
     */
    bool IsSupportObjectEditor();
    /**
     * @brief Check if the device is in DLP environment.
     *
     * @return true if the device is in DLP environment, false otherwise.
     */
    bool CheckIsInDlp();
    /**
     * @brief Check if the object editor is supported on the device.
     *
     * @return ContentEmbed_ErrorCode::CE_ERR_OK if the object editor is supported,
     *         ContentEmbed_ErrorCode::CE_ERR_DEVICE_NOT_SUPPORTED if device not support,
     *         ContentEmbed_ErrorCode::CE_ERR_IN_DLP_SANDBOX if device in dlp environment.
     */
    ContentEmbed_ErrorCode CheckIsSupported();
private:
    DeviceConfiguration<bool> isSupportObjectEditor_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_CONFIG_H
