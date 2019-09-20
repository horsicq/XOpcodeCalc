// Copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "guimainwindow.h"
#include "ui_guimainwindow.h"

GuiMainWindow::GuiMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GuiMainWindow)
{
    ui->setupUi(this);

    setWindowTitle(QString("%1 v%2").arg(X_APPLICATIONNAME).arg(X_APPLICATIONVERSION));

    DialogOptions::loadOptions(&options);
    adjustWindow();

#ifdef OPCODE32
    loadOpcodes(ASM_DEF::asm_records32,sizeof(ASM_DEF::asm_records32)/sizeof(ASM_DEF::OPCODE_RECORD));
#else
    loadOpcodes(ASM_DEF::asm_records64,sizeof(ASM_DEF::asm_records64)/sizeof(ASM_DEF::OPCODE_RECORD));
#endif

    ui->comboBoxMode->addItem(tr("HEX"),ModeValidator::MODE_HEX);
    ui->comboBoxMode->addItem(tr("Signed"),ModeValidator::MODE_SIGNED);
    ui->comboBoxMode->addItem(tr("Unsigned"),ModeValidator::MODE_UNSIGNED);

    currentMode=ModeValidator::MODE_HEX;

    setLineEditValue(ui->lineEditOperand1,currentMode,0);
    setLineEditValue(ui->lineEditOperand2,currentMode,0);

    ui->lineEditOperand1->setValidator(&(modeValidator[0]));
    ui->lineEditOperand2->setValidator(&(modeValidator[1]));
    ui->lineEditFlagsBefore->setValidator(&modeValidatorFlag);
}

GuiMainWindow::~GuiMainWindow()
{
    DialogOptions::saveOptions(&options);
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

void GuiMainWindow::on_pushButton_clicked()
{
    DialogOptions dialogOptions(this,&options);
    dialogOptions.exec();

    adjustWindow();
}

void GuiMainWindow::adjustWindow()
{
    Qt::WindowFlags wf=windowFlags();
    if(options.bStayOnTop)
    {
        wf|=Qt::WindowStaysOnTopHint;
    }
    else
    {
        wf&=~(Qt::WindowStaysOnTopHint);
    }
    setWindowFlags(wf);

    show();
}

void GuiMainWindow::calc()
{
    ModeValidator::MODE mode=static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());
    ASM_DEF::OPCODE_RECORD currentRecord=mapOpcodes.value(static_cast<ASM_DEF::OP>(ui->comboBoxModeOpcode->currentData().toInt()));

    RECDATA data=RECDATA_INIT;

    data.OPERAND[0]=getLineEditValue(ui->lineEditOperand1,mode);
    data.OPERAND[1]=getLineEditValue(ui->lineEditOperand2,mode);
    data.FLAG[0]=getLineEditValue(ui->lineEditFlagsBefore,mode);

    data.FLAG[0]&=((ASM_DEF::AF)|(ASM_DEF::CF)|(ASM_DEF::OF)|(ASM_DEF::PF)|(ASM_DEF::SF)|(ASM_DEF::ZF)); // Filter

    bool bSuccess=true;

    if(     (currentRecord.opcode==ASM_DEF::OP_DIV)||
            (currentRecord.opcode==ASM_DEF::OP_IDIV))
    {
        if(data.OPERAND[1]==0)
        {
            bSuccess=false;
        }
    }

    if(bSuccess)
    {
        currentRecord.asm_func(&data);

        setLineEditValue(ui->lineEditResult1,mode,data.RESULT[0]);
        setLineEditValue(ui->lineEditResult2,mode,data.RESULT[1]);
    }
    else
    {
        ui->lineEditResult1->clear();
        ui->lineEditResult2->clear();

        data.FLAG[1]=data.FLAG[0];
    }

    XVALUE nFlag=data.FLAG[1];

    nFlag&=(~(static_cast<XVALUE>(0x202))); // remove

    setLineEditValue(ui->lineEditFlagsAfter,mode,nFlag);

    ui->labelFlagAF->setEnabled(nFlag&(ASM_DEF::AF));
    ui->labelFlagCF->setEnabled(nFlag&(ASM_DEF::CF));
    ui->labelFlagOF->setEnabled(nFlag&(ASM_DEF::OF));
    ui->labelFlagPF->setEnabled(nFlag&(ASM_DEF::PF));
    ui->labelFlagSF->setEnabled(nFlag&(ASM_DEF::SF));
    ui->labelFlagZF->setEnabled(nFlag&(ASM_DEF::ZF));
}

void GuiMainWindow::loadOpcodes(const ASM_DEF::OPCODE_RECORD *pRecords, qint32 nRecordsSize)
{
    for(int i=0;i<nRecordsSize;i++)
    {
        mapOpcodes.insert(pRecords[i].opcode,pRecords[i]);

        ui->comboBoxModeOpcode->addItem(pRecords[i].pszName,static_cast<int>(pRecords[i].opcode));
    }
}

void GuiMainWindow::on_comboBoxModeOpcode_currentIndexChanged(int index)
{
    if(index!=-1)
    {
        adjustMode();
        calc();
    }
}

void GuiMainWindow::adjustValue(QGroupBox *pGroupBox, ASM_DEF::VALUE_RECORD vr)
{
    if(vr.nMaxValue)
    {
        pGroupBox->show();
        pGroupBox->setTitle(vr.pszRegName);
    }
    else
    {
        pGroupBox->hide();
    }
}

