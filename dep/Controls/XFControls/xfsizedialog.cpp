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

#include "xfsizedialog.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

XFSizeDialog::XFSizeDialog(QWidget *pParent) : QDialog(pParent)
{
    m_nValue = 0;
    m_valueType = XBinary::VT_UINT32;
    m_bIsReadonly = false;
    m_bUpdating = false;

    setWindowTitle(tr("Edit Size"));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pLabelName = new QLabel(this);
    pMainLayout->addWidget(m_pLabelName);

    QGridLayout *pGridLayout = new QGridLayout();

    QLabel *pLabelHex = new QLabel(tr("Hex") + QString(":"), this);
    pGridLayout->addWidget(pLabelHex, 0, 0);

    m_pLineEditHex = new QLineEdit(this);
    m_pLineEditHex->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9a-fA-F]*"), this));
    pGridLayout->addWidget(m_pLineEditHex, 0, 1);

    QLabel *pLabelDec = new QLabel(tr("Dec") + QString(":"), this);
    pGridLayout->addWidget(pLabelDec, 1, 0);

    m_pLineEditDec = new QLineEdit(this);
    m_pLineEditDec->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]*"), this));
    pGridLayout->addWidget(m_pLineEditDec, 1, 1);

    QLabel *pLabelSize = new QLabel(tr("Size") + QString(":"), this);
    pGridLayout->addWidget(pLabelSize, 2, 0);

    m_pLabelHuman = new QLabel(this);
    pGridLayout->addWidget(m_pLabelHuman, 2, 1);

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

    resize(400, 200);
}

XFSizeDialog::~XFSizeDialog()
{
}

void XFSizeDialog::setData(const QString &sFieldName, quint64 nValue, XBinary::VT valueType)
{
    m_valueType = valueType;
    m_nValue = maskValue(nValue);

    m_pLabelName->setText(sFieldName);

    updateDisplay();
}

quint64 XFSizeDialog::getValue()
{
    return m_nValue;
}

void XFSizeDialog::setReadonly(bool bIsReadonly)
{
    m_bIsReadonly = bIsReadonly;
    m_pLineEditHex->setReadOnly(bIsReadonly);
    m_pLineEditDec->setReadOnly(bIsReadonly);
    m_pPushButtonOK->setEnabled(!bIsReadonly);
}

void XFSizeDialog::onOkClicked()
{
    accept();
}

void XFSizeDialog::onCancelClicked()
{
    close();
}

void XFSizeDialog::onHexChanged(const QString &sText)
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
        m_pLabelHuman->setText(sizeToHumanString(m_nValue));
        m_bUpdating = false;
    }
}

void XFSizeDialog::onDecChanged(const QString &sText)
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
        m_pLabelHuman->setText(sizeToHumanString(m_nValue));
        m_bUpdating = false;
    }
}

void XFSizeDialog::updateDisplay()
{
    m_bUpdating = true;
    m_pLineEditHex->setText(QString::number(m_nValue, 16).toUpper());
    m_pLineEditDec->setText(QString::number(m_nValue));
    m_pLabelHuman->setText(sizeToHumanString(m_nValue));
    m_bUpdating = false;
}

quint64 XFSizeDialog::maskValue(quint64 nValue)
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

QString XFSizeDialog::sizeToHumanString(quint64 nSize)
{
    QString sResult;

    if (nSize < 1024) {
        sResult = QString("%1 bytes").arg(nSize);
    } else if (nSize < (quint64)1024 * 1024) {
        double dKiB = (double)nSize / 1024.0;
        sResult = QString("%1 KiB (%2 bytes)").arg(dKiB, 0, 'f', 2).arg(nSize);
    } else if (nSize < (quint64)1024 * 1024 * 1024) {
        double dMiB = (double)nSize / (1024.0 * 1024.0);
        sResult = QString("%1 MiB (%2 bytes)").arg(dMiB, 0, 'f', 2).arg(nSize);
    } else {
        double dGiB = (double)nSize / (1024.0 * 1024.0 * 1024.0);
        sResult = QString("%1 GiB (%2 bytes)").arg(dGiB, 0, 'f', 2).arg(nSize);
    }

    return sResult;
}
