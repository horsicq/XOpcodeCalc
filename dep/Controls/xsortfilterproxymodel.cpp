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

#include "xsortfilterproxymodel.h"

XSortFilterProxyModel::XSortFilterProxyModel(QObject *pParent) : QSortFilterProxyModel(pParent)
{
    m_bIsXmodel = false;
    m_bIsCustomFilter = false;
    m_bIsCustomSort = false;
    m_pXModel = nullptr;
    m_bSortCacheValid = false;
    m_sortCacheMethod = XModel::SORT_METHOD_DEFAULT;
}

void XSortFilterProxyModel::setFilters(const QList<QString> &listFilters)
{
    m_listFilters = listFilters;
}

void XSortFilterProxyModel::setColumnFilter(qint32 nColumn, const QString &sFilter)
{
    while (m_listFilters.count() <= nColumn) {
        m_listFilters.append(QString());
    }

    m_listFilters[nColumn] = sFilter;
}

QList<QString> XSortFilterProxyModel::getFilters() const
{
    return m_listFilters;
}

void XSortFilterProxyModel::setSortMethod(qint32 nColumn, XModel::SORT_METHOD sortMethod)
{
    m_mapSortMethods.insert(nColumn, sortMethod);
}

void XSortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    m_listFilters.clear();

    m_pXModel = dynamic_cast<XModel *>(sourceModel);

    if (m_pXModel) {
        qint32 nNumberOfColumns = m_pXModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            XModel::SORT_METHOD sortMethod = m_pXModel->getSortMethod(i);

            setSortMethod(i, sortMethod);
        }

        m_bIsXmodel = true;
        m_bIsCustomFilter = m_pXModel->isCustomFilter();
        m_bIsCustomSort = m_pXModel->isCustomSort();
    } else {
        m_bIsXmodel = false;
        m_bIsCustomFilter = false;
        m_bIsCustomSort = false;
    }

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

QVariant XSortFilterProxyModel::data(const QModelIndex &index, int nRole) const
{
    return QSortFilterProxyModel::data(index, nRole);
}

void XSortFilterProxyModel::sort(int column, Qt::SortOrder order)
{
    if (m_bIsCustomSort && m_pXModel) {
        m_pXModel->sortByColumn(column, order);
    } else {
        buildSortCache(column);
        QSortFilterProxyModel::sort(column, order);
        clearSortCache();
    }
}

void XSortFilterProxyModel::resetModel()
{
    beginResetModel();
    endResetModel();
}

bool XSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    bool bResult = true;

    if (m_bIsCustomFilter) {
        bResult = !(m_pXModel->isRowHidden(sourceRow));
    } else {
        qint32 nCount = m_listFilters.count();

        for (qint32 i = 0; i < nCount; i++) {
            QString sFilter = m_listFilters.at(i);
            if (sFilter != "") {
                QModelIndex index = sourceModel()->index(sourceRow, i, sourceParent);

                if (index.isValid()) {
                    QString sValue = sourceModel()->data(index).toString();

                    if (!sValue.contains(sFilter, Qt::CaseInsensitive)) {
                        bResult = false;
                        break;
                    }
                }
            }
        }
    }

    return bResult;
}

bool XSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    bool bResult = false;

    if (m_bIsCustomSort) {
        bResult = (left.row() < right.row());
    } else if (m_bSortCacheValid) {
        qint32 nLeftRow = left.row();
        qint32 nRightRow = right.row();

        if (m_sortCacheMethod == XModel::SORT_METHOD_HEX) {
            bResult = m_vecSortCacheHex.at(nLeftRow) < m_vecSortCacheHex.at(nRightRow);
        } else {
            bResult = m_vecSortCacheStr.at(nLeftRow) < m_vecSortCacheStr.at(nRightRow);
        }
    } else {
        qint32 nColumn = left.column();

        XModel::SORT_METHOD sortMethod = m_mapSortMethods.value(nColumn, XModel::SORT_METHOD_DEFAULT);

        if (sortMethod == XModel::SORT_METHOD_HEX) {
            if (m_bIsXmodel && m_pXModel && m_pXModel->hasSortKeyHex()) {
                bResult = m_pXModel->getSortKeyHex(left.row(), nColumn) < m_pXModel->getSortKeyHex(right.row(), nColumn);
            } else {
                QString sLeft = left.data().toString();
                QString sRight = right.data().toString();

                sLeft = sLeft.remove(" ");
                sRight = sRight.remove(" ");

                bResult = sLeft.toULongLong(0, 16) < sRight.toULongLong(0, 16);
            }
        } else {
            bResult = QSortFilterProxyModel::lessThan(left, right);
        }
    }

    return bResult;
}

void XSortFilterProxyModel::buildSortCache(qint32 nColumn)
{
    QAbstractItemModel *pSource = sourceModel();

    if (!pSource) {
        m_bSortCacheValid = false;
        return;
    }

    qint32 nRowCount = pSource->rowCount();
    m_sortCacheMethod = m_mapSortMethods.value(nColumn, XModel::SORT_METHOD_DEFAULT);

    if (m_sortCacheMethod == XModel::SORT_METHOD_HEX) {
        m_vecSortCacheHex.resize(nRowCount);

        if (m_bIsXmodel && m_pXModel && m_pXModel->hasSortKeyHex()) {
            for (qint32 i = 0; i < nRowCount; i++) {
                m_vecSortCacheHex[i] = m_pXModel->getSortKeyHex(i, nColumn);
            }
        } else {
            for (qint32 i = 0; i < nRowCount; i++) {
                QModelIndex idx = pSource->index(i, nColumn);
                QString sVal = pSource->data(idx).toString().remove(" ");
                m_vecSortCacheHex[i] = sVal.toULongLong(nullptr, 16);
            }
        }
    } else {
        m_vecSortCacheStr.resize(nRowCount);

        for (qint32 i = 0; i < nRowCount; i++) {
            QModelIndex idx = pSource->index(i, nColumn);
            m_vecSortCacheStr[i] = pSource->data(idx).toString();
        }
    }

    m_bSortCacheValid = true;
}

void XSortFilterProxyModel::clearSortCache()
{
    m_vecSortCacheHex.clear();
    m_vecSortCacheStr.clear();
    m_bSortCacheValid = false;
}
