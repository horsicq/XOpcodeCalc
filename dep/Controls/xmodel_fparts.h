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
#ifndef XMODEL_FPARTS_H
#define XMODEL_FPARTS_H

#include "xbinary.h"
#include "xmodel.h"

class XModel_FPARTS : public XModel {
    Q_OBJECT

public:
    enum COLUMN {
        COLUMN_NAME = 0,
        COLUMN_OFFSET,
        COLUMN_SIZE,
        COLUMN_VADDRESS,
        COLUMN_VSIZE,
        COLUMN_PART,
        __COLUMN_COUNT
    };

    explicit XModel_FPARTS(QList<XBinary::FPART> *pListFParts, QObject *pParent = nullptr);

    QVariant data(const QModelIndex &index, int nRole = Qt::DisplayRole) const override;
    QVariant headerData(int nSection, Qt::Orientation orientation, int nRole = Qt::DisplayRole) const override;
    virtual SORT_METHOD getSortMethod(qint32 nColumn) override;
    virtual bool isCustomFilter() override;
    virtual bool isCustomSort() override;

private:
    QList<XBinary::FPART> *m_pListFParts;
    void _initColumns();
};

#endif  // XMODEL_FPARTS_H
