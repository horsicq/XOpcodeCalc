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

#ifndef XMODEL_H
#define XMODEL_H

#include <QAbstractItemModel>
#include <QVector>

class XModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum SORT_METHOD {
        SORT_METHOD_DEFAULT,
        SORT_METHOD_HEX,
    };

    enum USERROLE {
        USERROLE_ORIGINDEX = 0,
        USERROLE_SIZE,
        USERROLE_OFFSET,
        USERROLE_ADDRESS,
        USERROLE_STRING1,
        USERROLE_STRING2
    };

    explicit XModel(QObject *pParent);
    virtual ~XModel() override;

    void setColumnSymbolSize(qint32 nColumn, qint32 nValue);
    qint32 getColumnSymbolSize(qint32 nColumn) const;
    void setColumnAlignment(qint32 nColumn, qint32 flag);
    qint32 getColumnAlignment(qint32 nColumn) const;
    void setColumnName(qint32 nColumn, const QString &sName);
    QString getColumnName(qint32 nColumn) const;
    virtual SORT_METHOD getSortMethod(qint32 nColumn);
    virtual bool isCustomFilter();
    virtual bool isCustomSort();
    virtual bool hasSortKeyHex() const;
    virtual quint64 getSortKeyHex(qint32 nRow, qint32 nColumn) const;
    virtual void sortByColumn(qint32 nColumn, Qt::SortOrder order);
    void setRowHidden(qint32 nRow, bool bState);
    void clearRowHidden();
    qint32 getVisibleRowCount() const;
    void setRowPrio(qint32 nRow, quint64 nPrio);
    bool isRowHidden(qint32 nRow);
    quint64 getRowPrio(qint32 nRow);
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    void _setRowCount(qint32 nRowCount);
    void _setColumnCount(qint32 nColumnCount);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    void adjustColumnToContent(qint32 nColumn, bool bHeader);
    void adjustColumnsToContent(bool bHeader);

    virtual QString toXML() const;
    virtual QString toJSON() const;

private:
    QVector<bool> m_vecRowHidden;
    QHash<qint32, quint64> m_hashRowPrio;
    QHash<qint32, qint32> m_hashColumnSymbolSize;
    QHash<qint32, qint32> m_hashColumnAlignment;
    QHash<qint32, QString> m_hashColumnName;
    qint32 m_nRowCount;
    qint32 m_nColumnCount;
};

#endif  // XMODEL_H
