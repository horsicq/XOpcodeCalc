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

#include "xmodel_msrecord.h"
#include <QFile>

XModel_MSRecord::XModel_MSRecord(QIODevice *pDevice, const XBinary::_MEMORY_MAP &memoryMap, QVector<XBinary::MS_RECORD> *pListRecods, XBinary::VT valueType,
                                 QObject *pParent)
    : XModel(pParent)
{
    m_pDevice = pDevice;
    m_memoryMap = memoryMap;
    m_pListRecords = pListRecods;
    m_pListSignatureRecords = nullptr;

    m_endian = XBinary::ENDIAN_LITTLE;
    m_valueType = valueType;
    m_bValueCacheValid = false;

    qint32 nRowCount = pListRecods->count();

    _setRowCount(nRowCount);
    _setColumnCount(__COLUMN_SIZE);

    m_vecSortIndex.resize(nRowCount);

    for (qint32 i = 0; i < nRowCount; i++) {
        m_vecSortIndex[i] = i;
    }

    m_modeAddress = XBinary::getWidthModeFromSize(memoryMap.nModuleAddress + memoryMap.nImageSize);
    m_modeOffset = XBinary::getWidthModeFromSize(memoryMap.nBinarySize);

    setColumnSymbolSize(COLUMN_NUMBER, QString::number(nRowCount).length());
    setColumnSymbolSize(COLUMN_OFFSET, XBinary::getByteSizeFromWidthMode(m_modeOffset) * 2);
    setColumnSymbolSize(COLUMN_ADDRESS, XBinary::getByteSizeFromWidthMode(m_modeAddress) * 2);
    setColumnSymbolSize(COLUMN_REGION, 1);
    setColumnSymbolSize(COLUMN_SIZE, 4);
    setColumnSymbolSize(COLUMN_INFO, 8);

    qint32 nNumberOfRegions = memoryMap.listRecords.count();
    qint32 nMaxRegionNameLength = 4;

    for (qint32 i = 0; i < nNumberOfRegions; i++) {
        nMaxRegionNameLength = qMax(nMaxRegionNameLength, memoryMap.listRecords.at(i).sName.length());
        nMaxRegionNameLength = qMin(50, nMaxRegionNameLength);
    }

    setColumnSymbolSize(COLUMN_REGION, nMaxRegionNameLength);
}

void XModel_MSRecord::setValue(XBinary::ENDIAN endian, XBinary::VT valueType, QVariant varValue)
{
    m_endian = endian;
    m_valueType = valueType;
    m_varValue = varValue;

    m_sValue = XBinary::getValueString(varValue, valueType);
}

void XModel_MSRecord::setSignaturesList(QList<XBinary::SIGNATUREDB_RECORD> *pListSignatureRecords)
{
    m_pListSignatureRecords = pListSignatureRecords;
}

