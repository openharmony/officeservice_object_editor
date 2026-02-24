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

#include <cstring>
#include <iomanip>
#include <sstream>
#include <list>
#include <securec.h>

#include "dirtree.h"
#include "utils.h"

namespace OHOS {
namespace ObjectEditor {
namespace {
constexpr size_t KINVALID_INDEX = static_cast<size_t>(-1);
}

bool DirEntry::Valid() const
{
    if ((type_ != EntryType::DIR) && (type_ != EntryType::FILE) && (type_ != EntryType::ROOT)) {
        return false;
    }
    if (name_.empty() || name_.find('/') != std::string::npos) {
        return false;
    }
    return true;
}

const std::array<std::uint8_t, CLSID_SIZE> &DirEntry::Clsid() const
{
    return clsid_;
}

void DirEntry::SetClsid(const std::array<std::uint8_t, CLSID_SIZE> &clsid, uint8_t size)
{
    if (size != std::size(clsid)) {
        return;
    }
    clsid_ = clsid;
    SetModif();
}

void DirTree::Clear()
{
    entries_.resize(1);
    current_ = 0;
}

const DirEntry *DirTree::Entry(size_t index) const
{
    if (index >= EntryCount())
        return NULL;
    return &entries_[index];
}

DirEntry *DirTree::GetEntryAt(size_t index)
{
    return Entry(index);
}

DirEntry *DirTree::Entry(size_t index)
{
    if (index >= EntryCount())
        return NULL;
    return &entries_[index];
}

size_t DirTree::IndexOf(const DirEntry *e) const
{
    for (size_t i = 0; i < EntryCount(); i++) {
        if (Entry(i) == e) {
            return i;
        }
    }
    return KINVALID_INDEX;
}

size_t DirTree::Parent(size_t index) const
{
    for (size_t j = 0; j < EntryCount(); j++) {
        std::vector<size_t> chi;
        Children(j, chi);
        for (size_t i = 0; i < chi.size(); i++)
            if (chi[i] == index)
                return j;
    }
    return KINVALID_INDEX;
}

void DirTree::FullName(size_t index, std::string &result) const
{
    if (index == 0) {
        result = "/";
        return;
    }

    if (!Entry(index)) {
        return;
    }
    result = Entry(index)->Name();
    result.insert(0, "/");
    size_t p = Parent(index);
    if (p == KINVALID_INDEX)
        return;
    const DirEntry *entryPtr = nullptr;
    while (p > 0) {
        entryPtr = Entry(p);
        if (entryPtr && entryPtr->IsDir() && entryPtr->Valid()) {
            result.insert(0, entryPtr->Name());
            result.insert(0, "/");
        }
        p = Parent(p);
        if (p == KINVALID_INDEX)
            break;
    }
}

void DirTree::SplitPath(const std::string &name, std::list<std::string> &parts) const
{
    parts.clear();
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    while (start < name.length()) {
        end = name.find_first_of('/', start);
        if (end == std::string::npos)
            end = name.length();
        if (end == start) {
            start = end + 1;
            continue;
        }
        parts.push_back(name.substr(start, end - start));
        start = end + 1;
    }
}

size_t DirTree::FindChild(size_t parent, const std::string &segment) const
{
    std::vector<size_t> child;
    Children(parent, child);
    for (size_t idx : child) {
        const DirEntry *ce = Entry(idx);
        if (ce && ce->Valid() && !segment.empty() && ce->Name() == segment)
            return idx;
    }
    return 0;
}

size_t DirTree::ReuseOrAppendSlot()
{
    for (size_t i = 1; i < EntryCount(); i++) {
        if (!entries_[i].Valid()) {
            return i;
        }
    }
    return EntryCount();
}

DirEntry DirTree::MakeNewEntry(const std::string &name, size_t index, uint32_t oldChild, int type) const
{
    const uint16_t nameLen = static_cast<uint16_t>(name.size() * 2 + 2);
    return DirEntry(name, nameLen, type, 0, 0, DIR_ENTRY_END,
                    oldChild,
                    DIR_ENTRY_END,
                    index);
}

DirEntry *DirTree::Entry(const std::string &name, bool create)
{
    return Entry(name, create, LEAF_TYPE_DEFAULT);
}

DirEntry *DirTree::Entry(const std::string &name, bool create, int leafType)
{
    if (name.empty())
        return NULL;
    if (name == "/")
        return &entries_[0];
    std::list<std::string> parts;
    SplitPath(name, parts);
    size_t index = (name[0] == '/') ? 0 : current_;
    auto it = parts.begin();
    while (it != parts.end()) {
        const std::string &seg = *it;
        const bool isLeaf = (std::next(it) == parts.end());
        const int segmentType = isLeaf ? leafType : 1;
        const size_t child = FindChild(index, seg);
        if (child > 0) {
            if (!isLeaf && !entries_[child].IsDir()) {
                return nullptr;
            }
            index = child;
            ++it;
            continue;
        }

        if (!create) {
            return nullptr;
        }

        if (index != 0 && !entries_[index].IsDir()) {
            return nullptr;
        }
        const size_t parentIndex = index;
        const uint32_t oldChild = entries_[parentIndex].Child();
        const size_t newIndex = ReuseOrAppendSlot();
        DirEntry e = MakeNewEntry(seg, newIndex, oldChild, segmentType);

        if (newIndex < EntryCount()) {
            entries_[newIndex] = e;
        } else {
            entries_.push_back(e);
        }
        index = newIndex;
        entries_[parentIndex].SetChild(index);
        if (oldChild != DIR_ENTRY_END && oldChild < EntryCount()) {
            DirEntry *siblingHead = Entry(oldChild);
            if (siblingHead) {
                siblingHead->SetPrev(index);
            }
        }
        ++it;
    }
    return Entry(index);
}

const DirEntry *DirTree::Entry(const std::string &name) const
{
    return const_cast<DirTree *>(this)->Entry(name, false);
}

void DirTree::Children(size_t index, std::vector<size_t> &result) const
{
    const DirEntry *e = Entry(index);
    if (e && (e->Valid() && e->Child() < EntryCount()))
        FindSiblings(result, e->Child());
}

void DirTree::ListDirectory(std::vector<const DirEntry *> &result) const
{
    std::vector<size_t> child;
    Children(current_, child);
    for (size_t i = 0; i < child.size(); i++) {
        result.push_back(Entry(child[i]));
    }
}

bool DirTree::EnterDirectory(const std::string &dir)
{
    const DirEntry *e = Entry(dir);
    if (!e || !e->Valid() || !e->IsDir()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "DirTree::EntryDirectory - entry is invalid");
        return false;
    }

