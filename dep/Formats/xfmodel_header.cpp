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

#include "xfmodel_header.h"

XFModel_header::XFModel_header(QObject *pParent) : XFModel(pParent)
{
}

XFModel_header::~XFModel_header()
{
}

void XFModel_header::setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader)
{
    XFModel::setData(pXBinary, xfHeader);

    beginResetModel();

    m_listHeaderValues.clear();

    if (m_pXBinary) {
        m_listHeaderValues = m_pXBinary->getXFRecordValues(m_xfHeader.listFields, m_xfHeader.xLoc);
    }

    endResetModel();
}

int XFModel_header::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_xfHeader.listFields.count();
}

QVariant XFModel_header::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (!index.isValid()) {
        return result;
    }

    qint32 nRow = index.row();
    qint32 nColumn = index.column();

    if (role == Qt::DisplayRole) {
        if ((nRow >= 0) && (nRow < m_xfHeader.listFields.count())) {
            XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nRow);

            if (nColumn == COLUMN_NAME) {
                result = xfRecord.sName;
            } else if (nColumn == COLUMN_OFFSET) {
                result = XBinary::valueToHexEx(xfRecord.nOffset);
            } else if (nColumn == COLUMN_SIZE) {
                result = QString::number(xfRecord.nSize);
            } else if (nColumn == COLUMN_TYPE) {
                result = XBinary::valueTypeToString(xfRecord.valueType, xfRecord.nSize);
            } else if (nColumn == COLUMN_VALUE) {
                if (nRow < m_listHeaderValues.count()) {
                    result = valueToString(xfRecord, m_listHeaderValues.at(nRow));
                }
            } else if (nColumn == COLUMN_COMMENT) {
                if (nRow < m_listHeaderValues.count()) {
                    result = commentForField(nRow, m_listHeaderValues.at(nRow));
                }
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        if ((nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_SIZE) || (nColumn == COLUMN_VALUE)) {
            result = (int)(Qt::AlignRight | Qt::AlignVCenter);
        }
    }

    return result;
}
