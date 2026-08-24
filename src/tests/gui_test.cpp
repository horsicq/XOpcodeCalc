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
// GUI smoke test. Builds the real GuiMainWindow on the offscreen platform and
// drives it the way a user would: pick a group, pick an opcode, type operands,
// toggle flags, switch display mode, press Copy.
//
// Typing goes through synthesised key events, not setText(), because that is the
// only path that runs the line edit's validator - which is exactly where the
// operand fields can silently refuse input.

#include <QApplication>
#include <QClipboard>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTranslator>
#include <QRegularExpression>
#include <QToolButton>
#include <cstdio>
#include <cstring>

#include "dialogoptions.h"
#include "guimainwindow.h"
#include "xlineedithex.h"

namespace {

qint64 g_nChecks = 0;
qint64 g_nFailures = 0;

void check(bool bOk, const QString &sWhat, const QString &sDetail = QString())
{
    ++g_nChecks;
    if (bOk) {
        std::printf("  ok    %s\n", sWhat.toUtf8().constData());
    } else {
        ++g_nFailures;
        std::printf("FAIL    %s%s%s\n", sWhat.toUtf8().constData(), sDetail.isEmpty() ? "" : " -- ", sDetail.toUtf8().constData());
    }
}

template <typename T>
T *findWidget(QWidget *pRoot, const QString &sName)
{
    T *pResult = pRoot->findChild<T *>(sName);
    if (!pResult) {
        ++g_nChecks;
        ++g_nFailures;
        std::printf("FAIL    widget not found: %s\n", sName.toUtf8().constData());
    }
    return pResult;
}

// Send the same key events a real keystroke produces, so the validator runs.
void typeText(QWidget *pWidget, const QString &sText)
{
    pWidget->setFocus(Qt::OtherFocusReason);

    for (qint32 i = 0; i < sText.size(); ++i) {
        const QChar character = sText.at(i);
        QKeyEvent pressEvent(QEvent::KeyPress, 0, Qt::NoModifier, QString(character));
        QKeyEvent releaseEvent(QEvent::KeyRelease, 0, Qt::NoModifier, QString(character));
        QCoreApplication::sendEvent(pWidget, &pressEvent);
        QCoreApplication::sendEvent(pWidget, &releaseEvent);
    }
}

void clearAndType(XLineEditHEX *pEdit, const QString &sText)
{
    pEdit->clear();
    typeText(pEdit, sText);
}

bool selectComboItem(QComboBox *pComboBox, const QString &sText)
{
    const qint32 nIndex = pComboBox->findText(sText);
    if (nIndex < 0) {
        return false;
    }
    pComboBox->setCurrentIndex(nIndex);
    return true;
}

struct Widgets {
    QComboBox *pGroup;
    QComboBox *pOpcode;
    QComboBox *pMode;
    XLineEditHEX *pOperand[4];
    XLineEditHEX *pResult[4];
    XLineEditHEX *pFlagsBefore;
    XLineEditHEX *pFlagsAfter;
    QGroupBox *pOperandBox[4];
    QGroupBox *pResultBox[4];
    QToolButton *pOpcodeButton;
    QPushButton *pCopy;
};

bool collect(GuiMainWindow *pWindow, Widgets *pWidgets)
{
    pWidgets->pGroup = findWidget<QComboBox>(pWindow, "comboBoxOpcodeGroup");
    pWidgets->pOpcode = findWidget<QComboBox>(pWindow, "comboBoxOpcode");
    pWidgets->pMode = findWidget<QComboBox>(pWindow, "comboBoxMode");
    pWidgets->pFlagsBefore = findWidget<XLineEditHEX>(pWindow, "lineEditFlagsBefore");
    pWidgets->pFlagsAfter = findWidget<XLineEditHEX>(pWindow, "lineEditFlagsAfter");
    pWidgets->pOpcodeButton = findWidget<QToolButton>(pWindow, "toolButtonOpcode");
    pWidgets->pCopy = findWidget<QPushButton>(pWindow, "pushButtonCopy");

    bool bOk = pWidgets->pGroup && pWidgets->pOpcode && pWidgets->pMode && pWidgets->pFlagsBefore && pWidgets->pFlagsAfter && pWidgets->pOpcodeButton &&
               pWidgets->pCopy;

    for (qint32 i = 0; i < 4; ++i) {
        pWidgets->pOperand[i] = findWidget<XLineEditHEX>(pWindow, QString("lineEditOperand%1").arg(i + 1));
        pWidgets->pResult[i] = findWidget<XLineEditHEX>(pWindow, QString("lineEditResult%1").arg(i + 1));
        pWidgets->pOperandBox[i] = findWidget<QGroupBox>(pWindow, QString("groupBoxOperand%1").arg(i + 1));
        pWidgets->pResultBox[i] = findWidget<QGroupBox>(pWindow, QString("groupBoxResult%1").arg(i + 1));
        bOk = bOk && pWidgets->pOperand[i] && pWidgets->pResult[i] && pWidgets->pOperandBox[i] && pWidgets->pResultBox[i];
    }

    return bOk;
}

// --- individual smoke checks --------------------------------------------------------

void checkStartup(GuiMainWindow *pWindow, const Widgets &w)
{
    std::printf("[startup]\n");
    check(pWindow->isVisible(), "main window is visible");
    check(!pWindow->windowTitle().isEmpty(), "window title is set", pWindow->windowTitle());
    check(w.pGroup->count() > 0, "opcode-group combo is populated");
    check(w.pOpcode->count() > 0, "opcode combo is populated");
    check(w.pMode->count() == 4, "mode combo offers HEX/Signed/Unsigned/Binary");
    check(!w.pOpcodeButton->text().isEmpty(), "opcode example label is set", w.pOpcodeButton->text());

    for (qint32 i = 0; i < 4; ++i) {
        check(w.pResult[i]->isReadOnly(), QString("result editor %1 is read-only").arg(i + 1));
    }
    check(w.pFlagsAfter->isReadOnly(), "flags-after editor is read-only");

    for (qint32 i = 0; i < 4; ++i) {
        std::printf("  info  operand %d starts as '%s' (box '%s', %s)\n", i + 1, w.pOperand[i]->text().toUtf8().constData(),
                    w.pOperandBox[i]->title().toUtf8().constData(), w.pOperandBox[i]->isHidden() ? "hidden" : "shown");
    }
    std::printf("  info  flags-before starts as '%s'\n", w.pFlagsBefore->text().toUtf8().constData());
}

// Walk every group and every opcode; nothing may crash and the example label must update.
void checkAllOpcodesReachable(const Widgets &w)
{
    std::printf("[every opcode selectable]\n");
    qint32 nTotal = 0;
    bool bAllLabelled = true;
    bool bAllHaveOperandBox = true;

    for (qint32 nGroup = 0; nGroup < w.pGroup->count(); ++nGroup) {
        w.pGroup->setCurrentIndex(nGroup);

        for (qint32 nOpcode = 0; nOpcode < w.pOpcode->count(); ++nOpcode) {
            w.pOpcode->setCurrentIndex(nOpcode);
            ++nTotal;

            if (w.pOpcodeButton->text().isEmpty()) {
                bAllLabelled = false;
                std::printf("        no example for %s\n", w.pOpcode->currentText().toUtf8().constData());
            }

            // At least one visible box, or none at all for the flag-only opcodes.
            qint32 nVisibleOperands = 0;
            for (qint32 i = 0; i < 4; ++i) {
                if (!w.pOperandBox[i]->isHidden()) {
                    ++nVisibleOperands;
                }
            }
            const QString sName = w.pOpcode->currentText();
            const bool bFlagsOnly = (sName == "clc") || (sName == "stc") || (sName == "cmc") || (sName == "rdtsc");
            if ((nVisibleOperands == 0) && !bFlagsOnly) {
                bAllHaveOperandBox = false;
                std::printf("        no operand box for %s\n", sName.toUtf8().constData());
            }
        }
    }

    check(nTotal > 0, QString("walked %1 opcodes across %2 groups").arg(nTotal).arg(w.pGroup->count()));
    check(bAllLabelled, "every opcode has an example label");
    check(bAllHaveOperandBox, "every non-flag-only opcode shows at least one operand box");
}

// The core question: can a user actually type an operand?
void checkOperandInput(const Widgets &w)
{
    std::printf("[operand input]\n");

    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "add"), "select ADD");
    check(selectComboItem(w.pMode, "HEX"), "select HEX display mode");

    clearAndType(w.pOperand[0], "12");
    check(w.pOperand[0]->text() == "12", "two hex digits reach operand 1", QString("got '%1'").arg(w.pOperand[0]->text()));

    clearAndType(w.pOperand[0], "FFFFFFFF");
    check(w.pOperand[0]->text() == "FFFFFFFF", "eight hex digits reach operand 1", QString("got '%1'").arg(w.pOperand[0]->text()));

    const qint32 nRegisterBits = (qint32)(sizeof(XVALUE) * 8);
    if (nRegisterBits == 64) {
        clearAndType(w.pOperand[0], "123456789ABCDEF0");
        check(w.pOperand[0]->text() == "123456789ABCDEF0", "a full 64-bit operand can be typed", QString("got '%1'").arg(w.pOperand[0]->text()));
    }

    check(selectComboItem(w.pMode, "Unsigned"), "select Unsigned display mode");
    clearAndType(w.pOperand[0], "1234");
    check(w.pOperand[0]->text() == "1234", "a positive decimal reaches operand 1 in Unsigned mode", QString("got '%1'").arg(w.pOperand[0]->text()));

    check(selectComboItem(w.pMode, "Signed"), "select Signed display mode");
    clearAndType(w.pOperand[0], "1234");
    check(w.pOperand[0]->text() == "1234", "a positive decimal reaches operand 1 in Signed mode", QString("got '%1'").arg(w.pOperand[0]->text()));

    check(selectComboItem(w.pMode, "Binary"), "select Binary display mode");
    clearAndType(w.pOperand[0], "1011");
    check(w.pOperand[0]->text() == "1011", "binary digits reach operand 1", QString("got '%1'").arg(w.pOperand[0]->text()));

    check(selectComboItem(w.pMode, "HEX"), "back to HEX display mode");
}