void GuiMainWindow::adjustMode()
{
    ASM_DEF::OPCODE_RECORD currentRecord=mapOpcodes.value(static_cast<ASM_DEF::OP>(ui->comboBoxModeOpcode->currentData().toInt()));

    ModeValidator::MODE mode=static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

    ModeValidator::DATA validatorData[2]={};
    ModeValidator::DATA validatorDataFlag;

    validatorData[0].mode=mode;
    validatorData[0].nMaxValue=currentRecord.vrOperand[0].nMaxValue;

    validatorData[1].mode=mode;
    validatorData[1].nMaxValue=currentRecord.vrOperand[1].nMaxValue;

    validatorDataFlag.mode=mode;
    validatorDataFlag.nMaxValue=0xFFFFFFFF;

    modeValidator[0].setData(validatorData[0]);
    modeValidator[1].setData(validatorData[1]);
    modeValidatorFlag.setData(validatorDataFlag);

    ui->labelOpcode->setText(currentRecord.pszExample);

    adjustValue(ui->groupBoxOperand1,currentRecord.vrOperand[0]);
    adjustValue(ui->groupBoxOperand2,currentRecord.vrOperand[1]);
    adjustValue(ui->groupBoxResult1,currentRecord.vrResult[0]);
    adjustValue(ui->groupBoxResult2,currentRecord.vrResult[1]);
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

void GuiMainWindow::on_pushButtonFlagCF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::CF,checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagPF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::PF,checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagAF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::AF,checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagZF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::ZF,checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagSF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::SF,checked);
    calc();
}

void GuiMainWindow::on_pushButtonFlagOF_toggled(bool checked)
{
    adjustFlags(ASM_DEF::OF,checked);
    calc();
}

void GuiMainWindow::on_lineEditFlagsBefore_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    XVALUE nFlag=getLineEditValue(ui->lineEditFlagsBefore,currentMode);

    ui->pushButtonFlagAF->setChecked(nFlag&(ASM_DEF::AF));
    ui->pushButtonFlagCF->setChecked(nFlag&(ASM_DEF::CF));
    ui->pushButtonFlagOF->setChecked(nFlag&(ASM_DEF::OF));
    ui->pushButtonFlagPF->setChecked(nFlag&(ASM_DEF::PF));
    ui->pushButtonFlagSF->setChecked(nFlag&(ASM_DEF::SF));
    ui->pushButtonFlagZF->setChecked(nFlag&(ASM_DEF::ZF));

    calc();
}

void GuiMainWindow::on_comboBoxMode_currentIndexChanged(int index)
{
    if(index!=-1)
    {
        RECDATA _data=RECDATA_INIT;

        _data.OPERAND[0]=getLineEditValue(ui->lineEditOperand1,currentMode);
        _data.OPERAND[1]=getLineEditValue(ui->lineEditOperand2,currentMode);
        _data.FLAG[0]=getLineEditValue(ui->lineEditFlagsBefore,currentMode);

        currentMode=static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

        adjustMode();

        setLineEditValue(ui->lineEditOperand1,currentMode,_data.OPERAND[0]);
        setLineEditValue(ui->lineEditOperand2,currentMode,_data.OPERAND[1]);
        setLineEditValue(ui->lineEditFlagsBefore,currentMode,_data.FLAG[0]);

        calc();
    }
}

XVALUE GuiMainWindow::getLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode)
{
    XVALUE nValue=0;

    QString sText=pLineEdit->text();

    if(mode==ModeValidator::MODE_HEX)
    {
#ifdef OPCODE32
        nValue=sText.toULong(nullptr,16);
#else
        nValue=sText.toULongLong(nullptr,16);
#endif
    }
    else if(mode==ModeValidator::MODE_SIGNED)
    {
#ifdef OPCODE32
        nValue=static_cast<XVALUE>(sText.toLong(nullptr,10));
#else
        nValue=static_cast<XVALUE>(sText.toLongLong(nullptr,10));
#endif
    }
    else if(mode==ModeValidator::MODE_UNSIGNED)
    {
#ifdef OPCODE32
        nValue=sText.toULong(nullptr,10);
#else
        nValue=sText.toULongLong(nullptr,10);
#endif
    }

    return nValue;
}

void GuiMainWindow::setLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode, XVALUE nValue)
{
    QString sText;

    if(mode==ModeValidator::MODE_HEX)
    {
        sText=QString::number(nValue,16);
    }
    else if(mode==ModeValidator::MODE_SIGNED)
    {

#ifdef OPCODE32
        sText=QString::number(static_cast<qint32>(nValue),10);
#else
        sText=QString::number(static_cast<qint64>(nValue),10);
#endif
    }
    else if(mode==ModeValidator::MODE_UNSIGNED)
    {
        sText=QString::number(nValue,10);
    }

    pLineEdit->setText(sText);
}

void GuiMainWindow::adjustFlags(quint32 nFlag, bool bState)
{
    ModeValidator::MODE mode=static_cast<ModeValidator::MODE>(ui->comboBoxMode->currentData().toInt());

    XVALUE nValue=getLineEditValue(ui->lineEditFlagsBefore,mode);

    if(bState)
    {
        nValue|=nFlag;
    }
    else
    {
        nValue&=(~nFlag);
    }

    setLineEditValue(ui->lineEditFlagsBefore,mode,nValue);
}