QVariant XModel_MSRecord::data(const QModelIndex &index, int nRole) const
{
    QVariant result;

    if (index.isValid()) {
        qint32 nRow = index.row();

        if ((nRow >= 0) && (nRow < m_vecSortIndex.size())) {
            qint32 nDataRow = m_vecSortIndex.at(nRow);
            qint32 nColumn = index.column();

            if (nRole == Qt::DisplayRole) {
                if (nColumn == COLUMN_NUMBER) {
                    result = nDataRow;
                } else if (nColumn == COLUMN_OFFSET) {
                    qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;
                    if (nRegionIndex != -1) {
                        if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                            result = XBinary::valueToHex(m_modeOffset, m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(nDataRow).nRelOffset);
                        }
                    } else {
                        result = XBinary::valueToHex(m_modeOffset, m_pListRecords->at(nDataRow).nRelOffset);
                    }
                } else if (nColumn == COLUMN_ADDRESS) {
                    qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;
                    if (nRegionIndex != -1) {
                        if (m_memoryMap.listRecords.at(nRegionIndex).nAddress != (XADDR)-1) {
                            result = XBinary::valueToHex(m_modeAddress, m_memoryMap.listRecords.at(nRegionIndex).nAddress + m_pListRecords->at(nDataRow).nRelOffset);
                        }
                    }
                } else if (nColumn == COLUMN_REGION) {
                    if (m_pListRecords->at(nDataRow).nRegionIndex >= 0) {
                        result = m_memoryMap.listRecords.at(m_pListRecords->at(nDataRow).nRegionIndex).sName;
                    }
                } else if (nColumn == COLUMN_SIZE) {
                    result = QString::number(m_pListRecords->at(nDataRow).nSize, 16);
                } else if (nColumn == COLUMN_INFO) {
                    if (m_valueType != XBinary::VT_SIGNATURE) {
                        result = XBinary::valueTypeToString((XBinary::VT)(m_pListRecords->at(nDataRow).nValueType), 0);
                    } else {
                        if (nColumn == COLUMN_INFO) {
                            if (m_valueType == XBinary::VT_SIGNATURE) {
                                if (m_pListSignatureRecords && (m_pListSignatureRecords->count() > m_pListRecords->at(nDataRow).nInfo)) {
                                    if (m_pListSignatureRecords->at(m_pListRecords->at(nDataRow).nInfo).sPatch != "") {
                                        result = tr("Patch");
                                    }
                                }
                            }
                        }
                    }
                } else if (nColumn == COLUMN_VALUE) {
                    if (m_bValueCacheValid && (nDataRow < m_vecValueCache.count())) {
                        result = m_vecValueCache.at(nDataRow);
                    } else if ((m_valueType == XBinary::VT_STRING) || (m_valueType == XBinary::VT_A_I) || (m_valueType == XBinary::VT_U_I) ||
                               (m_valueType == XBinary::VT_UTF8_I)) {
                        XBinary binary(m_pDevice);
                        XBinary::VT valueType = m_valueType;
                        if (m_valueType == XBinary::VT_STRING) {
                            valueType = (XBinary::VT)(m_pListRecords->at(nDataRow).nValueType);
                        }
                        qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;

                        if (nRegionIndex != -1) {
                            if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                                qint64 _nOffset = m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(nDataRow).nRelOffset;
                                result = binary.read_value(valueType, _nOffset, m_pListRecords->at(nDataRow).nSize, m_endian == XBinary::ENDIAN_BIG).toString();
                            }
                        } else {
                            qint64 _nOffset = m_pListRecords->at(nDataRow).nRelOffset;
                            result = binary.read_value(valueType, _nOffset, m_pListRecords->at(nDataRow).nSize, m_endian == XBinary::ENDIAN_BIG).toString();
                        }
                    } else if (m_valueType == XBinary::VT_SIGNATURE) {
                        if (m_pListSignatureRecords && (m_pListSignatureRecords->count() > m_pListRecords->at(nDataRow).nInfo)) {
                            result = m_pListSignatureRecords->at(m_pListRecords->at(nDataRow).nInfo).sName;
                        } else {
                            result = m_sValue;
                        }
                    } else {
                        result = m_sValue;
                    }
                }
            } else if (nRole == Qt::TextAlignmentRole) {
                if ((nColumn == COLUMN_NUMBER) || (nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_ADDRESS) || (nColumn == COLUMN_SIZE)) {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignRight;
                } else {
                    result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignLeft;
                }
            } else if (nRole == Qt::UserRole + USERROLE_ORIGINDEX) {
                result = nDataRow;
            } else if (nRole == Qt::UserRole + USERROLE_ADDRESS) {
                qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;
                if (nRegionIndex != -1) {
                    if (m_memoryMap.listRecords.at(nRegionIndex).nAddress != (XADDR)-1) {
                        result = m_memoryMap.listRecords.at(nRegionIndex).nAddress + m_pListRecords->at(nDataRow).nRelOffset;
                    }
                }
            } else if (nRole == Qt::UserRole + USERROLE_OFFSET) {
                qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;
                if (nRegionIndex != -1) {
                    if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                        result = m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(nDataRow).nRelOffset;
                    }
                } else {
                    result = m_pListRecords->at(nDataRow).nRelOffset;
                }
            } else if (nRole == Qt::UserRole + USERROLE_SIZE) {
                result = m_pListRecords->at(nDataRow).nSize;
            } else if (nRole == Qt::UserRole + USERROLE_STRING1) {
                result = m_pListRecords->at(nDataRow).nValueType;
            }
        }
    }

    return result;
}