    size_t index = e->Index();
    if (index == KINVALID_INDEX)
        return false;
    current_ = index;
    return true;
}

void DirTree::LeaveDirectory()
{
    if (current_ == 0)
        return;
    size_t p = Parent(current_);
    if (p != KINVALID_INDEX)
        current_ = p;
}

bool DirTree::Load(Byte *buffer, size_t size)
{
    if (buffer == nullptr && size > 0) [[unlikely]]
        return false;

    entries_.clear();
    current_ = 0;
    size_t initCount = size / 128;

    for (size_t i = 0; i < initCount; i++) {
        size_t p = i * 128;
        std::string name;
        uint16_t nameLen = ReadUint16(buffer + 0x40 + p);
        if (nameLen > MAX_NAME_LENGTH)
            nameLen = MAX_NAME_LENGTH;
        const uint16_t step = 2;
        for (uint16_t j = 0; (buffer[j + p]) && (j < nameLen); j += step)
            name.append(1, static_cast<char>(buffer[j + p]));

        uint8_t type = buffer[0x42 + p];
        uint16_t len = ReadUint16(buffer + 0x40 + p);
        uint32_t start = ReadUint32(buffer + 0x74 + p);
        uint64_t entrySize = ReadUint32(buffer + 0x78 + p);
        entrySize |= static_cast<uint64_t>(ReadUint32(buffer + 0x7C + p)) << BIT_SHIFT;
        uint32_t prev = ReadUint32(buffer + 0x44 + p);
        uint32_t next = ReadUint32(buffer + 0x48 + p);
        uint32_t child = ReadUint32(buffer + 0x4C + p);

        std::array<std::uint8_t, CLSID_SIZE> clsid;
        for (size_t j = 0; j < CLSID_SIZE; j++)
            clsid[j] = buffer[0x50 + p + j];

        DirEntry e(name, len, type, entrySize, start, prev, next, child, entries_.size());
        e.SetClsid(clsid, std::size(clsid));
        entries_.push_back(e);
    }
    return true;
}

bool DirTree::Save(Byte *buffer, size_t len)
{
    size_t size = 128 * EntryCount();
    if (len < size || buffer == nullptr) [[unlikely]]
        return false;
    auto ec = memset_s(buffer, len, 0, size);
    if (ec != EOK) {
        return false;
    }
    for (size_t i = 0; i < EntryCount(); i++) {
        const DirEntry *e = Entry(i);
        const size_t maxNameLength = 32;
        if (!e)
            return false;
        std::string name = e->Name();
        if (name.length() > maxNameLength)
            name.erase(maxNameLength, name.length());
        for (size_t j = 0; j < name.length(); j++)
            buffer[i * BUFFER_OFFSET + j * TWO_BYTE_SIZE] = static_cast<Byte>(name[j]);
        WriteUint16(buffer + i * BUFFER_OFFSET + 0x40, (uint16_t)(name.length() * TWO_BYTE_SIZE + TWO_BYTE_SIZE));
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x74, e->Start());
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x78, static_cast<uint32_t>(e->Size() & 0xFFFFFFFFULL));
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x7C, static_cast<uint32_t>((e->Size() >> BIT_SHIFT) & 0xFFFFFFFFULL));
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x44, e->Prev());
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x48, e->Next());
        WriteUint32(buffer + i * BUFFER_OFFSET + 0x4C, e->Child());

        const auto &clsid = e->Clsid();
        for (size_t j = 0; j < CLSID_SIZE; j++)
            buffer[i * BUFFER_OFFSET + CLSID_OFFSET + j] = clsid[j];
        buffer[i * BUFFER_OFFSET + TYPE_OFFSET] = e->Type();
        buffer[i * BUFFER_OFFSET + FLAG_OFFSET] = 1;
    }
    return true;
}

