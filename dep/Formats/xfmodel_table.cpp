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

#include "xfmodel_table.h"

XFModel_table::XFModel_table(QObject *pParent) : XFModel(pParent)
{
    m_bShowOffset = true;
    m_bShowPresentation = false;
}

XFModel_table::~XFModel_table()
{
}

void XFModel_table::setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader)
{
    XFModel::setData(pXBinary, xfHeader);

    beginResetModel();

    m_listColumnFields.clear();
    m_listRowFields.clear();
    m_listTableRowValues.clear();

    if (m_pXBinary) {
        qint32 nRowCount = m_xfHeader.listRowLocations.count();

        for (qint32 i = 0; i < nRowCount; i++) {
            XBinary::XLOC rowLoc = {};
            rowLoc.locType = XBinary::LT_OFFSET;
            rowLoc.nLocation = m_xfHeader.listRowLocations.at(i);

            QList<XBinary::XFRECORD> listFields = m_pXBinary->getXFRecords(m_xfHeader.fileType, m_xfHeader.structID, rowLoc);
            QList<QVariant> listRowValues = m_pXBinary->getXFRecordValues(listFields, rowLoc);
            m_listRowFields.append(listFields);
            m_listTableRowValues.append(listRowValues);

            if (i == 0) {
                m_listColumnFields = listFields;
            }
        }
    }

    _rebuildColumnMap();

    endResetModel();
}

int XFModel_table::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_listTableRowValues.count();
}

int XFModel_table::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_listColumnMap.count();
}

void XFModel_table::setShowOffset(bool bShowOffset)
{
    beginResetModel();
    m_bShowOffset = bShowOffset;
    _rebuildColumnMap();
    endResetModel();
}

void XFModel_table::setShowPresentation(bool bShowPresentation)
{
    beginResetModel();
    m_bShowPresentation = bShowPresentation;
    _rebuildColumnMap();
    endResetModel();
}

void XFModel_table::_rebuildColumnMap()
{
    m_listPresentationColumns.clear();
    m_listColumnMap.clear();

    // Build presentation column list per field
    qint32 nFieldCount = m_listColumnFields.count();
    qint32 nDataStCount = m_xfHeader.listDataSt.count();

    for (qint32 i = 0; i < nFieldCount; i++) {
        // XFDATAST-based presentations
        for (qint32 j = 0; j < nDataStCount; j++) {
            XBinary::XFDATAST xfDataSt = m_xfHeader.listDataSt.at(j);

            if (xfDataSt.nFieldIndex == i) {
                PRESENTATION_COLUMN presCol = {};
                presCol.nFieldIndex = i;
                presCol.nDataStIndex = j;

                if (xfDataSt.xfDataStType == XBinary::XFDATASTYPE_LIST) {
                    presCol.presentationType = PT_DATAST_LIST;
                } else if (xfDataSt.xfDataStType == XBinary::XFDATASTYPE_FLAGS) {
                    presCol.presentationType = PT_DATAST_FLAGS;
                } else {
                    continue;
                }

                m_listPresentationColumns.append(presCol);
            }
        }

        // XFRECORD_FLAG-based presentations
        XBinary::XFRECORD xfRecord = m_listColumnFields.at(i);

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_DOSTIME) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_DOSTIME;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_DOSDATE) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_DOSDATE;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_UNIXTIME) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_UNIXTIME;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_FILETIME) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_FILETIME;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        const quint32 nVersionMask = XBinary::XFRECORD_FLAG_VERSION_MAJOR | XBinary::XFRECORD_FLAG_VERSION_MINOR | XBinary::XFRECORD_FLAG_VERSION_PATCH |
                                     XBinary::XFRECORD_FLAG_VERSION_BUILD | XBinary::XFRECORD_FLAG_VERSION | XBinary::XFRECORD_FLAG_VERSION_DIVMOD;

        if (xfRecord.nFlags & nVersionMask) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_VERSION;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_COUNT) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_COUNT;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_SIZE) {
            PRESENTATION_COLUMN presCol = {};
            presCol.nFieldIndex = i;
            presCol.presentationType = PT_SIZE;
            presCol.nDataStIndex = -1;
            m_listPresentationColumns.append(presCol);
        }
    }

    // Build ordered column map
    COLUMN_ENTRY entry = {};

    // # column
    entry.columnEntryType = CET_NUMBER;
    entry.nIndex = 0;
    m_listColumnMap.append(entry);

    // Name column (optional, only if listRowNames is not empty)
    if (!m_xfHeader.listRowNames.isEmpty()) {
        entry.columnEntryType = CET_NAME;
        entry.nIndex = 0;
        m_listColumnMap.append(entry);
    }

    // Offset column (optional)
    if (m_bShowOffset) {
        entry.columnEntryType = CET_OFFSET;
        entry.nIndex = 0;
        m_listColumnMap.append(entry);
    }

    // Field columns, with presentation columns interleaved after their source field
    for (qint32 i = 0; i < nFieldCount; i++) {
        entry.columnEntryType = CET_FIELD;
        entry.nIndex = i;
        m_listColumnMap.append(entry);

        if (m_bShowPresentation) {
            // Insert presentation columns for this field right after it
            for (qint32 j = 0; j < m_listPresentationColumns.count(); j++) {
                if (m_listPresentationColumns.at(j).nFieldIndex == i) {
                    entry.columnEntryType = CET_PRESENTATION;
                    entry.nIndex = j;
                    m_listColumnMap.append(entry);
                }
            }
        }
    }
}

