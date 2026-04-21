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

#include "xfvaluedialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

XFValueDialog::XFValueDialog(QWidget *pParent) : QDialog(pParent)
{
    m_nValue = 0;
    m_valueType = XBinary::VT_UINT32;
    m_bIsReadonly = false;
    m_bUpdating = false;

    setWindowTitle(tr("Edit Value"));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pLabelName = new QLabel(this);
    pMainLayout->addWidget(m_pLabelName);

    QGridLayout *pGridLayout = new QGridLayout();

    m_pLabelType = new QLabel(this);
    pGridLayout->addWidget(m_pLabelType, 0, 0, 1, 2);

    QLabel *pLabelHex = new QLabel(tr("Hex") + QString(":"), this);
    pGridLayout->addWidget(pLabelHex, 1, 0);

    m_pLineEditHex = new QLineEdit(this);
    m_pLineEditHex->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9a-fA-F]*"), this));
    pGridLayout->addWidget(m_pLineEditHex, 1, 1);

    QLabel *pLabelDec = new QLabel(tr("Dec") + QString(":"), this);
    pGridLayout->addWidget(pLabelDec, 2, 0);

    m_pLineEditDec = new QLineEdit(this);
    m_pLineEditDec->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    pGridLayout->addWidget(m_pLineEditDec, 2, 1);

    pMainLayout->addLayout(pGridLayout);

    QHBoxLayout *pButtonLayout = new QHBoxLayout();
    pButtonLayout->addStretch();

    m_pPushButtonOK = new QPushButton(tr("OK"), this);
    m_pPushButtonCancel = new QPushButton(tr("Cancel"), this);

    pButtonLayout->addWidget(m_pPushButtonOK);
    pButtonLayout->addWidget(m_pPushButtonCancel);

    pMainLayout->addLayout(pButtonLayout);

    setLayout(pMainLayout);

    connect(m_pPushButtonOK, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    connect(m_pPushButtonCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
    connect(m_pLineEditHex, SIGNAL(textChanged(QString)), this, SLOT(onHexChanged(QString)));
    connect(m_pLineEditDec, SIGNAL(textChanged(QString)), this, SLOT(onDecChanged(QString)));

    resize(400, 180);
}

XFValueDialog::~XFValueDialog()
{
}

void XFValueDialog::setData(const QString &sFieldName, quint64 nValue, XBinary::VT valueType)
{
    m_nValue = nValue;
    m_valueType = valueType;

    m_pLabelName->setText(sFieldName);

    QString sTypeName;

    if (valueType == XBinary::VT_UINT8) {
        sTypeName = "BYTE (8-bit)";
    } else if (valueType == XBinary::VT_UINT16) {
        sTypeName = "WORD (16-bit)";
    } else if (valueType == XBinary::VT_UINT32) {
        sTypeName = "DWORD (32-bit)";
    } else if (valueType == XBinary::VT_UINT64) {
        sTypeName = "QWORD (64-bit)";
    } else {
        sTypeName = "UNKNOWN";
    }

    m_pLabelType->setText(tr("Type") + QString(": %1").arg(sTypeName));

    m_nValue = maskValue(nValue);

    updateDisplay();
}

quint64 XFValueDialog::getValue()
{
    return m_nValue;
}

void XFValueDialog::setReadonly(bool bIsReadonly)
{
    m_bIsReadonly = bIsReadonly;
    m_pLineEditHex->setReadOnly(bIsReadonly);
    m_pLineEditDec->setReadOnly(bIsReadonly);
    m_pPushButtonOK->setEnabled(!bIsReadonly);
}

void XFValueDialog::onOkClicked()
{
    accept();
}

void XFValueDialog::onCancelClicked()
{
    close();
}

void XFValueDialog::onHexChanged(const QString &sText)
{
    if (m_bUpdating) {
        return;
    }

    bool bOk = false;
    quint64 nNewValue = sText.toULongLong(&bOk, 16);

    if (bOk) {
        m_nValue = maskValue(nNewValue);

        m_bUpdating = true;
        m_pLineEditDec->setText(QString::number(m_nValue));
        m_bUpdating = false;
    }
}

void XFValueDialog::onDecChanged(const QString &sText)
{
    if (m_bUpdating) {
        return;
    }

    bool bOk = false;
    quint64 nNewValue = sText.toULongLong(&bOk, 10);

    if (bOk) {
        m_nValue = maskValue(nNewValue);

        m_bUpdating = true;
        m_pLineEditHex->setText(QString::number(m_nValue, 16).toUpper());
        m_bUpdating = false;
    }
}

void XFValueDialog::updateDisplay()
{
    m_bUpdating = true;
    m_pLineEditHex->setText(QString::number(m_nValue, 16).toUpper());
    m_pLineEditDec->setText(QString::number(m_nValue));
    m_bUpdating = false;
}

quint64 XFValueDialog::maskValue(quint64 nValue)
{
    if (m_valueType == XBinary::VT_UINT8) {
        return nValue & 0xFF;
    } else if (m_valueType == XBinary::VT_UINT16) {
        return nValue & 0xFFFF;
    } else if (m_valueType == XBinary::VT_UINT32) {
        return nValue & 0xFFFFFFFF;
    }

    return nValue;
}
