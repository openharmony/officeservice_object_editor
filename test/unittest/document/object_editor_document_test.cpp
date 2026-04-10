/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2026-2026. All rights reserved.
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
#include <functional>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "object_editor_document.h"
#include "storage.h"
#include "stream.h"
#include "stub.h"
#include "pole.h"
#include "mock_object_editor_client.h"
using FileSizeFunc = std::uint64_t (*)(const std::filesystem::path&);
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class ObjectEditorDocumentTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<ObjectEditorDocument> document_;
};

void ObjectEditorDocumentTest::SetUpTestCase()
{
}

void ObjectEditorDocumentTest::TearDownTestCase()
{
}

void ObjectEditorDocumentTest::SetUp()
{
    document_ = std::make_unique<ObjectEditorDocument>();
}

void ObjectEditorDocumentTest::TearDown()
{
}

namespace {

constexpr int32_t MOCK_FILE_SIZE = 5;
constexpr int32_t MOCK_DATA_SIZE_GREATER_FILE_SIZE = 6;
constexpr int32_t MOCK_DATA_SIZE_SMALLER_FILE_SIZE = 3;

bool MockStorageResult()
{
    return Storage::OpenFailed;
}

bool MockStorageResultOk()
{
    return Storage::Ok;
}

std::unique_ptr<ObjectEditorDocument> MockCreateByHmid()
{
    return nullptr;
}

DIrEntry* MockGetRootEntry()
{
    return nullptr;
}

DirEntry* MockGetRootEntryNonEmpty()
{
    DirEntry *entry = new DirEntry();
    return entry;
}

bool MockFlush()
{
    return true;
}

std::string MockGetTmpFilePath()
{
    return "tempfile";
}

std::string MockGetTmpFilePathEmpty()
{
    return "";
}

bool MockWriteBoolLinking()
{
    return false;
}

bool MockWriteInt32OperateType()
{
    return false;
}

bool MockWriteString()
{
    return false;
}

void MockSeek()
{
}

std::uintmax_t MockFileSizeZero()
{
    return 0;
}

std::uintmax_t MockFileSizeNonZero()
{
    return MOCK_FILE_SIZE; // return non-zero size for test
}

uint64_t MockComputeLivaDataSize()
{
    return MOCK_DATA_SIZE_GREATER_FILE_SIZE; // bigger than mock file size
}

uint64_t MockComputeLivaDataSizeSamll()
{
    return MOCK_DATA_SIZE_SMALLER_FILE_SIZE; // smaller than mock file size
}

std::unique_ptr<Stream> g_stream;

Stream *MockGetStream()
{
    std::string hmid = "00000000000000000000000000000000";
    std::unique_ptr<StorageIO> io = std::make_unique<StorageIO>(hmid);
    auto impl = std::make_unique<StreamImpl>(io.get(), "/test/file");
    g_stream = std::make_unique<Stream>(impl.get());
    impl.release();
    return g_stream.get();
}

/**
 * @tc.name CreateByHmid_001
 * @tc.desc 测试CreateByHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CreateByHmid_001, TestSize.Level1)
{
    const std::string hmid;
    Stub stub;
    stub.set(ADDR(Storage, Result), MockStorageResult);
    std::unique_ptr<ObjectEditorDocument> doc = document_->CreateByHmid(hmid);
    EXPECT_EQ(doc, nullptr);
}

/**
 * @tc.name CreateByHmid_002
 * @tc.desc 测试CreateByHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CreateByHmid_002, TestSize.Level1)
{
    const std::string hmid = "00000000000000000000000000000000";
    std::unique_ptr<ObjectEditorDocument> doc = document_->CreateByHmid(hmid);
    EXPECT_NE(doc, nullptr);
}

/**
 * @tc.name CreateByFile_001
 * @tc.desc 测试CreateByFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CreateByFile_001, TestSize.Level1)
{
    const std::string path;
    bool isLinking = false;
    std::unique_ptr<ObjectEditorDocument> doc = document_->CreateByFile(path, isLinking);
    EXPECT_NE(doc, nullptr);
}

/**
 * @tc.name CreateByFile_001
 * @tc.desc 测试CreateByFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CreateByFile_001, TestSize.Level1)
{
    const std::string path;
    bool isLinking = false;
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, CreateByHmid), MockCreateByHmid);
    std::unique_ptr<ObjectEditorDocument> doc = document_->CreateByFile(path, isLinking);
    EXPECT_EQ(doc, nullptr);
}

/**
 * @tc.name LoadFromFile_001
 * @tc.desc 测试LoadFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, LoadFromFile_001, TestSize.Level1)
{
    const std::string path;
    Stub stub;
    stub.set(ADDR(Storage, Result), MockStorageResult);
    std::unique_ptr<ObjectEditorDocument> doc = document_->LoadFromFile(path);
    EXPECT_EQ(doc, nullptr);
}

/**
 * @tc.name LoadFromFile_001
 * @tc.desc 测试LoadFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, LoadFromFile_001, TestSize.Level1)
{
    const std::string path;
    Stub stub;
    stub.set(ADDR(Storage, Result), MockStorageResult);
    std::unique_ptr<ObjectEditorDocument> doc = document_->LoadFromFile(path);
    EXPECT_EQ(doc, nullptr);
}

/**
 * @tc.name LoadFromFile_002
 * @tc.desc 测试LoadFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, LoadFromFile_002, TestSize.Level1)
{
    const std::string path = "tempfile";
    Stub stub;
    stub.set(ADDR(Storage, Result), MockStorageResultOk);
    std::unique_ptr<ObjectEditorDocument> doc = document_->LoadFromFile(path);
    EXPECT_NE(doc, nullptr);
}

/**
 * @tc.name LoadFromFile_003
 * @tc.desc 测试LoadFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, LoadFromFile_003, TestSize.Level1)
{
    const std::string path = "tempfile";
    Stub stub;
    stub.set(ADDR(Storage, Result), MockStorageResult);
    std::unique_ptr<ObjectEditorDocument> doc = document_->LoadFromFile(path);
    EXPECT_EQ(doc, nullptr);
}

/**
 * @tc.name GetHmid_001
 * @tc.desc 测试GetHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetHmid_001, TestSize.Level1)
{
    std::string res = document_->GetHmid();
    EXPECT_NE(res, "");
}

/**
 * @tc.name FlushHmid_001
 * @tc.desc 测试FlushHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, FlushHmid_001, TestSize.Level1)
{
    const std::string hmid = "00000000000000000000000000000000";
    Stub stub;
    stub.set(ADDR(Storage, GetRootEntry), MockGetRootEntry);
    document_->SetHmid(hmid);
    auto res = document_->FlushHmid();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name FlushHmid_002
 * @tc.desc 测试FlushHmid方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, FlushHmid_002, TestSize.Level1)
{
    const std::string hmid = "00000000000000000000000000000000";
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(Storage, Flush), MockFlush);
    document_->SetHmid(hmid);
    auto res = document_->FlushHmid();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name GetRootStorage_001
 * @tc.desc 测试GetRootStorage方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetRootStorage_001, TestSize.Level1)
{
    auto res = document_->GetRootStorage();
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetOriFileUri_001
 * @tc.desc 测试GetOriFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetOriFileUri_001, TestSize.Level1)
{
    auto res = document_->GetOriFileUri();
    EXPECT_EQ(res, std::nullopt);
}

/**
 * @tc.name GetOriFileUri_002
 * @tc.desc 测试GetOriFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetOriFileUri_002, TestSize.Level1)
{
    document_->oriFileUri_ = "tempfile";
    auto res = document_->GetOriFileUri();
    EXPECT_EQ(res, "tempfile");
}

/**
 * @tc.name SetOriFileUri_001
 * @tc.desc 测试SetOriFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, SetOriFileUri_001, TestSize.Level1)
{
    const std::string uri = "tempfile";
    auto res = document_->SetOriFileUri(uri);
    EXPECT_EQ(document_->oriFileUri_, uri);
}

/**
 * @tc.name GetTmpFileUri_001
 * @tc.desc 测试GetTmpFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetTmpFileUri_001, TestSize.Level1)
{
    auto res = document_->GetTmpFileUri();
    EXPECT_EQ(res, std::nullopt);
}

/**
 * @tc.name GetTmpFileUri_002
 * @tc.desc 测试GetTmpFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetOriFileUri_002, TestSize.Level1)
{
    document_->tmpFileUri_ = "tempfile";
    auto res = document_->GetTmpFileUri();
    EXPECT_EQ(res, "tempfile");
}

/**
 * @tc.name SetTmpFileUri_001
 * @tc.desc 测试SetTmpFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, SetTmpFileUri_001, TestSize.Level1)
{
    const std::string uri = "tempfile";
    auto res = document_->SetTmpFileUri(uri);
    EXPECT_EQ(document_->tmpFileUri_, uri);
}

/**
 * @tc.name GetNativeFileUri_001
 * @tc.desc 测试GetNativeFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetNativeFileUri_001, TestSize.Level1)
{
    auto res = document_->GetNativeFileUri();
    EXPECT_EQ(res, std::nullopt);
}

/**
 * @tc.name GetNativeFileUri_002
 * @tc.desc 测试GetNativeFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetNativeFileUri_002, TestSize.Level1)
{
    document_->tmpFileUri_ = "tempfile";
    auto res = document_->GetNativeFileUri();
    EXPECT_EQ(res, "tempfile");
}

/**
 * @tc.name SetNativeFileUri_001
 * @tc.desc 测试SetNativeFileUri方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, SetNativeFileUri_001, TestSize.Level1)
{
    const std::string uri = "tempfile";
    auto res = document_->SetNativeFileUri(uri);
    EXPECT_EQ(document_->nativeFileUri_, uri);
}

/**
 * @tc.name RestoreStorage_001
 * @tc.desc 测试RestoreStorage方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, RestoreStorage_001, TestSize.Level1)
{
    const std::string uri = "/temp";
    document_->SetTmpFileUri(uri);
    document_->RestoreStorage();
    EXPECT_EQ(document_->storage_, std::make_unique<Storage>(uri.c_str()));
}

/**
 * @tc.name Flush_001
 * @tc.desc 测试Flush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Flush_001, TestSize.Level1)
{
    auto res = document_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Flush_002
 * @tc.desc 测试Flush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Flush_002, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    document_->userTmpFilePath_ = "/test";
    auto res = document_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Flush_003
 * @tc.desc 测试Flush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Flush_003, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePathEmpty);
    document_->userTmpFilePath_ = "/test";
    auto res = document_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Flush_004
 * @tc.desc 测试Flush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Flush_004, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    document_->userTmpFilePath_ = "";
    auto res = document_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Flush_005
 * @tc.desc 测试Flush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Flush_005, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePathEmpty);
    auto res = document_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name GenerateTempPath_001
 * @tc.desc 测试GenerateTempPath方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GenerateTempPath_001, TestSize.Level1)
{
    auto res = document_->GenerateTempPath("");
    EXPECT_EQ(res, "");
}

/**
 * @tc.name ComputeLivaDataSize_001
 * @tc.desc 测试ComputeLivaDataSize方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ComputeLivaDataSize_001, TestSize.Level1)
{
    document_->storage_ = nullptr;
    auto res = document_->ComputeLivaDataSize();
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name ComputeLivaDataSize_002
 * @tc.desc 测试ComputeLivaDataSize方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ComputeLivaDataSize_002, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    auto res = document_->ComputeLivaDataSize();
    EXPECT_EQ(res, 0);
}

/**
 * @tc.name ShouldRebuild_001
 * @tc.desc 测试ShouldRebuild方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ShouldRebuild_001, TestSize.Level1)
{
    document_->storage_ = nullptr;
    auto res = document_->ShouldRebuild();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ShouldRebuild_002
 * @tc.desc 测试ShouldRebuild方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ShouldRebuild_002, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePathEmpty);
    auto res = document_->ShouldRebuild();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ShouldRebuild_003
 * @tc.desc 测试ShouldRebuild方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ShouldRebuild_003, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    stub.set<FileSizeFunc>(std::filesystem::file_size, MockFileSizeZero);
    auto res = document_->ShouldRebuild();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ShouldRebuild_004
 * @tc.desc 测试ShouldRebuild方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ShouldRebuild_004, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    stub.set<FileSizeFunc>(std::filesystem::file_size, MockFileSizeZero);
    stub.set(ADDR(ObjectEditorDocument, ComputeLivaDataSize), MockComputeLivaDataSize);
    auto res = document_->ShouldRebuild();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ShouldRebuild_005
 * @tc.desc 测试ShouldRebuild方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, ShouldRebuild_005, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    stub.set<FileSizeFunc>(std::filesystem::file_size, MockFileSizeZero);
    stub.set(ADDR(ObjectEditorDocument, ComputeLivaDataSize), MockComputeLivaDataSizeSmall);
    auto res = document_->ShouldRebuild();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyStreamData_001
 * @tc.desc 测试CopyStreamData方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyStreamData_001, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "";
    std::unique_ptr<Storage> src = std::make_unique<Storage>(hmid);
    std::unique_ptr<Storage> dst = std::make_unique<Storage>(hmid);
    auto res = document_->CopyStreamData(src.get(), dst.get(), path, 0);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyStreamData_002
 * @tc.desc 测试CopyStreamData方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyStreamData_002, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "test";
    std::unique_ptr<Storage> src = std::make_unique<Storage>(hmid);
    std::unique_ptr<Storage> dst = std::make_unique<Storage>(hmid);
    Stub stub;
    stub.set(ADDR(Storage, GetStream), MockGetStream);
    auto res = document_->CopyStreamData(src.get(), dst.get(), path, 0);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name CopyStreamData_003
 * @tc.desc 测试CopyStreamData方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyStreamData_003, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "test";
    std::unique_ptr<Storage> src = std::make_unique<Storage>(hmid);
    std::unique_ptr<Storage> dst = std::make_unique<Storage>(hmid);
    Stub stub;
    stub.set(ADDR(Storage, GetStream), MockGetStream);
    stub.set(ADDR(Stream, Seek), MockSeek);
    auto res = document_->CopyStreamData(src.get(), dst.get(), path, 16);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyAllStreamRecursively_001
 * @tc.desc 测试CopyAllStreamRecursively方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyAllStreamRecursively_001, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "";
    Storage *src = nullptr;
    std::unique_ptr<Storage> dst = std::make_unique<Storage>(hmid);
    auto res = document_->CopyStreamRecursivelyData(src, dst.get(), path);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyAllStreamRecursively_002
 * @tc.desc 测试CopyAllStreamRecursively方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyAllStreamRecursively_002, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "";
    Storage *dst = nullptr;
    std::unique_ptr<Storage> src = std::make_unique<Storage>(hmid);
    auto res = document_->CopyStreamRecursivelyData(src.get(), dst, path);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyAllStreamRecursively_003
 * @tc.desc 测试CopyAllStreamRecursively方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyAllStreamRecursively_003, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "";
    Storage *src = nullptr;
    Storage *dst = nullptr;
    auto res = document_->CopyStreamRecursivelyData(src, dst, path);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CopyAllStreamRecursively_004
 * @tc.desc 测试CopyAllStreamRecursively方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, CopyAllStreamRecursively_004, TestSize.Level1)
{
    std::string hmid = "00000000000000000000000000000000";
    std::string path = "/test";
    std::unique_ptr<Storage> src = std::make_unique<Storage>(hmid);
    std::unique_ptr<Storage> dst = std::make_unique<Storage>(hmid);
    auto res = document_->CopyStreamRecursivelyData(src.get(), dst.get(), path);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name RebuildAndFlush_001
 * @tc.desc 测试RebuildAndFlush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, RebuildAndFlush_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePathEmpty);
    std::string hmid = "00000000000000000000000000000000";
    document_->storage_ = std::make_unique<Storage>(hmid);
    auto res = document_->RebuildAndFlush();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name RebuildAndFlush_002
 * @tc.desc 测试RebuildAndFlush方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, RebuildAndFlush_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(ObjectEditorDocument, GetTmpFilePath), MockGetTmpFilePath);
    auto res = document_->RebuildAndFlush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name GetHmidInternal_001
 * @tc.desc 测试GetHmidInternal方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetHmidInternal_001, TestSize.Level1)
{
    document_->storage_ = nullptr;
    auto res = document_->GetHmidInternal();
    EXPECT_EQ(res, "");
}

/**
 * @tc.name GetHmidInternal_002
 * @tc.desc 测试GetHmidInternal方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetHmidInternal_002, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(Storage, GetRootEntry), MockGetRootEntry);
    auto res = document_->GetHmidInternal();
    EXPECT_EQ(res, "");
}

/**
 * @tc.name GetHmidInternal_003
 * @tc.desc 测试GetHmidInternal方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, GetHmidInternal_003, TestSize.Level1)
{
    std::string path = "tempfile";
    document_->storage_ = std::make_unique<Storage>(path.c_str());
    Stub stub;
    stub.set(ADDR(Storage, GetRootEntry), MockGetRootEntryNonEmpty);
    auto res = document_->GetHmidInternal();
    EXPECT_NE(res, "");
    delete MockGetRootEntryNonEmpty();
}

/**
 * @tc.name Marshalling_001
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Marshalling_001, TestSize.Level1)
{
    Parcel parcel;
    document_->hmid_ = "00000000000000000000000000000000";
    Stub stub;
    stub.set(ADDR(Parcel, WriteString), MockWriteString);
    auto res = document_->Marshalling(parcel);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Marshalling_002
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Marshalling_002, TestSize.Level1)
{
    Parcel parcel;
    Stub stub;
    stub.set(ADDR(Parcel, WriteBool), MockWriteBoolLinking);
    auto res = document_->Marshalling(parcel);
    EXPECT_EQ(res, false);
    EXPECT_TRUE(parcel.WriteString(document_->hmid_), true);
}

/**
 * @tc.name Marshalling_003
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Marshalling_003, TestSize.Level1)
{
    Parcel parcel;
    Stub stub;
    stub.set(ADDR(Parcel, WriteInt32), MockWriteInt32OperateType);
    auto res = document_->Marshalling(parcel);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Marshalling_004
 * @tc.desc 测试Marshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Marshalling_004, TestSize.Level1)
{
    Parcel parcel;
    auto res = document_->Marshalling(parcel);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Unmarshalling_004
 * @tc.desc 测试Unmarshalling方法
 * @tc.type FUNC
 */
HWTEST_F(ObjectEditorDocumentTest, Unmarshalling_004, TestSize.Level1)
{
    Parcel parcel;
    auto res = document_->Unmarshalling(parcel);
    EXPECT_NE(res, nullptr);
}
}
}
}