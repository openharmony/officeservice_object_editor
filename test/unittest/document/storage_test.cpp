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
#include "storage.h"
#include "stream.h"
#include "stub.h"
#include "pole.h"
#include "alloctable.h"
#include "dirtree.h"
#include "header.h"
#include "utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class StorageTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    
    std::string hmid = "00000000000000000000000000000000";
    std::unique_ptr<StorageIO> storage_;
};

void StorageTest::SetUpTestCase()
{
}

void StorageTest::TearDownTestCase()
{
}

void StorageTest::SetUp()
{
    storage_ = std::make_unique<StorageIO>(hmid);
}

void StorageTest::TearDown()
{
}

namespace {

constexpr int32_t MOCK_BIG_BLOCKS = 48;
constexpr int32_t MOCK_BIG_BLOCK_SIZE = 512;
constexpr int32_t MOCK_DIFAT_START = 10;
constexpr int32_t MOCK_UNUSED_BLOCKS = 100;

bool MockSave()
{
    return false;
}

[[maybe_unused]] bool MockSaveSuccess()
{
    return true;
}

bool MockValidateHeader()
{
    return false;
}

bool MockLoadDifatChain()
{
    return false;
}

bool MockLoadFatChain()
{
    return false;
}

bool MockLoadDirectoryTree()
{
    return false;
}

bool MockLoadMiniFat()
{
    return false;
}

bool MockReadAndLoadHeader()
{
    return false;
}

bool MockValidateSectorSizes()
{
    return false;
}

bool MockCheckClaimedTableSizes()
{
    return false;
}

bool MockValidateHeaderSectorIndex()
{
    return false;
}

uint32_t MockLoadBigBlocks()
{
    return MOCK_BIG_BLOCKS;
}

uint32_t MockLoadBigBlocksZero()
{
    return 0;
}

uint32_t MockLoadBigBlockSuccess()
{
    return MOCK_BIG_BLOCK_SIZE;
}

uint32_t MockLoadBigBlockFail()
{
    return 0;
}

uint32_t MockDifatStart()
{
    return MOCK_DIFAT_START;
}

uint32_t MockDifatStartFail()
{
    return AllocTable::Eof;
}

bool MockReadDifatSectorFail()
{
    return false;
}

bool MockReadDifatSectorSuccess()
{
    return true;
}

bool MockProcessDifatEntriesFail()
{
    return false;
}

bool MockProcessDifatEntriesSuccess()
{
    return true;
}

bool MockFollowFail()
{
    return false;
}

bool MockFollowSuccess()
{
    return true;
}

bool MockLoadFail()
{
    return false;
}

bool MockLoadSuccess()
{
    return true;
}

bool MockLoadMiniFatBlocksFail()
{
    return false;
}

bool MockLoadMiniFatBlocksSuccess()
{
    return true;
}

bool MockFollowMiniStreamFail()
{
    return false;
}
bool MockFollowMiniStreamSuccess()
{
    return true;
}

bool MockValidateMiniFatEntriesFail()
{
    return false;
}
bool MockValidateMiniFatEntriesSuccess()
{
    return true;
}

uint64_t MockBigBlockSizeZero()
{
    return 0;
}

uint64_t MockBigBlockSizeNonZero()
{
    return 1;
}

bool MockFlushFail()
{
    return false;
}

bool MockFlushSuccess()
{
    return true;
}

bool MockValidateMiniBlockAccessFail()
{
    return false;
}
bool MockValidateMiniBlockAccessSuccess()
{
    return true;
}

bool MockReadMiniBLockFail()
{
    return false;
}
bool MockReadMiniBLockSuccess()
{
    return true;
}

uint32_t MockSaveBlockToBufferFail()
{
    return 0;
}

[[maybe_unused]] uint32_t MockSaveBlockFail()
{
    return 0;
}

[[maybe_unused]] bool MockFollowSmallBlockTableFail()
{
    return false;
}

[[maybe_unused]] bool MockFollowSmallBlockTableSuccess()
{
    return true;
}

[[maybe_unused]] bool MockWriteBlockCheckedFail()
{
    return false;
}

[[maybe_unused]] bool MockWriteBlockCheckedSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidateConvertInputsSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidateConvertInputsFail()
{
    return false;
}

[[maybe_unused]] bool MockReadMiniStreamSuccess()
{
    return true;
}

[[maybe_unused]] bool MockReadMiniStreamFail()
{
    return false;
}

[[maybe_unused]] bool MockComputeBlocksNeededSuccess()
{
    return true;
}

[[maybe_unused]] bool MockComputeBlocksNeededFail()
{
    return false;
}

[[maybe_unused]] bool MockAllocateBigBlocksForConvertSuccess()
{
    return true;
}

[[maybe_unused]] bool MockAllocateBigBlocksForConvertFail()
{
    return false;
}

[[maybe_unused]] bool MockCopyDataToBigBlocksSuccess()
{
    return true;
}

[[maybe_unused]] bool MockCopyDataToBigBlocksFail()
{
    return false;
}

[[maybe_unused]] void MockReleaseMiniBlocksSuccess()
{
}

[[maybe_unused]] bool MockFlushDirectoryTreeSuccess()
{
    return true;
}
[[maybe_unused]] bool MockFlushDirectoryTreeFail()
{
    return false;
}

[[maybe_unused]] bool MockFlushMiniFatSuccess()
{
    return true;
}
[[maybe_unused]] bool MockFlushMiniFatFail()
{
    return false;
}

[[maybe_unused]] bool MockFlushFatChainSuccess()
{
    return true;
}
[[maybe_unused]] bool MockFlushFatChainFail()
{
    return false;
}

[[maybe_unused]] bool MockFlushDifatChainSuccess()
{
    return true;
}
[[maybe_unused]] bool MockFlushDifatChainFail()
{
    return false;
}

[[maybe_unused]] bool MockFlushHeaderSuccess()
{
    return true;
}
[[maybe_unused]] bool MockCollectSubtreeEntriesSuccess()
{
    return true;
}
[[maybe_unused]] bool MockCollectSubtreeEntriesFail()
{
    return false;
}

[[maybe_unused]] bool MockCollectChainForEntrySuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidateDeletePreconditionsSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidateDeletePreconditionsFail()
{
    return false;
}

[[maybe_unused]] bool MockCollectDeleteTargetsFail()
{
    return false;
}

[[maybe_unused]] bool MockCollectDeleteTargetsSuccess()
{
    return true;
}

[[maybe_unused]] bool MockBuildReleasePlanSuccess()
{
    return true;
}

[[maybe_unused]] bool MockBuildReleasePlanFail()
{
    return false;
}

[[maybe_unused]] bool MockDeleteEntrySuccess()
{
    return true;
}

[[maybe_unused]] bool MockFollowBigBlockTableSuccess()
{
    return true;
}

[[maybe_unused]] bool MockExtendRootStreamSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidateAndFetchExistingChainFail()
{
    return false;
}

[[maybe_unused]] size_t MockUnused()
{
    return MOCK_UNUSED_BLOCKS;
}

[[maybe_unused]] uint32_t MockAllocateProtectedSector()
{
    return AllocTable::Eof;
}

[[maybe_unused]] bool MockValidateFlushStateSuccess()
{
    return true;
}

[[maybe_unused]] bool MockPrepareDirectoryBlocksSuccess()
{
    return true;
}

[[maybe_unused]] bool MockPrepareDirectoryBlocksFail()
{
    return false;
}

[[maybe_unused]] bool MockSetupStagingBufferSuccess()
{
    return true;
}

[[maybe_unused]] bool MockSetupStagingBufferFail()
{
    return false;
}

[[maybe_unused]] bool MockSaveMiniFatSuccess()
{
    return true;
}

[[maybe_unused]] bool MockSaveDifatSuccess()
{
    return true;
}

[[maybe_unused]] bool MockIsCompoundDocumentFail()
{
    return false;
}

[[maybe_unused]] bool MockIsCompoundDocumentSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidSuccess()
{
    return true;
}

[[maybe_unused]] bool MockValidFail()
{
    return false;
}

/**
 * @tc.name StorageIO_001
 * @tc.desc Test StorageIO method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, StorageIO_001, TestSize.Level1)
{
    EXPECT_EQ(storage_->dtModified_, false);
}

/**
 * @tc.name ConfigMinimalCd_001
 * @tc.desc Test ConfigMinimalCd method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ConfigMinimalCd_001, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = nullptr;
    storage_->header_ = nullptr;
    storage_->bbat_ = nullptr;
    bool res = storage_->ConfigMinimalCd(hmid);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ConfigMinimalCd_002
 * @tc.desc Test ConfigMinimalCd method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ConfigMinimalCd_002, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = nullptr;
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    bool res = storage_->ConfigMinimalCd(hmid);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ConfigMinimalCd_003
 * @tc.desc Test ConfigMinimalCd method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ConfigMinimalCd_003, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocTable>();
    bool res = storage_->ConfigMinimalCd(hmid);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ConfigMinimalCd_004
 * @tc.desc Test ConfigMinimalCd method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ConfigMinimalCd_004, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = nullptr;
    bool res = storage_->ConfigMinimalCd(hmid);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name ConfigMinimalCd_005
 * @tc.desc Test ConfigMinimalCd method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ConfigMinimalCd_005, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    bool res = storage_->ConfigMinimalCd(hmid);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name SerializeToMemory_001
 * @tc.desc Test SerializeToMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SerializeToMemory_001, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = nullptr;
    storage_->header_ = nullptr;
    storage_->bbat_ = nullptr;
    bool res = storage_->SerializeToMemory();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SerializeToMemory_002
 * @tc.desc Test SerializeToMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SerializeToMemory_002, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = nullptr;
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    bool res = storage_->SerializeToMemory();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SerializeToMemory_003
 * @tc.desc Test SerializeToMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SerializeToMemory_003, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocTable>();
    bool res = storage_->SerializeToMemory();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SerializeToMemory_004
 * @tc.desc Test SerializeToMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SerializeToMemory_004, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = nullptr;
    bool res = storage_->SerializeToMemory();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SerializeToMemory_005
 * @tc.desc Test SerializeToMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SerializeToMemory_005, TestSize.Level1)
{
    storage_->Init();
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    Stub stub;
    stub.set(ADDR(Header, Save), MockSave);
    bool res = storage_->SerializeToMemory();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Init_001
 * @tc.desc Test Init method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Init_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->stream_, nullptr);
}

/**
 * @tc.name IsValid_001
 * @tc.desc Test IsValid method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, IsValid_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->IsValid(), true);
}

/**
 * @tc.name IsDirty_001
 * @tc.desc Test IsDirty method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, IsDirty_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->IsDirty(), false);
}

/**
 * @tc.name LastError_001
 * @tc.desc Test LastError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LastError_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->LastError(), StorageIO::ErrorCode::Ok);
}

/**
 * @tc.name ErrorMessage_001
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_001, TestSize.Level1)
{
    const std::string errorMsg = "test";
    storage_->errorMsg_ = errorMsg;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, errorMsg.c_str());
}

/**
 * @tc.name ErrorMessage_002
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_002, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::Ok;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "No error");
}

/**
 * @tc.name ErrorMessage_003
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_003, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::OpenFailed;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Failed to open file");
}

/**
 * @tc.name ErrorMessage_004
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_004, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::NotOLE;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Not a valid OLE file");
}

/**
 * @tc.name ErrorMessage_005
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_005, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::BadOLE;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Corrupted OLE structure");
}

/**
 * @tc.name ErrorMessage_006
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_006, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::IOError;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "I/O operation failed");
}

/**
 * @tc.name ErrorMessage_007
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_007, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::AllocationFailed;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Block allocation failed");
}

/**
 * @tc.name ErrorMessage_008
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_008, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::InvalidOperation;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Invalid operation");
}

/**
 * @tc.name ErrorMessage_009
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_009, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::Corruption;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Data corruption detected");
}

/**
 * @tc.name ErrorMessage_010
 * @tc.desc Test ErrorMessage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ErrorMessage_010, TestSize.Level1)
{
    const std::string errorMsg = "";
    storage_->errorMsg_ = errorMsg;
    storage_->error_ = StorageIO::ErrorCode::Unknown;
    const char *result = storage_->ErrorMessage();
    EXPECT_STREQ(result, "Unknown error");
}

/**
 * @tc.name ClearError_001
 * @tc.desc Test ClearError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ClearError_001, TestSize.Level1)
{
    storage_->ClearError();
    EXPECT_EQ(storage_->IsValid(), true);
}

/**
 * @tc.name SetError_001
 * @tc.desc Test SetError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_001, TestSize.Level1)
{
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "Failed to open file";
    storage_->SetError(errorCode, errorMsg, true);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_002
 * @tc.desc Test SetError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_002, TestSize.Level1)
{
    storage_->isValid_ = true;
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "Failed to open file";
    storage_->SetError(errorCode, errorMsg, false);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_003
 * @tc.desc Test SetError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_003, TestSize.Level1)
{
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "";
    storage_->SetError(errorCode, errorMsg, true);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_004
 * @tc.desc Test SetError method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_004, TestSize.Level1)
{
    storage_->isValid_ = true;
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "";
    storage_->SetError(errorCode, errorMsg, false);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name Load_001
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_001, TestSize.Level1)
{
    storage_->stream_ = nullptr;
    EXPECT_EQ(storage_->Load(), false);
}

/**
 * @tc.name Load_002
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_002, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateHeader), MockValidateHeader);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name Load_003
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_003, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, LoadDifatChain), MockLoadDifatChain);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name Load_004
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_004, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, LoadFatChain), MockLoadFatChain);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name Load_005
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_005, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, LoadDirectory), MockLoadDirectoryTree);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name Load_006
 * @tc.desc Test Load method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_006, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFat), MockLoadMiniFat);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name ValidateHeader_001
 * @tc.desc Test ValidateHeader method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, ReadAndLoadHeader), MockReadAndLoadHeader);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_002
 * @tc.desc Test ValidateHeader method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateSectorSizes), MockValidateSectorSizes);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_003
 * @tc.desc Test ValidateHeader method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, CheckClaimTableSizes), MockCheckClaimedTableSizes);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_004
 * @tc.desc Test ValidateHeader method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_004, TestSize.Level1)
{
    storage_->header_ = nullptr;
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_005
 * @tc.desc Test ValidateHeader method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_005, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateHeaderSectorIndex), MockValidateHeaderSectorIndex);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name LoadFatChain_001
 * @tc.desc Test LoadFatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_001, TestSize.Level1)
{
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    EXPECT_EQ(storage_->LoadFatChain(0, fatBlocks), true);
}

/**
 * @tc.name LoadFatChain_002
 * @tc.desc Test LoadFatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    EXPECT_EQ(storage_->LoadFatChain(4096, fatBlocks), false);
}

/**
 * @tc.name LoadFatChain_003
 * @tc.desc Test LoadFatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    EXPECT_EQ(storage_->LoadFatChain(16, fatBlocks), true);
}

/**
 * @tc.name LoadFatChain_004
 * @tc.desc Test LoadFatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    storage_->bbat_ = std::make_unique<AllocTable>();
    EXPECT_EQ(storage_->LoadFatChain(16, fatBlocks), false);
}

/**
 * @tc.name ReadDifatSector_001
 * @tc.desc Test ReadDifatSector method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlockSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.push_back(difatSector);
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), false);
}

/**
 * @tc.name ReadDifatSector_002
 * @tc.desc Test ReadDifatSector method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlockFail);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), false);
}

/**
 * @tc.name ReadDifatSector_003
 * @tc.desc Test ReadDifatSector method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlockSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.push_back(11);
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), false);
}

/**
 * @tc.name ReadDifatSector_004
 * @tc.desc Test ReadDifatSector method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlockSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.clear();
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), false);
}

/**
 * @tc.name ProcessDifatEntries_001
 * @tc.desc Test ProcessDifatEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ProcessDifatEntries_001, TestSize.Level1)
{
    uint32_t entriesPerSector = 128;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 100;
    uint32_t loadedFromDifat = 0;
    std::vector<uint32_t> fatBlocks;
    std::vector<uint8_t> difatBuf(entriesPerSector * 4);
    for (uint32_t i = 0; i < entriesPerSector; i++) {
        if (i < expectedFromDifat) {
            WriteUint32(difatBuf.data() + i * 4, i + 1);
        } else {
            WriteUint32(difatBuf.data() + i * 4, 0);
        }
    }

    bool ret = storage_->ProcessDifatEntries(entriesPerSector, expectedFromDifat, expectedMaxSectors,
        loadedFromDifat, difatBuf, fatBlocks);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ProcessDifatEntries_002
 * @tc.desc Test ProcessDifatEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ProcessDifatEntries_002, TestSize.Level1)
{
    uint32_t entriesPerSector = 128;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    uint32_t loadedFromDifat = 0;
    std::vector<uint32_t> fatBlocks;
    std::vector<uint8_t> difatBuf(entriesPerSector * 4);
    for (uint32_t i = 0; i < entriesPerSector; i++) {
        if (i < expectedFromDifat) {
            WriteUint32(difatBuf.data() + i * 4, i + 1);
        } else {
            WriteUint32(difatBuf.data() + i * 4, 1);
        }
    }

    bool ret = storage_->ProcessDifatEntries(entriesPerSector, expectedFromDifat, expectedMaxSectors,
        loadedFromDifat, difatBuf, fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_001
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_001, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 100;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = nullptr;
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_002
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_002, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStart);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_003
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_003, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStart);
    stub.set(ADDR(StorageIO, ReadDifatSector), MockReadDifatSectorFail);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_004
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_004, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStart);
    stub.set(ADDR(StorageIO, ReadDifatSector), MockReadDifatSectorSuccess);
    stub.set(ADDR(StorageIO, ProcessDifatEntries), MockProcessDifatEntriesFail);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_005
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_005, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStart);
    stub.set(ADDR(StorageIO, ReadDifatSector), MockReadDifatSectorSuccess);
    stub.set(ADDR(StorageIO, ProcessDifatEntries), MockProcessDifatEntriesSuccess);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_006
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_006, TestSize.Level1)
{
    uint32_t sectorSize = 0;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStartFail);
    stub.set(ADDR(StorageIO, ReadDifatSector), MockReadDifatSectorSuccess);
    stub.set(ADDR(StorageIO, ProcessDifatEntries), MockProcessDifatEntriesSuccess);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_007
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_007, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStart);
    stub.set(ADDR(StorageIO, ReadDifatSector), MockReadDifatSectorSuccess);
    stub.set(ADDR(StorageIO, ProcessDifatEntries), MockProcessDifatEntriesSuccess);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_008
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_008, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 10;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStartFail);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDifatChainBody_009
 * @tc.desc Test LoadDifatChainBody method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChainBody_009, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    uint32_t expectedFromDifat = 0;
    uint32_t expectedMaxSectors = 1;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(Header, DifatStart), MockDifatStartFail);
    bool ret = storage_->LoadDifatChainBody(sectorSize, expectedFromDifat, expectedMaxSectors,
        fatBlocks);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadDifatChain_001
 * @tc.desc Test LoadDifatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChain_001, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = nullptr;
    bool ret = storage_->LoadDifatChain(sectorSize, fatBlocks);
    EXPECT_EQ(ret, false);
    EXPECT_EQ(fatBlocks.size(), 0);
}

/**
 * @tc.name LoadDifatChain_002
 * @tc.desc Test LoadDifatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChain_002, TestSize.Level1)
{
    uint32_t sectorSize = 512;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    bool ret = storage_->LoadDifatChain(sectorSize, fatBlocks);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadDifatChain_003
 * @tc.desc Test LoadDifatChain method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDifatChain_003, TestSize.Level1)
{
    uint32_t sectorSize = 0;
    std::vector<uint32_t> fatBlocks;
    storage_->header_ = std::make_unique<Header>();
    bool ret = storage_->LoadDifatChain(sectorSize, fatBlocks);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadDirectoryTree_001
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_001, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = nullptr;
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_002
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_002, TestSize.Level1)
{
    storage_->header_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocTable>();
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_003
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_003, TestSize.Level1)
{
    storage_->header_ = nullptr;
    storage_->bbat_ = nullptr;
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_004
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_004, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_005
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_005, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->dirtree_ = nullptr;
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksZero);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_006
 * @tc.desc Test LoadDirectoryTree method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_006, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->dirtree_ = std::make_unique<DirTree>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksZero);
    stub.set(ADDR(DirTree, Load), MockLoadFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFat_001
 * @tc.desc Test LoadMiniFat method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFat_002
 * @tc.desc Test LoadMiniFat method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksSuccess);
    stub.set(ADDR(StorageIO, FollowMiniStream), MockFollowMiniStreamFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFat_003
 * @tc.desc Test LoadMiniFat method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksSuccess);
    stub.set(ADDR(StorageIO, FollowMiniStream), MockFollowMiniStreamSuccess);
    stub.set(ADDR(StorageIO, ValidateMiniFatEntries), MockValidateMiniFatEntriesFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFat_004
 * @tc.desc Test LoadMiniFat method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksSuccess);
    stub.set(ADDR(StorageIO, FollowMiniStream), MockFollowMiniStreamSuccess);
    stub.set(ADDR(StorageIO, ValidateMiniFatEntries), MockValidateMiniFatEntriesSuccess);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFatBlocks_001
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_001, TestSize.Level1)
{
    storage_->bbat_ = nullptr;
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_002
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_002, TestSize.Level1)
{
    storage_->header_ = nullptr;
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_003
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowFail);
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_004
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_004, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_005
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_005, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 1;
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFatBlocks_006
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_006, TestSize.Level1)
{
    storage_->sbat_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 48;
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_007
 * @tc.desc Test LoadMiniFatBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_007, TestSize.Level1)
{
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 48;
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    stub.set(ADDR(AllocTable, Load), MockLoadSuccess);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name FollowMiniStream_001
 * @tc.desc Test FollowMiniStream method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, FollowMiniStream_007, TestSize.Level1)
{
    storage_->bbat_ = nullptr;
    bool ret = storage_->FollowMiniStream(0);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name FollowMiniStream_002
 * @tc.desc Test FollowMiniStream method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, FollowMiniStream_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowFail);
    bool ret = storage_->FollowMiniStream(0);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name FollowMiniStream_003
 * @tc.desc Test FollowMiniStream method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, FollowMiniStream_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    Stub stub;
    stub.set(ADDR(AllocTable, Follow), MockFollowSuccess);
    bool ret = storage_->FollowMiniStream(0);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniFatEntries_001
 * @tc.desc Test ValidateMiniFatEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniFatEntries_001, TestSize.Level1)
{
    storage_->sbat_ = std::make_unique<AllocTable>();
    size_t size = 0;
    bool ret = storage_->ValidateMiniFatEntries(size);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniFatEntries_002
 * @tc.desc Test ValidateMiniFatEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniFatEntries_002, TestSize.Level1)
{
    storage_->sbat_ = nullptr;
    size_t size = 0;
    bool ret = storage_->ValidateMiniFatEntries(size);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ValidateMiniRootCoverage_001
 * @tc.desc Test ValidateMiniRootCoverage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_001, TestSize.Level1)
{
    size_t size = 0;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniRootCoverage_002
 * @tc.desc Test ValidateMiniRootCoverage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, BigBlockSize), MockBigBlockSizeNonZero);
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->sbat_->blockSize_ = 0;
    storage_->sbBlocks_.resize(10);
    size_t size = 0;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniRootCoverage_003
 * @tc.desc Test ValidateMiniRootCoverage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, BigBlockSize), MockBigBlockSizeNonZero);
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->sbat_->blockSize_ = 1;
    storage_->bbat_->blockSize_ = 1;
    size_t size = 20;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ValidateMiniRootCoverage_004
 * @tc.desc Test ValidateMiniRootCoverage method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, BigBlockSize), MockBigBlockSizeZero);
    size_t size = 20;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Create_001
 * @tc.desc Test Create method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Create_001, TestSize.Level1)
{
    const char *validPath = "valid_path";
    bool ret = storage_->Create(validPath);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name Create_002
 * @tc.desc Test Create method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Create_002, TestSize.Level1)
{
    const char *invalidPath = "/invalid_path/test";
    bool ret = storage_->Create(invalidPath);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Create_003
 * @tc.desc Test Create method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Create_003, TestSize.Level1)
{
    const char *validPath = "";
    bool ret = storage_->Create(validPath);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name Close_001
 * @tc.desc Test Close method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Close_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, Flush), MockFlushFail);
    storage_->Close();
    EXPECT_EQ(storage_->stream_, nullptr);
}

/**
 * @tc.name Close_002
 * @tc.desc Test Close method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Close_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, Flush), MockFlushSuccess);
    storage_->Close();
    EXPECT_EQ(storage_->stream_, nullptr);
}

/**
 * @tc.name ReadBigBlocksFromMemory_001
 * @tc.desc Test ReadBigBlocksFromMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromMemory_001, TestSize.Level1)
{
    storage_->memoryBuffer_ = nullptr;
    std::vector<uint32_t> blocks = {0};
    Byte data[1024] = {0};
    uint32_t maxlen = 1024;
    EXPECT_EQ(storage_->ReadBigBlocksFromMemory(blocks, data, maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromMemory_002
 * @tc.desc Test ReadBigBlocksFromMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromMemory_002, TestSize.Level1)
{
    storage_->bbat_ = nullptr;
    std::vector<uint32_t> blocks = {0};
    Byte data[1024] = {0};
    uint32_t maxlen = 1024;
    EXPECT_EQ(storage_->ReadBigBlocksFromMemory(blocks, data, maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromMemory_003
 * @tc.desc Test ReadBigBlocksFromMemory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromMemory_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 1;
    std::vector<Byte> data(8, 0);
    storage_->memoryBuffer_ = std::make_unique<std::vector<uint8_t>>(std::move(data));
    std::vector<uint32_t> blocks = {0};
    Byte databuf[1024] = {0};
    uint32_t maxlen = 1024;
    EXPECT_NE(storage_->ReadBigBlocksFromMemory(blocks, databuf, maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromFile_001
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_001, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->size_ = 1024;
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name ReadBigBlocksFromFile_002
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_002, TestSize.Level1)
{
    storage_->stream_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromFile_003
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_003, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->size_ = 1024;
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 0;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name ReadBigBlocksFromFile_004
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_004, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;

    std::vector<uint32_t> blocks;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name ReadBigBlocksFromFile_005
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_005, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = nullptr;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name ReadBigBlocksFromFile_006
 * @tc.desc Test ReadBigBlocksFromFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_006, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 0;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name LoadBigBlocks_001
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_001, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->size_ = 1024;
    storage_->memoryBuffer_ = nullptr;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_002
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->size_ = 1024;
    std::vector<Byte> memoryBuffer(8, 0);
    storage_->memoryBuffer_ = std::make_unique<std::vector<Byte>>(std::move(memoryBuffer));
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_003
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, nullptr, maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_004
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_004, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_005
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_005, TestSize.Level1)
{
    storage_->bbat_ = nullptr;
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_006
 * @tc.desc Test LoadBigBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_006, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 0;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlock_001
 * @tc.desc Test LoadBigBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlock_001, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->memoryBuffer_ = nullptr;
    
    uint32_t block = 1;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlock(block, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlock_002
 * @tc.desc Test LoadBigBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlock_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    
    uint32_t block = 1;
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlock(block, nullptr, maxlen), 0);
}

/**
 * @tc.name ValidateMiniBlockAccess_001
 * @tc.desc Test ValidateMiniBlockAccess method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBlockAccess_001, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 512;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    storage_->sbBlocks_.resize(16);
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBlockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), true);
}

/**
 * @tc.name ValidateMiniBlockAccess_002
 * @tc.desc Test ValidateMiniBlockAccess method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBlockAccess_002, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 2048;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBlockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ValidateMiniBlockAccess_003
 * @tc.desc Test ValidateMiniBlockAccess method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBlockAccess_003, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = static_cast<uint32_t>(std::numeric_limits<uint64_t>::max() - 512 + 1);
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBlockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ValidateMiniBlockAccess_004
 * @tc.desc Test ValidateMiniBlockAccess method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBlockAccess_004, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 1024;
    uint64_t miniStreamSize = 2048;
    uint32_t bigBlockSize = 512;
    storage_->sbBlocks_.resize(2);
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBlockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ReadMiniBlock_001
 * @tc.desc Test ReadMiniBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlock_001, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 2048;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSz = 512;
    uint32_t smallBlockSz = 256;
    uint32_t remaining = 256;
    std::vector<Byte> dest(256);
    uint32_t copied = 0;
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateMiniBlockAccess), MockValidateMiniBlockAccessFail);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), false);
}

/**
 * @tc.name ReadMiniBlock_002
 * @tc.desc Test ReadMiniBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlock_002, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 512;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSz = 512;
    uint32_t smallBlockSz = 256;
    uint32_t remaining = 256;
    std::vector<Byte> dest(256);
    uint32_t copied = 0;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateMiniBlockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockFail);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), true);
}

/**
 * @tc.name ReadMiniBlock_003
 * @tc.desc Test ReadMiniBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlock_003, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = static_cast<uint32_t>(std::numeric_limits<uint64_t>::max() - 256 + 1);
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSz = 512;
    uint32_t smallBlockSz = 256;
    uint32_t remaining = 256;
    std::vector<Byte> dest(256);
    uint32_t copied = 0;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateMiniBlockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockSuccess);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), false);
}

/**
 * @tc.name ReadMiniBlock_004
 * @tc.desc Test ReadMiniBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlock_004, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 512;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSz = 512;
    uint32_t smallBlockSz = 256;
    uint32_t remaining = 256;
    std::vector<Byte> dest(256);
    uint32_t copied = 0;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateMiniBlockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockSuccess);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), true);
}

/**
 * @tc.name ReadMiniBlocks_001
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_001, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    uint32_t maxlen = 512;

    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, nullptr, maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_002
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_002, TestSize.Level1)
{
    std::vector<uint32_t> blocks;
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;

    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_003
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_003, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 0;

    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_004
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_004, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = nullptr;
    storage_->sbat_ = std::make_unique<AllocTable>();
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_005
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_005, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = nullptr;
    storage_->sbat_ = nullptr;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_006
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_006, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 0;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_007
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_007, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 0;
    storage_->sbat_->blockSize_ = 0;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_008
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_008, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->sbat_->blockSize_ = 256;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ReadMiniBlock), MockReadMiniBLockFail);
    constexpr int32_t res = 256;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), res);
}

/**
 * @tc.name ReadMiniBlocks_009
 * @tc.desc Test ReadMiniBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_009, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocTable>();
    storage_->sbat_ = std::make_unique<AllocTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->sbat_->blockSize_ = 256;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ReadMiniBlock), MockReadMiniBLockSuccess);
    constexpr int32_t res = 256;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), res);
}

/**
 * @tc.name LoadSmallBlocks_001
 * @tc.desc Test LoadSmallBlocks method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadSmallBlocks_001, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadSmallBlocks(blocks, nullptr, maxlen), 0);
}

/**
 * @tc.name LoadSmallBlock_001
 * @tc.desc Test LoadSmallBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadSmallBlock_001, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t maxlen = 0;
    std::vector<Byte> dest(256);
    EXPECT_EQ(storage_->LoadSmallBlock(block, dest.data(), maxlen), 0);
}

/**
 * @tc.name LoadSmallBlock_002
 * @tc.desc Test LoadSmallBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadSmallBlock_002, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t maxlen = 256;
    EXPECT_EQ(storage_->LoadSmallBlock(block, nullptr, maxlen), 0);
}

/**
 * @tc.name SaveBlockToFile_001
 * @tc.desc Test SaveBlockToFile method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlockToFile_001, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256, 0xAA);
    uint32_t len = 256;
    storage_->file_ = nullptr;
    storage_->size_ = 0;
    EXPECT_EQ(storage_->SaveBlockToFile(physicalOffset, dest.data(), len), 0);
}

/**
 * @tc.name SaveBlockToBuffer_001
 * @tc.desc Test SaveBlockToBuffer method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlockToBuffer_001, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(512, 0xBB);
    uint32_t len = 512;
    std::vector<Byte> buffer(256);
    EXPECT_EQ(storage_->SaveBlockToBuffer(physicalOffset, dest.data(), len, buffer), 512);
}

/**
 * @tc.name ListDirectory_001
 * @tc.desc Test ListDirectory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ListDirectory_001, TestSize.Level1)
{
    std::list<std::string> result;
    storage_->dirtree_ = nullptr;
    storage_->ListDirectory(result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name ListDirectory_002
 * @tc.desc Test ListDirectory method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ListDirectory_002, TestSize.Level1)
{
    std::list<std::string> result;
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->ListDirectory(result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name ListEntries_001
 * @tc.desc Test ListEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ListEntries_001, TestSize.Level1)
{
    std::vector<const DirEntry *> result;
    storage_->dirtree_ = nullptr;
    storage_->ListEntries(result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name ListEntries_002
 * @tc.desc Test ListEntries method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ListEntries_002, TestSize.Level1)
{
    std::vector<const DirEntry *> result;
    storage_->dirtree_ = std::make_unique<DirTree>();
    storage_->ListEntries(result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name Entry_001
 * @tc.desc Test Entry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Entry_001, TestSize.Level1)
{
    std::string path = "file1.txt";
    bool create = false;
    storage_->dirtree_ = nullptr;
    auto res = storage_->Entry(path, create);
    EXPECT_EQ(res, nullptr);
    EXPECT_EQ(storage_->dtModified_, false);
}

/**
 * @tc.name Entry_002
 * @tc.desc Test Entry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Entry_002, TestSize.Level1)
{
    std::string path = "file1.txt";
    bool create = true;
    storage_->dirtree_ = nullptr;
    auto res = storage_->Entry(path, create);
    EXPECT_EQ(res, nullptr);
    EXPECT_EQ(storage_->dtModified_, false);
}

/**
 * @tc.name Entry_003
 * @tc.desc Test Entry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Entry_003, TestSize.Level1)
{
    std::string path = "file1.txt";
    bool create = false;
    storage_->dirtree_ = std::make_unique<DirTree>();
    auto res = storage_->Entry(path, create);
    EXPECT_EQ(res, nullptr);
    EXPECT_EQ(storage_->dtModified_, false);
}

/**
 * @tc.name Entry_004
 * @tc.desc Test Entry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Entry_004, TestSize.Level1)
{
    std::string path = "file1.txt";
    bool create = true;
    storage_->dirtree_ = std::make_unique<DirTree>();
    auto res = storage_->Entry(path, create);
    EXPECT_NE(res, nullptr);
    EXPECT_EQ(storage_->dtModified_, true);
}

/**
 * @tc.name GetRootEntry_001
 * @tc.desc Test GetRootEntry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, GetRootEntry_001, TestSize.Level1)
{
    storage_->dirtree_ = nullptr;
    DirEntry *res = storage_->GetRootEntry();
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetRootEntry_002
 * @tc.desc Test GetRootEntry method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, GetRootEntry_002, TestSize.Level1)
{
    storage_->dirtree_ = std::make_unique<DirTree>();
    DirEntry *res = storage_->GetRootEntry();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name SaveBlock_001
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_001, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    uint32_t len = 256;
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, nullptr, len), 0);
}

/**
 * @tc.name SaveBlock_002
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_002, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), len);
}

/**
 * @tc.name SaveBlock_003
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_003, TestSize.Level1)
{
    uint64_t physicalOffset = std::numeric_limits<uint64_t>::max();
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), 0);
}

/**
 * @tc.name SaveBlock_004
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_004, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    storage_->file_ = std::make_unique<std::fstream>();
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), 0);
}

/**
 * @tc.name SaveBlock_005
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_005, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    storage_->file_ = nullptr;
    Stub stub;
    stub.set(ADDR(StorageIO, SaveBlockToBuffer), MockSaveBlockToBufferFail);
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), len);
}

/**
 * @tc.name SaveBlock_006
 * @tc.desc Test SaveBlock method
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_006, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    storage_->file_ = nullptr;
    storage_->flushWriteBuffer_ = nullptr;
    storage_->memoryBuffer_ = nullptr;
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), 0);
}

}
}
}