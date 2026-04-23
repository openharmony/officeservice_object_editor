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

#include <filesystem>
#include <fstream>

#include "package_data.h"

namespace OHOS {
namespace ObjectEditor {
namespace fs = std::filesystem;
// LCOV_EXCL_START
namespace {
constexpr const char* PACKAGE_STREAM_NATIVE_NAME = "\x01Ole10Native";
constexpr const char* PACKAGE_STREAM_COMPOBJ_NAME = "\x01CompObj";
constexpr const char* PACKAGE_STREAM_OBJINFO_NAME = "\x03ObjInfo";
constexpr uint32_t U16_BUF_LEN = 2;
constexpr uint32_t U32_BUF_LEN = 4;
constexpr uint32_t MARKER_VAL = 2;
constexpr uint32_t MARKER2_VAL = 0x30000;
constexpr uint32_t CHUNK_SIZE = 4 * 1024 * 1024; // 4MB
constexpr uint32_t BLOCK_SIZE = 4 * 1024; // 4KB
}

std::unique_ptr<PackageData> PackageData::CreateByDocument(std::shared_ptr<ObjectEditorDocument> document)
{
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return nullptr;
    }
    std::unique_ptr<PackageData> data = std::make_unique<PackageData>();
    if (data == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "PackageData is null");
        return nullptr;
    }
    fs::path filepath(document->GetNativeFilePath());
    fs::path filename = filepath.filename();
    data->filename_ = filename.string();
    data->filepath_ = filepath.string();
    data->fileLink_ = document->GetOriFilePath();
    data->document_ = document;
    if (!data->SaveData()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "SaveData failed");
        return nullptr;
    }
    return data;
}

std::unique_ptr<PackageData> PackageData::LoadFromDocument(std::shared_ptr<ObjectEditorDocument> document)
{
    if (document == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return nullptr;
    }
    std::unique_ptr<PackageData> data = std::make_unique<PackageData>();
    if (data == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "PackageData is null");
        return nullptr;
    }
    auto rootStorage = document->GetRootStorage();
    if (rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "rootStorage is null");
        return nullptr;
    }
    auto stream = rootStorage->GetStream(PACKAGE_STREAM_NATIVE_NAME);
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return nullptr;
    }
    if (!data->ParseOle10NativeStream(stream, document->GetTmpFilePath())) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "LoadData failed");
        return nullptr;
    }
    data->document_ = document;
    return data;
}

bool ReadStreamUint32(Stream *stream, uint64_t streamSize, StreamPos &offset, uint32_t &value)
{
    if (streamSize > U32_BUF_LEN && offset > streamSize - U32_BUF_LEN) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "Insufficient data for file size");
        return false;
    }
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return false;
    }
    stream->Seek(offset);
    Byte fileSizeBuf[U32_BUF_LEN];
    stream->Read(fileSizeBuf, U32_BUF_LEN);
    value = ReadUint32(fileSizeBuf, U32_BUF_LEN);
    offset += U32_BUF_LEN;
    return true;
}

void WriteStreamUint32(std::vector<Byte> &buffer, const uint32_t &value)
{
    Byte valueBuf[U32_BUF_LEN];
    WriteUint32(valueBuf, value, U32_BUF_LEN);
    buffer.insert(buffer.end(), std::begin(valueBuf), std::end(valueBuf));
}

void WriteStreamString(std::vector<Byte> &buffer, const std::string &value)
{
    std::vector<Byte> valueBuf(value.begin(), value.end());
    valueBuf.push_back('\0');
    buffer.insert(buffer.end(), std::begin(valueBuf), std::end(valueBuf));
}

