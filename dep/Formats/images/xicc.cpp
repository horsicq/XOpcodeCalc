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
#include "xicc.h"

XBinary::XCONVERT _TABLE_XICC_STRUCTID[] = {
    {XICC::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XICC::STRUCTID_HEADER, "Header", QObject::tr("Header")},
    {XICC::STRUCTID_TAG, "Tag", QObject::tr("Tag")},
    {XICC::STRUCTID_REGION, "Region", QString("Region")},
};

XICC::XICC(QIODevice *pDevice) : XBinary(pDevice)
{
}

XICC::~XICC()
{
}

bool XICC::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 128) {
        // ICC profiles start with profile size and have specific signatures
        quint32 nProfileSize = read_uint32(0, true);
        quint32 nSignature = read_uint32(12, true);

        // 'scnr' 73636E72h input devices - scanners and digital cameras
        // 'mntr' 6D6E7472h display devices - CRTs and LCDs
        // 'prtr' 70727472h output devices - printers.

        bIsValid = (nProfileSize <= getSize()) && ((nSignature == 0x73636E72) || (nSignature == 0x6D6E7472) || (nSignature == 0x70727472));
    }

    return bIsValid;
}

bool XICC::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XICC xicc(pDevice);

    return xicc.isValid();
}

XBinary::_MEMORY_MAP XICC::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_REGIONS;  // Default mode
    }

    if (mapMode == MAPMODE_REGIONS) {
        result = _getMemoryMap(FILEPART_HEADER | FILEPART_TABLE | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
    }

    return result;
}

XBinary::FT XICC::getFileType()
{
    return FT_ICC;
}

QString XICC::getFileFormatExt()
{
    return "icc";
}

QString XICC::getFileFormatExtsString()
{
    return "ICC Profile (*.icc *.icm)";
}

qint64 XICC::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XICC::getVersion()
{
    QString sResult;

    quint32 nVersion = read_uint32(8, true);
    quint8 nMajor = (nVersion >> 24) & 0xFF;
    quint8 nMinor = (nVersion >> 20) & 0x0F;
    quint8 nBugFix = (nVersion >> 16) & 0x0F;

    sResult = QString("%1.%2.%3").arg(QString::number(nMajor), QString::number(nMinor), QString::number(nBugFix));

    return sResult;
}

XICC::HEADER XICC::getHeader()
{
    HEADER result = {};

    result.nProfileSize = read_uint32(0, true);
    result.nCMMType = read_uint32(4, true);
    result.nVersion = read_uint32(8, true);
    result.nDeviceClass = read_uint32(12, true);
    result.nDataColorSpace = read_uint32(16, true);
    result.nPCS = read_uint32(20, true);

    read_array(24, (char *)result.nDate, 12);

    result.nPlatform = read_uint32(40, true);
    result.nFlags = read_uint32(44, true);
    result.nDeviceManufacturer = read_uint32(48, true);
    result.nDeviceModel = read_uint32(52, true);
    result.nDeviceAttributes = read_uint64(56, true);
    result.nRenderingIntent = read_uint32(64, true);
    result.nIlluminantX = read_uint32(68, true);
    result.nIlluminantY = read_uint32(72, true);
    result.nIlluminantZ = read_uint32(76, true);
    result.nCreator = read_uint32(80, true);

    read_array(84, (char *)result.nReserved, 44);

    return result;
}

QList<XICC::TAG> XICC::getTags(PDSTRUCT *pPdStruct)
{
    QList<TAG> listResult;

    if (getSize() > 132) {
        quint32 nTagCount = read_uint32(128, true);

        for (quint32 i = 0; (i < nTagCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
            TAG tag = _readTag(132 + (i * 12));

            if (tag.bValid) {
                listResult.append(tag);
            }
        }
    }

    return listResult;
}

QList<XICC::TAG> XICC::_getTagsBySignature(QList<TAG> *pListTags, quint32 nSignature)
{
    QList<XICC::TAG> listResult;

    qint32 nNumberOfRecords = pListTags->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListTags->at(i).nSignature == nSignature) {
            listResult.append(pListTags->at(i));
        }
    }

    return listResult;
}

QString XICC::getTagName(quint32 nSignature)
{
    QString sResult;

    switch (nSignature) {
        case 0x64657363: sResult = "desc"; break;  // Description
        case 0x63707274: sResult = "cprt"; break;  // Copyright
        case 0x77747074: sResult = "wtpt"; break;  // White point
        case 0x72545243: sResult = "rTRC"; break;  // Red tone reproduction curve
        case 0x67545243: sResult = "gTRC"; break;  // Green tone reproduction curve
        case 0x62545243: sResult = "bTRC"; break;  // Blue tone reproduction curve
        case 0x72585950: sResult = "rXYZ"; break;  // Red colorant
        case 0x67585950: sResult = "gXYZ"; break;  // Green colorant
        case 0x62585950: sResult = "bXYZ"; break;  // Blue colorant
        case 0x646D6E64: sResult = "dmnd"; break;  // Device manufacturer description
        case 0x646D6464: sResult = "dmdd"; break;  // Device model description
        default: sResult = _fourCCToString(nSignature); break;
    }

    return sResult;
}

