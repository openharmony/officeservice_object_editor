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
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "alloctable.h"
#include "stub.h"
#include "pole.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class HeaderTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<Header> header_;
};

void HeaderTest::SetUpTestCase()
{
}

void HeaderTest::TearDownTestCase()
{
}

void HeaderTest::SetUp()
{
    header_ = std::make_unique<Header>();
}

void HeaderTest::TearDown()
{
}

namespace {
constexpr int32_t MAX_SECTOR_SHIFT_SIZE = 31;
static const Byte g_cdMagic[] = {0xD0u, 0xCFu, 0x11u, 0xE0u, 0xA1u, 0xB1u, 0x1Au, 0xE1u};
/**
 * @tc.name Valid_001
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_001, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = HEADER_DIFAT_ARRAY_SIZE;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), true);
}

/**
 * @tc.name Valid_002
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_002, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF + 1;
    header_->numBat_ = 100;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name Valid_003
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_003, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = 0;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name Valid_004
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_004, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = 100;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name Valid_005
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_005, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = 100;
    header_->bigBlockShift_ = MAX_SECTOR_SHIFT_SIZE + 1;
    header_->miniBlockShift_ = header_->bigBlockShift_ - 1;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name Valid_006
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_006, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = 100;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name Valid_007
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_007, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = HEADER_DIFAT_ARRAY_SIZE + 100;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    const uint32_t sectorSize = 1u << header_->bigBlockShift_;
    const uint32_t entriesPerSector = sectorSize / static_cast<uint32_t>(sizeof(uint32_t));
    const uint32_t difatEntries = entriesPerSector > 0 ? entriesPerSector - 1 : 0;

    if (difatEntries != 0) {
        const uint32_t extraNeeded = header_->numBat_ - HEADER_DIFAT_ARRAY_SIZE;
        const uint32_t difatSectorNeeded = (extraNeeded + difatEntries - 1) / difatEntries;
        if (header_->numDifat_ < difatSectorNeeded) {
            EXPECT_EQ(header_->Valid(), false);
        }
    }
}

/**
 * @tc.name Valid_008
 * @tc.desc 测试Valid方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Valid_008, TestSize.Level1)
{
    header_->threshold_ = MINI_STREAM_CUTOFF;
    header_->numBat_ = 50;
    header_->bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header_->miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header_->numDifat_ = 10;
    EXPECT_EQ(header_->Valid(), false);
}

/**
 * @tc.name IsCompoundDocument_001
 * @tc.desc 测试IsCompoundDocument方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, IsCompoundDocument_001, TestSize.Level1)
{
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        header_->id_[i] = g_cdMagic[i];
    }
    EXPECT_EQ(header_->IsCompoundDocument(), true);
}

/**
 * @tc.name IsCompoundDocument_002
 * @tc.desc 测试IsCompoundDocument方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, IsCompoundDocument_002, TestSize.Level1)
{
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        header_->id_[i] = g_cdMagic[i];
    }
    header_->id_[0] = 0xFF;
    EXPECT_EQ(header_->IsCompoundDocument(), false);
}

/**
 * @tc.name IsCompoundDocument_003
 * @tc.desc 测试IsCompoundDocument方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, IsCompoundDocument_003, TestSize.Level1)
{
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        header_->id_[i] = 0x00;
    }
    EXPECT_EQ(header_->IsCompoundDocument(), false);
}

/**
 * @tc.name Load_001
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_001, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        data[i] = g_cdMagic[i];
    }
    WriteUint32(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET, MINI_STREAM_CUTOFF);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        WriteUint32(data.data() + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE, i);
    }
    EXPECT_EQ(header_->Load(data.data(), data.size()), true);
}

/**
 * @tc.name Load_002
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_002, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required - 1, 0);
    EXPECT_EQ(header_->Load(data.data(), data.size()), false);
}

/**
 * @tc.name Load_003
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_003, TestSize.Level1)
{
    EXPECT_EQ(header_->Load(nullptr, 0), false);
}

/**
 * @tc.name Load_004
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_004, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required - 1, 0);
    EXPECT_EQ(header_->Load(nullptr, required + 1), false);
}

/**
 * @tc.name Load_005
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_005, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        data[i] = g_cdMagic[i];
    }
    WriteUint16(data.data() + HEADER_SECTOR_SHIFT_OFFSET, DEFAULT_MINI_SECTOR_SHIFT + 1);
    WriteUint16(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET, DEFAULT_MINI_SECTOR_SHIFT);
    WriteUint32(data.data() + HEADER_FAT_SECTOR_OFFSET, 100);
    WriteUint32(data.data() + HEADER_FIRST_DIR_SECTOR_OFFSET, 1);
    WriteUint32(data.data() + HEADER_TRANSACTION_SIGNATURE_OFFSET, 0x12345678);
    WriteUint32(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET, MINI_STREAM_CUTOFF + 1);
    WriteUint32(data.data() + HEADER_FIRST_MINI_FAT_SECTOR_OFFSET, 2);
    WriteUint32(data.data() + HEADER_MINI_FAT_SECTOR_NUMBER_OFFSET, 20);
    WriteUint32(data.data() + HEADER_FIRST_DIFAT_SECTOR_OFFSET, 3);
    WriteUint32(data.data() + HEADER_DIFAT_SECTOR_NUMBER_OFFSET, 30);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        WriteUint32(data.data() + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE, i);
    }
    EXPECT_EQ(header_->Load(data.data(), data.size()), false);
}

/**
 * @tc.name Load_006
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Load_006, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    for (size_t i = 0; i < FILE_SIGNATURE_SIZE; ++i) {
        data[i] = g_cdMagic[i];
    }
    WriteUint16(data.data() + HEADER_SECTOR_SHIFT_OFFSET, DEFAULT_MINI_SECTOR_SHIFT + 1);
    WriteUint16(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET, DEFAULT_MINI_SECTOR_SHIFT);
    WriteUint32(data.data() + HEADER_FAT_SECTOR_OFFSET, 100);
    WriteUint32(data.data() + HEADER_FIRST_DIR_SECTOR_OFFSET, 1);
    WriteUint32(data.data() + HEADER_TRANSACTION_SIGNATURE_OFFSET, 0x12345678);
    WriteUint32(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET, MINI_STREAM_CUTOFF + 1);
    WriteUint32(data.data() + HEADER_FIRST_MINI_FAT_SECTOR_OFFSET, 2);
    WriteUint32(data.data() + HEADER_MINI_FAT_SECTOR_NUMBER_OFFSET, 20);
    WriteUint32(data.data() + HEADER_FIRST_DIFAT_SECTOR_OFFSET, 3);
    WriteUint32(data.data() + HEADER_DIFAT_SECTOR_NUMBER_OFFSET, 30);
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE - 1; ++i) {
        WriteUint32(data.data() + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE, i);
    }
    EXPECT_EQ(header_->Load(data.data(), data.size()), false);
}

/**
 * @tc.name Save_001
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Save_001, TestSize.Level1)
{
    Header header;
    header.threshold_ = MINI_STREAM_CUTOFF;
    header.numBat_ = 100;
    header.bigBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT + 1;
    header.miniBlockShift_ = DEFAULT_MINI_SECTOR_SHIFT;
    header.direntStart_ = 1;
    header.transactionSignature_ = 0x12345678;
    header.sbatStart_ = 2;
    header.numSbat_ = 20;
    header.difatStart_ = 3;
    header.numDifat_ = 30;
    
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        header.bbBlocks_[i] = i;
    }
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    header.Save(data.data(), data.size());

    EXPECT_EQ(ReadUint16(data.data() + HEADER_MINOR_VERSION_OFFSET), DEFAULT_MINOR_VERSION);
    EXPECT_EQ(ReadUint16(data.data() + HEADER_MAJOR_VERSION_OFFSET), DEFAULT_MAJOR_VERSION);
    EXPECT_EQ(ReadUint32(data.data() + HEADER_DIFAT_SECTOR_NUMBER_OFFSET), 30);
}

/**
 * @tc.name Save_002
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Save_002, TestSize.Level1)
{
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required - 1, 0);
    EXPECT_EQ(header_->Save(data.data(), data.size()), false);
}

/**
 * @tc.name Save_003
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Save_003, TestSize.Level1)
{
    EXPECT_EQ(header_->Save(nullptr, 0), false);
}

/**
 * @tc.name Save_004
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Save_004, TestSize.Level1)
{
    header_->threshold_ = 0x12345678;
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    header_->Save(data.data(), data.size());
    EXPECT_EQ(ReadUint32(data.data() + HEADER_MINI_STREAM_CUTOFF_OFFSET), MINI_STREAM_CUTOFF);
}

/**
 * @tc.name Save_005
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Save_005, TestSize.Level1)
{
    Header header;
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        header.bbBlocks_[i] = i * 0x12345678;
    }
    const size_t required = HEADER_FIXED_SIZE + HEADER_DIFAT_ARRAY_SIZE * FOUR_BYTE_SIZE;
    std::vector<uint8_t> data(required, 0);
    header_->Save(data.data(), data.size());
    for (size_t i = 0; i < HEADER_DIFAT_ARRAY_SIZE; ++i) {
        EXPECT_EQ(ReadUint32(data.data() + HEADER_FIXED_SIZE + i * FOUR_BYTE_SIZE), header.bbBlocks_[i]);
    }
}

/**
 * @tc.name Debug
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(HeaderTest, Debug, TestSize.Level1)
{
    header_->numDifat_ = 10;
    header_->Debug();
    EXPECT_EQ(header_->numDifat_, 10);
}

}
}
}