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
#include "xmodel_fparts.h"

XModel_FPARTS::XModel_FPARTS(QList<XBinary::FPART> *pListFParts, QObject *pParent) : XModel(pParent)
{
    m_pListFParts = pListFParts;
    if (pListFParts) {
        _setRowCount(pListFParts->count());
    } else {
        _setRowCount(0);
    }
    _setColumnCount(__COLUMN_COUNT);
    _initColumns();
}

void XModel_FPARTS::_initColumns()
{
    setColumnName(COLUMN_NAME, QObject::tr("Name"));
    setColumnName(COLUMN_OFFSET, QObject::tr("Offset"));
    setColumnName(COLUMN_SIZE, QObject::tr("Size"));
    setColumnName(COLUMN_VADDRESS, QObject::tr("Address"));
    setColumnName(COLUMN_VSIZE, QObject::tr("V.Size"));
    setColumnName(COLUMN_PART, QObject::tr("Part"));

    setColumnAlignment(COLUMN_NAME, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_SIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_VADDRESS, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_VSIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_PART, Qt::AlignVCenter | Qt::AlignLeft);

    setColumnSymbolSize(COLUMN_NAME, 20);
    setColumnSymbolSize(COLUMN_OFFSET, 16);
    setColumnSymbolSize(COLUMN_SIZE, 16);
    setColumnSymbolSize(COLUMN_VADDRESS, 16);
    setColumnSymbolSize(COLUMN_VSIZE, 16);
    setColumnSymbolSize(COLUMN_PART, 12);
}

QVariant XModel_FPARTS::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListFParts) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListFParts->count()) {
            qint32 nColumn = index.column();
            const XBinary::FPART &rec = m_pListFParts->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_NAME) {
                    result = rec.sName;
                } else if (nColumn == COLUMN_OFFSET) {
                    result = QString::number(rec.nFileOffset, 16);
                } else if (nColumn == COLUMN_SIZE) {
                    result = QString::number(rec.nFileSize, 16);
                } else if (nColumn == COLUMN_VADDRESS) {
                    if (rec.nVirtualAddress != (XADDR)-1) {
                        result = QString::number(rec.nVirtualAddress, 16);
                    }
                } else if (nColumn == COLUMN_VSIZE) {
                    if (rec.nVirtualAddress != (XADDR)-1) {
                        result = QString::number(rec.nVirtualSize, 16);
                    }
                } else if (nColumn == COLUMN_PART) {
                    result = XBinary::recordFilePartIdToString(rec.filePart);
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                // Expose some raw values for sorting/filtering
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nFileOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nFileSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_ADDRESS)) {
                    result = (qulonglong)rec.nVirtualAddress;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    result = rec.sName;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING2)) {
                    result = XBinary::recordFilePartIdToString(rec.filePart);
                }
            }
        }
    }

    return result;
}

QVariant XModel_FPARTS::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            result = getColumnName(nSection);
        } else if (nRole == Qt::TextAlignmentRole) {
            result = getColumnAlignment(nSection);
        }
    }

    return result;
}

XModel::SORT_METHOD XModel_FPARTS::getSortMethod(qint32 nColumn)
{
    SORT_METHOD result = SORT_METHOD_DEFAULT;

    if ((nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_SIZE) || (nColumn == COLUMN_VADDRESS) || (nColumn == COLUMN_VSIZE)) {
        result = SORT_METHOD_HEX;
    }

    return result;
}

bool XModel_FPARTS::isCustomFilter()
{
    return false;
}

bool XModel_FPARTS::isCustomSort()
{
    return false;
}
