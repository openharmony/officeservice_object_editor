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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <utility>
#include <vector>

#include "application_context.h"

#include "dirtree.h"
#include "object_editor_document.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
    constexpr std::size_t MAX_DEPTH = 256;
    constexpr std::size_t MAX_VISITS = 10000;
}
// LCOV_EXCL_START
namespace fs = std::filesystem;

ObjectEditorDocument::~ObjectEditorDocument()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "destructor oeid: %{public}s, documentId: %{private}s",
        oeid_.c_str(), documentId_.c_str());
}

ObjectEditorDocument::ObjectEditorDocument(std::unique_ptr<Storage> storage,
    std::string tmpFilePath) noexcept
    : storage_(std::move(storage)),
      tmpFileUri_(SystemUtils::GetUriFromPath(tmpFilePath))
{
    if (!storage_) {
        return;
    }
    DirEntry *root = storage_->GetRootEntry();
    if (root) {
        oeid_ = FormatClsidToOEid(root->Clsid());
    }
}

std::unique_ptr<ObjectEditorDocument> ObjectEditorDocument::CreateByOEid(const std::string &oeid)
{
    auto storage = std::make_unique<Storage>(oeid);
    if (!storage || storage->Result() != Storage::Ok) {
        return nullptr;
    }
    ObjectEditorDocument instance(std::move(storage), std::string{});
    instance.oeid_ = oeid;
    instance.operateType_ = OperateType::CREATE_BY_OEID;
    return std::make_unique<ObjectEditorDocument>(std::move(instance));
}

std::unique_ptr<ObjectEditorDocument> ObjectEditorDocument::CreateByFile(
    const std::string &path, [[maybe_unused]] bool isLinking)
{
    static constexpr char kDefaultOEid[] = "00000000000000000000000000000000";
    auto documentPtr = CreateByOEid(kDefaultOEid);
    if (!documentPtr) {
        return nullptr;
    }
    documentPtr->oriFileUri_ = SystemUtils::GetUriFromPath(path);
    documentPtr->operateType_ = OperateType::CREATE_BY_FILE;
    documentPtr->SetLinking(isLinking);
    return documentPtr;
}

std::unique_ptr<ObjectEditorDocument> ObjectEditorDocument::LoadFromFile(const std::string &path)
{
    auto storage = std::make_unique<Storage>(path.c_str());
    if (!storage || storage->Result() != Storage::Ok) {
        return nullptr;
    }

    ObjectEditorDocument instance(std::move(storage), std::string{});
    instance.operateType_ = OperateType::EDIT;
    instance.userTmpFilePath_ = path;
    return std::make_unique<ObjectEditorDocument>(std::move(instance));
}

std::string ObjectEditorDocument::GetOEid() const
{
    return oeid_;
}

void ObjectEditorDocument::SetOEid(const std::string &oeid)
{
    oeid_ = oeid;
}

bool ObjectEditorDocument::FlushOEid()
{
    if (!storage_) {
        return false;
    }
    DirEntry *root = storage_->GetRootEntry();
    if (!root) {
        return false;
    }

    const auto clsidOpt = ParseOEidToClsid(oeid_);
    if (!clsidOpt) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "parse oeid failed");
        return false;
    }

    root->SetClsid(*clsidOpt, std::size(*clsidOpt));
    return storage_->Flush();
}

std::string ObjectEditorDocument::GetOEidInternal() const
{
    if (!storage_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "storage_ is null");
        return std::string{};
    }
    DirEntry *root = storage_->GetRootEntry();
    if (!root) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "root entry is null");
        return std::string{};
    }
    return std::string(reinterpret_cast<const char*>(root->Clsid().data()), root->Clsid().size());
}

Storage *ObjectEditorDocument::GetRootStorage() noexcept
{
    return storage_.get();
}

const Storage *ObjectEditorDocument::GetRootStorage() const noexcept
{
    return storage_.get();
}

std::optional<std::string> ObjectEditorDocument::GetOriFileUri() const noexcept
{
    if (oriFileUri_.empty()) {
        return std::nullopt;
    }
    return oriFileUri_;
}

