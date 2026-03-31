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

bool MockSaveSuccess()
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

uint32_t MockSaveBlockFail()
{
    return 0;
}

bool MockFollowSmallBlockTableFail()
{
    return false;
}

bool MockFollowSmallBlockTableSuccess()
{
    return true;
}

bool MockWriteBlockCheckedFail()
{
    return false;
}

bool MockWriteBlockCheckedSuccess()
{
    return true;
}

bool MockValidateConvertInputsSuccess()
{
    return true;
}

bool MockValidateConvertInputsFail()
{
    return false;
}

bool MockReadMiniStreamSuccess()
{
    return true;
}

bool MockReadMiniStreamFail()
{
    return false;
}

bool MockComputeBlocksNeededSuccess()
{
    return true;
}

bool MockComputeBlocksNeededFail()
{
    return false;
}

bool MockAllocateBigBlocksForConvertSuccess()
{
    return true;
}

bool MockAllocateBigBlocksForConvertFail()
{
    return false;
}

bool MockCopyDataToBigBlocksSuccess()
{
    return true;
}

bool MockCopyDataToBigBlocksFail()
{
    return false;
}

void MockReleaseMiniBlocksSuccess()
{
}

bool MockFlushDirectoryTreeSuccess()
{
    return true;
}
bool MockFlushDirectoryTreeFail()
{
    return false;
}

bool MockFlushMiniFatSuccess()
{
    return true;
}
bool MockFlushMiniFatFail()
{
    return false;
}

bool MockFlushFatChainSuccess()
{
    return true;
}
bool MockFlushFatChainFail()
{
    return false;
}

bool MockFlushDifatChainSuccess()
{
    return true;
}
bool MockFlushDifatChainFail()
{
    return false;
}

bool MockFlushHeaderSuccess()
{
    return true;
}
bool MockCollectSubtreeEntriesSuccess()
{
    return true;
}
bool MockCollectSubtreeEntriesFail()
{
    return false;
}

bool MockCollectChainForEntrySuccess()
{
    return true;
}

bool MockValidateDeletePreconditionsSuccess()
{
    return true;
}

bool MockValidateDeletePreconditionsFail()
{
    return false;
}

bool MockCollectDeleteTargetsFail()
{
    return false;
}

bool MockCollectDeleteTargetsSuccess()
{
    return true;
}

bool MockBuildReleasePlanSuccess()
{
    return true;
}

bool MockBuildReleasePlanFail()
{
    return false;
}

bool MockDeleteEntrySuccess()
{
    return true;
}

bool MockFollowBigBlockTableSuccess()
{
    return true;
}

bool MockExtendRootStreamSuccess()
{
    return true;
}

bool MockValidateAndFetchExistingChainFail()
{
    return false;
}

size_t MockUnused()
{
    return MOCK_UNUSED_BLOCKS;
}

uint32_t MockAllocateProtectedSector()
{
    return AllocTable::Eof;
}

bool MockValidateFlushStateSuccess()
{
    return true;
}

bool MockPrepareDirectoryBlocksSuccess()
{
    return true;
}

bool MockPrepareDirectoryBlocksFail()
{
    return false;
}

bool MockSetupStagingBufferSuccess()
{
    return true;
}

bool MockSetupStagingBufferFail()
{
    return false;
}

bool MockSaveMiniFatSuccess()
{
    return true;
}

bool MockSaveDifatSuccess()
{
    return true;
}

bool MockIsCompoundDocumentFail()
{
    return false;
}

bool MockIsCompoundDocumentSuccess()
{
    return true;
}

bool MockValidSuccess()
{
    return true;
}

bool MockValidFail()
{
    return false;
}

