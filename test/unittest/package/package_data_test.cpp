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

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#define final
#include "package_data.h"
#include "mock_object_editor_document.h"
#undef final
#include "mock_storage.h"
#include "pole.h"
#include "stream.h"
#include "storage.h"
#include "stub.h"
#include "hilog_object_editor.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {
bool ReadStreamUint32(Stream *stream, uint64_t streamSize, StreamPos &offset, uint32_t &value);
void WriteStreamUint32(std::vector<Byte> &buffer, const uint32_t &value);
void WriteStreamString(std::vector<Byte> &buffer, const std::string &value);
bool HandleFileError(const std::ifstream &file, const uint32_t &chunkCount,
    const size_t &totalWritten, const std::streamsize &bytesRead);

class PackageDataTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<PackageData> packageData_;
};

void PackageDataTest::SetUpTestCase()
{
}

void PackageDataTest::TearDownTestCase()
{
}

void PackageDataTest::SetUp()
{
    packageData_ = std::make_unique<PackageData>();
}

void PackageDataTest::TearDown()
{
    packageData_ = nullptr;
}

namespace {

std::shared_ptr<MockObjectEditorDocument> g_mockDocument;

[[maybe_unused]] std::shared_ptr<ObjectEditorDocument> MockCreateByDocument()
{
    return nullptr;
}

Storage *MockGetRootStorageNull()
{
    return nullptr;
}

std::unique_ptr<Storage> g_mockStorage;
Storage *MockGetRootStorageValid()
{
    g_mockStorage = std::make_unique<MockStorage>("test_storage");
    return g_mockStorage.get();
}

std::unique_ptr<Stream> g_mockStream;
Stream *MockGetStreamNull()
{
    return nullptr;
}

[[maybe_unused]] Stream *MockGetStreamValid()
{
    std::string hmid = "00000000000000000000000000000000";
    std::unique_ptr<StorageIO> io = std::make_unique<StorageIO>(hmid);
    auto impl = std::make_unique<StreamImpl>(io.get(), "/test/file");
    g_mockStream = std::make_unique<Stream>(impl.get());
    impl.release();
    return g_mockStream.get();
}

bool MockSaveDataFalse()
{
    return false;
}

bool MockSaveDataTrue()
{
    return true;
}

[[maybe_unused]] bool MockParseOle10NativeStreamFalse(Stream *stream, const std::string &tmpFilePath)
{
    return false;
}

std::string MockGetNativeFilePath()
{
    return "/data/test/nativefile.doc";
}

std::string MockGetOriFilePath()
{
    return "/data/test/orifile.doc";
}

std::string MockGetTmpFilePath()
{
    return "/data/test/tmpfile.doc";
}

}