// Each field must be as wide as the register it stands for - no wider (you could
// enter a value the CPU cannot hold) and no narrower (you could not enter a real one).
void checkRegisterWidths(const Widgets &w)
{
    std::printf("[register widths]\n");

    const qint32 nRegisterBits = (qint32)(sizeof(XVALUE) * 8);
    const QString sWide = (nRegisterBits == 64) ? "123456789ABCDEF0" : "12345678";

    check(selectComboItem(w.pMode, "HEX"), "select HEX display mode");

    // Full-width register operand.
    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "add"), "select ADD");
    clearAndType(w.pOperand[0], sWide);
    check(w.pOperand[0]->text() == sWide, "a full-width operand is accepted", QString("got '%1'").arg(w.pOperand[0]->text()));
    check(w.pResult[0]->text().length() == nRegisterBits / 4, "the result is rendered at the register width",
          QString("got '%1'").arg(w.pResult[0]->text()));

    // 8-bit operand: MOVZX r,r8 takes CL, so three hex digits must not fit.
    if (selectComboItem(w.pOpcode, QString("movzx %1,r8").arg(nRegisterBits == 64 ? "r64" : "r32"))) {
        clearAndType(w.pOperand[1], "FF");
        check(w.pOperand[1]->text() == "FF", "an 8-bit operand accepts two hex digits", QString("got '%1'").arg(w.pOperand[1]->text()));
        clearAndType(w.pOperand[1], "1FF");
        check(w.pOperand[1]->text() != "1FF", "an 8-bit operand rejects three hex digits", QString("got '%1'").arg(w.pOperand[1]->text()));

        clearAndType(w.pOperand[1], "80");
        check(w.pResult[0]->text().toULongLong(0, 16) == 0x80, "MOVZX r,r8 zero-extends 0x80", QString("got '%1'").arg(w.pResult[0]->text()));
    } else {
        check(false, "select MOVZX r,r8", "opcode not offered");
    }

    // 16-bit operand.
    if (selectComboItem(w.pOpcode, QString("movsx %1,r16").arg(nRegisterBits == 64 ? "r64" : "r32"))) {
        clearAndType(w.pOperand[1], "8000");
        check(w.pOperand[1]->text() == "8000", "a 16-bit operand accepts four hex digits", QString("got '%1'").arg(w.pOperand[1]->text()));
        clearAndType(w.pOperand[1], "18000");
        check(w.pOperand[1]->text() != "18000", "a 16-bit operand rejects five hex digits", QString("got '%1'").arg(w.pOperand[1]->text()));

        clearAndType(w.pOperand[1], "8000");
        const QString sExpected = (nRegisterBits == 64) ? "ffffffffffff8000" : "ffff8000";
        check(w.pResult[0]->text().toLower() == sExpected, "MOVSX r,r16 sign-extends 0x8000",
              QString("got '%1', wanted '%2'").arg(w.pResult[0]->text(), sExpected));
    } else {
        check(false, "select MOVSX r,r16", "opcode not offered");
    }

    // The shift count field is a CL, so it must not take a third digit either - and
    // its range is the mask the CPU applies to the count, which is width-specific:
    // 0..0x3F for a 64-bit shift, 0..0x1F for a 32-bit one. Hardcoding 3F here made
    // this check silently 64-bit-only; on a 32-bit build the field correctly refused
    // it, because a count of 63 cannot address a 32-bit register.
    const QString sMaxCount = (nRegisterBits == 64) ? QStringLiteral("3F") : QStringLiteral("1F");
    const QString sOverCount = (nRegisterBits == 64) ? QStringLiteral("40") : QStringLiteral("20");

    check(selectComboItem(w.pGroup, "Shift"), "select the Shift group");
    check(selectComboItem(w.pOpcode, "shl"), "select SHL");
    clearAndType(w.pOperand[1], sMaxCount);
    check(w.pOperand[1]->text() == sMaxCount, QString("the CL count field accepts %1").arg(sMaxCount),
          QString("got '%1'").arg(w.pOperand[1]->text()));
    clearAndType(w.pOperand[1], sOverCount);
    check(w.pOperand[1]->text() != sOverCount, QString("the CL count field rejects %1, past the shift mask").arg(sOverCount),
          QString("got '%1'").arg(w.pOperand[1]->text()));
    clearAndType(w.pOperand[1], "1FF");
    check(w.pOperand[1]->text() != "1FF", "the CL count field rejects three hex digits", QString("got '%1'").arg(w.pOperand[1]->text()));
}

