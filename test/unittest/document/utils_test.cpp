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
#include "utils.h"
#include "stub.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class UtilsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void UtilsTest::SetUpTestCase()
{
}

void UtilsTest::TearDownTestCase()
{
}

void UtilsTest::SetUp()
{
}

void UtilsTest::TearDown()
{
}

/**
 * @tc.name IsValidOEid_001
 * @tc.desc Test IsValidOEid with valid OEid containing hyphens
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_001, TestSize.Level1)
{
    std::string validOEidWithHyphen = "01234567-89AB-CDEF-0123-456789ABCDEF";
    EXPECT_TRUE(IsValidOEid(validOEidWithHyphen));
}

/**
 * @tc.name IsValidOEid_002
 * @tc.desc Test IsValidOEid with valid OEid without hyphens
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_002, TestSize.Level1)
{
    std::string validOEidNoHyphen = "0123456789ABCDEF0123456789ABCDEF";
    EXPECT_TRUE(IsValidOEid(validOEidNoHyphen));
}

/**
 * @tc.name IsValidOEid_003
 * @tc.desc Test IsValidOEid with invalid length string
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_003, TestSize.Level1)
{
    std::string invalidLength = "01234567";
    EXPECT_FALSE(IsValidOEid(invalidLength));
}

/**
 * @tc.name IsValidOEid_004
 * @tc.desc Test IsValidOEid with empty string
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_004, TestSize.Level1)
{
    std::string emptyOEid = "";
    EXPECT_FALSE(IsValidOEid(emptyOEid));
}

/**
 * @tc.name IsValidOEid_005
 * @tc.desc Test IsValidOEid with invalid hex characters
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_005, TestSize.Level1)
{
    std::string invalidChars = "01234567-89XB-CDEF-0123-456789ABCDEF";
    EXPECT_FALSE(IsValidOEid(invalidChars));
}

/**
 * @tc.name IsValidOEid_006
 * @tc.desc Test IsValidOEid with wrong dash positions
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_006, TestSize.Level1)
{
    std::string wrongDashPositions = "012-3456789AB-CDEF-0123-456789ABCDEF";
    EXPECT_FALSE(IsValidOEid(wrongDashPositions));
}

/**
 * @tc.name IsValidOEid_007
 * @tc.desc Test IsValidOEid with all-zero OEid
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_007, TestSize.Level1)
{
    std::string allZeroOEid = "00000000-0000-0000-0000-000000000000";
    EXPECT_TRUE(IsValidOEid(allZeroOEid));
}

/**
 * @tc.name IsValidOEid_008
 * @tc.desc Test IsValidOEid with all-F OEid
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_008, TestSize.Level1)
{
    std::string allF = "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    EXPECT_TRUE(IsValidOEid(allF));
}

/**
 * @tc.name IsValidOEid_009
 * @tc.desc Test IsValidOEid with lowercase hex characters
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, IsValidOEid_009, TestSize.Level1)
{
    std::string lowercase = "01234567-89ab-cdef-0123-456789abcdef";
    EXPECT_TRUE(IsValidOEid(lowercase));
}

/**
 * @tc.name ParseOEidToClsid_001
 * @tc.desc Test ParseOEidToClsid with valid OEid with hyphens
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ParseOEidToClsid_001, TestSize.Level1)
{
    std::string validOEid = "01234567-89AB-CDEF-0123-456789ABCDEF";
    auto result = ParseOEidToClsid(validOEid);
    EXPECT_TRUE(result.has_value());
}

/**
 * @tc.name ParseOEidToClsid_002
 * @tc.desc Test ParseOEidToClsid with valid OEid without hyphens
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ParseOEidToClsid_002, TestSize.Level1)
{
    std::string validOEidNoHyphen = "0123456789ABCDEF0123456789ABCDEF";
    auto result = ParseOEidToClsid(validOEidNoHyphen);
    EXPECT_TRUE(result.has_value());
}

/**
 * @tc.name ParseOEidToClsid_003
 * @tc.desc Test ParseOEidToClsid with invalid OEid string
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ParseOEidToClsid_003, TestSize.Level1)
{
    std::string invalidOEid = "invalid";
    auto result = ParseOEidToClsid(invalidOEid);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name ParseOEidToClsid_004
 * @tc.desc Test ParseOEidToClsid with empty string
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ParseOEidToClsid_004, TestSize.Level1)
{
    std::string emptyOEid = "";
    auto result = ParseOEidToClsid(emptyOEid);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name ParseOEidToClsid_005
 * @tc.desc Test ParseOEidToClsid with invalid hex characters
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ParseOEidToClsid_005, TestSize.Level1)
{
    std::string invalidChars = "01234567-89XB-CDEF-0123-456789ABCDEF";
    auto result = ParseOEidToClsid(invalidChars);
    EXPECT_FALSE(result.has_value());
}

/**
 * @tc.name FormatClsidToOEid_001
 * @tc.desc Test FormatClsidToOEid round-trip with valid OEid
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, FormatClsidToOEid_001, TestSize.Level1)
{
    std::string validOEid = "01234567-89AB-CDEF-0123-456789ABCDEF";
    auto clsidOpt = ParseOEidToClsid(validOEid);
    ASSERT_TRUE(clsidOpt.has_value());
    std::string result = FormatClsidToOEid(*clsidOpt);
    EXPECT_EQ(result, validOEid);
}

/**
 * @tc.name FormatClsidToOEid_002
 * @tc.desc Test FormatClsidToOEid with zero CLSID
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, FormatClsidToOEid_002, TestSize.Level1)
{
    CLSID zeroClsid{};
    zeroClsid.fill(0);
    std::string result = FormatClsidToOEid(zeroClsid);
    EXPECT_EQ(result, "00000000-0000-0000-0000-000000000000");
}

/**
 * @tc.name RoundTripOEid_001
 * @tc.desc Test round-trip conversion of all-F OEid
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, RoundTripOEid_001, TestSize.Level1)
{
    std::string allF = "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF";
    auto clsidOpt = ParseOEidToClsid(allF);
    ASSERT_TRUE(clsidOpt.has_value());
    std::string result = FormatClsidToOEid(*clsidOpt);
    EXPECT_EQ(result, allF);
}

/**
 * @tc.name HexToNibble_001
 * @tc.desc Test HexToNibble conversion for valid and invalid characters
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, HexToNibble_001, TestSize.Level1)
{
    EXPECT_EQ(HexToNibble('0'), 0);
    EXPECT_EQ(HexToNibble('9'), 9);
    EXPECT_EQ(HexToNibble('a'), 10);
    EXPECT_EQ(HexToNibble('f'), 15);
    EXPECT_EQ(HexToNibble('A'), 10);
    EXPECT_EQ(HexToNibble('F'), 15);
    EXPECT_EQ(HexToNibble('x'), -1);
    EXPECT_EQ(HexToNibble('-'), -1);
}

/**
 * @tc.name ReadUint32_001
 * @tc.desc Test ReadUint32 with 4-byte buffer
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ReadUint32_001, TestSize.Level1)
{
    Byte buf[4] = {0x01, 0x02, 0x03, 0x04};
    uint32_t result = ReadUint32(buf);
    EXPECT_EQ(result, 0x04030201);
}

/**
 * @tc.name ReadUint32_002
 * @tc.desc Test ReadUint32 with insufficient buffer size
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ReadUint32_002, TestSize.Level1)
{
    Byte buf[2] = {0x01, 0x02};
    uint32_t result = ReadUint32(buf, 2);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name WriteUint32_001
 * @tc.desc Test WriteUint32 writes bytes in little-endian order
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, WriteUint32_001, TestSize.Level1)
{
    Byte buf[4] = {0};
    WriteUint32(buf, 0x04030201);
    EXPECT_EQ(buf[0], 0x01);
    EXPECT_EQ(buf[1], 0x02);
    EXPECT_EQ(buf[2], 0x03);
    EXPECT_EQ(buf[3], 0x04);
}

/**
 * @tc.name WriteUint32_002
 * @tc.desc Test WriteUint32 with insufficient buffer size
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, WriteUint32_002, TestSize.Level1)
{
    Byte buf[2] = {0};
    WriteUint32(buf, 0x04030201, 2);
    EXPECT_EQ(buf[0], 0x00);
    EXPECT_EQ(buf[1], 0x00);
}

/**
 * @tc.name ReadUint16_001
 * @tc.desc Test ReadUint16 with 2-byte buffer
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ReadUint16_001, TestSize.Level1)
{
    Byte buf[2] = {0x01, 0x02};
    uint16_t result = ReadUint16(buf);
    EXPECT_EQ(result, 0x0201);
}

/**
 * @tc.name ReadUint16_002
 * @tc.desc Test ReadUint16 with insufficient buffer size
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, ReadUint16_002, TestSize.Level1)
{
    Byte buf[1] = {0x01};
    uint16_t result = ReadUint16(buf, 1);
    EXPECT_EQ(result, 0);
}

/**
 * @tc.name WriteUint16_001
 * @tc.desc Test WriteUint16 writes bytes in little-endian order
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, WriteUint16_001, TestSize.Level1)
{
    Byte buf[2] = {0};
    WriteUint16(buf, 0x0201);
    EXPECT_EQ(buf[0], 0x01);
    EXPECT_EQ(buf[1], 0x02);
}

/**
 * @tc.name WriteUint16_002
 * @tc.desc Test WriteUint16 with insufficient buffer size
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, WriteUint16_002, TestSize.Level1)
{
    Byte buf[1] = {0};
    WriteUint16(buf, 0x0201, 1);
    EXPECT_EQ(buf[0], 0x00);
}

/**
 * @tc.name GetCurrentFileTime_001
 * @tc.desc Test GetCurrentFileTime returns non-zero and monotonically increasing
 * @tc.type FUNC
 */
HWTEST_F(UtilsTest, GetCurrentFileTime_001, TestSize.Level1)
{
    uint64_t time1 = GetCurrentFileTime();
    uint64_t time2 = GetCurrentFileTime();
    EXPECT_GT(time1, 0);
    EXPECT_GE(time2, time1);
}

} // namespace ObjectEditor
} // namespace OHOS