bool PackageData::ParseOle10NativeStream(Stream *stream, const std::string &tmpFilePath)
{
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return false;
    }
    uint64_t streamSize = stream->Size();
    StreamPos offset = 0;
    // fileSize 4 byte little endian
    if (!ReadStreamUint32(stream, streamSize, offset, fileSize_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to read fileSize");
        return false;
    }
    // skip 2 byte marker
    offset += U16_BUF_LEN;
    // read filename terminated by null character
    stream->Seek(offset);
    std::vector<Byte> filenameBuf;
    auto filenameSize = stream->ReadBufferUntilNull(filenameBuf);
    if (filenameSize <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "filename is empty");
        return false;
    }
    filename_ = std::string(filenameBuf.begin(), filenameBuf.end());
    offset += static_cast<size_t>(filenameSize) + 1;
    // read filepath terminated by null character
    stream->Seek(offset);
    std::vector<Byte> filepathBuf;
    auto filepathSize = stream->ReadBufferUntilNull(filepathBuf);
    if (filepathSize <= 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "filepath is empty");
        return false;
    }
    filepath_ = std::string(filepathBuf.begin(), filepathBuf.end());
    offset += static_cast<size_t>(filepathSize) + 1;
    // skip 4 byte
    offset += U32_BUF_LEN;
    // read fileLinkSize 4 byte little endian
    uint32_t fileLinkSize = 0;
    if (!ReadStreamUint32(stream, streamSize, offset, fileLinkSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to read fileLinkSize");
        return false;
    }
    // skip file link
    offset += fileLinkSize;
    // dataSize 4 byte little endian
    if (!ReadStreamUint32(stream, streamSize, offset, dataSize_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to read dataSize");
        return false;
    }
    if (!WriteFileToSandbox(stream, offset, tmpFilePath)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to write sandbox");
        return false;
    }
    return true;
}

bool PackageData::FormatOle10NativeStream(const std::string &tmpFilePath, std::vector<Byte> &buffer, bool &withData)
{
    if (!fs::exists(filepath_)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "tmpFilePath: %{private}s not exists", tmpFilePath.c_str());
        return false;
    }
    uint64_t dataSize = fs::file_size(filepath_);
    if (dataSize > UINT32_MAX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "the file size exceeds 4GB");
        return false;
    }
    // write fileSize 4 byte little endian
    WriteStreamUint32(buffer, 0);
    // write marker 2 byte
    Byte marker[U16_BUF_LEN];
    WriteUint16(marker, MARKER_VAL, U16_BUF_LEN);
    buffer.insert(buffer.end(), std::begin(marker), std::end(marker));
    // write filename terminated by null character
    WriteStreamString(buffer, filename_);
    // write filepath terminated by null character
    WriteStreamString(buffer, filepath_);
    // write marker2 4 byte little endian
    WriteStreamUint32(buffer, MARKER2_VAL);
    // fileLink
    std::string fileLink = tmpFilePath;
    std::vector<Byte> fileLinkBuf(std::begin(fileLink), std::end(fileLink));
    fileLinkBuf.push_back('\0');
    uint32_t fileLinkSize = static_cast<uint32_t>(fileLinkBuf.size());
    WriteStreamUint32(buffer, fileLinkSize);
    // write fileLink
    buffer.insert(buffer.end(), std::begin(fileLinkBuf), std::end(fileLinkBuf));
    // write dataSize 4 byte little endian
    WriteStreamUint32(buffer, dataSize);
    // update fileSize_
    if (buffer.size() > UINT32_MAX - dataSize_) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "insufficient data for fileSize");
        return false;
    }
    uint32_t fileSize = buffer.size() + dataSize;
    Byte fileSizeBuf[U32_BUF_LEN];
    WriteUint32(fileSizeBuf, fileSize, U32_BUF_LEN);
    std::vector<Byte> newFileSizeBuf(fileSizeBuf, fileSizeBuf + U32_BUF_LEN);
    std::copy(newFileSizeBuf.begin(), newFileSizeBuf.end(), buffer.begin());
    withData = dataSize_ < BLOCK_SIZE;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "buffer size: %{public}u, dataSize: %{public}u",
        static_cast<uint32_t>(buffer.size()), dataSize_);
    return true;
}

bool PackageData::WriteFileToSandbox(Stream *stream, StreamPos &offset, const std::string &tmpFilePath)
{
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return false;
    }
    fs::path filepath(tmpFilePath);
    fs::path parentDir = filepath.parent_path();
    if (!fs::exists(parentDir)) {
        fs::create_directories(parentDir);
    }
    fs::path safeFilename = fs::path(filename_).filename();
    char *canonicalPath = realpath(parentDir.string().c_str(), nullptr);
    if (canonicalPath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to get canonical path");
        return false;
    }
    std::string outputPathStr = std::string(canonicalPath) + "/" + safeFilename.string();
    free(canonicalPath);
    canonicalPath = nullptr;
    std::ofstream outFile(outputPathStr, std::ios::binary);
    if (!outFile) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "cannot create file: %{private}s", tmpFilePath.c_str());
        return false;
    }
    uint32_t readOffset = 0;
    do {
        stream->Seek(offset + readOffset);
        uint32_t readLen = CHUNK_SIZE;
        if (dataSize_ - readOffset < readLen) {
            readLen = dataSize_ - readOffset;
        }
        std::vector<Byte> data(readLen);
        stream->Read(data.data(), readLen);
        if (!outFile.write(reinterpret_cast<const char*>(data.data()), readLen)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write failed");
            if (fs::exists(outputPathStr)) {
                fs::remove(outputPathStr);
            }
            return false;
        }
        readOffset += readLen;
    } while (readOffset < dataSize_);
    if (!outFile.good()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write file data failed");
        if (fs::exists(outputPathStr)) {
            fs::remove(outputPathStr);
        }
        return false;
    }
    filepath_ = outputPathStr;
    return true;
}

