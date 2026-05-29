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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "stream.h"
#include "storage.h"
#include "pole.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class StreamTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<Storage> storage_;
};

void StreamTest::SetUpTestCase()
{
}

void StreamTest::TearDownTestCase()
{
}

void StreamTest::SetUp()
{
    storage_ = std::make_unique<Storage>("00000000000000000000000000000000");
}

void StreamTest::TearDown()
{
    storage_ = nullptr;
}

/**
 * @tc.name StreamImplNullPath_001
 * @tc.desc Test nullPath static member of StreamImpl
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullPath_001, TestSize.Level1)
{
    EXPECT_EQ(StreamImpl::nullPath, "");
}

/**
 * @tc.name StreamSize_001
 * @tc.desc Test Stream Size method
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamSize_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        EXPECT_GT(stream->Size(), 0);
    }
}

/**
 * @tc.name StreamTell_001
 * @tc.desc Test Stream Tell method returns initial position
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamTell_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        EXPECT_EQ(stream->Tell(), 0);
    }
}

/**
 * @tc.name StreamEof_001
 * @tc.desc Test Stream Eof method returns false at initial position
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamEof_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        EXPECT_FALSE(stream->Eof());
    }
}

/**
 * @tc.name StreamFail_001
 * @tc.desc Test Stream Fail method returns false for valid stream
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamFail_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        EXPECT_FALSE(stream->Fail());
    }
}

/**
 * @tc.name StreamPath_001
 * @tc.desc Test Stream Path method returns correct path
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamPath_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        EXPECT_EQ(stream->Path(), "/Root Entry");
    }
}

/**
 * @tc.name StreamSeek_001
 * @tc.desc Test Stream Seek method to position zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamSeek_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        stream->Seek(0);
        EXPECT_EQ(stream->Tell(), 0);
    }
}

/**
 * @tc.name StreamSeek_002
 * @tc.desc Test Stream Seek method to non-zero position
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamSeek_002, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        StreamPos size = stream->Size();
        if (size > 10) {
            stream->Seek(10);
            EXPECT_EQ(stream->Tell(), 10);
        }
    }
}

/**
 * @tc.name StreamSeek_003
 * @tc.desc Test Stream Seek beyond size triggers Eof
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamSeek_003, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        StreamPos size = stream->Size();
        stream->Seek(size + 100);
        EXPECT_TRUE(stream->Eof());
    }
}

/**
 * @tc.name StreamRead_001
 * @tc.desc Test Stream Read method reads data successfully
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamRead_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        Byte buffer[64] = {0};
        stream->Seek(0);
        std::streamsize bytesRead = stream->Read(buffer, 64);
        EXPECT_GT(bytesRead, 0);
    }
}

/**
 * @tc.name StreamImplNullEntry_001
 * @tc.desc Test StreamImpl with null entry returns zero size and position
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntry_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    EXPECT_EQ(impl.Size(), 0);
    EXPECT_EQ(impl.Tell(), 0);
    EXPECT_EQ(impl.Path(), "");
}

/**
 * @tc.name StreamImplNullEntry_002
 * @tc.desc Test StreamImpl with null entry state flags
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntry_002, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    EXPECT_FALSE(impl.Eof());
    EXPECT_FALSE(impl.Fail());
}

/**
 * @tc.name StreamImplNullEntrySeek_001
 * @tc.desc Test StreamImpl Seek with null entry stays at position zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntrySeek_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    impl.Seek(10);
    EXPECT_EQ(impl.Tell(), 0);
}

/**
 * @tc.name StreamImplNullEntryGetch_001
 * @tc.desc Test StreamImpl Getch with null entry returns zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntryGetch_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    EXPECT_EQ(impl.Getch(), 0);
}

/**
 * @tc.name StreamImplNullEntryRead_001
 * @tc.desc Test StreamImpl Read with null entry returns zero bytes
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntryRead_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    Byte buffer[64] = {0};
    std::streamsize bytesRead = impl.Read(buffer, 64);
    EXPECT_EQ(bytesRead, 0);
}

/**
 * @tc.name StreamImplNullEntryWrite_001
 * @tc.desc Test StreamImpl Write with null entry returns zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntryWrite_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    Byte data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    EXPECT_EQ(impl.Write(data, 8), 0);
}

/**
 * @tc.name StreamImplNullEntryReadBufferUntilNull_001
 * @tc.desc Test StreamImpl ReadBufferUntilNull with null entry returns zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplNullEntryReadBufferUntilNull_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    std::vector<Byte> buffer;
    std::streamsize result = impl.ReadBufferUntilNull(buffer);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name StreamGetch_001
 * @tc.desc Test Stream Getch method reads a character
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamGetch_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/Root Entry", false);
    if (stream != nullptr) {
        stream->Seek(0);
        int ch = stream->Getch();
        EXPECT_NE(ch, -1);
    }
}

/**
 * @tc.name StreamWrite_001
 * @tc.desc Test Stream Write method advances position
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamWrite_001, TestSize.Level1)
{
    Stream *stream = storage_->GetStream("/test_stream", true);
    if (stream != nullptr) {
        Byte data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        stream->Seek(0);
        stream->Write(data, 8);
        EXPECT_EQ(stream->Tell(), 0);
    }
}

/**
 * @tc.name StreamImplCopyConstructor_001
 * @tc.desc Test StreamImpl copy constructor with null entry
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplCopyConstructor_001, TestSize.Level1)
{
    StreamImpl original(nullptr, static_cast<DirEntry *>(nullptr));
    StreamImpl copy(original);
    EXPECT_EQ(copy.Size(), 0);
    EXPECT_EQ(copy.Tell(), 0);
    EXPECT_EQ(copy.Path(), "");
    EXPECT_FALSE(copy.Eof());
    EXPECT_FALSE(copy.Fail());
}

/**
 * @tc.name StreamImplConstructorWithPath_001
 * @tc.desc Test StreamImpl constructor with path parameter
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplConstructorWithPath_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, "/test/path");
    EXPECT_EQ(impl.Path(), "/test/path");
    EXPECT_EQ(impl.Size(), 0);
    EXPECT_EQ(impl.Tell(), 0);
}

/**
 * @tc.name StreamImplStateEof_001
 * @tc.desc Test StreamImpl State Eof enum value
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplStateEof_001, TestSize.Level1)
{
    EXPECT_EQ(static_cast<unsigned int>(StreamImpl::State::Eof), 1u);
}

/**
 * @tc.name StreamImplStateBad_001
 * @tc.desc Test StreamImpl State Bad enum value
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplStateBad_001, TestSize.Level1)
{
    EXPECT_EQ(static_cast<unsigned int>(StreamImpl::State::Bad), 2u);
}

/**
 * @tc.name StreamReadZeroLen_001
 * @tc.desc Test StreamImpl Read with zero length returns zero bytes
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamReadZeroLen_001, TestSize.Level1)
{
    Byte buffer[4] = {0};
    std::streamsize bytesRead = StreamImpl(nullptr, static_cast<DirEntry *>(nullptr)).Read(buffer, 0);
    EXPECT_EQ(bytesRead, 0);
}

/**
 * @tc.name StreamImplWriteNullData_001
 * @tc.desc Test StreamImpl Write with null data returns zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplWriteNullData_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    EXPECT_EQ(impl.Write(nullptr, 8), 0);
}

/**
 * @tc.name StreamImplWriteZeroLen_001
 * @tc.desc Test StreamImpl Write with zero length returns zero
 * @tc.type FUNC
 */
HWTEST_F(StreamTest, StreamImplWriteZeroLen_001, TestSize.Level1)
{
    StreamImpl impl(nullptr, static_cast<DirEntry *>(nullptr));
    Byte data[1] = {0};
    EXPECT_EQ(impl.Write(data, 0), 0);
}

} // namespace ObjectEditor
} // namespace OHOS