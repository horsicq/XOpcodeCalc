/* Copyright (c) 2019-2024 hors<horsicq@gmail.com>
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

#include "bigint.h"

GuiMainWindow::GuiMainWindow(QWidget *pParent) : QMainWindow(pParent), ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));

    QFont font = ui->toolButtonOpcode->font();
    font.setPointSizeF(font.pointSizeF() * 1.5);
    font.setBold(true);
    ui->toolButtonOpcode->setFont(font);

    g_xOptions.setName(X_OPTIONSFILE);

    g_xOptions.addID(XOptions::ID_VIEW_STYLE, "Fusion");
    g_xOptions.addID(XOptions::ID_VIEW_STAYONTOP, false);

    g_xOptions.load();

    adjustWindow();

    ui->comboBoxOpcodeGroup->addItem(tr("Two operands"), OG_TWOOPERANDS);
    ui->comboBoxOpcodeGroup->addItem(tr("One operand"), OG_ONEOPERAND);
    ui->comboBoxOpcodeGroup->addItem(QString("Mul/Div"), OG_MULDIV);
    ui->comboBoxOpcodeGroup->addItem(QString("Shift"), OG_SHIFT);
    ui->comboBoxOpcodeGroup->addItem(QString("Bits"), OG_BITS);
#ifndef OPCODE64
    ui->comboBoxOpcodeGroup->addItem(QString("BCD"), OG_BCD);
#endif
    ui->comboBoxOpcodeGroup->addItem(QString("Special"), OG_SPECIAL);

    ui->comboBoxMode->addItem(QString("HEX"), ModeValidator::MODE_HEX);
    ui->comboBoxMode->addItem(QString("Signed"), ModeValidator::MODE_SIGNED);
    ui->comboBoxMode->addItem(QString("Unsigned"), ModeValidator::MODE_UNSIGNED);

    g_currentMode = ModeValidator::MODE_HEX;

    setLineEditValue(ui->lineEditOperand1, g_currentMode, 0);
    setLineEditValue(ui->lineEditOperand2, g_currentMode, 0);
    setLineEditValue(ui->lineEditOperand3, g_currentMode, 0);
    setLineEditValue(ui->lineEditOperand4, g_currentMode, 0);

    ui->lineEditOperand1->setValidator(&(g_modeValidator[0]));
    ui->lineEditOperand2->setValidator(&(g_modeValidator[1]));
    ui->lineEditOperand3->setValidator(&(g_modeValidator[2]));
    ui->lineEditOperand4->setValidator(&(g_modeValidator[3]));
    ui->lineEditFlagsBefore->setValidator(&g_modeValidatorFlag);
}

GuiMainWindow::~GuiMainWindow()
{
    g_xOptions.save();

    delete ui;
}

void GuiMainWindow::on_pushButtonExit_clicked()
{
    this->close();
}

void GuiMainWindow::on_pushButtonAbout_clicked()
{
    DialogAbout di(this);

    di.exec();
}

void GuiMainWindow::adjustWindow()
{
    g_xOptions.adjustStayOnTop(this);
}

void GuiMainWindow::calc()
{
    ui->lineEditOperand1->blockSignals(true);
    ui->lineEditOperand2->blockSignals(true);
    ui->lineEditOperand3->blockSignals(true);
    ui->lineEditOperand4->blockSignals(true);
    ui->lineEditResult1->blockSignals(true);
    ui->lineEditResult2->blockSignals(true);
    ui->lineEditResult3->blockSignals(true);
    ui->lineEditResult4->blockSignals(true);

    ModeValidator::MODE mode = static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());
    ASM_DEF::OPCODE_RECORD currentRecord = g_mapOpcodes.value(static_cast<ASM_DEF::OP>(ui->comboBoxOpcode->currentData().toInt()));

    RECDATA data = RECDATA_INIT;

    data.OPERAND[0] = getLineEditValue(ui->lineEditOperand1, mode);
    data.OPERAND[1] = getLineEditValue(ui->lineEditOperand2, mode);
    data.OPERAND[2] = getLineEditValue(ui->lineEditOperand3, mode);
    data.OPERAND[3] = getLineEditValue(ui->lineEditOperand4, mode);
    data.FLAG[0] = getLineEditValue(ui->lineEditFlagsBefore, mode);

    data.FLAG[0] &= ((ASM_DEF::AF) | (ASM_DEF::CF) | (ASM_DEF::OF) | (ASM_DEF::PF) | (ASM_DEF::SF) | (ASM_DEF::ZF));  // Filter

    bool bSuccess = true;

    if ((currentRecord.opcode == ASM_DEF::OP_DIV) || (currentRecord.opcode == ASM_DEF::OP_IDIV)) {
        if (data.OPERAND[1] == 0) {
            bSuccess = false;
        }

        if (bSuccess) {
            BigInt a0(QString::number(data.OPERAND[0]).toStdString());
            BigInt a1(QString::number(data.OPERAND[1]).toStdString());
            BigInt a2(QString::number(data.OPERAND[2]).toStdString());

#ifdef OPCODE32
            BigInt biMax(QString::number(0xFFFFFFFF).toStdString());
            BigInt biMax0(QString::number(0x7FFFFFFF).toStdString());
#endif
#ifdef OPCODE64
            BigInt biMax(QString::number(0xFFFFFFFFFFFFFFFF).toStdString());
            BigInt biMax0(QString::number(0x7FFFFFFFFFFFFFFF).toStdString());
#endif
            BigInt res = (a0 + (a2 * (biMax + 1))) / a1;

#ifdef QT_DEBUG
            qDebug("a0:     %s", a0.to_string().c_str());
            qDebug("a1:     %s", a1.to_string().c_str());
            qDebug("a2:     %s", a2.to_string().c_str());
            qDebug("biMax:  %s", biMax.to_string().c_str());
            qDebug("biMax0: %s", biMax0.to_string().c_str());
            qDebug("res:    %s", res.to_string().c_str());
#endif

            if (res >= biMax) {
                bSuccess = false;
            }

            if (currentRecord.opcode == ASM_DEF::OP_IDIV) {
                if ((a0 >= biMax0) && (a1 >= biMax0)) {
                    bSuccess = false;
                }
                if ((a2 > 0) && (a1 >= biMax0)) {
                    bSuccess = false;
                }
            }
        }
    }

    if (bSuccess) {
        currentRecord.asm_func(&data);
    }

    if (bSuccess) {
        setLineEditValue(ui->lineEditResult1, mode, data.RESULT[0]);

        if (currentRecord.opcode == ASM_DEF::OP_XADD) {
            setLineEditValue(ui->lineEditResult2, mode, data.RESULT[2]);
            setLineEditValue(ui->lineEditResult3, mode, data.RESULT[1]);
        } else {
            setLineEditValue(ui->lineEditResult2, mode, data.RESULT[1]);
            setLineEditValue(ui->lineEditResult3, mode, data.RESULT[2]);
        }

        setLineEditValue(ui->lineEditResult4, mode, data.RESULT[3]);
    } else {
        ui->lineEditResult1->clear();
        ui->lineEditResult2->clear();
        ui->lineEditResult3->clear();
        ui->lineEditResult4->clear();

        data.FLAG[1] = data.FLAG[0];
    }

    XVALUE nFlag = data.FLAG[1];

    nFlag &= (~(static_cast<XVALUE>(0x202)));  // remove

    setLineEditValue(ui->lineEditFlagsAfter, mode, nFlag);

    bool bAF = nFlag & (ASM_DEF::AF);
    bool bCF = nFlag & (ASM_DEF::CF);
    bool bOF = nFlag & (ASM_DEF::OF);
    bool bPF = nFlag & (ASM_DEF::PF);
    bool bSF = nFlag & (ASM_DEF::SF);
    bool bZF = nFlag & (ASM_DEF::ZF);

    ui->labelFlagAF->setEnabled(bAF);
    ui->labelFlagCF->setEnabled(bCF);
    ui->labelFlagOF->setEnabled(bOF);
    ui->labelFlagPF->setEnabled(bPF);
    ui->labelFlagSF->setEnabled(bSF);
    ui->labelFlagZF->setEnabled(bZF);

    ui->labelJA->setEnabled((bCF == false) && (bZF == false));
    ui->labelJBE->setEnabled((bCF == true) || (bZF == true));
    ui->labelJGE->setEnabled(bSF == bOF);
    ui->labelJL->setEnabled(bSF != bOF);
    ui->labelJLE->setEnabled((bZF == true) || (bSF != bOF));
    ui->labelJG->setEnabled((bZF == false) && (bSF == bOF));
    ui->labelJB->setEnabled(bCF == true);
    ui->labelJAE->setEnabled(bCF == false);
    ui->labelJE->setEnabled(bZF == true);
    ui->labelJNE->setEnabled(bZF == false);
    ui->labelJP->setEnabled(bPF == true);
    ui->labelJNP->setEnabled(bPF == false);
    ui->labelJS->setEnabled(bSF == true);
    ui->labelJNS->setEnabled(bSF == false);
    ui->labelJO->setEnabled(bOF == true);
    ui->labelJNO->setEnabled(bOF == false);

    ui->lineEditOperand1->blockSignals(false);
    ui->lineEditOperand2->blockSignals(false);
    ui->lineEditOperand3->blockSignals(false);
    ui->lineEditOperand4->blockSignals(false);
    ui->lineEditResult1->blockSignals(false);
    ui->lineEditResult2->blockSignals(false);
    ui->lineEditResult3->blockSignals(false);
    ui->lineEditResult4->blockSignals(false);
}

void GuiMainWindow::loadOpcodes(const ASM_DEF::OPCODE_RECORD *pRecords, qint32 nRecordsSize)
{
#if QT_VERSION >= 0x050300
    QSignalBlocker blocker(ui->comboBoxOpcode);
#else
    const bool bBlocked1 = ui->comboBoxOpcode->blockSignals(true);
#endif

    g_mapOpcodes.clear();
    ui->comboBoxOpcode->clear();

    for (int i = 0; i < nRecordsSize; i++) {
        g_mapOpcodes.insert(pRecords[i].opcode, pRecords[i]);

        ui->comboBoxOpcode->addItem(pRecords[i].pszName, static_cast<int>(pRecords[i].opcode));
    }

#if QT_VERSION < 0x050300
    ui->comboBoxOpcode->blockSignals(bBlocked1);
#endif
}

void GuiMainWindow::on_comboBoxOpcode_currentIndexChanged(int nIndex)
{
    if (nIndex != -1) {
        adjustMode();
        calc();
    }
}

void GuiMainWindow::adjustValue(QGroupBox *pGroupBox, ASM_DEF::VALUE_RECORD vr)
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
    ASM_DEF::OPCODE_RECORD currentRecord = g_mapOpcodes.value(static_cast<ASM_DEF::OP>(ui->comboBoxOpcode->currentData().toInt()));

    ModeValidator::MODE mode = static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

    ModeValidator::DATA validatorData[4] = {};
    ModeValidator::DATA validatorDataFlag;

    for (qint32 i = 0; i < 4; i++) {
        validatorData[i].mode = mode;
        validatorData[i].nMaxValue = currentRecord.vrOperand[i].nMaxValue;
    }

    validatorDataFlag.mode = mode;
    validatorDataFlag.nMaxValue = 0xFFFFFFFF;

    for (qint32 i = 0; i < 4; i++) {
        g_modeValidator[i].setData(validatorData[i]);
    }

    g_modeValidatorFlag.setData(validatorDataFlag);

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

void GuiMainWindow::on_lineEditResult1_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditResult2_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditResult3_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_lineEditResult4_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    calc();
}

void GuiMainWindow::on_pushButtonFlagCF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::CF, checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagPF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::PF, checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagAF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::AF, checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagZF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::ZF, checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagSF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::SF, checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagOF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::OF, checked);
    calc();
}

void GuiMainWindow::on_lineEditFlagsBefore_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    XVALUE nFlag = getLineEditValue(ui->lineEditFlagsBefore, g_currentMode);

    ui->pushButtonFlagAF->setChecked(nFlag & (ASM_DEF::AF));
    ui->pushButtonFlagCF->setChecked(nFlag & (ASM_DEF::CF));
    ui->pushButtonFlagOF->setChecked(nFlag & (ASM_DEF::OF));
    ui->pushButtonFlagPF->setChecked(nFlag & (ASM_DEF::PF));
    ui->pushButtonFlagSF->setChecked(nFlag & (ASM_DEF::SF));
    ui->pushButtonFlagZF->setChecked(nFlag & (ASM_DEF::ZF));

    calc();
}

void GuiMainWindow::on_comboBoxMode_currentIndexChanged(int index)
{
    if (index != -1) {
        RECDATA _data = RECDATA_INIT;

        _data.OPERAND[0] = getLineEditValue(ui->lineEditOperand1, g_currentMode);
        _data.OPERAND[1] = getLineEditValue(ui->lineEditOperand2, g_currentMode);
        _data.OPERAND[2] = getLineEditValue(ui->lineEditOperand3, g_currentMode);
        _data.OPERAND[3] = getLineEditValue(ui->lineEditOperand4, g_currentMode);
        _data.FLAG[0] = getLineEditValue(ui->lineEditFlagsBefore, g_currentMode);

        g_currentMode = static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

        adjustMode();

        setLineEditValue(ui->lineEditOperand1, g_currentMode, _data.OPERAND[0]);
        setLineEditValue(ui->lineEditOperand2, g_currentMode, _data.OPERAND[1]);
        setLineEditValue(ui->lineEditOperand3, g_currentMode, _data.OPERAND[2]);
        setLineEditValue(ui->lineEditOperand4, g_currentMode, _data.OPERAND[3]);
        setLineEditValue(ui->lineEditFlagsBefore, g_currentMode, _data.FLAG[0]);

        calc();
    }
}

XVALUE GuiMainWindow::getLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode)
{
    XVALUE nValue = 0;

    QString sText = pLineEdit->text();

    if (mode == ModeValidator::MODE_HEX) {
#ifdef OPCODE32
        nValue = sText.toULong(nullptr, 16);
#else
        nValue = sText.toULongLong(nullptr, 16);
#endif
    } else if (mode == ModeValidator::MODE_SIGNED) {
#ifdef OPCODE32
        nValue = static_cast<XVALUE>(sText.toLong(nullptr, 10));
#else
        nValue = static_cast<XVALUE>(sText.toLongLong(nullptr, 10));
#endif
    } else if (mode == ModeValidator::MODE_UNSIGNED) {
#ifdef OPCODE32
        nValue = sText.toULong(nullptr, 10);
#else
        nValue = sText.toULongLong(nullptr, 10);
#endif
    }

    return nValue;
}

void GuiMainWindow::setLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode, XVALUE nValue)
{
    QString sText;

    if (mode == ModeValidator::MODE_HEX) {
        sText = QString::number(nValue, 16);
    } else if (mode == ModeValidator::MODE_SIGNED) {
        sText = QString::number(static_cast<SXVALUE>(nValue), 10);
    } else if (mode == ModeValidator::MODE_UNSIGNED) {
        sText = QString::number(nValue, 10);
    }

    pLineEdit->setText(sText);
}

void GuiMainWindow::adjustFlags(XVALUE nFlag, bool bState)
{
    ModeValidator::MODE mode = static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

    XVALUE nValue = getLineEditValue(ui->lineEditFlagsBefore, mode);

    if (bState) {
        nValue |= nFlag;
    } else {
        nValue &= (~nFlag);
    }

    setLineEditValue(ui->lineEditFlagsBefore, mode, nValue);
}

void GuiMainWindow::on_comboBoxOpcodeGroup_currentIndexChanged(int index)
{
    if (index != -1) {
        switch (ui->comboBoxOpcodeGroup->currentData(Qt::UserRole).toUInt()) {
            case OG_TWOOPERANDS: loadOpcodes(ASM_DEF::asm_twooperands, sizeof(ASM_DEF::asm_twooperands) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
            case OG_ONEOPERAND: loadOpcodes(ASM_DEF::asm_oneoperand, sizeof(ASM_DEF::asm_oneoperand) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
            case OG_MULDIV: loadOpcodes(ASM_DEF::asm_muldiv, sizeof(ASM_DEF::asm_muldiv) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
            case OG_SHIFT: loadOpcodes(ASM_DEF::asm_shift, sizeof(ASM_DEF::asm_shift) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
            case OG_BITS: loadOpcodes(ASM_DEF::asm_bits, sizeof(ASM_DEF::asm_bits) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
#ifndef OPCODE64
            case OG_BCD: loadOpcodes(ASM_DEF::asm_bcd, sizeof(ASM_DEF::asm_bcd) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
#endif
            case OG_SPECIAL: loadOpcodes(ASM_DEF::asm_special, sizeof(ASM_DEF::asm_special) / sizeof(ASM_DEF::OPCODE_RECORD)); break;
        }

        adjustMode();
        calc();
    }
}

void GuiMainWindow::on_pushButtonOptions_clicked()
{
    DialogOptions dialogOptions(this, &g_xOptions);

    dialogOptions.exec();

    adjustWindow();
}

void GuiMainWindow::on_toolButtonOpcode_clicked()
{
    calc();
}
