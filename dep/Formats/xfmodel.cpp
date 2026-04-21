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

#include "xfmodel.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>

XFModel::XFModel(QObject *pParent) : XModel(pParent)
{
    m_pXBinary = nullptr;
}

XFModel::~XFModel()
{
}

void XFModel::setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader)
{
    beginResetModel();

    m_pXBinary = pXBinary;
    m_xfHeader = xfHeader;

    endResetModel();
}

int XFModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return 0;
}

int XFModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return __COLUMN_SIZE;
}

QVariant XFModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index)
    Q_UNUSED(role)

    return QVariant();
}

QVariant XFModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;

    if ((orientation == Qt::Horizontal) && (role == Qt::DisplayRole)) {
        if (section == COLUMN_NAME) {
            result = tr("Name");
        } else if (section == COLUMN_OFFSET) {
            result = tr("Offset");
        } else if (section == COLUMN_SIZE) {
            result = tr("Size");
        } else if (section == COLUMN_TYPE) {
            result = tr("Type");
        } else if (section == COLUMN_VALUE) {
            result = tr("Value");
        } else if (section == COLUMN_COMMENT) {
            result = tr("Comment");
        }
    }

    return result;
}

QString XFModel::valueToString(const XBinary::XFRECORD &xfRecord, const QVariant &varValue) const
{
    QString sResult;

    if ((xfRecord.valueType == XBinary::VT_UINT8) || (xfRecord.valueType == XBinary::VT_UINT16) || (xfRecord.valueType == XBinary::VT_UINT32) ||
        (xfRecord.valueType == XBinary::VT_UINT64)) {
        quint64 nValue = varValue.toULongLong();

        if (xfRecord.valueType == XBinary::VT_UINT8) {
            sResult = XBinary::valueToHex((quint8)nValue);
        } else if (xfRecord.valueType == XBinary::VT_UINT16) {
            sResult = XBinary::valueToHex((quint16)nValue);
        } else if (xfRecord.valueType == XBinary::VT_UINT32) {
            sResult = XBinary::valueToHex((quint32)nValue);
        } else if (xfRecord.valueType == XBinary::VT_UINT64) {
            sResult = XBinary::valueToHex((quint64)nValue);
        }
    } else if ((xfRecord.valueType == XBinary::VT_INT8) || (xfRecord.valueType == XBinary::VT_INT16) || (xfRecord.valueType == XBinary::VT_INT32) ||
               (xfRecord.valueType == XBinary::VT_INT64)) {
        sResult = QString::number(varValue.toLongLong());
    } else if (xfRecord.valueType == XBinary::VT_FLOAT) {
        sResult = QString::number(varValue.toFloat());
    } else if (xfRecord.valueType == XBinary::VT_DOUBLE) {
        sResult = QString::number(varValue.toDouble());
    } else if (xfRecord.valueType == XBinary::VT_CHAR_ARRAY) {
        sResult = varValue.toString();
    } else if (xfRecord.valueType == XBinary::VT_BYTE_ARRAY) {
        QByteArray baData = varValue.toByteArray();
        sResult = baData.toHex(' ').toUpper();
    } else {
        sResult = varValue.toString();
    }

    return sResult;
}

