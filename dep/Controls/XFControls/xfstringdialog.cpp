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

#include "xfstringdialog.h"

XFStringDialog::XFStringDialog(QWidget *pParent) : QDialog(pParent)
{
    m_nMaxLength = 0;
    m_bIsReadonly = false;

    setWindowTitle(tr("Edit String"));

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);

    m_pLabelName = new QLabel(this);
    pMainLayout->addWidget(m_pLabelName);

    m_pLineEditValue = new QLineEdit(this);
    pMainLayout->addWidget(m_pLineEditValue);

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

    resize(400, 120);
}

XFStringDialog::~XFStringDialog()
{
}

void XFStringDialog::setData(const QString &sFieldName, const QString &sValue, qint32 nMaxLength)
{
    m_sValue = sValue;
    m_nMaxLength = nMaxLength;

    m_pLabelName->setText(sFieldName);
    m_pLineEditValue->setText(sValue);

    if (nMaxLength > 0) {
        m_pLineEditValue->setMaxLength(nMaxLength);
    }
}

QString XFStringDialog::getValue()
{
    return m_sValue;
}

void XFStringDialog::setReadonly(bool bIsReadonly)
{
    m_bIsReadonly = bIsReadonly;
    m_pLineEditValue->setReadOnly(bIsReadonly);
    m_pPushButtonOK->setEnabled(!bIsReadonly);
}

void XFStringDialog::onOkClicked()
{
    m_sValue = m_pLineEditValue->text();
    accept();
}

void XFStringDialog::onCancelClicked()
{
    close();
}