// Negative operands must survive the trip through the widget in Signed mode.
void checkSignedInput(const Widgets &w)
{
    std::printf("[signed input]\n");

    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "mov"), "select MOV");
    check(selectComboItem(w.pMode, "Signed"), "select Signed display mode");

    clearAndType(w.pOperand[0], "0");
    clearAndType(w.pOperand[1], "-1");
    check(w.pOperand[1]->text() == "-1", "a negative operand is accepted", QString("got '%1'").arg(w.pOperand[1]->text()));
    check(w.pResult[0]->text() == "-1", "MOV of -1 produces -1", QString("got '%1'").arg(w.pResult[0]->text()));

    clearAndType(w.pOperand[1], "1234");
    check(w.pOperand[1]->text() == "1234", "a positive operand is still accepted in Signed mode",
          QString("got '%1'").arg(w.pOperand[1]->text()));
    check(w.pResult[0]->text() == "1234", "MOV of 1234 produces 1234", QString("got '%1'").arg(w.pResult[0]->text()));

    // Signed and unsigned must be two views of the same bits.
    clearAndType(w.pOperand[1], "-2");
    check(selectComboItem(w.pMode, "HEX"), "switch to HEX");
    const QString sExpectedHex = QString(sizeof(XVALUE) * 2, QChar('f'));
    const QString sGot = w.pResult[0]->text().toLower();
    check(sGot == sExpectedHex.left(sExpectedHex.length() - 1) + "e", "-2 reads back as all-ones-except-bit-0 in HEX",
          QString("got '%1'").arg(sGot));
}

// A known-answer calculation end to end through the widgets.
void checkKnownAnswers(const Widgets &w)
{
    std::printf("[known answers]\n");

    struct Answer {
        const char *pszGroup;
        const char *pszOpcode;
        const char *pszA;
        const char *pszC;
        const char *pszD;
        const char *pszFlags;
        const char *pszResult1;
    };

    // Values chosen to stay inside 32 bits so the same table works for both builds.
    const Answer answers[] = {
        {"Two operands", "add", "10", "20", "0", "0", "30"},
        {"Two operands", "sub", "30", "10", "0", "0", "20"},
        {"Two operands", "and", "F0F0", "FF00", "0", "0", "F000"},
        {"Two operands", "xor", "F0F0", "FF00", "0", "0", "FF0"},
        {"Two operands", "mov", "1", "DEAD", "0", "0", "DEAD"},
        {"One operand", "inc", "FF", "0", "0", "0", "100"},
        {"One operand", "dec", "100", "0", "0", "0", "FF"},
        {"Mul/Div", "mul", "10", "10", "0", "0", "100"},
        {"Mul/Div", "div", "64", "A", "0", "0", "A"},
        {"Shift", "shl", "1", "4", "0", "0", "10"},
        {"Shift", "shr", "10", "4", "0", "0", "1"},
        {"Bits", "bts", "0", "4", "0", "0", "10"},
    };

    for (const Answer &answer : answers) {
        const QString sLabel = QString("%1 -> %2").arg(answer.pszOpcode, answer.pszResult1);

        if (!selectComboItem(w.pGroup, QString::fromUtf8(answer.pszGroup))) {
            check(false, sLabel, QString("group '%1' not found").arg(answer.pszGroup));
            continue;
        }
        if (!selectComboItem(w.pOpcode, QString::fromUtf8(answer.pszOpcode))) {
            check(false, sLabel, QString("opcode '%1' not offered (CPU feature gate?)").arg(answer.pszOpcode));
            continue;
        }

        clearAndType(w.pFlagsBefore, QString::fromUtf8(answer.pszFlags));
        clearAndType(w.pOperand[0], QString::fromUtf8(answer.pszA));
        clearAndType(w.pOperand[1], QString::fromUtf8(answer.pszC));
        clearAndType(w.pOperand[2], QString::fromUtf8(answer.pszD));

        const QString sGot = w.pResult[0]->text();
        const bool bOk = (sGot.toULongLong(0, 16) == QString::fromUtf8(answer.pszResult1).toULongLong(0, 16)) && !sGot.isEmpty();
        check(bOk, sLabel, QString("operands were '%1','%2' and result 1 read '%3'").arg(w.pOperand[0]->text(), w.pOperand[1]->text(), sGot));
    }
}

// The BCD adjusts exist only in a 32-bit build - they are invalid in 64-bit mode, so
// this whole group is unreachable from the normal build and had never been driven
// through the GUI at all.
void checkBcdGroup(const Widgets &w)
{
    if (sizeof(XVALUE) != 4) {
        std::printf("[bcd] skipped: 64-bit build has no BCD group\n");
        return;
    }

    std::printf("[bcd]\n");

    if (!selectComboItem(w.pGroup, "BCD")) {
        check(false, "select the BCD group", "group not offered in a 32-bit build");
        return;
    }
    check(true, "select the BCD group");
    check(selectComboItem(w.pMode, "HEX"), "select HEX display mode");

    struct BcdCase {
        const char *pszOpcode;
        const char *pszAx;
        const char *pszFlags;
        quint32 nExpected;
        const char *pszWhy;
    };

    // The operand is AX; each adjust works on AL and may carry into AH.
    const BcdCase cases[] = {
        // AL=0x0A: low nibble > 9, so add 6 -> 0x10, AF set.
        {"daa", "A", "0", 0x0010, "DAA of 0x0A is 0x10"},
        // AL=0x9A: low nibble > 9 and AL > 0x99, so +6 then +0x60 -> 0x00 with CF.
        {"daa", "9A", "0", 0x0000, "DAA of 0x9A wraps to 0x00"},
        // AL=0x0F: subtract 6 -> 0x09, AF set.
        {"das", "F", "0", 0x0009, "DAS of 0x0F is 0x09"},
        // AAM: unpack AL into AH=AL/10, AL=AL%10. 0x0F = 15 -> 1 and 5.
        {"aam", "F", "0", 0x0105, "AAM of 15 is 1 and 5"},
        // AAD: pack AH*10+AL. 0x0105 -> 15 = 0x0F.
        {"aad", "105", "0", 0x000F, "AAD of 1 and 5 is 15"},
        // AAA: low nibble > 9 -> AX += 0x106, then AL &= 0x0F.
        {"aaa", "A", "0", 0x0100, "AAA of 0x0A is AH=1 AL=0"},
    };

    for (const BcdCase &bcdCase : cases) {
        if (!selectComboItem(w.pOpcode, QString::fromUtf8(bcdCase.pszOpcode))) {
            check(false, QString("select %1").arg(QString::fromUtf8(bcdCase.pszOpcode)), "opcode not offered");
            continue;
        }

        clearAndType(w.pFlagsBefore, QString::fromUtf8(bcdCase.pszFlags));
        clearAndType(w.pOperand[0], QString::fromUtf8(bcdCase.pszAx));

        const QString sGot = w.pResult[0]->text();
        check(sGot.toUInt(0, 16) == bcdCase.nExpected, QString::fromUtf8(bcdCase.pszWhy),
              QString("AX=%1 gave '%2', wanted %3").arg(QString::fromUtf8(bcdCase.pszAx), sGot, QString::number(bcdCase.nExpected, 16)));
    }

    // The AX operand is 16 bits wide, so the field must be four hex digits, not eight.
    check(w.pOperandBox[0]->title() == QStringLiteral("AX"), "the BCD operand is labelled AX",
          QString("got '%1'").arg(w.pOperandBox[0]->title()));
    clearAndType(w.pOperand[0], "FFFF");
    check(w.pOperand[0]->text() == QStringLiteral("FFFF"), "a 16-bit BCD operand accepts four hex digits",
          QString("got '%1'").arg(w.pOperand[0]->text()));
    clearAndType(w.pOperand[0], "1FFFF");
    check(w.pOperand[0]->text() != QStringLiteral("1FFFF"), "a 16-bit BCD operand rejects five",
          QString("got '%1'").arg(w.pOperand[0]->text()));
}

