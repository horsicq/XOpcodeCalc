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
#include "xmodel_archiverecords.h"

XModel_ArchiveRecords::XModel_ArchiveRecords(const QList<XBinary::FPART_PROP> &listColumns, QList<XBinary::ARCHIVERECORD> *pListArchiveRecords, QObject *pParent)
    : XModel(pParent)
{
    m_listColumns = listColumns;

    m_pListArchiveRecords = pListArchiveRecords;
    if (pListArchiveRecords) {
        _setRowCount(pListArchiveRecords->count());
    } else {
        _setRowCount(0);
    }

    _setColumnCount(m_listColumns.count());
    _initColumns();
}

void XModel_ArchiveRecords::_initColumns()
{
    qint32 nNumberOfRecords = m_listColumns.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XBinary::FPART_PROP fpartProp = m_listColumns.at(i);

        QString sName;
        qint32 nFlags = Qt::AlignVCenter | Qt::AlignLeft;
        qint32 nSymbolSize = 16;

        if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) sName = QObject::tr("Name");
        else if (fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE) sName = QObject::tr("Compressed size");
        else if (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE) sName = QObject::tr("Size");
        else if (fpartProp == XBinary::FPART_PROP_STREAMOFFSET) sName = QObject::tr("Stream offset");
        else if (fpartProp == XBinary::FPART_PROP_STREAMSIZE) sName = QObject::tr("Stream size");
        else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD) sName = QObject::tr("Method");

        if ((fpartProp == XBinary::FPART_PROP_ORIGINALNAME) || (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD)) nFlags = Qt::AlignVCenter | Qt::AlignLeft;
        else nFlags = Qt::AlignVCenter | Qt::AlignRight;

        if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) nSymbolSize = 20;
        else nSymbolSize = 16;

        setColumnName(i, sName);
        setColumnAlignment(i, nFlags);
        setColumnSymbolSize(i, nSymbolSize);
    }
}

QVariant XModel_ArchiveRecords::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!m_pListArchiveRecords) {
        return result;
    }

    if (index.isValid()) {
        qint32 nRow = index.row();
        if (nRow >= 0 && nRow < m_pListArchiveRecords->count()) {
            qint32 nColumn = index.column();
            const XBinary::ARCHIVERECORD &rec = m_pListArchiveRecords->at(nRow);
            if (nRole == Qt::DisplayRole) {
                if (nColumn < m_listColumns.count()) {
                    XBinary::FPART_PROP fpartProp = m_listColumns.at(nColumn);

                    if (fpartProp == XBinary::FPART_PROP_ORIGINALNAME) {
                        result = rec.mapProperties.value(fpartProp).toString();
                    } else if ((fpartProp == XBinary::FPART_PROP_COMPRESSEDSIZE) || (fpartProp == XBinary::FPART_PROP_UNCOMPRESSEDSIZE)) {
                        result = rec.mapProperties.value(fpartProp).toLongLong();
                    } else if (fpartProp == XBinary::FPART_PROP_STREAMOFFSET) {
                        result = rec.nStreamOffset;
                    } else if (fpartProp == XBinary::FPART_PROP_STREAMSIZE) {
                        result = rec.nStreamSize;
                    } else if (fpartProp == XBinary::FPART_PROP_HANDLEMETHOD) {
                        result = XBinary::getHandleMethods(rec.mapProperties);
                    }
                }

                // if (nColumn == COLUMN_NAME) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                //         result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                //     }
                // } else if (nColumn == COLUMN_STREAMOFFSET) {
                //     result = QString::number(rec.nStreamOffset, 16);
                // } else if (nColumn == COLUMN_STREAMSIZE) {
                //     result = QString::number(rec.nStreamSize, 16);
                // // } else if (nColumn == COLUMN_DECOMPRESSEDOFFSET) {
                // //     result = QString::number(rec.nDecompressedOffset, 16);
                // // } else if (nColumn == COLUMN_DECOMPRESSEDSIZE) {
                // //     result = QString::number(rec.nDecompressedSize, 16);
                // } else if (nColumn == COLUMN_COMPRESSMETHOD) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_COMPRESSMETHOD)) {
                //         XBinary::COMPRESS_METHOD cm = (XBinary::COMPRESS_METHOD)rec.mapProperties.value(XBinary::FPART_PROP_COMPRESSMETHOD).toInt();
                //         result = XBinary::compressMethodToString(cm);
                //     }
                // } else if (nColumn == COLUMN_CRC) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_CRC_VALUE)) {
                //         quint32 nCRC = rec.mapProperties.value(XBinary::FPART_PROP_CRC_VALUE).toUInt();
                //         result = QString::number(nCRC, 16);
                //     }
                // } else if (nColumn == COLUMN_DATETIME) {
                //     if (rec.mapProperties.contains(XBinary::FPART_PROP_DATETIME)) {
                //         QDateTime dt = rec.mapProperties.value(XBinary::FPART_PROP_DATETIME).toDateTime();
                //         if (dt.isValid()) {
                //             result = dt.toString("yyyy-MM-dd HH:mm:ss");
                //         }
                //     }
                // }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            } else if (nRole >= Qt::UserRole) {
                if (nRole == (Qt::UserRole + XModel::USERROLE_ORIGINDEX)) {
                    result = nRow;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_OFFSET)) {
                    result = rec.nStreamOffset;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_SIZE)) {
                    result = rec.nStreamSize;
                } else if (nRole == (Qt::UserRole + XModel::USERROLE_STRING1)) {
                    if (rec.mapProperties.contains(XBinary::FPART_PROP_ORIGINALNAME)) {
                        result = rec.mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
                    }
                }
            }
        }
    }

    return result;
}

QVariant XModel_ArchiveRecords::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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
