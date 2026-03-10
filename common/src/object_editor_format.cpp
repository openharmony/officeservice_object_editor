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
bool ObjectEditorFormat::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(hmid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write hmid failed");
        return false;
    }
    if (!parcel.WriteString(bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write bundleName failed");
        return false;
    }
    if (!parcel.WriteString(moduleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write moduleName failed");
        return false;
    }
    if (!parcel.WriteString(abilityName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write abilityName failed");
        return false;
    }
    if (!parcel.WriteString(version)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write version failed");
        return false;
    }
    if (!parcel.WriteString(locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write locale failed");
        return false;
    }
    if (!parcel.WriteString(formatName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write formatName failed");
        return false;
    }
    if (!parcel.WriteString(description)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write description failed");
        return false;
    }
    if (!parcel.WriteString(fileExts)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write fileExts failed");
        return false;
    }
    if (!parcel.WriteParcelable(pIconPixelMap.get())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write pIconPixelMap failed");
        return false;
    }
    if (!parcel.WriteInt64(createTime)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "write createTime failed");
        return false;
    }
    return true;
}

ObjectEditorFormat *ObjectEditorFormat::Unmarshalling(Parcel &parcel)
{
    ObjectEditorFormat *format = new (std::nothrow) ObjectEditorFormat();
    if (format == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "alloc format failed");
        return nullptr;
    }
    if (!parcel.ReadString(format->hmid)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read hmid failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->bundleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read bundleName failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->moduleName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read moduleName failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->abilityName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read abilityName failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->version)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read version failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->locale)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read locale failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->formatName)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read formatName failed");
        delete format;
        return nullptr;
    }

    if (!parcel.ReadString(format->description)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read description failed");
        delete format;
        return nullptr;
    }
    if (!parcel.ReadString(format->fileExts)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read fileExts failed");
        delete format;
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pIconPixelMap(parcel.ReadParcelable<Media::PixelMap>());
    format->pIconPixelMap = pIconPixelMap;
    if (!parcel.ReadInt64(format->createTime)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::COMMON, "read createTime failed");
        delete format;
        return nullptr;
    }
    return format;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
