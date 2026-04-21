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

#include "xmodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>

namespace {
QString getHeaderName(const QAbstractItemModel *pModel, qint32 nColumn)
{
    QString sResult = pModel->headerData(nColumn, Qt::Horizontal, Qt::DisplayRole).toString().trimmed();

    if (sResult.isEmpty()) {
        sResult = QString("field_%1").arg(nColumn);
    }

    return sResult;
}

QString headerToXmlTagName(const QString &sHeader, qint32 nColumn)
{
    QString sSource = sHeader;

    if (sSource.isEmpty()) {
        sSource = QString("field_%1").arg(nColumn);
    }

    sSource.replace("#", "Number");

    QString sResult;
    sResult.reserve(sSource.size());

    for (QChar c : sSource) {
        if (c.isLetterOrNumber() || (c == '_') || (c == '-') || (c == '.')) {
            sResult.append(c);
        } else {
            sResult.append('_');
        }
    }

    if (sResult.isEmpty() || (!(sResult.at(0).isLetter()) && (sResult.at(0) != '_'))) {
        sResult.prepend("field_");
    }

    return sResult;
}
}  // namespace

XModel::XModel(QObject *pParent) : QAbstractItemModel(pParent)
{
    m_nRowCount = 0;
    m_nColumnCount = 0;
}

XModel::~XModel()
{
}

void XModel::setColumnSymbolSize(qint32 nColumn, qint32 nValue)
{
    m_hashColumnSymbolSize[nColumn] = nValue;  // TODO optimize use allocated memory
}

qint32 XModel::getColumnSymbolSize(qint32 nColumn) const
{
    return m_hashColumnSymbolSize.value(nColumn, 40);
}

void XModel::setColumnAlignment(qint32 nColumn, qint32 flag)
{
    m_hashColumnAlignment[nColumn] = flag;  // TODO optimize use allocated memory
}

qint32 XModel::getColumnAlignment(qint32 nColumn) const
{
    return m_hashColumnAlignment.value(nColumn, Qt::AlignVCenter | Qt::AlignLeft);
}

void XModel::setColumnName(qint32 nColumn, const QString &sName)
{
    m_hashColumnName[nColumn] = sName;  // TODO optimize use allocated memory
}

QString XModel::getColumnName(qint32 nColumn) const
{
    return m_hashColumnName.value(nColumn, "");
}

bool XModel::isCustomFilter()
{
    return false;
}

bool XModel::isCustomSort()
{
    return false;
}

bool XModel::hasSortKeyHex() const
{
    return false;
}

quint64 XModel::getSortKeyHex(qint32 nRow, qint32 nColumn) const
{
    Q_UNUSED(nRow)
    Q_UNUSED(nColumn)

    return 0;
}

void XModel::sortByColumn(qint32 nColumn, Qt::SortOrder order)
{
    Q_UNUSED(nColumn)
    Q_UNUSED(order)
}

void XModel::setRowHidden(qint32 nRow, bool bState)
{
    if ((nRow >= 0) && (nRow < m_vecRowHidden.size())) {
        m_vecRowHidden[nRow] = bState;
    }
}

void XModel::clearRowHidden()
{
    m_vecRowHidden.fill(false);
}

qint32 XModel::getVisibleRowCount() const
{
    qint32 nResult = m_nRowCount;

    qint32 nSize = m_vecRowHidden.size();

    for (qint32 i = 0; i < nSize; i++) {
        if (m_vecRowHidden.at(i)) {
            nResult--;
        }
    }

    return nResult;
}

void XModel::setRowPrio(qint32 nRow, quint64 nPrio)
{
    m_hashRowPrio[nRow] = nPrio;
}

bool XModel::isRowHidden(qint32 nRow)
{
    if ((nRow >= 0) && (nRow < m_vecRowHidden.size())) {
        return m_vecRowHidden.at(nRow);
    }

    return false;
}

quint64 XModel::getRowPrio(qint32 nRow)
{
    return m_hashRowPrio.value(nRow, 0);
}

QModelIndex XModel::index(int row, int column, const QModelIndex &parent) const
{
    QModelIndex result;

    if (hasIndex(row, column, parent)) {
        result = createIndex(row, column);
    }

    return result;
}

QModelIndex XModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)

    return QModelIndex();
}

void XModel::_setRowCount(qint32 nRowCount)
{
    m_nRowCount = nRowCount;
    m_vecRowHidden.resize(nRowCount);
    m_vecRowHidden.fill(false);
}

void XModel::_setColumnCount(qint32 nColumnCount)
{
    m_nColumnCount = nColumnCount;
}

int XModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_nRowCount;
}

int XModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_nColumnCount;
}

void XModel::adjustColumnToContent(qint32 nColumn, bool bHeader)
{
    qint32 nSymbolSize = 0;

    if (bHeader) {
        nSymbolSize = qMax(nSymbolSize, headerData(nColumn, Qt::Horizontal).toString().length());
    }

    qint32 nNumberOfRows = rowCount();

    for (qint32 i = 0; i < nNumberOfRows; i++) {
        QModelIndex index = this->index(i, nColumn);
        QString sData = data(index, Qt::DisplayRole).toString();

        nSymbolSize = qMax(nSymbolSize, sData.length());
    }

    setColumnSymbolSize(nColumn, nSymbolSize);
}

void XModel::adjustColumnsToContent(bool bHeader)
{
    qint32 nNumberOfColumns = columnCount();

    for (qint32 i = 0; i < nNumberOfColumns; i++) {
        adjustColumnToContent(i, bHeader);
    }
}

QString XModel::toXML() const
{
    QString sResult;
    QXmlStreamWriter xmlWriter(&sResult);

    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("data");

    const qint32 nNumberOfRows = rowCount();
    const qint32 nNumberOfColumns = columnCount();

    for (qint32 nRow = 0; nRow < nNumberOfRows; nRow++) {
        if ((nRow < m_vecRowHidden.size()) && m_vecRowHidden.at(nRow)) {
            continue;
        }

        xmlWriter.writeStartElement("row");

        for (qint32 nColumn = 0; nColumn < nNumberOfColumns; nColumn++) {
            const QString sHeader = getHeaderName(this, nColumn);
            const QString sValue = data(index(nRow, nColumn), Qt::DisplayRole).toString();

            xmlWriter.writeTextElement(headerToXmlTagName(sHeader, nColumn), sValue);
        }

        xmlWriter.writeEndElement();
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();

    return sResult;
}

QString XModel::toJSON() const
{
    QJsonArray jsonRows;

    const qint32 nNumberOfRows = rowCount();
    const qint32 nNumberOfColumns = columnCount();

    for (qint32 nRow = 0; nRow < nNumberOfRows; nRow++) {
        if ((nRow < m_vecRowHidden.size()) && m_vecRowHidden.at(nRow)) {
            continue;
        }

        QJsonObject jsonRow;

        for (qint32 nColumn = 0; nColumn < nNumberOfColumns; nColumn++) {
            const QString sHeader = getHeaderName(this, nColumn);
            const QString sValue = data(index(nRow, nColumn), Qt::DisplayRole).toString();

            jsonRow.insert(sHeader, sValue);
        }

        jsonRows.append(jsonRow);
    }

    QJsonDocument jsonDocument(jsonRows);

    return jsonDocument.toJson(QJsonDocument::Indented);
}

XModel::SORT_METHOD XModel::getSortMethod(qint32 nColumn)
{
    Q_UNUSED(nColumn)

    return SORT_METHOD_DEFAULT;
}
