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

#ifndef XFMODEL_TABLE_H
#define XFMODEL_TABLE_H

#include "xfmodel.h"

class XFModel_table : public XFModel {
    Q_OBJECT

public:
    explicit XFModel_table(QObject *pParent);
    virtual ~XFModel_table() override;

    virtual void setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader) override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setShowOffset(bool bShowOffset);
    void setShowPresentation(bool bShowPresentation);

private:
    enum COLUMN_ENTRY_TYPE {
        CET_NUMBER = 0,
        CET_NAME,
        CET_OFFSET,
        CET_FIELD,
        CET_PRESENTATION
    };

    struct COLUMN_ENTRY {
        COLUMN_ENTRY_TYPE columnEntryType;
        qint32 nIndex;  // field index for CET_FIELD, presentation index for CET_PRESENTATION
    };

    struct PRESENTATION_COLUMN {
        qint32 nFieldIndex;
        PRESENTATION_TYPE presentationType;
        qint32 nDataStIndex;  // only used for PT_DATAST_LIST / PT_DATAST_FLAGS
    };

    void _rebuildColumnMap();

    bool m_bShowOffset;
    bool m_bShowPresentation;
    QList<XBinary::XFRECORD> m_listColumnFields;
    QList<QList<XBinary::XFRECORD>> m_listRowFields;
    QList<QList<QVariant>> m_listTableRowValues;
    QList<PRESENTATION_COLUMN> m_listPresentationColumns;
    QList<COLUMN_ENTRY> m_listColumnMap;
};

#endif  // XFMODEL_TABLE_H
