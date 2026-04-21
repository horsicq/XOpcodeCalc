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

#include "xfflagsdialog.h"

XFFlagsDialog::XFFlagsDialog(QWidget *pParent) : QDialog(pParent)
{
    m_nValue = 0;
    m_bIsReadonly = false;
    m_bUpdating = false;

    setWindowTitle(tr("Edit Flags"));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pLabelName = new QLabel(this);
    pMainLayout->addWidget(m_pLabelName);

    QHBoxLayout *pValueLayout = new QHBoxLayout();
    m_pLabelValue = new QLabel(tr("Value") + QString(":"), this);
    pValueLayout->addWidget(m_pLabelValue);

    m_pLineEditHex = new QLineEdit(this);
    m_pLineEditHex->setReadOnly(true);
    pValueLayout->addWidget(m_pLineEditHex);

    pMainLayout->addLayout(pValueLayout);

    QScrollArea *pScrollArea = new QScrollArea(this);
    pScrollArea->setWidgetResizable(true);

    QWidget *pScrollWidget = new QWidget(pScrollArea);
    QVBoxLayout *pScrollLayout = new QVBoxLayout(pScrollWidget);
    pScrollWidget->setLayout(pScrollLayout);
    pScrollArea->setWidget(pScrollWidget);

    pMainLayout->addWidget(pScrollArea);

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

    resize(500, 400);
}

XFFlagsDialog::~XFFlagsDialog()
{
}

void XFFlagsDialog::setData(const QString &sFieldName, quint64 nValue, XBinary::XIDSTRING *pRecords, qint32 nRecordsSize)
{
    m_nValue = nValue;

    m_pLabelName->setText(sFieldName);

    // Clear existing checkboxes
    for (qint32 i = 0; i < m_listCheckBoxes.count(); i++) {
        delete m_listCheckBoxes.at(i);
    }

    m_listCheckBoxes.clear();
    m_listFlagValues.clear();

    // Find scroll area layout
    QScrollArea *pScrollArea = findChild<QScrollArea *>();

    if (pScrollArea && pScrollArea->widget()) {
        QVBoxLayout *pScrollLayout = qobject_cast<QVBoxLayout *>(pScrollArea->widget()->layout());

        if (pScrollLayout) {
            for (qint32 i = 0; i < nRecordsSize; i++) {
                quint64 nFlagValue = pRecords[i].nID;
                QString sName = pRecords[i].sString;

                QString sLabel = QString("0x%1 - %2").arg(QString::number(nFlagValue, 16).toUpper()).arg(sName);

                QCheckBox *pCheckBox = new QCheckBox(sLabel, pScrollArea->widget());
                bool bChecked = (nValue & nFlagValue) == nFlagValue;
                pCheckBox->setChecked(bChecked);

                connect(pCheckBox, SIGNAL(toggled(bool)), this, SLOT(onCheckBoxToggled(bool)));

                pScrollLayout->addWidget(pCheckBox);

                m_listCheckBoxes.append(pCheckBox);
                m_listFlagValues.append(nFlagValue);
            }

            pScrollLayout->addStretch();
        }
    }

    updateDisplay();
}

quint64 XFFlagsDialog::getValue()
{
    return m_nValue;
}

void XFFlagsDialog::setReadonly(bool bIsReadonly)
{
    m_bIsReadonly = bIsReadonly;

    for (qint32 i = 0; i < m_listCheckBoxes.count(); i++) {
        m_listCheckBoxes.at(i)->setEnabled(!bIsReadonly);
    }

    m_pPushButtonOK->setEnabled(!bIsReadonly);
}

void XFFlagsDialog::onOkClicked()
{
    accept();
}

void XFFlagsDialog::onCancelClicked()
{
    close();
}

void XFFlagsDialog::onCheckBoxToggled(bool bChecked)
{
    Q_UNUSED(bChecked)

    if (m_bUpdating) {
        return;
    }

    m_nValue = 0;

    for (qint32 i = 0; i < m_listCheckBoxes.count(); i++) {
        if (m_listCheckBoxes.at(i)->isChecked()) {
            m_nValue |= m_listFlagValues.at(i);
        }
    }

    updateDisplay();
}

void XFFlagsDialog::updateDisplay()
{
    m_bUpdating = true;
    m_pLineEditHex->setText(QString("0x%1").arg(QString::number(m_nValue, 16).toUpper()));
    m_bUpdating = false;
}
