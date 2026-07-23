/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// Opcode backend test harness.
//
// Runs every opcode over directed edge values and random fuzz inputs through
// BOTH the assembly backend (op_*) and the portable C reference (ref_op_*),
// then compares results and the architecturally-defined flags. On x86 this
// pits the hand-written assembly against independent C; a wrong DIV/IDIV
// guard crashes the harness with a hardware #DE, which is itself the signal.
//
// Also cross-checks DivisionCheck::isDivisionDefined() against a full
// double-width long-division oracle.

#include <cstdio>
#include <cstring>
#include <vector>

#include "../asm/asm.h"
#include "../asm/asm_c.h"
#include "../cpufeatures.h"
#include "../divisioncheck.h"

namespace {

constexpr XVALUE FLAG_CF = 0x0001;
constexpr XVALUE FLAG_PF = 0x0004;
constexpr XVALUE FLAG_AF = 0x0010;
constexpr XVALUE FLAG_ZF = 0x0040;
constexpr XVALUE FLAG_SF = 0x0080;
constexpr XVALUE FLAG_OF = 0x0800;
constexpr XVALUE FLAG_ALL6 = FLAG_CF | FLAG_PF | FLAG_AF | FLAG_ZF | FLAG_SF | FLAG_OF;

constexpr qint32 BITS = static_cast<qint32>(sizeof(XVALUE) * 8);

typedef void (*op_func)(RECDATA *pRecData);

enum CATEGORY {
    CAT_DEFAULT = 0,  // compare all results + the fixed flag mask
    CAT_SHIFT,        // count in C; count==0 -> all flags, else mask (+OF when count==1)
    CAT_SHIFTD,       // count in D low byte; same rule
    CAT_BITSCAN,      // compare ZF; skip RESULT[0] when the source is zero
    CAT_DIVISION,     // run only when isDivisionDefined(); no flag comparison
    CAT_RDTSC         // compare only preserved C/B results
};

struct OP_SPEC {
    const char *pszName;
    op_func asmFunc;
    op_func refFunc;
    XVALUE nFlagMask;  // base mask of flags to compare
    CATEGORY category;
    bool bSigned;                // CAT_DIVISION only
    ASM_DEF::FEATURE feature;    // skipped when the CPU lacks the feature
};

const XVALUE MASK_LOGIC = FLAG_CF | FLAG_OF | FLAG_SF | FLAG_ZF | FLAG_PF;              // AF undefined
const XVALUE MASK_SHIFT = FLAG_CF | FLAG_SF | FLAG_ZF | FLAG_PF;                        // AF undefined, OF only for count==1
const XVALUE MASK_ROTATE = FLAG_CF | FLAG_SF | FLAG_ZF | FLAG_PF | FLAG_AF;             // SF/ZF/PF/AF preserved, OF only for count==1

const OP_SPEC g_specs[] = {
    {"add", op_add, ref_op_add, FLAG_ALL6, CAT_DEFAULT, false},
    {"sub", op_sub, ref_op_sub, FLAG_ALL6, CAT_DEFAULT, false},
    {"adc", op_adc, ref_op_adc, FLAG_ALL6, CAT_DEFAULT, false},
    {"sbb", op_sbb, ref_op_sbb, FLAG_ALL6, CAT_DEFAULT, false},
    {"and", op_and, ref_op_and, MASK_LOGIC, CAT_DEFAULT, false},
    {"or", op_or, ref_op_or, MASK_LOGIC, CAT_DEFAULT, false},
    {"xor", op_xor, ref_op_xor, MASK_LOGIC, CAT_DEFAULT, false},
    {"test", op_test, ref_op_test, MASK_LOGIC, CAT_DEFAULT, false},
    {"cmp", op_cmp, ref_op_cmp, FLAG_ALL6, CAT_DEFAULT, false},
    {"xadd", op_xadd, ref_op_xadd, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmpxchg", op_cmpxchg, ref_op_cmpxchg, FLAG_ALL6, CAT_DEFAULT, false},
    {"mov", op_mov, ref_op_mov, FLAG_ALL6, CAT_DEFAULT, false},
    {"xchg", op_xchg, ref_op_xchg, FLAG_ALL6, CAT_DEFAULT, false},
    {"movzx_r8", op_movzx_r8, ref_op_movzx_r8, FLAG_ALL6, CAT_DEFAULT, false},
    {"movzx_r16", op_movzx_r16, ref_op_movzx_r16, FLAG_ALL6, CAT_DEFAULT, false},
    {"movsx_r8", op_movsx_r8, ref_op_movsx_r8, FLAG_ALL6, CAT_DEFAULT, false},
    {"movsx_r16", op_movsx_r16, ref_op_movsx_r16, FLAG_ALL6, CAT_DEFAULT, false},
    {"inc", op_inc, ref_op_inc, FLAG_ALL6, CAT_DEFAULT, false},
    {"dec", op_dec, ref_op_dec, FLAG_ALL6, CAT_DEFAULT, false},
    {"not", op_not, ref_op_not, FLAG_ALL6, CAT_DEFAULT, false},
    {"neg", op_neg, ref_op_neg, FLAG_ALL6, CAT_DEFAULT, false},
    {"bswap", op_bswap, ref_op_bswap, FLAG_ALL6, CAT_DEFAULT, false},
    {"mul", op_mul, ref_op_mul, FLAG_CF | FLAG_OF, CAT_DEFAULT, false},
    {"imul", op_imul, ref_op_imul, FLAG_CF | FLAG_OF, CAT_DEFAULT, false},
    {"div", op_div, ref_op_div, 0, CAT_DIVISION, false},
    {"idiv", op_idiv, ref_op_idiv, 0, CAT_DIVISION, true},
    {"shl", op_shl, ref_op_shl, MASK_SHIFT, CAT_SHIFT, false},
    {"shr", op_shr, ref_op_shr, MASK_SHIFT, CAT_SHIFT, false},
    {"sar", op_sar, ref_op_sar, MASK_SHIFT, CAT_SHIFT, false},
    {"rol", op_rol, ref_op_rol, MASK_ROTATE, CAT_SHIFT, false},
    {"ror", op_ror, ref_op_ror, MASK_ROTATE, CAT_SHIFT, false},
    {"rcl", op_rcl, ref_op_rcl, MASK_ROTATE, CAT_SHIFT, false},
    {"rcr", op_rcr, ref_op_rcr, MASK_ROTATE, CAT_SHIFT, false},
    {"shld", op_shld, ref_op_shld, MASK_SHIFT, CAT_SHIFTD, false},
    {"shrd", op_shrd, ref_op_shrd, MASK_SHIFT, CAT_SHIFTD, false},
    {"bt", op_bt, ref_op_bt, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false},
    {"bts", op_bts, ref_op_bts, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false},
    {"btr", op_btr, ref_op_btr, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false},
    {"btc", op_btc, ref_op_btc, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false},
    {"bsf", op_bsf, ref_op_bsf, FLAG_ZF, CAT_BITSCAN, false},
    {"bsr", op_bsr, ref_op_bsr, FLAG_ZF, CAT_BITSCAN, false},
    {"clc", op_clc, ref_op_clc, FLAG_ALL6, CAT_DEFAULT, false},
    {"stc", op_stc, ref_op_stc, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmc", op_cmc, ref_op_cmc, FLAG_ALL6, CAT_DEFAULT, false},
    {"lahf", op_lahf, ref_op_lahf, FLAG_ALL6, CAT_DEFAULT, false},
    {"sahf", op_sahf, ref_op_sahf, FLAG_ALL6, CAT_DEFAULT, false},
    {"setc", op_setc, ref_op_setc, FLAG_ALL6, CAT_DEFAULT, false},
    {"setnc", op_setnc, ref_op_setnc, FLAG_ALL6, CAT_DEFAULT, false},
    {"setz", op_setz, ref_op_setz, FLAG_ALL6, CAT_DEFAULT, false},
    {"setnz", op_setnz, ref_op_setnz, FLAG_ALL6, CAT_DEFAULT, false},
    {"sets", op_sets, ref_op_sets, FLAG_ALL6, CAT_DEFAULT, false},
    {"setns", op_setns, ref_op_setns, FLAG_ALL6, CAT_DEFAULT, false},
    {"seto", op_seto, ref_op_seto, FLAG_ALL6, CAT_DEFAULT, false},
    {"setno", op_setno, ref_op_setno, FLAG_ALL6, CAT_DEFAULT, false},
    {"setp", op_setp, ref_op_setp, FLAG_ALL6, CAT_DEFAULT, false},
    {"setnp", op_setnp, ref_op_setnp, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovc", op_cmovc, ref_op_cmovc, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovnc", op_cmovnc, ref_op_cmovnc, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovz", op_cmovz, ref_op_cmovz, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovnz", op_cmovnz, ref_op_cmovnz, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovs", op_cmovs, ref_op_cmovs, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovns", op_cmovns, ref_op_cmovns, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovo", op_cmovo, ref_op_cmovo, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovno", op_cmovno, ref_op_cmovno, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovp", op_cmovp, ref_op_cmovp, FLAG_ALL6, CAT_DEFAULT, false},
    {"cmovnp", op_cmovnp, ref_op_cmovnp, FLAG_ALL6, CAT_DEFAULT, false},
    {"cpuid", op_cpuid, ref_op_cpuid, FLAG_ALL6, CAT_DEFAULT, false},
    {"rdtsc", op_rdtsc, ref_op_rdtsc, FLAG_ALL6, CAT_RDTSC, false},
    {"popcnt", op_popcnt, ref_op_popcnt, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_POPCNT},
    {"lzcnt", op_lzcnt, ref_op_lzcnt, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false, ASM_DEF::FEAT_LZCNT},
    {"tzcnt", op_tzcnt, ref_op_tzcnt, FLAG_CF | FLAG_ZF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"andn", op_andn, ref_op_andn, FLAG_SF | FLAG_ZF | FLAG_CF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"bextr", op_bextr, ref_op_bextr, FLAG_ZF | FLAG_CF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"blsi", op_blsi, ref_op_blsi, FLAG_CF | FLAG_ZF | FLAG_SF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"blsmsk", op_blsmsk, ref_op_blsmsk, FLAG_CF | FLAG_ZF | FLAG_SF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"blsr", op_blsr, ref_op_blsr, FLAG_CF | FLAG_ZF | FLAG_SF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI1},
    {"bzhi", op_bzhi, ref_op_bzhi, FLAG_CF | FLAG_ZF | FLAG_SF | FLAG_OF, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"pdep", op_pdep, ref_op_pdep, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"pext", op_pext, ref_op_pext, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"shlx", op_shlx, ref_op_shlx, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"shrx", op_shrx, ref_op_shrx, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"sarx", op_sarx, ref_op_sarx, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_BMI2},
    {"crc32", op_crc32, ref_op_crc32, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_SSE42},
    {"adcx", op_adcx, ref_op_adcx, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_ADX},
    {"adox", op_adox, ref_op_adox, FLAG_ALL6, CAT_DEFAULT, false, ASM_DEF::FEAT_ADX},
#ifdef OPCODE64
    {"movsxd", op_movsxd, ref_op_movsxd, FLAG_ALL6, CAT_DEFAULT, false},
    {"cdqe", op_cdqe, ref_op_cdqe, FLAG_ALL6, CAT_DEFAULT, false},
    {"cqo", op_cqo, ref_op_cqo, FLAG_ALL6, CAT_DEFAULT, false},
#else
    {"cbw", op_cbw, ref_op_cbw, FLAG_ALL6, CAT_DEFAULT, false},
    {"cwde", op_cwde, ref_op_cwde, FLAG_ALL6, CAT_DEFAULT, false},
    {"cwd", op_cwd, ref_op_cwd, FLAG_ALL6, CAT_DEFAULT, false},
    {"cdq", op_cdq, ref_op_cdq, FLAG_ALL6, CAT_DEFAULT, false},
    {"daa", op_daa, ref_op_daa, FLAG_CF | FLAG_AF | FLAG_SF | FLAG_ZF | FLAG_PF, CAT_DEFAULT, false},
    {"das", op_das, ref_op_das, FLAG_CF | FLAG_AF | FLAG_SF | FLAG_ZF | FLAG_PF, CAT_DEFAULT, false},
    {"aaa", op_aaa, ref_op_aaa, FLAG_CF | FLAG_AF, CAT_DEFAULT, false},
    {"aas", op_aas, ref_op_aas, FLAG_CF | FLAG_AF, CAT_DEFAULT, false},
    {"aam", op_aam, ref_op_aam, FLAG_SF | FLAG_ZF | FLAG_PF, CAT_DEFAULT, false},
    {"aad", op_aad, ref_op_aad, FLAG_SF | FLAG_ZF | FLAG_PF, CAT_DEFAULT, false},
#endif
};

quint64 g_nRandomState = 0x9E3779B97F4A7C15ull;

quint64 random64()
{
    g_nRandomState ^= g_nRandomState << 13;
    g_nRandomState ^= g_nRandomState >> 7;
    g_nRandomState ^= g_nRandomState << 17;
    return g_nRandomState;
}

std::vector<XVALUE> interestingValues()
{
    const XVALUE nMSB = XVALUE(1) << (BITS - 1);
    std::vector<XVALUE> values = {0, 1, 2, 3, 7, 9, 0x0F, 0x10, 0x63, 0x7F, 0x80, 0x81, 0x99, 0xFF, 0x100, 0x7FFF, 0x8000, 0xFFFF,
                                  XVALUE(nMSB - 1), nMSB, XVALUE(nMSB + 1), XVALUE(~XVALUE(0) - 1), ~XVALUE(0)};
#ifdef OPCODE64
    values.push_back(0x7FFFFFFFull);
    values.push_back(0x80000000ull);
    values.push_back(0xFFFFFFFFull);
    values.push_back(0x100000000ull);
#endif
    return values;
}

XVALUE flagMaskFor(const OP_SPEC &spec, const RECDATA &input)
{
    if (spec.category == CAT_SHIFT || spec.category == CAT_SHIFTD) {
        const XVALUE nRaw = (spec.category == CAT_SHIFT) ? input.OPERAND[1] : (input.OPERAND[2] & 0xFF);
        const qint32 nCount = static_cast<qint32>(nRaw & (BITS - 1));
        if (nCount == 0) {
            return FLAG_ALL6;  // count 0 leaves every flag untouched
        }
        return (nCount == 1) ? (spec.nFlagMask | FLAG_OF) : spec.nFlagMask;
    }

    return spec.nFlagMask;
}

// Independent oracle for the #DE predicate: full double-width long division.
bool oracleDivisionDefined(bool bSigned, XVALUE nLow, XVALUE nDivisor, XVALUE nHigh)
{
    if (nDivisor == 0) {
        return false;
    }

    const XVALUE nMSB = XVALUE(1) << (BITS - 1);

    XVALUE mLow = nLow, mHigh = nHigh, mDivisor = nDivisor;
    bool bNegativeQuotient = false;
    bool bNegativeDividend = false;

    if (bSigned) {
        bNegativeDividend = (nHigh & nMSB) != 0;
        const bool bNegativeDivisor = (nDivisor & nMSB) != 0;
        bNegativeQuotient = (bNegativeDividend != bNegativeDivisor);
        if (bNegativeDividend) {
            mLow = XVALUE(~nLow + 1);
            mHigh = XVALUE(~nHigh + ((nLow == 0) ? 1 : 0));
        }
        if (bNegativeDivisor) {
            mDivisor = XVALUE(~nDivisor + 1);
        }
    }

    // Bit-by-bit restoring division of the full 2W-bit magnitude.
    XVALUE nQuotientHigh = 0, nQuotientLow = 0, nRemainder = 0;
    for (qint32 i = 2 * BITS - 1; i >= 0; --i) {
        const bool bCarry = nRemainder >> (BITS - 1);
        const XVALUE nBit = (i >= BITS) ? ((mHigh >> (i - BITS)) & 1) : ((mLow >> i) & 1);
        nRemainder = (nRemainder << 1) | nBit;
        if (bCarry || (nRemainder >= mDivisor)) {
            nRemainder -= mDivisor;
            if (i >= BITS) {
                nQuotientHigh |= XVALUE(1) << (i - BITS);
            } else {
                nQuotientLow |= XVALUE(1) << i;
            }
        }
    }

    if (nQuotientHigh != 0) {
        return false;
    }
    if (!bSigned) {
        return true;
    }
    return bNegativeQuotient ? (nQuotientLow <= nMSB) : (nQuotientLow < nMSB);
}

qint64 g_nFailures = 0;
qint64 g_nChecks = 0;

void runCase(const OP_SPEC &spec, XVALUE a, XVALUE c, XVALUE d, XVALUE b, XVALUE nFlags)
{
    RECDATA input;
    std::memset(&input, 0, sizeof(input));
    input.OPERAND[0] = a;
    input.OPERAND[1] = c;
    input.OPERAND[2] = d;
    input.OPERAND[3] = b;
    input.FLAG[0] = nFlags & FLAG_ALL6;

    if (spec.category == CAT_DIVISION) {
        // First cross-check the #DE predicate against the independent oracle.
        const bool bDefined = DivisionCheck::isDivisionDefined(spec.bSigned, a, c, d);
        const bool bOracle = oracleDivisionDefined(spec.bSigned, a, c, d);
        ++g_nChecks;
        if (bDefined != bOracle) {
            std::printf("FAIL %s: isDivisionDefined=%d oracle=%d (a=%llx c=%llx d=%llx)\n", spec.pszName, bDefined, bOracle, (unsigned long long)a,
                        (unsigned long long)c, (unsigned long long)d);
            ++g_nFailures;
            return;
        }
        if (!bDefined) {
            return;  // executing would raise #DE
        }
    }

    RECDATA asmData = input;
    RECDATA refData = input;
    spec.asmFunc(&asmData);   // a wrong division guard crashes right here
    spec.refFunc(&refData);
    ++g_nChecks;

    bool bCompareResult[4] = {true, true, true, true};
    if (spec.category == CAT_BITSCAN && c == 0) {
        bCompareResult[0] = false;  // destination architecturally undefined
    }
    if (spec.category == CAT_RDTSC) {
        bCompareResult[0] = false;  // time stamp differs between the two reads
        bCompareResult[1] = false;
    }

    for (qint32 i = 0; i < 4; ++i) {
        if (bCompareResult[i] && (asmData.RESULT[i] != refData.RESULT[i])) {
            std::printf("FAIL %s: RESULT[%d] asm=%llx ref=%llx (a=%llx c=%llx d=%llx b=%llx fl=%llx)\n", spec.pszName, i, (unsigned long long)asmData.RESULT[i],
                        (unsigned long long)refData.RESULT[i], (unsigned long long)a, (unsigned long long)c, (unsigned long long)d, (unsigned long long)b,
                        (unsigned long long)nFlags);
            ++g_nFailures;
            return;
        }
    }

    const XVALUE nMask = flagMaskFor(spec, input);
    if ((asmData.FLAG[1] ^ refData.FLAG[1]) & nMask) {
        std::printf("FAIL %s: FLAGS asm=%llx ref=%llx mask=%llx (a=%llx c=%llx d=%llx b=%llx fl=%llx)\n", spec.pszName, (unsigned long long)asmData.FLAG[1],
                    (unsigned long long)refData.FLAG[1], (unsigned long long)nMask, (unsigned long long)a, (unsigned long long)c, (unsigned long long)d,
                    (unsigned long long)b, (unsigned long long)nFlags);
        ++g_nFailures;
    }
}

}  // namespace

int main()
{
    const std::vector<XVALUE> values = interestingValues();
    const XVALUE aFlagSets[] = {0, FLAG_CF, FLAG_AF, FLAG_CF | FLAG_ZF, FLAG_ALL6};
    const XVALUE aThirdOperands[] = {0, 1, 2, XVALUE(1) << (BITS - 1), ~XVALUE(0)};

    qint32 nSkipped = 0;

    for (const OP_SPEC &spec : g_specs) {
        if (!CpuFeatures::isSupported(spec.feature)) {
            std::printf("SKIP %s: CPU feature not available\n", spec.pszName);
            ++nSkipped;
            continue;
        }

        // Directed edge-value grid.
        for (XVALUE a : values) {
            for (XVALUE c : values) {
                for (XVALUE d : aThirdOperands) {
                    for (XVALUE nFlags : aFlagSets) {
                        runCase(spec, a, c, d, XVALUE(0x0123456789ABCDEFull), nFlags);
                    }
                }
            }
        }

        // Random fuzz.
        for (qint32 i = 0; i < 20000; ++i) {
            runCase(spec, XVALUE(random64()), XVALUE(random64()), XVALUE(random64()), XVALUE(random64()), XVALUE(random64()) & FLAG_ALL6);
        }
    }

    std::printf("%s: %lld checks, %lld failures (%d-bit build, %u opcodes, %d skipped)\n", g_nFailures ? "FAILED" : "OK", (long long)g_nChecks,
                (long long)g_nFailures, BITS, (unsigned)(sizeof(g_specs) / sizeof(g_specs[0])), nSkipped);

    return g_nFailures ? 1 : 0;
}
