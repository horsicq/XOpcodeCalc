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
#ifndef XMODEL_BINARY_H
#define XMODEL_BINARY_H

#include "xbinary.h"
#include "xmodel.h"

class XModel_Binary : public XModel {
    Q_OBJECT

    enum COLUMN_HEADER {
        COLUMN_HEADER_NAME = 0,
        COLUMN_HEADER_OFFSET,
        COLUMN_HEADER_SIZE,
        COLUMN_HEADER_TYPE,
        COLUMN_HEADER_VALUE,
        COLUMN_HEADER_INFO,
        COLUMN_HEADER_COMMENT,
        __COLUMN_HEADER_SIZE
    };

public:
    XModel_Binary(const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<XBinary::DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles,
                  QObject *pParent);

    virtual QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const;
    virtual QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const;
    void setComments(QList<QString> *pListComments);

private:
    XBinary::DATA_RECORDS_OPTIONS m_dataRecordsOptions;
    QList<XBinary::DATA_RECORD_ROW> *m_pListDataRecords;
    QList<QString> *m_pListTitles;
    QList<QString> *m_pListComments;
};

#endif  // XMODEL_BINARY_H