QString XFModel::flagsToString(quint64 nFlags) const
{
    QStringList listFlags;

    if (nFlags & XBinary::XFRECORD_FLAG_SIZE) listFlags.append("SIZE");
    if (nFlags & XBinary::XFRECORD_FLAG_OFFSET) listFlags.append("OFFSET");
    if (nFlags & XBinary::XFRECORD_FLAG_RELATIVE_OFFSET) listFlags.append("REL_OFFSET");
    if (nFlags & XBinary::XFRECORD_FLAG_ADDRESS) listFlags.append("ADDRESS");
    if (nFlags & XBinary::XFRECORD_FLAG_RELATIVE_ADDRESS) listFlags.append("REL_ADDRESS");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION_MAJOR) listFlags.append("VER_MAJOR");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION_MINOR) listFlags.append("VER_MINOR");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION_PATCH) listFlags.append("VER_PATCH");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION_BUILD) listFlags.append("VER_BUILD");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION) listFlags.append("VERSION");
    if (nFlags & XBinary::XFRECORD_FLAG_VERSION_DIVMOD) listFlags.append("VER_DIVMOD");
    if (nFlags & XBinary::XFRECORD_FLAG_DATETIME) listFlags.append("DATETIME");
    if (nFlags & XBinary::XFRECORD_FLAG_DATE) listFlags.append("DATE");
    if (nFlags & XBinary::XFRECORD_FLAG_TIME) listFlags.append("TIME");
    if (nFlags & XBinary::XFRECORD_FLAG_DOSDATE) listFlags.append("DOSDATE");
    if (nFlags & XBinary::XFRECORD_FLAG_DOSTIME) listFlags.append("DOSTIME");
    if (nFlags & XBinary::XFRECORD_FLAG_UNIXTIME) listFlags.append("UNIXTIME");
    if (nFlags & XBinary::XFRECORD_FLAG_FILETIME) listFlags.append("FILETIME");
    if (nFlags & XBinary::XFRECORD_FLAG_COUNT) listFlags.append("COUNT");

    return listFlags.join("|");
}

QString XFModel::presentationToString(PRESENTATION_TYPE presentationType, quint64 nValue, const XBinary::XFRECORD &xfRecord, const XBinary::XFDATAST &xfDataSt)
{
    QString sResult;

    if (presentationType == PT_DATAST_LIST) {
        sResult = XBinary::XIDSTRING_idToString(nValue, xfDataSt.pRecords, xfDataSt.nRecordsSize);
    } else if (presentationType == PT_DATAST_FLAGS) {
        quint64 nMaskedValue = nValue & xfDataSt.nMask;
        QStringList listFlagStrings;

        for (qint32 j = 0; j < xfDataSt.nRecordsSize; j++) {
            if (nMaskedValue & xfDataSt.pRecords[j].nID) {
                listFlagStrings.append(xfDataSt.pRecords[j].sString);
            }
        }

        sResult = listFlagStrings.join("|");
    } else if (presentationType == PT_DOSTIME) {
        sResult = XBinary::valueToTimeString(nValue, XBinary::DT_TYPE_DOSTIME);
    } else if (presentationType == PT_DOSDATE) {
        sResult = XBinary::valueToTimeString(nValue, XBinary::DT_TYPE_DOSDATE);
    } else if (presentationType == PT_UNIXTIME) {
        sResult = XBinary::valueToTimeString(nValue, XBinary::DT_TYPE_UNIXTIME);
    } else if (presentationType == PT_FILETIME) {
        sResult = XBinary::valueToTimeString(nValue, XBinary::DT_TYPE_FILETIME);
    } else if (presentationType == PT_VERSION) {
        quint32 nFlags = xfRecord.nFlags;
        const quint32 nFullVersionMask =
            XBinary::XFRECORD_FLAG_VERSION_MAJOR | XBinary::XFRECORD_FLAG_VERSION_MINOR | XBinary::XFRECORD_FLAG_VERSION_PATCH | XBinary::XFRECORD_FLAG_VERSION_BUILD;

        if (nFlags & XBinary::XFRECORD_FLAG_VERSION_DIVMOD) {
            if (xfRecord.valueType == XBinary::VT_UINT8) {
                sResult = XBinary::versionDivModByteToString((quint8)nValue);
            } else if (xfRecord.valueType == XBinary::VT_UINT16) {
                sResult = XBinary::versionDivModWordToString((quint16)nValue);
            }
        } else if (nFlags & nFullVersionMask) {
            quint32 nVersionFlags = nFlags & nFullVersionMask;
            // Single version component (only one of MAJOR/MINOR/PATCH/BUILD set):
            // show plain decimal, e.g. "10" instead of byte-split "0.10"
            bool bSingleComponent = (nVersionFlags == XBinary::XFRECORD_FLAG_VERSION_MAJOR) || (nVersionFlags == XBinary::XFRECORD_FLAG_VERSION_MINOR) ||
                                    (nVersionFlags == XBinary::XFRECORD_FLAG_VERSION_PATCH) || (nVersionFlags == XBinary::XFRECORD_FLAG_VERSION_BUILD);

            if (bSingleComponent) {
                sResult = QString("\"%1\"").arg(QString::number(nValue));
            } else if (xfRecord.valueType == XBinary::VT_UINT8) {
                sResult = XBinary::fullVersionByteToString((quint8)nValue);
            } else if (xfRecord.valueType == XBinary::VT_UINT16) {
                sResult = XBinary::fullVersionWordToString((quint16)nValue);
            } else if (xfRecord.valueType == XBinary::VT_UINT32) {
                sResult = XBinary::fullVersionDwordToString((quint32)nValue);
            } else if (xfRecord.valueType == XBinary::VT_UINT64) {
                sResult = XBinary::fullVersionQwordToString(nValue);
            }
        } else if (nFlags & XBinary::XFRECORD_FLAG_VERSION) {
            if (xfRecord.valueType == XBinary::VT_UINT16) {
                sResult = XBinary::versionWordToString((quint16)nValue);
            } else if (xfRecord.valueType == XBinary::VT_UINT32) {
                sResult = XBinary::versionDwordToString((quint32)nValue);
            }
        }
    } else if (presentationType == PT_COUNT) {
        sResult = XBinary::numberToString(nValue);
    } else if (presentationType == PT_SIZE) {
        sResult = XBinary::bytesCountToString(nValue);
    }

    return sResult;
}

