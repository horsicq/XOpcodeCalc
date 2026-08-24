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
#include "guimainwindow.h"

#include "ui_guimainwindow.h"

#include <QAbstractButton>
#include <QApplication>
#include <QClipboard>
#include <QVBoxLayout>
#include <array>
#include <utility>

namespace {

constexpr XVALUE kSupportedFlagsMask =
    ASM_DEF::AF | ASM_DEF::CF | ASM_DEF::OF | ASM_DEF::PF | ASM_DEF::SF | ASM_DEF::ZF;
constexpr XVALUE kHiddenFlagsMask = 0x0002 | 0x0200;  // EFLAGS reserved bit 1 (always set) and IF

template <size_t N>
qint32 opcodeRecordCount(const ASM_DEF::OPCODE_RECORD (&)[N])
{
    return static_cast<qint32>(N);
}

}  // namespace

GuiMainWindow::GuiMainWindow(QWidget *pParent) : QMainWindow(pParent), ui(new Ui::GuiMainWindow), m_pLabelStatus(nullptr)
{
    ui->setupUi(this);
    initializeUi();
}

GuiMainWindow::~GuiMainWindow()
{
    m_options.save();

    delete ui;
}

void GuiMainWindow::initializeUi()
{
    setWindowTitle(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));

    // Sits directly under the result values and explains why they are blank. It is
    // created here rather than in the .ui so it can stay next to the code that fills
    // it; it is always visible with empty text, so the layout never jumps.
    m_pLabelStatus = new QLabel(this);
    m_pLabelStatus->setObjectName(QStringLiteral("labelStatus"));
    m_pLabelStatus->setAlignment(Qt::AlignCenter);
    m_pLabelStatus->setWordWrap(true);
    m_pLabelStatus->setStyleSheet(QStringLiteral("QLabel { color: #c42b1c; font-weight: bold; }"));

    QVBoxLayout *pResultLayout = qobject_cast<QVBoxLayout *>(ui->groupBoxResult->layout());
    if (pResultLayout) {
        pResultLayout->insertWidget(1, m_pLabelStatus);
    }

    QFont opcodeFont = ui->toolButtonOpcode->font();
    opcodeFont.setPointSizeF(opcodeFont.pointSizeF() * 1.5);
    opcodeFont.setBold(true);
    ui->toolButtonOpcode->setFont(opcodeFont);

    const QList<XLineEditHEX *> outputs = resultEditors();
    for (XLineEditHEX *editor : outputs) {
        editor->setReadOnly(true);
    }
    ui->lineEditFlagsAfter->setReadOnly(true);

    m_options.setName(X_OPTIONSFILE);
    m_options.addID(XOptions::ID_VIEW_STYLE, QStringLiteral("Fusion"));
    m_options.addID(XOptions::ID_VIEW_LANG, QStringLiteral("System"));
    m_options.addID(XOptions::ID_VIEW_STAYONTOP, false);
    m_options.load();

    adjustWindow();
    initializeOpcodeGroups();
    initializeModes();

    on_comboBoxOpcodeGroup_currentIndexChanged(ui->comboBoxOpcodeGroup->currentIndex());
    on_comboBoxMode_currentIndexChanged(ui->comboBoxMode->currentIndex());
}

void GuiMainWindow::initializeOpcodeGroups()
{
    ui->comboBoxOpcodeGroup->addItem(tr("Two operands"), OG_TWOOPERANDS);
    ui->comboBoxOpcodeGroup->addItem(tr("One operand"), OG_ONEOPERAND);
    ui->comboBoxOpcodeGroup->addItem(tr("Mul/Div"), OG_MULDIV);
    ui->comboBoxOpcodeGroup->addItem(tr("Shift"), OG_SHIFT);
    ui->comboBoxOpcodeGroup->addItem(tr("Bits"), OG_BITS);
#ifndef OPCODE64
    ui->comboBoxOpcodeGroup->addItem(tr("BCD"), OG_BCD);
#endif
    ui->comboBoxOpcodeGroup->addItem(tr("Special"), OG_SPECIAL);
}

void GuiMainWindow::initializeModes()
{
    ui->comboBoxMode->addItem(tr("HEX"), XLineEditHEX::_MODE_HEX);
    ui->comboBoxMode->addItem(tr("Signed"), XLineEditHEX::_MODE_SIGN_DEC);
    ui->comboBoxMode->addItem(tr("Unsigned"), XLineEditHEX::_MODE_DEC);
    ui->comboBoxMode->addItem(tr("Binary"), XLineEditHEX::_MODE_BIN);
}

