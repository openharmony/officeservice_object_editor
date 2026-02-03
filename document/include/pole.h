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
class StreamIpl;

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
        UnknowError
    };
    explicit Storage(const char *filename);
    explicit Storage(const std::string &hmid);
    ~Storage();
    [[nodiscard]] int Result() const;
    void Path(std::string &result) const;
    void ListEntries(std::vector<const DirEntry *> &result) const;
    [[nodiscard]] bool EntryDirectory(const std::string &directory);
    void LeaveDirectory();
    [[nodiscard]] DirEntry *GetStorage(const std::string &path, bool create = false);
    [[nodiscard]] Stream *GetStream(const std::string &path, bool create = false, bool reuse = false);
    [[nodiscard]] DirEntry *GetRootEntry();
    [[nodiscard]] DirEntry *GetEntry(const std::string &path, bool create = false);
    [[nodiscard]] bool Flush();
    bool IsDirty() const;
    bool ReadRawCd(size_t offset, uint8_t *buf, size_t len, size_t *outRead);
    [[nodiscard]] bool DeleteEntry(const std::string &path);
    [[nodiscard]] bool SaveToFile(const char *filename);
    void Debug();
    [[nodiscard]] const CLSID &Clsid() const;
    [[nodiscard]] uint32_t transactionSignature() const;
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
    Stream &Write(const Byte *data, uint32_t len);
    Stream(StreamIpl *i)
    {
        impl = i;
    }

private:
    ~Stream();
    Stream();
    Stream &operator=(const Stream &);
    StreamIpl *impl = nullptr;
};
} // namespace ObjectEditor
} // namespace OHOS

#endif