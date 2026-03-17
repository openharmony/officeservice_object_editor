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

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_FORMAT_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_FORMAT_H

#include "parcel.h"
#include "pixel_map.h"

namespace OHOS {
namespace ObjectEditor {
struct ObjectEditorFormat : public Parcelable {
    std::string hmid;
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    std::string version;

    std::string locale;
    std::string formatName;
    std::string description;
    std::string fileExts;

    std::shared_ptr<Media::PixelMap> pIconPixelMap;
    int64_t createTime = 0;

    virtual bool Marshalling(Parcel &parcel) const override;
    static ObjectEditorFormat *Unmarshalling(Parcel &parcel);
};

} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_FORMAT_H