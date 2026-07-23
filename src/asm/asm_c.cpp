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
#include "asm_c.h"

#include "../divisioncheck.h"

#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
#include <intrin.h>
#define XOPCODECALC_HOST_X86
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
#include <cpuid.h>
#define XOPCODECALC_HOST_X86
#else
#include <chrono>
#endif

// Undefined-flag policy (architecturally undefined bits get a fixed value so
// the backend is deterministic; tests mask them when comparing with hardware):
//   logic ops             : AF = 0
//   MUL/IMUL              : SF/ZF/PF from the low result, AF = 0
//   DIV/IDIV              : all six status flags left unchanged
//   shifts (count > 1)    : OF from the count == 1 rule
//   BT/BTS/BTR/BTC        : OF/SF/AF/PF left unchanged
//   BSF/BSR               : CF/OF/SF/AF/PF left unchanged
//   AAM/AAD/DAA/DAS       : OF left unchanged; AAA/AAS: OF/SF/ZF/PF unchanged

namespace {

constexpr XVALUE FLAG_CF = 0x0001;
constexpr XVALUE FLAG_PF = 0x0004;
constexpr XVALUE FLAG_AF = 0x0010;
constexpr XVALUE FLAG_ZF = 0x0040;
constexpr XVALUE FLAG_SF = 0x0080;
constexpr XVALUE FLAG_OF = 0x0800;
constexpr XVALUE FLAG_STATUS_MASK = FLAG_CF | FLAG_PF | FLAG_AF | FLAG_ZF | FLAG_SF | FLAG_OF;
constexpr XVALUE FLAG_PUSHF_EXTRA = 0x202;  // reserved bit 1 (always set) + IF

constexpr qint32 BITS = static_cast<qint32>(sizeof(XVALUE) * 8);
constexpr XVALUE MSB = XVALUE(1) << (BITS - 1);

struct Regs {
    XVALUE A;  // EAX/RAX
    XVALUE C;  // ECX/RCX
    XVALUE D;  // EDX/RDX
    XVALUE B;  // EBX/RBX
    XVALUE F;  // status flags
};

Regs load(const RECDATA *pRecData)
{
    return {pRecData->OPERAND[0], pRecData->OPERAND[1], pRecData->OPERAND[2], pRecData->OPERAND[3], pRecData->FLAG[0] & FLAG_STATUS_MASK};
}

void store(RECDATA *pRecData, const Regs &r)
{
    pRecData->RESULT[0] = r.A;
    pRecData->RESULT[1] = r.D;
    pRecData->RESULT[2] = r.C;
    pRecData->RESULT[3] = r.B;
    pRecData->FLAG[1] = (r.F & FLAG_STATUS_MASK) | FLAG_PUSHF_EXTRA;
}

void setFlag(XVALUE &nFlags, XVALUE nBit, bool bValue)
{
    nFlags = bValue ? (nFlags | nBit) : (nFlags & ~nBit);
}

bool parityEven(XVALUE nValue)
{
    XVALUE v = nValue & 0xFF;
    v ^= v >> 4;
    v ^= v >> 2;
    v ^= v >> 1;
    return !(v & 1);
}

void setSZP(XVALUE &nFlags, XVALUE nResult)
{
    setFlag(nFlags, FLAG_SF, nResult & MSB);
    setFlag(nFlags, FLAG_ZF, nResult == 0);
    setFlag(nFlags, FLAG_PF, parityEven(nResult));
}

// SZP of an 8-bit result (BCD/partial-width ops)
void setSZP8(XVALUE &nFlags, XVALUE nResult8)
{
    setFlag(nFlags, FLAG_SF, nResult8 & 0x80);
    setFlag(nFlags, FLAG_ZF, (nResult8 & 0xFF) == 0);
    setFlag(nFlags, FLAG_PF, parityEven(nResult8));
}

XVALUE doAdd(XVALUE &nFlags, XVALUE x, XVALUE y, bool bCarryIn)
{
    const XVALUE t = x + y;
    const XVALUE res = t + (bCarryIn ? 1 : 0);
    setFlag(nFlags, FLAG_CF, (t < x) || (res < t));
    setFlag(nFlags, FLAG_AF, ((x ^ y ^ res) >> 4) & 1);
    setFlag(nFlags, FLAG_OF, ((~(x ^ y)) & (x ^ res)) & MSB);
    setSZP(nFlags, res);
    return res;
}

XVALUE doSub(XVALUE &nFlags, XVALUE x, XVALUE y, bool bBorrowIn)
{
    const XVALUE res = x - y - (bBorrowIn ? 1 : 0);
    setFlag(nFlags, FLAG_CF, (x < y) || (bBorrowIn && (x == y)));
    setFlag(nFlags, FLAG_AF, ((x ^ y ^ res) >> 4) & 1);
    setFlag(nFlags, FLAG_OF, ((x ^ y) & (x ^ res)) & MSB);
    setSZP(nFlags, res);
    return res;
}

XVALUE doLogicFlags(XVALUE &nFlags, XVALUE nResult)
{
    setFlag(nFlags, FLAG_CF, false);
    setFlag(nFlags, FLAG_OF, false);
    setFlag(nFlags, FLAG_AF, false);
    setSZP(nFlags, nResult);
    return nResult;
}

XVALUE mulHighUnsigned(XVALUE a, XVALUE b)
{
    const qint32 H = BITS / 2;
    const XVALUE MASKH = (XVALUE(1) << H) - 1;
    const XVALUE al = a & MASKH, ah = a >> H;
    const XVALUE bl = b & MASKH, bh = b >> H;
    const XVALUE t0 = al * bl;
    const XVALUE t1 = ah * bl + (t0 >> H);
    const XVALUE t2 = al * bh + (t1 & MASKH);
    return ah * bh + (t1 >> H) + (t2 >> H);
}

XVALUE mulHighSigned(XVALUE a, XVALUE b)
{
    XVALUE nHigh = mulHighUnsigned(a, b);
    if (a & MSB) nHigh -= b;
    if (b & MSB) nHigh -= a;
    return nHigh;
}

XVALUE signExtend(XVALUE nValue, qint32 nFromBits)
{
    const XVALUE nMask = (nFromBits < BITS) ? ((XVALUE(1) << nFromBits) - 1) : ~XVALUE(0);
    const XVALUE nSign = XVALUE(1) << (nFromBits - 1);
    XVALUE v = nValue & nMask;
    if (v & nSign) {
        v |= ~nMask;
    }
    return v;
}

}  // namespace