QString XFModel::commentForField(qint32 nFieldIndex, const QVariant &varValue) const
{
    QStringList listComments;

    quint64 nValue = varValue.toULongLong();
    XBinary::XFDATAST xfDataStEmpty = {};

    // Check XFDATAST for this field
    qint32 nDataStCount = m_xfHeader.listDataSt.count();

    for (qint32 i = 0; i < nDataStCount; i++) {
        XBinary::XFDATAST xfDataSt = m_xfHeader.listDataSt.at(i);

        if (xfDataSt.nFieldIndex == nFieldIndex) {
            PRESENTATION_TYPE presType;

            if (xfDataSt.xfDataStType == XBinary::XFDATASTYPE_LIST) {
                presType = PT_DATAST_LIST;
            } else if (xfDataSt.xfDataStType == XBinary::XFDATASTYPE_FLAGS) {
                presType = PT_DATAST_FLAGS;
            } else {
                continue;
            }

            XBinary::XFRECORD xfRecordEmpty = {};
            QString sPresentation = presentationToString(presType, nValue, xfRecordEmpty, xfDataSt);

            if (!sPresentation.isEmpty()) {
                listComments.append(sPresentation);
            }
        }
    }

    // Check XFRECORD flags for presentations
    if ((nFieldIndex >= 0) && (nFieldIndex < m_xfHeader.listFields.count())) {
        XBinary::XFRECORD xfRecord = m_xfHeader.listFields.at(nFieldIndex);

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_DOSTIME) {
            listComments.append(presentationToString(PT_DOSTIME, nValue, xfRecord, xfDataStEmpty));
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_DOSDATE) {
            listComments.append(presentationToString(PT_DOSDATE, nValue, xfRecord, xfDataStEmpty));
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_UNIXTIME) {
            listComments.append(presentationToString(PT_UNIXTIME, nValue, xfRecord, xfDataStEmpty));
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_FILETIME) {
            listComments.append(presentationToString(PT_FILETIME, nValue, xfRecord, xfDataStEmpty));
        }

        const quint32 nVersionMask = XBinary::XFRECORD_FLAG_VERSION_MAJOR | XBinary::XFRECORD_FLAG_VERSION_MINOR | XBinary::XFRECORD_FLAG_VERSION_PATCH |
                                     XBinary::XFRECORD_FLAG_VERSION_BUILD | XBinary::XFRECORD_FLAG_VERSION | XBinary::XFRECORD_FLAG_VERSION_DIVMOD;

        if (xfRecord.nFlags & nVersionMask) {
            listComments.append(presentationToString(PT_VERSION, nValue, xfRecord, xfDataStEmpty));
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_COUNT) {
            listComments.append(presentationToString(PT_COUNT, nValue, xfRecord, xfDataStEmpty));
        }

        if (xfRecord.nFlags & XBinary::XFRECORD_FLAG_SIZE) {
            listComments.append(presentationToString(PT_SIZE, nValue, xfRecord, xfDataStEmpty));
        }
    }

    return listComments.join("; ");
}