/**
 * @tc.name: CreateByDocument_001
 * @tc.desc: Test CreateByDocument with null document returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, CreateByDocument_001, TestSize.Level1)
{
    auto result = PackageData::CreateByDocument(nullptr);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateByDocument_002
 * @tc.desc: Test CreateByDocument with SaveData returning false returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, CreateByDocument_002, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetNativeFilePath()).WillRepeatedly(Return(MockGetNativeFilePath()));
    EXPECT_CALL(*document, GetOriFilePath()).WillRepeatedly(Return(MockGetOriFilePath()));
    Stub stub;
    stub.set(ADDR(PackageData, SaveData), MockSaveDataFalse);
    auto result = PackageData::CreateByDocument(document);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: CreateByDocument_003
 * @tc.desc: Test CreateByDocument with SaveData returning true returns valid instance
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, CreateByDocument_003, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetNativeFilePath()).WillRepeatedly(Return(MockGetNativeFilePath()));
    EXPECT_CALL(*document, GetOriFilePath()).WillRepeatedly(Return(MockGetOriFilePath()));
    Stub stub;
    stub.set(ADDR(PackageData, SaveData), MockSaveDataTrue);
    auto result = PackageData::CreateByDocument(document);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: LoadFromDocument_001
 * @tc.desc: Test LoadFromDocument with null document returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, LoadFromDocument_001, TestSize.Level1)
{
    auto result = PackageData::LoadFromDocument(nullptr);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: LoadFromDocument_002
 * @tc.desc: Test LoadFromDocument with null root storage returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, LoadFromDocument_002, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetRootStorage()).WillRepeatedly(Return(MockGetRootStorageNull()));
    auto result = PackageData::LoadFromDocument(document);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: LoadFromDocument_003
 * @tc.desc: Test LoadFromDocument with valid root storage but null stream returns nullptr
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, LoadFromDocument_003, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetRootStorage()).WillRepeatedly(Return(MockGetRootStorageValid()));
    EXPECT_CALL(*document, GetTmpFilePath()).WillRepeatedly(Return(MockGetTmpFilePath()));
    g_mockStorage = std::make_unique<MockStorage>("test_storage");
    Stub stub;
    stub.set(ADDR(Storage, GetStream), MockGetStreamNull);
    auto result = PackageData::LoadFromDocument(document);
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetFilePath_001
 * @tc.desc: Test GetFilePath returns set filepath value
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, GetFilePath_001, TestSize.Level1)
{
    packageData_->filepath_ = "/data/test/file.doc";
    EXPECT_EQ(packageData_->GetFilePath(), "/data/test/file.doc");
}

/**
 * @tc.name: GetFilePath_002
 * @tc.desc: Test GetFilePath returns empty string when filepath not set
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, GetFilePath_002, TestSize.Level1)
{
    EXPECT_EQ(packageData_->GetFilePath(), "");
}

/**
 * @tc.name: SaveData_001
 * @tc.desc: Test SaveData with null document returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, SaveData_001, TestSize.Level1)
{
    packageData_->document_ = std::shared_ptr<ObjectEditorDocument>();
    auto result = packageData_->SaveData();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: SaveData_002
 * @tc.desc: Test SaveData with null root storage returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, SaveData_002, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    EXPECT_CALL(*document, GetRootStorage()).WillRepeatedly(Return(MockGetRootStorageNull()));
    packageData_->document_ = document;
    auto result = packageData_->SaveData();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ReadStreamUint32_001
 * @tc.desc: Test ReadStreamUint32 with null stream and streamSize less than 4 returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_001, TestSize.Level1)
{
    uint32_t value = 0;
    StreamPos offset = 0;
    uint64_t streamSize = 3;
    Stream *stream = nullptr;
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ReadStreamUint32_002
 * @tc.desc: Test ReadStreamUint32 with null stream and valid streamSize returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_002, TestSize.Level1)
{
    uint32_t value = 0;
    StreamPos offset = 0;
    uint64_t streamSize = 4;
    auto result = ReadStreamUint32(nullptr, streamSize, offset, value);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ReadStreamUint32_003
 * @tc.desc: Test ReadStreamUint32 with offset exceeding streamSize returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_003, TestSize.Level1)
{
    uint32_t value = 0;
    StreamPos offset = 100;
    uint64_t streamSize = 10;
    Stream *stream = nullptr;
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteStreamUint32_001
 * @tc.desc: Test WriteStreamUint32 with non-zero value writes 4 bytes
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamUint32_001, TestSize.Level1)
{
    std::vector<Byte> buffer;
    uint32_t value = 0x12345678;
    WriteStreamUint32(buffer, value);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(4));
}

/**
 * @tc.name: WriteStreamUint32_002
 * @tc.desc: Test WriteStreamUint32 with zero value writes 4 zero bytes
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamUint32_002, TestSize.Level1)
{
    std::vector<Byte> buffer;
    uint32_t value = 0;
    WriteStreamUint32(buffer, value);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(4));
    EXPECT_EQ(buffer[0], 0);
    EXPECT_EQ(buffer[1], 0);
    EXPECT_EQ(buffer[2], 0);
    EXPECT_EQ(buffer[3], 0);
}

/**
 * @tc.name: WriteStreamString_001
 * @tc.desc: Test WriteStreamString with non-empty string writes length plus null terminator
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamString_001, TestSize.Level1)
{
    std::vector<Byte> buffer;
    std::string value = "test";
    WriteStreamString(buffer, value);
    EXPECT_GE(buffer.size(), static_cast<size_t>(5));
}

/**
 * @tc.name: WriteStreamString_002
 * @tc.desc: Test WriteStreamString with empty string writes single null terminator byte
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamString_002, TestSize.Level1)
{
    std::vector<Byte> buffer;
    std::string value = "";
    WriteStreamString(buffer, value);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(1));
    EXPECT_EQ(buffer[0], '\0');
}

/**
 * @tc.name: HandleFileError_001
 * @tc.desc: Test HandleFileError with closed ifstream returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, HandleFileError_001, TestSize.Level1)
{
    std::ifstream file;
    uint32_t chunkCount = 0;
    size_t totalWritten = 0;
    std::streamsize bytesRead = 0;
    auto result = HandleFileError(file, chunkCount, totalWritten, bytesRead);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: FormatOle10NativeStream_001
 * @tc.desc: Test FormatOle10NativeStream with nonexistent filepath returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_001, TestSize.Level1)
{
    packageData_->filepath_ = "/nonexistent/path/file.doc";
    std::vector<Byte> buffer;
    bool withData = false;
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->FormatOle10NativeStream(tmpFilePath, buffer, withData);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: ParseOle10NativeStream_001
 * @tc.desc: Test ParseOle10NativeStream with null stream returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_001, TestSize.Level1)
{
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->ParseOle10NativeStream(nullptr, tmpFilePath);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteDataToStream_001
 * @tc.desc: Test WriteDataToStream with null stream returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToStream_001, TestSize.Level1)
{
    auto result = packageData_->WriteDataToStream(nullptr);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteDataToBuffer_001
 * @tc.desc: Test WriteDataToBuffer with nonexistent filepath returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToBuffer_001, TestSize.Level1)
{
    packageData_->filepath_ = "/nonexistent/path/file.doc";
    std::vector<Byte> buffer;
    auto result = packageData_->WriteDataToBuffer(buffer);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteFileToSandbox_001
 * @tc.desc: Test WriteFileToSandbox with null stream returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_001, TestSize.Level1)
{
    StreamPos offset = 0;
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->WriteFileToSandbox(nullptr, offset, tmpFilePath);
    EXPECT_EQ(result, false);
}

}
}