// Division by zero must blank the results instead of executing and faulting,
// and must say why.
void checkDivisionGuard(GuiMainWindow *pWindow, const Widgets &w)
{
    std::printf("[division guard]\n");

    QLabel *pStatus = findWidget<QLabel>(pWindow, "labelStatus");
    if (!pStatus) {
        return;
    }

    check(selectComboItem(w.pMode, "HEX"), "select HEX display mode");
    check(selectComboItem(w.pGroup, "Mul/Div"), "select the Mul/Div group");
    check(selectComboItem(w.pOpcode, "div"), "select DIV");

    clearAndType(w.pOperand[0], "64");
    clearAndType(w.pOperand[1], "0");
    clearAndType(w.pOperand[2], "0");
    check(w.pResult[0]->text().isEmpty(), "DIV by zero blanks result 1 instead of faulting", QString("got '%1'").arg(w.pResult[0]->text()));
    check(pStatus->text().contains("#DE"), "DIV by zero reports a #DE", QString("status was '%1'").arg(pStatus->text()));
    check(pStatus->text().contains("divisor"), "the message names the divisor as the cause", QString("status was '%1'").arg(pStatus->text()));

    // Quotient overflow: high half >= divisor.
    clearAndType(w.pOperand[1], "2");
    clearAndType(w.pOperand[2], "FF");
    check(w.pResult[0]->text().isEmpty(), "DIV with a quotient overflow blanks result 1", QString("got '%1'").arg(w.pResult[0]->text()));
    check(pStatus->text().contains("#DE"), "a quotient overflow reports a #DE", QString("status was '%1'").arg(pStatus->text()));
    check(pStatus->text().contains("quotient"), "the message names the quotient as the cause", QString("status was '%1'").arg(pStatus->text()));
    check(pStatus->text().contains(w.pResultBox[0]->title()), "the message names the destination register",
          QString("status was '%1', register is '%2'").arg(pStatus->text(), w.pResultBox[0]->title()));

    // The report the Copy button builds must carry the reason too.
    QApplication::clipboard()->clear();
    w.pCopy->click();
    check(QApplication::clipboard()->text().contains("#DE"), "the Copy report carries the #DE reason");

    // And it recovers.
    clearAndType(w.pOperand[2], "0");
    clearAndType(w.pOperand[1], "A");
    check(!w.pResult[0]->text().isEmpty(), "a defined DIV produces a result again");
    check(pStatus->text().isEmpty(), "the message clears once the divide is defined", QString("status was '%1'").arg(pStatus->text()));

    // A normal opcode must never leave a stale message behind.
    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "add"), "select ADD");
    check(pStatus->text().isEmpty(), "no message for an ordinary opcode", QString("status was '%1'").arg(pStatus->text()));

    // IDIV has its own predicate; the most negative dividend over -1 overflows.
    check(selectComboItem(w.pGroup, "Mul/Div"), "select the Mul/Div group");
    if (selectComboItem(w.pOpcode, "idiv")) {
        const QString sAllOnes = QString(sizeof(XVALUE) * 2, QChar('F'));
        clearAndType(w.pOperand[0], "0");
        clearAndType(w.pOperand[2], "8" + QString(sizeof(XVALUE) * 2 - 1, QChar('0')));
        clearAndType(w.pOperand[1], sAllOnes);
        check(w.pResult[0]->text().isEmpty(), "IDIV of the most negative dividend by -1 is refused",
              QString("got '%1'").arg(w.pResult[0]->text()));
        check(pStatus->text().contains("#DE"), "IDIV overflow reports a #DE", QString("status was '%1'").arg(pStatus->text()));
    } else {
        check(false, "select IDIV", "opcode not offered");
    }
}

// Flag buttons and the flags-before field must stay in sync, without a signal loop.
void checkFlagRoundTrip(GuiMainWindow *pWindow, const Widgets &w)
{
    std::printf("[flags]\n");

    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "adc"), "select ADC");
    clearAndType(w.pOperand[0], "1");
    clearAndType(w.pOperand[1], "1");

    QPushButton *pCarryButton = findWidget<QPushButton>(pWindow, "pushButtonFlagCF");
    if (!pCarryButton) {
        return;
    }

    clearAndType(w.pFlagsBefore, "0");
    check(!pCarryButton->isChecked(), "CF button follows flags-before = 0");
    check(w.pResult[0]->text().toULongLong(0, 16) == 2, "ADC 1,1 with CF=0 is 2", QString("got '%1'").arg(w.pResult[0]->text()));

    pCarryButton->setChecked(true);
    check(w.pFlagsBefore->text().toULongLong(0, 16) & 0x1, "pressing the CF button sets bit 0 of flags-before",
          QString("got '%1'").arg(w.pFlagsBefore->text()));
    check(w.pResult[0]->text().toULongLong(0, 16) == 3, "ADC 1,1 with CF=1 is 3", QString("got '%1'").arg(w.pResult[0]->text()));

    pCarryButton->setChecked(false);
    check(!(w.pFlagsBefore->text().toULongLong(0, 16) & 0x1), "releasing the CF button clears bit 0 of flags-before");

    // Flags-after must never show the reserved/IF bits that PUSHF sets.
    clearAndType(w.pOperand[0], "0");
    clearAndType(w.pOperand[1], "0");
    const quint64 nAfter = w.pFlagsAfter->text().toULongLong(0, 16);
    check(!(nAfter & 0x202), "flags-after hides EFLAGS reserved bit 1 and IF", QString("got 0x%1").arg(nAfter, 0, 16));
    check(nAfter & 0x40, "ADD 0,0 sets ZF", QString("got 0x%1").arg(nAfter, 0, 16));
}