void GuiMainWindow::on_pushButtonExit_clicked()
{
    close();
}

void GuiMainWindow::on_pushButtonAbout_clicked()
{
    DialogAbout dialog(this);
    dialog.exec();
}

void GuiMainWindow::adjustWindow()
{
    m_options.adjustStayOnTop(this);
}

ASM_DEF::OPCODE_RECORD GuiMainWindow::currentOpcodeRecord() const
{
    const auto opcode = static_cast<ASM_DEF::OP>(ui->comboBoxOpcode->currentData().toInt());
    return m_opcodeMap.value(opcode);
}

QList<XLineEditHEX *> GuiMainWindow::resultEditors() const
{
    return {ui->lineEditResult1, ui->lineEditResult2, ui->lineEditResult3, ui->lineEditResult4};
}

QList<XLineEditHEX *> GuiMainWindow::operandEditors() const
{
    return {ui->lineEditOperand1, ui->lineEditOperand2, ui->lineEditOperand3, ui->lineEditOperand4};
}

XLineEditHEX::_MODE GuiMainWindow::currentDisplayMode() const
{
    const XLineEditHEX::_MODE mode = static_cast<XLineEditHEX::_MODE>(ui->comboBoxMode->currentData().toInt());

    // Before the combo box is populated there is no selection to read.
    return (mode == XLineEditHEX::_MODE_UNKNOWN) ? XLineEditHEX::_MODE_HEX : mode;
}

// Width of the register a field stands for, taken from the mask the opcode table
// declares for it: CL -> 8, CX -> 16, EAX -> 32, RAX -> 64.
qint32 GuiMainWindow::registerBits(quint64 nMaxValue)
{
    if (nMaxValue <= 0xFFull) {
        return 8;
    } else if (nMaxValue <= 0xFFFFull) {
        return 16;
    } else if (nMaxValue <= 0xFFFFFFFFull) {
        return 32;
    }

    return 64;
}

XLineEditValidator::MODE GuiMainWindow::validatorModeFor(XLineEditHEX::_MODE displayMode, qint32 nBits)
{
    if (displayMode == XLineEditHEX::_MODE_DEC) {
        if (nBits == 8) return XLineEditValidator::MODE_DEC_8;
        if (nBits == 16) return XLineEditValidator::MODE_DEC_16;
        if (nBits == 32) return XLineEditValidator::MODE_DEC_32;
        return XLineEditValidator::MODE_DEC_64;
    } else if (displayMode == XLineEditHEX::_MODE_SIGN_DEC) {
        if (nBits == 8) return XLineEditValidator::MODE_SIGN_DEC_8;
        if (nBits == 16) return XLineEditValidator::MODE_SIGN_DEC_16;
        if (nBits == 32) return XLineEditValidator::MODE_SIGN_DEC_32;
        return XLineEditValidator::MODE_SIGN_DEC_64;
    } else if (displayMode == XLineEditHEX::_MODE_BIN) {
        if (nBits == 8) return XLineEditValidator::MODE_BIN_8;
        if (nBits == 16) return XLineEditValidator::MODE_BIN_16;
        if (nBits == 32) return XLineEditValidator::MODE_BIN_32;
        return XLineEditValidator::MODE_BIN_64;
    }

    if (nBits == 8) return XLineEditValidator::MODE_HEX_8;
    if (nBits == 16) return XLineEditValidator::MODE_HEX_16;
    if (nBits == 32) return XLineEditValidator::MODE_HEX_32;
    return XLineEditValidator::MODE_HEX_64;
}

// XLineEditHEX derives its digit count from the validator mode it is already in,
// and nothing else in the app ever promotes an operand field past the 32-bit
// default it is constructed with. Pin both the width and the accepted range here,
// from the register the opcode table names for this slot.
void GuiMainWindow::applyEditorWidth(XLineEditHEX *pEditor, quint64 nMaxValue)
{
    // A hidden slot still needs a usable validator if it is shown again later.
    const quint64 nEffectiveMax = nMaxValue ? nMaxValue : static_cast<quint64>(~XVALUE(0));

    pEditor->setMaxValue(nEffectiveMax);
    pEditor->setValidatorMode(validatorModeFor(currentDisplayMode(), registerBits(nEffectiveMax)));
}