QString XICC::getDescription(QList<TAG> *pListTags, PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    QString sResult;

    QList<XICC::TAG> listDesc = _getTagsBySignature(pListTags, 0x64657363);  // 'desc'

    if (listDesc.count() > 0) {
        TAG tagDesc = listDesc.at(0);
        quint32 nType = read_uint32(tagDesc.nOffset, true);

        if (nType == 0x64657363) {  // 'desc' type
            sResult = _readTextType(tagDesc.nOffset);
        } else if (nType == 0x6D6C7563) {  // 'mluc' type
            sResult = _readMultiLocalizedUnicodeType(tagDesc.nOffset);
        }
    }

    return sResult;
}

QString XICC::getDescription()
{
    QList<TAG> listTags = getTags();

    return getDescription(&listTags);
}

QString XICC::getCopyright(QList<TAG> *pListTags, PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    QString sResult;

    QList<XICC::TAG> listCopyright = _getTagsBySignature(pListTags, 0x63707274);  // 'cprt'

    if (listCopyright.count() > 0) {
        TAG tagCopyright = listCopyright.at(0);
        quint32 nType = read_uint32(tagCopyright.nOffset, true);

        if (nType == 0x74657874) {  // 'text' type
            sResult = _readTextType(tagCopyright.nOffset);
        } else if (nType == 0x6D6C7563) {  // 'mluc' type
            sResult = _readMultiLocalizedUnicodeType(tagCopyright.nOffset);
        }
    }

    return sResult;
}

QString XICC::getCopyright()
{
    QList<TAG> listTags = getTags();

    return getCopyright(&listTags);
}

QString XICC::getDeviceManufacturer()
{
    HEADER header = getHeader();
    return _fourCCToString(header.nDeviceManufacturer);
}

QString XICC::getDeviceModel()
{
    HEADER header = getHeader();
    return _fourCCToString(header.nDeviceModel);
}

QString XICC::getColorSpace()
{
    HEADER header = getHeader();
    return _fourCCToString(header.nDataColorSpace);
}

QString XICC::getDeviceClass()
{
    HEADER header = getHeader();

    QString sResult;
    switch (header.nDeviceClass) {
        case 0x73636E72: sResult = "Input"; break;       // 'scnr'
        case 0x6D6E7472: sResult = "Display"; break;     // 'mntr'
        case 0x70727472: sResult = "Output"; break;      // 'prtr'
        case 0x6C696E6B: sResult = "DeviceLink"; break;  // 'link'
        case 0x73706163: sResult = "ColorSpace"; break;  // 'spac'
        case 0x61627374: sResult = "Abstract"; break;    // 'abst'
        case 0x6E6D636C: sResult = "NamedColor"; break;  // 'nmcl'
        default: sResult = _fourCCToString(header.nDeviceClass); break;
    }

    return sResult;
}

QString XICC::getMIMEString()
{
    return "application/vnd.iccprofile";
}

