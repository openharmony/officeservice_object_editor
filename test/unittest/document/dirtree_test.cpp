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
#include "dirtree.h"
#include "stub.h"
#include "pole.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace ObjectEditor {

class DirTreeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    std::unique_ptr<DirTree> dirtree_;
    std::unique_ptr<DirEntry> direntry_;
};

void DirTreeTest::SetUpTestCase()
{
}

void DirTreeTest::TearDownTestCase()
{
}

void DirTreeTest::SetUp()
{
    dirtree_ = std::make_unique<DirTree>();
    direntry_ = std::make_unique<DirEntry>();
}

void DirTreeTest::TearDown()
{
}

/**
 * @tc.name DirEntry_001
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DirEntry_001, TestSize.Level1)
{
    direntry_->type_ = 3;
    EXPECT_EQ(direntry_->Valid(), false);
}

/**
 * @tc.name DirEntry_002
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DirEntry_002, TestSize.Level1)
{
    direntry_->type_ = 1;
    direntry_->name = "";
    EXPECT_EQ(direntry_->Valid(), false);
}

/**
 * @tc.name DirEntry_003
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DirEntry_003, TestSize.Level1)
{
    direntry_->type_ = 1;
    direntry_->name = "tempfile";
    EXPECT_EQ(direntry_->Valid(), true);
}

/**
 * @tc.name Clsid_001
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Clsid_001, TestSize.Level1)
{
    std::array<uint8_t, 16> newClsid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    direntry_->SetClsid(newClsid, newClsid.size());
    EXPECT_EQ(direntry_->Clsid(), newClsid);
}

/**
 * @tc.name SetClsid_001
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, SetClsid_001, TestSize.Level1)
{
    std::array<uint8_t, 16> newClsid = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    direntry_->SetClsid(newClsid, newClsid.size());
    EXPECT_EQ(direntry_->clsid_, newClsid);
}

/**
 * @tc.name Clear_001
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Clear_001, TestSize.Level1)
{
    dirtree_->Clear();
    EXPECT_EQ(dirtree_->entries_.size(), 0);
}

/**
 * @tc.name Entry_001
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    EXPECT_EQ(dirtree_->Entry(1), NULL);
}

/**
 * @tc.name Entry_002
 * @tc.desc 测试DirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry_002, TestSize.Level1)
{
    dirtree_->entries_.resize(2);
    EXPECT_EQ(dirtree_->Entry(1), &(dirtree_->entries_[1]));
}

/**
 * @tc.name GetEntryAt_001
 * @tc.desc 测试GetEntryAt方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, GetEntryAt_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    EXPECT_EQ(dirtree_->GetEntryAt(1), NULL);
}

/**
 * @tc.name IndexOf_001
 * @tc.desc 测试IndexOf方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, IndexOf_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    EXPECT_EQ(dirtree_->IndexOf(nullptr), -1);
}

/**
 * @tc.name Parent_001
 * @tc.desc 测试Parent方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Parent_001, TestSize.Level1)
{
    dirtree_->entries_.resize(2);
    EXPECT_EQ(dirtree_->Parent(1), 1);
}

/**
 * @tc.name FullName_001
 * @tc.desc 测试FullName方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FullName_001, TestSize.Level1)
{
    dirtree_->entries_.resize(2);
    std::string result = "";
    dirtree_->FullName(0, result);
    EXPECT_EQ(result, "/");
}

/**
 * @tc.name FullName_002
 * @tc.desc 测试FullName方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FullName_002, TestSize.Level1)
{
    dirtree_->entries_.resize(5);
    std::string result = "";
    dirtree_->FullName(1, result);
    EXPECT_EQ(result, "/");
}

/**
 * @tc.name SplitPath_001
 * @tc.desc 测试SplitPath方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, SplitPath_001, TestSize.Level1)
{
    dirtree_->entries_.resize(5);
    std::string name = "/temp/file";
    std::list<std::string> parts;
    dirtree_->SplitPath(name, parts);
    EXPECT_EQ(parts.front(), "temp");
}

/**
 * @tc.name FindChild_001
 * @tc.desc 测试FindChild方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindChild_001, TestSize.Level1)
{
    dirtree_->entries_.resize(5);
    const std::string segment;
    EXPECT_EQ(dirtree_->FindChild(0, segment), 0);
}

/**
 * @tc.name FindChild_001
 * @tc.desc 测试FindChild方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindChild_001, TestSize.Level1)
{
    dirtree_->entries_.resize(5);
    const std::string segment;
    EXPECT_EQ(dirtree_->FindChild(0, segment), 0);
}

/**
 * @tc.name ReuseOrAppendSlot_001
 * @tc.desc 测试ReuseOrAppendSlot方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, ReuseOrAppendSlot_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    EXPECT_EQ(dirtree_->ReuseOrAppendSlot(), 1);
}

/**
 * @tc.name Entry2_001
 * @tc.desc 测试Entry2方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry2_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name;
    EXPECT_EQ(dirtree_->Entry(name, true, 2), NULL);
}

/**
 * @tc.name Entry2_002
 * @tc.desc 测试Entry2方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry2_002, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name = "/";
    EXPECT_EQ(dirtree_->Entry(name, true, 2), &(dirtree_->entries_[0]));
}

/**
 * @tc.name Entry2_003
 * @tc.desc 测试Entry2方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry2_003, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name = "/test";
    EXPECT_EQ(dirtree_->Entry(name, true, 2), NULL);
}

/**
 * @tc.name Entry2_004
 * @tc.desc 测试Entry2方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Entry2_004, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name = "/test";
    EXPECT_EQ(dirtree_->Entry(name, false, 2), NULL);
}

/**
 * @tc.name EnterDirectory_001
 * @tc.desc 测试EnterDirectory方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, EnterDirectory_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name = "";
    EXPECT_EQ(dirtree_->EnterDirectory(name), false);
}

/**
 * @tc.name EnterDirectory_002
 * @tc.desc 测试EnterDirectory方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, EnterDirectory_002, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::string name = "test";
    EXPECT_EQ(dirtree_->EnterDirectory(name), false);
}

/**
 * @tc.name LeaveDirectory_001
 * @tc.desc 测试LeaveDirectory方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, LeaveDirectory_001, TestSize.Level1)
{
    dirtree_->entries_.resize(2);
    dirtree_->current_ = 1;
    dirtree_->LeaveDirectory();
    EXPECT_EQ(dirtree_->current_, 1);
}

/**
 * @tc.name LeaveDirectory_002
 * @tc.desc 测试LeaveDirectory方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, LeaveDirectory_002, TestSize.Level1)
{
    dirtree_->entries_.resize(0);
    dirtree_->LeaveDirectory();
    EXPECT_EQ(dirtree_->current_, 0);
}

/**
 * @tc.name Load_001
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Load_001, TestSize.Level1)
{
    bool res = dirtree_->Load(nullptr, 100);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Load_002
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Load_002, TestSize.Level1)
{
    bool res = dirtree_->Load(nullptr, 0);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Load_003
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Load_003, TestSize.Level1)
{
    std::vector<uint8_t> data(10);
    bool res = dirtree_->Load(data.data(), 0);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Load_004
 * @tc.desc 测试Load方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Load_004, TestSize.Level1)
{
    std::vector<uint8_t> data(10);
    bool res = dirtree_->Load(data.data(), 128);
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Save_001
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Load_003, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    bool res = dirtree_->Save(nullptr, 128);
    EXPECT_EQ(res, false);

    std::vector<uint8_t> data(64);
    res = dirtree_->Save(data.data(), data.size());
    EXPECT_EQ(res, false);
    res = dirtree_->Save(nullptr, 64);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Save_002
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Save_002, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::vector<uint8_t> data(64);
    res = dirtree_->Save(data.data(), data.size());
    EXPECT_EQ(res, false);
}

/**
 * @tc.name Save_003
 * @tc.desc 测试Save方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Save_003, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::vector<uint8_t> data(256);
    res = dirtree_->Save(data.data(), data.size());
    EXPECT_EQ(res, true);
}

/**
 * @tc.name Debug_001
 * @tc.desc 测试Debug方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, Debug_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    dirtree_->Debug();
}

/**
 * @tc.name FindSiblings_001
 * @tc.desc 测试FindSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindSiblings_001, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<size_t> result;
    dirtree_->FindSiblings(result, 4);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name FindSiblings_002
 * @tc.desc 测试FindSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindSiblings_002, TestSize.Level1)
{
    std::vector<size_t> result;
    dirtree_->Entry(0)->type_ = 3;
    dirtree_->FindSiblings(result, 0);
    EXPECT_EQ(dirtree_->Entry(0)->Valid(), false);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name FindSiblings_003
 * @tc.desc 测试FindSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindSiblings_003, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<size_t> result;
    dirtree_->FindSiblings(result, 0);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0], 0);
}

/**
 * @tc.name FindSiblings_004
 * @tc.desc 测试FindSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindSiblings_004, TestSize.Level1)
{
    DirEntry entry1("test1", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    DirEntry entry2("test2", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    DirEntry entry3("test3", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry1);
    dirtree_->entries_.push_back(entry2);
    dirtree_->entries_.push_back(entry3);
    std::vector<size_t> result;
    dirtree_->FindSiblings(result, 4);
    EXPECT_EQ(result.size(), 1);
}

/**
 * @tc.name CollectSubtreeEntries_001
 * @tc.desc 测试CollectSubtreeEntries方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSubtreeEntries_001, TestSize.Level1)
{
    std::vector<DirEntry> result;
    std::string path = "";
    bool res = dirtree_->CollectSubtreeEntries(path, result);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CollectSubtreeEntries_002
 * @tc.desc 测试CollectSubtreeEntries方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSubtreeEntries_001, TestSize.Level1)
{
    std::vector<DirEntry> result;
    std::string path = "invalid";
    dirtree_->Entry(0)->type_ = 3;
    bool res = dirtree_->CollectSubtreeEntries(path, result);
    EXPECT_EQ(res, false);
}

/**
 * @tc.name CollectSubtree_001
 * @tc.desc 测试CollectSubtree方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSubtree_001, TestSize.Level1)
{
    dirtree_->entries_.resize(1);
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->CollectSubtree(1, visited, result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name CollectSubtree_002
 * @tc.desc 测试CollectSubtree方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSubtree_002, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->Entry(0)->type_ = 3;
    dirtree_->CollectSubtree(0, visited, result);
    EXPECT_EQ(dirtree_->Entry(0)->Valid(), false);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name CollectSubtree_003
 * @tc.desc 测试CollectSubtree方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSubtree_003, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->CollectSubtree(0, visited, result);
    EXPECT_EQ(result.size(), 1);
}

/**
 * @tc.name CollectSiblingChain_001
 * @tc.desc 测试CollectSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSiblingChain_001, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->CollectSiblingChain(5, visited, result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name CollectSiblingChain_002
 * @tc.desc 测试CollectSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSiblingChain_002, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->CollectSiblingChain(DIR_ENTRY_END, visited, result);
    EXPECT_EQ(result.size(), 0);
}

/**
 * @tc.name CollectSiblingChain_003
 * @tc.desc 测试CollectSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSiblingChain_003, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->Entry(0)->type_ = 3;
    dirtree_->CollectSiblingChain(0, visited, result);
    EXPECT_EQ(result.size(), 0);
    EXPECT_EQ(dirtree_->Entry(0)->Valid(), false);
}

/**
 * @tc.name CollectSiblingChain_004
 * @tc.desc 测试CollectSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, CollectSiblingChain_004, TestSize.Level1)
{
    DirEntry entry("Root Entry", 128, 5, 1024, 0, 0, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(entry);
    std::vector<bool> visited(1, false);
    std::vector<DirEntry> result;
    dirtree_->CollectSiblingChain(0, visited, result);
    EXPECT_EQ(result.size(), 1);
}

/**
 * @tc.name SearchPrevLink_001
 * @tc.desc 测试SearchPrevLink方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, SearchPrevLink_001, TestSize.Level1)
{
    DirEntry parent("Root Entry", 128, 5, 1024, 0, 0, 0, 1, 0, 0, 0);
    DirEntry child("Child Entry", 128, 5, 1024, 0, 0, 0, 2, 0, 0, 0);
    parent.SetChild(child.Index());
    dirtree_->entries_.push_back(child);
    dirtree_->entries_.push_back(parent);
    size_t result = dirtree_->CollectSiblingChain(child.Index());
    EXPECT_EQ(result, 1);
}

/**
 * @tc.name SearchPrevLink_002
 * @tc.desc 测试SearchPrevLink方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, SearchPrevLink_002, TestSize.Level1)
{
    DirEntry parent("Root Entry", 128, 5, 1024, 0, 0, 0, 1, 0, 0, 0);
    dirtree_->entries_.push_back(parent);
    size_t result = dirtree_->CollectSiblingChain(1);
    EXPECT_EQ(result, static_cast<size_t>(-1));
}

/**
 * @tc.name SetPrevLink_001
 * @tc.desc 测试SetPrevLink方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, SetPrevLink_001, TestSize.Level1)
{
    bool result = dirtree_->SetPrevLink(4, 1, 3);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FindRightmostSiblings_001
 * @tc.desc 测试FindRightmostSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindRightmostSiblings_001, TestSize.Level1)
{
    size_t result = dirtree_->FindRightmostSiblings(0);
    EXPECT_EQ(result, static_cast<size_t>(-1));
}

/**
 * @tc.name FindRightmostSiblings_002
 * @tc.desc 测试FindRightmostSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindRightmostSiblings_002, TestSize.Level1)
{
    size_t result = dirtree_->FindRightmostSiblings(static_cast<size_t>(-1)));
    EXPECT_EQ(result, static_cast<size_t>(-1));
}

/**
 * @tc.name FindRightmostSiblings_003
 * @tc.desc 测试FindRightmostSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindRightmostSiblings_003, TestSize.Level1)
{
    size_t result = dirtree_->FindRightmostSiblings(5);
    EXPECT_EQ(result, static_cast<size_t>(-1));
}

/**
 * @tc.name FindRightmostSiblings_004
 * @tc.desc 测试FindRightmostSiblings方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FindRightmostSiblings_004, TestSize.Level1)
{
    dirtree_->entries_.resize(5);
    size_t result = dirtree_->FindRightmostSiblings(3);
    EXPECT_EQ(result, static_cast<size_t>(-1));
}

/**
 * @tc.name EnsureVisitedBuffer_001
 * @tc.desc 测试EnsureVisitedBuffer方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, EnsureVisitedBuffer_001, TestSize.Level1)
{
    std::vector<bool> *visited = nullptr;
    bool result = dirtree_->EnsureVisitedBuffer(visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteChildrenRecursive_001
 * @tc.desc 测试DeleteChildrenRecursive方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteChildrenRecursive_001, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteChildrenRecursive("root", &parent, 0, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteChildrenRecursive_002
 * @tc.desc 测试DeleteChildrenRecursive方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteChildrenRecursive_002, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteChildrenRecursive("root", nullptr, 0, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteChildrenRecursive_003
 * @tc.desc 测试DeleteChildrenRecursive方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteChildrenRecursive_003, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteChildrenRecursive("root", nullptr, 0, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteChildrenRecursive_004
 * @tc.desc 测试DeleteChildrenRecursive方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteChildrenRecursive_004, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteChildrenRecursive("/root", &parent, 1, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteSiblingChain_001
 * @tc.desc 测试DeleteSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteSiblingChain_001, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteSiblingChain("/root", &entry, 0, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteSiblingChain_002
 * @tc.desc 测试DeleteSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteSiblingChain_002, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteSiblingChain("/root", nullptr, 1, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteSiblingChain_003
 * @tc.desc 测试DeleteSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteSiblingChain_003, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteSiblingChain("/root", nullptr, 0, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name DeleteSiblingChain_004
 * @tc.desc 测试DeleteSiblingChain方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteSiblingChain_004, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    std::vector<bool> visited(1, false);
    bool result = dirtree_->DeleteSiblingChain("/root", &entry, 1, &visited);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name FixParentLinks_001
 * @tc.desc 测试FixParentLinks方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FixParentLinks_001, TestSize.Level1)
{
    std::vector<bool> visited(1, false);
    bool result = dirtree_->FixParentLinks(nullptr, 0);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name FixParentLinks_002
 * @tc.desc 测试FixParentLinks方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, FixParentLinks_002, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    std::vector<bool> visited(1, false);
    bool result = dirtree_->FixParentLinks(&parent, 0);
    EXPECT_EQ(result, true);
}

/**
 * @tc.name ClearDirEntry_001
 * @tc.desc 测试ClearDirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, ClearDirEntry_001, TestSize.Level1)
{
    bool result = dirtree_->ClearDirEntry(nullptr);
}

/**
 * @tc.name ClearDirEntry_002
 * @tc.desc 测试ClearDirEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, ClearDirEntry_002, TestSize.Level1)
{
    DirEntry parent;
    dirtree_->entries_.push_back(parent);
    bool result = dirtree_->ClearDirEntry(&parent);
}

/**
 * @tc.name DeleteEntry_001
 * @tc.desc 测试DeleteEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteEntry_001, TestSize.Level1)
{
    std::vector<bool> visited;
    bool result = dirtree_->DeleteEntry("/", 0, &visited);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name DeleteEntry_002
 * @tc.desc 测试DeleteEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteEntry_002, TestSize.Level1)
{
    std::vector<bool> visited;
    bool result = dirtree_->DeleteEntry("", 0, &visited);
    EXPECT_EQ(result, false);
}

/**
 * @tc.name DeleteEntry_003
 * @tc.desc 测试DeleteEntry方法
 * @tc.type FUNC
 */
HWTEST_F(DirTreeTest, DeleteEntry_003, TestSize.Level1)
{
    DirEntry leaf("leaf", 128, 2, 1024, 0, DIR_ENTRY_END, 0, 0, 0, 0, 0);
    dirtree_->entries_.push_back(leaf);
    std::vector<bool> visited;
    bool result = dirtree_->DeleteEntry("leaf", 0, &visited);
    EXPECT_EQ(result, false);
}

}
}