// Render a value at the width the field is pinned to, instead of letting the
// magnitude of the value pick the width.
void GuiMainWindow::setEditorValue(XLineEditHEX *pEditor, XVALUE nValue)
{
    switch (XLineEditValidator::getNumberOfBits(pEditor->getValidatorMode())) {
        case 8: pEditor->setValue_uint8(static_cast<quint8>(nValue), XLineEditHEX::_MODE_UNKNOWN); break;
        case 16: pEditor->setValue_uint16(static_cast<quint16>(nValue), XLineEditHEX::_MODE_UNKNOWN); break;
        case 32: pEditor->setValue_uint32(static_cast<quint32>(nValue), XLineEditHEX::_MODE_UNKNOWN); break;
        default: pEditor->setValue_uint64(static_cast<quint64>(nValue), XLineEditHEX::_MODE_UNKNOWN); break;
    }
}

void GuiMainWindow::clearResultEditors()
{
    const QList<XLineEditHEX *> editors = resultEditors();
    for (XLineEditHEX *editor : editors) {
        editor->clear();
    }
}

RECDATA GuiMainWindow::createInputData() const
{
    RECDATA data = RECDATA_INIT;

    data.OPERAND[0] = static_cast<XVALUE>(ui->lineEditOperand1->getValue_uint64());
    data.OPERAND[1] = static_cast<XVALUE>(ui->lineEditOperand2->getValue_uint64());
    data.OPERAND[2] = static_cast<XVALUE>(ui->lineEditOperand3->getValue_uint64());
    data.OPERAND[3] = static_cast<XVALUE>(ui->lineEditOperand4->getValue_uint64());
    data.FLAG[0] = static_cast<XVALUE>(ui->lineEditFlagsBefore->getValue_uint64()) & kSupportedFlagsMask;

    return data;
}

bool GuiMainWindow::canExecuteOpcode(const ASM_DEF::OPCODE_RECORD &record, const RECDATA &data) const
{
    if (!CpuFeatures::isSupported(record.feature)) {
        return false;  // the instruction would raise #UD on this CPU
    }

    if ((record.opcode != ASM_DEF::OP_DIV) && (record.opcode != ASM_DEF::OP_IDIV)) {
        return true;
    }

    return validateDivision(record, data);
}

bool GuiMainWindow::validateDivision(const ASM_DEF::OPCODE_RECORD &record, const RECDATA &data) const
{
    // OPERAND[0] = EAX/RAX (low), OPERAND[1] = ECX/RCX (divisor), OPERAND[2] = EDX/RDX (high)
    return DivisionCheck::isDivisionDefined(record.opcode == ASM_DEF::OP_IDIV, data.OPERAND[0], data.OPERAND[1], data.OPERAND[2]);
}

void GuiMainWindow::applyResultData(const ASM_DEF::OPCODE_RECORD &record, RECDATA *data, const bool success)
{
    if (success) {
        std::array<XVALUE, 4> resultValues = {
            static_cast<XVALUE>(data->RESULT[0]),
            static_cast<XVALUE>(data->RESULT[1]),
            static_cast<XVALUE>(data->RESULT[2]),
            static_cast<XVALUE>(data->RESULT[3]),
        };

        if ((record.opcode == ASM_DEF::OP_XADD) || (record.opcode == ASM_DEF::OP_XCHG) || (record.opcode == ASM_DEF::OP_CMPXCHG)) {
            std::swap(resultValues[1], resultValues[2]);
        }

        const QList<XLineEditHEX *> editors = resultEditors();
        for (int index = 0; index < editors.count(); ++index) {
            setEditorValue(editors.at(index), resultValues.at(index));
        }
    } else {
        clearResultEditors();
        data->FLAG[1] = data->FLAG[0];
    }
}