/**
 * @tc.name StorageIO_001
 * @tc.desc 测试StorageIO方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, StorageIO_001, TestSize.Level1)
{
    EXPECT_EQ(storage_->dtModified_, false);
}

/**
 * @tc.name ConfigMinimalCd_001
 * @tc.desc 测试ConfigMinimalCd方法
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
 * @tc.desc 测试ConfigMinimalCd方法
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
 * @tc.desc 测试ConfigMinimalCd方法
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
 * @tc.desc 测试ConfigMinimalCd方法
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
 * @tc.desc 测试ConfigMinimalCd方法
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
 * @tc.desc 测试SerializeToMemory方法
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
 * @tc.desc 测试SerializeToMemory方法
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
 * @tc.desc 测试SerializeToMemory方法
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
 * @tc.desc 测试SerializeToMemory方法
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
 * @tc.desc 测试SerializeToMemory方法
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
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Init_001
 * @tc.desc 测试Init方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Init_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->stream_, nullptr);
}

/**
 * @tc.name IsValid_001
 * @tc.desc 测试IsValid方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, IsValid_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->IsValid(), true);
}

/**
 * @tc.name IsDirty_001
 * @tc.desc 测试IsDirty方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, IsDirty_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->IsDirty(), false);
}

/**
 * @tc.name LastError_001
 * @tc.desc 测试LastError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LastError_001, TestSize.Level1)
{
    storage_->Init();
    EXPECT_EQ(storage_->LastError(), StorageIO::ErrorCode::Ok);
}

/**
 * @tc.name ErrorMessage_001
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ErrorMessage方法
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
 * @tc.desc 测试ClearError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ClearError_001, TestSize.Level1)
{
    storage_->error_ = ClearError();
    EXPECT_EQ(storage_->IsValid(), true);
}

/**
 * @tc.name SetError_001
 * @tc.desc 测试SetError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_001, TestSize.Level1)
{
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "Failed to open file";
    storage_->error_ = SetError(errorCode, errorMsg, true);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_002
 * @tc.desc 测试SetError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_002, TestSize.Level1)
{
    storage_->isValid_ = true;
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "Failed to open file";
    storage_->error_ = SetError(errorCode, errorMsg, false);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_003
 * @tc.desc 测试SetError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_003, TestSize.Level1)
{
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "";
    storage_->error_ = SetError(errorCode, errorMsg, true);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name SetError_004
 * @tc.desc 测试SetError方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SetError_004, TestSize.Level1)
{
    storage_->isValid_ = true;
    const StorageIO::ErrorCode errorCode = StorageIO::ErrorCode::OpenFailed;
    const std::string errorMsg = "";
    storage_->error_ = SetError(errorCode, errorMsg, false);
    EXPECT_EQ(storage_->error_, errorCode);
    EXPECT_EQ(storage_->errorMsg_, errorMsg);
}

/**
 * @tc.name Load_001
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_001, TestSize.Level1)
{
    storage_->stream_ = nullptr;
    EXPECT_EQ(storage_->Load(), false);
}

/**
 * @tc.name Load_002
 * @tc.desc 测试Load方法
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
 * @tc.desc 测试Load方法
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
 * @tc.desc 测试Load方法
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
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Load_005, TestSize.Level1)
{
    const char *testFile = "test_file.txt";
    storage_->stream_ = new std::fstream(testFile, std::ios::in | std::ios::binary | std::ios::out);
    Stub stub;
    stub.set(ADDR(StorageIO, LoadDirectory), MockLoadDirectory);
    EXPECT_EQ(storage_->Load(), false);
    delete storage_->stream_;
}

/**
 * @tc.name Load_006
 * @tc.desc 测试Load方法
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
 * @tc.desc 测试ValidateHeader方法
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
 * @tc.desc 测试ValidateHeader方法
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
 * @tc.desc 测试ValidateHeader方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, CheckClaimTableSizes), MockCheckClaimTableSizes);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_004
 * @tc.desc 测试ValidateHeader方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_004, TestSize.Level1)
{
    storage_->header_ = nullptr;
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name ValidateHeader_005
 * @tc.desc 测试ValidateHeader方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateHeader_005, TestSize.Level1)
{
    storage_->header_ = std::make_shared<Header>();
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateHeaderSectorIndex), MockValidateHeaderSectorIndex);
    EXPECT_EQ(storage_->ValidateHeader(64), false);
}

/**
 * @tc.name LoadFatChain_001
 * @tc.desc 测试LoadFatChain方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_001, TestSize.Level1)
{
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    EXPECT_EQ(storage_->LoadFatChain(0, fatBlocks), true);
}

/**
 * @tc.name LoadFatChain_002
 * @tc.desc 测试LoadFatChain方法
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
 * @tc.desc 测试LoadFatChain方法
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
 * @tc.desc 测试LoadFatChain方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadFatChain_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> fatBlocks = {1, 2, 3};
    storage_->bbat_ = std::make_unique<AllocaTable>();
    EXPECT_EQ(storage_->LoadFatChain(16, fatBlocks), true);
}

/**
 * @tc.name ReadDifatSector_001
 * @tc.desc 测试ReadDifatSector方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.push_back(difatSector);
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), true);
}

/**
 * @tc.name ReadDifatSector_002
 * @tc.desc 测试ReadDifatSector方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksFail);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), false);
}

/**
 * @tc.name ReadDifatSector_003
 * @tc.desc 测试ReadDifatSector方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.push_back(11);
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), true);
}

/**
 * @tc.name ReadDifatSector_004
 * @tc.desc 测试ReadDifatSector方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadDifatSector_004, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksSuccess);
    uint32_t sectorSize = 512;
    uint32_t difatSector = 10;
    std::vector<uint8_t> difatBuf;
    storage_->difatSectors_.clear();
    EXPECT_EQ(storage_->ReadDifatSector(sectorSize, difatSector, difatBuf), true);
}

/**
 * @tc.name ProcessDifatEntries_001
 * @tc.desc 测试ProcessDifatEntries方法
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
 * @tc.desc 测试ProcessDifatEntries方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChainBody方法
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
 * @tc.desc 测试LoadDifatChain方法
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
 * @tc.desc 测试LoadDifatChain方法
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
 * @tc.desc 测试LoadDifatChain方法
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
 * @tc.desc 测试LoadDirectoryTree方法
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
 * @tc.desc 测试LoadDirectoryTree方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_002, TestSize.Level1)
{
    storage_->header_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_003
 * @tc.desc 测试LoadDirectoryTree方法
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
 * @tc.desc 测试LoadDirectoryTree方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_004, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocaTable>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_005
 * @tc.desc 测试LoadDirectoryTree方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_005, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->dirtree_ = nullptr;
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksZero);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadDirectoryTree_006
 * @tc.desc 测试LoadDirectoryTree方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadDirectoryTree_006, TestSize.Level1)
{
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->dirtree_ = std::make_unique<DirTree>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocksZero);
    stub.set(ADDR(DirTree, Load), MockLoadFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadDirectoryTree(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFat_001
 * @tc.desc 测试LoadMiniFat方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFat_002
 * @tc.desc 测试LoadMiniFat方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFat_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, LoadMiniFatBlocks), MockLoadMiniFatBlocksSuccess);
    stub.set(ADDR(StorageIO, FollowMiniStream), MockFollowMiniStreamFail);
    SectorIndex sbStart = 0;
    bool ret = storage_->LoadMiniFat(sbStart);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFat_003
 * @tc.desc 测试LoadMiniFat方法
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
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFat_004
 * @tc.desc 测试LoadMiniFat方法
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
 * @tc.desc 测试LoadMiniFatBlocks方法
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
 * @tc.desc 测试LoadMiniFatBlocks方法
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
 * @tc.desc 测试LoadMiniFatBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowFail);
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_004
 * @tc.desc 测试LoadMiniFatBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_004, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->header_ = std::make_unique<Header>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    std::vector<uint32_t> blocks;
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name LoadMiniFatBlocks_005
 * @tc.desc 测试LoadMiniFatBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_005, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 1;
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_006
 * @tc.desc 测试LoadMiniFatBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_006, TestSize.Level1)
{
    storage_->sbat_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 48;
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name LoadMiniFatBlocks_007
 * @tc.desc 测试LoadMiniFatBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadMiniFatBlocks_007, TestSize.Level1)
{
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->header_ = std::make_unique<Header>();
    storage_->bbat_->blockSize_ = 48;
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlocks), MockLoadBigBlocks);
    stub.set(ADDR(AllocaTable, Load), MockLoadSuccess);
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> buffer;
    bool ret = storage_->LoadMiniFatBlocks(blocks, buffer);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name FollowMiniStream_001
 * @tc.desc 测试FollowMiniStream方法
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
 * @tc.desc 测试FollowMiniStream方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, FollowMiniStream_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowFail);
    bool ret = storage_->FollowMiniStream(0);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name FollowMiniStream_003
 * @tc.desc 测试FollowMiniStream方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, FollowMiniStream_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    Stub stub;
    stub.set(ADDR(AllocaTable, Follow), MockFollowSuccess);
    bool ret = storage_->FollowMiniStream(0);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniFatEntries_001
 * @tc.desc 测试ValidateMiniFatEntries方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniFatEntries_001, TestSize.Level1)
{
    storage_->sbat_ = std::make_unique<AllocaTable>();
    size_t size = 0;
    bool ret = storage_->ValidateMiniFatEntries(size);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniFatEntries_002
 * @tc.desc 测试ValidateMiniFatEntries方法
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
 * @tc.desc 测试ValidateMiniRootCoverage方法
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
 * @tc.desc 测试ValidateMiniRootCoverage方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, BigBlockSize), MockBigBlockSizeNonZero);
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_->blockSize_ = 0;
    storage_->sbBlocks_.resize(10);
    size_t size = 0;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, true);
}

/**
 * @tc.name ValidateMiniRootCoverage_003
 * @tc.desc 测试ValidateMiniRootCoverage方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniRootCoverage_003, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, BigBlockSize), MockBigBlockSizeNonZero);
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_->blockSize_ = 1;
    storage_->bbat_->blockSize_ = 1;
    size_t size = 20;
    bool ret = storage_->ValidateMiniRootCoverage(size);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name ValidateMiniRootCoverage_004
 * @tc.desc 测试ValidateMiniRootCoverage方法
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
 * @tc.desc 测试Create方法
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
 * @tc.desc 测试Create方法
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
 * @tc.desc 测试Create方法
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
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Close_001, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, Flush), MockFlushFail);
    storage_->Close();
    EXPECT_EQ(storage_->stream, nullptr);
}

/**
 * @tc.name Close_002
 * @tc.desc 测试Close方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, Close_002, TestSize.Level1)
{
    Stub stub;
    stub.set(ADDR(StorageIO, Flush), MockFlushSuccess);
    storage_->Close();
    EXPECT_EQ(storage_->stream, nullptr);
}

/**
 * @tc.name ReadBigBlocksFromMemory_001
 * @tc.desc 测试ReadBigBlocksFromMemory方法
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
 * @tc.desc 测试ReadBigBlocksFromMemory方法
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
 * @tc.desc 测试ReadBigBlocksFromMemory方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromMemory_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 1;
    std::vector<Byte> data(8, 0);
    storage_->memoryBuffer_ = std::make_unique<std::vector<uint8_t>>(std::move(data));
    std::vector<uint32_t> blocks = {0};
    Byte data[1024] = {0};
    uint32_t maxlen = 1024;
    EXPECT_NE(storage_->ReadBigBlocksFromMemory(blocks, data, maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromFile_001
 * @tc.desc 测试ReadBigBlocksFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_001, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocaTable>();
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
 * @tc.desc 测试ReadBigBlocksFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_002, TestSize.Level1)
{
    storage_->stream_ = nullptr;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name ReadBigBlocksFromFile_003
 * @tc.desc 测试ReadBigBlocksFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_003, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocaTable>();
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
 * @tc.desc 测试ReadBigBlocksFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_004, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;

    std::vector<uint32_t> blocks;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name ReadBigBlocksFromFile_005
 * @tc.desc 测试ReadBigBlocksFromFile方法
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
 * @tc.desc 测试ReadBigBlocksFromFile方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadBigBlocksFromFile_006, TestSize.Level1)
{
    const char *testPath = "test_path";
    storage_->stream_ = new std::fstream(testPath, std::ios::in | std::ios::binary | std::ios::out);
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 0;

    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->ReadBigBlocksFromFile(blocks, data.data(), maxlen), 0);
    delete storage_->stream_;
}

/**
 * @tc.name LoadBigBlocks_001
 * @tc.desc 测试LoadBigBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_001, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
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
 * @tc.desc 测试LoadBigBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
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
 * @tc.desc 测试LoadBigBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_003, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, nullptr, maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_004
 * @tc.desc 测试LoadBigBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_004, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlocks_005
 * @tc.desc 测试LoadBigBlocks方法
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
 * @tc.desc 测试LoadBigBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlocks_006, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> data(512);
    uint32_t maxlen = 0;
    EXPECT_EQ(storage_->LoadBigBlocks(blocks, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlock_001
 * @tc.desc 测试LoadBigBlock方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlock_001, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->memoryBuffer_ = nullptr;
    
    uint32_t block = 1;
    std::vector<Byte> data(512);
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlock(block, data.data(), maxlen), 0);
}

/**
 * @tc.name LoadBigBlock_002
 * @tc.desc 测试LoadBigBlock方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, LoadBigBlock_002, TestSize.Level1)
{
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    
    uint32_t block = 1;
    uint32_t maxlen = 512;
    EXPECT_EQ(storage_->LoadBigBlock(block, nullptr, maxlen), 0);
}

/**
 * @tc.name ValidateMiniBLockAccess_001
 * @tc.desc 测试ValidateMiniBLockAccess方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBLockAccess_001, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 512;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    storage_->sbBlocks_.resize(16);
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBLockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), true);
}

/**
 * @tc.name ValidateMiniBLockAccess_002
 * @tc.desc 测试ValidateMiniBLockAccess方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBLockAccess_002, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 2048;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBLockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ValidateMiniBLockAccess_003
 * @tc.desc 测试ValidateMiniBLockAccess方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBLockAccess_003, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = std::numeric_limits<uint64_t>::max() - 512 + 1;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSize = 512;
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBLockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ValidateMiniBLockAccess_004
 * @tc.desc 测试ValidateMiniBLockAccess方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ValidateMiniBLockAccess_004, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = 1024;
    uint64_t miniStreamSize = 2048;
    uint32_t bigBlockSize = 512;
    storage_->sbBlocks_.resize(2);
    uint32_t bbindex = 0;
    EXPECT_EQ(storage_->ValidateMiniBLockAccess(block, pos64, miniStreamSize, bigBlockSize, bbindex), false);
}

/**
 * @tc.name ReadMiniBlock_001
 * @tc.desc 测试ReadMiniBlock方法
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
    stub.set(ADDR(StorageIO, ValidateMiniBLockAccess), MockValidateMiniBlockAccessFail);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), false);
}

/**
 * @tc.name ReadMiniBlock_002
 * @tc.desc 测试ReadMiniBlock方法
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
    stub.set(ADDR(StorageIO, ValidateMiniBLockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockFail);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), false);
}

/**
 * @tc.name ReadMiniBlock_003
 * @tc.desc 测试ReadMiniBlock方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlock_003, TestSize.Level1)
{
    uint32_t block = 1;
    uint32_t pos64 = std::numeric_limits<uint64_t>::max() - 256 + 1;
    uint64_t miniStreamSize = 1024;
    uint32_t bigBlockSz = 512;
    uint32_t smallBlockSz = 256;
    uint32_t remaining = 256;
    std::vector<Byte> dest(256);
    uint32_t copied = 0;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ValidateMiniBLockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockSuccess);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), false);
}

/**
 * @tc.name ReadMiniBlock_004
 * @tc.desc 测试ReadMiniBlock方法
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
    stub.set(ADDR(StorageIO, ValidateMiniBLockAccess), MockValidateMiniBlockAccessSuccess);
    stub.set(ADDR(StorageIO, LoadBigBlock), MockLoadBigBlockSuccess);
    EXPECT_EQ(storage_->ReadMiniBlock(block, pos64, miniStreamSize,
        bigBlockSz, smallBlockSz, remaining, dest.data(), copied), true);
}

/**
 * @tc.name ReadMiniBlocks_001
 * @tc.desc 测试ReadMiniBlocks方法
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
 * @tc.desc 测试ReadMiniBlocks方法
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
 * @tc.desc 测试ReadMiniBlocks方法
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
 * @tc.desc 测试ReadMiniBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_004, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = nullptr;
    storage_->sbat_ = std::make_unique<AllocaTable>();
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_005
 * @tc.desc 测试ReadMiniBlocks方法
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
 * @tc.desc 测试ReadMiniBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_006, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 0;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_007
 * @tc.desc 测试ReadMiniBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_007, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 0;
    storage_->sbat_->blockSize_ = 0;
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_008
 * @tc.desc 测试ReadMiniBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_008, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->sbat_->blockSize_ = 256;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ReadMiniBlock), MockReadMiniBlockFail);
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name ReadMiniBlocks_009
 * @tc.desc 测试ReadMiniBlocks方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, ReadMiniBlocks_009, TestSize.Level1)
{
    std::vector<uint32_t> blocks = {1, 2};
    std::vector<Byte> dest(512);
    uint32_t maxlen = 512;
    storage_->bbat_ = std::make_unique<AllocaTable>();
    storage_->sbat_ = std::make_unique<AllocaTable>();
    storage_->bbat_->blockSize_ = 512;
    storage_->sbat_->blockSize_ = 256;
    storage_->sbBlocks_ = {1, 2, 3};
    Stub stub;
    stub.set(ADDR(StorageIO, ReadMiniBlock), MockReadMiniBlockSuccess);
    EXPECT_EQ(storage_->ReadMiniBlocks(blocks, dest.data(), maxlen), 0);
}

/**
 * @tc.name LoadSmallBlocks_001
 * @tc.desc 测试LoadSmallBlocks方法
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
 * @tc.desc 测试LoadSmallBlock方法
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
 * @tc.desc 测试LoadSmallBlock方法
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
 * @tc.desc 测试SaveBlockToFile方法
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
 * @tc.desc 测试SaveBlockToBuffer方法
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
 * @tc.desc 测试ListDirectory方法
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
 * @tc.desc 测试ListDirectory方法
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
 * @tc.desc 测试ListEntries方法
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
 * @tc.desc 测试ListEntries方法
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
 * @tc.desc 测试Entry方法
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
 * @tc.desc 测试Entry方法
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
 * @tc.desc 测试Entry方法
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
 * @tc.desc 测试Entry方法
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
 * @tc.desc 测试GetRootEntry方法
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
 * @tc.desc 测试GetRootEntry方法
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
 * @tc.desc 测试SaveBlock方法
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
 * @tc.desc 测试SaveBlock方法
 * @tc.type FUNC
 */
HWTEST_F(StorageTest, SaveBlock_002, TestSize.Level1)
{
    uint64_t physicalOffset = 0;
    std::vector<Byte> dest(256);
    uint32_t len = 256;
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), 0);
}

/**
 * @tc.name SaveBlock_003
 * @tc.desc 测试SaveBlock方法
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
 * @tc.desc 测试SaveBlock方法
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
 * @tc.desc 测试SaveBlock方法
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
    EXPECT_EQ(storage_->SaveBlock(physicalOffset, dest.data(), len), 0);
}

/**
 * @tc.name SaveBlock_006
 * @tc.desc 测试SaveBlock方法
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