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
#include "xtiff.h"

static XBinary::XCONVERT _TABLE_XTIFF_STRUCTID[] = {
    {XTiff::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XTiff::STRUCTID_SIGNATURE, "Signature", QString("Signature")},
    {XTiff::STRUCTID_IFD_TABLE, "IFD_TABLE", QString("IFD Table")},
    {XTiff::STRUCTID_IFD_ENTRY, "IFD_ENTRY", QString("IFD Entry")},
};

XTiff::XTiff(QIODevice *pDevice) : XBinary(pDevice)
{
}

XTiff::~XTiff()
{
}

bool XTiff::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 8) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);
        if (compareSignature(&memoryMap, "'MM'002A", 0, pPdStruct) || compareSignature(&memoryMap, "'II'2A00", 0, pPdStruct)) {
            bool bIsBigEndian = isBigEndian();

            quint32 nOffset = read_uint32(4, bIsBigEndian);

            if ((nOffset > 0) && (nOffset < getSize())) {
                bIsValid = true;
            }
        }
    }

    return bIsValid;
}

bool XTiff::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XTiff xtiff(pDevice);

    return xtiff.isValid();
}

XBinary::_MEMORY_MAP XTiff::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    qint32 nIndex = 0;

    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nBinarySize = nTotalSize;
    result.endian = getEndian();

    bool bIsBigEndian = (result.endian == ENDIAN_BIG);

    if (result.nBinarySize > 8) {
        {
            _MEMORY_RECORD record = {};

            record.nIndex = nIndex++;
            record.filePart = FILEPART_HEADER;
            record.nOffset = 0;
            record.nSize = 8;
            record.nAddress = -1;
            record.sName = tr("Header");

            result.listRecords.append(record);
        }

        qint64 nTableOffset = read_uint32(4, bIsBigEndian);

        while (nTableOffset) {
            quint16 nTableCount = read_uint16(nTableOffset, bIsBigEndian);

            {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.filePart = FILEPART_TABLE;
                record.nOffset = nTableOffset;
                record.nSize = sizeof(quint16) + sizeof(IFD_ENTRY) * nTableCount;
                record.nAddress = -1;
                record.sName = tr("Table");

                result.listRecords.append(record);
            }

            qint64 nCurrentOffset = nTableOffset + sizeof(quint16);

            for (qint32 i = 0; i < nTableCount; i++) {
                quint16 nTag = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nTag), bIsBigEndian);
                quint16 nType = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nType), bIsBigEndian);
                quint32 nCount = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nCount), bIsBigEndian);

                qint32 nBaseTypeSize = getBaseTypeSize(nType);

                qint64 nDataSize = nBaseTypeSize * nCount;

                if (nDataSize > 4) {
                    quint32 nOffset = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nOffset), bIsBigEndian);

                    _MEMORY_RECORD record = {};

                    record.nIndex = nIndex++;
                    record.filePart = FILEPART_REGION;
                    record.nOffset = nOffset;
                    record.nSize = nDataSize;
                    record.nAddress = -1;
                    record.sName = QString("%1-%2").arg(XBinary::valueToHex(nTag), XBinary::valueToHex(nType));

                    result.listRecords.append(record);
                }

                nCurrentOffset += sizeof(IFD_ENTRY);
            }

            nTableOffset = read_uint32(nCurrentOffset, bIsBigEndian);

            {
                _MEMORY_RECORD record = {};

                record.nIndex = nIndex++;
                record.filePart = FILEPART_DATA;
                record.nOffset = nCurrentOffset;
                record.nSize = sizeof(quint32);
                record.nAddress = -1;

                result.listRecords.append(record);
            }
        }
    }

    _handleOverlay(&result);

    return result;
}

XBinary::FT XTiff::getFileType()
{
    return FT_TIFF;
}

QString XTiff::getFileFormatExt()
{
    return "tiff";
}

QString XTiff::getFileFormatExtsString()
{
    return "TIFF (*.tiff *.tif)";
}

qint64 XTiff::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

XBinary::ENDIAN XTiff::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint32 nData = read_uint32(0);

    if (nData == 0x4D4D002A) {
        result = ENDIAN_LITTLE;
    } else if (nData == 0x2A004D4D) {
        result = ENDIAN_BIG;
    }

    return result;
}

QList<XTiff::CHUNK> XTiff::getChunks(PDSTRUCT *pPdStruct)
{
    // Image
    // 0fe NewSubfileType LONG 1
    // 100 ImageWidth SHORT or LONG
    // 101 ImageLength SHORT or LONG
    // 102 BitsPerSample  SHORT 4 or 8
    // 103 Compression  SHORT 1, 2 or 32773
    // 106 PhotometricInterpretation  SHORT 0 or 1
    // 111 StripOffsets SHORT or LONG
    // 115 SamplesPerPixel SHORT
    // 116 RowsPerStrip  SHORT or LONG
    // 117 StripByteCounts LONG or SHORT
    // 11a XResolution RATIONAL
    // 11b YResolution RATIONAL
    // 11c PlanarConfiguration SHORT
    // 128 ResolutionUnit  SHORT 1, 2 or 3
    // 13d Predictor SHORT
    // 152 ExtraSamples SHORT 1

    // Exif
    // 10f Make ASCII
    // 110 Model ASCII
    // 112 Orientation SHORT 1
    // 131 Software ASCII
    // 132 DateTime ASCII
    // 213 YCbCrPositioning 1
    // 8769
    // 8825
    // 201 JPEGInterchangeFormat LONG 1
    // 202 JPEGInterchangeFormatLngth LONG 1
    // 213 YCbCrPositioning 1

    QList<XTiff::CHUNK> listResult;

    bool bIsBigEndian = isBigEndian();

    qint64 nTableOffset = read_uint32(4, bIsBigEndian);

    while ((nTableOffset) && isPdStructNotCanceled(pPdStruct)) {
        quint16 nTableCount = read_uint16(nTableOffset, bIsBigEndian);

        qint64 nCurrentOffset = nTableOffset + sizeof(quint16);

        for (qint32 i = 0; i < nTableCount; i++) {
            XTiff::CHUNK record = {};

            quint16 nTag = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nTag), bIsBigEndian);
            quint16 nType = read_uint16(nCurrentOffset + offsetof(IFD_ENTRY, nType), bIsBigEndian);
            quint32 nCount = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nCount), bIsBigEndian);

            qint32 nBaseTypeSize = getBaseTypeSize(nType);

            qint64 nDataSize = nBaseTypeSize * nCount;

            if (nDataSize > 4) {
                record.nOffset = read_uint32(nCurrentOffset + offsetof(IFD_ENTRY, nOffset), bIsBigEndian);
            } else {
                record.nOffset = nCurrentOffset + offsetof(IFD_ENTRY, nOffset);
            }

            record.nSize = nDataSize;
            record.nTag = nTag;

            nCurrentOffset += sizeof(IFD_ENTRY);

            listResult.append(record);
        }

        qint64 nTempTableOffset = read_uint32(nCurrentOffset, bIsBigEndian);
        if (nTempTableOffset < (qint64)(nTableOffset + sizeof(quint16) + nTableCount * sizeof(IFD_ENTRY) + sizeof(quint64))) {
            break;
        } else {
            nTableOffset = nTempTableOffset;
        }
    }

    return listResult;
}

QList<XTiff::CHUNK> XTiff::_getChunksByTag(QList<CHUNK> *pListChunks, quint16 nTag)
{
    QList<XTiff::CHUNK> listResult;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nTag == nTag) {
            listResult.append(pListChunks->at(i));
        }
    }

    return listResult;
}

QString XTiff::getExifCameraName(QIODevice *pDevice, OFFSETSIZE osExif, QList<CHUNK> *pListChunks)
{
    QString sResult;

    if (osExif.nSize) {
        SubDevice sd(pDevice, osExif.nOffset, osExif.nSize);

        if (sd.open(QIODevice::ReadOnly)) {
            XTiff tiff(&sd);

            if (tiff.isValid()) {
                QList<CHUNK> listMake = XTiff::_getChunksByTag(pListChunks, 0x10f);
                QList<CHUNK> listModel = XTiff::_getChunksByTag(pListChunks, 0x110);

                QString sMake;
                QString sModel;

                if (listMake.count()) {
                    sMake = tiff.read_ansiString(listMake.at(0).nOffset, listMake.at(0).nSize);
                }

                if (listModel.count()) {
                    sModel = tiff.read_ansiString(listModel.at(0).nOffset, listModel.at(0).nSize);
                }

                if ((sMake != "") || (sModel != "")) {
                    sResult = QString("%1(%2)").arg(sMake, sModel);
                }
            }
        }
    }

    return sResult;
}

QList<XTiff::CHUNK> XTiff::getExifChunks(QIODevice *pDevice, OFFSETSIZE osExif, PDSTRUCT *pPdStruct)
{
    QList<XTiff::CHUNK> listResult;

    if (osExif.nSize) {
        SubDevice sd(pDevice, osExif.nOffset, osExif.nSize);

        if (sd.open(QIODevice::ReadOnly)) {
            XTiff tiff(&sd);

            if (tiff.isValid()) {
                listResult = tiff.getChunks(pPdStruct);
            }

            sd.close();
        }
    }

    return listResult;
}

QString XTiff::getMIMEString()
{
    return "image/tiff-exif";
}

QString XTiff::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XTIFF_STRUCTID, sizeof(_TABLE_XTIFF_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XTiff::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.fileType = getFileType();

        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.nID = STRUCTID_SIGNATURE;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;
        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);
        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_SIGNATURE) {
                DATA_HEADER dh = _initDataHeader(dataHeadersOptions, structIDToString(dataHeadersOptions.nID));
                dh.nSize = 8;
                dh.listRecords.append(getDataRecord(0, 2, "Byte order", VT_CHAR_ARRAY, DRF_UNKNOWN, ENDIAN_LITTLE));
                dh.listRecords.append(getDataRecord(2, 2, "Magic", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                dh.listRecords.append(getDataRecord(4, 4, "IFD0 offset", VT_UINT32, DRF_OFFSET, isBigEndian() ? ENDIAN_BIG : ENDIAN_LITTLE));
                listResult.append(dh);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.dhMode = XBinary::DHMODE_TABLE;
                    _dataHeadersOptions.nID = STRUCTID_IFD_TABLE;
                    _dataHeadersOptions.nLocation = read_uint32(4, isBigEndian());
                    _dataHeadersOptions.locType = XBinary::LT_OFFSET;
                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_IFD_TABLE) {
                DATA_HEADER dh = _initDataHeader(dataHeadersOptions, structIDToString(dataHeadersOptions.nID));
                listResult.append(dh);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XTiff::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    QList<FPART> listResult;
    Q_UNUSED(nLimit)

    qint64 nTotal = getSize();

    if (nFileParts & FILEPART_HEADER) {
        FPART rec = {};
        rec.filePart = FILEPART_HEADER;
        rec.nFileOffset = 0;
        rec.nFileSize = 8;
        rec.nVirtualAddress = -1;
        rec.sName = tr("Header");
        listResult.append(rec);
    }

    if (nFileParts & (FILEPART_TABLE | FILEPART_REGION)) {
        QList<CHUNK> chunks = getChunks(pPdStruct);
        for (int i = 0; i < chunks.size(); ++i) {
            const CHUNK &c = chunks.at(i);
            if (c.nSize <= 4) continue;  // small inline values
            FPART rec = {};
            rec.filePart = (nFileParts & FILEPART_REGION) ? FILEPART_REGION : FILEPART_TABLE;
            rec.nFileOffset = c.nOffset;
            rec.nFileSize = qMin<qint64>(c.nSize, nTotal - c.nOffset);
            rec.nVirtualAddress = -1;
            rec.sName = tr("Data");
            listResult.append(rec);
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        qint64 nMax = 0;
        for (int i = 0; i < listResult.size(); ++i) {
            const FPART &p = listResult.at(i);
            nMax = qMax(nMax, p.nFileOffset + p.nFileSize);
        }
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

qint32 XTiff::getBaseTypeSize(quint16 nType)
{
    // 1 = BYTE 8-bit unsigned integer.
    // 2 = ASCII 8-bit byte that contains a 7-bit ASCII code; the last byte must be NUL (binary zero).
    // 3 = SHORT 16-bit (2-byte) unsigned integer.
    // 4 = LONG 32-bit (4-byte) unsigned integer.
    // 5 = RATIONAL Two LONGs: the first represents the numerator of a fraction; the second, the denominator
    // 6 = SBYTE An 8-bit signed (twos-complement) integer.
    // 7 = UNDEFINED An 8-bit byte that may contain anything, depending on the definition of the field.
    // 8 = SSHORT A 16-bit (2-byte) signed (twos-complement) integer.
    // 9 = SLONG A 32-bit (4-byte) signed (twos-complement) integer.
    // 10 = SRATIONAL Two SLONG’s: the first represents the numerator of a fraction, the second the denominator.
    // 11 = FLOAT Single precision (4-byte) IEEE format.
    // 12 = DOUBLE Double precision (8-byte) IEEE format.

    qint32 nResult = 0;

    switch (nType) {
        case 1: nResult = 1; break;
        case 2: nResult = 1; break;
        case 3: nResult = 2; break;
        case 4: nResult = 4; break;
        case 5: nResult = 8; break;
        case 6: nResult = 1; break;
        case 7: nResult = 1; break;
        case 8: nResult = 2; break;
        case 9: nResult = 4; break;
        case 10: nResult = 8; break;
        case 11: nResult = 4; break;
        case 12: nResult = 8; break;
        default: nResult = 0;
    }

    return nResult;
}

QList<QString> XTiff::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'II'..");
    listResult.append("'MM'..");

    return listResult;
}

XBinary *XTiff::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XTiff(pDevice);
}