void GuiMainWindow::updateFlagButtons(const XVALUE flags)
{
    struct FlagButtonBinding {
        QAbstractButton *button;
        XVALUE mask;
    };

    const std::array<FlagButtonBinding, 6> bindings = {{
        {ui->pushButtonFlagAF, ASM_DEF::AF},
        {ui->pushButtonFlagCF, ASM_DEF::CF},
        {ui->pushButtonFlagOF, ASM_DEF::OF},
        {ui->pushButtonFlagPF, ASM_DEF::PF},
        {ui->pushButtonFlagSF, ASM_DEF::SF},
        {ui->pushButtonFlagZF, ASM_DEF::ZF},
    }};

    // Restore whatever each button was blocking before, not an assumed false.
    std::array<bool, 6> wasBlocked = {};

    for (qint32 i = 0; i < 6; ++i) {
        wasBlocked[i] = bindings.at(i).button->blockSignals(true);
    }

    for (const FlagButtonBinding &binding : bindings) {
        binding.button->setChecked(flags & binding.mask);
    }

    for (qint32 i = 0; i < 6; ++i) {
        bindings.at(i).button->blockSignals(wasBlocked[i]);
    }
}

void GuiMainWindow::updateFlagLabels(const XVALUE flags)
{
    struct FlagLabelBinding {
        QWidget *label;
        XVALUE mask;
    };

    const std::array<FlagLabelBinding, 6> bindings = {{
        {ui->labelFlagAF, ASM_DEF::AF},
        {ui->labelFlagCF, ASM_DEF::CF},
        {ui->labelFlagOF, ASM_DEF::OF},
        {ui->labelFlagPF, ASM_DEF::PF},
        {ui->labelFlagSF, ASM_DEF::SF},
        {ui->labelFlagZF, ASM_DEF::ZF},
    }};

    for (const FlagLabelBinding &binding : bindings) {
        binding.label->setEnabled(flags & binding.mask);
    }
}

void GuiMainWindow::updateJumpLabels(const bool carry,
                                     const bool parity,
                                     const bool overflow,
                                     const bool sign,
                                     const bool zero)
{
    ui->labelJA->setEnabled((!carry) && (!zero));
    ui->labelJBE->setEnabled(carry || zero);
    ui->labelJGE->setEnabled(sign == overflow);
    ui->labelJL->setEnabled(sign != overflow);
    ui->labelJLE->setEnabled(zero || (sign != overflow));
    ui->labelJG->setEnabled((!zero) && (sign == overflow));
    ui->labelJB->setEnabled(carry);
    ui->labelJAE->setEnabled(!carry);
    ui->labelJE->setEnabled(zero);
    ui->labelJNE->setEnabled(!zero);
    ui->labelJP->setEnabled(parity);
    ui->labelJNP->setEnabled(!parity);
    ui->labelJS->setEnabled(sign);
    ui->labelJNS->setEnabled(!sign);
    ui->labelJO->setEnabled(overflow);
    ui->labelJNO->setEnabled(!overflow);
}

void GuiMainWindow::calc()
{
    const ASM_DEF::OPCODE_RECORD record = currentOpcodeRecord();
    RECDATA data = createInputData();
    const bool success = record.asm_func && canExecuteOpcode(record, data);

    if (success) {
        record.asm_func(&data);
    }

    applyResultData(record, &data, success);
    setStatus(success ? QString() : unavailableReason(record, data));

    const XVALUE visibleFlags = data.FLAG[1] & (~static_cast<XVALUE>(kHiddenFlagsMask));
    setEditorValue(ui->lineEditFlagsAfter, visibleFlags);

    updateFlagLabels(visibleFlags);
    updateJumpLabels(visibleFlags & ASM_DEF::CF,
                     visibleFlags & ASM_DEF::PF,
                     visibleFlags & ASM_DEF::OF,
                     visibleFlags & ASM_DEF::SF,
                     visibleFlags & ASM_DEF::ZF);
}

void GuiMainWindow::loadOpcodes(const ASM_DEF::OPCODE_RECORD *pRecords, qint32 nRecordsSize)
{
    const bool wasBlocked = ui->comboBoxOpcode->blockSignals(true);

    m_opcodeMap.clear();
    ui->comboBoxOpcode->clear();

    for (qint32 index = 0; index < nRecordsSize; ++index) {
        const ASM_DEF::OPCODE_RECORD &record = pRecords[index];
        if (!CpuFeatures::isSupported(record.feature)) {
            continue;  // the instruction would raise #UD on this CPU
        }
        m_opcodeMap.insert(record.opcode, record);
        ui->comboBoxOpcode->addItem(record.pszName, static_cast<int>(record.opcode));
    }

    ui->comboBoxOpcode->blockSignals(wasBlocked);
}

void GuiMainWindow::on_comboBoxOpcode_currentIndexChanged(int nIndex)
{
    if (nIndex != -1) {
        adjustMode();
        calc();
    }
}