void checkCopyReport(const Widgets &w)
{
    std::printf("[copy]\n");

    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "add"), "select ADD");
    clearAndType(w.pOperand[0], "10");
    clearAndType(w.pOperand[1], "20");

    QApplication::clipboard()->clear();
    w.pCopy->click();

    const QString sText = QApplication::clipboard()->text();
    check(!sText.isEmpty(), "Copy puts something on the clipboard");
    check(sText.contains("ADD"), "the report names the opcode", sText.left(40));
    check(sText.contains("30"), "the report carries the result", sText.left(80));
    check(!sText.contains("\n\n"), "the report has no blank lines");
}

// Switching display mode must not lose the current values.
void checkModeSwitchKeepsValues(const Widgets &w)
{
    std::printf("[display modes]\n");

    check(selectComboItem(w.pGroup, "Two operands"), "select the two-operand group");
    check(selectComboItem(w.pOpcode, "add"), "select ADD");
    check(selectComboItem(w.pMode, "HEX"), "select HEX display mode");
    clearAndType(w.pOperand[0], "10");
    clearAndType(w.pOperand[1], "20");

    const QString sHexResult = w.pResult[0]->text();
    check(sHexResult.toULongLong(0, 16) == 0x30, "HEX mode shows 30");

    check(selectComboItem(w.pMode, "Unsigned"), "switch to Unsigned");
    check(w.pOperand[0]->text().toULongLong() == 0x10, "operand 1 keeps its value across the mode switch",
          QString("got '%1'").arg(w.pOperand[0]->text()));
    check(w.pResult[0]->text().toULongLong() == 0x30, "result 1 is re-rendered as decimal 48", QString("got '%1'").arg(w.pResult[0]->text()));

    check(selectComboItem(w.pMode, "Binary"), "switch to Binary");
    check(w.pResult[0]->text().contains(QRegularExpression("^[01]+$")), "result 1 renders as binary digits",
          QString("got '%1'").arg(w.pResult[0]->text()));

    check(selectComboItem(w.pMode, "HEX"), "switch back to HEX");
    check(w.pResult[0]->text().toULongLong(0, 16) == 0x30, "result 1 is back to 30", QString("got '%1'").arg(w.pResult[0]->text()));
}