void ObjectEditorDocument::SetOriFileUri(const std::string &oriFileUri) noexcept
{
    oriFileUri_ = oriFileUri;
}

std::optional<std::string> ObjectEditorDocument::GetTmpFileUri() const noexcept
{
    if (tmpFileUri_.empty()) {
        return std::nullopt;
    }
    return tmpFileUri_;
}

void ObjectEditorDocument::SetTmpFileUri(const std::string &tmpFileUri) noexcept
{
    tmpFileUri_ = tmpFileUri;
}

std::optional<std::string> ObjectEditorDocument::GetNativeFileUri() const noexcept
{
    if (nativeFileUri_.empty()) {
        return std::nullopt;
    }
    return nativeFileUri_;
}

void ObjectEditorDocument::SetNativeFileUri(const std::string &nativeFileUri) noexcept
{
    nativeFileUri_ = nativeFileUri;
}

void ObjectEditorDocument::RestoreStorage()
{
    storage_ = std::make_unique<Storage>(GetTmpFilePath().c_str());
}

bool ObjectEditorDocument::HandleUserTempScenario()
{
    if (!storage_) {
        return false;
    }
    const bool hasUserTmp = !userTmpFilePath_.empty();
    std::string tmpFilePath = GetTmpFilePath();
    const bool hasTmpFilePath = !tmpFilePath.empty();
    if (hasUserTmp && !hasTmpFilePath) {
        return storage_->Flush();
    }

    if (hasUserTmp && hasTmpFilePath) {
        if (storage_->IsDirty()) {
            return false;
        }
        std::filesystem::path sourcePath(userTmpFilePath_);
        std::filesystem::path targetPath(GetTmpFilePath());
        std::error_code copyEc;
        std::filesystem::copy_file(sourcePath, targetPath, std::filesystem::copy_options::overwrite_existing, copyEc);
        if (copyEc) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "copy file failed");
            return false;
        }
        RestoreStorage();
        userTmpFilePath_.clear();
        return true;
    }
    if (hasTmpFilePath) {
        if (std::filesystem::exists(std::filesystem::path(tmpFilePath)))
            return storage_->Flush();
        return storage_->SaveToFile(tmpFilePath.c_str());
    }
    return true;
}

bool ObjectEditorDocument::GenerateAndSaveUniqueFile()
{
    if (!storage_) {
        return false;
    }
    const auto base = std::filesystem::current_path();
    std::random_device rd;
    std::mt19937_64 generator(rd());
    std::uniform_int_distribution<uint64_t> distribution;

    static constexpr char kHexAlphabet[] = "0123456789abcdef";
    constexpr uint32_t ATTEMPT_TIMES = 64;
    constexpr uint32_t FILENAME_SUFFIX_SIZE = 16;
    for (uint32_t attempt = 0; attempt < ATTEMPT_TIMES; ++attempt) {
        auto value = distribution(generator);
        std::array<char, FILENAME_SUFFIX_SIZE + 1> suffix{};
        for (uint32_t index = 0; index < FILENAME_SUFFIX_SIZE; ++index) {
            suffix[FILENAME_SUFFIX_SIZE - 1 - index] = kHexAlphabet[value & 0x0Fu];
            value >>= 4U;
        }

        std::string filename = "oe-storage-";
        filename.append(suffix.data(), FILENAME_SUFFIX_SIZE);
        filename.append(".bin");
        std::filesystem::path candidate = base / filename;
        if (std::filesystem::exists(candidate)) {
            continue;
        }

        if (!storage_->SaveToFile(candidate.string().c_str())) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "save to file failed");
            return false;
        }
        tmpFileUri_ = SystemUtils::GetUriFromPath(candidate.string());
        return true;
    }
    return false;
}

bool ObjectEditorDocument::Flush()
{
    if (!storage_) {
        return false;
    }
    if (ShouldRebuild()) {
        return RebuildAndFlush();
    }
    if (HandleUserTempScenario()) {
        return true;
    }
    return GenerateAndSaveUniqueFile();
}