void XFModel::printToConsole(const QString &sTitle) const
{
    QString sOutput = modelToString(sTitle);

    QStringList listLines = sOutput.split("\n");

    for (qint32 i = 0; i < listLines.count(); i++) {
        qDebug().noquote() << listLines.at(i);
    }
}

QString XFModel::modelToString(const QString &sTitle) const
{
    QString sResult;

    qint32 nRowCount = rowCount();
    qint32 nColumnCount = columnCount();

    if ((nRowCount == 0) || (nColumnCount == 0)) {
        if (!sTitle.isEmpty()) {
            sResult += sTitle + "\n";
            sResult += "(empty)\n";
        }
        return sResult;
    }

    QList<qint32> listWidths = calculateColumnWidths(this);

    // Title
    if (!sTitle.isEmpty()) {
        sResult += sTitle + "\n";
    }

    // Header row
    QStringList listHeaderValues;

    for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
        QVariant varHeader = headerData(nCol, Qt::Horizontal, Qt::DisplayRole);
        listHeaderValues.append(varHeader.toString());
    }

    QList<bool> listAlignRight = calculateColumnAlignRight(this);

    sResult += separatorLine(listWidths) + "\n";
    sResult += formatRow(listHeaderValues, listWidths) + "\n";
    sResult += separatorLine(listWidths) + "\n";

    // Data rows
    for (qint32 nRow = 0; nRow < nRowCount; nRow++) {
        QStringList listRowValues;

        for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
            QModelIndex idx = index(nRow, nCol);
            QVariant varData = data(idx, Qt::DisplayRole);
            listRowValues.append(varData.toString());
        }

        sResult += formatRow(listRowValues, listWidths, listAlignRight) + "\n";
    }

    sResult += separatorLine(listWidths);

    return sResult;
}

QList<qint32> XFModel::calculateColumnWidths(const QAbstractItemModel *pModel)
{
    QList<qint32> listWidths;

    qint32 nRowCount = pModel->rowCount();
    qint32 nColumnCount = pModel->columnCount();

    // Initialize with header widths
    for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
        QVariant varHeader = pModel->headerData(nCol, Qt::Horizontal, Qt::DisplayRole);
        listWidths.append(varHeader.toString().length());
    }

    // Expand with data widths
    for (qint32 nRow = 0; nRow < nRowCount; nRow++) {
        for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
            QModelIndex idx = pModel->index(nRow, nCol);
            QVariant varData = pModel->data(idx, Qt::DisplayRole);
            qint32 nLen = varData.toString().length();

            if (nLen > listWidths[nCol]) {
                listWidths[nCol] = nLen;
            }
        }
    }

    // Minimum width of 2, maximum of 60
    for (qint32 i = 0; i < listWidths.count(); i++) {
        if (listWidths[i] < 2) {
            listWidths[i] = 2;
        }
        if (listWidths[i] > 60) {
            listWidths[i] = 60;
        }
    }

    return listWidths;
}

