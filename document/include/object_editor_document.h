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

enum class OperateType : uint32_t { UNKNOWN, CREATE_BY_FILE, CREATE_BY_OEID, EDIT };

static const inline std::string PACKAGE_OEID = "0003000C-0000-0000-C000-000000000046";

class ObjectEditorDocument final : public Parcelable {
public:
    ObjectEditorDocument() : storage_{nullptr}, oriFileUri_{}, tmpFileUri_{}, nativeFileUri_{}, snapshotUri_{},
        oeid_{}, isLinking_{false}, operateType_{OperateType::UNKNOWN}, documentId_{} {}
    ObjectEditorDocument(const ObjectEditorDocument &) = delete;
    ObjectEditorDocument &operator=(const ObjectEditorDocument &) = delete;
    ObjectEditorDocument(ObjectEditorDocument &&) noexcept = default;
    ObjectEditorDocument &operator=(ObjectEditorDocument &&) noexcept = default;
    ~ObjectEditorDocument();

    bool Marshalling(Parcel &parcel) const override;
    static ObjectEditorDocument *Unmarshalling(Parcel &parcel);

    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> CreateByOEid(const std::string &oeid);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> CreateByFile(const std::string &path,
                                                                            bool isLinking = false);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromFile(const std::string &path);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromBytes(const std::vector<std::uint8_t> &data);
    [[nodiscard]] static std::unique_ptr<ObjectEditorDocument> LoadFromStream(std::istream &stream);

    std::string GetOEid() const;
    void SetOEid(const std::string &oeid);

    bool FlushOEid();
    std::string GetOEidInternal() const;

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
    [[nodiscard]] std::optional<std::string> GetOriFileUri() const noexcept;
    void SetOriFileUri(const std::string &oriFileUri) noexcept;
    [[nodiscard]] std::optional<std::string> GetTmpFileUri() const noexcept;
    void SetTmpFileUri(const std::string &tmpFileUri) noexcept;
    [[nodiscard]] std::optional<std::string> GetNativeFileUri() const noexcept;
    void SetNativeFileUri(const std::string &nativeFileUri) noexcept;

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

    std::string GetDocumentId() const;
    void SetDocumentId(const std::string &documentId);
private:
    ObjectEditorDocument(std::unique_ptr<Storage> storage, std::string tmpFileUri) noexcept;
    bool RebuildAndFlush();
    bool ShouldRebuild() const;
    uint64_t ComputeLiveDataSize() const;
    bool CopyAllStreamsRecursively(Storage* src, Storage* dst, const std::string& basePath);
    bool CopyAllStreamRecursivelyImpl(Storage *src, Storage *dst, const std::string& path,
        std::size_t depth, std::size_t &visitCount);
    bool CopyStreamData(Storage* src, Storage* dst, const std::string& path, uint64_t size);
    bool HandleUserTempScenario();
    bool HandleTempFilePathScenario();
    void TraverseDirectory(const std::string &path, std::size_t depth,
        uint64_t &total, std::size_t &visitCount) const;
    bool GenerateAndSaveUniqueFile();

    std::unique_ptr<Storage> storage_;
    std::string oriFileUri_;
    std::string tmpFileUri_;
    std::string nativeFileUri_;
    std::string snapshotUri_;
    std::string oeid_;
    bool isLinking_{false};
    OperateType operateType_{OperateType::UNKNOWN};
    std::string userTmpFilePath_;
    std::string documentId_;
};
} // namespace ObjectEditor
} // namespace OHOS

#endif // OHOS_OBJECT_EDITOR_OBJECT_EDITOR_DOCUMENT_H