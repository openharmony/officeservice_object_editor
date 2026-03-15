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

#ifndef OHOS_OBJECT_EDITOR_PACKAGE_DATA_H
#define OHOS_OBJECT_EDITOR_PACKAGE_DATA_H

#include "object_editor_document.h"

namespace OHOS {
namespace ObjectEditor {

class PackageData  {
public:
    PackageData() = default;
    ~PackageData() = default;
    static std::unique_ptr<PackageData> CreateByDocument(std::shared_ptr<ObjectEditorDocument> document);
    static std::unique_ptr<PackageData> LoadFromDocument(std::shared_ptr<ObjectEditorDocument> document);
    bool SaveData();
    const std::string &GetFilePath() const
    {
        return filepath_;
    }

private:
    bool ParseOle10NativeStream(Stream *stream, const std::string &oriFilePath);
    bool FormatOle10NativeStream(const std::string &tmpFilePath, std::vector<Byte> &buffer, bool &withData);
    bool WriteFileToSandbox(Stream *stream, StreamPos &offset, const std::string &oriFilePath);
    bool WriteDataToStream(Stream *stream);
    bool WriteDataToBuffer(std::vector<Byte> &buffer);
    uint32_t fileSize_ = 0;
    std::string filename_;
    std::string filepath_;
    std::string fileLink_;
    uint32_t dataSize_ = 0;
    std::weak_ptr<ObjectEditorDocument> document_;
};
} // namespace ObjectEditor
} // namespace OHOS
#endif // OHOS_OBJECT_EDITOR_PACKAGE_DATA_H
