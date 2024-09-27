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
#ifndef GUIMAINWINDOW_H
#define GUIMAINWINDOW_H

#include <QGroupBox>
#include <QLineEdit>
#include <QMainWindow>

#include "../asm/asm.h"
#include "../asm_defs.h"
#include "../global.h"
#include "dialogabout.h"
#include "dialogoptions.h"
#include "modevalidator.h"
#include "xoptions.h"

namespace Ui {
class GuiMainWindow;
}

class GuiMainWindow : public QMainWindow {
    Q_OBJECT

    enum OG {
        OG_TWOOPERANDS = 0,
        OG_ONEOPERAND,
        OG_MULDIV,
        OG_SHIFT,
        OG_BITS,
#ifndef OPCODE64
        OG_BCD,
#endif
        OG_SPECIAL
    };

public:
    explicit GuiMainWindow(QWidget *pParent = nullptr);
    ~GuiMainWindow();

private slots:
    void on_pushButtonExit_clicked();
    void on_pushButtonAbout_clicked();
    void on_comboBoxOpcode_currentIndexChanged(int nIndex);
    void on_lineEditOperand1_textChanged(const QString &arg1);
    void on_lineEditOperand2_textChanged(const QString &arg1);
    void on_lineEditOperand3_textChanged(const QString &arg1);
    void on_lineEditOperand4_textChanged(const QString &arg1);
    void on_lineEditResult1_textChanged(const QString &arg1);
    void on_lineEditResult2_textChanged(const QString &arg1);
    void on_lineEditResult3_textChanged(const QString &arg1);
    void on_lineEditResult4_textChanged(const QString &arg1);
    void on_pushButtonFlagCF_toggled(bool checked);
    void on_pushButtonFlagPF_toggled(bool checked);
    void on_pushButtonFlagAF_toggled(bool checked);
    void on_pushButtonFlagZF_toggled(bool checked);
    void on_pushButtonFlagSF_toggled(bool checked);
    void on_pushButtonFlagOF_toggled(bool checked);
    void on_lineEditFlagsBefore_textChanged(const QString &arg1);
    void on_comboBoxMode_currentIndexChanged(int index);
    void adjustWindow();
    void calc();
    void loadOpcodes(const ASM_DEF::OPCODE_RECORD *pRecords, qint32 nRecordsSize);
    void adjustValue(QGroupBox *pGroupBox, ASM_DEF::VALUE_RECORD vr);
    void adjustMode();
    XVALUE getLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode);
    void setLineEditValue(QLineEdit *pLineEdit, ModeValidator::MODE mode, XVALUE nValue);
    void adjustFlags(XVALUE nFlag, bool bState);
    void on_comboBoxOpcodeGroup_currentIndexChanged(int index);
    void on_pushButtonOptions_clicked();
    void on_toolButtonOpcode_clicked();

private:
    Ui::GuiMainWindow *ui;
    XOptions g_xOptions;
    QMap<ASM_DEF::OP, ASM_DEF::OPCODE_RECORD> g_mapOpcodes;
    ModeValidator g_modeValidator[4];
    ModeValidator g_modeValidatorFlag;
    ModeValidator::MODE g_currentMode;
};

#endif  // GUIMAINWINDOW_H
