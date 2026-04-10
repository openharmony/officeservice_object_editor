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
#include "alloctable.h"
#include "stub.h"
#include "pole.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class AlloctableTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<Allocatable> table_;
};

void AlloctableTest::SetUpTestCase()
{
}

void AlloctableTest::TearDownTestCase()
{
}

void AlloctableTest::SetUp()
{
    table_ = std::make_unique<Allocatable>();
}

void AlloctableTest::TearDown()
{
}

/**
 * @tc.name Set_001
 * @tc.desc 测试Set方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Set_001, TestSize.Level1)
{
    table_->Resize(5);
    table_->Set(3, 0x12345678);
    EXPECT_EQ(table_->data_[3], 0x12345678);
}

/**
 * @tc.name SetChain_001
 * @tc.desc 测试SetChain方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, SetChain_001, TestSize.Level1)
{
    table_->Resize(5);
    std::vector<uint32_t> empty;
    table_->SetChain(empty);
    EXPECT_EQ(table_->Count(), 5);
}

/**
 * @tc.name SetChain_002
 * @tc.desc 测试SetChain方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, SetChain_002, TestSize.Level1)
{
    table_->Resize(5);
    std::vector<uint32_t> empty = {1, 7, 3};
    table_->SetChain(empty);
    EXPECT_EQ(table_->Count(), 8);
}

/**
 * @tc.name Follow_001
 * @tc.desc 测试Follow方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Follow_001, TestSize.Level1)
{
    table_->Resize(10);
    std::vector<uint32_t> chain;
    auto res = table_->Follow(AllocaTable::Eof, chain);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Follow_002
 * @tc.desc 测试Follow方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Follow_002, TestSize.Level1)
{
    table_->Resize(5);
    std::vector<uint32_t> chain;
    auto res = table_->Follow(10, chain);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Follow_003
 * @tc.desc 测试Follow方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Follow_003, TestSize.Level1)
{
    table_->Resize(10);
    std::vector<uint32_t> chain;
    auto res = table_->Follow(5, chain);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Unused_001
 * @tc.desc 测试Unused方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Unused_001, TestSize.Level1)
{
    table_->Resize(10);
    table_->Set(0, AllocaTable::Avail);
    EXPECT_EQ(table_->Unused(), 0);
}

/**
 * @tc.name Unused_002
 * @tc.desc 测试Unused方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Unused_002, TestSize.Level1)
{
    table_->Resize(5);
    for (uint32_t i = 0; i < 5; i++) {
        table_->Set(i, 0xDEADBEEF);
    }
    EXPECT_EQ(table_->Unused(), 5);
}

/**
 * @tc.name Load_001
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Load_001, TestSize.Level1)
{
    auto res = table_->Load(nullptr, 5);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Load_002
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Load_002, TestSize.Level1)
{
    auto res = table_->Load(nullptr, 8);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Load_003
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Load_003, TestSize.Level1)
{
    uint8_t data[12] = {0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0x11, 0x22, 0x33, 0x44};
    auto res = table_->Load(data, 8);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Save_001
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Save_001, TestSize.Level1)
{
    table_->Resize(2);
    auto res = table_->Save(nullptr, 5);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Save_002
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Save_002, TestSize.Level1)
{
    table_->Resize(1);
    auto res = table_->Save(nullptr, 5);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Save_003
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Save_003, TestSize.Level1)
{
    table_->Resize(1);
    uint8_t buffer[12] = {0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0x11, 0x22, 0x33, 0x44};
    auto res = table_->Save(buffer, 5);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Debug_001
 * @tc.desc 测试Debug方法
 * @tc.type FUNC
 */
HWTEST_F(AllocatableTest, Debug_001, TestSize.Level1)
{
    table_->Resize(1);
    auto res = table_->Debug();
}

}
}