void DirTree::Debug()
{
    std::ostringstream oss;
    oss << "==================== DIRECTORY TREE ====================" << std::endl;
    constexpr uint32_t TWO_WIDTH = 2;
    constexpr uint32_t FOUR_WIDTH = 4;

    for (size_t i = 0; i < EntryCount(); i++) {
        const DirEntry *e = Entry(i);
        if (!e) {
            continue;
        }
        oss << "[0x" << std::uppercase << std::hex << std::setw(TWO_WIDTH) << std::setfill('0') << i << std::dec
            << "] " << e->Name() << " " << "(" << (e->IsDir() ? "Dir" : "File") << ")";
        if (!e->Valid()) {
            oss << " [INVALID]";
        }
        oss << std::endl;
        oss << "       " << "Size: " << e->Size() << " bytes, " << "Start: 0x" << std::uppercase
            << std::hex << std::setw(FOUR_WIDTH) << std::setfill('0') << e->Start() << std::dec << std::endl;
        oss << "              CLSID: ";
        {
            const auto &clsid = e->Clsid();
            oss << std::uppercase << std::hex << std::setfill('0');
            for (size_t j = 0; j < clsid.size(); ++j) {
                oss << std::setw(TWO_WIDTH) << static_cast<uint32_t>(clsid[j]);
                if (j + 1 < clsid.size()) {
                    oss << ' ';
                }
            }
            oss << std::dec << std::endl;
        }
        oss << "              Tree: child=";
        if (e->Child() == DIR_ENTRY_END) {
            oss << "[END]";
        } else {
            oss << "0x" << std::uppercase << std::hex << std::setw(TWO_WIDTH) << std::setfill('0') << e->Child()
                << std::dec;
        }
        oss << " prev=";
        if (e->Prev() == DIR_ENTRY_END) {
            oss << "[END]";
        } else {
            oss << "0x" << std::uppercase << std::hex << std::setw(TWO_WIDTH) << std::setfill('0') << e->Prev()
                << std::dec;
        }
        oss << " next=";
        if (e->Next() == DIR_ENTRY_END) {
            oss << "[END]";
        } else {
            oss << "0x" << std::uppercase << std::hex << std::setw(TWO_WIDTH) <<
                std::setfill('0') << e->Next() << std::dec;
        }
        oss << std::endl << std::endl;
    }
    oss << "=====================================================================" << std::endl;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "dirtree dump: %{public}s", oss.str().c_str());
}

