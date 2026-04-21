/* Copyright (c) 2024-2026 hors<horsicq@gmail.com>
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
#include "xheaderview.h"

XHeaderView::XHeaderView(QWidget *pParent) : QHeaderView(Qt::Horizontal, pParent)
{
    connect(this, SIGNAL(sectionResized(int, int, int)), this, SLOT(onSectionResized(int, int, int)));
    connect(this, SIGNAL(sectionClicked(int)), this, SLOT(onSectionClicked(int)));

    setSectionsClickable(true);
}

XHeaderView::~XHeaderView()
{
    qint32 nCount = m_listLineEdits.count();

    for (qint32 i = 0; i < nCount; i++) {
        delete m_listLineEdits.at(i);
    }

    m_listLineEdits.clear();
}

QSize XHeaderView::sizeHint() const
{
    QSize baseSize = QHeaderView::sizeHint();

    if (m_listLineEdits.count()) {
        baseSize.setHeight(baseSize.height() + 4 + m_listLineEdits.at(0)->sizeHint().height());
    }

    return baseSize;
}

void XHeaderView::setNumberOfFilters(qint32 nNumberOfFilters)
{
    qint32 nOldCount = m_listLineEdits.count();

    for (qint32 i = 0; i < nOldCount; i++) {
        delete m_listLineEdits.at(i);
    }

    m_listLineEdits.clear();

    for (qint32 i = 0; i < nNumberOfFilters; i++) {
        QLineEdit *pLineEdit = new QLineEdit(this);
        pLineEdit->setPlaceholderText(tr("Filter"));
        pLineEdit->setToolTip(tr("Filter"));
        connect(pLineEdit, SIGNAL(textChanged(QString)), this, SLOT(_textChanged(QString)));
        m_listLineEdits.append(pLineEdit);
    }

    adjustPositions();
}

void XHeaderView::clearFilters()
{
    qint32 nCount = m_listLineEdits.count();

    for (qint32 i = 0; i < nCount; i++) {
        m_listLineEdits.at(i)->clear();
    }
}

void XHeaderView::updateGeometries()
{
    if (m_listLineEdits.count()) {
        qint32 nLineEditHeight = m_listLineEdits.at(0)->sizeHint().height();
        setViewportMargins(0, 0, 0, nLineEditHeight + 4);
    } else {
        setViewportMargins(0, 0, 0, 0);
    }

    QHeaderView::updateGeometries();
    adjustPositions();
}

void XHeaderView::adjustPositions()
{
    qint32 nCount = m_listLineEdits.count();

    QSize baseSize = QHeaderView::sizeHint();

    for (qint32 i = 0; i < nCount; i++) {
        qint32 nLineEditHeight = m_listLineEdits.at(i)->sizeHint().height();

        m_listLineEdits.at(i)->move(sectionPosition(i) - offset(), baseSize.height() + 2);
        m_listLineEdits.at(i)->resize(sectionSize(i) - 2, nLineEditHeight);
        m_listLineEdits.at(i)->show();
    }
}

QList<QString> XHeaderView::getFilters()
{
    QList<QString> listResult;

    qint32 nCount = m_listLineEdits.count();

    for (qint32 i = 0; i < nCount; i++) {
        listResult.append(m_listLineEdits.at(i)->text());
    }

    return listResult;
}

void XHeaderView::setFilterEnabled(qint32 nColumn, bool bFilterEnabled)
{
    // TODO
    if ((nColumn >= 0) && (nColumn < m_listLineEdits.count())) {
        m_listLineEdits.at(nColumn)->setReadOnly(!bFilterEnabled);
    }
}

void XHeaderView::_textChanged(const QString &sText)
{
    Q_UNUSED(sText)

    emit filterChanged();
}

void XHeaderView::onSectionResized(int i, int nOldSize, int nNewSize)
{
    Q_UNUSED(i)
    Q_UNUSED(nOldSize)
    Q_UNUSED(nNewSize)

    adjustPositions();
}

void XHeaderView::onSectionClicked(int logicalIndex)
{
    Qt::SortOrder sortOrder = sortIndicatorOrder();

    if (sortIndicatorSection() == logicalIndex) {
        sortOrder = (sortOrder == Qt::AscendingOrder) ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        sortOrder = Qt::AscendingOrder;
    }

    emit sortIndicatorChanged(logicalIndex, sortOrder);
}