QVariant XFModel_table::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if ((section >= 0) && (section < m_listColumnMap.count())) {
            COLUMN_ENTRY entry = m_listColumnMap.at(section);

            if (entry.columnEntryType == CET_NUMBER) {
                result = tr("#");
            } else if (entry.columnEntryType == CET_NAME) {
                result = tr("Name");
            } else if (entry.columnEntryType == CET_OFFSET) {
                result = tr("Offset");
            } else if (entry.columnEntryType == CET_FIELD) {
                if ((entry.nIndex >= 0) && (entry.nIndex < m_listColumnFields.count())) {
                    result = m_listColumnFields.at(entry.nIndex).sName;
                }
            } else if (entry.columnEntryType == CET_PRESENTATION) {
                if ((entry.nIndex >= 0) && (entry.nIndex < m_listPresentationColumns.count())) {
                    qint32 nFieldIndex = m_listPresentationColumns.at(entry.nIndex).nFieldIndex;
                    if ((nFieldIndex >= 0) && (nFieldIndex < m_listColumnFields.count())) {
                        result = QString("[%1]").arg(m_listColumnFields.at(nFieldIndex).sName);
                    }
                }
            }
        }
    }

    return result;
}

QVariant XFModel_table::data(const QModelIndex &index, int role) const
{
    QVariant result;

    if (!index.isValid()) {
        return result;
    }

    qint32 nRow = index.row();
    qint32 nColumn = index.column();

    if ((nColumn < 0) || (nColumn >= m_listColumnMap.count())) {
        return result;
    }

    COLUMN_ENTRY entry = m_listColumnMap.at(nColumn);

    if (role == Qt::DisplayRole) {
        if ((nRow >= 0) && (nRow < m_listTableRowValues.count())) {
            if (entry.columnEntryType == CET_NUMBER) {
                result = QString::number(nRow);
            } else if (entry.columnEntryType == CET_NAME) {
                if (nRow < m_xfHeader.listRowNames.count()) {
                    result = m_xfHeader.listRowNames.at(nRow);
                }
            } else if (entry.columnEntryType == CET_OFFSET) {
                if (nRow < m_xfHeader.listRowLocations.count()) {
                    result = XBinary::valueToHexEx(m_xfHeader.listRowLocations.at(nRow));
                }
            } else if (entry.columnEntryType == CET_FIELD) {
                qint32 nFieldIndex = entry.nIndex;
                QList<QVariant> listRowValues = m_listTableRowValues.at(nRow);
                QList<XBinary::XFRECORD> listRowFieldDefs = m_listRowFields.at(nRow);

                if ((nFieldIndex >= 0) && (nFieldIndex < listRowValues.count()) && (nFieldIndex < listRowFieldDefs.count())) {
                    result = valueToString(listRowFieldDefs.at(nFieldIndex), listRowValues.at(nFieldIndex));
                }
            } else if (entry.columnEntryType == CET_PRESENTATION) {
                qint32 nPresIndex = entry.nIndex;
                if ((nPresIndex >= 0) && (nPresIndex < m_listPresentationColumns.count())) {
                    PRESENTATION_COLUMN presCol = m_listPresentationColumns.at(nPresIndex);
                    qint32 nFieldIndex = presCol.nFieldIndex;

                    QList<QVariant> listRowValues = m_listTableRowValues.at(nRow);

                    if ((nFieldIndex >= 0) && (nFieldIndex < listRowValues.count())) {
                        quint64 nValue = listRowValues.at(nFieldIndex).toULongLong();

                        XBinary::XFRECORD xfRecord = {};
                        if ((presCol.presentationType == PT_VERSION) && (nFieldIndex < m_listRowFields.at(nRow).count())) {
                            xfRecord = m_listRowFields.at(nRow).at(nFieldIndex);
                        }

                        XBinary::XFDATAST xfDataSt = {};
                        qint32 nDataStIndex = presCol.nDataStIndex;
                        if ((nDataStIndex >= 0) && (nDataStIndex < m_xfHeader.listDataSt.count())) {
                            xfDataSt = m_xfHeader.listDataSt.at(nDataStIndex);
                        }

                        result = presentationToString(presCol.presentationType, nValue, xfRecord, xfDataSt);
                    }
                }
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (entry.columnEntryType == CET_OFFSET) {
            result = (int)(Qt::AlignRight | Qt::AlignVCenter);
        } else if (entry.columnEntryType == CET_FIELD) {
            qint32 nFieldIndex = entry.nIndex;
            if ((nFieldIndex >= 0) && (nFieldIndex < m_listColumnFields.count())) {
                XBinary::XFRECORD xfRecord = m_listColumnFields.at(nFieldIndex);
                if ((xfRecord.valueType == XBinary::VT_UINT8) || (xfRecord.valueType == XBinary::VT_UINT16) || (xfRecord.valueType == XBinary::VT_UINT32) ||
                    (xfRecord.valueType == XBinary::VT_UINT64) || (xfRecord.valueType == XBinary::VT_INT8) || (xfRecord.valueType == XBinary::VT_INT16) ||
                    (xfRecord.valueType == XBinary::VT_INT32) || (xfRecord.valueType == XBinary::VT_INT64)) {
                    result = (int)(Qt::AlignRight | Qt::AlignVCenter);
                }
            }
        }
    }

    return result;
}
