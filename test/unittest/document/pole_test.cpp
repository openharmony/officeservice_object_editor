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

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class PoleTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<Storage> storage_;
    std::string hmid = "00000000000000000000000000000000";
    std::unique_ptr<Stream> stream_;
};

void PoleTest::SetUpTestCase()
{
}

void PoleTest::TearDownTestCase()
{
}

void PoleTest::SetUp()
{
    storage_ = std::make_unique<Storage>(hmid);
    auto io = std::make_unique<StorageIO>(hmid);
    auto impl = std::make_unique<StreamImpl>(io.get(), "test");
    stream_ = std::make_unique<Stream>(impl.get());
    impl.release();
}

void PoleTest::TearDown()
{
}

namespace {

/**
 * @tc.name Result_001
 * @tc.desc Test Result method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Result_001, TestSize.Level1)
{
    int res = storage_->Result();
    EXPECT_NE(res, Storage::UnknownError);
}

/**
 * @tc.name Result_002
 * @tc.desc Test Result method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Result_002, TestSize.Level1)
{
    storage_->io_ = nullptr;
    int res = storage_->Result();
    EXPECT_EQ(res, Storage::UnknownError);
}

/**
 * @tc.name ListEntries_001
 * @tc.desc Test ListEntries method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, ListEntries_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    std::vector<const DirEntry*> result;
    storage_->ListEntries(result);
    EXPECT_EQ(result.empty(), true);
}

/**
 * @tc.name ListEntries_002
 * @tc.desc Test ListEntries method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, ListEntries_002, TestSize.Level1)
{
    std::vector<const DirEntry*> result;
    storage_->ListEntries(result);
    EXPECT_EQ(result.empty(), true);
}

/**
 * @tc.name EnterDirectory_001
 * @tc.desc Test EnterDirectory method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, EnterDirectory_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    auto res = storage_->EnterDirectory("test");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name EnterDirectory_002
 * @tc.desc Test EnterDirectory method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, EnterDirectory_002, TestSize.Level1)
{
    auto res = storage_->EnterDirectory("test");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name LeaveDirectory_001
 * @tc.desc Test LeaveDirectory method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, LeaveDirectory_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    storage_->LeaveDirectory();
}

/**
 * @tc.name LeaveDirectory_002
 * @tc.desc Test LeaveDirectory method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, LeaveDirectory_002, TestSize.Level1)
{
    storage_->LeaveDirectory();
}

/**
 * @tc.name GetStorage_001
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_001, TestSize.Level1)
{
    DirEntry *res = storage_->GetStorage("", false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStorage_002
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_002, TestSize.Level1)
{
    storage_->io_ = nullptr;
    DirEntry *res = storage_->GetStorage("", false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStorage_003
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_003, TestSize.Level1)
{
    storage_->io_ = nullptr;
    DirEntry *res = storage_->GetStorage("test", false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStorage_004
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_004, TestSize.Level1)
{
    DirEntry *res = storage_->GetStorage("/test", false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStorage_005
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_005, TestSize.Level1)
{
    DirEntry *res = storage_->GetStorage("/a/test", false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStorage_006
 * @tc.desc Test GetStorage method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStorage_006, TestSize.Level1)
{
    DirEntry *res = storage_->GetStorage("/a/test", true);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name Path_001
 * @tc.desc Test Path method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Path_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    std::string res = "original_path";
    storage_->Path(res);
    EXPECT_EQ(res, "original_path");
}

/**
 * @tc.name Path_002
 * @tc.desc Test Path method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Path_002, TestSize.Level1)
{
    std::string res = "original_path";
    storage_->Path(res);
    EXPECT_NE(res, "original_path");
}

/**
 * @tc.name GetStream_001
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_001, TestSize.Level1)
{
    Stream *res = storage_->GetStream("", false, false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStream_002
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_002, TestSize.Level1)
{
    storage_->io_ = nullptr;
    Stream *res = storage_->GetStream("", false, false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStream_003
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_003, TestSize.Level1)
{
    storage_->io_ = nullptr;
    Stream *res = storage_->GetStream("test", false, false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStream_004
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_004, TestSize.Level1)
{
    Stream *res = storage_->GetStream("/test", false, false);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStream_005
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_005, TestSize.Level1)
{
    Stream *res = storage_->GetStream("/a/test", false, true);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetStream_006
 * @tc.desc Test GetStream method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetStream_006, TestSize.Level1)
{
    Stream *res = storage_->GetStream("/a/test", true, true);
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetRootEntry_001
 * @tc.desc Test GetRootEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetRootEntry_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    DirEntry *res = storage_->GetRootEntry();
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetRootEntry_002
 * @tc.desc Test GetRootEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetRootEntry_002, TestSize.Level1)
{
    DirEntry *res = storage_->GetRootEntry();
    EXPECT_NE(res, nullptr);
}

/**
 * @tc.name GetEntry_001
 * @tc.desc Test GetEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetEntry_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    DirEntry *res = storage_->GetEntry("", true);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name GetEntry_002
 * @tc.desc Test GetEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, GetEntry_002, TestSize.Level1)
{
    DirEntry *res = storage_->GetEntry("", true);
    EXPECT_EQ(res, nullptr);
}

/**
 * @tc.name Flush_001
 * @tc.desc Test Flush method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Flush_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    bool res =  storage_->Flush();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Flush_002
 * @tc.desc Test Flush method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Flush_002, TestSize.Level1)
{
    bool res =  storage_->Flush();
    EXPECT_EQ(res, true);
}

/**
 * @tc.name IsDirty_001
 * @tc.desc Test IsDirty method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, IsDirty_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    bool res =  storage_->IsDirty();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name IsDirty_002
 * @tc.desc Test IsDirty method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, IsDirty_002, TestSize.Level1)
{
    bool res =  storage_->IsDirty();
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SaveToFile_001
 * @tc.desc Test SaveToFile method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, SaveToFile_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    bool res =  storage_->SaveToFile("test");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name SaveToFile_002
 * @tc.desc Test SaveToFile method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, SaveToFile_002, TestSize.Level1)
{
    bool res =  storage_->SaveToFile("test");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Debug_001
 * @tc.desc Test Debug method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Debug_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    storage_->Debug();
}

/**
 * @tc.name Debug_002
 * @tc.desc Test Debug method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Debug_002, TestSize.Level1)
{
    storage_->Debug();
}

/**
 * @tc.name DeleteEntry_001
 * @tc.desc Test DeleteEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, DeleteEntry_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    bool res =  storage_->DeleteEntry("");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name DeleteEntry_002
 * @tc.desc Test DeleteEntry method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, DeleteEntry_002, TestSize.Level1)
{
    bool res =  storage_->DeleteEntry("");
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Clsid_001
 * @tc.desc Test Clssid method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Clsid_001, TestSize.Level1)
{
    CLSID empty{};
    storage_->io_ = nullptr;
    EXPECT_EQ(storage_->Clsid(), empty);
}

/**
 * @tc.name Clsid_002
 * @tc.desc Test Clssid method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Clsid_002, TestSize.Level1)
{
    CLSID empty{};
    EXPECT_EQ(storage_->Clsid(), empty);
}

/**
 * @tc.name TransactionSignature_001
 * @tc.desc Test TransactionSignature method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, TransactionSignature_001, TestSize.Level1)
{
    storage_->io_ = nullptr;
    EXPECT_EQ(storage_->TransactionSignature(), 0);
}

/**
 * @tc.name TransactionSignature_002
 * @tc.desc Test TransactionSignature method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, TransactionSignature_002, TestSize.Level1)
{
    EXPECT_EQ(storage_->TransactionSignature(), 0);
}

/**
 * @tc.name Path_001
 * @tc.desc Test Path method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Path_003, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Path(), StreamImpl::nullPath);
}

/**
 * @tc.name Path_002
 * @tc.desc Test Path method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Path_004, TestSize.Level1)
{
    EXPECT_EQ(stream_->Path(), "test");
}

/**
 * @tc.name Tell_001
 * @tc.desc Test Tell method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Tell_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Tell(), 0);
}

/**
 * @tc.name Tell_002
 * @tc.desc Test Tell method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Tell_002, TestSize.Level1)
{
    EXPECT_EQ(stream_->Tell(), 0);
}

/**
 * @tc.name Eof_001
 * @tc.desc Test Eof method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Eof_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Eof(), true);
}

/**
 * @tc.name Eof_002
 * @tc.desc Test Eof method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Eof_002, TestSize.Level1)
{
    EXPECT_EQ(stream_->Eof(), false);
}

/**
 * @tc.name Fail_001
 * @tc.desc Test Fail method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Fail_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Fail(), true);
}

/**
 * @tc.name Fail_002
 * @tc.desc Test Fail method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Fail_002, TestSize.Level1)
{
    EXPECT_EQ(stream_->Fail(), false);
}

/**
 * @tc.name Seek_001
 * @tc.desc Test Seek method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Seek_001, TestSize.Level1)
{
    stream_->Seek(0);
}

/**
 * @tc.name Size_001
 * @tc.desc Test Size method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Size_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Size(), 0);
}

/**
 * @tc.name Size_002
 * @tc.desc Test Size method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Size_002, TestSize.Level1)
{
    EXPECT_EQ(stream_->Size(), 0);
}

/**
 * @tc.name Getch_001
 * @tc.desc Test Getch method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Getch_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Getch(), 0);
}

/**
 * @tc.name Getch_002
 * @tc.desc Test Getch method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Size_003, TestSize.Level1)
{
    EXPECT_EQ(stream_->Getch(), 0);
}

/**
 * @tc.name Read_001
 * @tc.desc Test Read method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Read_001, TestSize.Level1)
{
    stream_->impl = nullptr;
    EXPECT_EQ(stream_->Read(nullptr, 1), 0);
}

/**
 * @tc.name Read_002
 * @tc.desc Test Read method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Read_002, TestSize.Level1)
{
    EXPECT_EQ(stream_->Read(nullptr, 1), 0);
}

/**
 * @tc.name Read_003
 * @tc.desc Test Read method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Read_003, TestSize.Level1)
{
    EXPECT_EQ(stream_->Read(nullptr, 0), 0);
}

/**
 * @tc.name Read_004
 * @tc.desc Test Read method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Read_004, TestSize.Level1)
{
    Byte *data = new Byte[10];
    EXPECT_EQ(stream_->Read(data, 0), 0);
    delete[] data;
}

/**
 * @tc.name Read_005
 * @tc.desc Test Read method
 * @tc.type FUNC
 */
HWTEST_F(PoleTest, Read_005, TestSize.Level1)
{
    Byte *data = new Byte[10];
    EXPECT_EQ(stream_->Read(data, 1), 0);
    delete[] data;
}

}
}
}