bool AtomicReplaceFile(const std::string &tempPath, const std::string &targetPath)
{
    return fs::copy_file(tempPath, targetPath, fs::copy_options::overwrite_existing);
}

std::string GenerateTempPath(const std::string &targetPath, const std::string documentId)
{
    std::string sandboxPath = "";
    std::shared_ptr<OHOS::AbilityRuntime::ApplicationContext> context =
        AbilityRuntime::Context::GetApplicationContext();
    if (context == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "get context failed");
        return sandboxPath;
    }
    std::string fileDirPath = context->GetTempDir();
    if (fileDirPath.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "get sandboxPath failed");
        return sandboxPath;
    }
    sandboxPath = fileDirPath + '/' + documentId;
    fs::path targetDirPath(sandboxPath);
    fs::create_directories(targetDirPath);
    thread_local std::random_device rd;
    thread_local std::mt19937 gen(rd());
    constexpr uint32_t PATH_LEN = 16;
    thread_local std::uniform_int_distribution<> dis(0, PATH_LEN - 1);
    static constexpr char kHex[] = "0123456789abcdef";

    std::ostringstream ss;
    ss << sandboxPath << "/ole.bin.rebuilding.";
    for (uint32_t i = 0; i < PATH_LEN; ++i) {
        ss << kHex[dis(gen)];
    }
    return ss.str();
}

void ObjectEditorDocument::TraverseDirectory(const std::string &path, std::size_t depth,
    uint64_t &total, std::size_t &visitCount) const
{
    if (!storage_) {
        return;
    }
    if (total == std::numeric_limits<uint64_t>::max()) {
        return;
    }
    if (depth > MAX_DEPTH) {
        total = std::numeric_limits<uint64_t>::max();
        return;
    }
    if (visitCount++ >= MAX_VISITS) {
        total = std::numeric_limits<uint64_t>::max();
        return;
    }
    
    std::string savedPath;
    storage_->Path(savedPath);
    std::vector<const DirEntry *> entries;
    if (!storage_->EnterDirectory(path)) {
        return;
    }
    storage_->ListEntries(entries);
    if (!savedPath.empty()) {
        (void)storage_->EnterDirectory(savedPath);
    }

    for (const DirEntry *e : entries) {
        if (e == nullptr) {
            continue;
        }

        std::string fullPath = (path == "/") ? ("/" + e->Name()) : (path + "/" + e->Name());
        if (e->IsFile()) {
            const uint64_t size = e->Size();
            if (total > std::numeric_limits<uint64_t>::max() - size) {
                total = std::numeric_limits<uint64_t>::max();
                return;
            }
            total += size;
        } else if (e->IsDir()) {
            TraverseDirectory(fullPath, depth + 1, total, visitCount);
        }
    }
}

uint64_t ObjectEditorDocument::ComputeLiveDataSize() const
{
    uint64_t total = 0;
    std::size_t visitCount = 0;
    TraverseDirectory("/", 0, total, visitCount);
    return total;
}

bool ObjectEditorDocument::ShouldRebuild() const
{
    if (!storage_) {
        return false;
    }

    std::string targetPath = GetTmpFilePath();
    if (targetPath.empty()) {
        return false;
    }

    std::error_code ec;
    const auto fileSizeRaw = std::filesystem::file_size(targetPath, ec);
    if (ec || fileSizeRaw == 0) {
        return false;
    }
    if (fileSizeRaw > std::numeric_limits<uint64_t>::max()) {
        return false;
    }

    const uint64_t fileSize = static_cast<uint64_t>(fileSizeRaw);
    const uint64_t liveDataSize = ComputeLiveDataSize();
    if (liveDataSize >= fileSize) {
        return false;
    }

    const uint64_t wasteBytes = fileSize - liveDataSize;
    constexpr uint64_t MIN_WASTE_BYTES = 8ULL * 1024 * 1024;
    constexpr uint64_t MIN_FILE_SIZE = 64ULL * 1024;
    constexpr uint32_t NUM_BASE = 10;
    constexpr uint32_t WASTE_RATIO = 3;
    constexpr uint64_t kMaxSafeFileSize = std::numeric_limits<uint64_t>::max() / NUM_BASE;

    bool ratioCheck = false;
    if (fileSize > kMaxSafeFileSize) {
        ratioCheck = wasteBytes >= (fileSize / NUM_BASE) * WASTE_RATIO;
    } else {
        ratioCheck = wasteBytes * NUM_BASE >= fileSize * WASTE_RATIO;
    }

    return fileSize > MIN_FILE_SIZE && wasteBytes >= MIN_WASTE_BYTES && ratioCheck;
}

