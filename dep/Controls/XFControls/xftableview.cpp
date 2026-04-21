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

#include "xftableview.h"

XFTableView::XFTableView(QWidget *pParent) : XTableView(pParent)
{
    m_pXBinary = nullptr;
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onCustomContextMenuRequested(QPoint)));
}

XFTableView::~XFTableView()
{
    clear();
}

void XFTableView::setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader)
{
    // Don't call clear() here — setCustomModel() handles old model deletion
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;

    m_pXBinary = pXBinary;
    m_xfHeader = xfHeader;

    if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
        m_pHeaderModel = new XFModel_header(this);
        m_pHeaderModel->setData(m_pXBinary, m_xfHeader);
        setCustomModel(m_pHeaderModel, true);
        resizeColumnsToContents();
    } else if (m_xfHeader.xfType == XBinary::XFTYPE_TABLE) {
        m_pTableModel = new XFModel_table(this);
        m_pTableModel->setShowPresentation(true);
        m_pTableModel->setData(m_pXBinary, m_xfHeader);
        setCustomModel(m_pTableModel, true);
    }
}

void XFTableView::clear()
{
    XTableView::clear();
    // Models are owned by Qt parent-child; don't double-delete
    m_pHeaderModel = nullptr;
    m_pTableModel = nullptr;
    m_pXBinary = nullptr;
}

void XFTableView::adjust()
{
    resizeColumnsToContents();
    resizeRowsToContents();
}

void XFTableView::setShowOffset(bool bShowOffset)
{
    if (m_pTableModel) {
        m_pTableModel->setShowOffset(bShowOffset);
        adjust();
    }
}

void XFTableView::setShowPresentation(bool bShowPresentation)
{
    if (m_pTableModel) {
        m_pTableModel->setShowPresentation(bShowPresentation);
        adjust();
    }
}

void XFTableView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    XTableView::currentChanged(current, previous);

    if (current.isValid()) {
        qint32 nRow = current.row();

        if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
            if ((nRow >= 0) && (nRow < m_xfHeader.listFields.count())) {
                XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nRow);
                QList<QVariant> listValues = m_pXBinary->getXFRecordValues(m_xfHeader.listFields, m_xfHeader.xLoc);
                quint64 nValue = 0;
                if (nRow < listValues.count()) {
                    nValue = listValues.at(nRow).toULongLong();
                }
                emit fieldSelected(nRow, nValue, xfRecord);
            }
        } else if (m_xfHeader.xfType == XBinary::XFTYPE_TABLE) {
            emit rowSelected(nRow);
        }
    }
}

void XFTableView::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    qint32 nRow = index.row();

    if (m_xfHeader.xfType == XBinary::XFTYPE_HEADER) {
        if ((nRow >= 0) && (nRow < m_xfHeader.listFields.count())) {
            XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nRow);
            QList<QVariant> listValues = m_pXBinary->getXFRecordValues(m_xfHeader.listFields, m_xfHeader.xLoc);
            quint64 nValue = 0;
            if (nRow < listValues.count()) {
                nValue = listValues.at(nRow).toULongLong();
            }
            emit fieldDoubleClicked(nRow, nValue, xfRecord);
        }
    }
}

void XFTableView::onCustomContextMenuRequested(const QPoint &pos)
{
    Q_UNUSED(pos)
    // TODO: context menu for copy, edit, etc.
}
