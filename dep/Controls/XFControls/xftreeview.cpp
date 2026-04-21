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

#include "xftreeview.h"

XFTreeView::XFTreeView(QWidget *pParent) : QTreeView(pParent)
{
    m_pXBinary = nullptr;
    m_pTreeModel = nullptr;

    setSelectionBehavior(QAbstractItemView::SelectRows);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setWordWrap(false);
}

XFTreeView::~XFTreeView()
{
    clear();
}

void XFTreeView::setData(XBinary *pXBinary, const QList<XBinary::XFHEADER> &listHeaders)
{
    clear();

    m_pXBinary = pXBinary;

    m_pTreeModel = new XFTreeModel(this);
    m_pTreeModel->setData(m_pXBinary, listHeaders);
    setModel(m_pTreeModel);

    adjust();

    expandAll();
}

void XFTreeView::clear()
{
    setModel(nullptr);

    if (m_pTreeModel) {
        delete m_pTreeModel;
        m_pTreeModel = nullptr;
    }

    m_pXBinary = nullptr;
}

void XFTreeView::adjust()
{
    qint32 nColumnCount = header()->count();

    for (qint32 i = 0; i < nColumnCount; i++) {
        resizeColumnToContents(i);
    }
}

XFTreeModel *XFTreeView::getTreeModel()
{
    return m_pTreeModel;
}

XBinary::XFHEADER XFTreeView::getSelectedHeader()
{
    XBinary::XFHEADER result = {};

    QModelIndex currentIndex = this->currentIndex();

    if (currentIndex.isValid() && m_pTreeModel) {
        XFTreeModel::TREEITEM *pItem = m_pTreeModel->itemFromIndex(currentIndex);

        if (pItem) {
            result = pItem->xfHeader;
        }
    }

    return result;
}

void XFTreeView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    QTreeView::currentChanged(current, previous);

    if (current.isValid() && m_pTreeModel) {
        XFTreeModel::TREEITEM *pItem = m_pTreeModel->itemFromIndex(current);

        if (pItem) {
            emit headerSelected(pItem->xfHeader);
        }
    }
}