// --- two-operand arithmetic / logic -------------------------------------------------

void ref_op_add(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doAdd(r.F, r.A, r.C, false);
    store(pRecData, r);
}

void ref_op_adc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doAdd(r.F, r.A, r.C, r.F & FLAG_CF);
    store(pRecData, r);
}

void ref_op_sub(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doSub(r.F, r.A, r.C, false);
    store(pRecData, r);
}

void ref_op_sbb(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doSub(r.F, r.A, r.C, r.F & FLAG_CF);
    store(pRecData, r);
}

void ref_op_cmp(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    doSub(r.F, r.A, r.C, false);
    store(pRecData, r);
}

void ref_op_and(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doLogicFlags(r.F, r.A & r.C);
    store(pRecData, r);
}

void ref_op_or(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doLogicFlags(r.F, r.A | r.C);
    store(pRecData, r);
}

void ref_op_xor(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doLogicFlags(r.F, r.A ^ r.C);
    store(pRecData, r);
}

void ref_op_test(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    doLogicFlags(r.F, r.A & r.C);
    store(pRecData, r);
}

void ref_op_xadd(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nOldA = r.A;
    r.A = doAdd(r.F, r.A, r.C, false);
    r.C = nOldA;
    store(pRecData, r);
}

void ref_op_cmpxchg(RECDATA *pRecData)
{
    // cmpxchg rcx,rdx: compare A with C; equal -> C = D, else A = C. Flags of CMP A,C.
    Regs r = load(pRecData);
    doSub(r.F, r.A, r.C, false);
    if (r.F & FLAG_ZF) {
        r.C = r.D;
    } else {
        r.A = r.C;
    }
    store(pRecData, r);
}

// --- one-operand arithmetic ---------------------------------------------------------

void ref_op_inc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const bool bCF = r.F & FLAG_CF;
    r.A = doAdd(r.F, r.A, 1, false);
    setFlag(r.F, FLAG_CF, bCF);  // INC preserves CF
    store(pRecData, r);
}

void ref_op_dec(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const bool bCF = r.F & FLAG_CF;
    r.A = doSub(r.F, r.A, 1, false);
    setFlag(r.F, FLAG_CF, bCF);  // DEC preserves CF
    store(pRecData, r);
}

void ref_op_not(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = ~r.A;
    store(pRecData, r);
}

void ref_op_neg(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = doSub(r.F, 0, r.A, false);
    store(pRecData, r);
}

void ref_op_bswap(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nResult = 0;
    for (qint32 i = 0; i < BITS / 8; ++i) {
        nResult = (nResult << 8) | ((r.A >> (i * 8)) & 0xFF);
    }
    r.A = nResult;
    store(pRecData, r);
}

// --- multiply / divide --------------------------------------------------------------

void ref_op_mul(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nHigh = mulHighUnsigned(r.A, r.C);
    const XVALUE nLow = r.A * r.C;
    const bool bOverflow = (nHigh != 0);
    setFlag(r.F, FLAG_CF, bOverflow);
    setFlag(r.F, FLAG_OF, bOverflow);
    setFlag(r.F, FLAG_AF, false);
    setSZP(r.F, nLow);
    r.A = nLow;
    r.D = nHigh;
    store(pRecData, r);
}

void ref_op_imul(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nHigh = mulHighSigned(r.A, r.C);
    const XVALUE nLow = r.A * r.C;
    const bool bOverflow = (nHigh != ((nLow & MSB) ? ~XVALUE(0) : XVALUE(0)));
    setFlag(r.F, FLAG_CF, bOverflow);
    setFlag(r.F, FLAG_OF, bOverflow);
    setFlag(r.F, FLAG_AF, false);
    setSZP(r.F, nLow);
    r.A = nLow;
    r.D = nHigh;
    store(pRecData, r);
}

void ref_op_div(RECDATA *pRecData)
{
    // Precondition: DivisionCheck::isDivisionDefined(false, A, C, D). Flags stay unchanged.
    Regs r = load(pRecData);
    XVALUE nRemainder = 0;
    const XVALUE nQuotient = DivisionCheck::divideDoubleWidth(r.D, r.A, r.C, &nRemainder);
    r.A = nQuotient;
    r.D = nRemainder;
    store(pRecData, r);
}

void ref_op_idiv(RECDATA *pRecData)
{
    // Precondition: DivisionCheck::isDivisionDefined(true, A, C, D). Flags stay unchanged.
    Regs r = load(pRecData);

    const bool bNegativeDividend = r.D & MSB;
    const bool bNegativeDivisor = r.C & MSB;

    XVALUE nLow = r.A;
    XVALUE nHigh = r.D;
    if (bNegativeDividend) {
        nLow = XVALUE(~r.A + 1);
        nHigh = XVALUE(~r.D + ((r.A == 0) ? 1 : 0));
    }
    const XVALUE nDivisor = bNegativeDivisor ? XVALUE(~r.C + 1) : r.C;

    XVALUE nRemainder = 0;
    XVALUE nQuotient = DivisionCheck::divideDoubleWidth(nHigh, nLow, nDivisor, &nRemainder);

    if (bNegativeDividend != bNegativeDivisor) {
        nQuotient = XVALUE(~nQuotient + 1);
    }
    if (bNegativeDividend) {
        nRemainder = XVALUE(~nRemainder + 1);  // the remainder takes the dividend's sign
    }

    r.A = nQuotient;
    r.D = nRemainder;
    store(pRecData, r);
}

// --- shifts / rotates ---------------------------------------------------------------

void ref_op_shl(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = r.A << nCount;
        setFlag(r.F, FLAG_CF, (r.A >> (BITS - nCount)) & 1);
        setFlag(r.F, FLAG_OF, ((nResult & MSB) != 0) != (((r.A >> (BITS - nCount)) & 1) != 0));  // count == 1 rule
        setSZP(r.F, nResult);
        r.A = nResult;
    }
    store(pRecData, r);
}

void ref_op_shr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = r.A >> nCount;
        setFlag(r.F, FLAG_CF, (r.A >> (nCount - 1)) & 1);
        setFlag(r.F, FLAG_OF, r.A & MSB);  // count == 1 rule
        setSZP(r.F, nResult);
        r.A = nResult;
    }
    store(pRecData, r);
}

void ref_op_sar(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        XVALUE nResult = r.A >> nCount;
        if (r.A & MSB) {
            nResult |= ~XVALUE(0) << (BITS - nCount);
        }
        setFlag(r.F, FLAG_CF, (r.A >> (nCount - 1)) & 1);
        setFlag(r.F, FLAG_OF, false);
        setSZP(r.F, nResult);
        r.A = nResult;
    }
    store(pRecData, r);
}

void ref_op_rol(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = (r.A << nCount) | (r.A >> (BITS - nCount));
        setFlag(r.F, FLAG_CF, nResult & 1);
        setFlag(r.F, FLAG_OF, ((nResult & MSB) != 0) != ((nResult & 1) != 0));  // count == 1 rule
        r.A = nResult;  // SF/ZF/AF/PF are not affected by rotates
    }
    store(pRecData, r);
}

void ref_op_ror(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = (r.A >> nCount) | (r.A << (BITS - nCount));
        setFlag(r.F, FLAG_CF, nResult & MSB);
        setFlag(r.F, FLAG_OF, ((nResult & MSB) != 0) != ((nResult & (MSB >> 1)) != 0));  // count == 1 rule
        r.A = nResult;
    }
    store(pRecData, r);
}

void ref_op_rcl(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        for (qint32 i = 0; i < nCount; ++i) {
            const bool bNewCarry = r.A & MSB;
            r.A = (r.A << 1) | ((r.F & FLAG_CF) ? 1 : 0);
            setFlag(r.F, FLAG_CF, bNewCarry);
        }
        setFlag(r.F, FLAG_OF, ((r.A & MSB) != 0) != ((r.F & FLAG_CF) != 0));  // count == 1 rule
    }
    store(pRecData, r);
}

void ref_op_rcr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.C & (BITS - 1));
    if (nCount) {
        for (qint32 i = 0; i < nCount; ++i) {
            const bool bNewCarry = r.A & 1;
            r.A = (r.A >> 1) | ((r.F & FLAG_CF) ? MSB : 0);
            setFlag(r.F, FLAG_CF, bNewCarry);
        }
        setFlag(r.F, FLAG_OF, ((r.A & MSB) != 0) != ((r.A & (MSB >> 1)) != 0));  // count == 1 rule
    }
    store(pRecData, r);
}

void ref_op_shld(RECDATA *pRecData)
{
    // asm: rbx = rcx; cl = dl; shld rax,rbx,cl (clobbers modeled identically)
    Regs r = load(pRecData);
    const XVALUE nSource = r.C;
    const XVALUE nCount8 = r.D & 0xFF;
    r.B = nSource;
    r.C = (r.C & ~XVALUE(0xFF)) | nCount8;
    const qint32 nCount = static_cast<qint32>(nCount8 & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = (r.A << nCount) | (nSource >> (BITS - nCount));
        setFlag(r.F, FLAG_CF, (r.A >> (BITS - nCount)) & 1);
        setFlag(r.F, FLAG_OF, ((nResult ^ r.A) & MSB) != 0);  // count == 1 rule: sign change
        setSZP(r.F, nResult);
        r.A = nResult;
    }
    store(pRecData, r);
}

void ref_op_shrd(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nSource = r.C;
    const XVALUE nCount8 = r.D & 0xFF;
    r.B = nSource;
    r.C = (r.C & ~XVALUE(0xFF)) | nCount8;
    const qint32 nCount = static_cast<qint32>(nCount8 & (BITS - 1));
    if (nCount) {
        const XVALUE nResult = (r.A >> nCount) | (nSource << (BITS - nCount));
        setFlag(r.F, FLAG_CF, (r.A >> (nCount - 1)) & 1);
        setFlag(r.F, FLAG_OF, ((nResult ^ r.A) & MSB) != 0);  // count == 1 rule: sign change
        setSZP(r.F, nResult);
        r.A = nResult;
    }
    store(pRecData, r);
}

// --- bit operations -----------------------------------------------------------------

void ref_op_bt(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    setFlag(r.F, FLAG_CF, (r.A >> (r.C & (BITS - 1))) & 1);
    store(pRecData, r);
}

void ref_op_bts(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nBit = XVALUE(1) << (r.C & (BITS - 1));
    setFlag(r.F, FLAG_CF, r.A & nBit);
    r.A |= nBit;
    store(pRecData, r);
}

void ref_op_btr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nBit = XVALUE(1) << (r.C & (BITS - 1));
    setFlag(r.F, FLAG_CF, r.A & nBit);
    r.A &= ~nBit;
    store(pRecData, r);
}

void ref_op_btc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nBit = XVALUE(1) << (r.C & (BITS - 1));
    setFlag(r.F, FLAG_CF, r.A & nBit);
    r.A ^= nBit;
    store(pRecData, r);
}

void ref_op_bsf(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    if (r.C == 0) {
        setFlag(r.F, FLAG_ZF, true);  // destination stays unchanged (matches real CPUs)
    } else {
        setFlag(r.F, FLAG_ZF, false);
        qint32 nIndex = 0;
        while (!((r.C >> nIndex) & 1)) {
            ++nIndex;
        }
        r.A = XVALUE(nIndex);
    }
    store(pRecData, r);
}

void ref_op_bsr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    if (r.C == 0) {
        setFlag(r.F, FLAG_ZF, true);  // destination stays unchanged (matches real CPUs)
    } else {
        setFlag(r.F, FLAG_ZF, false);
        qint32 nIndex = BITS - 1;
        while (!((r.C >> nIndex) & 1)) {
            --nIndex;
        }
        r.A = XVALUE(nIndex);
    }
    store(pRecData, r);
}

// --- moves --------------------------------------------------------------------------

void ref_op_mov(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = r.C;
    store(pRecData, r);
}

void ref_op_xchg(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nOldA = r.A;
    r.A = r.C;
    r.C = nOldA;
    store(pRecData, r);
}

void ref_op_movzx_r8(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = r.C & 0xFF;
    store(pRecData, r);
}

void ref_op_movzx_r16(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = r.C & 0xFFFF;
    store(pRecData, r);
}

void ref_op_movsx_r8(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = signExtend(r.C, 8);
    store(pRecData, r);
}

void ref_op_movsx_r16(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = signExtend(r.C, 16);
    store(pRecData, r);
}

// --- flag manipulation --------------------------------------------------------------

void ref_op_clc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    setFlag(r.F, FLAG_CF, false);
    store(pRecData, r);
}

void ref_op_stc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    setFlag(r.F, FLAG_CF, true);
    store(pRecData, r);
}

void ref_op_cmc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    setFlag(r.F, FLAG_CF, !(r.F & FLAG_CF));
    store(pRecData, r);
}

void ref_op_lahf(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nFlagByte = (r.F & (FLAG_SF | FLAG_ZF | FLAG_AF | FLAG_PF | FLAG_CF)) | 0x02;
    r.A = (r.A & ~XVALUE(0xFF00)) | (nFlagByte << 8);
    store(pRecData, r);
}

void ref_op_sahf(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nFlagByte = (r.A >> 8) & 0xFF;
    r.F = (r.F & ~XVALUE(FLAG_SF | FLAG_ZF | FLAG_AF | FLAG_PF | FLAG_CF)) | (nFlagByte & (FLAG_SF | FLAG_ZF | FLAG_AF | FLAG_PF | FLAG_CF));
    store(pRecData, r);
}

// --- SETcc / CMOVcc -----------------------------------------------------------------

#define XOPCODECALC_DEFINE_SETCC(name, condition)                 \
    void ref_op_##name(RECDATA *pRecData)                         \
    {                                                             \
        Regs r = load(pRecData);                                  \
        r.A = (r.A & ~XVALUE(0xFF)) | ((condition) ? 1 : 0);      \
        store(pRecData, r);                                       \
    }

XOPCODECALC_DEFINE_SETCC(setc, r.F & FLAG_CF)
XOPCODECALC_DEFINE_SETCC(setnc, !(r.F & FLAG_CF))
XOPCODECALC_DEFINE_SETCC(setz, r.F & FLAG_ZF)
XOPCODECALC_DEFINE_SETCC(setnz, !(r.F & FLAG_ZF))
XOPCODECALC_DEFINE_SETCC(sets, r.F & FLAG_SF)
XOPCODECALC_DEFINE_SETCC(setns, !(r.F & FLAG_SF))
XOPCODECALC_DEFINE_SETCC(seto, r.F & FLAG_OF)
XOPCODECALC_DEFINE_SETCC(setno, !(r.F & FLAG_OF))
XOPCODECALC_DEFINE_SETCC(setp, r.F & FLAG_PF)
XOPCODECALC_DEFINE_SETCC(setnp, !(r.F & FLAG_PF))
#undef XOPCODECALC_DEFINE_SETCC

#define XOPCODECALC_DEFINE_CMOVCC(name, condition)  \
    void ref_op_##name(RECDATA *pRecData)           \
    {                                               \
        Regs r = load(pRecData);                    \
        if (condition) {                            \
            r.A = r.C;                              \
        }                                           \
        store(pRecData, r);                         \
    }

XOPCODECALC_DEFINE_CMOVCC(cmovc, r.F & FLAG_CF)
XOPCODECALC_DEFINE_CMOVCC(cmovnc, !(r.F & FLAG_CF))
XOPCODECALC_DEFINE_CMOVCC(cmovz, r.F & FLAG_ZF)
XOPCODECALC_DEFINE_CMOVCC(cmovnz, !(r.F & FLAG_ZF))
XOPCODECALC_DEFINE_CMOVCC(cmovs, r.F & FLAG_SF)
XOPCODECALC_DEFINE_CMOVCC(cmovns, !(r.F & FLAG_SF))
XOPCODECALC_DEFINE_CMOVCC(cmovo, r.F & FLAG_OF)
XOPCODECALC_DEFINE_CMOVCC(cmovno, !(r.F & FLAG_OF))
XOPCODECALC_DEFINE_CMOVCC(cmovp, r.F & FLAG_PF)
XOPCODECALC_DEFINE_CMOVCC(cmovnp, !(r.F & FLAG_PF))
#undef XOPCODECALC_DEFINE_CMOVCC

// --- CPUID / RDTSC ------------------------------------------------------------------

void ref_op_cpuid(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    quint32 nEax = 0, nEbx = 0, nEcx = 0, nEdx = 0;
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    int aInfo[4] = {0, 0, 0, 0};
    __cpuidex(aInfo, static_cast<int>(r.A & 0xFFFFFFFF), static_cast<int>(r.C & 0xFFFFFFFF));
    nEax = static_cast<quint32>(aInfo[0]);
    nEbx = static_cast<quint32>(aInfo[1]);
    nEcx = static_cast<quint32>(aInfo[2]);
    nEdx = static_cast<quint32>(aInfo[3]);
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
    __cpuid_count(static_cast<quint32>(r.A & 0xFFFFFFFF), static_cast<quint32>(r.C & 0xFFFFFFFF), nEax, nEbx, nEcx, nEdx);
#endif
    // In 64-bit mode CPUID zero-extends all four registers.
    r.A = nEax;
    r.B = nEbx;
    r.C = nEcx;
    r.D = nEdx;
    store(pRecData, r);
}

void ref_op_rdtsc(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    quint64 nTsc = 0;
#if defined(_MSC_VER) && (defined(_M_IX86) || defined(_M_X64))
    nTsc = __rdtsc();
#elif (defined(__GNUC__) || defined(__clang__)) && (defined(__i386__) || defined(__x86_64__))
    nTsc = __builtin_ia32_rdtsc();
#else
    nTsc = static_cast<quint64>(std::chrono::steady_clock::now().time_since_epoch().count());
#endif
    r.A = static_cast<XVALUE>(nTsc & 0xFFFFFFFF);
    r.D = static_cast<XVALUE>((nTsc >> 32) & 0xFFFFFFFF);
    store(pRecData, r);
}

// --- width-specific opcodes ---------------------------------------------------------

#ifdef OPCODE64

void ref_op_movsxd(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = signExtend(r.C, 32);
    store(pRecData, r);
}

void ref_op_cdqe(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = signExtend(r.A, 32);
    store(pRecData, r);
}

void ref_op_cqo(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.D = (r.A & MSB) ? ~XVALUE(0) : 0;
    store(pRecData, r);
}

#else  // OPCODE32

void ref_op_cbw(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = (r.A & ~XVALUE(0xFFFF)) | (signExtend(r.A, 8) & 0xFFFF);
    store(pRecData, r);
}

void ref_op_cwde(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = signExtend(r.A, 16);
    store(pRecData, r);
}

void ref_op_cwd(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.D = (r.D & ~XVALUE(0xFFFF)) | ((r.A & 0x8000) ? 0xFFFF : 0);
    store(pRecData, r);
}

void ref_op_cdq(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.D = (r.A & MSB) ? ~XVALUE(0) : 0;
    store(pRecData, r);
}

void ref_op_daa(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nOldAL = r.A & 0xFF;
    const bool bOldCF = r.F & FLAG_CF;
    XVALUE nAL = nOldAL;

    if (((nAL & 0xF) > 9) || (r.F & FLAG_AF)) {
        setFlag(r.F, FLAG_CF, bOldCF || (nAL + 6 > 0xFF));
        nAL = (nAL + 6) & 0xFF;
        setFlag(r.F, FLAG_AF, true);
    } else {
        setFlag(r.F, FLAG_AF, false);
    }

    if ((nOldAL > 0x99) || bOldCF) {
        nAL = (nAL + 0x60) & 0xFF;
        setFlag(r.F, FLAG_CF, true);
    } else {
        setFlag(r.F, FLAG_CF, false);
    }

    setSZP8(r.F, nAL);
    r.A = (r.A & ~XVALUE(0xFF)) | nAL;
    store(pRecData, r);
}

void ref_op_das(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nOldAL = r.A & 0xFF;
    const bool bOldCF = r.F & FLAG_CF;
    XVALUE nAL = nOldAL;
    setFlag(r.F, FLAG_CF, false);

    if (((nAL & 0xF) > 9) || (r.F & FLAG_AF)) {
        setFlag(r.F, FLAG_CF, bOldCF || (nAL < 6));
        nAL = (nAL - 6) & 0xFF;
        setFlag(r.F, FLAG_AF, true);
    } else {
        setFlag(r.F, FLAG_AF, false);
    }

    if ((nOldAL > 0x99) || bOldCF) {
        nAL = (nAL - 0x60) & 0xFF;
        setFlag(r.F, FLAG_CF, true);
    }

    setSZP8(r.F, nAL);
    r.A = (r.A & ~XVALUE(0xFF)) | nAL;
    store(pRecData, r);
}

void ref_op_aaa(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nAX = r.A & 0xFFFF;

    if (((nAX & 0xF) > 9) || (r.F & FLAG_AF)) {
        nAX = (nAX + 0x106) & 0xFFFF;
        setFlag(r.F, FLAG_AF, true);
        setFlag(r.F, FLAG_CF, true);
    } else {
        setFlag(r.F, FLAG_AF, false);
        setFlag(r.F, FLAG_CF, false);
    }

    nAX &= 0xFF0F;
    r.A = (r.A & ~XVALUE(0xFFFF)) | nAX;
    store(pRecData, r);
}

void ref_op_aas(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nAX = r.A & 0xFFFF;

    if (((nAX & 0xF) > 9) || (r.F & FLAG_AF)) {
        nAX = (nAX - 6) & 0xFFFF;
        nAX = (nAX - 0x100) & 0xFFFF;  // AH -= 1
        setFlag(r.F, FLAG_AF, true);
        setFlag(r.F, FLAG_CF, true);
    } else {
        setFlag(r.F, FLAG_AF, false);
        setFlag(r.F, FLAG_CF, false);
    }

    nAX &= 0xFF0F;
    r.A = (r.A & ~XVALUE(0xFFFF)) | nAX;
    store(pRecData, r);
}

void ref_op_aam(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nAL = r.A & 0xFF;
    const XVALUE nNewAH = nAL / 10;
    const XVALUE nNewAL = nAL % 10;
    setSZP8(r.F, nNewAL);
    r.A = (r.A & ~XVALUE(0xFFFF)) | (nNewAH << 8) | nNewAL;
    store(pRecData, r);
}

void ref_op_aad(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nAL = r.A & 0xFF;
    const XVALUE nAH = (r.A >> 8) & 0xFF;
    const XVALUE nNewAL = (nAL + nAH * 10) & 0xFF;
    setSZP8(r.F, nNewAL);
    r.A = (r.A & ~XVALUE(0xFFFF)) | nNewAL;
    store(pRecData, r);
}

#endif  // OPCODE32

// --- bit-manipulation / checksum extensions -----------------------------------------
// Undefined-flag policy for these: flags the SDM lists as undefined are left
// unchanged; everything else follows the SDM exactly.

void ref_op_popcnt(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nCount = 0;
    for (XVALUE v = r.C; v; v &= v - 1) {
        ++nCount;
    }
    setFlag(r.F, FLAG_CF, false);
    setFlag(r.F, FLAG_OF, false);
    setFlag(r.F, FLAG_SF, false);
    setFlag(r.F, FLAG_AF, false);
    setFlag(r.F, FLAG_PF, false);
    setFlag(r.F, FLAG_ZF, r.C == 0);
    r.A = nCount;
    store(pRecData, r);
}

void ref_op_lzcnt(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    qint32 nCount = 0;
    while ((nCount < BITS) && !((r.C >> (BITS - 1 - nCount)) & 1)) {
        ++nCount;
    }
    setFlag(r.F, FLAG_CF, r.C == 0);
    setFlag(r.F, FLAG_ZF, nCount == 0);
    r.A = XVALUE(nCount);
    store(pRecData, r);
}

void ref_op_tzcnt(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    qint32 nCount = 0;
    while ((nCount < BITS) && !((r.C >> nCount) & 1)) {
        ++nCount;
    }
    setFlag(r.F, FLAG_CF, r.C == 0);
    setFlag(r.F, FLAG_ZF, nCount == 0);
    r.A = XVALUE(nCount);
    store(pRecData, r);
}

void ref_op_andn(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nResult = ~r.C & r.D;
    setFlag(r.F, FLAG_SF, nResult & MSB);
    setFlag(r.F, FLAG_ZF, nResult == 0);
    setFlag(r.F, FLAG_CF, false);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_bextr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nStart = static_cast<qint32>(r.D & 0xFF);
    const qint32 nLength = static_cast<qint32>((r.D >> 8) & 0xFF);
    XVALUE nResult = (nStart < BITS) ? (r.C >> nStart) : 0;
    if (nLength < BITS) {
        nResult &= (XVALUE(1) << nLength) - 1;
    }
    setFlag(r.F, FLAG_ZF, nResult == 0);
    setFlag(r.F, FLAG_CF, false);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_blsi(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nResult = r.C & (XVALUE(0) - r.C);
    setFlag(r.F, FLAG_CF, r.C != 0);
    setFlag(r.F, FLAG_ZF, nResult == 0);
    setFlag(r.F, FLAG_SF, nResult & MSB);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_blsmsk(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nResult = r.C ^ (r.C - 1);
    setFlag(r.F, FLAG_CF, r.C == 0);
    setFlag(r.F, FLAG_ZF, false);
    setFlag(r.F, FLAG_SF, nResult & MSB);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_blsr(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE nResult = r.C & (r.C - 1);
    setFlag(r.F, FLAG_CF, r.C == 0);
    setFlag(r.F, FLAG_ZF, nResult == 0);
    setFlag(r.F, FLAG_SF, nResult & MSB);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_bzhi(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nIndex = static_cast<qint32>(r.D & 0xFF);
    XVALUE nResult = r.C;
    if (nIndex < BITS) {
        nResult &= (XVALUE(1) << nIndex) - 1;
    }
    setFlag(r.F, FLAG_CF, nIndex > BITS - 1);
    setFlag(r.F, FLAG_ZF, nResult == 0);
    setFlag(r.F, FLAG_SF, nResult & MSB);
    setFlag(r.F, FLAG_OF, false);
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_pdep(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nResult = 0;
    qint32 nSourceBit = 0;
    for (qint32 i = 0; i < BITS; ++i) {
        if ((r.D >> i) & 1) {
            nResult |= ((r.C >> nSourceBit) & 1) << i;
            ++nSourceBit;
        }
    }
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_pext(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    XVALUE nResult = 0;
    qint32 nDestinationBit = 0;
    for (qint32 i = 0; i < BITS; ++i) {
        if ((r.D >> i) & 1) {
            nResult |= ((r.C >> i) & 1) << nDestinationBit;
            ++nDestinationBit;
        }
    }
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_shlx(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = r.C << (r.D & (BITS - 1));
    store(pRecData, r);
}

void ref_op_shrx(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    r.A = r.C >> (r.D & (BITS - 1));
    store(pRecData, r);
}

void ref_op_sarx(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const qint32 nCount = static_cast<qint32>(r.D & (BITS - 1));
    XVALUE nResult = r.C >> nCount;
    if ((r.C & MSB) && nCount) {
        nResult |= ~XVALUE(0) << (BITS - nCount);
    }
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_crc32(RECDATA *pRecData)
{
    // CRC-32C (Castagnoli), reflected polynomial 0x82F63B78; the accumulator is
    // the low 32 bits of A, the source bytes are consumed LSB first.
    Regs r = load(pRecData);
    quint32 nCrc = static_cast<quint32>(r.A & 0xFFFFFFFF);
    for (qint32 i = 0; i < BITS / 8; ++i) {
        nCrc ^= static_cast<quint32>((r.C >> (i * 8)) & 0xFF);
        for (qint32 j = 0; j < 8; ++j) {
            nCrc = (nCrc >> 1) ^ (0x82F63B78u & (0u - (nCrc & 1)));
        }
    }
    r.A = nCrc;
    store(pRecData, r);
}

void ref_op_adcx(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE t = r.A + r.C;
    const XVALUE nResult = t + ((r.F & FLAG_CF) ? 1 : 0);
    setFlag(r.F, FLAG_CF, (t < r.A) || (nResult < t));  // only CF is affected
    r.A = nResult;
    store(pRecData, r);
}

void ref_op_adox(RECDATA *pRecData)
{
    Regs r = load(pRecData);
    const XVALUE t = r.A + r.C;
    const XVALUE nResult = t + ((r.F & FLAG_OF) ? 1 : 0);
    setFlag(r.F, FLAG_OF, (t < r.A) || (nResult < t));  // only OF is affected
    r.A = nResult;
    store(pRecData, r);
}

// --- non-x86 targets run on the C backend directly ----------------------------------

#ifdef XOPCODECALC_C_BACKEND
extern "C" {
#define XOPCODECALC_DEFINE_FORWARDER(name)     \
    void op_##name(RECDATA *pRecData)          \
    {                                          \
        ref_op_##name(pRecData);               \
    }
XOPCODECALC_OPS_COMMON(XOPCODECALC_DEFINE_FORWARDER)
XOPCODECALC_OPS_WIDTH(XOPCODECALC_DEFINE_FORWARDER)
#undef XOPCODECALC_DEFINE_FORWARDER
}
#endif  // XOPCODECALC_C_BACKEND