// A .qm that compiles is not the same as a .qm that reaches the screen. Install one
// and build a fresh window with it, then look for the translated strings.
void checkTranslations(const QString &sTranslationDirectory)
{
    std::printf("[translations]\n");

    struct Expectation {
        const char *pszLanguage;
        const char *pszResultGroup;   // "Result"
        const char *pszTwoOperands;   // "Two operands"
        const char *pszExit;          // "Exit"
    };

    // One representative per script family, so a broken encoding shows up.
    const Expectation expectations[] = {
        {"de", "Ergebnis", "Zwei Operanden", "Beenden"},
        {"ru", "\xd0\xa0\xd0\xb5\xd0\xb7\xd1\x83\xd0\xbb\xd1\x8c\xd1\x82\xd0\xb0\xd1\x82", "\xd0\x94\xd0\xb2\xd0\xb0 \xd0\xbe\xd0\xbf\xd0\xb5\xd1\x80\xd0\xb0\xd0\xbd\xd0\xb4\xd0\xb0", "\xd0\x92\xd1\x8b\xd1\x85\xd0\xbe\xd0\xb4"},
        {"zh", "\xe7\xbb\x93\xe6\x9e\x9c", "\xe5\x8f\x8c\xe6\x93\x8d\xe4\xbd\x9c\xe6\x95\xb0", "\xe9\x80\x80\xe5\x87\xba"},
    };

    for (const Expectation &expectation : expectations) {
        const QString sPath = QString("%1/xocalc_%2.qm").arg(sTranslationDirectory, QString::fromUtf8(expectation.pszLanguage));

        if (!QFileInfo::exists(sPath)) {
            check(false, QString("found %1").arg(sPath), "build the project first");
            continue;
        }

        QTranslator translator;
        if (!translator.load(sPath)) {
            check(false, QString("loaded %1").arg(sPath));
            continue;
        }

        QCoreApplication::installTranslator(&translator);

        // Retranslation happens in setupUi(), so the window has to be built after
        // the translator is installed.
        GuiMainWindow translatedWindow;
        translatedWindow.show();
        QCoreApplication::processEvents();

        QGroupBox *pResultGroup = translatedWindow.findChild<QGroupBox *>("groupBoxResult");
        QComboBox *pGroupCombo = translatedWindow.findChild<QComboBox *>("comboBoxOpcodeGroup");
        QPushButton *pExitButton = translatedWindow.findChild<QPushButton *>("pushButtonExit");

        const QString sLanguage = QString::fromUtf8(expectation.pszLanguage);

        if (pResultGroup) {
            check(pResultGroup->title() == QString::fromUtf8(expectation.pszResultGroup), QString("%1: the Result group is translated").arg(sLanguage),
                  QString("got '%1'").arg(pResultGroup->title()));
        }
        if (pGroupCombo) {
            check(pGroupCombo->itemText(0) == QString::fromUtf8(expectation.pszTwoOperands), QString("%1: the opcode groups are translated").arg(sLanguage),
                  QString("got '%1'").arg(pGroupCombo->itemText(0)));
        }
        if (pExitButton) {
            check(pExitButton->text() == QString::fromUtf8(expectation.pszExit), QString("%1: the buttons are translated").arg(sLanguage),
                  QString("got '%1'").arg(pExitButton->text()));
        }

        QCoreApplication::removeTranslator(&translator);
    }

    // Every shipped .qm, not just the three sampled above: a translation that loses
    // its %1 makes QString::arg() silently drop the register name from the #DE
    // message, and one that loses the "#DE"/"#UD" prefix hides which fault it was.
    // Checked against the compiled .qm rather than the .ts, because the .qm is what
    // actually ships.
    struct PlaceholderString {
        const char *pszContext;
        const char *pszSource;
        const char *pszMustContain;
    };

    const PlaceholderString placeholderStrings[] = {
        {"GuiMainWindow", "#UD: this CPU does not implement %1", "#UD"},
        {"GuiMainWindow", "#DE: divide error - the quotient does not fit into %1", "#DE"},
        {"GuiMainWindow", "#DE: divide error - the divisor is zero", "#DE"},
    };

    const QStringList qmFiles = QDir(sTranslationDirectory).entryList(QStringList() << "xocalc_*.qm", QDir::Files, QDir::Name);
    check(qmFiles.count() == 22, "all 22 languages are compiled", QString("found %1").arg(qmFiles.count()));

    qint32 nBadPlaceholders = 0;
    qint32 nBadPrefixes = 0;
    qint32 nMissing = 0;

    for (const QString &sFile : qmFiles) {
        QTranslator translator;
        if (!translator.load(sTranslationDirectory + QLatin1Char('/') + sFile)) {
            check(false, QString("loaded %1").arg(sFile));
            continue;
        }
        QCoreApplication::installTranslator(&translator);

        for (const PlaceholderString &entry : placeholderStrings) {
            const QString sTranslated = QCoreApplication::translate(entry.pszContext, entry.pszSource);

            if (sTranslated == QString::fromUtf8(entry.pszSource)) {
                ++nMissing;
                std::printf("        %s: %s is untranslated\n", sFile.toUtf8().constData(), entry.pszSource);
                continue;
            }

            const QString sSource = QString::fromUtf8(entry.pszSource);
            if (sSource.count(QStringLiteral("%1")) != sTranslated.count(QStringLiteral("%1"))) {
                ++nBadPlaceholders;
                std::printf("        %s: %%1 count differs for '%s' -> '%s'\n", sFile.toUtf8().constData(), entry.pszSource,
                            sTranslated.toUtf8().constData());
            }
            if (!sTranslated.contains(QString::fromUtf8(entry.pszMustContain))) {
                ++nBadPrefixes;
                std::printf("        %s: lost '%s' in '%s'\n", sFile.toUtf8().constData(), entry.pszMustContain,
                            sTranslated.toUtf8().constData());
            }
        }

        QCoreApplication::removeTranslator(&translator);
    }

    check(nMissing == 0, "every language translates the fault messages", QString("%1 untranslated").arg(nMissing));
    check(nBadPlaceholders == 0, "every language preserves the %1 placeholder", QString("%1 broken").arg(nBadPlaceholders));
    check(nBadPrefixes == 0, "every language keeps the #DE / #UD mnemonic", QString("%1 broken").arg(nBadPrefixes));

    // Every shipped catalogue must be reachable from a system locale of its own
    // language. A .qm named after a tag Qt cannot match is dead weight: ID_VIEW_LANG
    // defaults to "System", which loads via QTranslator::load(QLocale::system(), ...),
    // and that walks QLocale::uiLanguages(). "ar_AR", "fa_FA" and "bn_BN" matched
    // nothing at all - AR is Argentina, FA is not a country code, BN is Brunei - so
    // Arabic, Persian and Bengali users got an English UI with the right translation
    // sitting unused in lang/.
    qint32 nUnreachable = 0;

    for (const QString &sFile : qmFiles) {
        // "xocalc_pt_BR.qm" -> "pt_BR"
        QString sTag = sFile;
        sTag.remove(QStringLiteral("xocalc_"));
        sTag.chop(QStringLiteral(".qm").length());

        QTranslator probe;
        const bool bLoaded = probe.load(QLocale(sTag), QStringLiteral("xocalc"), QStringLiteral("_"), sTranslationDirectory, QStringLiteral(".qm"));
        const QString sPicked = bLoaded ? probe.filePath().section(QLatin1Char('/'), -1) : QString();

        if (sPicked != sFile) {
            ++nUnreachable;
            std::printf("        %s is not what QLocale(\"%s\") resolves to (got '%s')\n", sFile.toUtf8().constData(), sTag.toUtf8().constData(),
                        bLoaded ? sPicked.toUtf8().constData() : "nothing");
        }
    }

    check(nUnreachable == 0, "every catalogue is reachable from its own locale", QString("%1 unreachable").arg(nUnreachable));

    // And the languages whose tags were wrong must now resolve from the territories a
    // real machine actually reports, not just from the tag itself.
    struct SystemLocale {
        const char *pszSystem;
        const char *pszExpected;
    };

    const SystemLocale systemLocales[] = {
        // The three tags that used to match nothing at all.
        {"ar_EG", "xocalc_ar.qm"}, {"ar_SA", "xocalc_ar.qm"},
        {"fa_IR", "xocalc_fa.qm"}, {"fa_AF", "xocalc_fa.qm"},
        {"bn_BD", "xocalc_bn.qm"}, {"bn_IN", "xocalc_bn.qm"},
        // A bare catalogue serves every territory of its language. de_AT and de_CH
        // used to get nothing at all, because only de_DE existed.
        {"de_DE", "xocalc_de.qm"}, {"de_AT", "xocalc_de.qm"}, {"de_CH", "xocalc_de.qm"},
        {"es_ES", "xocalc_es.qm"}, {"es_MX", "xocalc_es.qm"},
        {"fr_FR", "xocalc_fr.qm"}, {"fr_CA", "xocalc_fr.qm"},
        {"ru_RU", "xocalc_ru.qm"}, {"he_IL", "xocalc_he.qm"},
        // ...and the territory-qualified exceptions still win over the bare one.
        {"pt_BR", "xocalc_pt_BR.qm"}, {"pt_PT", "xocalc_pt_PT.qm"},
        {"zh_CN", "xocalc_zh.qm"}, {"zh_TW", "xocalc_zh_TW.qm"},
    };

    for (const SystemLocale &systemLocale : systemLocales) {
        QTranslator probe;
        const bool bLoaded =
            probe.load(QLocale(QString::fromUtf8(systemLocale.pszSystem)), QStringLiteral("xocalc"), QStringLiteral("_"), sTranslationDirectory,
                       QStringLiteral(".qm"));
        const QString sPicked = bLoaded ? probe.filePath().section(QLatin1Char('/'), -1) : QStringLiteral("nothing");

        check(sPicked == QString::fromUtf8(systemLocale.pszExpected),
              QString("a %1 system loads %2").arg(QString::fromUtf8(systemLocale.pszSystem), QString::fromUtf8(systemLocale.pszExpected)),
              QString("got '%1'").arg(sPicked));
    }

    // Layout direction, asserted on the property Qt actually exposes rather than on
    // a translation key.
    //
    // The earlier version of this check asked
    // QCoreApplication::translate("QCoreApplication", "QT_LAYOUT_DIRECTION"), which
    // was self-referential twice over: lupdate had scraped that very literal out of
    // this test file into the .ts, so the check only read back a string the test
    // itself had caused to exist - and the context was wrong anyway. Qt calls
    // QGuiApplication::tr(), i.e. context "QGuiApplication", so the real UI never
    // flipped while this check reported "ok".
    //
    // Qt applies the direction when it handles a LanguageChange event. installTranslator()
    // only sends one once the event loop is running (QCoreApplicationPrivate::is_app_running),
    // and this test never calls exec(), so send it the same way QGuiApplication would.
    struct DirectionCase {
        const char *pszLanguage;
        Qt::LayoutDirection direction;
    };

    const DirectionCase directionCases[] = {
        {"ar", Qt::RightToLeft}, {"he", Qt::RightToLeft}, {"fa", Qt::RightToLeft},
        {"de", Qt::LeftToRight}, {"ru", Qt::LeftToRight}, {"zh", Qt::LeftToRight},
    };

    for (const DirectionCase &directionCase : directionCases) {
        const QString sLanguage = QString::fromUtf8(directionCase.pszLanguage);
        const QString sPath = QString("%1/xocalc_%2.qm").arg(sTranslationDirectory, sLanguage);

        QTranslator translator;
        if (!translator.load(sPath)) {
            check(false, QString("loaded %1").arg(sPath));
            continue;
        }

        QCoreApplication::installTranslator(&translator);
        QEvent languageChange(QEvent::LanguageChange);
        QCoreApplication::sendEvent(QCoreApplication::instance(), &languageChange);

        const bool bRightToLeft = (QGuiApplication::layoutDirection() == Qt::RightToLeft);
        const bool bWantRightToLeft = (directionCase.direction == Qt::RightToLeft);

        check(bRightToLeft == bWantRightToLeft,
              QString("%1 lays out %2").arg(sLanguage, bWantRightToLeft ? "right-to-left" : "left-to-right"),
              QString("QGuiApplication::layoutDirection() is %1").arg(bRightToLeft ? "RightToLeft" : "LeftToRight"));

        QCoreApplication::removeTranslator(&translator);
        QCoreApplication::sendEvent(QCoreApplication::instance(), &languageChange);
    }

    // Back to the default for whatever runs after this.
    QGuiApplication::setLayoutDirection(Qt::LeftToRight);
}