void GuiMainWindow::adjustValue(QGroupBox *pGroupBox, const ASM_DEF::VALUE_RECORD vr)
{
    // Title the box even when hiding it: an unused slot keeps its Designer
    // placeholder ("Operand3") otherwise, which then shows up in screen readers
    // and in the accessibility tree.
    pGroupBox->setTitle(QString::fromLatin1(vr.pszRegName));

    if (vr.nMaxValue) {
        pGroupBox->show();
    } else {
        pGroupBox->hide();
    }
}

void GuiMainWindow::adjustMode()
{
    const ASM_DEF::OPCODE_RECORD currentRecord = currentOpcodeRecord();

    // EFLAGS is 32 bits wide whatever the register width is.
    applyEditorWidth(ui->lineEditFlagsBefore, ASM_DEF::LIM32);
    applyEditorWidth(ui->lineEditFlagsAfter, ASM_DEF::LIM32);

    if (!currentRecord.asm_func) {
        return;
    }

    const QList<XLineEditHEX *> operands = operandEditors();
    const QList<XLineEditHEX *> results = resultEditors();
    for (qint32 i = 0; i < 4; ++i) {
        applyEditorWidth(operands.at(i), currentRecord.vrOperand[i].nMaxValue);
        applyEditorWidth(results.at(i), currentRecord.vrResult[i].nMaxValue);
    }

    ui->toolButtonOpcode->setText(currentRecord.pszExample);

    adjustValue(ui->groupBoxOperand1, currentRecord.vrOperand[0]);
    adjustValue(ui->groupBoxOperand2, currentRecord.vrOperand[1]);
    adjustValue(ui->groupBoxOperand3, currentRecord.vrOperand[2]);
    adjustValue(ui->groupBoxOperand4, currentRecord.vrOperand[3]);
    adjustValue(ui->groupBoxResult1, currentRecord.vrResult[0]);
    adjustValue(ui->groupBoxResult2, currentRecord.vrResult[1]);
    adjustValue(ui->groupBoxResult3, currentRecord.vrResult[2]);
    adjustValue(ui->groupBoxResult4, currentRecord.vrResult[3]);
}

void GuiMainWindow::on_lineEditOperand1_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditOperand2_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditOperand3_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditOperand4_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_pushButtonFlagCF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::CF, checked);
}

void GuiMainWindow::on_pushButtonFlagPF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::PF, checked);
}

void GuiMainWindow::on_pushButtonFlagAF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::AF, checked);
}

void GuiMainWindow::on_pushButtonFlagZF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::ZF, checked);
}

void GuiMainWindow::on_pushButtonFlagSF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::SF, checked);
}

void GuiMainWindow::on_pushButtonFlagOF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::OF, checked);
}

void GuiMainWindow::on_lineEditFlagsBefore_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    const XVALUE flags = static_cast<XVALUE>(ui->lineEditFlagsBefore->getValue_uint64());
    updateFlagButtons(flags);
    calc();
}

void GuiMainWindow::on_comboBoxMode_currentIndexChanged(int index)
{
    if (index != -1) {
        adjustMode();
        calc();
    }
}

void GuiMainWindow::adjustFlags(XVALUE nFlag, bool bState)
{
    XVALUE nValue = static_cast<XVALUE>(ui->lineEditFlagsBefore->getValue_uint64());

    if (bState) {
        nValue |= nFlag;
    } else {
        nValue &= (~nFlag);
    }

    setEditorValue(ui->lineEditFlagsBefore, nValue);
}

void GuiMainWindow::on_comboBoxOpcodeGroup_currentIndexChanged(int index)
{
    if (index != -1) {
        switch (ui->comboBoxOpcodeGroup->currentData(Qt::UserRole).toUInt()) {
            case OG_TWOOPERANDS: loadOpcodes(ASM_DEF::asm_twooperands, opcodeRecordCount(ASM_DEF::asm_twooperands)); break;
            case OG_ONEOPERAND: loadOpcodes(ASM_DEF::asm_oneoperand, opcodeRecordCount(ASM_DEF::asm_oneoperand)); break;
            case OG_MULDIV: loadOpcodes(ASM_DEF::asm_muldiv, opcodeRecordCount(ASM_DEF::asm_muldiv)); break;
            case OG_SHIFT: loadOpcodes(ASM_DEF::asm_shift, opcodeRecordCount(ASM_DEF::asm_shift)); break;
            case OG_BITS: loadOpcodes(ASM_DEF::asm_bits, opcodeRecordCount(ASM_DEF::asm_bits)); break;
#ifndef OPCODE64
            case OG_BCD: loadOpcodes(ASM_DEF::asm_bcd, opcodeRecordCount(ASM_DEF::asm_bcd)); break;
#endif
            case OG_SPECIAL: loadOpcodes(ASM_DEF::asm_special, opcodeRecordCount(ASM_DEF::asm_special)); break;
        }

        adjustMode();
        calc();
    }
}

