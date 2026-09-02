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

#ifndef STUB_H
#define STUB_H

#include <cstdint>
#include <cstring>
#include <map>
#include <securec.h>
#include <sys/mman.h>
#include <unistd.h>

// Binary-patching mock framework.
//
// Patches the first N bytes of a target function with a jump to a mock
// function, then restores the original bytes on destruction/reset. Works on:
//   - x86 / x86_64  : 5-byte  jmp rel32
//   - aarch64       : 16-byte ldr x16,[pc,#8]; br x16; .quad addr
//   - ARM 32 Thumb  : 10-byte movw r12,#lo; movt r12,#hi; bx r12
//   - ARM 32 ARM     : 8-byte  ldr pc,[pc,#-4]; .word addr
class Stub {
public:
    ~Stub()
    {
        for (const auto &entry : stubs_) {
            RestoreCode(entry.first, entry.second.originalCode, entry.second.patchSize);
        }
    }

    template <typename TargetPtr, typename MockPtr>
    void set(TargetPtr targetPtr, MockPtr mockPtr)
    {
        // Cast through uintptr_t to support both function pointers and
        // const-qualified pointer-like types (e.g. string literals used as
        // target specifiers in some tests). The intermediate integer cast
        // avoids "casts away qualifiers" errors.
        auto targetAddr = reinterpret_cast<uintptr_t>(targetPtr);
        auto mockAddr = reinterpret_cast<uintptr_t>(mockPtr);
        auto *target = reinterpret_cast<uint8_t *>(targetAddr);
        auto *mock = reinterpret_cast<uint8_t *>(mockAddr);
        if (target == nullptr || mock == nullptr) {
            return;
        }

        // On ARM 32-bit, bit 0 of the function pointer is the Thumb-mode
        // indicator. Strip it to obtain the real memory address for the
        // read/write/flush operations below. On other architectures bit 0 is
        // always 0 for aligned code, so this is a harmless no-op.
        bool isThumb = (targetAddr & THUMB_MODE_BIT) != 0;
        target = reinterpret_cast<uint8_t *>(targetAddr & ~THUMB_MODE_BIT);

        StubInfo info;
        info.patchSize = GetPatchSize(isThumb);

        memcpy_s(info.originalCode, MAX_PATCH_SIZE, target, info.patchSize);
        MakeWritable(target, info.patchSize);
        WriteJump(target, mock, isThumb);
        FlushCache(target, info.patchSize);

        stubs_[target] = info;
    }

    void reset()
    {
        for (const auto &entry : stubs_) {
            RestoreCode(entry.first, entry.second.originalCode, entry.second.patchSize);
        }
        stubs_.clear();
    }

private:
    static constexpr size_t MAX_PATCH_SIZE = 16;
    // Scratch register for the Thumb-2 MOVW/MOVT sequence. R12 (IP) is the
    // intra-procedure-call scratch register per the AAPCS — safe to clobber.
    static constexpr int STUB_SCRATCH_REG = 12;

    // ARM 32-bit Thumb mode indicator bit (bit 0 of the function pointer).
    static constexpr uintptr_t THUMB_MODE_BIT = 1;

    // Patch sizes per architecture.
    static constexpr size_t X86_PATCH_SIZE = 5;        // jmp rel32: E9 + 4 bytes
    static constexpr size_t AARCH64_PATCH_SIZE = 16;   // ldr + br + .quad
    static constexpr size_t ARM_THUMB_PATCH_SIZE = 10; // movw + movt + bx
    static constexpr size_t ARM_MODE_PATCH_SIZE = 8;   // ldr pc + .word

    // Instruction encodings (little-endian byte order).
    static constexpr uint8_t X86_JMP_REL32 = 0xE9;
    static constexpr uint32_t AARCH64_LDR_X16 = 0x58000050; // LDR X16, [PC, #8]
    static constexpr uint32_t AARCH64_BR_X16 = 0xD61F0200;  // BR X16
    static constexpr uint16_t THUMB_BX_R12 = 0x4760;        // BX R12
    static constexpr uint32_t ARM_LDR_PC_LIT = 0xE51FF004;  // LDR PC, [PC, #-4]
    // Thumb-2 32-bit instruction hw1 bases (bits [15:11]=11110, plus opcode).
    static constexpr uint16_t THUMB_MOVW_HW1 = 0xF240; // bits [9:4] = 100100
    static constexpr uint16_t THUMB_MOVT_HW1 = 0xF2C0; // bits [9:4] = 101100

