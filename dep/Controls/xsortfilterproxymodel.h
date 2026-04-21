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

#ifndef XSORTFILTERPROXYMODEL_H
#define XSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVector>
#include "xmodel.h"

class XSortFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit XSortFilterProxyModel(QObject *pParent = nullptr);
    void setFilters(const QList<QString> &listFilters);
    void setColumnFilter(qint32 nColumn, const QString &sFilter);
    QList<QString> getFilters() const;
    void setSortMethod(qint32 nColumn, XModel::SORT_METHOD sortMethod);
    void setSourceModel(QAbstractItemModel *sourceModel) override;
    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;
    void resetModel();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    void buildSortCache(qint32 nColumn);
    void clearSortCache();

    bool m_bIsXmodel;
    bool m_bIsCustomFilter;
    bool m_bIsCustomSort;
    XModel *m_pXModel;
    QList<QString> m_listFilters;
    QMap<qint32, XModel::SORT_METHOD> m_mapSortMethods;
    bool m_bSortCacheValid;
    XModel::SORT_METHOD m_sortCacheMethod;
    QVector<quint64> m_vecSortCacheHex;
    QVector<QString> m_vecSortCacheStr;
};

#endif  // XSORTFILTERPROXYMODEL_H
