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

#ifndef XFSTRINGDIALOG_H
#define XFSTRINGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "xbinary.h"

class XFStringDialog : public QDialog {
    Q_OBJECT

public:
    explicit XFStringDialog(QWidget *pParent);
    virtual ~XFStringDialog();

    void setData(const QString &sFieldName, const QString &sValue, qint32 nMaxLength);
    QString getValue();
    void setReadonly(bool bIsReadonly);

private slots:
    void onOkClicked();
    void onCancelClicked();

private:
    QLineEdit *m_pLineEditValue;
    QLabel *m_pLabelName;
    QPushButton *m_pPushButtonOK;
    QPushButton *m_pPushButtonCancel;
    QString m_sValue;
    qint32 m_nMaxLength;
    bool m_bIsReadonly;
};

#endif  // XFSTRINGDIALOG_H
