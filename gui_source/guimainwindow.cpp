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

    ui->comboBoxMode->addItem(QString("HEX"), XLineEditHEX::_MODE_HEX);
    ui->comboBoxMode->addItem(QString("Signed"), XLineEditHEX::_MODE_SIGN_DEC);
    ui->comboBoxMode->addItem(QString("Unsigned"), XLineEditHEX::_MODE_DEC);

    adjustMode();
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

   // ModeValidator::MODE mode = static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());
    ASM_DEF::OPCODE_RECORD currentRecord = g_mapOpcodes.value(static_cast<ASM_DEF::OP>(ui->comboBoxOpcode->currentData().toInt()));

    RECDATA data = RECDATA_INIT;

    data.OPERAND[0] = (XVALUE)ui->lineEditOperand1->getValue_uint64();
    data.OPERAND[1] = (XVALUE)ui->lineEditOperand2->getValue_uint64();
    data.OPERAND[2] = (XVALUE)ui->lineEditOperand3->getValue_uint64();
    data.OPERAND[3] = (XVALUE)ui->lineEditOperand4->getValue_uint64();
    data.FLAG[0] = (XVALUE)ui->lineEditOperand4->getValue_uint64();

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
        ui->lineEditResult1->setValue32_64((XVALUE)data.RESULT[0], XLineEditHEX::_MODE_UNKNOWN);

        if (currentRecord.opcode == ASM_DEF::OP_XADD) {
            ui->lineEditResult2->setValue32_64((XVALUE)data.RESULT[2], XLineEditHEX::_MODE_UNKNOWN);
            ui->lineEditResult3->setValue32_64((XVALUE)data.RESULT[1], XLineEditHEX::_MODE_UNKNOWN);
        } else {
            ui->lineEditResult2->setValue32_64((XVALUE)data.RESULT[1], XLineEditHEX::_MODE_UNKNOWN);
            ui->lineEditResult3->setValue32_64((XVALUE)data.RESULT[2], XLineEditHEX::_MODE_UNKNOWN);
        }

        ui->lineEditResult4->setValue32_64((XVALUE)data.RESULT[3], XLineEditHEX::_MODE_UNKNOWN);
    } else {
        ui->lineEditResult1->clear();
        ui->lineEditResult2->clear();
        ui->lineEditResult3->clear();
        ui->lineEditResult4->clear();

        data.FLAG[1] = data.FLAG[0];
    }

    XVALUE nFlag = data.FLAG[1];

    nFlag &= (~(static_cast<XVALUE>(0x202)));  // remove

    ui->lineEditFlagsAfter->setValue32_64(nFlag, XLineEditHEX::_MODE_UNKNOWN);

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

    XVALUE nFlag = (XVALUE)ui->lineEditFlagsBefore->getValue_uint64();

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
        XLineEditHEX::_MODE mode = static_cast<XLineEditHEX::_MODE>(ui->comboBoxMode->currentData().toInt());

        ui->lineEditOperand1->setMode(mode);
        ui->lineEditOperand2->setMode(mode);
        ui->lineEditOperand3->setMode(mode);
        ui->lineEditOperand4->setMode(mode);
        ui->lineEditResult1->setMode(mode);
        ui->lineEditResult2->setMode(mode);
        ui->lineEditResult3->setMode(mode);
        ui->lineEditResult4->setMode(mode);
        ui->lineEditFlagsBefore->setMode(mode);
        ui->lineEditFlagsAfter->setMode(mode);

        adjustMode();

        calc();
    }
}

void GuiMainWindow::adjustFlags(XVALUE nFlag, bool bState)
{
    XVALUE nValue = (XVALUE)ui->lineEditFlagsBefore->getValue_uint64();

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
