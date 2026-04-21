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

#ifndef XMODEL_MSRECORD_H
#define XMODEL_MSRECORD_H

#include "xbinary.h"
#include "xmodel.h"
#include <algorithm>

class XModel_MSRecord : public XModel {
    Q_OBJECT

public:
    enum COLUMN {
        COLUMN_NUMBER,
        COLUMN_OFFSET,
        COLUMN_ADDRESS,
        COLUMN_REGION,
        COLUMN_SIZE,
        COLUMN_INFO,
        COLUMN_VALUE,
        __COLUMN_SIZE
    };

    XModel_MSRecord(QIODevice *pDevice, const XBinary::_MEMORY_MAP &memoryMap, QVector<XBinary::MS_RECORD> *pListRecods, XBinary::VT valueType,
                    QObject *pParent = nullptr);

    void setValue(XBinary::ENDIAN endian, XBinary::VT valueType, QVariant varValue);
    void setSignaturesList(QList<XBinary::SIGNATUREDB_RECORD> *pListSignatureRecords);

    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    virtual QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const;
    virtual SORT_METHOD getSortMethod(qint32 nColumn);
    virtual bool isCustomFilter();
    virtual bool isCustomSort();
    virtual bool hasSortKeyHex() const;
    virtual quint64 getSortKeyHex(qint32 nRow, qint32 nColumn) const;
    virtual void sortByColumn(qint32 nColumn, Qt::SortOrder order);
    void buildValueCache();
    void clearValueCache();
    bool isValueCacheValid() const;

private:
    quint64 _getRawSortKey(qint32 nDataRow, qint32 nColumn) const;

    QIODevice *m_pDevice;
    XBinary::_MEMORY_MAP m_memoryMap;
    QVector<XBinary::MS_RECORD> *m_pListRecords;
    XBinary::MODE m_modeAddress;
    XBinary::MODE m_modeOffset;
    XBinary::ENDIAN m_endian;
    QVariant m_varValue;
    XBinary::VT m_valueType;
    QString m_sValue;
    QList<XBinary::SIGNATUREDB_RECORD> *m_pListSignatureRecords;
    QVector<QString> m_vecValueCache;
    bool m_bValueCacheValid;
    QVector<qint32> m_vecSortIndex;
};

#endif  // XMODEL_MSRECORD_H
