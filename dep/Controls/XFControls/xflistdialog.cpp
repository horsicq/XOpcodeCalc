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

#include "xflistdialog.h"

XFListDialog::XFListDialog(QWidget *pParent) : QDialog(pParent)
{
    m_nValue = 0;
    m_bIsReadonly = false;

    setWindowTitle(tr("Select Value"));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pLabelName = new QLabel(this);
    pMainLayout->addWidget(m_pLabelName);

    QHBoxLayout *pValueLayout = new QHBoxLayout();
    m_pLabelCurrentValue = new QLabel(tr("Current") + QString(":"), this);
    pValueLayout->addWidget(m_pLabelCurrentValue);

    m_pLineEditHex = new QLineEdit(this);
    m_pLineEditHex->setReadOnly(true);
    pValueLayout->addWidget(m_pLineEditHex);

    pMainLayout->addLayout(pValueLayout);

    m_pComboBox = new QComboBox(this);
    pMainLayout->addWidget(m_pComboBox);

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
    connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxChanged(int)));

    resize(450, 180);
}

XFListDialog::~XFListDialog()
{
}

void XFListDialog::setData(const QString &sFieldName, quint64 nValue, XBinary::XIDSTRING *pRecords, qint32 nRecordsSize)
{
    m_nValue = nValue;

    m_pLabelName->setText(sFieldName);

    m_pComboBox->blockSignals(true);
    m_pComboBox->clear();

    qint32 nSelectedIndex = -1;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        quint64 nID = pRecords[i].nID;
        QString sName = pRecords[i].sString;

        QString sLabel = QString("0x%1 - %2").arg(QString::number(nID, 16).toUpper()).arg(sName);
        m_pComboBox->addItem(sLabel, QVariant((qulonglong)nID));

        if (nID == nValue) {
            nSelectedIndex = i;
        }
    }

    if (nSelectedIndex >= 0) {
        m_pComboBox->setCurrentIndex(nSelectedIndex);
    }

    m_pComboBox->blockSignals(false);

    updateDisplay();
}

quint64 XFListDialog::getValue()
{
    return m_nValue;
}

void XFListDialog::setReadonly(bool bIsReadonly)
{
    m_bIsReadonly = bIsReadonly;
    m_pComboBox->setEnabled(!bIsReadonly);
    m_pPushButtonOK->setEnabled(!bIsReadonly);
}

void XFListDialog::onOkClicked()
{
    accept();
}

void XFListDialog::onCancelClicked()
{
    close();
}

void XFListDialog::onComboBoxChanged(qint32 nIndex)
{
    if (nIndex >= 0) {
        m_nValue = m_pComboBox->itemData(nIndex).toULongLong();
        updateDisplay();
    }
}

void XFListDialog::updateDisplay()
{
    m_pLineEditHex->setText(QString("0x%1").arg(QString::number(m_nValue, 16).toUpper()));
}
