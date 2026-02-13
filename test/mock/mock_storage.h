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

#ifndef MOCK_STORAGE_H
#define MOCK_STORAGE_H

#include <string>
#include <vector>
#include "gmock/gmock.h"
#include "pole.h"

namespace OHOS {
namespace ObjectEditor {
class MockStorage : public Storage {
public:
    MockStorage(const char *filename) : Storage(filename) {}
    MockStorage(const std::string &hmid) : Storage(hmid) {}
    MOCK_METHOD(int, Result, (), (const));
    MOCK_METHOD(void, Path, (std::string & result), (const));
    MOCK_METHOD(void, ListEntries, (std::vector<const DirEntry *> & result), (const));
    MOCK_METHOD(bool, EnterDirectory, (const std::string &directory), ());
    MOCK_METHOD(void, LeaveDirectory, (), ());
    MOCK_METHOD(DirEntry *, GetStorage, (const std::string &path, bool create), ());
    MOCK_METHOD(Stream *, GetStream, (const std::string &name, bool create, bool reuse), ());
    MOCK_METHOD(DirEntry *, GetRootEntry, (), ());
    MOCK_METHOD(DirEntry *, GetEntry, (const std::string &path, bool create), ());
    MOCK_METHOD(bool, Flush, (), ());
    MOCK_METHOD(bool, IsDirty, (), ());
    MOCK_METHOD(bool, DeleteEntry, (const std::string &path), ());
    MOCK_METHOD(bool, SaveToFile, (const char *filename), ());
    MOCK_METHOD(const CLSID &, Clsid, (), (const));
    MOCK_METHOD(uint32_t, TransactionSignature, (), ());
};

}
}
#endif  // MOCK_STORAGE_H