bool ObjectEditorDocument::CopyStreamData(Storage *src, Storage *dst, const std::string &path, uint64_t size)
{
    if (!src || !dst) {
        return false;
    }
    Stream *dstStream = dst->GetStream(path, true);
    if (!dstStream) {
        return false;
    }
    if (size == 0) {
        return true;
    }
    Stream *srcStream = src->GetStream(path, false);
    if (!srcStream) {
        return false;
    }
    
    constexpr uint64_t kChunkSize = 1ULL * 1024 * 1024;
    const std::size_t bufferSize = static_cast<std::size_t>(std::min<uint64_t>(kChunkSize, size));
    std::vector<std::uint8_t> buffer(bufferSize);
    srcStream->Seek(0);
    dstStream->Seek(0);
    uint64_t remaining = size;
    while (remaining > 0) {
        const std::size_t toRead = static_cast<std::size_t>(
            std::min<uint64_t>(remaining, static_cast<uint64_t>(buffer.size())));

        auto bytesRead = srcStream->Read(buffer.data(), static_cast<std::streamsize>(toRead));
        if (bytesRead <= 0 || bytesRead > static_cast<std::streamsize>(toRead)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Read out of range");
            return false;
        }
        dstStream->Write(buffer.data(), static_cast<std::uint32_t>(bytesRead));
        if (dstStream->Fail()) {
            return false;
        }

        remaining -= static_cast<uint64_t>(bytesRead);
    }
    return true;
}

bool ObjectEditorDocument::CopyAllStreamRecursivelyImpl(Storage *src, Storage *dst,
    const std::string& path, std::size_t depth, std::size_t &visitCount)
{
    if (!src || !dst) {
        return false;
    }
    if (depth > MAX_DEPTH) {
        return false;
    }
    if (visitCount++ >= MAX_VISITS) {
        return false;
    }
    std::string savedPath;
    src->Path(savedPath);
    std::vector<const DirEntry *> entries;
    if (!src->EnterDirectory(path)) {
        if (!savedPath.empty()) {
            (void)src->EnterDirectory(savedPath);
        }
        return false;
    }
    src->ListEntries(entries);
    if (!savedPath.empty()) {
        (void)src->EnterDirectory(savedPath);
    }
    for (const DirEntry *entry : entries) {
        if (!entry) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry is null");
            return false;
        }
        if (!entry->IsFile() && !entry->IsDir()) {
            return false;
        }
        std::string fullPath = path;
        if (path != "/") {
            fullPath += "/";
        }
        fullPath += entry->Name();

        if (entry->IsDir()) {
            DirEntry *newDir = dst->GetStorage(fullPath, true);
            if (!newDir) {
                return false;
            }
            newDir->SetClsid(entry->Clsid(), std::size(entry->Clsid()));
            if (!CopyAllStreamRecursivelyImpl(src, dst, fullPath, depth + 1, visitCount))
                return false;
            continue;
        }

        if (!CopyStreamData(src, dst, fullPath, entry->Size()))
            return false;
    }
    return true;
}

bool ObjectEditorDocument::CopyAllStreamsRecursively(Storage *src, Storage *dst, const std::string &basePath)
{
    if (!src || !dst)
        return false;
    std::size_t visitCount = 0;
    return CopyAllStreamRecursivelyImpl(src, dst, basePath, 0, visitCount);
}

