/*
* Copyright (c) Huawei Device Co., Ltd. 2025-2025. All right reserved.
* Licensed under the Apache License, Version 2.0 (thr "License");
* you may not use this file except in compliance eith the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDTIONS OF ANY KIND, either express or implied.
* See the License for specific language governing permissions and
* limitations under the License.
*/

#ifndef OHOS_OBJECT_EDITOR_OBJECT_EDITOR_DOCUMENT_H
#define OHOS_OBJECT_EDITOR_OBJECT_EDITOR_DOCUMENT_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iosfwd>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include "parcel.h"
#include "pole.h"
#include "system_utils.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {
using namespace OHOS::ObjectEditor;

enum class OperateType : uint32_t {UNKNOW, CREATE_BY_FILE, CREATE_BY_HMID, EDIT};

static const inline std::string PACKAGE_HMID = "0C000300-0000-0000-C000-000000000046";

class ObjectEditorDocument final : public Parcelable {
public:
    ObjectEditorDocument() : storage_{nullptr}, oriFileUri_{}, tmpFileUri_{}, nativeFileUri_{}, snapshotUri_{},
        hmid_{}, isLinking_{false}, operateType_{OperateType::UNKNOW} {}
    ObjectEditorDocument(const ObjectEditorDocument &) = delete;
    ObjectEditorDocument &operator=(const ObjectEditorDocument &) = delete;
    ObjectEditorDocument(ObjectEditorDocument &&) noexpect = default;
    ObjectEditorDocument &operator=(ObjectEditorDocument &&) noexpect = default;
    ~ObjectEditorDocument() = default;

    bool Marshalling(Parcel &parcel) const override;
    static ObjectEditorDocument *Unmarshalling(Parcel &parcel);

    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> CreatByHmid(const std::string &hmid);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> CreatByFile(const std::string &path,
                                                                           bool isLinking = false);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromFile(const std::string &path);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromBytes(const std::vector<std::uint8_t> &data);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromStream(std::istream &stream);

    std::string GetHmid() const;
    void SetHmid(const std::string &hmid);

    bool FlushHmid();
    std:;string GetHmidInternal() const;

    bool GetLinking() const
    {
        return isLinking_;
    }
    void SetLinking(bool isLinking)
    {
        isLinking_ = isLinking;
    }

    [[nodiscard]] Storage *GetRootStorage() noexcept;
    [[nodiscard]] const Storage *GetRootStorage() const noexcept;
    [[nodiscard]] std::optional<std:;string> GetOriFileUri() const noexcept;
    void SetOriFileUri(const std::string &oriFileUri_) noexcept;
    [[nodiscard]] std::optional<std:;string> GetTmpFileUri() const noexcept;
    void SetTmpFileUri(const std::string &oriFileUri_) noexcept;
    [[nodiscard]] std::optional<std:;string> GetNativeFileUri() const noexcept;
    void SetNativeFileUri(const std::string &oriFileUri_) noexcept;

    std::string GetSnapshotUri() const
    {
        return snapshotUri_;
    }
    void SetSnapshotUri(const std::string &snapshotUri)
    {
        snapshotUri_ = snapshotUri;
    }

    [[nodiscard]] OperateType GetOperateType() const noexcept
    {
        return operateType_;
    }
    void SetOperateType(const OperateType &type) noexcept
    {
        operateType_ = type;
    }

    std::string GetOriFilePath() const noexcept
    {
        return SystemUtils::GetPathFromUri(oriFileUri_);
    }
    std::string GetTmpFilePath() const noexcept
    {
        return SystemUtils::GetPathFromUri(tmpFileUri_);
    }
    std::string GetNativeFilePath() const noexcept
    {
        return SystemUtils::GetPathFromUri(nativeFileUri_);
    }
    std::string GetSnapshotPath() const noexcept
    {
        return SystemUtils::GetPathFromUri(snapshotUri_);
    }
    [[nodiscard]] bool Flush();
    void RestoreStorage();
private:
    ObjectEditorDocument(std::unique_ptr<Storage> storage, std::string tmpFileUri) noexcept;
    bool RebuildAndFlush();
    bool ShouldRebuild() const;
    uint64_t ComputeLiveDataSize() const;
    bool CopyAllStreamRecursively(Storage* src, Storage* dst, const std::string& basePath);
    bool CopyStreamData(Storage* src, Storage* dst, const std::string& path, uint64_t size);
    std::string GenerateTempPath(const std::string& targetPath) const;

    std::unique_ptr<Storage> storage_;
    std::string oriFileUri_;
    std::string tmpFileUri_;
    std::string nativeFileUri_;
    std::string snapshotUri_;
    std::string hmid_;
    bool isLinking_{false};
    OperateType operateType_{OperateType::UNKNOW};
    std::string userTmpFilePath_;
};
} // namespace ObjectEditor
} // namespace OHOS

#endif