    // Patch layout byte offsets (position of each element within the patch).
    static constexpr size_t X86_OPCODE_SIZE = sizeof(uint8_t);            // 1-byte E9
    static constexpr size_t AARCH64_BR_OFFSET = sizeof(uint32_t);         // after LDR
    static constexpr size_t AARCH64_ADDR_OFFSET = 2 * sizeof(uint32_t);    // after LDR+BR
    static constexpr size_t ARM_THUMB_MOVT_OFFSET = sizeof(uint32_t);      // after MOVW
    static constexpr size_t ARM_THUMB_BX_OFFSET = 2 * sizeof(uint32_t);     // after MOVW+MOVT
    static constexpr size_t ARM_WORD_OFFSET = sizeof(uint32_t);            // after LDR PC
    static constexpr size_t THUMB_HW2_OFFSET = sizeof(uint16_t);           // hw2 after hw1

    // Thumb-2 MOVW/MOVT immediate field bit positions and extraction masks.
    static constexpr int THUMB_IMM4_SHIFT = 12;    // imm4: bits [15:12] of imm16
    static constexpr uint16_t THUMB_IMM4_MASK = 0xF;
    static constexpr int THUMB_I_BIT_SHIFT = 11;   // i: bit [11] of imm16
    static constexpr uint16_t THUMB_I_BIT_MASK = 0x1;
    static constexpr int THUMB_IMM3_SHIFT = 8;     // imm3: bits [10:8] of imm16
    static constexpr uint16_t THUMB_IMM3_MASK = 0x7;
    static constexpr uint16_t THUMB_IMM8_MASK = 0xFF;  // imm8: bits [7:0]
    // Position of each field within the encoded half-words.
    static constexpr int THUMB_I_HW1_SHIFT = 10;      // i bit in hw1: bit [10]
    static constexpr int THUMB_IMM3_HW2_SHIFT = 12;   // imm3 in hw2: bits [14:12]
    static constexpr int THUMB_RD_HW2_SHIFT = 8;       // Rd in hw2: bits [11:8]

    // 16-bit address split constants for MOVW/MOVT.
    static constexpr int UINT16_BIT_WIDTH = 16;
    static constexpr uintptr_t UINT16_MASK = 0xFFFF;

    struct StubInfo {
        uint8_t originalCode[MAX_PATCH_SIZE] = {0};
        size_t patchSize = 0;
    };

    std::map<uint8_t *, StubInfo> stubs_;

    static size_t GetPatchSize(bool isThumb)
    {
#if defined(__x86_64__) || defined(__i386__)
        (void)isThumb;
        return X86_PATCH_SIZE;
#elif defined(__aarch64__)
        (void)isThumb;
        return AARCH64_PATCH_SIZE;
#elif defined(__arm__) || defined(__thumb__)
        return isThumb ? ARM_THUMB_PATCH_SIZE : ARM_MODE_PATCH_SIZE;
#else
        (void)isThumb;
        return 0;
#endif
    }

    static void MakeWritable(uint8_t *addr, size_t len)
    {
        long pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize <= 0) {
            return;
        }
        auto pageMask = static_cast<uintptr_t>(pageSize) - 1;
        uintptr_t start = reinterpret_cast<uintptr_t>(addr) & ~pageMask;
        uintptr_t end = reinterpret_cast<uintptr_t>(addr) + len;
        uintptr_t endPage = (end + static_cast<uintptr_t>(pageSize) - 1) & ~pageMask;
        size_t totalLen = endPage - start;
        mprotect(reinterpret_cast<void *>(start), totalLen, PROT_READ | PROT_WRITE | PROT_EXEC);
    }

    // Encode a 32-bit Thumb-2 MOVW or MOVT instruction (4 bytes) into buf.
    // rd: destination register (0-15); imm: 16-bit immediate; isMovt: MOVT vs MOVW.
    static void WriteThumb2Mov(uint8_t *buf, int rd, uint16_t imm, bool isMovt)
    {
        uint16_t imm4 = static_cast<uint16_t>((imm >> THUMB_IMM4_SHIFT) & THUMB_IMM4_MASK);
        uint16_t i = static_cast<uint16_t>((imm >> THUMB_I_BIT_SHIFT) & THUMB_I_BIT_MASK);
        uint16_t imm3 = static_cast<uint16_t>((imm >> THUMB_IMM3_SHIFT) & THUMB_IMM3_MASK);
        uint16_t imm8 = static_cast<uint16_t>(imm & THUMB_IMM8_MASK);
        uint16_t hw1 = static_cast<uint16_t>(
            (isMovt ? THUMB_MOVT_HW1 : THUMB_MOVW_HW1) | (i << THUMB_I_HW1_SHIFT) | imm4);
        uint16_t hw2 = static_cast<uint16_t>(
            (imm3 << THUMB_IMM3_HW2_SHIFT) | (rd << THUMB_RD_HW2_SHIFT) | imm8);
        memcpy_s(buf, sizeof(hw1), &hw1, sizeof(hw1));
        memcpy_s(buf + THUMB_HW2_OFFSET, sizeof(hw2), &hw2, sizeof(hw2));
    }

    static void WriteJump(uint8_t *target, uint8_t *mock, bool isThumb)
    {
#if defined(__x86_64__) || defined(__i386__)
        (void)isThumb;
        // jmp rel32: E9 <4-byte signed relative offset>
        // offset = mock - (target + X86_PATCH_SIZE)
        int64_t diff = static_cast<int64_t>(mock - (target + X86_PATCH_SIZE));
        auto rel32 = static_cast<int32_t>(diff);
        target[0] = X86_JMP_REL32;
        memcpy_s(target + X86_OPCODE_SIZE, sizeof(rel32), &rel32, sizeof(rel32));
#elif defined(__aarch64__)
        (void)isThumb;
        // LDR X16, [PC, #8]  → loads the .quad at offset 8
        // BR X16             → branch to the mock address
        // .quad mock_addr
        uint32_t ldr = AARCH64_LDR_X16;
        uint32_t br = AARCH64_BR_X16;
        uint64_t mockAddr = static_cast<uint64_t>(reinterpret_cast<uintptr_t>(mock));
        memcpy_s(target, sizeof(ldr), &ldr, sizeof(ldr));
        memcpy_s(target + AARCH64_BR_OFFSET, sizeof(br), &br, sizeof(br));
        memcpy_s(target + AARCH64_ADDR_OFFSET, sizeof(mockAddr), &mockAddr, sizeof(mockAddr));
#elif defined(__arm__) || defined(__thumb__)
        uintptr_t mockAddr = reinterpret_cast<uintptr_t>(mock);
        if (isThumb) {
            // Thumb-2: MOVW R12, #low16; MOVT R12, #high16; BX R12
            // The mock address preserves the Thumb bit, so BX switches to the
            // correct execution mode automatically.
            uint16_t low16 = static_cast<uint16_t>(mockAddr & UINT16_MASK);
            uint16_t high16 =
                static_cast<uint16_t>((mockAddr >> UINT16_BIT_WIDTH) & UINT16_MASK);
            WriteThumb2Mov(target, STUB_SCRATCH_REG, low16, false);
            WriteThumb2Mov(target + ARM_THUMB_MOVT_OFFSET, STUB_SCRATCH_REG, high16, true);
            uint16_t bx = THUMB_BX_R12;
            memcpy_s(target + ARM_THUMB_BX_OFFSET, sizeof(bx), &bx, sizeof(bx));
        } else {
            // ARM mode: LDR PC, [PC, #-4]; .word mockAddr
            // PC is current instruction + 8 in ARM mode, so [PC, #-4] loads
            // the word immediately following the LDR instruction.
            uint32_t ldr = ARM_LDR_PC_LIT;
            memcpy_s(target, sizeof(ldr), &ldr, sizeof(ldr));
            memcpy_s(target + ARM_WORD_OFFSET, sizeof(mockAddr), &mockAddr, sizeof(mockAddr));
        }
#endif
    }

    static void FlushCache(uint8_t *addr, size_t len)
    {
#if defined(__aarch64__) || defined(__arm__) || defined(__thumb__)
        __builtin___clear_cache(reinterpret_cast<char *>(addr), reinterpret_cast<char *>(addr + len));
#else
        (void)addr;
        (void)len;
#endif
    }

    void RestoreCode(uint8_t *target, const uint8_t *original, size_t size)
    {
        MakeWritable(target, size);
        memcpy_s(target, size, original, size);
        FlushCache(target, size);
    }
};

// Extract the raw address of a non-virtual member function.
// On GCC/Clang, a PMF for non-virtual functions is just a plain function pointer,
// but reinterpret_cast is technically UB — use memcpy_s for portability.
template <typename MemberFuncPtr>
inline uintptr_t StubToAddr(MemberFuncPtr pmf)
{
    uintptr_t addr = 0;
    memcpy_s(&addr, sizeof(addr), &pmf, sizeof(addr));
    return addr;
}

#define ADDR(pmf) StubToAddr((pmf))

#endif // STUB_H
