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

#include "object_editor_format.h"
#include "hilog_object_editor.h"
#include "pixel_map.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
bool ObjectEditorFormat::Marshalling(Parcel &data) const
{
    if (!data.WriteString(hmid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write hmid failed");
        return false;
    }
    if (!data.WriteString(bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write bundleName failed");
        return false;
    }
    if (!data.WriteString(moduleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write moduleName failed");
        return false;
    }
    if (!data.WriteString(abilityName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write abilityName failed");
        return false;
    }
    if (!data.WriteString(version)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write version failed");
        return false;
    }
    if (!data.WriteString(locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write locale failed");
        return false;
    }
    if (!data.WriteString(formatName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write formatName failed");
        return false;
    }
    if (!data.WriteString(description)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write description failed");
        return false;
    }
    if (!data.WriteString(fileExts)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write fileExts failed");
        return false;
    }
    if (!data.WriteParcelable(pIconPixelMap.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write pIconPixelMap failed");
        return false;
    }
    if (!data.WriteInt64(createTime)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Write createTime failed");
        return false;
    }
    return true;
}

ObjectEditorFormat *ObjectEditorFormat::Unmarshalling(Parcel &data)
{
    ObjectEditorFormat *format = new (std::nothrow) ObjectEditorFormat();
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "alloc format failed");
        return nullptr;
    }
    if (!data.ReadString(format->hmid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read hmid failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read bundleName failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->moduleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read moduleName failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->abilityName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read abilityName failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->version)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read version failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read locale failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->formatName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read formatName failed");
        delete format;
        return nullptr;
    }

    if (!data.ReadString(format->description)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read description failed");
        delete format;
        return nullptr;
    }
    if (!data.ReadString(format->fileExts)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read fileExts failed");
        delete format;
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pIconPixelMap = data.ReadParcelable<Media::PixelMap>();
    format->pIconPixelMap = pIconPixelMap;
    if (!data.ReadInt64(format->createTime)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "Read createTime failed");
        delete format;
        return nullptr;
    }
    return format;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
