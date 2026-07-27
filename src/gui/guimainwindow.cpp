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

GuiMainWindow::GuiMainWindow(QWidget *pParent) : QMainWindow(pParent), ui(new Ui::GuiMainWindow)
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
            editors.at(index)->setValue32_64(resultValues.at(index), XLineEditHEX::_MODE_UNKNOWN);
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

    for (const FlagButtonBinding &binding : bindings) {
        binding.button->blockSignals(true);
    }

    for (const FlagButtonBinding &binding : bindings) {
        binding.button->setChecked(flags & binding.mask);
    }

    for (const FlagButtonBinding &binding : bindings) {
        binding.button->blockSignals(false);
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

void GuiMainWindow::setEditorsMode(const XLineEditHEX::_MODE mode)
{
    const QList<XLineEditHEX *> editors = {
        ui->lineEditOperand1,
        ui->lineEditOperand2,
        ui->lineEditOperand3,
        ui->lineEditOperand4,
        ui->lineEditResult1,
        ui->lineEditResult2,
        ui->lineEditResult3,
        ui->lineEditResult4,
        ui->lineEditFlagsBefore,
        ui->lineEditFlagsAfter,
    };

    for (XLineEditHEX *editor : editors) {
        editor->setMode(mode);
    }
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

    const XVALUE visibleFlags = data.FLAG[1] & (~static_cast<XVALUE>(kHiddenFlagsMask));
    ui->lineEditFlagsAfter->setValue32_64(visibleFlags, XLineEditHEX::_MODE_UNKNOWN);

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
    if (vr.nMaxValue) {
        pGroupBox->show();
        pGroupBox->setTitle(vr.pszRegName);
    } else {
        pGroupBox->hide();
    }
}

void GuiMainWindow::adjustMode()
{
    const ASM_DEF::OPCODE_RECORD currentRecord = currentOpcodeRecord();
    if (!currentRecord.asm_func) {
        return;
    }

    ui->lineEditOperand1->setMaxValue(currentRecord.vrOperand[0].nMaxValue);
    ui->lineEditOperand2->setMaxValue(currentRecord.vrOperand[1].nMaxValue);
    ui->lineEditOperand3->setMaxValue(currentRecord.vrOperand[2].nMaxValue);
    ui->lineEditOperand4->setMaxValue(currentRecord.vrOperand[3].nMaxValue);

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
        const XLineEditHEX::_MODE mode = static_cast<XLineEditHEX::_MODE>(ui->comboBoxMode->currentData().toInt());
        setEditorsMode(mode);
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

    ui->lineEditFlagsBefore->setValue32_64(nValue, XLineEditHEX::_MODE_UNKNOWN);
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
