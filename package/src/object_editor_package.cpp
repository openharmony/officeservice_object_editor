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

#include "object_editor_config.h"
#include "object_editor_package.h"
#include "hilog_object_editor.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
const std::string PACKAGE_STREAM_NATIVE_NAME = "\x01Ole10Native";
const std::string PACKAGE_STREAM_NAME = "\x01Ole";
const std::string WANT_ACTION = "ohos.want.action.viewData";
const std::string SHOW_DEFAULT_PICKER = "ability.params.showDefaultPicker";
constexpr int32_t FILE_SIZE_OFFSET = 4;
constexpr int32_t FILE_SIZE_MIN = 10;
constexpr int32_t FILE_MARKER_SIZE = 2;
constexpr int32_t FILE_READ_BUFFER_SIZE = 4096;
constexpr int32_t FILE_WRITE_BUFFER_SIZE = 2048;
constexpr uint16_t PACKAGE_TYPE = 0x0001;
constexpr int32_t FILE_NAME_MAX_SIZE = 259;
}
// LCOV_EXCL_START
ObjectEditorPackage::ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "constructor");
}

ObjectEditorPackage::~ObjectEditorPackage()
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "destructor");
}

ErrCode ObjectEditorPackage::GetSnapshot(std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    std::unique_ptr<ObjectEditorDocument> document = nullptr;
    if (document_->GetOperateType() == OperateType::CREATE_BY_FILE) {
        document = ObjectEditorDocument::CreateByFile(document_->GetOriFilePath());
        if (document == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create document by file failed");
            return ERR_INVALID_VALUE;
        }
        document->SetHmid(PACKAGE_HMID);
        if (document_->GetNativeFileUri().has_value()) {
            document->SetNativeFileUri(document_->GetNativeFileUri().value());
        }
    } else if (document_->GetOperateType() == OperateType::EDIT) {
        document = ObjectEditorDocument::CreateByHmid(PACKAGE_HMID);
        if (document == nullptr) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "create document by memory failed");
            return ERR_INVALID_VALUE;
        }
        document->SetOperateType(OperateType::EDIT);
    }
    if (document_->GetTmpFileUri().has_value()) {
        document->SetTmpFileUri(document_->GetTmpFileUri().value());
    }
    document->SetSnapshotUri(document_->GetSnapshotUri());
    if (clientCb_ != nullptr) {
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package update document");
        clientCb_->OnUpdate(document);
    }
    return ERR_OK;
}

ErrCode ObjectEditorPackage::DoEdit(std::string &documentId)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    auto rootStorage = document_->GetRootStorage();
    if (rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "rootStorage is null");
        return ERR_INVALID_VALUE;
    }
    auto stream = rootStorage->GetStream(PACKAGE_STREAM_NATIVE_NAME);
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return ERR_INVALID_VALUE;
    }
    auto result = ParseOle10NativeStream(stream);
    AAFwk::Want want;
    want.SetAction(WANT_ACTION);
    want.SetUri(SystemUtils::GetUriFromPath(result.outputPath));
    int32_t flags = AAFwk::Want::FLAG_AUTH_READ_URI_PERMISSION | AAFwk::Want::FLAG_AUTH_WRITE_URI_PERMISSION;
    want.setFlags(flags);
    want.setParam(SHOW_DEFAULT_PICKER, true);
    auto callerToken = GetRemoteObject();
    if (callerToken == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "callerToken is null");
    }
    auto abilityManagerClient = AppExecFwk::AbilityManagerClient::GetInstance();
    if (abilityManagerClient == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "abilityManagerClient is null");
        return ObjectEditorManagerErrCode::SA_DISCONNECT_ABILITY_FAILED;
    }
    auto ret = abilityManagerClient->StartAbility(want, callerToken, UserMgr::GetInstance().GetUserId());
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package StartAbility result: %{public}d", ret);
    return ERR_OK;
}

