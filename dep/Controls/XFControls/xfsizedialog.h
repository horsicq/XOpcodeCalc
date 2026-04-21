/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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

#ifndef XFSIZEDIALOG_H
#define XFSIZEDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include "xbinary.h"

class XFSizeDialog : public QDialog {
    Q_OBJECT

public:
    explicit XFSizeDialog(QWidget *pParent);
    virtual ~XFSizeDialog();

    void setData(const QString &sFieldName, quint64 nValue, XBinary::VT valueType);
    quint64 getValue();
    void setReadonly(bool bIsReadonly);

private slots:
    void onOkClicked();
    void onCancelClicked();
    void onHexChanged(const QString &sText);
    void onDecChanged(const QString &sText);

private:
    void updateDisplay();
    quint64 maskValue(quint64 nValue);
    static QString sizeToHumanString(quint64 nSize);

    QLabel *m_pLabelName;
    QLabel *m_pLabelHuman;
    QLineEdit *m_pLineEditHex;
    QLineEdit *m_pLineEditDec;
    QPushButton *m_pPushButtonOK;
    QPushButton *m_pPushButtonCancel;
    quint64 m_nValue;
    XBinary::VT m_valueType;
    bool m_bIsReadonly;
    bool m_bUpdating;
};

#endif  // XFSIZEDIALOG_H