void DirTree::FindSiblings(std::vector<size_t> &result, uint32_t index) const
{
    const DirEntry *e = Entry(index);
    if (!e)
        return;
    if (!e->Valid())
        return;
    for (size_t i = 0; i < result.size(); i++)
        if (result[i] == index)
            return;
    result.push_back(index);

    uint32_t prev = e->Prev();
    if ((prev > 0) && (prev < EntryCount())) {
        for (size_t i = 0; i < result.size(); i++)
            if (result[i] == prev)
                prev = 0;
        if (prev)
            FindSiblings(result, prev);
    }
    uint32_t next = e->Next();
    if ((next > 0) && (next < EntryCount())) {
        for (size_t i = 0; i < result.size(); i++)
            if (result[i] == next)
                next = 0;
        if (next)
            FindSiblings(result, next);
    }
}

bool DirTree::CollectSubtreeEntries(const std::string &path, std::vector<DirEntry> &result) const
{
    const DirEntry *root = Entry(path);
    if (!root || !root->Valid())
        return false;
    std::vector<bool> visited(EntryCount(), false);
    CollectSubtree(root->Index(), visited, result);
    return true;
}

void DirTree::CollectSubtree(size_t index, std::vector<bool> &visited,
    std::vector<DirEntry> &result) const
{
    if (index >= EntryCount())
        return;
    const DirEntry *node = Entry(index);
    if (!node || !node->Valid())
        return;
    if (visited[index])
        return;
    visited[index] = true;
    result.push_back(*node);
    if (node->IsDir() && node->Child() != DIR_ENTRY_END)
        CollectSiblingChain(node->Child(), visited, result);
}

void DirTree::CollectSiblingChain(uint32_t index, std::vector<bool> &visited,
    std::vector<DirEntry> &result) const
{
    if (index == DIR_ENTRY_END || index >= EntryCount())
        return;
    if (visited[index])
        return;
    const DirEntry *node = Entry(index);
    if (!node || !node->Valid())
        return;
    CollectSubtree(index, visited, result);
    if (node->Prev() != DIR_ENTRY_END)
        CollectSiblingChain(node->Prev(), visited, result);
    if (node->Next() != DIR_ENTRY_END)
        CollectSiblingChain(node->Next(), visited, result);
}

size_t DirTree::SearchPrevLink(size_t entryIndex)
{
    size_t parIndex = Parent(entryIndex);
    if (parIndex == KINVALID_INDEX)
        return KINVALID_INDEX;
    if (entries_[parIndex].Child() == entryIndex)
        return parIndex;
    else {
        std::vector<size_t> brothers;
        Children(parIndex, brothers);
        if (brothers.size() == 0)
            return KINVALID_INDEX;
        for (size_t ndx = 0; ndx < brothers.size(); ++ndx) {
            if (entries_[brothers[ndx]].Next() == entryIndex ||
                entries_[brothers[ndx]].Prev() == entryIndex) {
                return brothers[ndx];
            }
        }
    }
    return KINVALID_INDEX;
}

bool DirTree::SetPrevLink(size_t prevLink, size_t entry, uint32_t value)
{
    DirEntry *pl = Entry(prevLink);
    if (!pl)
        return false;
    if (pl->Prev() == entry)
        pl->SetPrev(value);
    if (pl->Next() == entry)
        pl->SetNext(value);
    if (pl->Child() == entry)
        pl->SetChild(value);
    return true;
}

size_t DirTree::FindRightmostSibling(size_t sib)
{
    if (sib == KINVALID_INDEX || sib == 0 || sib >= EntryCount())
        return KINVALID_INDEX;
    size_t current = sib;
    size_t loopControl = 0;
    while (loopControl++ < EntryCount()) {
        DirEntry *entryPtr = Entry(current);
        if (!entryPtr || !entryPtr->Valid())
            return KINVALID_INDEX;

        const uint32_t prev = entryPtr->Prev();
        if (prev == DIR_ENTRY_END)
            return entryPtr->Index();
        if (prev == 0 || prev >= EntryCount())
            return KINVALID_INDEX;
        current = static_cast<size_t>(prev);
    }
    return KINVALID_INDEX;
}

namespace {
std::string MakeChildPath(const std::string &parent, const std::string &name)
{
    if (parent.empty()) {
        return name;
    }
    if (parent.back() == '/') {
        return parent + name;
    }

    return parent + "/" + name;
}
}

