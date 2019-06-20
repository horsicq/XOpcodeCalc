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

    calc();
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
#ifdef OPCODE32
    RECDATA32 data={0};

    data.OPERAND[0]=0x10;
    data.OPERAND[1]=0x20;

    // TODO
    op_add_32(&data);
#else
    RECDATA64 data={0};

    data.OPERAND[0]=0x10;
    data.OPERAND[1]=0x20;

    // TODO
    op_add_64(&data);
#endif
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