QVariant XModel_MSRecord::headerData(int nSection, Qt::Orientation orientation, int nRole) const
{
    QVariant result;

    if (orientation == Qt::Horizontal) {
        if (nRole == Qt::DisplayRole) {
            if (nSection == COLUMN_NUMBER) {
                result = "#";
            } else if (nSection == COLUMN_OFFSET) {
                result = tr("Offset");
            } else if (nSection == COLUMN_ADDRESS) {
                result = tr("Address");
            } else if (nSection == COLUMN_REGION) {
                result = tr("Region");
            } else if (nSection == COLUMN_SIZE) {
                result = tr("Size");
            } else if (nSection == COLUMN_INFO) {
                // if (g_valueType == XBinary::VT_SIGNATURE) {
                //     result = tr("Patch");
                // } else {
                //     result = tr("Info");
                // }
                result = tr("Info");
            } else if (nSection == COLUMN_VALUE) {
                if (m_valueType == XBinary::VT_STRING) {
                    result = tr("String");
                } else if (m_valueType == XBinary::VT_SIGNATURE) {
                    result = tr("Signature");
                } else {
                    result = tr("Value");
                }
            }
        } else if (nRole == Qt::TextAlignmentRole) {
            if ((nSection == COLUMN_NUMBER) || (nSection == COLUMN_OFFSET) || (nSection == COLUMN_ADDRESS) || (nSection == COLUMN_SIZE)) {
                result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignRight;
            } else {
                result = (qint32)Qt::AlignVCenter + (qint32)Qt::AlignLeft;
            }
        }
    }

    return result;
}

bool XModel_MSRecord::isCustomFilter()
{
    return true;
}

bool XModel_MSRecord::isCustomSort()
{
    return true;
}

XModel::SORT_METHOD XModel_MSRecord::getSortMethod(qint32 nColumn)
{
    SORT_METHOD result = SORT_METHOD_DEFAULT;

    if ((nColumn == COLUMN_OFFSET) || (nColumn == COLUMN_ADDRESS) || (nColumn == COLUMN_SIZE)) {
        result = SORT_METHOD_HEX;
    }

    return result;
}

bool XModel_MSRecord::hasSortKeyHex() const
{
    return true;
}

quint64 XModel_MSRecord::getSortKeyHex(qint32 nRow, qint32 nColumn) const
{
    if ((nRow >= 0) && (nRow < m_vecSortIndex.size())) {
        return _getRawSortKey(m_vecSortIndex.at(nRow), nColumn);
    }

    return 0;
}

void XModel_MSRecord::sortByColumn(qint32 nColumn, Qt::SortOrder order)
{
    qint32 nRowCount = m_pListRecords->count();

    m_vecSortIndex.resize(nRowCount);

    if (nColumn == COLUMN_NUMBER) {
        for (qint32 i = 0; i < nRowCount; i++) {
            m_vecSortIndex[i] = i;
        }

        if (order == Qt::DescendingOrder) {
            std::reverse(m_vecSortIndex.begin(), m_vecSortIndex.end());
        }

        return;
    }

    SORT_METHOD sortMethod = getSortMethod(nColumn);

    if (sortMethod == SORT_METHOD_HEX) {
        QVector<QPair<quint64, qint32>> vecPairs(nRowCount);

        for (qint32 i = 0; i < nRowCount; i++) {
            vecPairs[i].first = _getRawSortKey(i, nColumn);
            vecPairs[i].second = i;
        }

        if (order == Qt::AscendingOrder) {
            std::sort(vecPairs.begin(), vecPairs.end(), [](const QPair<quint64, qint32> &a, const QPair<quint64, qint32> &b) { return a.first < b.first; });
        } else {
            std::sort(vecPairs.begin(), vecPairs.end(), [](const QPair<quint64, qint32> &a, const QPair<quint64, qint32> &b) { return a.first > b.first; });
        }

        for (qint32 i = 0; i < nRowCount; i++) {
            m_vecSortIndex[i] = vecPairs[i].second;
        }
    } else {
        QVector<QPair<QString, qint32>> vecPairs(nRowCount);

        for (qint32 i = 0; i < nRowCount; i++) {
            if ((nColumn == COLUMN_VALUE) && m_bValueCacheValid) {
                vecPairs[i].first = m_vecValueCache.at(i);
            } else if (nColumn == COLUMN_REGION) {
                if (m_pListRecords->at(i).nRegionIndex >= 0) {
                    vecPairs[i].first = m_memoryMap.listRecords.at(m_pListRecords->at(i).nRegionIndex).sName;
                }
            } else if (nColumn == COLUMN_INFO) {
                vecPairs[i].first = XBinary::valueTypeToString((XBinary::VT)(m_pListRecords->at(i).nValueType), 0);
            }
            vecPairs[i].second = i;
        }

        if (order == Qt::AscendingOrder) {
            std::sort(vecPairs.begin(), vecPairs.end(), [](const QPair<QString, qint32> &a, const QPair<QString, qint32> &b) { return a.first < b.first; });
        } else {
            std::sort(vecPairs.begin(), vecPairs.end(), [](const QPair<QString, qint32> &a, const QPair<QString, qint32> &b) { return a.first > b.first; });
        }

        for (qint32 i = 0; i < nRowCount; i++) {
            m_vecSortIndex[i] = vecPairs[i].second;
        }
    }
}

