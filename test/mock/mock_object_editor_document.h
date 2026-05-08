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

#ifndef MOCK_OBJECT_EDITOR_DOCUMENT_H
#define MOCK_OBJECT_EDITOR_DOCUMENT_H

#include <gmock/gmock.h>
#define final
#include "object_editor_document.h"
#undef final

namespace OHOS {
namespace ObjectEditor {

// Mock class needs to inherit from ObjectEditorDocument
// Since ObjectEditorDocument is a final class and cannot be directly inherited,
// friend classes or other approaches are needed
// Here we assume ObjectEditorDocument allows mock class to access its private members
class MockObjectEditorDocument : public ObjectEditorDocument {
public:
    MockObjectEditorDocument() = default;
    ~MockObjectEditorDocument() = default;
    MOCK_METHOD(std::string, GetOEid, (), (const));
    MOCK_METHOD(void, SetOEid, (const std::string &oeid));
    MOCK_METHOD(bool, FlushOEid, ());
    MOCK_METHOD(std::string, GetOEidInternal, (), (const));
    MOCK_METHOD(bool, GetLinking, ());
    MOCK_METHOD(void, SetLinking, (bool isLinking));
    MOCK_METHOD(Storage *, GetRootStorage, (), (noexcept));
    MOCK_METHOD(const Storage *, GetRootStorage, (), (const, noexcept));
    MOCK_METHOD(std::optional<std::string>, GetOriFileUri, (), (const, noexcept));
    MOCK_METHOD(void, SetOriFileUri, (const std::string &oriFileUri), (noexcept));
    MOCK_METHOD(std::optional<std::string>, GetTmpFileUri, (), (const, noexcept));
    MOCK_METHOD(void, SetTmpFileUri, (const std::string &tmpFileUri), (noexcept));
    MOCK_METHOD(std::optional<std::string>, GetNativeFileUri, (), (const, noexcept));
    MOCK_METHOD(void, SetNativeFileUri, (const std::string &nativeFileUri), (noexcept));
    MOCK_METHOD(std::string, GetSnapshotUri, ());
    MOCK_METHOD(void, SetSnapshotUri, (const std::string &snapshotUri));
    MOCK_METHOD(OperateType, GetOperateType, (), (noexcept));
    MOCK_METHOD(void, SetOperateType, (const OperateType &type), (noexcept));
    MOCK_METHOD(std::string, GetOriFilePath, (), (noexcept));
    MOCK_METHOD(std::string, GetTmpFilePath, (), (noexcept));
    MOCK_METHOD(std::string, GetNativeFilePath, (), (noexcept));
    MOCK_METHOD(std::string, GetSnapshotPath, (), (noexcept));
    MOCK_METHOD(bool, Flush, ());
    MOCK_METHOD(void, RestoreStorage, ());
};
}
}
#endif