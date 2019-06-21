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

    loadOpcodes((ASM_DEF::OPCODE_RECORD *)ASM_DEF::records32,sizeof(ASM_DEF::records32)/sizeof(ASM_DEF::OPCODE_RECORD));

    ui->comboBoxMode->addItem(tr("HEX"),ModeValidator::MODE_HEX);
    ui->comboBoxMode->addItem(tr("Signed"),ModeValidator::MODE_SIGNED);
    ui->comboBoxMode->addItem(tr("Unsigned"),ModeValidator::MODE_UNSIGNED);

    ui->lineEditOperand1->setValidator(&(modeValidator[0]));
    ui->lineEditOperand2->setValidator(&(modeValidator[1]));
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
    ModeValidator::MODE mode=(ModeValidator::MODE)(ui->comboBoxMode->currentData().toInt());

    ModeValidator::DATA validatorData[2]={};

    validatorData[0].mode=mode;
    validatorData[1].mode=mode;

    modeValidator[0].setData(validatorData[0]);
    modeValidator[1].setData(validatorData[1]);


    QString sOperand[2];
    QString sResult[2];
#ifdef OPCODE32
    quint32 nOperand[2];
    quint32 nResult[2];
#else
    quint64 nOperand[2];
    quint64 nResult[2];
#endif
    sOperand[0]=ui->lineEditOperand1->text();
    sOperand[1]=ui->lineEditOperand2->text();

    if(mode==ModeValidator::MODE_HEX)
    {
#ifdef OPCODE32
        nOperand[0]=sOperand[0].toULong(0,16);
        nOperand[1]=sOperand[1].toULong(0,16);
#else
        nOperand[0]=sOperand[0].toULongLong(0,16);
        nOperand[1]=sOperand[1].toULongLong(0,16);
#endif
    }

#ifdef OPCODE32
    RECDATA32 data={0};

    data.OPERAND[0]=nOperand[0];
    data.OPERAND[1]=nOperand[1];

    // TODO
    op_add_32(&data);

    nResult[0]=data.RESULT[0];
    nResult[1]=data.RESULT[1];
#else
    RECDATA64 data={0};

    data.OPERAND[0]=0x10;
    data.OPERAND[1]=0x20;

    op_add_64(&data);
#endif

    if(mode==ModeValidator::MODE_HEX)
    {
        sResult[0]=QString::number(nResult[0],16);
        sResult[1]=QString::number(nResult[1],16);
    }

    ui->lineEditResult1->setText(sResult[0]);
    ui->lineEditResult2->setText(sResult[1]);
}

void GuiMainWindow::loadOpcodes(ASM_DEF::OPCODE_RECORD *pRecords, qint32 nRecordsSize)
{
    for(int i=0;i<nRecordsSize;i++)
    {
        mapOpcodes.insert(pRecords[i].opcode,pRecords[i]);

        ui->comboBoxModeOpcode->addItem(pRecords[i].pszName,(int)pRecords[i].opcode);
    }
}

void GuiMainWindow::on_comboBoxModeOpcode_currentIndexChanged(int index)
{
    if(index!=-1)
    {
        int nData=ui->comboBoxModeOpcode->currentData().toInt();
        currentRecord=mapOpcodes.value((ASM_DEF::OP)nData);

        ui->labelOpcode->setText(currentRecord.pszExample);

        adjustValue(ui->groupBoxOperand1,currentRecord.vrOperand[0]);
        adjustValue(ui->groupBoxOperand2,currentRecord.vrOperand[1]);
        adjustValue(ui->groupBoxResult1,currentRecord.vrResult[0]);
        adjustValue(ui->groupBoxResult2,currentRecord.vrResult[1]);
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

void GuiMainWindow::on_lineEditOperand1_textChanged(const QString &arg1)
{
    calc();
}

void GuiMainWindow::on_lineEditOperand2_textChanged(const QString &arg1)
{
    calc();
}

void GuiMainWindow::on_lineEditResult1_textChanged(const QString &arg1)
{
    calc();
}

void GuiMainWindow::on_lineEditResult2_textChanged(const QString &arg1)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagCF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagPF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagAF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagZF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagSF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_pushButtonFlagOF_toggled(bool checked)
{
    calc();
}

void GuiMainWindow::on_lineEditFlagsBefore_textChanged(const QString &arg1)
{
    calc();
}

void GuiMainWindow::on_comboBoxMode_currentIndexChanged(int index)
{
    if(index!=-1)
    {

    }
}