quint64 XModel_MSRecord::_getRawSortKey(qint32 nDataRow, qint32 nColumn) const
{
    quint64 nResult = 0;

    if ((nDataRow >= 0) && (nDataRow < m_pListRecords->count())) {
        if (nColumn == COLUMN_OFFSET) {
            qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;

            if (nRegionIndex != -1) {
                if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                    nResult = m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(nDataRow).nRelOffset;
                }
            } else {
                nResult = m_pListRecords->at(nDataRow).nRelOffset;
            }
        } else if (nColumn == COLUMN_ADDRESS) {
            qint16 nRegionIndex = m_pListRecords->at(nDataRow).nRegionIndex;

            if (nRegionIndex != -1) {
                if (m_memoryMap.listRecords.at(nRegionIndex).nAddress != (XADDR)-1) {
                    nResult = m_memoryMap.listRecords.at(nRegionIndex).nAddress + m_pListRecords->at(nDataRow).nRelOffset;
                }
            }
        } else if (nColumn == COLUMN_SIZE) {
            nResult = m_pListRecords->at(nDataRow).nSize;
        }
    }

    return nResult;
}

void XModel_MSRecord::buildValueCache()
{
    qint32 nRowCount = m_pListRecords->count();
    m_vecValueCache.resize(nRowCount);

    // Try memory mapping for fast access (avoids per-byte I/O)
    QFile *pFile = qobject_cast<QFile *>(m_pDevice);
    qint64 nFileSize = m_pDevice->size();
    uchar *pMapped = nullptr;

    if (pFile && (nFileSize > 0)) {
        pMapped = pFile->map(0, nFileSize);
    }

    if (pMapped) {
        bool bBigEndian = (m_endian == XBinary::ENDIAN_BIG);

        for (qint32 i = 0; i < nRowCount; i++) {
            QString sValue;

            if ((m_valueType == XBinary::VT_STRING) || (m_valueType == XBinary::VT_A_I) || (m_valueType == XBinary::VT_U_I) || (m_valueType == XBinary::VT_UTF8_I)) {
                XBinary::VT valueType = m_valueType;

                if (m_valueType == XBinary::VT_STRING) {
                    valueType = (XBinary::VT)(m_pListRecords->at(i).nValueType);
                }

                qint64 nOffset = -1;
                qint16 nRegionIndex = m_pListRecords->at(i).nRegionIndex;

                if (nRegionIndex != -1) {
                    if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                        nOffset = m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(i).nRelOffset;
                    }
                } else {
                    nOffset = m_pListRecords->at(i).nRelOffset;
                }

                qint64 nSize = qMin((qint64)(m_pListRecords->at(i).nSize), (qint64)128);

                if ((nOffset >= 0) && (nOffset + nSize <= nFileSize)) {
                    const char *pData = (const char *)(pMapped + nOffset);

                    if ((valueType == XBinary::VT_A) || (valueType == XBinary::VT_A_I)) {
                        qint32 nLen = 0;

                        for (qint32 j = 0; j < nSize; j++) {
                            if (pData[j] == 0) {
                                break;
                            }

                            nLen++;
                        }

                        sValue = QString::fromLatin1(pData, nLen);
                    } else if ((valueType == XBinary::VT_U) || (valueType == XBinary::VT_U_I)) {
                        qint32 nCharCount = nSize / 2;
                        qint32 nLen = 0;
                        const quint16 *pUData = (const quint16 *)pData;

                        for (qint32 j = 0; j < nCharCount; j++) {
                            quint16 nCh = pUData[j];

                            if (bBigEndian) {
                                nCh = ((nCh >> 8) & 0xFF) | ((nCh << 8) & 0xFF00);
                            }

                            if (nCh == 0) {
                                break;
                            }

                            nLen++;
                        }

                        if (bBigEndian) {
                            QVector<quint16> vecSwapped(nLen);

                            for (qint32 j = 0; j < nLen; j++) {
                                quint16 nCh = pUData[j];
                                vecSwapped[j] = ((nCh >> 8) & 0xFF) | ((nCh << 8) & 0xFF00);
                            }

                            sValue = QString::fromUtf16(vecSwapped.constData(), nLen);
                        } else {
                            sValue = QString::fromUtf16(pUData, nLen);
                        }
                    } else if ((valueType == XBinary::VT_UTF8) || (valueType == XBinary::VT_UTF8_I)) {
                        qint32 nLen = 0;

                        for (qint32 j = 0; j < nSize; j++) {
                            if (pData[j] == 0) {
                                break;
                            }

                            nLen++;
                        }

                        sValue = QString::fromUtf8(pData, nLen);
                    }
                }
            } else if (m_valueType == XBinary::VT_SIGNATURE) {
                if (m_pListSignatureRecords && (m_pListSignatureRecords->count() > m_pListRecords->at(i).nInfo)) {
                    sValue = m_pListSignatureRecords->at(m_pListRecords->at(i).nInfo).sName;
                } else {
                    sValue = m_sValue;
                }
            } else {
                sValue = m_sValue;
            }

            m_vecValueCache[i] = sValue;
        }

        pFile->unmap(pMapped);
    } else {
        // Fallback to original per-byte I/O for non-file QIODevice
        XBinary binary(m_pDevice);

        for (qint32 i = 0; i < nRowCount; i++) {
            QString sValue;

            if ((m_valueType == XBinary::VT_STRING) || (m_valueType == XBinary::VT_A_I) || (m_valueType == XBinary::VT_U_I) || (m_valueType == XBinary::VT_UTF8_I)) {
                XBinary::VT valueType = m_valueType;

                if (m_valueType == XBinary::VT_STRING) {
                    valueType = (XBinary::VT)(m_pListRecords->at(i).nValueType);
                }

                qint16 nRegionIndex = m_pListRecords->at(i).nRegionIndex;

                if (nRegionIndex != -1) {
                    if (m_memoryMap.listRecords.at(nRegionIndex).nOffset != -1) {
                        qint64 nOffset = m_memoryMap.listRecords.at(nRegionIndex).nOffset + m_pListRecords->at(i).nRelOffset;
                        sValue = binary.read_value(valueType, nOffset, m_pListRecords->at(i).nSize, m_endian == XBinary::ENDIAN_BIG).toString();
                    }
                } else {
                    qint64 nOffset = m_pListRecords->at(i).nRelOffset;
                    sValue = binary.read_value(valueType, nOffset, m_pListRecords->at(i).nSize, m_endian == XBinary::ENDIAN_BIG).toString();
                }
            } else if (m_valueType == XBinary::VT_SIGNATURE) {
                if (m_pListSignatureRecords && (m_pListSignatureRecords->count() > m_pListRecords->at(i).nInfo)) {
                    sValue = m_pListSignatureRecords->at(m_pListRecords->at(i).nInfo).sName;
                } else {
                    sValue = m_sValue;
                }
            } else {
                sValue = m_sValue;
            }

            m_vecValueCache[i] = sValue;
        }
    }

    m_bValueCacheValid = true;
}

void XModel_MSRecord::clearValueCache()
{
    m_vecValueCache.clear();
    m_bValueCacheValid = false;
}

bool XModel_MSRecord::isValueCacheValid() const
{
    return m_bValueCacheValid;
}