// The Options dialog is the only way a user can pick a language. XOptions only
// installs a translator when ID_VIEW_LANG is registered, and the dialog only lets
// you choose one if it binds a combo to it - both were missing.
// A row whose widgets are all hidden must occupy no height. QSpacerItem's
// cross-axis size is policy-independent - QSpacerItem::sizeHint() returns the
// declared QSize whatever the size policy says - and QBoxLayout never drops a
// spacer item the way it drops a hidden widget. So the four row spacers, each
// declared 40x20 by Designer's default, kept exactly 20px of dead height on
// every row the opcode had no use for. Assert the layout rather than pixels:
// the .ui now declares those spacers with sizeHint height 0.
void checkHiddenRowsCollapse(GuiMainWindow *pWindow, const Widgets &w)
{
    std::printf("[hidden rows collapse]\n");

    struct Row {
        const char *pszLayout;
        bool bResult;
        qint32 nBox0;
        qint32 nBox1;
    };

    const Row rows[4] = {
        {"horizontalLayout_2", false, 0, 1},
        {"horizontalLayout_9", false, 2, 3},
        {"horizontalLayout_6", true, 0, 1},
        {"horizontalLayout_8", true, 2, 3},
    };

    qint32 nCollapsed = 0;
    qint32 nExpanded = 0;
    bool bAllCollapse = true;
    bool bAllExpand = true;
    QString sWorst;

    for (qint32 nRow = 0; nRow < 4; ++nRow) {
        QHBoxLayout *pLayout = pWindow->findChild<QHBoxLayout *>(QString::fromUtf8(rows[nRow].pszLayout));
        if (!pLayout) {
            check(false, QString("row layout %1 exists").arg(rows[nRow].pszLayout));
            continue;
        }

        for (qint32 nGroup = 0; nGroup < w.pGroup->count(); ++nGroup) {
            w.pGroup->setCurrentIndex(nGroup);

            for (qint32 nOpcode = 0; nOpcode < w.pOpcode->count(); ++nOpcode) {
                w.pOpcode->setCurrentIndex(nOpcode);

                QGroupBox *pBox0 = rows[nRow].bResult ? w.pResultBox[rows[nRow].nBox0] : w.pOperandBox[rows[nRow].nBox0];
                QGroupBox *pBox1 = rows[nRow].bResult ? w.pResultBox[rows[nRow].nBox1] : w.pOperandBox[rows[nRow].nBox1];

                const bool bRowEmpty = pBox0->isHidden() && pBox1->isHidden();

                // A nested layout caches its size hint and is NOT invalidated when a
                // grandchild is hidden - only the parent widget's top-level layout is.
                // Without this the reading is whatever the row measured when it was
                // last visible (45px, the height of a populated operand box).
                pLayout->invalidate();
                const qint32 nHeight = pLayout->sizeHint().height();

                if (bRowEmpty) {
                    ++nCollapsed;
                    if (nHeight != 0) {
                        if (bAllCollapse) {
                            sWorst = QString("%1 leaves %2px on %3").arg(rows[nRow].pszLayout).arg(nHeight).arg(w.pOpcode->currentText());
                        }
                        bAllCollapse = false;
                    }
                } else {
                    ++nExpanded;
                    if (nHeight <= 0) {
                        bAllExpand = false;
                    }
                }
            }
        }
    }

    check(bAllCollapse, QString("a fully hidden row takes 0px (%1 cases)").arg(nCollapsed), sWorst);
    check(bAllExpand, QString("a row with a visible box still has height (%1 cases)").arg(nExpanded));
    check(nCollapsed > 0, "there is at least one hidden row to collapse");
}

// Tab order is declared in the .ui rather than left to widget creation order.
// The implicit order happened to be right, but nothing pinned it: reordering
// anything in Designer would have changed keyboard navigation silently.
void checkTabOrder(GuiMainWindow *pWindow)
{
    std::printf("[tab order]\n");

    const char *pszExpected[] = {"comboBoxOpcodeGroup",
                                 "comboBoxMode",
                                 "comboBoxOpcode",
                                 "toolButtonOpcode",
                                 "lineEditOperand1",
                                 "lineEditOperand2",
                                 "lineEditOperand3",
                                 "lineEditOperand4",
                                 "pushButtonFlagCF",
                                 "pushButtonFlagPF",
                                 "pushButtonFlagAF",
                                 "pushButtonFlagZF",
                                 "pushButtonFlagSF",
                                 "pushButtonFlagOF",
                                 "lineEditFlagsBefore",
                                 "lineEditResult1",
                                 "lineEditResult2",
                                 "lineEditResult3",
                                 "lineEditResult4",
                                 "lineEditFlagsAfter",
                                 "pushButtonOptions",
                                 "pushButtonCopy",
                                 "pushButtonAbout",
                                 "pushButtonExit"};
    const qint32 nExpected = (qint32)(sizeof(pszExpected) / sizeof(pszExpected[0]));

    QStringList listExpected;
    bool bAllExist = true;
    for (qint32 i = 0; i < nExpected; ++i) {
        const QString sName = QString::fromUtf8(pszExpected[i]);
        listExpected.append(sName);
        if (!pWindow->findChild<QWidget *>(sName)) {
            bAllExist = false;
            std::printf("        no such widget: %s\n", pszExpected[i]);
        }
    }
    check(bAllExist, "every declared tabstop names a real widget");

    // Walk the focus chain, which includes hidden widgets, and keep only the
    // widgets we pinned. That is the order Qt will actually use.
    QStringList listActual;
    QWidget *pCurrent = pWindow;
    for (qint32 nStep = 0; nStep < 4096; ++nStep) {
        pCurrent = pCurrent->nextInFocusChain();
        if (!pCurrent || (pCurrent == pWindow)) {
            break;
        }
        const QString sName = pCurrent->objectName();
        if (listExpected.contains(sName) && !listActual.contains(sName)) {
            listActual.append(sName);
        }
    }

    check(listActual == listExpected, "the focus chain follows the declared tab order",
          QString("got %1").arg(listActual.join(QStringLiteral(","))));
}

