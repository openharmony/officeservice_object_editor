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

#ifndef POLE_H
#define POLE_H

#include <array>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class DirEntry;
class StorageIO;
class Stream;
class StreamImpl;

class Storage {
public:
    enum {
        Ok,
        OpenFailed,
        NotOLE,
        BadOLE,
        IOError,
        AllocationFailed,
        InvalidOperation,
        Corruption,
        UnknownError
    };
    explicit Storage(const char *filename);
    explicit Storage(const std::string &hmid);
    ~Storage();
    [[nodiscard]] int Result() const;
    void Path(std::string &result) const;
    void ListEntries(std::vector<const DirEntry *> &result) const;
    [[nodiscard]] bool EnterDirectory(const std::string &directory);
    void LeaveDirectory();
    [[nodiscard]] DirEntry *GetStorage(const std::string &path, bool create = false);
    [[nodiscard]] Stream *GetStream(const std::string &name, bool create = false, bool reuse = false);
    [[nodiscard]] DirEntry *GetRootEntry();
    [[nodiscard]] DirEntry *GetEntry(const std::string &path, bool create = false);
    [[nodiscard]] bool Flush();
    bool IsDirty() const;
    bool ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead);
    [[nodiscard]] bool DeleteEntry(const std::string &path);
    [[nodiscard]] bool SaveToFile(const char *filename);
    void Debug();
    [[nodiscard]] const CLSID &Clsid() const;
    [[nodiscard]] uint32_t TransactionSignature() const;
private:
    std::unique_ptr<StorageIO> io_;
    std::list<std::unique_ptr<Stream>> streams_;
    Storage(const Storage &);
    Storage &operator=(const Storage &);
};

class Stream {
    friend class Storage;
    friend struct std::default_delete<Stream>;

public:
    [[nodiscard]] const std::string &Path() const;
    [[nodiscard]] StreamPos Size() const;
    [[nodiscard]] StreamPos Tell() const;
    [[nodiscard]] bool Eof() const;
    [[nodiscard]] bool Fail() const;
    void Seek(StreamPos pos);
    int Getch();
    std::streamsize Read(Byte *data, std::streamsize maxlen);

    std::streamsize ReadBufferUntilNull(std::vector<Byte> &buffer);

    Stream &Write(const Byte *data, uint32_t len);
    Stream(StreamImpl *i)
    {
        impl = i;
    }

private:
    ~Stream();
    Stream();
    Stream(const Stream &);
    Stream &operator=(const Stream &);
    StreamImpl *impl = nullptr;
};
} // namespace ObjectEditor
} // namespace OHOS

#endif // POLE_H