ExtractionResult ObjectEditorPackage::ParseOle10NativeStream(Stream *stream)
{
    ExtractionResult result;
    result.success = false;
    if (stream == nullptr) {
        result.error = "NULL stream pointer";
        return result;
    }
    uint64_t streamSize = stream->Size();
    if (streamSize == 0 || streamSize < FILE_SIZE_MIN) {
        result.error = "stream too small: " + std::to_string(streamSize);
        return result;
    }
    StreamPos offset = 0;
    if (offset + FILE_SIZE_OFFSET > streamSize) {
        result.error = "Insufficient data for file size";
        return result;
    }
    stream->Seek(offset);
    Byte fileSizeBuf[FILE_SIZE_OFFSET];
    stream->Read(fileSizeBuf, FILE_SIZE_OFFSET);
    result.fileSize = ReadUInt32(fileSizeBuf);
    offset += FILE_SIZE_OFFSET;
    offset += FILE_MARKER_SIZE;
    stream->Seek(offset);

    std::vector<Byte> filenameBuf;
    auto filenameSize = stream->ReadBufferUntilNull(filenameBuf);
    result.filename = std::string(filenameBuf.begin(), filenameBuf.begin() + filenameSize);
    offset += filenameSize + 1;
    stream->Seek(offset);

    std::vector<Byte> filepathBuf;
    auto filepathSize = stream->ReadBufferUntilNull(filepathBuf);
    result.filepath = std::string(filepathBuf.begin(), filepathBuf.begin() + filepathSize);
    offset += filepathSize + 1;
    offset += FILE_SIZE_OFFSET;

    if (offset + FILE_SIZE_OFFSET > streamSize) {
        result.error = "Insufficient data for file size";
        return result;
    }
    stream->Seek(offset);

    Byte fileLinkSizeBuf[FILE_SIZE_OFFSET];
    stream->Read(fileLinkSizeBuf, FILE_SIZE_OFFSET);
    auto fileLinkSize = ReadUInt32(fileLinkSizeBuf);
    offset += FILE_SIZE_OFFSET;
    stream->Seek(offset);

    // read file link
    Byte fileLinkBuf[fileLinkSize];
    stream->Read(fileLinkBuf, fileLinkSize);
    std::string fileLink(reinterpret_cast<const char*>(fileLinkBuf), fileLinkSize);
    offset += fileLinkSize;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "fileLink: %{private}s", fileLink.c_str());

    // dataSize 4 byte little endian
    if (offset + FILE_SIZE_OFFSET > streamSize) {
        result.error = "Insufficient data for data size";
        return result;
    }
    stream->Seek(offset);
    Byte dataSizeBuf[FILE_SIZE_OFFSET];
    stream->Read(dataSizeBuf, FILE_SIZE_OFFSET);
    auto dataSize = ReadUInt32(dataSizeBuf);
    offset += FILE_SIZE_OFFSET;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "dataSize: %{public}d", dataSize);

    // write data to sandbox
    fs::path tmpFilePath(document_->GetSnapshotPath());
    fs::path parentDir = tmpFilePath.parent_path();
    fs::path outputPath = parentDir / result.filename;
    auto outputPathStr = outputPath.string();
    std::ofstream outputFile(outputPathStr, std::ios::binary);
    if (!outputFile) {
        result.error = "cannot create file";
        return result;
    }
    uint32_t readOffset = 0;
    do {
        stream->Seek(offset + readOffset);
        auto readLen = FILE_WRITE_BUFFER_SIZE;
        if (dataSize - readOffset < FILE_WRITE_BUFFER_SIZE) {
            readLen = dataSize - readOffset;
        }
        Byte readBuf[readLen];
        stream->Read(readBuf, readLen);
        outputFile.write(reinterpret_cast<const char*>(readBuf), readLen);
        readOffset += readLen;
        OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "readOffset: %{public}d", readOffset);
    } while (readOffset < dataSize);
    outputFile.close();
    if (!outputFile.good()) {
        result.error = "write file data failed";
        return result;
    }
    result.outputPath = outputPath;
    result.success = true;
    return result;
}

