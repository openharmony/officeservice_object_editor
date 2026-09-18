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
#include "utils.h"
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

std::shared_ptr<Storage> MockGetRootStorageNull()
{
    return nullptr;
}

std::shared_ptr<Storage> g_mockStorageShared;
std::shared_ptr<Storage> MockGetRootStorageValid()
{
    g_mockStorageShared = std::make_shared<MockStorage>("test_storage");
    return g_mockStorageShared;
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
    stub.set(ADDR(&PackageData::SaveData), MockSaveDataFalse);
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
    stub.set(ADDR(&PackageData::SaveData), MockSaveDataTrue);
    auto result = PackageData::CreateByDocument(document);
    EXPECT_NE(result, nullptr);
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
    g_mockStorageShared = std::make_shared<MockStorage>("test_storage");
    Stub stub;
    stub.set(ADDR(&Storage::GetStream), MockGetStreamNull);
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

/**
 * @tc.name: WriteFileToSandbox_002
 * @tc.desc: Test WriteFileToSandbox succeeds and creates output file with valid stream and path
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_002, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_002.dat";
    packageData_->dataSize_ = 4;
    // Create an in-memory OLE2 Storage with a stream containing 4 bytes of test data
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_data", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_002/oe_test_write_002.dat";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(packageData_->filepath_.empty());
    std::error_code ec;
    bool fileExists = std::filesystem::exists(packageData_->filepath_, ec);
    EXPECT_TRUE(fileExists);
    // Clean up
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_002", ec);
}

/**
 * @tc.name: WriteFileToSandbox_003
 * @tc.desc: Test WriteFileToSandbox returns false when output file cannot be opened (parent is not a directory)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_003, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_003.dat";
    packageData_->dataSize_ = 4;
    auto stream = std::make_unique<Stream>(nullptr);
    StreamPos offset = 0;
    std::string tmpFilePath = "/dev/null/oe_test_write_003.dat";
    auto result = packageData_->WriteFileToSandbox(stream.get(), offset, tmpFilePath);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteFileToSandbox_004
 * @tc.desc: Test WriteFileToSandbox returns false when parent directory cannot be created
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_004, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_004.dat";
    packageData_->dataSize_ = 4;
    auto stream = std::make_unique<Stream>(nullptr);
    StreamPos offset = 0;
    std::string tmpFilePath = "/dev/null/subdir/oe_test_write_004.dat";
    auto result = packageData_->WriteFileToSandbox(stream.get(), offset, tmpFilePath);
    EXPECT_EQ(result, false);
}

constexpr uint32_t TEST_U16_BUF_LEN = 2;
constexpr uint32_t TEST_U32_BUF_LEN = 4;
constexpr uint16_t TEST_MARKER_VAL = 2;
constexpr uint32_t TEST_MARKER2_VAL = 0x30000;

std::vector<Byte> BuildOle10NativeData(const std::string &filename, const std::string &filepath,
    const std::string &fileLink, const std::vector<Byte> &data)
{
    std::vector<Byte> buffer;
    WriteStreamUint32(buffer, 0);
    Byte marker[TEST_U16_BUF_LEN];
    WriteUint16(marker, TEST_MARKER_VAL, TEST_U16_BUF_LEN);
    buffer.insert(buffer.end(), std::begin(marker), std::end(marker));
    WriteStreamString(buffer, filename);
    WriteStreamString(buffer, filepath);
    WriteStreamUint32(buffer, TEST_MARKER2_VAL);
    std::vector<Byte> fileLinkBuf(fileLink.begin(), fileLink.end());
    fileLinkBuf.push_back('\0');
    WriteStreamUint32(buffer, static_cast<uint32_t>(fileLinkBuf.size()));
    buffer.insert(buffer.end(), fileLinkBuf.begin(), fileLinkBuf.end());
    WriteStreamUint32(buffer, static_cast<uint32_t>(data.size()));
    buffer.insert(buffer.end(), data.begin(), data.end());
    uint32_t fileSize = static_cast<uint32_t>(buffer.size());
    Byte fileSizeBuf[TEST_U32_BUF_LEN];
    WriteUint32(fileSizeBuf, fileSize, TEST_U32_BUF_LEN);
    std::copy(fileSizeBuf, fileSizeBuf + TEST_U32_BUF_LEN, buffer.begin());
    return buffer;
}

bool WriteStreamToFile(const std::string &path, const std::vector<Byte> &data)
{
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    file.write(reinterpret_cast<const char *>(data.data()), data.size());
    file.close();
    return true;
}

std::vector<Byte> ReadFileToBuffer(const std::string &path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return {};
    }
    auto size = file.tellg();
    if (size <= 0) {
        return {};
    }
    file.seekg(0, std::ios::beg);
    std::vector<Byte> data(static_cast<size_t>(size));
    file.read(reinterpret_cast<char *>(data.data()), size);
    return data;
}

Stream *CreateStreamWithData(const std::string &streamName, const std::vector<Byte> &data)
{
    static auto storage = std::make_unique<Storage>(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage->GetStream(streamName, true, true);
    if (stream == nullptr) {
        return nullptr;
    }
    stream->Write(data.data(), data.size());
    if (!storage->Flush()) {
        return nullptr;
    }
    stream->Seek(0);
    return stream;
}

/**
 * @tc.name: ParseOle10NativeStream_002
 * @tc.desc: Test ParseOle10NativeStream with valid OLE10Native data
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_002, TestSize.Level1)
{
    std::vector<Byte> data = {0x41, 0x42, 0x43, 0x44};
    auto oleData = BuildOle10NativeData("test.doc", "/data/test/test.doc", "/data/test/link.doc", data);
    Stream *stream = CreateStreamWithData("test_parse_002", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_002/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->filename_, "test.doc");
    EXPECT_EQ(packageData_->dataSize_, static_cast<uint32_t>(data.size()));
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_002", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_003
 * @tc.desc: Test ParseOle10NativeStream with filename exceeding MAX_FILENAME_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_003, TestSize.Level1)
{
    std::string longFilename(1025, 'a');
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData(longFilename, "/data/test/test.doc", "", data);
    Stream *stream = CreateStreamWithData("test_parse_003", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_003/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, false);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_003", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_004
 * @tc.desc: Test ParseOle10NativeStream with filepath exceeding MAX_FILEPATH_SIZE
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_004, TestSize.Level1)
{
    std::string longFilepath(4097, 'b');
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData("test.doc", longFilepath, "", data);
    Stream *stream = CreateStreamWithData("test_parse_004", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_004/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, false);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_004", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_005
 * @tc.desc: Test ParseOle10NativeStream with empty filename
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_005, TestSize.Level1)
{
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData("", "/data/test/test.doc", "", data);
    Stream *stream = CreateStreamWithData("test_parse_005", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_005/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, false);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_005", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_006
 * @tc.desc: Test ParseOle10NativeStream with empty filepath
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_006, TestSize.Level1)
{
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData("test.doc", "", "", data);
    Stream *stream = CreateStreamWithData("test_parse_006", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_006/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, false);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_006", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_007
 * @tc.desc: Test ParseOle10NativeStream with empty data
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_007, TestSize.Level1)
{
    std::vector<Byte> data;
    auto oleData = BuildOle10NativeData("test.doc", "/data/test/test.doc", "", data);
    Stream *stream = CreateStreamWithData("test_parse_007", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_007/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->dataSize_, 0u);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_007", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_008
 * @tc.desc: Test ParseOle10NativeStream with large data (multiple chunks)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_008, TestSize.Level1)
{
    std::vector<Byte> data(8 * 1024, 0x5A);
    auto oleData = BuildOle10NativeData("large.dat", "/data/test/large.dat", "", data);
    Stream *stream = CreateStreamWithData("test_parse_008", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_008/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->dataSize_, static_cast<uint32_t>(data.size()));
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_008", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_009
 * @tc.desc: Test ParseOle10NativeStream with fileLink data
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_009, TestSize.Level1)
{
    std::vector<Byte> data = {0x01, 0x02, 0x03};
    auto oleData = BuildOle10NativeData("link.doc", "/data/test/link.doc", "/data/test/source.doc", data);
    Stream *stream = CreateStreamWithData("test_parse_009", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_009/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->filename_, "link.doc");
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_009", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_010
 * @tc.desc: Test ParseOle10NativeStream with filename at boundary (exactly MAX_FILENAME_SIZE)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_010, TestSize.Level1)
{
    std::string boundaryFilename(200, 'c');
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData(boundaryFilename, "/data/test/test.doc", "", data);
    Stream *stream = CreateStreamWithData("test_parse_010", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_010/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->filename_, boundaryFilename);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_010", ec);
}

/**
 * @tc.name: WriteFileToSandbox_005
 * @tc.desc: Test WriteFileToSandbox creates native subdirectory and file within it
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_005, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_005.dat";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_005", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_005/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    std::error_code ec;
    bool nativeDirExists = std::filesystem::exists("/data/local/tmp/oe_test_write_005/native", ec);
    EXPECT_TRUE(nativeDirExists);
    bool fileInNativeDir = std::filesystem::exists(packageData_->filepath_, ec);
    EXPECT_TRUE(fileInNativeDir);
    EXPECT_NE(packageData_->filepath_.find("/native/"), std::string::npos);
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_005", ec);
}

/**
 * @tc.name: WriteFileToSandbox_006
 * @tc.desc: Test WriteFileToSandbox does not overwrite system files (snapshot.png)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_006, TestSize.Level1)
{
    packageData_->filename_ = "snapshot.png";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_006", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0xDE, 0xAD, 0xBE, 0xEF};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_006/ole.bin";
    std::error_code ec;
    std::filesystem::create_directories("/data/local/tmp/oe_test_write_006", ec);
    std::string snapshotPath = "/data/local/tmp/oe_test_write_006/snapshot.png";
    WriteStreamToFile(snapshotPath, {0xAA, 0xBB, 0xCC, 0xDD});
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    auto snapshotContent = ReadFileToBuffer(snapshotPath);
    EXPECT_EQ(snapshotContent, (std::vector<Byte>{0xAA, 0xBB, 0xCC, 0xDD}));
    auto nativeContent = ReadFileToBuffer(packageData_->filepath_);
    EXPECT_EQ(nativeContent, (std::vector<Byte>{0xDE, 0xAD, 0xBE, 0xEF}));
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_006", ec);
}

/**
 * @tc.name: WriteFileToSandbox_007
 * @tc.desc: Test WriteFileToSandbox does not overwrite ole.bin
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_007, TestSize.Level1)
{
    packageData_->filename_ = "ole.bin";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_007", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x11, 0x22, 0x33, 0x44};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_007/ole.bin";
    std::error_code ec;
    std::filesystem::create_directories("/data/local/tmp/oe_test_write_007", ec);
    std::string oleBinPath = "/data/local/tmp/oe_test_write_007/ole.bin";
    WriteStreamToFile(oleBinPath, {0xFF, 0xFF, 0xFF, 0xFF});
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_NE(packageData_->filepath_, oleBinPath);
    auto oleBinContent = ReadFileToBuffer(oleBinPath);
    EXPECT_EQ(oleBinContent, (std::vector<Byte>{0xFF, 0xFF, 0xFF, 0xFF}));
    auto nativeContent = ReadFileToBuffer(packageData_->filepath_);
    EXPECT_EQ(nativeContent, (std::vector<Byte>{0x11, 0x22, 0x33, 0x44}));
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_007", ec);
}

/**
 * @tc.name: WriteFileToSandbox_008
 * @tc.desc: Test WriteFileToSandbox with native subdirectory already existing
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_008, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_008.dat";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_008", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_008/ole.bin";
    std::error_code ec;
    std::filesystem::create_directories("/data/local/tmp/oe_test_write_008/native", ec);
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_NE(packageData_->filepath_.find("/native/"), std::string::npos);
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_008", ec);
}

/**
 * @tc.name: WriteFileToSandbox_009
 * @tc.desc: Test WriteFileToSandbox with path traversal in filename
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_009, TestSize.Level1)
{
    packageData_->filename_ = "../../../etc/passwd";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_009", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_009/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_EQ(packageData_->filepath_.find("../../../"), std::string::npos);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_009", ec);
}

/**
 * @tc.name: WriteFileToSandbox_010
 * @tc.desc: Test WriteFileToSandbox with zero dataSize
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_010, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_010.dat";
    packageData_->dataSize_ = 0;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_010", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_010/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_010", ec);
}

/**
 * @tc.name: FormatOle10NativeStream_002
 * @tc.desc: Test FormatOle10NativeStream succeeds with valid existing file
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_002, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_fmt_002";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData = {0x41, 0x42, 0x43, 0x44, 0x45};
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    packageData_->filename_ = "test.dat";
    packageData_->fileLink_ = "";
    std::vector<Byte> buffer;
    bool withData = false;
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->FormatOle10NativeStream(tmpFilePath, buffer, withData);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(buffer.empty());
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: FormatOle10NativeStream_003
 * @tc.desc: Test FormatOle10NativeStream with small file (withData should be true)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_003, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_fmt_003";
    std::string testFile = testDir + "/small.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData(100, 0x5A);
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    packageData_->filename_ = "small.dat";
    packageData_->dataSize_ = 0;
    std::vector<Byte> buffer;
    bool withData = false;
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->FormatOle10NativeStream(tmpFilePath, buffer, withData);
    EXPECT_EQ(result, true);
    EXPECT_TRUE(withData);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: FormatOle10NativeStream_004
 * @tc.desc: Test FormatOle10NativeStream with large file (withData should be false)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_004, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_fmt_004";
    std::string testFile = testDir + "/large.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData(8 * 1024, 0x5A);
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    packageData_->filename_ = "large.dat";
    packageData_->dataSize_ = 8 * 1024;
    std::vector<Byte> buffer;
    bool withData = false;
    std::string tmpFilePath = "/tmp/test";
    auto result = packageData_->FormatOle10NativeStream(tmpFilePath, buffer, withData);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(withData);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: ReadStreamUint32_004
 * @tc.desc: Test ReadStreamUint32 with valid stream reads correct value
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_004, TestSize.Level1)
{
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_rsu_004", true, true);
    ASSERT_NE(stream, nullptr);
    Byte data[] = {0x78, 0x56, 0x34, 0x12, 0x00, 0x00};
    stream->Write(data, sizeof(data));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    uint32_t value = 0;
    StreamPos offset = 0;
    uint64_t streamSize = sizeof(data);
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, true);
    EXPECT_EQ(value, 0x12345678u);
    EXPECT_EQ(offset, static_cast<StreamPos>(4));
}

/**
 * @tc.name: ReadStreamUint32_005
 * @tc.desc: Test ReadStreamUint32 with stream read returning fewer bytes
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_005, TestSize.Level1)
{
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_rsu_005", true, true);
    ASSERT_NE(stream, nullptr);
    Byte data[] = {0x01, 0x02};
    stream->Write(data, sizeof(data));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    uint32_t value = 0;
    StreamPos offset = 0;
    uint64_t streamSize = 10;
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteDataToBuffer_002
 * @tc.desc: Test WriteDataToBuffer succeeds with valid file
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToBuffer_002, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_wdb_002";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData = {0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    std::vector<Byte> buffer;
    buffer.push_back(0xFF);
    auto result = packageData_->WriteDataToBuffer(buffer);
    EXPECT_EQ(result, true);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(7));
    EXPECT_EQ(buffer[0], 0xFF);
    EXPECT_EQ(buffer[1], 0x41);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteDataToBuffer_003
 * @tc.desc: Test WriteDataToBuffer with empty file
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToBuffer_003, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_wdb_003";
    std::string testFile = testDir + "/empty.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    WriteStreamToFile(testFile, {});
    packageData_->filepath_ = testFile;
    std::vector<Byte> buffer;
    auto result = packageData_->WriteDataToBuffer(buffer);
    EXPECT_EQ(result, true);
    EXPECT_TRUE(buffer.empty());
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteDataToStream_002
 * @tc.desc: Test WriteDataToStream succeeds with valid file and stream
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToStream_002, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_wds_002";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData = {0x41, 0x42, 0x43, 0x44};
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wds_002", true, true);
    ASSERT_NE(stream, nullptr);
    auto result = packageData_->WriteDataToStream(stream);
    EXPECT_EQ(result, true);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteDataToStream_003
 * @tc.desc: Test WriteDataToStream with nonexistent file returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToStream_003, TestSize.Level1)
{
    packageData_->filepath_ = "/nonexistent/path/does_not_exist.dat";
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wds_003", true, true);
    ASSERT_NE(stream, nullptr);
    auto result = packageData_->WriteDataToStream(stream);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteDataToStream_004
 * @tc.desc: Test WriteDataToStream with invalid filepath (validate fails)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToStream_004, TestSize.Level1)
{
    packageData_->filepath_ = "";
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wds_004", true, true);
    ASSERT_NE(stream, nullptr);
    auto result = packageData_->WriteDataToStream(stream);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: HandleFileError_002
 * @tc.desc: Test HandleFileError with bad bit set returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, HandleFileError_002, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_hfe_002";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    WriteStreamToFile(testFile, {0x41, 0x42, 0x43});
    std::ifstream file(testFile, std::ios::binary);
    file.clear(std::ios::badbit);
    uint32_t chunkCount = 0;
    size_t totalWritten = 0;
    std::streamsize bytesRead = 0;
    auto result = HandleFileError(file, chunkCount, totalWritten, bytesRead);
    EXPECT_EQ(result, false);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: HandleFileError_003
 * @tc.desc: Test HandleFileError with no fail bit returns false
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, HandleFileError_003, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_hfe_003";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    WriteStreamToFile(testFile, {0x41, 0x42});
    std::ifstream file(testFile, std::ios::binary);
    file.clear();
    uint32_t chunkCount = 5;
    size_t totalWritten = 100;
    std::streamsize bytesRead = 50;
    auto result = HandleFileError(file, chunkCount, totalWritten, bytesRead);
    EXPECT_EQ(result, false);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteStreamUint32_003
 * @tc.desc: Test WriteStreamUint32 with UINT32_MAX value
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamUint32_003, TestSize.Level1)
{
    std::vector<Byte> buffer;
    WriteStreamUint32(buffer, 0xFFFFFFFF);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(4));
    EXPECT_EQ(buffer[0], 0xFF);
    EXPECT_EQ(buffer[1], 0xFF);
    EXPECT_EQ(buffer[2], 0xFF);
    EXPECT_EQ(buffer[3], 0xFF);
}

/**
 * @tc.name: WriteStreamString_003
 * @tc.desc: Test WriteStreamString with long string
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamString_003, TestSize.Level1)
{
    std::vector<Byte> buffer;
    std::string value(256, 'X');
    WriteStreamString(buffer, value);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(257));
    EXPECT_EQ(buffer[256], '\0');
}

/**
 * @tc.name: WriteStreamString_004
 * @tc.desc: Test WriteStreamString with string containing null bytes
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamString_004, TestSize.Level1)
{
    std::vector<Byte> buffer;
    std::string value(1, '\0');
    value += "abc";
    WriteStreamString(buffer, value);
    EXPECT_GE(buffer.size(), static_cast<size_t>(5));
}

/**
 * @tc.name: GetFilePath_003
 * @tc.desc: Test GetFilePath returns long filepath correctly
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, GetFilePath_003, TestSize.Level1)
{
    std::string longPath = "/data/test/very/long/path/to/a/file.dat";
    packageData_->filepath_ = longPath;
    EXPECT_EQ(packageData_->GetFilePath(), longPath);
}

/**
 * @tc.name: SaveData_003
 * @tc.desc: Test SaveData with valid document but null compObj stream
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, SaveData_003, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    g_mockStorageShared = std::make_shared<MockStorage>("test_storage");
    EXPECT_CALL(*document, GetRootStorage()).WillRepeatedly(Return(g_mockStorageShared));
    packageData_->document_ = document;
    Stub stub;
    stub.set(ADDR(&Storage::GetStream), MockGetStreamNull);
    auto result = packageData_->SaveData();
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: CreateByDocument_004
 * @tc.desc: Test CreateByDocument sets filename and filepath correctly
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, CreateByDocument_004, TestSize.Level1)
{
    auto document = std::make_shared<MockObjectEditorDocument>();
    Stub stub;
    stub.set(ADDR(&ObjectEditorDocument::GetNativeFilePath), MockGetNativeFilePath);
    stub.set(ADDR(&ObjectEditorDocument::GetOriFilePath), MockGetOriFilePath);
    stub.set(ADDR(&PackageData::SaveData), MockSaveDataTrue);
    auto result = PackageData::CreateByDocument(document);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->filename_, "nativefile.doc");
    EXPECT_EQ(result->filepath_, "/data/test/nativefile.doc");
}

/**
 * @tc.name: ParseOle10NativeStream_011
 * @tc.desc: Test ParseOle10NativeStream with filepath at boundary (exactly MAX_FILEPATH_SIZE)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_011, TestSize.Level1)
{
    std::string boundaryFilepath(4096, 'd');
    std::vector<Byte> data = {0x01};
    auto oleData = BuildOle10NativeData("test.doc", boundaryFilepath, "", data);
    Stream *stream = CreateStreamWithData("test_parse_011", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_011/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, false);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_011", ec);
}

/**
 * @tc.name: WriteFileToSandbox_011
 * @tc.desc: Test WriteFileToSandbox with large data (multiple chunks)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_011, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_011.dat";
    packageData_->dataSize_ = 8 * 1024;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_011", true, true);
    ASSERT_NE(stream, nullptr);
    std::vector<Byte> testData(8 * 1024, 0x5A);
    stream->Write(testData.data(), testData.size());
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_011/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    auto nativeContent = ReadFileToBuffer(packageData_->filepath_);
    EXPECT_EQ(nativeContent.size(), static_cast<size_t>(8 * 1024));
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_011", ec);
}

/**
 * @tc.name: WriteFileToSandbox_012
 * @tc.desc: Test WriteFileToSandbox writes correct data content
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_012, TestSize.Level1)
{
    packageData_->filename_ = "oe_test_write_012.dat";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_012", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0xCA, 0xFE, 0xBA, 0xBE};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_012/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    auto nativeContent = ReadFileToBuffer(packageData_->filepath_);
    ASSERT_EQ(nativeContent.size(), static_cast<size_t>(4));
    EXPECT_EQ(nativeContent[0], 0xCA);
    EXPECT_EQ(nativeContent[1], 0xFE);
    EXPECT_EQ(nativeContent[2], 0xBA);
    EXPECT_EQ(nativeContent[3], 0xBE);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_012", ec);
}

/**
 * @tc.name: WriteFileToSandbox_013
 * @tc.desc: Test WriteFileToSandbox with filename containing special chars
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_013, TestSize.Level1)
{
    packageData_->filename_ = "test file (1).dat";
    packageData_->dataSize_ = 4;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_013", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_013/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_013", ec);
}

/**
 * @tc.name: WriteDataToBuffer_004
 * @tc.desc: Test WriteDataToBuffer with existing buffer content preserved
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToBuffer_004, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_wdb_004";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData = {0x10, 0x20, 0x30};
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    std::vector<Byte> buffer = {0xAA, 0xBB};
    auto result = packageData_->WriteDataToBuffer(buffer);
    EXPECT_EQ(result, true);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(5));
    EXPECT_EQ(buffer[0], 0xAA);
    EXPECT_EQ(buffer[1], 0xBB);
    EXPECT_EQ(buffer[2], 0x10);
    EXPECT_EQ(buffer[3], 0x20);
    EXPECT_EQ(buffer[4], 0x30);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteDataToBuffer_005
 * @tc.desc: Test WriteDataToBuffer with invalid filepath
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToBuffer_005, TestSize.Level1)
{
    packageData_->filepath_ = "";
    std::vector<Byte> buffer;
    auto result = packageData_->WriteDataToBuffer(buffer);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteDataToStream_005
 * @tc.desc: Test WriteDataToStream with large file (multiple chunks)
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteDataToStream_005, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_wds_005";
    std::string testFile = testDir + "/large.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    std::vector<Byte> fileData(5 * 1024 * 1024, 0x5A);
    WriteStreamToFile(testFile, fileData);
    packageData_->filepath_ = testFile;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wds_005", true, true);
    ASSERT_NE(stream, nullptr);
    auto result = packageData_->WriteDataToStream(stream);
    EXPECT_EQ(result, true);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: ReadStreamUint32_006
 * @tc.desc: Test ReadStreamUint32 with offset at boundary
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_006, TestSize.Level1)
{
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_rsu_006", true, true);
    ASSERT_NE(stream, nullptr);
    Byte data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    stream->Write(data, sizeof(data));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    uint32_t value = 0;
    StreamPos offset = 1;
    uint64_t streamSize = 5;
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, true);
    EXPECT_EQ(offset, static_cast<StreamPos>(5));
}

/**
 * @tc.name: ReadStreamUint32_007
 * @tc.desc: Test ReadStreamUint32 with offset exactly at streamSize - U32_BUF_LEN
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ReadStreamUint32_007, TestSize.Level1)
{
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_rsu_007", true, true);
    ASSERT_NE(stream, nullptr);
    Byte data[] = {0x01, 0x02, 0x03, 0x04};
    stream->Write(data, sizeof(data));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    uint32_t value = 0;
    StreamPos offset = 0;
    uint64_t streamSize = 4;
    auto result = ReadStreamUint32(stream, streamSize, offset, value);
    EXPECT_EQ(result, true);
    EXPECT_EQ(offset, static_cast<StreamPos>(4));
}

/**
 * @tc.name: ParseOle10NativeStream_012
 * @tc.desc: Test ParseOle10NativeStream with fileLink containing data
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_012, TestSize.Level1)
{
    std::vector<Byte> data = {0x01, 0x02};
    std::string fileLink = "/data/test/source_link.doc";
    auto oleData = BuildOle10NativeData("test.doc", "/data/test/test.doc", fileLink, data);
    Stream *stream = CreateStreamWithData("test_parse_012", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_012/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_012", ec);
}

/**
 * @tc.name: FormatOle10NativeStream_005
 * @tc.desc: Test FormatOle10NativeStream with empty filename
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_005, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_fmt_005";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    WriteStreamToFile(testFile, {0x41, 0x42});
    packageData_->filepath_ = testFile;
    packageData_->filename_ = "";
    packageData_->fileLink_ = "";
    std::vector<Byte> buffer;
    bool withData = false;
    auto result = packageData_->FormatOle10NativeStream("/tmp/test", buffer, withData);
    EXPECT_EQ(result, true);
    EXPECT_FALSE(buffer.empty());
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: FormatOle10NativeStream_006
 * @tc.desc: Test FormatOle10NativeStream with fileLink set
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, FormatOle10NativeStream_006, TestSize.Level1)
{
    std::string testDir = "/data/local/tmp/oe_fmt_006";
    std::string testFile = testDir + "/test.dat";
    std::error_code ec;
    std::filesystem::create_directories(testDir, ec);
    WriteStreamToFile(testFile, {0x41, 0x42, 0x43});
    packageData_->filepath_ = testFile;
    packageData_->filename_ = "test.dat";
    packageData_->fileLink_ = "/data/test/link.doc";
    std::vector<Byte> buffer;
    bool withData = false;
    std::string tmpFilePath = "/tmp/test_link";
    auto result = packageData_->FormatOle10NativeStream(tmpFilePath, buffer, withData);
    EXPECT_EQ(result, true);
    std::filesystem::remove_all(testDir, ec);
}

/**
 * @tc.name: WriteFileToSandbox_014
 * @tc.desc: Test WriteFileToSandbox verifies output path is under native subdir
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteFileToSandbox_014, TestSize.Level1)
{
    packageData_->filename_ = "verify_path.dat";
    packageData_->dataSize_ = 2;
    Storage storage(std::string("0003000C-0000-0000-C000-000000000046"));
    Stream *stream = storage.GetStream("test_wfs_014", true, true);
    ASSERT_NE(stream, nullptr);
    Byte testData[] = {0xAA, 0xBB};
    stream->Write(testData, sizeof(testData));
    EXPECT_TRUE(storage.Flush());
    stream->Seek(0);
    StreamPos offset = 0;
    std::string tmpFilePath = "/data/local/tmp/oe_test_write_014/ole.bin";
    auto result = packageData_->WriteFileToSandbox(stream, offset, tmpFilePath);
    EXPECT_EQ(result, true);
    std::string expectedSubstr = "/native/verify_path.dat";
    EXPECT_NE(packageData_->filepath_.find(expectedSubstr), std::string::npos);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_test_write_014", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_013
 * @tc.desc: Test ParseOle10NativeStream verifies extracted file goes to native subdirectory
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_013, TestSize.Level1)
{
    std::vector<Byte> data = {0x41, 0x42, 0x43};
    auto oleData = BuildOle10NativeData("extracted.doc", "/data/test/extracted.doc", "", data);
    Stream *stream = CreateStreamWithData("test_parse_013", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_013/ole.bin";
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    EXPECT_NE(packageData_->filepath_.find("/native/extracted.doc"), std::string::npos);
    std::error_code ec;
    std::filesystem::remove_all("/data/local/tmp/oe_parse_013", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_014
 * @tc.desc: Test ParseOle10NativeStream with snapshot.png as filename does not overwrite
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_014, TestSize.Level1)
{
    std::vector<Byte> data = {0x11, 0x22};
    auto oleData = BuildOle10NativeData("snapshot.png", "/data/test/snapshot.png", "", data);
    Stream *stream = CreateStreamWithData("test_parse_014", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_014/ole.bin";
    std::error_code ec;
    std::filesystem::create_directories("/data/local/tmp/oe_parse_014", ec);
    std::string snapshotPath = "/data/local/tmp/oe_parse_014/snapshot.png";
    WriteStreamToFile(snapshotPath, {0xFF, 0xEE});
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    auto snapshotContent = ReadFileToBuffer(snapshotPath);
    EXPECT_EQ(snapshotContent, (std::vector<Byte>{0xFF, 0xEE}));
    auto nativeContent = ReadFileToBuffer(packageData_->filepath_);
    EXPECT_EQ(nativeContent, (std::vector<Byte>{0x11, 0x22}));
    std::filesystem::remove_all("/data/local/tmp/oe_parse_014", ec);
}

/**
 * @tc.name: ParseOle10NativeStream_015
 * @tc.desc: Test ParseOle10NativeStream with ole.bin as filename does not overwrite
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, ParseOle10NativeStream_015, TestSize.Level1)
{
    std::vector<Byte> data = {0x33, 0x44};
    auto oleData = BuildOle10NativeData("ole.bin", "/data/test/ole.bin", "", data);
    Stream *stream = CreateStreamWithData("test_parse_015", oleData);
    ASSERT_NE(stream, nullptr);
    std::string tmpFilePath = "/data/local/tmp/oe_parse_015/ole.bin";
    std::error_code ec;
    std::filesystem::create_directories("/data/local/tmp/oe_parse_015", ec);
    std::string oleBinPath = "/data/local/tmp/oe_parse_015/ole.bin";
    WriteStreamToFile(oleBinPath, {0xAA, 0xBB});
    auto result = packageData_->ParseOle10NativeStream(stream, tmpFilePath);
    EXPECT_EQ(result, true);
    auto oleBinContent = ReadFileToBuffer(oleBinPath);
    EXPECT_EQ(oleBinContent, (std::vector<Byte>{0xAA, 0xBB}));
    std::filesystem::remove_all("/data/local/tmp/oe_parse_015", ec);
}

/**
 * @tc.name: HandleFileError_004
 * @tc.desc: Test HandleFileError with bytesRead zero and no fail
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, HandleFileError_004, TestSize.Level1)
{
    std::ifstream file;
    file.clear(std::ios::failbit);
    file.clear(std::ios::eofbit);
    uint32_t chunkCount = 1;
    size_t totalWritten = 0;
    std::streamsize bytesRead = 0;
    auto result = HandleFileError(file, chunkCount, totalWritten, bytesRead);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name: WriteStreamUint32_004
 * @tc.desc: Test WriteStreamUint32 appends to existing buffer content
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamUint32_004, TestSize.Level1)
{
    std::vector<Byte> buffer = {0xAB};
    WriteStreamUint32(buffer, 0x12345678);
    EXPECT_EQ(buffer.size(), static_cast<size_t>(5));
    EXPECT_EQ(buffer[0], 0xAB);
    EXPECT_EQ(buffer[1], 0x78);
    EXPECT_EQ(buffer[4], 0x12);
}

/**
 * @tc.name: WriteStreamString_005
 * @tc.desc: Test WriteStreamString appends to existing buffer
 * @tc.type: FUNC
 */
HWTEST_F(PackageDataTest, WriteStreamString_005, TestSize.Level1)
{
    std::vector<Byte> buffer = {0xAB, 0xCD};
    WriteStreamString(buffer, "hi");
    EXPECT_EQ(buffer.size(), static_cast<size_t>(5));
    EXPECT_EQ(buffer[0], 0xAB);
    EXPECT_EQ(buffer[1], 0xCD);
    EXPECT_EQ(buffer[2], 'h');
    EXPECT_EQ(buffer[3], 'i');
    EXPECT_EQ(buffer[4], '\0');
}

}
}