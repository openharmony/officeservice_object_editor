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
constexpr size_t INVALID_INDEX = static_cast<size_t>(-1);

// UTF-8 encoding constants
constexpr uint8_t UTF8_CONT_PREFIX = 0x80;
constexpr uint8_t UTF8_CONT_LOW_MASK = 0x3F;
constexpr uint8_t UTF8_CONT_MASK = 0xC0;
constexpr uint8_t UTF8_2BYTE_PREFIX = 0xC0;
constexpr uint8_t UTF8_3BYTE_PREFIX = 0xE0;
constexpr uint8_t UTF8_4BYTE_PREFIX = 0xF0;
constexpr uint8_t UTF8_2BYTE_MASK = 0xE0;
constexpr uint8_t UTF8_3BYTE_MASK = 0xF0;
constexpr uint8_t UTF8_4BYTE_MASK = 0xF8;
constexpr uint8_t UTF8_2BYTE_LOW_MASK = 0x1F;
constexpr uint8_t UTF8_3BYTE_LOW_MASK = 0x0F;
constexpr uint8_t UTF8_4BYTE_LOW_MASK = 0x07;
constexpr uint16_t UTF8_2BYTE_MAX = 0x800; // Upper bound of 2-byte UTF-8 range (U+0800 starts 3-byte)

// UTF-8 encoding bit shifts
constexpr uint32_t UTF8_SHIFT_6 = 6;
constexpr uint32_t UTF8_SHIFT_12 = 12;
constexpr uint32_t UTF8_SHIFT_18 = 18;

// UTF-16 surrogate pair constants
constexpr uint16_t SURROGATE_HIGH_START = 0xD800;
constexpr uint16_t SURROGATE_HIGH_END = 0xDBFF;
constexpr uint16_t SURROGATE_LOW_START = 0xDC00;
constexpr uint16_t SURROGATE_LOW_END = 0xDFFF;
constexpr uint32_t SURROGATE_PAIR_SHIFT = 10;
constexpr uint32_t SURROGATE_BASE = 0x10000;
constexpr uint32_t SURROGATE_LOW_MASK = 0x3FF;
constexpr uint32_t UNICODE_MAX_BMP = 0xFFFF;
constexpr uint32_t UNICODE_MAX = 0x10FFFF;
constexpr uint32_t UNICODE_REPLACEMENT_CHAR = 0xFFFD;

// UTF-8 byte sequence lengths
constexpr size_t UTF8_SINGLE_ADVANCE = 1;
constexpr size_t UTF8_SEQ_LEN_2 = 2;
constexpr size_t UTF8_SEQ_LEN_3 = 3;
constexpr size_t UTF8_SEQ_LEN_4 = 4;

// UTF-8 continuation byte counts (for AppendUtf8Continuation)
constexpr uint32_t UTF8_CONT_COUNT_2 = 2;  // 3-byte sequence: 2 continuation bytes
constexpr uint32_t UTF8_CONT_COUNT_3 = 3;  // 4-byte sequence: 3 continuation bytes

// UTF-8 byte position offsets within multi-byte sequences
constexpr size_t UTF8_BYTE_POS_1 = 1;
constexpr size_t UTF8_BYTE_POS_2 = 2;
constexpr size_t UTF8_BYTE_POS_3 = 3;

// CFB directory entry name limits
constexpr uint16_t MAX_UTF16_CODE_UNITS = 31; // Max UTF-16 code units excluding null terminator

void AppendUtf8Continuation(std::string &result, uint32_t codePoint, uint32_t byteCount)
{
    for (uint32_t shift = (byteCount - 1) * UTF8_SHIFT_6; shift > 0; shift -= UTF8_SHIFT_6) {
        result.push_back(static_cast<char>(UTF8_CONT_PREFIX | ((codePoint >> shift) & UTF8_CONT_LOW_MASK)));
    }
    result.push_back(static_cast<char>(UTF8_CONT_PREFIX | (codePoint & UTF8_CONT_LOW_MASK)));
}

bool DecodeSurrogatePair(const Byte *buffer, uint16_t codeUnitCount, uint16_t &i, std::string &result)
{
    if (i + 1 >= codeUnitCount) {
        return false;
    }
    uint16_t low = static_cast<uint16_t>(buffer[(i + 1) * TWO_BYTE_SIZE]) |
        (static_cast<uint16_t>(buffer[(i + 1) * TWO_BYTE_SIZE + 1]) << BITS_PER_BYTE);
    if (low < SURROGATE_LOW_START || low > SURROGATE_LOW_END) {
        return false;
    }
    uint16_t high = static_cast<uint16_t>(buffer[i * TWO_BYTE_SIZE]) |
        (static_cast<uint16_t>(buffer[i * TWO_BYTE_SIZE + 1]) << BITS_PER_BYTE);
    uint32_t codePoint = SURROGATE_BASE +
        ((static_cast<uint32_t>(high) - SURROGATE_HIGH_START) << SURROGATE_PAIR_SHIFT) +
        (static_cast<uint32_t>(low) - SURROGATE_LOW_START);
    result.push_back(static_cast<char>(UTF8_4BYTE_PREFIX | (codePoint >> UTF8_SHIFT_18)));
    AppendUtf8Continuation(result, codePoint, UTF8_CONT_COUNT_3);
    i++;
    return true;
}

// Convert UTF-16LE code units from buffer to UTF-8 string
std::string Utf16LeToUtf8(const Byte *buffer, uint16_t byteLen)
{
    std::string result;
    const uint16_t codeUnitCount = byteLen / TWO_BYTE_SIZE;
    for (uint16_t i = 0; i < codeUnitCount; i++) {
        uint16_t codeUnit = static_cast<uint16_t>(buffer[i * TWO_BYTE_SIZE]) |
            (static_cast<uint16_t>(buffer[i * TWO_BYTE_SIZE + 1]) << BITS_PER_BYTE);
        if (codeUnit == 0) {
            break;
        }
        if (codeUnit < UTF8_CONT_PREFIX) {
            result.push_back(static_cast<char>(codeUnit));
        } else if (codeUnit < UTF8_2BYTE_MAX) {
            result.push_back(static_cast<char>(UTF8_2BYTE_PREFIX | (codeUnit >> UTF8_SHIFT_6)));
            result.push_back(static_cast<char>(UTF8_CONT_PREFIX | (codeUnit & UTF8_CONT_LOW_MASK)));
        } else if (codeUnit >= SURROGATE_HIGH_START && codeUnit <= SURROGATE_HIGH_END) {
            if (!DecodeSurrogatePair(buffer, codeUnitCount, i, result)) {
                result.push_back(static_cast<char>(UTF8_3BYTE_PREFIX | (UNICODE_REPLACEMENT_CHAR >> UTF8_SHIFT_12)));
                AppendUtf8Continuation(result, UNICODE_REPLACEMENT_CHAR, UTF8_CONT_COUNT_2);
            }
        } else if (codeUnit >= SURROGATE_LOW_START && codeUnit <= SURROGATE_LOW_END) {
            result.push_back(static_cast<char>(UTF8_3BYTE_PREFIX | (UNICODE_REPLACEMENT_CHAR >> UTF8_SHIFT_12)));
            AppendUtf8Continuation(result, UNICODE_REPLACEMENT_CHAR, UTF8_CONT_COUNT_2);
        } else {
            result.push_back(static_cast<char>(UTF8_3BYTE_PREFIX | (codeUnit >> UTF8_SHIFT_12)));
            AppendUtf8Continuation(result, codeUnit, UTF8_CONT_COUNT_2);
        }
    }
    return result;
}

bool IsContinuationByte(uint8_t byte)
{
    return (byte & UTF8_CONT_MASK) == UTF8_CONT_PREFIX;
}

size_t CountValidContBytes(const std::string &utf8, size_t i, size_t maxCount)
{
    size_t count = 0;
    while (count < maxCount && i + count + UTF8_SINGLE_ADVANCE < utf8.size() &&
        IsContinuationByte(static_cast<uint8_t>(utf8[i + count + UTF8_SINGLE_ADVANCE]))) {
        count++;
    }
    return count;
}

uint32_t DecodeUtf8CodePoint(const std::string &utf8, size_t &i)
{
    uint32_t codePoint = UNICODE_REPLACEMENT_CHAR;
    uint8_t byte = static_cast<uint8_t>(utf8[i]);
    if (byte < UTF8_CONT_PREFIX) {
        codePoint = byte;
        i += UTF8_SINGLE_ADVANCE;
    } else if ((byte & UTF8_2BYTE_MASK) == UTF8_2BYTE_PREFIX) {
        size_t needed = UTF8_SEQ_LEN_2 - UTF8_SINGLE_ADVANCE;
        size_t validCont = CountValidContBytes(utf8, i, needed);
        if (validCont == needed) {
            codePoint = byte & UTF8_2BYTE_LOW_MASK;
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_1]) & UTF8_CONT_LOW_MASK);
        }
        i += UTF8_SINGLE_ADVANCE + validCont;
    } else if ((byte & UTF8_3BYTE_MASK) == UTF8_3BYTE_PREFIX) {
        size_t needed = UTF8_SEQ_LEN_3 - UTF8_SINGLE_ADVANCE;
        size_t validCont = CountValidContBytes(utf8, i, needed);
        if (validCont == needed) {
            codePoint = byte & UTF8_3BYTE_LOW_MASK;
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_1]) & UTF8_CONT_LOW_MASK);
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_2]) & UTF8_CONT_LOW_MASK);
        }
        i += UTF8_SINGLE_ADVANCE + validCont;
    } else if ((byte & UTF8_4BYTE_MASK) == UTF8_4BYTE_PREFIX) {
        size_t needed = UTF8_SEQ_LEN_4 - UTF8_SINGLE_ADVANCE;
        size_t validCont = CountValidContBytes(utf8, i, needed);
        if (validCont == needed) {
            codePoint = byte & UTF8_4BYTE_LOW_MASK;
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_1]) & UTF8_CONT_LOW_MASK);
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_2]) & UTF8_CONT_LOW_MASK);
            codePoint = (codePoint << UTF8_SHIFT_6) |
                (static_cast<uint8_t>(utf8[i + UTF8_BYTE_POS_3]) & UTF8_CONT_LOW_MASK);
        }
        i += UTF8_SINGLE_ADVANCE + validCont;
    } else {
        i += UTF8_SINGLE_ADVANCE;
    }
    return codePoint;
}

void WriteUtf16CodeUnit(Byte *buffer, uint16_t &codeUnitCount, uint16_t codeUnit)
{
    buffer[codeUnitCount * TWO_BYTE_SIZE] = static_cast<Byte>(codeUnit & BYTE_MASK);
    buffer[codeUnitCount * TWO_BYTE_SIZE + 1] = static_cast<Byte>((codeUnit >> BITS_PER_BYTE) & BYTE_MASK);
    codeUnitCount++;
}

// Convert UTF-8 string to UTF-16LE code units, return code unit count
uint16_t Utf8ToUtf16Le(const std::string &utf8, Byte *buffer, size_t bufferBytes)
{
    uint16_t codeUnitCount = 0;
    const size_t maxCodeUnits = bufferBytes / TWO_BYTE_SIZE;
    size_t i = 0;
    while (i < utf8.size() && codeUnitCount < maxCodeUnits) {
        uint32_t codePoint = DecodeUtf8CodePoint(utf8, i);
        if (codePoint == 0 && i > 0 && i <= utf8.size() && utf8[i - 1] != 0) {
            continue;
        }
        if (codePoint <= UNICODE_MAX_BMP) {
            WriteUtf16CodeUnit(buffer, codeUnitCount, static_cast<uint16_t>(codePoint));
        } else if (codePoint <= UNICODE_MAX && codeUnitCount + 1 < maxCodeUnits) {
            codePoint -= SURROGATE_BASE;
            uint16_t high = static_cast<uint16_t>(SURROGATE_HIGH_START + (codePoint >> SURROGATE_PAIR_SHIFT));
            uint16_t low = static_cast<uint16_t>(SURROGATE_LOW_START + (codePoint & SURROGATE_LOW_MASK));
            WriteUtf16CodeUnit(buffer, codeUnitCount, high);
            WriteUtf16CodeUnit(buffer, codeUnitCount, low);
        } else if (codePoint <= UNICODE_MAX) {
            WriteUtf16CodeUnit(buffer, codeUnitCount, static_cast<uint16_t>(UNICODE_REPLACEMENT_CHAR));
        }
    }
    return codeUnitCount;
}

void TruncateUtf16Name(uint16_t &codeUnitCount, Byte *nameBuffer)
{
    if (codeUnitCount > MAX_UTF16_CODE_UNITS) {
        codeUnitCount = MAX_UTF16_CODE_UNITS;
    }
    if (codeUnitCount > 0) {
        size_t idx = (codeUnitCount - 1) * TWO_BYTE_SIZE;
        uint16_t lastUnit = static_cast<uint16_t>(nameBuffer[idx]) |
            (static_cast<uint16_t>(nameBuffer[idx + 1]) << BITS_PER_BYTE);
        if (lastUnit >= SURROGATE_HIGH_START && lastUnit <= SURROGATE_HIGH_END) {
            codeUnitCount--;
        }
    }
    nameBuffer[codeUnitCount * TWO_BYTE_SIZE] = 0;
    nameBuffer[codeUnitCount * TWO_BYTE_SIZE + 1] = 0;
}
}

bool DirEntry::Valid() const
{
    if ((type_ != EntryType::DIR) && (type_ != EntryType::FILE) && (type_ != EntryType::ROOT)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "type: %{public}d is invalid", type_);
        return false;
    }
    if (name_.empty() || name_.find('/') != std::string::npos) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "name: %{private}s is invalid", name_.c_str());
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
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "clsid size: %{public}d is invalid",
            static_cast<int32_t>(size));
        return;
    }
    clsid_ = clsid;
    SetModify(true, false);
}

void DirTree::Clear()
{
    entries_.resize(1);
    current_ = 0;
}

const DirEntry *DirTree::Entry(size_t index) const
{
    if (index >= EntryCount()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "index: %{public}d is out of range",
            static_cast<int32_t>(index));
        return NULL;
    }
    return &entries_[index];
}

DirEntry *DirTree::GetEntryAt(size_t index)
{
    return Entry(index);
}

DirEntry *DirTree::Entry(size_t index)
{
    if (index >= EntryCount()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "index: %{public}d is out of range",
            static_cast<int32_t>(index));
        return NULL;
    }
    return &entries_[index];
}

size_t DirTree::IndexOf(const DirEntry *e) const
{
    for (size_t i = 0; i < EntryCount(); i++) {
        if (Entry(i) == e) {
            return i;
        }
    }
    return INVALID_INDEX;
}

size_t DirTree::Parent(size_t index) const
{
    for (size_t j = 0; j < EntryCount(); j++) {
        std::vector<size_t> chi;
        Children(j, chi);
        for (size_t i = 0; i < chi.size(); i++)
            if (chi[i] == index) {
                return j;
            }
    }
    return INVALID_INDEX;
}

void DirTree::FullName(size_t index, std::string &result) const
{
    if (index == 0) {
        result = "/";
        return;
    }

    if (!Entry(index)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "index: %{public}d is out of range",
            static_cast<int32_t>(index));
        return;
    }
    result = Entry(index)->Name();
    result.insert(0, "/");
    size_t p = Parent(index);
    if (p == INVALID_INDEX) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "parent out of range for index: %{public}d",
            static_cast<int32_t>(index));
        return;
    }
    const DirEntry *entryPtr = nullptr;
    while (p > 0) {
        entryPtr = Entry(p);
        if (entryPtr && entryPtr->IsDir() && entryPtr->Valid()) {
            result.insert(0, entryPtr->Name());
            result.insert(0, "/");
        }
        p = Parent(p);
        if (p == INVALID_INDEX)
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
    Byte nameBuffer[DIR_MAX_NAME_LENGTH] = {0};
    uint16_t codeUnitCount = Utf8ToUtf16Le(name, nameBuffer, sizeof(nameBuffer));
    TruncateUtf16Name(codeUnitCount, nameBuffer);
    const uint16_t nameLen = static_cast<uint16_t>((codeUnitCount + 1) * TWO_BYTE_SIZE);
    DirEntry e(name, nameLen, type, 0, 0, DIR_ENTRY_END,
                    oldChild,
                    DIR_ENTRY_END,
                    index, 0, 0);
    return e;
}

DirEntry *DirTree::Entry(const std::string &name, bool create)
{
    return Entry(name, create, LEAF_TYPE_DEFAULT);
}

DirEntry *DirTree::Entry(const std::string &name, bool create, int leafType)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "name: %{private}s, create: %{public}d, leafType: %{public}d",
        name.c_str(), create, leafType);
    if (name.empty()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "name is empty");
        return NULL;
    }
    if (name == "/") {
        return &entries_[0];
    }
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
                OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                    "child: %{public}d is not a directory", static_cast<int32_t>(child));
                return nullptr;
            }
            index = child;
            ++it;
            continue;
        }

        if (!create) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "name: %{private}s is not found", name.c_str());
            return nullptr;
        }

        if (index > 0 && index < EntryCount() && !entries_[index].IsDir()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "parent is not a directory");
            return nullptr;
        }
        const size_t parentIndex = index;
        if (parentIndex < 0 || parentIndex >= EntryCount()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "parent index out of range");
            return nullptr;
        }
        const uint32_t oldChild = entries_[parentIndex].Child();
        const size_t newIndex = ReuseOrAppendSlot();
        DirEntry e = MakeNewEntry(seg, newIndex, oldChild, segmentType);

        if (newIndex >=0 && newIndex < EntryCount()) {
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
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "dir: %{private}s", dir.c_str());
    const DirEntry *e = Entry(dir);
    if (!e || !e->Valid() || !e->IsDir()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "name: %{private}s is invalid", dir.c_str());
        return false;
    }

    size_t index = e->Index();
    if (index == INVALID_INDEX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "invalid index");
        return false;
    }
    current_ = index;
    return true;
}

void DirTree::LeaveDirectory()
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "current: %{public}zu", current_);
    if (current_ == 0) {
        return;
    }
    size_t p = Parent(current_);
    if (p != INVALID_INDEX) {
        current_ = p;
    } else {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "no parent");
    }
}

bool DirTree::Load(Byte *buffer, size_t size)
{
    if (buffer == nullptr || size <= 0) [[unlikely]] {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "buffer is null");
        return false;
    }

    entries_.clear();
    current_ = 0;
    if (size % BUFFER_ENTRY_SIZE != 0) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "size is invalid");
        return false;
    }
    size_t initCount = size / BUFFER_ENTRY_SIZE;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "size: %{public}zu, initCount: %{public}zu", size, initCount);
    for (size_t i = 0; i < initCount; i++) {
        size_t p = i * BUFFER_ENTRY_SIZE;
        uint16_t nameLen = ReadUint16(buffer + DIR_ENTRY_NAME_LENGTH_OFFSET + p);
        if (nameLen > DIR_MAX_NAME_LENGTH) {
            nameLen = DIR_MAX_NAME_LENGTH;
        }
        std::string name = Utf16LeToUtf8(buffer + p, nameLen);
        uint8_t type = buffer[TYPE_OFFSET + p];
        uint8_t color = buffer[FLAG_OFFSET + p];
        uint16_t len = ReadUint16(buffer + DIR_ENTRY_NAME_LENGTH_OFFSET + p);
        uint32_t start = ReadUint32(buffer + DIR_ENTRY_START_OFFSET + p);
        uint64_t entrySize = ReadUint32(buffer + DIR_ENTRY_SIZE_OFFSET + p);
        entrySize |= static_cast<uint64_t>(ReadUint32(buffer + DIR_ENTRY_SIZE_HIGH_OFFSET + p)) << BIT_SHIFT;
        uint32_t prev = ReadUint32(buffer + DIR_ENTRY_PREV_OFFSET + p);
        uint32_t next = ReadUint32(buffer + DIR_ENTRY_NEXT_OFFSET + p);
        uint32_t child = ReadUint32(buffer + CHILD_OFFSET + p);
        uint32_t stateBits = ReadUint32(buffer + DIR_ENTRY_STATE_BITS_OFFSET + p);
        uint64_t creationTime = static_cast<uint64_t>(ReadUint32(buffer + DIR_ENTRY_CREATION_TIME_LOW_OFFSET + p));
        creationTime |= static_cast<uint64_t>(
            ReadUint32(buffer + DIR_ENTRY_CREATION_TIME_HIGH_OFFSET + p)) << BIT_SHIFT;
        uint64_t modifiedTime = static_cast<uint64_t>(ReadUint32(buffer + DIR_ENTRY_MODIFIED_TIME_LOW_OFFSET + p));
        modifiedTime |= static_cast<uint64_t>(
            ReadUint32(buffer + DIR_ENTRY_MODIFIED_TIME_HIGH_OFFSET + p)) << BIT_SHIFT;
        std::array<std::uint8_t, CLSID_SIZE> clsid;
        for (size_t j = 0; j < CLSID_SIZE; j++) {
            clsid[j] = buffer[DIR_CLSID_OFFSET + p + j];
        }
        DirEntry e(name, len, type, entrySize, start, prev, next, child, entries_.size(), creationTime, modifiedTime,
                   color, stateBits);
        e.SetClsid(clsid, std::size(clsid));
        entries_.push_back(e);
    }
    return true;
}

bool DirTree::SaveDirEntryName(Byte *buffer, size_t len, size_t i, const DirEntry *e)
{
    std::string name = e->Name();
    Byte nameBuffer[DIR_MAX_NAME_LENGTH] = {0};
    uint16_t codeUnitCount = Utf8ToUtf16Le(name, nameBuffer, sizeof(nameBuffer));
    TruncateUtf16Name(codeUnitCount, nameBuffer);
    size_t offset = i * BUFFER_OFFSET;
    if (len < BUFFER_OFFSET || offset > len - BUFFER_OFFSET) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "buffer too small for entry name %{public}zu", i);
        return false;
    }
    if (memcpy_s(buffer + offset, len - offset, nameBuffer,
        (codeUnitCount + 1) * TWO_BYTE_SIZE) != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "memcpy_s name failed");
        return false;
    }
    WriteUint16(buffer + offset + DIR_ENTRY_NAME_LENGTH_OFFSET,
        static_cast<uint16_t>((codeUnitCount + 1) * TWO_BYTE_SIZE));
    return true;
}