ErrCode ObjectEditorPackage::CreatePackageObject()
{
    if (document_ == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return ERR_INVALID_VALUE;
    }
    std::string filePath = document_->GetNativeFilePath();
    size_t pos = filePath.find_last_of('/');
    if (pos == std::string::npos) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "filePath: %{private}s is invalid", filePath.c_str());
        return ERR_INVALID_VALUE;
    }
    auto filename = filePath.substr(pos + 1);
    uint64_t fileSize = SystemUtils::GetFileSize(filePath);
    auto rootStorage = document_->GetRootStorage();
    if (rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "rootStorage is null");
        return ERR_INVALID_VALUE;
    }
    auto oleStream = rootStorage->GetStream(PACKAGE_STREAM_NAME, true, true);
    if (oleStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "oleStream is null");
        return ERR_INVALID_VALUE;
    }
    oleStream->Seek(0);
    auto ole10NativeStream = rootStorage->GetStream(PACKAGE_STREAM_NATIVE_NAME, true, true);
    if (ole10NativeStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "ole10NativeStream is null");
        return ERR_INVALID_VALUE;
    }
    Ole10NativeHeader nativeHeader = {0};
    nativeHeader.totalSize = sizeof(Ole10NativeHeader) + fileSize - FILE_SIZE_OFFSET;
    nativeHeader.dataSize = fileSize;
    nativeHeader.type = PACKAGE_TYPE;
    for (size_t i = 0; i < filename.size() && i < FILE_NAME_MAX_SIZE; ++i) {
        nativeHeader.filename[i] = filename[i];
    }
    ole10NativeStream->Seek(0);
    ole10NativeStream->Write(reinterpret_cast<const Byte*>(&nativeHeader), sizeof(Ole10NativeHeader));
    std::ifstream inputFile(filePath, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "cannot open file: %{private}s errno: %{public}d", filePath.c_str(), errno);
        return errno;
    }
    Byte readBuf[FILE_READ_BUFFER_SIZE];
    
    // write data to stream
    std::streamsize totalSize = file.tellg();
    inputFile.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(FILE_READ_BUFFER_SIZE);
    size_t totalWritten = 0;
    size_t chunkCount = 0;
    while (true) {
        file.read(reinterpret_cast<char*>(buffer.data()), FILE_READ_BUFFER_SIZE);
        std::streamsize bytesRead = file.gcount();
        if (bytesRead == 0) {
            if (file.eof()) {
                break;
            }
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read file data failed errno: %{public}d", errno);
            return ERR_INVALID_VALUE;
        }
        ole10NativeStream->Write(buffer.data(), bytesRead);
        totalWritten += bytesRead;
        chunkCount++;
        if (bytesRead < FILE_READ_BUFFER_SIZE) {
            break;
        }
    }
    if (totalWritten != static_cast<size_t>(totalSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write file data not match, %{public}d:%{public}d",
            totalWritten, totalSize);
        return ERR_INVALID_VALUE;
    }
    return document_->Flush() ? ERR_OK : ERR_INVALID_VALUE;
}

ErrCode ObjectEditorPackage::GetEditStatus(std::string &documentId, bool *isEditing, bool *isModified)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetExtensionEditStatus(bool &isEditing)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::GetCapability(std::string &documentId, uint32_t *bitmask)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Close(std::string &documentId, bool &isAllObjectsRemoved)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    return ERR_OK;
}

ErrCode ObjectEditorPackage::Initial(std::unique_ptr<ObjectEditorDocument> document,
    const sptr<IObjectEditorClientCallback> &clientCb)
{
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "package");
    if (document == nullptr || clientCb == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document or clientCb is null");
        return ERR_INVALID_VALUE;
    }
    document->RestoreStorage();
    document->FlushHmid();
    document_ = std::move(document);
    clientCb_ = clientCb;
    if (document_->GetOperateType() == OperateType::CREATE_BY_FILE) {
        return CreatePackageObject();
    }
    return ERR_OK;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS
