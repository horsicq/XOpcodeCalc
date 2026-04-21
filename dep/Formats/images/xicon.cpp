/* Copyright (c) 2022-2026 hors<horsicq@gmail.com>
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
#include "xicon.h"

static XBinary::XCONVERT _TABLE_XICON_STRUCTID[] = {
    {XIcon::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XIcon::STRUCTID_ICONDIR, "ICONDIR", QString("ICONDIR")},
    {XIcon::STRUCTID_ICONDIRENTRY, "ICONDIRENTRY", QString("ICONDIRENTRY")},
};

XIcon::XIcon(QIODevice *pDevice) : XBinary(pDevice)
{
}

XIcon::~XIcon()
{
}

bool XIcon::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    bool bResult = false;
    // TODO more checks !!!
    if (getSize() > (qint64)(sizeof(ICONDIR) + sizeof(ICONDIRENTRY))) {
        ICONDIR iconDir = readICONDIR();

        if ((iconDir.idReserved == 0) && ((iconDir.idType == 1) || (iconDir.idType == 2)) && (iconDir.idCount > 0) && (iconDir.idCount < 100)) {
            ICONDIRENTRY iconDirectory = readICONDIRENTRY(sizeof(ICONDIR));

            if ((iconDirectory.bReserved == 0) && (iconDirectory.dwBytesInRes > 0)) {
                if (iconDir.idType == 1) {
                    if ((iconDirectory.wPlanes == 0) || (iconDirectory.wPlanes == 1)) {
                        bResult = true;
                    }
                } else if (iconDir.idType == 2) {
                    bResult = true;
                }
            }
        }
    }

    return bResult;
}

bool XIcon::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XIcon xicon(pDevice);

    return xicon.isValid();
}

XBinary::FT XIcon::getFileType()
{
    FT result = FT_ICO;

    quint32 nHeader = read_uint32(0);

    if (nHeader == 0x00010000) {
        result = FT_ICO;
    } else if (nHeader == 0x00020000) {
        result = FT_CUR;
    }

    return result;
}

QString XIcon::getFileFormatExt()
{
    QString sResult;

    if (getFileType() == FT_CUR) {
        sResult = QString("cur");
    } else {
        sResult = QString("ico");
    }

    return sResult;
}

QString XIcon::getFileFormatExtsString()
{
    return QString("Icons, cursors (*.ico *.cur)");
}

qint64 XIcon::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XIcon::getMIMEString()
{
    return QString("image/x-icon");
}

XBinary::_MEMORY_MAP XIcon::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    _MEMORY_MAP result = {};

    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.filePart = FILEPART_HEADER;
        record.nOffset = 0;
        record.nSize = sizeof(ICONDIR);
        record.nAddress = -1;
        record.sName = tr("Header");

        result.listRecords.append(record);
    }

    ICONDIR iconDir = readICONDIR();

    qint64 nDataOffset = sizeof(ICONDIR) + (iconDir.idCount * sizeof(ICONDIRENTRY));

    bool bError = false;

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; (i < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        ICONDIRENTRY iconDirectory = readICONDIRENTRY(nOffset);

        if ((iconDirectory.dwBytesInRes == 0) || (iconDirectory.dwImageOffset < nDataOffset) || (iconDirectory.bReserved != 0)) {
            bError = true;
            break;
        }

        if (iconDir.idType == 1) {
            if ((iconDirectory.wPlanes != 0) && (iconDirectory.wPlanes != 1)) {
                bError = true;
                break;
            }
        }

        quint32 nHeader = read_uint32(iconDirectory.dwImageOffset);

        if ((nHeader != 0x00000028) && nHeader != (0x474e5089)) {  // PNG
            bError = true;
            break;
        }

        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;
        record.filePart = FILEPART_DATA;
        record.nOffset = iconDirectory.dwImageOffset;
        record.nSize = iconDirectory.dwBytesInRes;
        record.nAddress = -1;

        result.listRecords.append(record);

        nOffset += sizeof(ICONDIRENTRY);
    }

    if (bError) {
        result.listRecords.clear();
    }

    _handleOverlay(&result);

    return result;
}

XIcon::ICONDIR XIcon::readICONDIR()
{
    ICONDIR result = {};

    result.idReserved = read_uint16(offsetof(ICONDIR, idReserved));
    result.idType = read_uint16(offsetof(ICONDIR, idType));
    result.idCount = read_uint16(offsetof(ICONDIR, idCount));

    return result;
}

XIcon::ICONDIRENTRY XIcon::readICONDIRENTRY(qint64 nOffset)
{
    ICONDIRENTRY result = {};

    result.bWidth = read_uint8(nOffset + offsetof(ICONDIRENTRY, bWidth));
    result.bHeight = read_uint8(nOffset + offsetof(ICONDIRENTRY, bHeight));
    result.bColorCount = read_uint8(nOffset + offsetof(ICONDIRENTRY, bColorCount));
    result.bReserved = read_uint8(nOffset + offsetof(ICONDIRENTRY, bReserved));
    result.wPlanes = read_uint16(nOffset + offsetof(ICONDIRENTRY, wPlanes));
    result.wBitCount = read_uint16(nOffset + offsetof(ICONDIRENTRY, wBitCount));
    result.dwBytesInRes = read_uint32(nOffset + offsetof(ICONDIRENTRY, dwBytesInRes));
    result.dwImageOffset = read_uint32(nOffset + offsetof(ICONDIRENTRY, dwImageOffset));

    return result;
}

XIcon::GRPICONDIRENTRY XIcon::readGPRICONDIRENTRY(qint64 nOffset)
{
    GRPICONDIRENTRY result = {};

    result.bWidth = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bWidth));
    result.bHeight = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bHeight));
    result.bColorCount = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bColorCount));
    result.bReserved = read_uint8(nOffset + offsetof(GRPICONDIRENTRY, bReserved));
    result.wPlanes = read_uint16(nOffset + offsetof(GRPICONDIRENTRY, wPlanes));
    result.wBitCount = read_uint16(nOffset + offsetof(GRPICONDIRENTRY, wBitCount));
    result.dwBytesInRes = read_uint32(nOffset + offsetof(GRPICONDIRENTRY, dwBytesInRes));
    result.nID = read_uint16(nOffset + offsetof(GRPICONDIRENTRY, nID));

    return result;
}

QList<XIcon::ICONDIRENTRY> XIcon::getIconDirectories()
{
    QList<XIcon::ICONDIRENTRY> listResult;

    ICONDIR iconDir = readICONDIR();

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        ICONDIRENTRY record = readICONDIRENTRY(nOffset);

        if ((record.dwBytesInRes == 0) || (record.dwImageOffset == 0)) {
            break;
        }

        listResult.append(record);

        nOffset += sizeof(ICONDIRENTRY);
    }

    return listResult;
}

QList<XIcon::GRPICONDIRENTRY> XIcon::getIconGPRDirectories()
{
    QList<XIcon::GRPICONDIRENTRY> listResult;

    ICONDIR iconDir = readICONDIR();

    qint32 nNumberOfRecords = iconDir.idCount;

    qint64 nOffset = sizeof(ICONDIR);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        GRPICONDIRENTRY record = readGPRICONDIRENTRY(nOffset);

        if ((record.dwBytesInRes == 0) || (record.nID == 0)) {
            break;
        }

        listResult.append(record);

        nOffset += sizeof(GRPICONDIRENTRY);
    }

    return listResult;
}

QString XIcon::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XICON_STRUCTID, sizeof(_TABLE_XICON_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XIcon::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = getFileType();

        _dataHeadersOptions.nID = STRUCTID_ICONDIR;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;
        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);
        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_ICONDIR) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XIcon::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = sizeof(ICONDIR);
                dataHeader.listRecords.append(getDataRecord(0, 2, "Reserved", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(2, 2, "Type", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(4, 2, "Count", VT_UINT16, DRF_COUNT, ENDIAN_LITTLE));
                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    ICONDIR dir = readICONDIR();
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.dhMode = XBinary::DHMODE_TABLE;
                    _dataHeadersOptions.nID = STRUCTID_ICONDIRENTRY;
                    _dataHeadersOptions.nLocation = dataHeadersOptions.nLocation + sizeof(ICONDIR);
                    _dataHeadersOptions.locType = dataHeadersOptions.locType;
                    _dataHeadersOptions.nCount = dir.idCount;
                    _dataHeadersOptions.nSize = sizeof(ICONDIRENTRY) * dir.idCount;
                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_ICONDIRENTRY) {
                // Describe the table of entries; row reading can be added later if needed
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XIcon::structIDToString(dataHeadersOptions.nID));
                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XIcon::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    QList<FPART> listResult;

    qint64 nTotal = getSize();
    qint64 nMax = sizeof(ICONDIR);

    if (nFileParts & FILEPART_HEADER) {
        FPART rec = {};
        rec.filePart = FILEPART_HEADER;
        rec.nFileOffset = 0;
        rec.nFileSize = sizeof(ICONDIR);
        rec.nVirtualAddress = -1;
        rec.sName = tr("Header");
        listResult.append(rec);
    }

    ICONDIR dir = readICONDIR();

    if (nFileParts & FILEPART_TABLE) {
        FPART rec = {};
        rec.filePart = FILEPART_TABLE;
        rec.nFileOffset = sizeof(ICONDIR);
        rec.nFileSize = sizeof(ICONDIRENTRY) * dir.idCount;
        rec.nVirtualAddress = -1;
        rec.sName = tr("Entries");
        listResult.append(rec);
    }

    nMax = qMax(nMax, (qint64)(sizeof(ICONDIR) + sizeof(ICONDIRENTRY) * dir.idCount));

    QList<ICONDIRENTRY> entries = getIconDirectories();
    for (qint32 nI = 0; nI < entries.size(); ++nI) {
        ICONDIRENTRY e = entries.at(nI);
        if ((e.dwImageOffset < nTotal) && (e.dwBytesInRes > 0)) {
            if (nFileParts & FILEPART_OBJECT) {
                FPART rec = {};
                rec.filePart = FILEPART_OBJECT;
                rec.nFileOffset = e.dwImageOffset;
                rec.nFileSize = qMin<qint64>(e.dwBytesInRes, nTotal - e.dwImageOffset);
                rec.nVirtualAddress = -1;
                rec.sName = tr("Icon");
                listResult.append(rec);
            }

            nMax = qMax(nMax, (qint64)(e.dwImageOffset + e.dwBytesInRes));
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        if (nMax < nTotal) {
            FPART rec = {};
            rec.filePart = FILEPART_OVERLAY;
            rec.nFileOffset = nMax;
            rec.nFileSize = nTotal - nMax;
            rec.nVirtualAddress = -1;
            rec.sName = tr("Overlay");
            listResult.append(rec);
        }
    }

    return listResult;
}

QList<QString> XIcon::getSearchSignatures()
{
    QList<QString> listResult;

    return listResult;
}

XBinary *XIcon::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XIcon(pDevice);
}

