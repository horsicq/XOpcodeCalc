/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef CPUFEATURES_H
#define CPUFEATURES_H

#include "asm_defs.h"

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#define CPUFEATURES_HOST_X86
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
#include <cpuid.h>
#define CPUFEATURES_HOST_X86
#endif

// Runtime CPU feature detection. Feature-gated opcodes are only offered when
// the host CPU implements them; on non-x86 builds the C backend implements
// everything, so every feature reports as available.
namespace CpuFeatures {

#ifdef CPUFEATURES_HOST_X86

namespace Detail {

inline void cpuidCall(unsigned int nLeaf, unsigned int nSubleaf, unsigned int anOut[4])
{
#if defined(_MSC_VER)
    int aInfo[4] = {0, 0, 0, 0};
    __cpuidex(aInfo, static_cast<int>(nLeaf), static_cast<int>(nSubleaf));
    anOut[0] = static_cast<unsigned int>(aInfo[0]);
    anOut[1] = static_cast<unsigned int>(aInfo[1]);
    anOut[2] = static_cast<unsigned int>(aInfo[2]);
    anOut[3] = static_cast<unsigned int>(aInfo[3]);
#else
    anOut[0] = anOut[1] = anOut[2] = anOut[3] = 0;
    __cpuid_count(nLeaf, nSubleaf, anOut[0], anOut[1], anOut[2], anOut[3]);
#endif
}

struct Flags {
    bool bPopcnt;
    bool bLzcnt;
    bool bBmi1;
    bool bBmi2;
    bool bSse42;
    bool bAdx;
};

inline const Flags &flags()
{
    static bool bInitialized = false;
    static Flags result = {};

    if (!bInitialized) {
        unsigned int anInfo[4];

        cpuidCall(0, 0, anInfo);
        const unsigned int nMaxLeaf = anInfo[0];

        cpuidCall(1, 0, anInfo);
        result.bSse42 = (anInfo[2] >> 20) & 1;   // ECX.SSE4_2
        result.bPopcnt = (anInfo[2] >> 23) & 1;  // ECX.POPCNT

        if (nMaxLeaf >= 7) {
            cpuidCall(7, 0, anInfo);
            result.bBmi1 = (anInfo[1] >> 3) & 1;  // EBX.BMI1
            result.bBmi2 = (anInfo[1] >> 8) & 1;  // EBX.BMI2
            result.bAdx = (anInfo[1] >> 19) & 1;  // EBX.ADX
        }

        cpuidCall(0x80000000, 0, anInfo);
        if (anInfo[0] >= 0x80000001) {
            cpuidCall(0x80000001, 0, anInfo);
            result.bLzcnt = (anInfo[2] >> 5) & 1;  // ECX.LZCNT/ABM
        }

        bInitialized = true;
    }

    return result;
}

}  // namespace Detail

inline bool isSupported(ASM_DEF::FEATURE feature)
{
    const Detail::Flags &flags = Detail::flags();

    switch (feature) {
        case ASM_DEF::FEAT_BASE: return true;
        case ASM_DEF::FEAT_POPCNT: return flags.bPopcnt;
        case ASM_DEF::FEAT_LZCNT: return flags.bLzcnt;
        case ASM_DEF::FEAT_BMI1: return flags.bBmi1;
        case ASM_DEF::FEAT_BMI2: return flags.bBmi2;
        case ASM_DEF::FEAT_SSE42: return flags.bSse42;
        case ASM_DEF::FEAT_ADX: return flags.bAdx;
    }

    return false;
}

#else  // !CPUFEATURES_HOST_X86: the C backend implements every opcode

inline bool isSupported(ASM_DEF::FEATURE feature)
{
    (void)feature;
    return true;
}

#endif

}  // namespace CpuFeatures

#endif  // CPUFEATURES_H