bool DirTree::SaveDirEntryFields(Byte *buffer, size_t len, size_t i, const DirEntry *e)
{
    size_t offset = i * BUFFER_OFFSET;
    if (len < BUFFER_OFFSET || offset > len - BUFFER_OFFSET) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "buffer too small for entry %{public}zu", i);
        return false;
    }
    uint64_t entrySize = e->Size();
    WriteUint32(buffer + offset + DIR_ENTRY_START_OFFSET, e->Start());
    WriteUint32(buffer + offset + DIR_ENTRY_SIZE_OFFSET,
        static_cast<uint32_t>(entrySize & FULL_MASK));
    WriteUint32(buffer + offset + DIR_ENTRY_SIZE_HIGH_OFFSET,
        static_cast<uint32_t>((entrySize >> BIT_SHIFT) & FULL_MASK));
    WriteUint32(buffer + offset + DIR_ENTRY_PREV_OFFSET, e->Prev());
    WriteUint32(buffer + offset + DIR_ENTRY_NEXT_OFFSET, e->Next());
    WriteUint32(buffer + offset + CHILD_OFFSET, e->Child());
    uint64_t ct = e->GetCreationTime();
    WriteUint32(buffer + offset + DIR_ENTRY_CREATION_TIME_LOW_OFFSET,
        static_cast<uint32_t>(ct & FULL_MASK));
    WriteUint32(buffer + offset + DIR_ENTRY_CREATION_TIME_HIGH_OFFSET,
        static_cast<uint32_t>((ct >> BIT_SHIFT) & FULL_MASK));
    uint64_t mt = e->GetModifiedTime();
    WriteUint32(buffer + offset + DIR_ENTRY_MODIFIED_TIME_LOW_OFFSET,
        static_cast<uint32_t>(mt & FULL_MASK));
    WriteUint32(buffer + offset + DIR_ENTRY_MODIFIED_TIME_HIGH_OFFSET,
        static_cast<uint32_t>((mt >> BIT_SHIFT) & FULL_MASK));
    const auto &clsid = e->Clsid();
    for (size_t j = 0; j < CLSID_SIZE; j++) {
        buffer[offset + CLSID_OFFSET + j] = clsid[j];
    }
    buffer[offset + TYPE_OFFSET] = e->Type();
    buffer[offset + FLAG_OFFSET] = e->Color();
    WriteUint32(buffer + offset + DIR_ENTRY_STATE_BITS_OFFSET, e->StateBits());
    return true;
}

bool DirTree::Save(Byte *buffer, size_t len)
{
    if (EntryCount() > SIZE_MAX / BUFFER_ENTRY_SIZE) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "EntryCount overflow in Save");
        return false;
    }
    size_t size = BUFFER_ENTRY_SIZE * EntryCount();
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "len: %{public}zu, size: %{public}zu", len, size);
    if (len < size || buffer == nullptr) [[unlikely]] {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "buffer too small or null");
        return false;
    }
    auto ec = memset_s(buffer, len, 0, size);
    if (ec != EOK) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "memset_s error: %{public}d", ec);
        return false;
    }
    for (size_t i = 0; i < EntryCount(); i++) {
        const DirEntry *e = Entry(i);
        if (!e) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry %{public}zu is null", i);
            return false;
        }
        if (!SaveDirEntryName(buffer, len, i, e)) {
            return false;
        }
        if (!SaveDirEntryFields(buffer, len, i, e)) {
            return false;
        }
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
        oss << "            CLSID: ";
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
    oss << "========================================================" << std::endl;
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "dirtree dump: %{private}s", oss.str().c_str());
}

void DirTree::FindSiblings(std::vector<size_t> &result, uint32_t index) const
{
    const DirEntry *e = Entry(index);
    if (!e) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry %{public}u is null", index);
        return;
    }
    if (!e->Valid()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry %{public}u is invalid", index);
        return;
    }
    for (size_t i = 0; i < result.size(); i++) {
        if (result[i] == index) {
            OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry %{public}u is already in result", index);
            return;
        }
    }
    result.push_back(index);

    uint32_t prev = e->Prev();
    if ((prev > 0) && (prev < EntryCount())) {
        for (size_t i = 0; i < result.size(); i++) {
            if (result[i] == prev) {
                prev = 0;
            }
        }
        if (prev) {
            FindSiblings(result, prev);
        }
    }
    uint32_t next = e->Next();
    if ((next > 0) && (next < EntryCount())) {
        for (size_t i = 0; i < result.size(); i++) {
            if (result[i] == next) {
                next = 0;
            }
        }
        if (next) {
            FindSiblings(result, next);
        }
    }
}

bool DirTree::CollectSubtreeEntries(const std::string &path, std::vector<DirEntry> &result) const
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "path: %{private}s", path.c_str());
    const DirEntry *root = Entry(path);
    if (!root || !root->Valid()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "root entry invalid");
        return false;
    }
    std::vector<bool> visited(EntryCount(), false);
    CollectSubtree(root->Index(), visited, result);
    return true;
}

