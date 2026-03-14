/*
 * Copyright (c) Huawei Device Co., Ltd. 2026-2026. All rights reserved.
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

#ifndef DIRTREE_H
#define DIRTREE_H

#include <array>
#include <cstdint>
#include <deque>
#include <list>
#include <string>
#include <vector>

#include "utils.h"

namespace OHOS {
namespace ObjectEditor {

class DirEntry {
public:
    static const uint32_t End;
    enum EntryType : uint8_t { DIR = 1, FILE = 2, ROOT = 5 };

public:
    DirEntry()
        : name_("Root Entry"),
          type_(EntryType::ROOT),
          size_(0),
          start_(DIR_ENTRY_END),
          prev_(DIR_ENTRY_END),
          next_(DIR_ENTRY_END),
          child_(DIR_ENTRY_END),
          clsid_{},
          index_(0),
          modify_(false)
    {
    }
    DirEntry(const std::string &name, uint16_t len, uint8_t type, uint64_t size, uint32_t start, uint32_t prev,
             uint32_t next, uint32_t child, size_t index)
    {
        Set(name, len, type, size, start, prev, next, child, index);
    }

public:
    bool Valid() const;
    const std::string &Name() const
    {
        return name_;
    }
    uint8_t Type() const
    {
        return type_;
    }
    bool Root() const
    {
        return (type_ == EntryType::ROOT);
    }
    bool IsDir() const
    {
        return ((type_ == EntryType::DIR) || (type_ == EntryType::ROOT));
    }
    bool IsFile() const
    {
        return (type_ == EntryType::FILE);
    }
    uint64_t Size() const
    {
        return size_;
    }
    uint32_t Start() const
    {
        return start_;
    }
    uint32_t Prev() const
    {
        return prev_;
    }
    uint32_t Next() const
    {
        return next_;
    }
    uint32_t Child() const
    {
        return child_;
    }
    const std::array<std::uint8_t, CLSID_SIZE> &Clsid() const;
    bool Modified() const
    {
        return modify_;
    }
    size_t Index() const
    {
        return index_;
    }

public:
    void Set(const std::string &name, uint16_t len, uint8_t type, uint64_t size, uint32_t start, uint32_t prev,
             uint32_t next, uint32_t child, size_t index, bool modify = false)
    {
        name_ = name;
        nameLen_ = len;
        type_ = type;
        size_ = size;
        start_ = start;
        prev_ = prev;
        next_ = next;
        child_ = child;
        clsid_.fill(0);
        index_ = index;
        modify_ = modify;
    }
    void SetName(const std::string &name)
    {
        name_ = name;
        SetModify();
    }
    void SetNameLen(uint16_t len)
    {
        nameLen_ = len;
        SetModify();
    }
    void SetType(uint8_t type)
    {
        type_ = type;
        SetModify();
    }
    void SetSize(uint64_t size)
    {
        size_ = size;
        SetModify();
    }
    void SetStart(uint32_t start)
    {
        start_ = start;
        SetModify();
    }
    void SetPrev(uint32_t prev)
    {
        prev_ = prev;
        SetModify();
    }
    void SetNext(uint32_t next)
    {
        next_ = next;
        SetModify();
    }
    void SetChild(uint32_t child)
    {
        child_ = child;
        SetModify();
    }

    void SetClsid(const std::array<std::uint8_t, CLSID_SIZE> &clsid, uint8_t size);
    void SetModify(bool modify = true)
    {
        modify_ = modify;
    }

private:
    std::string name_;
    uint16_t nameLen_ = 0;
    uint8_t type_ = EntryType::ROOT;
    uint64_t size_ = 0;
    uint32_t start_ = 0;
    uint32_t prev_ = 0;
    uint32_t next_ = 0;
    uint32_t child_ = 0;
    std::array<std::uint8_t, CLSID_SIZE> clsid_;
    size_t index_ = 0;
    bool modify_ = false;
};

class DirTree {
public:
    DirTree() : current_(0)
    {
        Clear();
    }
public:
    size_t EntryCount() const
    {
        return entries_.size();
    }
    size_t IndexOf(const DirEntry *e) const;
    size_t Parent(size_t index) const;
    void FullName(size_t index, std::string &) const;
    void Path(std::string &result) const
    {
        FullName(current_, result);
    }
    void Children(size_t index, std::vector<size_t> &) const;
    void ListDirectory(std::vector<const DirEntry *> &) const;
    const DirEntry *Entry(size_t index) const;
    DirEntry *GetEntryAt(size_t index);
public:
    void Clear();
    DirEntry *Entry(const std::string &name, bool create = false);
    const DirEntry *Entry(const std::string &name) const;
    bool EnterDirectory(const std::string &dir);
    void LeaveDirectory();

    bool DeleteEntry(const std::string &path, int level = 0, std::vector<bool> *visited = nullptr);
    bool CollectSubtreeEntries(const std::string &path, std::vector<DirEntry> &result) const;
    bool Load(Byte *buffer, size_t len);
    bool Save(Byte *buffer, size_t len);
    void Debug();

private:
    DirEntry *Entry(size_t index);
    DirEntry *Entry(const std::string &name, bool create, int leafType);
    void FindSiblings(std::vector<size_t> &result, uint32_t index) const;
    void CollectSubtree(size_t index, std::vector<bool> &visited, std::vector<DirEntry> &result) const;
    void CollectSiblingChain(uint32_t index, std::vector<bool> &visited, std::vector<DirEntry> &result) const;
    size_t SearchPrevLink(size_t entry);
    size_t FindRightmostSibling(size_t leftSib);
    bool SetPrevLink(size_t prevLink, size_t entry, uint32_t value);
    bool EnsureVisitedBuffer(std::vector<bool> *&visited);
    bool DeleteChildrenRecursive(const std::string &path, DirEntry *e, int level, std::vector<bool> *visited);
    bool DeleteSiblingChain(const std::string &path, DirEntry *e, int level, std::vector<bool> *visited);
    bool FixParentLinks(DirEntry *e, size_t prevLink);
    void ClearDirEntry(DirEntry *e);
    void SplitPath(const std::string &name, std::list<std::string> &parts) const;
    size_t FindChild(size_t parent, const std::string &segment) const;
    size_t ReuseOrAppendSlot();
    DirEntry MakeNewEntry(const std::string &name, size_t index, uint32_t oldChild, int type) const;

    size_t current_;
    std::deque<DirEntry> entries_;

    DirTree(const DirTree &);
    DirTree &operator=(const DirTree &);
};
} // namespace ObjectEditor
} // namespace OHOS

#endif