void checkOptionsDialog()
{
    std::printf("[options dialog]\n");

    XOptions options;
    options.setName(QStringLiteral(X_OPTIONSFILE));
    options.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    options.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    options.addID(XOptions::ID_VIEW_STAYONTOP, false);
    options.load();

    check(options.isIDPresent(XOptions::ID_VIEW_LANG), "the language option is registered");

    DialogOptions dialog(nullptr, &options);

    QComboBox *pLangCombo = dialog.findChild<QComboBox *>("comboBoxLang");
    QComboBox *pStyleCombo = dialog.findChild<QComboBox *>("comboBoxStyle");

    check(pLangCombo != nullptr, "the Options dialog has a language selector");
    check(pStyleCombo != nullptr, "the Options dialog still has a style selector");

    if (pLangCombo) {
        // XOptions always offers English and System, then one entry per installed .qm.
        check(pLangCombo->count() >= 2, "the language selector is populated", QString("got %1 items").arg(pLangCombo->count()));
        check(pLangCombo->findText(QStringLiteral("English")) >= 0, "the language selector offers English");
        check(pLangCombo->findData(QStringLiteral("System")) >= 0, "the language selector offers System");
        std::printf("  info  language selector holds %d entries\n", pLangCombo->count());
    }

    // A configured value the combo cannot offer - a language whose .qm is not
    // installed on this machine - must survive opening the dialog and pressing OK.
    // It used to be silently rewritten to whatever sat at index 0 ("English", an
    // empty value), so merely visiting Options destroyed the setting.
    XOptions preset;
    preset.setName(QStringLiteral(X_OPTIONSFILE));
    preset.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    preset.setValue(XOptions::ID_VIEW_LANG, QStringLiteral("xocalc_de_DE"));

    DialogOptions presetDialog(nullptr, &preset);
    QComboBox *pPresetCombo = presetDialog.findChild<QComboBox *>("comboBoxLang");

    if (pPresetCombo) {
        check(pPresetCombo->currentData().toString() == QStringLiteral("xocalc_de_DE"), "an unavailable configured language is still offered",
              QString("combo shows '%1'").arg(pPresetCombo->currentData().toString()));

        preset.getComboBox(pPresetCombo, XOptions::ID_VIEW_LANG);
        check(preset.getValue(XOptions::ID_VIEW_LANG).toString() == QStringLiteral("xocalc_de_DE"), "pressing OK does not erase it",
              QString("became '%1'").arg(preset.getValue(XOptions::ID_VIEW_LANG).toString()));
    }
}

// Render the window to a PNG. QWidget::grab() paints through the same paint path
// the visible window uses, so this is a faithful picture even on the offscreen
// platform - and unlike driving the real window over UIAutomation, it cannot flake.
void saveShot(GuiMainWindow *pWindow, const QString &sDirectory, const QString &sName)
{
    QDir().mkpath(sDirectory);
    const QString sPath = sDirectory + QLatin1Char('/') + sName + QStringLiteral(".png");
    const QPixmap pixmap = pWindow->grab();

    if (pixmap.save(sPath)) {
        std::printf("  shot  %s (%dx%d)\n", sPath.toUtf8().constData(), pixmap.width(), pixmap.height());
    } else {
        check(false, QString("saved screenshot %1").arg(sPath));
    }
}

// Drive the window into the states worth looking at and write one PNG each.
void captureShots(GuiMainWindow *pWindow, const Widgets &w, const QString &sDirectory)
{
    std::printf("[screenshots]\n");

    selectComboItem(w.pMode, "HEX");
    selectComboItem(w.pGroup, "Two operands");
    selectComboItem(w.pOpcode, "add");
    clearAndType(w.pFlagsBefore, "0");
    clearAndType(w.pOperand[0], "DEADBEEF12345678");
    clearAndType(w.pOperand[1], "1");
    QCoreApplication::processEvents();
    saveShot(pWindow, sDirectory, "state-add");

    selectComboItem(w.pGroup, "Mul/Div");
    selectComboItem(w.pOpcode, "div");
    clearAndType(w.pOperand[0], "64");
    clearAndType(w.pOperand[2], "0");
    clearAndType(w.pOperand[1], "0");
    QCoreApplication::processEvents();
    saveShot(pWindow, sDirectory, "state-divide-by-zero");

    clearAndType(w.pOperand[1], "2");
    clearAndType(w.pOperand[2], "FF");
    QCoreApplication::processEvents();
    saveShot(pWindow, sDirectory, "state-quotient-overflow");

    clearAndType(w.pOperand[2], "0");
    clearAndType(w.pOperand[1], "A");
    QCoreApplication::processEvents();
    saveShot(pWindow, sDirectory, "state-divide-ok");

    selectComboItem(w.pMode, "Signed");
    QCoreApplication::processEvents();
    saveShot(pWindow, sDirectory, "state-signed-mode");

    // Only a 32-bit build has this group at all.
    selectComboItem(w.pMode, "HEX");
    if (selectComboItem(w.pGroup, "BCD")) {
        selectComboItem(w.pOpcode, "aam");
        clearAndType(w.pOperand[0], "F");
        QCoreApplication::processEvents();
        saveShot(pWindow, sDirectory, "state-bcd-aam");
    }

    selectComboItem(w.pMode, "HEX");
}

}  // namespace

int main(int argc, char *argv[])
{
    // Headless by default; QT_QPA_PLATFORM=windows gives a real window (and real
    // fonts) when the --shot images need to be readable.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    QApplication application(argc, argv);
    QCoreApplication::setOrganizationName(X_ORGANIZATIONNAME);
    QCoreApplication::setApplicationName(X_APPLICATIONNAME);

    GuiMainWindow window;
    window.show();
    QCoreApplication::processEvents();

    Widgets widgets;
    std::memset(&widgets, 0, sizeof(widgets));

    if (!collect(&window, &widgets)) {
        std::printf("FAILED: could not find the expected widgets\n");
        return 1;
    }

    // --shot <dir>: also write PNGs of a few interesting states, for eyeballing.
    QString sShotDirectory;
    for (qint32 i = 1; i < argc; ++i) {
        if ((QString::fromUtf8(argv[i]) == QStringLiteral("--shot")) && ((i + 1) < argc)) {
            sShotDirectory = QString::fromUtf8(argv[i + 1]);
        }
    }

    checkStartup(&window, widgets);
    checkAllOpcodesReachable(widgets);
    checkOperandInput(widgets);
    checkRegisterWidths(widgets);
    checkSignedInput(widgets);
    checkKnownAnswers(widgets);
    checkBcdGroup(widgets);
    checkDivisionGuard(&window, widgets);
    checkFlagRoundTrip(&window, widgets);
    checkCopyReport(widgets);
    checkModeSwitchKeepsValues(widgets);
    checkHiddenRowsCollapse(&window, widgets);
    checkTabOrder(&window);

    checkOptionsDialog();

    // The .qm files sit next to the test binary's sibling translations directory.
    checkTranslations(QCoreApplication::applicationDirPath() + QStringLiteral("/../translations"));

    if (!sShotDirectory.isEmpty()) {
        captureShots(&window, widgets, sShotDirectory);
    }

    std::printf("%s: %lld checks, %lld failures\n", g_nFailures ? "FAILED" : "OK", (long long)g_nChecks, (long long)g_nFailures);

    return g_nFailures ? 1 : 0;
}