QList<bool> XFModel::calculateColumnAlignRight(const QAbstractItemModel *pModel)
{
    QList<bool> listAlignRight;

    qint32 nColumnCount = pModel->columnCount();
    qint32 nRowCount = pModel->rowCount();

    for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
        bool bAlignRight = false;

        if (nRowCount > 0) {
            QModelIndex idx = pModel->index(0, nCol);
            QVariant varAlign = pModel->data(idx, Qt::TextAlignmentRole);

            if (varAlign.isValid()) {
                qint32 nAlign = varAlign.toInt();
                bAlignRight = (nAlign & Qt::AlignRight);
            }
        }

        listAlignRight.append(bAlignRight);
    }

    return listAlignRight;
}

QString XFModel::formatRow(const QStringList &listValues, const QList<qint32> &listWidths, const QList<bool> &listAlignRight)
{
    QString sResult = "|";

    for (qint32 i = 0; i < listValues.count(); i++) {
        qint32 nWidth = (i < listWidths.count()) ? listWidths.at(i) : 10;
        QString sValue = listValues.at(i).left(nWidth);
        bool bRight = (i < listAlignRight.count()) ? listAlignRight.at(i) : false;

        if (bRight) {
            sResult += " " + sValue.rightJustified(nWidth) + " |";
        } else {
            sResult += " " + sValue.leftJustified(nWidth) + " |";
        }
    }

    return sResult;
}

QString XFModel::separatorLine(const QList<qint32> &listWidths)
{
    QString sResult = "+";

    for (qint32 i = 0; i < listWidths.count(); i++) {
        sResult += QString("-").repeated(listWidths.at(i) + 2) + "+";
    }

    return sResult;
}

QString XFModel::exportToString(const QAbstractItemModel *pModel, EXPORT_FORMAT exportFormat)
{
    QString sResult;

    qint32 nRowCount = pModel->rowCount();
    qint32 nColumnCount = pModel->columnCount();

    if ((nRowCount == 0) || (nColumnCount == 0)) {
        return sResult;
    }

    // Collect headers
    QStringList listHeaders;

    for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
        QVariant varHeader = pModel->headerData(nCol, Qt::Horizontal, Qt::DisplayRole);
        listHeaders.append(varHeader.toString());
    }

    // Collect data
    QList<QStringList> listRows;

    for (qint32 nRow = 0; nRow < nRowCount; nRow++) {
        QStringList listRowValues;

        for (qint32 nCol = 0; nCol < nColumnCount; nCol++) {
            QModelIndex idx = pModel->index(nRow, nCol);
            QVariant varData = pModel->data(idx, Qt::DisplayRole);
            listRowValues.append(varData.toString());
        }

        listRows.append(listRowValues);
    }

    if (exportFormat == EXPORT_PLAINTEXT) {
        QList<qint32> listWidths = calculateColumnWidths(pModel);
        QList<bool> listAlignRight = calculateColumnAlignRight(pModel);

        sResult += separatorLine(listWidths) + "\n";
        sResult += formatRow(listHeaders, listWidths) + "\n";
        sResult += separatorLine(listWidths) + "\n";

        for (qint32 i = 0; i < listRows.count(); i++) {
            sResult += formatRow(listRows.at(i), listWidths, listAlignRight) + "\n";
        }

        sResult += separatorLine(listWidths);
    } else if ((exportFormat == EXPORT_CSV) || (exportFormat == EXPORT_TSV)) {
        QChar cSeparator = (exportFormat == EXPORT_CSV) ? QChar(',') : QChar('\t');

        // Header
        QStringList listEscapedHeaders;

        for (qint32 i = 0; i < listHeaders.count(); i++) {
            QString sField = listHeaders.at(i);

            if (exportFormat == EXPORT_CSV) {
                sField.replace("\"", "\"\"");
                sField = "\"" + sField + "\"";
            }

            listEscapedHeaders.append(sField);
        }

        sResult += listEscapedHeaders.join(cSeparator) + "\n";

        // Rows
        for (qint32 i = 0; i < listRows.count(); i++) {
            QStringList listEscapedValues;

            for (qint32 j = 0; j < listRows.at(i).count(); j++) {
                QString sField = listRows.at(i).at(j);

                if (exportFormat == EXPORT_CSV) {
                    sField.replace("\"", "\"\"");
                    sField = "\"" + sField + "\"";
                }

                listEscapedValues.append(sField);
            }

            sResult += listEscapedValues.join(cSeparator) + "\n";
        }
    } else if (exportFormat == EXPORT_JSON) {
        QJsonArray jsonRows;

        for (qint32 i = 0; i < listRows.count(); i++) {
            QJsonObject jsonRow;

            for (qint32 j = 0; j < listHeaders.count(); j++) {
                QString sValue = (j < listRows.at(i).count()) ? listRows.at(i).at(j) : QString();
                jsonRow.insert(listHeaders.at(j), sValue);
            }

            jsonRows.append(jsonRow);
        }

        QJsonDocument jsonDoc(jsonRows);
        sResult = jsonDoc.toJson(QJsonDocument::Indented);
    } else if (exportFormat == EXPORT_XML) {
        QXmlStreamWriter xmlWriter(&sResult);
        xmlWriter.setAutoFormatting(true);
        xmlWriter.writeStartDocument();
        xmlWriter.writeStartElement("data");

        for (qint32 i = 0; i < listRows.count(); i++) {
            xmlWriter.writeStartElement("row");

            for (qint32 j = 0; j < listHeaders.count(); j++) {
                QString sValue = (j < listRows.at(i).count()) ? listRows.at(i).at(j) : QString();
                QString sTag = listHeaders.at(j);
                sTag.replace(" ", "_");
                sTag.replace("#", "Number");

                if (sTag.isEmpty() || sTag.at(0).isDigit()) {
                    sTag = "field_" + sTag;
                }

                xmlWriter.writeTextElement(sTag, sValue);
            }

            xmlWriter.writeEndElement();
        }

        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
    }

    return sResult;
}