std::string GenerateSafeTempPath(const std::string &targetPath, const std::string &documentId)
{
    constexpr uint32_t ATTEMPT_COUNT = 64;
    std::string tempPath;
    for (uint32_t attempt = 0; attempt < ATTEMPT_COUNT; ++attempt) {
        tempPath = GenerateTempPath(targetPath, documentId);
        std::error_code existsEc;
        if (!std::filesystem::exists(tempPath, existsEc) && !existsEc) {
            return tempPath;
        }
        if (existsEc || attempt == ATTEMPT_COUNT - 1) {
            return "";
        }
    }
    return "";
}

struct TempGuard {
    std::string path;
    bool shouldClean = true;
    ~TempGuard()
    {
        if (shouldClean && !path.empty()) {
            std::error_code ec;
            std::filesystem::remove(path, ec);
        }
    }
};

bool ObjectEditorDocument::RebuildAndFlush()
{
    std::string targetPath = GetTmpFilePath();
    if (targetPath.empty()) {
        return storage_ && storage_->Flush();
    }
    
    std::string tempPath = GenerateSafeTempPath(targetPath, documentId_);
    if (tempPath.empty())
        return false;

    TempGuard guard{tempPath, true};

    auto newDoc = ObjectEditorDocument::CreateByOEid(GetOEid());
    if (!newDoc)
        return false;
    Storage *newStorage = newDoc->GetRootStorage();
    if (!newStorage)
        return false;
    if (!newStorage->SaveToFile(tempPath.c_str()))
        return false;
    if (!CopyAllStreamsRecursively(storage_.get(), newStorage, "/")) {
        return false;
    }
    if (!newStorage->Flush()) {
        return false;
    }

    {
        auto verify = std::make_unique<ObjectEditor::Storage>(tempPath.c_str());
        if (!verify || verify->Result() != ObjectEditor::Storage::Ok) {
            return false;
        }
    }

    newDoc.reset();
    auto oldStorage = std::move(storage_);
    if (!AtomicReplaceFile(tempPath, targetPath)) {
        storage_ = std::move(oldStorage);
        return false;
    }
    guard.shouldClean = false;
    auto reloaded = std::make_unique<ObjectEditor::Storage>(targetPath.c_str());
    if (!reloaded || reloaded->Result() != ObjectEditor::Storage::Ok) {
        storage_.reset();
        return false;
    }
    storage_ = std::move(reloaded);
    return true;
}

bool ObjectEditorDocument::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(oeid_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write oeid failed");
        return false;
    }
    if (!parcel.WriteBool(isLinking_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write isLinking failed");
        return false;
    }
    if (!parcel.WriteString(tmpFileUri_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write tmpFileUri failed");
        return false;
    }
    if (!parcel.WriteString(oriFileUri_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write oriFileUri failed");
        return false;
    }
    if (!parcel.WriteString(snapshotUri_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write snapshotUri failed");
        return false;
    }
    if (!parcel.WriteString(nativeFileUri_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write nativeFileUri failed");
        return false;
    }
    if (!parcel.WriteInt32(static_cast<int32_t>(operateType_))) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write operateType failed");
        return false;
    }
    if (!parcel.WriteString(documentId_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Write documentId failed");
        return false;
    }
    return true;
}

ObjectEditorDocument *ObjectEditorDocument::Unmarshalling(Parcel &parcel)
{
    ObjectEditorDocument *doc = new (std::nothrow) ObjectEditorDocument();
    if (doc == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "Create ObjectEditorDocument failed");
        return nullptr;
    }
    doc->oeid_ = parcel.ReadString();
    doc->isLinking_ = parcel.ReadBool();
    doc->tmpFileUri_ = parcel.ReadString();
    doc->oriFileUri_ = parcel.ReadString();
    doc->snapshotUri_ = parcel.ReadString();
    doc->nativeFileUri_ = parcel.ReadString();
    doc->operateType_ = static_cast<OperateType>(parcel.ReadInt32());
    doc->documentId_ = parcel.ReadString();
    return doc;
}

std::string ObjectEditorDocument::GetDocumentId() const
{
    return documentId_;
}

void ObjectEditorDocument::SetDocumentId(const std::string &documentId)
{
    documentId_ = documentId;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS