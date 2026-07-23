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
#ifndef DIVISIONCHECK_H
#define DIVISIONCHECK_H

#include "asm/asm.h"

// Exact x86 #DE predicates for DIV/IDIV, shared by the GUI and the tests.
namespace DivisionCheck {

// Restoring division of the double-width value (nHigh:nLow) by nDivisor.
// Precondition: nHigh < nDivisor, so the quotient fits into one XVALUE.
inline XVALUE divideDoubleWidth(XVALUE nHigh, XVALUE nLow, XVALUE nDivisor, XVALUE *pnRemainder = nullptr)
{
    const qint32 nBits = static_cast<qint32>(sizeof(XVALUE) * 8);
    XVALUE nQuotient = 0;
    XVALUE nRemainder = nHigh;

    for (qint32 i = nBits - 1; i >= 0; --i) {
        const bool bCarry = nRemainder >> (nBits - 1);
        nRemainder = (nRemainder << 1) | ((nLow >> i) & 1);

        if (bCarry || (nRemainder >= nDivisor)) {
            nRemainder -= nDivisor;
            nQuotient |= XVALUE(1) << i;
        }
    }

    if (pnRemainder) {
        *pnRemainder = nRemainder;
    }

    return nQuotient;
}

// Whether DIV (bSigned == false) or IDIV (bSigned == true) with the dividend
// (nHigh:nLow) and nDivisor executes without raising #DE.
inline bool isDivisionDefined(bool bSigned, XVALUE nLow, XVALUE nDivisor, XVALUE nHigh)
{
    if (nDivisor == 0) {
        return false;
    }

    if (!bSigned) {
        // DIV raises #DE exactly when the quotient does not fit the register,
        // i.e. when the high part of the dividend is >= the divisor.
        return nHigh < nDivisor;
    }

    // IDIV: compare magnitudes, then bound the quotient by the signed range.
    const XVALUE nSignBit = XVALUE(1) << (sizeof(XVALUE) * 8 - 1);
    const bool bNegativeDividend = nHigh & nSignBit;
    const bool bNegativeDivisor = nDivisor & nSignBit;

    XVALUE nMagnitudeLow = nLow;
    XVALUE nMagnitudeHigh = nHigh;
    if (bNegativeDividend) {
        nMagnitudeLow = XVALUE(~nLow + 1);
        nMagnitudeHigh = XVALUE(~nHigh + ((nLow == 0) ? 1 : 0));
    }
    const XVALUE nMagnitudeDivisor = bNegativeDivisor ? XVALUE(~nDivisor + 1) : nDivisor;

    if (nMagnitudeHigh >= nMagnitudeDivisor) {
        return false;  // the quotient magnitude does not even fit the unsigned register
    }

    const XVALUE nQuotient = divideDoubleWidth(nMagnitudeHigh, nMagnitudeLow, nMagnitudeDivisor);

    if (bNegativeDividend != bNegativeDivisor) {
        return nQuotient <= nSignBit;  // negative quotients reach down to -2^(W-1)
    }

    return nQuotient < nSignBit;  // positive quotients stop at 2^(W-1) - 1
}

}  // namespace DivisionCheck

#endif  // DIVISIONCHECK_H