bool XFModel::exportToFile(const QAbstractItemModel *pModel, EXPORT_FORMAT exportFormat, const QString &sFileName)
{
    QString sContent = exportToString(pModel, exportFormat);

    if (sContent.isEmpty()) {
        return false;
    }

    QFile file(sFileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    stream << sContent;

    file.close();

    return true;
}

QString XFModel::exportFormatToFilter(EXPORT_FORMAT exportFormat)
{
    QString sResult;

    if (exportFormat == EXPORT_PLAINTEXT) {
        sResult = "Plain Text (*.txt)";
    } else if (exportFormat == EXPORT_CSV) {
        sResult = "CSV (*.csv)";
    } else if (exportFormat == EXPORT_TSV) {
        sResult = "TSV (*.tsv)";
    } else if (exportFormat == EXPORT_JSON) {
        sResult = "JSON (*.json)";
    } else if (exportFormat == EXPORT_XML) {
        sResult = "XML (*.xml)";
    }

    return sResult;
}

QString XFModel::exportAllFilters()
{
    QStringList listFilters;

    listFilters.append(exportFormatToFilter(EXPORT_PLAINTEXT));
    listFilters.append(exportFormatToFilter(EXPORT_CSV));
    listFilters.append(exportFormatToFilter(EXPORT_TSV));
    listFilters.append(exportFormatToFilter(EXPORT_JSON));
    listFilters.append(exportFormatToFilter(EXPORT_XML));

    return listFilters.join(";;");
}

XFModel::EXPORT_FORMAT XFModel::filterToExportFormat(const QString &sFilter)
{
    if (sFilter.contains("*.csv")) {
        return EXPORT_CSV;
    } else if (sFilter.contains("*.tsv")) {
        return EXPORT_TSV;
    } else if (sFilter.contains("*.json")) {
        return EXPORT_JSON;
    } else if (sFilter.contains("*.xml")) {
        return EXPORT_XML;
    }

    return EXPORT_PLAINTEXT;
}
