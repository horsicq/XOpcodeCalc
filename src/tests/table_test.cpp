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
// Opcode-table smoke test (no GUI, no window system needed).
//
// The GUI never re-derives what an opcode does: it drives the ASM_DEF tables and
// trusts them. These checks pin the invariants the GUI relies on, so a new row
// that breaks one is caught before it reaches a combo box:
//
//   1. every row is executable and labelled (asm_func / pszName / pszExample)
//   2. opcode ids are unique inside a group (GuiMainWindow keys a QMap by opcode,
//      so a duplicate silently loses a combo-box entry)
//   3. operand and result slots form a contiguous prefix (adjustValue() hides a
//      slot whose max value is 0, so a gap hides everything after it)
//   4. the register named in vrResult[i] really is the one GuiMainWindow puts
//      into result editor i, including the XADD/XCHG/CMPXCHG swap
//   5. no opcode changes a register it does not declare as a result
//   6. the DIV/IDIV #DE predicate agrees with long division and never lets a
//      divide that would fault through
//   7. every CPU-feature gate answers stably, and FEAT_BASE is always available

#include <cstdio>
#include <cstring>

#include "../asm_defs.h"
#include "../cpufeatures.h"
#include "../divisioncheck.h"
#include "../macbundlepath.h"

namespace {

qint64 g_nChecks = 0;
qint64 g_nFailures = 0;

void report(bool bOk, const char *pszGroup, const char *pszOpcode, const char *pszWhat)
{
    ++g_nChecks;
    if (!bOk) {
        ++g_nFailures;
        std::printf("FAIL [%s] %s: %s\n", pszGroup, pszOpcode ? pszOpcode : "?", pszWhat);
    }
}

// A/C/D/B index of a register name as the tables spell it (RAX/EAX/AX -> 0, ...).
// Returns -1 for an empty or unrecognised name.
qint32 regIndexFromName(const char *pszName)
{
    if (!pszName || !pszName[0]) {
        return -1;
    }

    const char *pszShort = pszName;
    if ((pszShort[0] == 'R') || (pszShort[0] == 'E')) {
        ++pszShort;  // RAX/EAX -> AX
    }

    if (pszShort[0] == 'A') return 0;
    if (pszShort[0] == 'C') return 1;
    if (pszShort[0] == 'D') return 2;
    if (pszShort[0] == 'B') return 3;

    return -1;
}

// RECDATA stores results as RESULT[0]=A, RESULT[1]=D, RESULT[2]=C, RESULT[3]=B.
qint32 regIndexFromResultSlot(qint32 nSlot)
{
    static const qint32 anMap[4] = {0, 2, 1, 3};
    return anMap[nSlot];
}

// GuiMainWindow::applyResultData(): editor i shows RESULT[i], except that
// XADD/XCHG/CMPXCHG swap slots 1 and 2 so that "A,C" lands in editors 0 and 1.
qint32 guiEditorSourceSlot(ASM_DEF::OP opcode, qint32 nEditor)
{
    const bool bSwapped = (opcode == ASM_DEF::OP_XADD) || (opcode == ASM_DEF::OP_XCHG) || (opcode == ASM_DEF::OP_CMPXCHG);

    if (bSwapped) {
        if (nEditor == 1) return 2;
        if (nEditor == 2) return 1;
    }

    return nEditor;
}

bool isDivision(ASM_DEF::OP opcode)
{
    return (opcode == ASM_DEF::OP_DIV) || (opcode == ASM_DEF::OP_IDIV);
}

// Non-deterministic opcodes cannot be compared against their own inputs.
bool isVolatile(ASM_DEF::OP opcode)
{
    return (opcode == ASM_DEF::OP_RDTSC) || (opcode == ASM_DEF::OP_CPUID);
}

quint64 g_nRandomState = 0xD1B54A32D192ED03ull;

quint64 random64()
{
    g_nRandomState ^= g_nRandomState << 13;
    g_nRandomState ^= g_nRandomState >> 7;
    g_nRandomState ^= g_nRandomState << 17;
    return g_nRandomState;
}

void checkSlots(const char *pszGroup, const ASM_DEF::OPCODE_RECORD &record, const ASM_DEF::VALUE_RECORD *pSlots, const char *pszKind)
{
    bool bSawEmpty = false;
    char szWhat[192];

    for (qint32 i = 0; i < 4; ++i) {
        const bool bUsed = (pSlots[i].nMaxValue != 0);

        if (bUsed && bSawEmpty) {
            std::snprintf(szWhat, sizeof(szWhat), "%s slot %d follows an empty slot, adjustValue() would hide it", pszKind, i);
            report(false, pszGroup, record.pszName, szWhat);
        } else {
            report(true, pszGroup, record.pszName, "");
        }

        if (!bUsed) {
            bSawEmpty = true;
            continue;
        }

        if (regIndexFromName(pSlots[i].pszRegName) < 0) {
            std::snprintf(szWhat, sizeof(szWhat), "%s slot %d has no usable register name", pszKind, i);
            report(false, pszGroup, record.pszName, szWhat);
        } else {
            report(true, pszGroup, record.pszName, "");
        }
    }
}

void checkGroup(const char *pszGroup, const ASM_DEF::OPCODE_RECORD *pRecords, qint32 nCount)
{
    char szWhat[256];

    for (qint32 i = 0; i < nCount; ++i) {
        const ASM_DEF::OPCODE_RECORD &record = pRecords[i];

        report(record.asm_func != 0, pszGroup, record.pszName, "asm_func is null");
        report((record.pszName != 0) && record.pszName[0], pszGroup, "?", "pszName is empty");
        report((record.pszExample != 0) && record.pszExample[0], pszGroup, record.pszName, "pszExample is empty");
        report(record.opcode != ASM_DEF::OP_UNKNOWN, pszGroup, record.pszName, "opcode is OP_UNKNOWN");

        // 2. unique opcode id inside the group
        for (qint32 j = 0; j < i; ++j) {
            if (pRecords[j].opcode == record.opcode) {
                std::snprintf(szWhat, sizeof(szWhat), "duplicate opcode id, collides with %s", pRecords[j].pszName);
                report(false, pszGroup, record.pszName, szWhat);
            }
        }

        checkSlots(pszGroup, record, record.vrOperand, "operand");
        checkSlots(pszGroup, record, record.vrResult, "result");

        // 4. the register named in result slot i is the one the GUI shows there
        for (qint32 nEditor = 0; nEditor < 4; ++nEditor) {
            if (record.vrResult[nEditor].nMaxValue == 0) {
                continue;
            }

            const qint32 nDeclared = regIndexFromName(record.vrResult[nEditor].pszRegName);
            const qint32 nActual = regIndexFromResultSlot(guiEditorSourceSlot(record.opcode, nEditor));

            if (nDeclared != nActual) {
                std::snprintf(szWhat, sizeof(szWhat), "result editor %d is labelled %s but receives register index %d", nEditor,
                              record.vrResult[nEditor].pszRegName, nActual);
                report(false, pszGroup, record.pszName, szWhat);
            } else {
                report(true, pszGroup, record.pszName, "");
            }
        }

        if (!record.asm_func) {
            continue;
        }

        if (!CpuFeatures::isSupported(record.feature)) {
            std::printf("SKIP [%s] %s: CPU feature not available\n", pszGroup, record.pszName);
            continue;
        }

        // 5. execute the opcode and check it only touches registers it declares
        bool bClobberReported = false;

        for (qint32 nCase = 0; (nCase < 256) && !bClobberReported; ++nCase) {
            RECDATA data = RECDATA_INIT;
            data.OPERAND[0] = XVALUE(random64());
            data.OPERAND[1] = XVALUE(random64());
            data.OPERAND[2] = XVALUE(random64());
            data.OPERAND[3] = XVALUE(random64());
            data.FLAG[0] = XVALUE(random64()) & (ASM_DEF::CF | ASM_DEF::PF | ASM_DEF::AF | ASM_DEF::ZF | ASM_DEF::SF | ASM_DEF::OF);

            if (isDivision(record.opcode)) {
                // GuiMainWindow::canExecuteOpcode() refuses to run an undefined divide.
                const bool bDefined =
                    DivisionCheck::isDivisionDefined(record.opcode == ASM_DEF::OP_IDIV, data.OPERAND[0], data.OPERAND[1], data.OPERAND[2]);
                if (!bDefined) {
                    continue;
                }
            }

            const XVALUE anBefore[4] = {data.OPERAND[0], data.OPERAND[2], data.OPERAND[1], data.OPERAND[3]};  // A, D, C, B

            record.asm_func(&data);
            ++g_nChecks;

            // FLAG[1] is a PUSHF image, so EFLAGS reserved bit 1 is always set.
            if (!(data.FLAG[1] & 0x2)) {
                report(false, pszGroup, record.pszName, "FLAG[1] is missing EFLAGS reserved bit 1");
            }

            if (isVolatile(record.opcode)) {
                continue;
            }

            for (qint32 nSlot = 0; (nSlot < 4) && !bClobberReported; ++nSlot) {
                if (data.RESULT[nSlot] == anBefore[nSlot]) {
                    continue;  // unchanged, nothing to declare
                }

                bool bDeclared = false;
                for (qint32 nEditor = 0; nEditor < 4; ++nEditor) {
                    if ((record.vrResult[nEditor].nMaxValue != 0) && (guiEditorSourceSlot(record.opcode, nEditor) == nSlot)) {
                        bDeclared = true;
                    }
                }

                if (!bDeclared) {
                    static const char *apszReg[4] = {"A", "D", "C", "B"};
                    std::snprintf(szWhat, sizeof(szWhat), "changes register %s but declares no result slot for it, the change is invisible in the GUI",
                                  apszReg[nSlot]);
                    report(false, pszGroup, record.pszName, szWhat);
                    bClobberReported = true;
                }
            }
        }
    }
}

void checkDivisionGuard()
{
    for (qint32 i = 0; i < 20000; ++i) {
        const XVALUE nLow = XVALUE(random64());
        const XVALUE nHigh = XVALUE(random64());

        report(!DivisionCheck::isDivisionDefined(false, nLow, 0, nHigh), "divisioncheck", "div", "divisor 0 reported as defined");
        report(!DivisionCheck::isDivisionDefined(true, nLow, 0, nHigh), "divisioncheck", "idiv", "divisor 0 reported as defined");

        const XVALUE nDivisor = XVALUE(random64() | 1);
        const bool bDefined = DivisionCheck::isDivisionDefined(false, nLow, nDivisor, nHigh);
        report(bDefined == (nHigh < nDivisor), "divisioncheck", "div", "unsigned #DE predicate disagrees with high < divisor");

        if (bDefined) {
            XVALUE nRemainder = 0;
            const XVALUE nQuotient = DivisionCheck::divideDoubleWidth(nHigh, nLow, nDivisor, &nRemainder);
            report(nRemainder < nDivisor, "divisioncheck", "div", "remainder is not less than the divisor");
            // quotient*divisor+remainder must reproduce the low half of the dividend;
            // nRemainder < nDivisor pins the high half.
            report(XVALUE(nQuotient * nDivisor + nRemainder) == nLow, "divisioncheck", "div", "quotient*divisor+remainder != dividend");
        }
    }
}

// The macOS plugin path is derived from argv[0] before QApplication exists, so it
// can only ever be exercised by a macOS release build. Pin it here instead, where
// every build runs it.
void checkMacBundlePath()
{
    struct Case {
        const char *pszExecutablePath;
        const char *pszExecutableName;
        const char *pszExpected;
        const char *pszWhat;
    };

    const Case cases[] = {
        {"/Applications/xocalc.app/Contents/MacOS/xocalc", "xocalc", "/Applications/xocalc.app/Contents/PlugIns", "a normal bundle"},
        {"/Users/x/build/src/gui/xocalc.app/Contents/MacOS/xocalc", "xocalc", "/Users/x/build/src/gui/xocalc.app/Contents/PlugIns", "a build-tree bundle"},
        // The bug this replaced: the name did not match, and the old code still
        // produced a path instead of backing off.
        {"/Applications/xocalc.app/Contents/MacOS/XOcalc", "xocalc", "", "a case-mismatched executable name"},
        {"/usr/local/bin/xocalc", "xocalc", "", "a bare binary outside a bundle"},
        {"xocalc", "xocalc", "", "a relative bare binary"},
        {"/Applications/xocalc.app/Contents/MacOS/xocalc", "", "", "an empty executable name"},
        {"/Applications/MacOS/xocalc/Contents/MacOS/xocalc", "xocalc", "/Applications/MacOS/xocalc/Contents/PlugIns", "MacOS appearing earlier in the path"},
    };

    for (const Case &testCase : cases) {
        const QString sResult =
            MacBundlePath::pluginPathFromExecutable(QString::fromUtf8(testCase.pszExecutablePath), QString::fromUtf8(testCase.pszExecutableName));
        const QString sExpected = QString::fromUtf8(testCase.pszExpected);

        if (sResult != sExpected) {
            std::printf("FAIL [macbundlepath] %s: got \"%s\", wanted \"%s\"\n", testCase.pszWhat, sResult.toUtf8().constData(),
                        sExpected.toUtf8().constData());
            ++g_nFailures;
        }
        ++g_nChecks;
    }
}

void checkCpuFeatures()
{
    report(CpuFeatures::isSupported(ASM_DEF::FEAT_BASE), "cpufeatures", "FEAT_BASE", "base opcodes reported unsupported");

    const ASM_DEF::FEATURE aFeatures[] = {ASM_DEF::FEAT_POPCNT, ASM_DEF::FEAT_LZCNT, ASM_DEF::FEAT_BMI1,
                                          ASM_DEF::FEAT_BMI2,   ASM_DEF::FEAT_SSE42, ASM_DEF::FEAT_ADX};
    const char *apszNames[] = {"POPCNT", "LZCNT", "BMI1", "BMI2", "SSE4.2", "ADX"};

    for (qint32 i = 0; i < 6; ++i) {
        const bool bFirst = CpuFeatures::isSupported(aFeatures[i]);
        const bool bSecond = CpuFeatures::isSupported(aFeatures[i]);
        report(bFirst == bSecond, "cpufeatures", apszNames[i], "isSupported() is not stable across calls");
        std::printf("INFO cpufeature %-7s %s\n", apszNames[i], bFirst ? "yes" : "no");
    }
}

}  // namespace

#define XCHECK_GROUP(table) checkGroup(#table, ASM_DEF::table, (qint32)(sizeof(ASM_DEF::table) / sizeof(ASM_DEF::table[0])))

int main()
{
    XCHECK_GROUP(asm_twooperands);
    XCHECK_GROUP(asm_oneoperand);
    XCHECK_GROUP(asm_muldiv);
    XCHECK_GROUP(asm_shift);
    XCHECK_GROUP(asm_bits);
#ifndef OPCODE64
    XCHECK_GROUP(asm_bcd);
#endif
    XCHECK_GROUP(asm_special);

    checkDivisionGuard();
    checkMacBundlePath();
    checkCpuFeatures();

    std::printf("%s: %lld checks, %lld failures (%d-bit build)\n", g_nFailures ? "FAILED" : "OK", (long long)g_nChecks, (long long)g_nFailures,
                (int)(sizeof(XVALUE) * 8));

    return g_nFailures ? 1 : 0;
}
