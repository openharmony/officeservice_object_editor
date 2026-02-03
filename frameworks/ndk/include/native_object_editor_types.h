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

#ifndef OHOS_OBJECT_EDITOR_NATIVE_OBJECT_EDITOR_TYPES_H
#define OHOS_OBJECT_EDITOR_NATIVE_OBJECT_EDITOR_TYPES_H

#include <string>
#include <vector>

#include "iremote_object.h"
#include "native_extension/context_impl.h"
#include "native_extension/extension_ability_impl.h"
#include "piexlmap_native_impl.h"
#include "pixel_map.h"

#include "content_embed_common.h"
#include "content_embed_extension.h"
#include "content_embed_proxy.h"
#include "iobject_editor_client_callback.h"
#include "object_editor_document.h"
#include "object_editor_format.h"

static constexpr uint32_t MAX_FILENAME_EXTENSIONS_NUM = 100;
struct ContentEmbed_Format {
    std::string hmid;
    std::string locale;
    std::string name;
    std::string description;
    std::shared_ptr<OHOS::Media::PixelMap> icon = nullptr;
    uint32_t fileNameExtensionsCount = 0;
    char **fileNameExtensions = nullptr;

    ~ContentEmbed_Format();
    void Build(std::unique_ptr<OHOS::ObjectEditor::ObjectEditorFormat> &format);
};

struct ContentEmbed_Info {
    std::vector<std::unique_ptr<ContentEmbed_Format>> formats;
    void Build(std::vector<std::unique_ptr<OHOS::ObjectEditor::ObjectEditorFormat>> &oeFormats);
};

struct ContentEmbed_Document {
    std::string hmid;
    bool isLink = false;
    std::string tmpFileUri;
    std::string oriFileUri;
    std::string nativeFileUri;
    std::string snapshotUri;
    std::unique_ptr<OHOS::ObjectEditor::ObjectEditorDocument> oeDocumentInner = nullptr;
};

struct ContentEmbed_ExtensionProxy {
    OHOS::sptr<OHOS::ObjectEditor::IObjectEditorService> oeExtensionRemoteObject = nullptr;
    OH_ContentEmbed_ClientCallbackOnUpdateFunc onUpdateFunc = nullptr;
    OH_ContentEmbed_ClientCallbackOnErrorFunc onErrorFunc = nullptr;
    OH_ContentEmbed_ClientCallbackOnEditingFinishedFunc onEditingFinishedFunc = nullptr;
    OH_ContentEmbed_ClientCallbackOnExtensionStoppedFunc onExtensionStoppedFunc = nullptr;
    ContentEmbed_Document *ceDocument = nullptr;
    bool isPackageExtension = false;
};

struct ContentEmbed_Storage {
    ContentEmbed_Document *owner = nullptr;
    std::string name;
};

struct ContentEmbed_Stream {
    ContentEmbed_Storage *owner = nullptr;
    std::string path;
    OHOS::ObjectEditor::Storage *storage = nullptr;
    OHOS::ObjectEditor::Stream *stream = nullptr;
    OHOS::ObjectEditor::StreamPos pos = 0;
};

struct ContentEmbed_Capability {
    bool isSupportSnapshot = false;
    bool isSupportEdit = false;
};

struct ContentEmbed_ExtensionInstance : public AbilityRuntime_ExtensionInstance {
    OH_ContentEmbed_Extension_OnCreateFunc onCreateFunc = nullptr;
    OH_ContentEmbed_Extension_OnDestroyFunc onDestroyFunc = nullptr;
    OH_ContentEmbed_Extension_OnObjectAttachFunc onObjectAttachFunc = nullptr;
    OH_ContentEmbed_Extension_OnObjectDetachFunc onObjectDetachFunc = nullptr;
    std::mutex objectsMutex;
    std::unordered_map<std::string, std::unique_ptr<ContentEmbed_Object>> objects;
};

struct ContentEmbed_Object {
    std::shared_ptr<ContentEmbed_ExtensionInstance> ceExtension = nullptr;
    std::unique_ptr<ContentEmbed_Document> document = nullptr;
    OHOS::sptr<OHOS::ObjectEditor::IObjectEditorClientCallback> clientCb = nullptr;
    OH_ContentEmbed_Extension_OnWriteToDataStreamFunc onWriteToDataStreamFunc = nullptr;
    OH_ContentEmbed_Extension_OnGetSnapshotFunc onGetSnapshotFunc = nullptr;
    OH_ContentEmbed_Extension_OnDoEditFunc onDoEditFunc = nullptr;
    OH_ContentEmbed_Extension_OnGetEditStatusFunc onGetEditStatusFunc = nullptr;
    OH_ContentEmbed_Extension_OnGetCapabilityFunc onGetCapabilityFunc = nullptr;
};

struct ContentEmbed_ExtensionContext : public AbilityRuntime_Context {
};

#endif // OHOS_OBJECT_EDITOR_NATIVE_OBJECT_EDITOR_TYPES_H