QString XICC::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XICC_STRUCTID, sizeof(_TABLE_XICC_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XICC::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        _dataHeadersOptions.nID = STRUCTID_HEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_HEADER) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XICC::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 128;

                dataHeader.listRecords.append(getDataRecord(0, 4, "Profile Size", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(4, 4, "CMM Type", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(8, 4, "Version", XBinary::VT_UINT32, DRF_VERSION, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(12, 4, "Device Class", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(16, 4, "Data Color Space", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(20, 4, "PCS", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(24, 12, "Date", XBinary::VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(40, 4, "Platform", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(44, 4, "Flags", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(48, 4, "Device Manufacturer", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(52, 4, "Device Model", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(56, 8, "Device Attributes", XBinary::VT_UINT64, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(64, 4, "Rendering Intent", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(68, 4, "Illuminant X", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(72, 4, "Illuminant Y", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(76, 4, "Illuminant Z", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(80, 4, "Creator", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(84, 44, "Reserved", XBinary::VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XICC::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<FPART> listResult;

    if (nFileParts & FILEPART_HEADER) {
        FPART record = {};

        record.filePart = FILEPART_HEADER;
        record.nFileOffset = 0;
        record.nFileSize = 128;
        record.nVirtualAddress = -1;

        listResult.append(record);
    }

    qint64 nMaxOffset = 128;
    qint64 nTotalSize = getSize();

    if (nFileParts & FILEPART_TABLE) {
        quint32 nTagCount = read_uint32(128, true);

        FPART record = {};

        record.filePart = FILEPART_TABLE;
        record.nFileOffset = 128;
        record.nFileSize = 4 + (nTagCount * 12);
        record.nVirtualAddress = -1;

        listResult.append(record);
    }

    if (nFileParts & FILEPART_OBJECT) {
        QList<XICC::TAG> listTags = getTags(pPdStruct);

        for (qint32 i = 0; i < listTags.count(); i++) {
            FPART record = {};

            record.filePart = FILEPART_OBJECT;
            record.nFileOffset = listTags.at(i).nOffset;
            record.nFileSize = listTags.at(i).nSize;
            record.nVirtualAddress = -1;

            listResult.append(record);

            nMaxOffset = qMax((record.nFileOffset + record.nFileSize), nMaxOffset);
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        if (nMaxOffset < nTotalSize) {
            FPART record = {};

            record.filePart = FILEPART_OVERLAY;
            record.nFileOffset = nMaxOffset;
            record.nFileSize = nTotalSize - nMaxOffset;
            record.nVirtualAddress = -1;

            listResult.append(record);
        }
    }

    return listResult;
}

// qint32 XICC::readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
//                             void *pUserData, PDSTRUCT *pPdStruct)
// {
//     Q_UNUSED(pUserData)
//     Q_UNUSED(pPdStruct)

//     qint32 nResult = 0;

//     if (dataRecordsOptions.nID == STRUCTID_TAG) {
//         if (nRow < getTags().count()) {
//             QList<XICC::TAG> listTags = getTags();
//             XICC::TAG tag = listTags.at(nRow);

//             DATA_RECORD_ROW record = {};

//             record.nRow = nRow;
//             record.nLocation = XBinary::getAbsoluteAddress(this, locType, nLocation + (nRow * 12));
//             record.nSize = 12;

//             record.listRecords.append(getDataRecord(0, 4, "Signature", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
//             record.listRecords.append(getDataRecord(4, 4, "Offset", XBinary::VT_UINT32, DRF_OFFSET, XBinary::ENDIAN_BIG));
//             record.listRecords.append(getDataRecord(8, 4, "Size", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));

//             pListDataRecords->append(record);

//             nResult = 1;
//         }
//     }

//     return nResult;
// }

XICC::TAG XICC::_readTag(qint64 nOffset)
{
    TAG result = {};

    result.nSignature = read_uint32(nOffset, true);
    result.nOffset = read_uint32(nOffset + 4, true);
    result.nSize = read_uint32(nOffset + 8, true);

    result.sTagName = getTagName(result.nSignature);
    result.bValid = (result.nOffset > 0) && (result.nSize > 0) && (result.nOffset + result.nSize <= getSize());

    return result;
}

QString XICC::_readTextType(qint64 nOffset)
{
    QString sResult;

    quint32 nType = read_uint32(nOffset, true);

    if (nType == 0x74657874) {  // 'text'
        // Skip type signature (4 bytes) and reserved (4 bytes)
        sResult = read_ansiString(nOffset + 8);
    } else if (nType == 0x64657363) {  // 'desc'
        // Skip type signature (4 bytes) and reserved (4 bytes)
        quint32 nLength = read_uint32(nOffset + 8, true);
        if (nLength > 0) {
            sResult = read_ansiString(nOffset + 12, qMin(nLength, quint32(1000)));
        }
    }

    return sResult;
}

QString XICC::_readMultiLocalizedUnicodeType(qint64 nOffset)
{
    QString sResult;

    quint32 nType = read_uint32(nOffset, true);

    if (nType == 0x6D6C7563) {  // 'mluc'
        // Skip type signature (4 bytes) and reserved (4 bytes)
        quint32 nRecordCount = read_uint32(nOffset + 8, true);

        if (nRecordCount > 0) {
            // Read first record (English preferred)
            quint32 nLength = read_uint32(nOffset + 16, true);
            quint32 nRecordOffset = read_uint32(nOffset + 20, true);

            if (nLength > 0 && nRecordOffset > 0) {
                sResult = read_unicodeString(nOffset + nRecordOffset, qMin(nLength / 2, quint32(500)), true);
            }
        }
    }

    return sResult;
}

QString XICC::_fourCCToString(quint32 nValue)
{
    QString sResult;

    char szBuffer[5];
    szBuffer[0] = (nValue >> 24) & 0xFF;
    szBuffer[1] = (nValue >> 16) & 0xFF;
    szBuffer[2] = (nValue >> 8) & 0xFF;
    szBuffer[3] = nValue & 0xFF;
    szBuffer[4] = '\0';

    sResult = QString::fromLatin1(szBuffer);

    return sResult;
}