bool DirTree::EnsureVisitedBuffer(std::vector<bool> *&visited)
{
    if (!visited) {
        thread_local std::vector<bool> owned;
        owned.assign(EntryCount(), false);
        visited = &owned;
    } else if (visited->size() < EntryCount()) {
        visited->resize(EntryCount(), false);
    }
    return true;
}

bool DirTree::DeleteChildrenRecursive(const std::string &path, DirEntry *e,
    int level, std::vector<bool> *visited)
{
    if (!e || e->Child() == DIR_ENTRY_END)
        return true;
    DirEntry *child = Entry(e->Child());
    if (!child)
        return false;
    const bool trailingSlash = !path.empty() && path.back() == '/';
    std::string childPath = trailingSlash ? path + child->Name() : MakeChildPath(path, child->Name());
    if (!DeleteEntry(childPath, level + 1, visited))
        return false;
    e->SetChild(DIR_ENTRY_END);
    return true;
}

bool DirTree::DeleteSiblingChain(const std:;string &path, DirEntry *e,
    int level, std::vector<bool> *visited)
{
    if (level == 0 || !e)
        return true;
    auto DeleteOne = [&](uint32_t idx) -> bool {
        if (idx == DIR_ENTRY_END)
            return true;
        DirEntry *s = Entry(idx);
        if (!s)
            return false;
        std::string childPath;
        const std::string::size_type pos = path.find_last_of("/");
        if (pos != std::string::npos)
            childPath = path.substr(0, pos + 1) + s->Name();
        else
            return false;

        const bool ok = DeleteEntry(childPath, level + 1, visited);
        if (ok) {
            if (idx == e->Prev())
                e->SetPrev(DIR_ENTRY_END);
            else if (idx == e->Next())
                e->SetNext(DIR_ENTRY_END);
        }
        return ok;
    };

    return DeleteOne(e->Prev()) && DeleteOne(e->Next());
}

bool DirTree::FixParentLinks(DirEntry *e, size_t prevLink)
{
    if (!e)
        return false;
    const uint32_t prevIdx = e->Prev();
    const uint32_t nextIdx = e->Next();
    if (nextIdx == DIR_ENTRY_END && prevIdx == DIR_ENTRY_END)
        return SetPrevLink(prevLink, e->Index(), DIR_ENTRY_END);
    if (prevIdx == DIR_ENTRY_END)
        return SetPrevLink(prevLink, e->Index(), nextIdx);
    if (nextIdx == DIR_ENTRY_END)
        return SetPrevLink(prevLink, e->Index(), prevIdx);

    size_t rightMost = FindRightmostSibling(nextIdx);
    if (rightMost == KINVALID_INDEX)
        return false;
    DirEntry *rightMostEntry = Entry(rightMost);
    DirEntry *nextEntry = Entry(nextIdx);
    if (!rightMostEntry || !nextEntry)
        return false;
    rightMostEntry->SetPrev(prevIdx);
    nextEntry->SetPrev(prevIdx);

    return SetPrevLink(prevLink, e->Index(), nextIdx);
}

void DirTree::ClearDirEntry(DirEntry *e)
{
    if (!e)
        return;
    e->Set("", 0, 0, 0, 0, DIR_ENTRY_END, DIR_ENTRY_END, DIR_ENTRY_END, 0, true);
}

bool DirTree::DeleteEntry(const std::string &path, int level, std::vector<bool> *visited)
{
    if (path == "/")
        return false;
    DirEntry *e = Entry(path);
    if (!e)
        return false;
    EnsureVisitedBuffer(visited);
    const size_t idx = e->Index();
    if (idx < visited->size() && (*visited)[idx])
        return true;
    if (idx >= visited->size())
        visited->resize(EntryCount(), false);
    (*visited)[idx] = true;
    if (e->Type() == 1 && !DeleteChildrenRecursive(path, e, level, visited))
        return false;
    if (!DeleteSiblingChain(path, e, level, visited))
        return false;
    if (level == 0) {
        size_t prevLink = SearchPrevLink(e->Index());
        if (prevLink == KINVALID_INDEX)
            return false;
        if (!FixParentLinks(e, prevLink))
            return false;
    }
    ClearDirEntry(e);
    return true;
}
}
}