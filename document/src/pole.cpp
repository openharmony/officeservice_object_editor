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

#include "pole.h"
#include "storage.h"
#include "stream.h"

namespace OHOS {
namespace ObjectEditor {
// LCOV_EXCL_START
Storage::Storage(const char *filename)
{
    io_ = std::make_unique<StorageIO>(filename);
}

Storage::Storage(const std::string &hmid)
{
    io_ = std::make_unique<StorageIO>(hmid);
}

Storage::~Storage() = default;

int Storage::Result() const
{
    if (!io_) {
        return Storage::UnknownError;
    }
    return io_->Result();
}

void Storage::ListEntries(std::vector<const DirEntry *> &result) const
{
    if (!io_) {
        return;
    }
    io_->ListEntries(result);
}

bool Storage::EnterDirectory(const std::string &directory)
{
    if (!io_) {
        return false;
    }
    return io_->EnterDirectory(directory);
}

void Storage::LeaveDirectory()
{
    if (!io_) {
        return;
    }
    io_->LeaveDirectory();
}

DirEntry *Storage::GetStorage(const std::string &path, bool create)
{
    if (!io_ || path.empty()) {
        return nullptr;
    }
    std::string normalized = path;
    if (normalized.front() != '/') {
        normalized.insert(0, "/");
    }

    while (normalized.size() > 1 && normalized.back() == '/') {
        normalized.pop_back();
    }

    if (normalized == "/") {
        return io_->GetRootEntry();
    }

    if (DirEntry *entry = io_->Entry(normalized, false)) {
        return entry->IsDir() ? entry : nullptr;
    }

    if (!create) {
        return nullptr;
    }
    const auto pos = normalized.find_last_of('/');
    const std::string parentPath = (pos == std::string::npos || pos == 0) ? std::string("/") :
        normalized.substr(0, pos);
    DirEntry *parent = io_->Entry(parentPath, false);
    if (!parent || !parent->IsDir()) {
        return nullptr;
    }
    DirEntry *created = io_->Entry(normalized, true);
    if (!created) {
        return nullptr;
    }
    created->SetType(1);
    created->SetChild(DIR_ENTRY_END);
    created->SetSize(0);
    created->SetStart(DIR_ENTRY_END);
    return created->IsDir() ? created : nullptr;
}

void Storage::Path(std::string &result) const
{
    if (!io_) {
        return;
    }
    io_->Path(result);
}

Stream *Storage::GetStream(const std::string &name, bool create, bool reuse)
{
    if (!io_ || !name.length()) {
        return nullptr;
    }
    std::string fullName = name;
    if (name[0] != '/') {
        std::string currentPath;
        Path(currentPath);
        if (currentPath.empty() || currentPath == "/") {
            fullName.insert(fullName.begin(), '/');
        } else if (currentPath.back() == '/') {
            fullName.insert(0, currentPath);
        } else {
            fullName.insert(0, currentPath + "/");
        }
    }

    if (reuse) {
        for (const auto &stream : streams_) {
            if (stream && stream->Path() == fullName) {
                return stream.get();
            }
        }
    }

    DirEntry *entry = io_->Entry(fullName, create);
    if (!entry || !entry->IsFile()) {
        return nullptr;
    }

    auto impl = std::make_unique<StreamImpl>(io_.get(), fullName);
    auto stream = std::make_unique<Stream>(impl.get());
    impl.release();
    streams_.push_back(std::move(stream));

    return streams_.back().get();
}

DirEntry *Storage::GetRootEntry()
{
    return io_ ? io_->GetRootEntry() : nullptr;
}

DirEntry *Storage::GetEntry(const std::string &path, bool create)
{
    return io_? io_->Entry(path, create) : nullptr;
}

bool Storage::Flush()
{
    return io_ ? io_->Flush() : false;
}

bool Storage::IsDirty() const
{
    return io_ ? io_->IsDirty() : false;
}

bool Storage::ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead)
{
    if (buf == nullptr || outRead == nullptr) {
        return false;
    }
    return io_ ? io_->ReadRawCd(offset, buf, len, outRead) : false;
}

bool Storage::SaveToFile(const char *filename)
{
    return io_ ? io_->SaveToFile(filename, true) : false;
}

void Storage::Debug()
{
    if (!io_) {
        return;
    }
    io_->Debug();
}

bool Storage::DeleteEntry(const std::string &path)
{
    if (io_) {
        return io_->DeleteEntry(path);
    }
    return false;
}

const CLSID &Storage::Clsid() const
{
    if (!io_ || io_->GetHeader() == nullptr) {
        static CLSID empty{};
        return empty;
    }
    return io_->GetHeader()->Clsid();
}

uint32_t Storage::TransactionSignature() const
{
    if (!io_ || io_->GetHeader() == nullptr) {
        return 0;
    }
    return io_->GetHeader()->TransactionSignature();
}

Stream::~Stream()
{
    delete impl;
}

const std::string &Stream::Path() const
{
    return impl ? impl->Path() : StreamImpl::nullPath;
}

StreamPos Stream::Tell() const
{
    return impl ? static_cast<StreamPos>(impl->Tell()) : 0;
}

bool Stream::Eof() const
{
    return impl ? impl->Eof() : true;
}

bool Stream::Fail() const
{
    return impl ? impl->Fail() : true;
}

void Stream::Seek(StreamPos newPos)
{
    if (impl) {
        impl->Seek(newPos);
    }
}

StreamPos Stream::Size() const
{
    return impl ? impl->Size() : 0;
}

int Stream::Getch()
{
    return impl ? impl->Getch() : 0;
}

std::streamsize Stream::Read(Byte *data, std::streamsize maxlen)
{
    if (!impl || !data || maxlen == 0) {
        return 0;
    }
    return impl->Read(data, maxlen);
}

std::streamsize Stream::ReadBufferUntilNull(std::vector<Byte> &buffer)
{
    if (!impl) {
        return 0;
    }
    return impl->ReadBufferUntilNull(buffer);
}

Stream &Stream::Write(const Byte *data, uint32_t len)
{
    if (impl && data && len > 0) {
        impl->Write(data, len);
    }
    return *this;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS