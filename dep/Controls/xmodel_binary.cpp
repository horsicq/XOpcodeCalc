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
#include "xmodel_binary.h"

XModel_Binary::XModel_Binary(const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<XBinary::DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles,
                             QObject *pParent)
    : XModel(pParent)
{
    m_dataRecordsOptions = dataRecordsOptions;
    m_pListDataRecords = pListDataRecords;
    m_pListTitles = pListTitles;
    m_pListComments = nullptr;

    if (dataRecordsOptions.dataHeaderFirst.dhMode == XBinary::DHMODE_HEADER) {
        _setRowCount(dataRecordsOptions.dataHeaderFirst.listRecords.count());
        _setColumnCount(__COLUMN_HEADER_SIZE);

        setColumnName(COLUMN_HEADER_NAME, tr("Name"));
        setColumnName(COLUMN_HEADER_OFFSET, tr("Offset"));
        setColumnName(COLUMN_HEADER_SIZE, tr("Size"));
        setColumnName(COLUMN_HEADER_TYPE, tr("Type"));
        setColumnName(COLUMN_HEADER_VALUE, tr("Value"));
        setColumnName(COLUMN_HEADER_INFO, tr(""));
        setColumnName(COLUMN_HEADER_COMMENT, tr("Comment"));
        setColumnAlignment(COLUMN_HEADER_NAME, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(COLUMN_HEADER_OFFSET, Qt::AlignVCenter | Qt::AlignRight);
        setColumnAlignment(COLUMN_HEADER_SIZE, Qt::AlignVCenter | Qt::AlignRight);
        setColumnAlignment(COLUMN_HEADER_TYPE, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(COLUMN_HEADER_VALUE, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(COLUMN_HEADER_INFO, Qt::AlignVCenter | Qt::AlignLeft);
        setColumnAlignment(COLUMN_HEADER_COMMENT, Qt::AlignVCenter | Qt::AlignLeft);
    } else if (dataRecordsOptions.dataHeaderFirst.dhMode == XBinary::DHMODE_TABLE) {
        qint32 nColumnCount = pListTitles->count();
        qint32 nRowCount = pListDataRecords->count();
        _setColumnCount(nColumnCount);
        _setRowCount(nRowCount);

        qint32 nNumberOfColumns = m_pListTitles->count();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 flag = Qt::AlignVCenter | Qt::AlignLeft;

            XBinary::VT valType = dataRecordsOptions.dataHeaderFirst.listRecords.at(i).valType;

            if (XBinary::isIntegerType(valType)) {
                flag = Qt::AlignVCenter | Qt::AlignRight;
            }

            setColumnAlignment(i, flag);
            setColumnName(i, m_pListTitles->at(i));

            if (nRowCount > 0) {
                qint32 nColumnSymbolSize = m_pListTitles->at(i).length();
                nColumnSymbolSize = qMax(nColumnSymbolSize, XBinary::getValueSymbolSize(valType));
                setColumnSymbolSize(i, nColumnSymbolSize);
            }
        }
    }
}

QVariant XModel_Binary::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (index.isValid()) {
        qint32 nRow = index.row();

        if (nRow >= 0) {
            qint32 nColumn = index.column();

            if (nRole == Qt::DisplayRole) {
                if (m_dataRecordsOptions.dataHeaderFirst.dhMode == XBinary::DHMODE_HEADER) {
                    if (nColumn == COLUMN_HEADER_NAME) {
                        result = m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).sName;
                    } else if (nColumn == COLUMN_HEADER_OFFSET) {
                        result = QString::number(m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).nRelOffset, 16);
                    } else if (nColumn == COLUMN_HEADER_SIZE) {
                        result = QString::number(m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).nSize, 16);
                    } else if (nColumn == COLUMN_HEADER_TYPE) {
                        result = XBinary::valueTypeToString(m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).valType,
                                                            m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).nSize);
                    } else if (nColumn == COLUMN_HEADER_VALUE) {
                        result = XBinary::getValueString(m_pListDataRecords->at(0).listValues.at(nRow), m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nRow).valType,
                                                         true);
                    } else if (nColumn == COLUMN_HEADER_INFO) {
                        // TODO
                    } else if (nColumn == COLUMN_HEADER_COMMENT) {
                        if (m_pListComments) {
                            if (nRow < m_pListComments->count()) {
                                result = m_pListComments->at(nRow);
                            }
                        }
                    }
                } else if (m_dataRecordsOptions.dataHeaderFirst.dhMode == XBinary::DHMODE_TABLE) {
                    result = XBinary::getValueString(m_pListDataRecords->at(nRow).listValues.at(nColumn),
                                                     m_dataRecordsOptions.dataHeaderFirst.listRecords.at(nColumn).valType, true);
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                result = getColumnAlignment(nColumn);
            }
        }
    }

    return result;
}

QVariant XModel_Binary::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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

void XModel_Binary::setComments(QList<QString> *pListComments)
{
    m_pListComments = pListComments;
}