bool HandleFileError(const std::ifstream &file, const uint32_t &chunkCount,
    const size_t &totalWritten, const std::streamsize &bytesRead)
{
    if (file.fail()) {
        if (file.bad()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read file failed");
            return false;
        }
        if (bytesRead == 0) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "no byte read from file");
            return false;
        }
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "partial read failed");
        return false;
    }
    std::ios::iostate state = file.rdstate();
    OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read file error: %{public}d",
        static_cast<int32_t>(state));
    return false;
}

bool PackageData::WriteDataToStream(Stream *stream)
{
    if (stream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "stream is null");
        return false;
    }
    char *canonicalPath = realpath(filepath_.c_str(), nullptr);
    if (canonicalPath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to get canonical path");
        return false;
    }
    std::ifstream file(canonicalPath, std::ios::binary | std::ios::ate);
    free(canonicalPath);
    canonicalPath = nullptr;
    if (!file.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to open file");
        return false;
    }
    std::streamsize totalSize = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(CHUNK_SIZE);
    size_t totalWritten = 0;
    uint32_t chunkCount = 0;
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "totalSize: %{public}u", static_cast<uint32_t>(totalSize));
    while (true) {
        file.clear();
        file.read(reinterpret_cast<char*>(buffer.data()), CHUNK_SIZE);
        std::streamsize bytesRead = file.gcount();
        totalWritten += static_cast<size_t>(bytesRead);
        if (file.good()) {
            stream->Write(buffer.data(), bytesRead);
            chunkCount++;
            continue;
        }
        if (file.eof()) {
            if (bytesRead > 0) {
                stream->Write(buffer.data(), bytesRead);
            }
            chunkCount++;
            OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE,
                "file read completed successfully, %{public}u bytes read", static_cast<uint32_t>(totalWritten));
            break;
        }
        return HandleFileError(file, chunkCount, totalWritten, bytesRead);
    }
    if (totalWritten != static_cast<size_t>(totalSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write file data not match, %{public}u:%{public}u",
            static_cast<uint32_t>(totalWritten), static_cast<uint32_t>(totalSize));
    }
    return true;
}

bool PackageData::WriteDataToBuffer(std::vector<Byte> &buffer)
{
    char *canonicalPath = realpath(filepath_.c_str(), nullptr);
    if (canonicalPath == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to get canonical path");
        return false;
    }
    std::ifstream file(canonicalPath, std::ios::binary | std::ios::ate);
    free(canonicalPath);
    canonicalPath = nullptr;
    if (!file.is_open()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "failed to open file");
        return false;
    }
    std::streamsize totalSize = file.tellg();
    file.seekg(0, std::ios::beg);
    size_t originalSize = buffer.size();
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::PACKAGE, "bufferSize: %{public}u, totalSize: %{public}u",
        static_cast<uint32_t>(originalSize), static_cast<uint32_t>(totalSize));
    buffer.resize(originalSize + totalSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data() + originalSize), totalSize)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "read file failed");
        buffer.resize(originalSize);
        return false;
    }
    return true;
}

bool PackageData::SaveData()
{
    auto documentPtr = document_.lock();
    if (documentPtr == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "document is null");
        return false;
    }
    auto rootStorage = documentPtr->GetRootStorage();
    if (rootStorage == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "rootStorage is null");
        return false;
    }
    auto compObjStream = rootStorage->GetStream(PACKAGE_STREAM_COMPOBJ_NAME, true, true);
    if (compObjStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "compObjStream is null");
        return false;
    }
    compObjStream->Seek(0);
    auto oleStream = rootStorage->GetStream(PACKAGE_STREAM_OBJINFO_NAME, true, true);
    if (oleStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "oleStream is null");
        return false;
    }
    oleStream->Seek(0);
    auto ole10NativeStream = rootStorage->GetStream(PACKAGE_STREAM_NATIVE_NAME, true, true);
    if (ole10NativeStream == nullptr) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "ole10NativeStream is null");
        return false;
    }
    ole10NativeStream->Seek(0);
    std::vector<Byte> buffer;
    bool withData = false;
    if (!FormatOle10NativeStream(documentPtr->GetTmpFilePath(), buffer, withData)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "format ole10 native stream failed");
        return false;
    }
    if (withData && !WriteDataToBuffer(buffer)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "write data to buffer failed");
        return false;
    }
    // write header to stream
    ole10NativeStream->Write(buffer.data(), buffer.size());
    // write data to stream
    if (!withData && !WriteDataToStream(ole10NativeStream)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::PACKAGE, "WriteDataToStream failed");
        return false;
    }
    return documentPtr->Flush();
}
// LCOV_EXCL_END
} // namespace ObjectEditor
} // namespace OHOS