void DirTree::CollectSubtree(size_t index, std::vector<bool> &visited,
    std::vector<DirEntry> &result) const
{
    if (index >= EntryCount()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "index: %{public}d is out of range",
            static_cast<int32_t>(index));
        return;
    }
    const DirEntry *node = Entry(index);
    if (!node || !node->Valid()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry %{public}d is invalid",
            static_cast<int32_t>(index));
        return;
    }
    if (visited[index]) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry %{public}d is already visited",
            static_cast<int32_t>(index));
        return;
    }
    visited[index] = true;
    result.push_back(*node);
    if (node->IsDir() && node->Child() != DIR_ENTRY_END) {
        CollectSiblingChain(node->Child(), visited, result);
    }
}

void DirTree::CollectSiblingChain(uint32_t index, std::vector<bool> &visited,
    std::vector<DirEntry> &result) const
{
    if (index == DIR_ENTRY_END || index >= EntryCount()) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "index: %{public}d is out of range",
            static_cast<int32_t>(index));
        return;
    }
    if (visited.size() < EntryCount()) {
        visited.resize(EntryCount(), false);
    }
    if (visited[index]) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry %{public}d is already visited",
            static_cast<int32_t>(index));
        return;
    }
    const DirEntry *node = Entry(index);
    if (!node || !node->Valid()) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry %{public}d is invalid",
            static_cast<int32_t>(index));
        return;
    }
    CollectSubtree(index, visited, result);
    if (node->Prev() != DIR_ENTRY_END) {
        CollectSiblingChain(node->Prev(), visited, result);
    }
    if (node->Next() != DIR_ENTRY_END) {
        CollectSiblingChain(node->Next(), visited, result);
    }
}

size_t DirTree::SearchPrevLink(size_t entryIndex)
{
    size_t parIndex = Parent(entryIndex);
    if (parIndex == INVALID_INDEX) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT,
            "parent %{public}d is invalid", static_cast<int32_t>(entryIndex));
        return INVALID_INDEX;
    }
    if (entries_[parIndex].Child() == entryIndex) {
        return parIndex;
    } else {
        std::vector<size_t> brothers;
        Children(parIndex, brothers);
        if (brothers.size() == 0) {
            OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT,
                "parent %{public}d has no siblings", static_cast<int32_t>(parIndex));
            return INVALID_INDEX;
        }
        for (size_t ndx = 0; ndx < brothers.size(); ++ndx) {
            if (entries_[brothers[ndx]].Next() == entryIndex ||
                entries_[brothers[ndx]].Prev() == entryIndex) {
                return brothers[ndx];
            }
        }
    }
    return INVALID_INDEX;
}

bool DirTree::SetPrevLink(size_t prevLink, size_t entry, uint32_t value)
{
    DirEntry *pl = Entry(prevLink);
    if (!pl) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "prev link %{public}d is null", static_cast<int32_t>(prevLink));
        return false;
    }
    if (pl->Prev() == entry) {
        pl->SetPrev(value);
    }
    if (pl->Next() == entry) {
        pl->SetNext(value);
    }
    if (pl->Child() == entry) {
        pl->SetChild(value);
    }
    return true;
}

size_t DirTree::FindRightmostSibling(size_t sib)
{
    if (sib == INVALID_INDEX || sib == 0 || sib >= EntryCount()) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "sibling %{public}d is out of range", static_cast<int32_t>(sib));
        return INVALID_INDEX;
    }
    size_t current = sib;
    size_t loopControl = 0;
    while (loopControl++ < EntryCount()) {
        DirEntry *entryPtr = Entry(current);
        if (!entryPtr || !entryPtr->Valid()) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "entry %{public}d is invalid", static_cast<int32_t>(current));
            return INVALID_INDEX;
        }
        const uint32_t prev = entryPtr->Prev();
        if (prev == DIR_ENTRY_END) {
            return entryPtr->Index();
        }
        if (prev == 0 || prev >= EntryCount()) {
            return INVALID_INDEX;
        }
        current = static_cast<size_t>(prev);
    }
    return INVALID_INDEX;
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
    if (!e || e->Child() == DIR_ENTRY_END) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry has no children");
        return true;
    }
    DirEntry *child = Entry(e->Child());
    if (!child) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "child %{public}d is null", e->Child());
        return false;
    }
    const bool trailingSlash = !path.empty() && path.back() == '/';
    std::string childPath = trailingSlash ? path + child->Name() : MakeChildPath(path, child->Name());
    if (childPath.size() > PATH_MAX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "path size exceed");
        return false;
    }
    if (!DeleteEntry(childPath, level + 1, visited)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "delete child %{private}s failed", childPath.c_str());
        return false;
    }
    e->SetChild(DIR_ENTRY_END);
    return true;
}

