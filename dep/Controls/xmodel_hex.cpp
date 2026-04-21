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
#include "xmodel_hex.h"

#include <QtGlobal>

XModel_Hex::XModel_Hex(QIODevice *pDevice, qint64 nOffset, qint64 nSize, quint64 nStartAddress, qint32 nBytesPerLine, QObject *pParent)
    : XModel(pParent), m_pDevice(pDevice), m_nOffset(nOffset), m_nSize(nSize), m_nStartAddress(nStartAddress), m_nBytesPerLine(nBytesPerLine)
{
    if (!m_pDevice) {
        _setRowCount(0);
        _setColumnCount(0);
        return;
    }

    if (m_nSize < 0) {
        m_nSize = m_pDevice->size() - m_nOffset;
        if (m_nSize < 0) m_nSize = 0;
    }

    if (m_nBytesPerLine <= 0) m_nBytesPerLine = 16;

    qint32 rows = (qint32)((m_nSize + m_nBytesPerLine - 1) / m_nBytesPerLine);
    _setRowCount(rows);
    _setColumnCount(__COLUMN_COUNT);

    setColumnName(COLUMN_ADDRESS, QObject::tr("Address"));
    setColumnName(COLUMN_HEX, QObject::tr("Hex"));
    setColumnName(COLUMN_ASCII, QString("ASCII"));

    setColumnAlignment(COLUMN_ADDRESS, Qt::AlignVCenter | Qt::AlignRight);
    setColumnAlignment(COLUMN_HEX, Qt::AlignVCenter | Qt::AlignLeft);
    setColumnAlignment(COLUMN_ASCII, Qt::AlignVCenter | Qt::AlignLeft);
}

QVariant XModel_Hex::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (!index.isValid()) return result;

    int row = index.row();
    int column = index.column();

    if (nRole == Qt::DisplayRole) {
        qint64 lineOffset = m_nOffset + (qint64)row * m_nBytesPerLine;
        int bytesThisLine = (int)qMin<qint64>(m_nBytesPerLine, m_nSize - (row * (qint64)m_nBytesPerLine));
        if (bytesThisLine < 0) bytesThisLine = 0;

        if (!m_pDevice->isSequential()) {
            qint64 prev = m_pDevice->pos();
            m_pDevice->seek(lineOffset);
            QByteArray chunk = m_pDevice->read(bytesThisLine);
            m_pDevice->seek(prev);

            if (column == COLUMN_ADDRESS) {
                result = QString::number((qulonglong)(m_nStartAddress + (quint64)(row * m_nBytesPerLine)), 16).rightJustified(8, '0');
            } else if (column == COLUMN_HEX) {
                result = _bytesToHex(chunk);
            } else if (column == COLUMN_ASCII) {
                result = _bytesToAscii(chunk);
            }
        }
    } else if (nRole == Qt::TextAlignmentRole) {
        result = getColumnAlignment(column);
    }

    return result;
}

QVariant XModel_Hex::headerData(int nSection, Qt::Orientation orientation, int nRole) const
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

QString XModel_Hex::_bytesToHex(const QByteArray &ba) const
{
    QString s;
    s.reserve(ba.size() * 3);
    for (int i = 0; i < ba.size(); ++i) {
        unsigned char c = (unsigned char)ba.at(i);
        s.append(QString::number(c, 16).rightJustified(2, '0'));
        if (i + 1 < ba.size()) s.append(' ');
    }
    return s;
}

QString XModel_Hex::_bytesToAscii(const QByteArray &ba) const
{
    QString s;
    s.reserve(ba.size());
    for (int i = 0; i < ba.size(); ++i) {
        unsigned char c = (unsigned char)ba.at(i);
        if (c >= 0x20 && c <= 0x7E) s.append(QChar(c));
        else s.append('.');
    }
    return s;
}