void GuiMainWindow::on_pushButtonOptions_clicked()
{
    DialogOptions dialogOptions(this, &m_options);

    dialogOptions.exec();

    adjustWindow();
}

void GuiMainWindow::setStatus(const QString &sText)
{
    if (m_pLabelStatus) {
        m_pLabelStatus->setText(sText);
    }
}

// Why calc() produced nothing. Empty when the opcode ran.
QString GuiMainWindow::unavailableReason(const ASM_DEF::OPCODE_RECORD &record, const RECDATA &data) const
{
    if (!record.asm_func) {
        return QString();
    }

    if (!CpuFeatures::isSupported(record.feature)) {
        return tr("#UD: this CPU does not implement %1").arg(QString::fromLatin1(record.pszName).toUpper());
    }

    if ((record.opcode == ASM_DEF::OP_DIV) || (record.opcode == ASM_DEF::OP_IDIV)) {
        const DivisionCheck::ERROR_CODE errorCode =
            DivisionCheck::divisionError(record.opcode == ASM_DEF::OP_IDIV, data.OPERAND[0], data.OPERAND[1], data.OPERAND[2]);

        if (errorCode == DivisionCheck::ERROR_DIVISOR_ZERO) {
            return tr("#DE: divide error - the divisor is zero");
        } else if (errorCode == DivisionCheck::ERROR_QUOTIENT_OVERFLOW) {
            return tr("#DE: divide error - the quotient does not fit into %1").arg(QString::fromLatin1(record.vrResult[0].pszRegName));
        }
    }

    return QString();
}

QString GuiMainWindow::buildReport() const
{
    QString sReport;
    sReport += ui->toolButtonOpcode->text() + QLatin1Char('\n');

    struct ValueLine {
        QGroupBox *pGroupBox;
        XLineEditHEX *pEditor;
    };

    const std::array<ValueLine, 4> operandLines = {{
        {ui->groupBoxOperand1, ui->lineEditOperand1},
        {ui->groupBoxOperand2, ui->lineEditOperand2},
        {ui->groupBoxOperand3, ui->lineEditOperand3},
        {ui->groupBoxOperand4, ui->lineEditOperand4},
    }};
    const std::array<ValueLine, 4> resultLines = {{
        {ui->groupBoxResult1, ui->lineEditResult1},
        {ui->groupBoxResult2, ui->lineEditResult2},
        {ui->groupBoxResult3, ui->lineEditResult3},
        {ui->groupBoxResult4, ui->lineEditResult4},
    }};

    for (const ValueLine &line : operandLines) {
        if (!line.pGroupBox->isHidden()) {
            sReport += QStringLiteral("%1: %2\n").arg(line.pGroupBox->title(), line.pEditor->text());
        }
    }

    sReport += tr("Result") + QLatin1Char('\n');
    for (const ValueLine &line : resultLines) {
        if (!line.pGroupBox->isHidden()) {
            sReport += QStringLiteral("%1: %2\n").arg(line.pGroupBox->title(), line.pEditor->text());
        }
    }

    sReport += QStringLiteral("%1: %2\n").arg(tr("Flags before"), ui->lineEditFlagsBefore->text());
    sReport += QStringLiteral("%1: %2\n").arg(tr("Flags after"), ui->lineEditFlagsAfter->text());

    if (m_pLabelStatus && !m_pLabelStatus->text().isEmpty()) {
        sReport += m_pLabelStatus->text() + QLatin1Char('\n');
    }

    return sReport;
}

void GuiMainWindow::on_pushButtonCopy_clicked()
{
    QApplication::clipboard()->setText(buildReport());
}

void GuiMainWindow::on_toolButtonOpcode_clicked()
{
    calc();
}