bool DirTree::DeleteSiblingChain(const std::string &path, DirEntry *e,
    int level, std::vector<bool> *visited)
{
    if (level == 0 || !e) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "entry is null");
        return true;
    }
    auto DeleteOne = [&](uint32_t idx) -> bool {
        if (idx == DIR_ENTRY_END) {
            OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "sibling %{public}d is null", idx);
            return true;
        }
        DirEntry *s = Entry(idx);
        if (!s) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "sibling %{public}d is null", idx);
            return false;
        }
        std::string childPath;
        const std::string::size_type pos = path.find_last_of("/");
        if (pos != std::string::npos) {
            childPath = path.substr(0, pos + 1) + s->Name();
        } else {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "sibling path %{private}s is invalid", childPath.c_str());
            return false;
        }

        const bool ok = DeleteEntry(childPath, level + 1, visited);
        if (ok) {
            if (idx == e->Prev()) {
                e->SetPrev(DIR_ENTRY_END);
            } else if (idx == e->Next()) {
                e->SetNext(DIR_ENTRY_END);
            }
        }
        return ok;
    };

    return DeleteOne(e->Prev()) && DeleteOne(e->Next());
}

bool DirTree::FixParentLinks(DirEntry *e, size_t prevLink)
{
    if (!e) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry is null");
        return false;
    }
    const uint32_t prevIdx = e->Prev();
    const uint32_t nextIdx = e->Next();
    if (nextIdx == DIR_ENTRY_END && prevIdx == DIR_ENTRY_END) {
        return SetPrevLink(prevLink, e->Index(), DIR_ENTRY_END);
    }
    if (prevIdx == DIR_ENTRY_END) {
        return SetPrevLink(prevLink, e->Index(), nextIdx);
    }
    if (nextIdx == DIR_ENTRY_END) {
        return SetPrevLink(prevLink, e->Index(), prevIdx);
    }

    size_t rightMost = FindRightmostSibling(nextIdx);
    if (rightMost == INVALID_INDEX) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "right most sibling %{public}d is invalid", static_cast<int32_t>(rightMost));
        return false;
    }
    DirEntry *rightMostEntry = Entry(rightMost);
    DirEntry *nextEntry = Entry(nextIdx);
    if (!rightMostEntry || !nextEntry) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "right most sibling %{public}d is null", static_cast<int32_t>(rightMost));
        return false;
    }
    rightMostEntry->SetPrev(prevIdx);
    nextEntry->SetPrev(prevIdx);

    return SetPrevLink(prevLink, e->Index(), nextIdx);
}

void DirTree::ClearDirEntry(DirEntry *e)
{
    if (!e) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry is null");
        return;
    }
    e->Set("", 0, 0, 0, 0, DIR_ENTRY_END, DIR_ENTRY_END, DIR_ENTRY_END, 0, 0, 0, 1, 0, true);
}

bool DirTree::DeleteEntry(const std::string &path, int level, std::vector<bool> *visited)
{
    OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT, "path: %{private}s, level: %{public}d", path.c_str(), level);
    if (path == "/") {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "cannot delete root");
        return false;
    }
    DirEntry *e = Entry(path);
    if (!e) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT, "entry not found, path: %{private}s", path.c_str());
        return false;
    }
    EnsureVisitedBuffer(visited);
    const size_t idx = e->Index();
    if (idx < visited->size() && (*visited)[idx]) {
        OBJECT_EDITOR_LOGD(ObjectEditorDomain::DOCUMENT,
            "entry %{public}d is already visited", static_cast<int32_t>(idx));
        return true;
    }
    if (idx >= visited->size()) {
        visited->resize(EntryCount(), false);
    }
    (*visited)[idx] = true;
    if (e->Type() == 1 && !DeleteChildrenRecursive(path, e, level, visited)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "delete children recursive failed, path: %{private}s", path.c_str());
        return false;
    }
    if (!DeleteSiblingChain(path, e, level, visited)) {
        OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
            "delete sibling chain failed, path: %{private}s", path.c_str());
        return false;
    }
    if (level == 0) {
        size_t prevLink = SearchPrevLink(e->Index());
        if (prevLink == INVALID_INDEX) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "search prev link failed, path: %{private}s", path.c_str());
            return false;
        }
        if (!FixParentLinks(e, prevLink)) {
            OBJECT_EDITOR_LOGE(ObjectEditorDomain::DOCUMENT,
                "fix parent links failed, path: %{private}s", path.c_str());
            return false;
        }
    }
    ClearDirEntry(e);
    OBJECT_EDITOR_LOGI(ObjectEditorDomain::DOCUMENT, "delete path: %{private}s", path.c_str());
    return true;
}
// LCOV_EXCL_STOP
} // namespace ObjectEditor
} // namespace OHOS