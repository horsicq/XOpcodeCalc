/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to do so, subject to the
 * following conditions:
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
#include "xmodel_streams.h"

XModel_Streams::XModel_Streams(QList<XBinary::FPART> *pListFParts, QObject *pParent) : XModel(pParent)
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

void XModel_Streams::_initColumns()
{
    setColumnName(COLUMN_NAME, QObject::tr("Name"));
    setColumnName(COLUMN_OFFSET, QObject::tr("Offset"));
    setColumnName(COLUMN_SIZE, QObject::tr("Size"));
    setColumnName(COLUMN_COMPRESSMETHOD, QObject::tr("Compress"));
    setColumnName(COLUMN_UNCOMPRESSEDSIZE, QObject::tr("U.Size"));

    setColumnAlignment(COLUMN_NAME, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_SIZE, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_COMPRESSMETHOD, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_UNCOMPRESSEDSIZE, Qt::AlignVCenter | Qt::AlignRight);

    setColumnSymbolSize(COLUMN_NAME, 20);
    setColumnSymbolSize(COLUMN_OFFSET, 16);
    setColumnSymbolSize(COLUMN_SIZE, 16);
    setColumnSymbolSize(COLUMN_COMPRESSMETHOD, 10);
    setColumnSymbolSize(COLUMN_UNCOMPRESSEDSIZE, 16);
}

QVariant XModel_Streams::data(const QModelIndex &index, int nRole) const
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
                } else if (nColumn == COLUMN_COMPRESSMETHOD) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_HANDLEMETHOD)) {
                        result = XBinary::getHandleMethods(rec.mapProperties);
                    }
                } else if (nColumn == COLUMN_UNCOMPRESSEDSIZE) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_UNCOMPRESSEDSIZE)) {
                        quint64 nUSz = rec.mapProperties.value(XBinary::FPART_PROP_UNCOMPRESSEDSIZE).toULongLong();
                        result = QString::number(nUSz, 16);
                    }
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nFileOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nFileSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    result = rec.sName;
                }
            }
        }
    }

    return result;
}

QVariant XModel_Streams::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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
