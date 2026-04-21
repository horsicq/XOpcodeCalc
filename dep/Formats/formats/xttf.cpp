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
#include "xttf.h"

XBinary::XCONVERT _TABLE_XTTF_STRUCTID[] = {
    {XTTF::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XTTF::STRUCTID_HEADER, "HEADER", QObject::tr("Header")},
    {XTTF::STRUCTID_TABLE_DIRECTORY, "TABLE_DIRECTORY", QObject::tr("Table Directory")},
    {XTTF::STRUCTID_TABLE, "TABLE", QObject::tr("Table")},
};

XTTF::XTTF(QIODevice *pDevice) : XBinary(pDevice)
{
}

XTTF::~XTTF()
{
}

bool XTTF::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    // TTF/OTF must have at least the offset table (12 bytes)
    if (getSize() >= 12) {
        quint32 nVersion = read_uint32(0, true);  // Big-endian

        // Check for valid TTF/OTF signatures:
        // 0x00010000 - TrueType
        // 'OTTO' (0x4F54544F) - OpenType with CFF
        // 'true' (0x74727565) - Apple TrueType
        // 'typ1' (0x74797031) - PostScript Type 1
        if ((nVersion == 0x00010000) || (nVersion == 0x4F54544F) || (nVersion == 0x74727565) || (nVersion == 0x74797031)) {
            // Verify number of tables is reasonable
            quint16 nNumTables = read_uint16(4, true);

            if ((nNumTables > 0) && (nNumTables <= 256)) {  // Reasonable limit
                // Verify the offset table size fits in the file
                qint64 nOffsetTableSize = 12 + (qint64)nNumTables * 16;

                if (getSize() >= nOffsetTableSize) {
                    bResult = true;
                }
            }
        }
    }

    return bResult;
}

bool XTTF::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XTTF xttf(pDevice);

    return xttf.isValid();
}

XBinary::FT XTTF::getFileType()
{
    return XBinary::FT_TTF;
}

XBinary::MODE XTTF::getMode()
{
    return XBinary::MODE_8;
}

QString XTTF::getMIMEString()
{
    return "font/ttf";
}

qint32 XTTF::getType()
{
    qint32 nResult = TYPE_UNKNOWN;

    if (isValid()) {
        quint32 nVersion = read_uint32(0, true);

        if (nVersion == 0x00010000 || nVersion == 0x74727565) {
            nResult = TYPE_TRUETYPE;
        } else if (nVersion == 0x4F54544F) {
            nResult = TYPE_OPENTYPE_CFF;
        }
    }

    return nResult;
}

QString XTTF::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_TRUETYPE: sResult = QString("TrueType"); break;
        case TYPE_OPENTYPE_CFF: sResult = QString("OpenType (CFF)"); break;
    }

    return sResult;
}

XBinary::ENDIAN XTTF::getEndian()
{
    return XBinary::ENDIAN_BIG;
}

QString XTTF::getArch()
{
    return QString();
}

QString XTTF::getFileFormatExt()
{
    QString sResult;

    qint32 nType = getType();

    if (nType == TYPE_OPENTYPE_CFF) {
        sResult = "otf";
    } else {
        sResult = "ttf";
    }

    return sResult;
}

QString XTTF::getFileFormatExtsString()
{
    QString sResult;

    qint32 nType = getType();

    if (nType == TYPE_OPENTYPE_CFF) {
        sResult = "OpenType Font (*.otf)";
    } else {
        sResult = "TrueType Font (*.ttf)";
    }

    return sResult;
}

qint64 XTTF::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return getSize();
}

bool XTTF::isSigned()
{
    return false;
}

XBinary::OSNAME XTTF::getOsName()
{
    return XBinary::OSNAME_UNKNOWN;
}

QString XTTF::getOsVersion()
{
    return QString();
}

QString XTTF::getVersion()
{
    QList<TTF_TABLE_RECORD> listTables = getTableDirectory(readHeader().numTables);

    return getVersion(&listTables, nullptr);
}

QString XTTF::getVersion(QList<TTF_TABLE_RECORD> *pListTables, PDSTRUCT *pPdStruct)
{
    QString sResult;

    qint32 nNumberOfTables = pListTables->count();
    qint64 nHeadOffset = -1;

    for (qint32 i = 0; (i < nNumberOfTables) && isPdStructNotCanceled(pPdStruct); i++) {
        if (pListTables->at(i).tag == 0x68656164) {  // 'head'
            nHeadOffset = pListTables->at(i).offset;
            break;
        }
    }

    if ((nHeadOffset != -1) && isOffsetValid(nHeadOffset + 8)) {
        quint32 nVersionFixed = read_uint32(nHeadOffset, true);
        quint32 nRevisionFixed = read_uint32(nHeadOffset + 4, true);

        QString sVersion = XBinary::get_uint32_version(nVersionFixed);
        QString sRevision = XBinary::get_uint32_version(nRevisionFixed);

        sResult = QString("%1 rev %2").arg(sVersion, sRevision);
    }

    return sResult;
}

bool XTTF::isEncrypted()
{
    return false;
}

QList<XBinary::MAPMODE> XTTF::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);
    listResult.append(MAPMODE_OBJECTS);

    return listResult;
}

XBinary::_MEMORY_MAP XTTF::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP result = {};

    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_REGIONS;  // Default mode
    }

    result.fileType = getFileType();
    result.mode = getMode();
    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = typeIdToString(getType());
    result.nImageSize = getSize();
    result.nBinarySize = getSize();
    result.nModuleAddress = getModuleAddress();

    if ((mapMode == MAPMODE_REGIONS) || (mapMode == MAPMODE_OBJECTS)) {
        // TTF Header
        TTF_HEADER header = readHeader();

        // Table Directory
        QList<TTF_TABLE_RECORD> tableRecords = getTableDirectory(header.numTables);

        if (mapMode == MAPMODE_REGIONS) {
            qint64 nHeaderOffset = 0;
            qint64 nHeaderSize = 12 + header.numTables * 16;

            _MEMORY_RECORD memHeader = {};
            memHeader.nOffset = nHeaderOffset;
            memHeader.nAddress = -1;
            memHeader.nSize = nHeaderSize;
            memHeader.filePart = FILEPART_HEADER;
            memHeader.sName = tr("Header + Table Directory");
            result.listRecords.append(memHeader);
        }

        // Add Table Objects
        if (mapMode == MAPMODE_OBJECTS) {
            for (qint32 i = 0, nNumberOfTables = tableRecords.size(); (i < nNumberOfTables) && isPdStructNotCanceled(pPdStruct); i++) {
                _MEMORY_RECORD rec = {};
                rec.nOffset = tableRecords.at(i).offset;
                rec.nSize = tableRecords.at(i).length;
                rec.filePart = FILEPART_OBJECT;
                rec.sName = tagToString(tableRecords.at(i).tag);
                rec.nIndex = i;
                rec.nAddress = -1;
                result.listRecords.append(rec);
            }
        }

        _handleOverlay(&result);
    }

    return result;
}

QList<XBinary::DATA_HEADER> XTTF::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<XBinary::DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;

        _dataHeadersOptions.nID = -1;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            // TODO
        }
    }

    return listResult;
}

QList<QString> XTTF::getTableTitles(const DATA_RECORDS_OPTIONS & /*dataRecordsOptions*/)
{
    QList<QString> list;
    list << "Tag"
         << "Checksum"
         << "Offset"
         << "Length";
    return list;
}

QString XTTF::tagToString(quint32 tag)
{
    QByteArray arr;
    arr.append(char((tag >> 24) & 0xFF));
    arr.append(char((tag >> 16) & 0xFF));
    arr.append(char((tag >> 8) & 0xFF));
    arr.append(char(tag & 0xFF));
    return QString::fromLatin1(arr);
}

quint32 XTTF::stringToTag(const QString &sTag)
{
    quint32 nResult = 0;

    if (sTag.length() >= 4) {
        QByteArray baTag = sTag.toLatin1();
        nResult = ((quint8)baTag[0] << 24) | ((quint8)baTag[1] << 16) | ((quint8)baTag[2] << 8) | (quint8)baTag[3];
    }

    return nResult;
}

QList<XTTF::TTF_TABLE_RECORD> XTTF::getTableDirectory(quint16 numTables)
{
    QList<TTF_TABLE_RECORD> listResult;

    qint64 nOffset = 12;
    qint64 nFileSize = getSize();

    for (qint32 i = 0; i < numTables; i++) {
        if (nOffset + 16 > nFileSize) {
            break;  // Prevent reading beyond file bounds
        }

        TTF_TABLE_RECORD rec = {};
        rec.tag = read_uint32(nOffset, true);
        rec.checkSum = read_uint32(nOffset + 4, true);
        rec.offset = read_uint32(nOffset + 8, true);
        rec.length = read_uint32(nOffset + 12, true);

        // Validate that table data is within file bounds
        if ((qint64)rec.offset + (qint64)rec.length <= nFileSize) {
            listResult.append(rec);
        }

        nOffset += 16;
    }

    return listResult;
}

XTTF::TTF_HEADER XTTF::readHeader()
{
    TTF_HEADER header = {};
    header.sfntVersion = read_uint32(0, true);
    header.numTables = read_uint16(4, true);
    header.searchRange = read_uint16(6, true);
    header.entrySelector = read_uint16(8, true);
    header.rangeShift = read_uint16(10, true);
    return header;
}

QString XTTF::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XTTF_STRUCTID, sizeof(_TABLE_XTTF_STRUCTID) / sizeof(XBinary::XCONVERT));
}

XTTF::TTF_TABLE_RECORD XTTF::getTableRecord(quint32 tag, QList<TTF_TABLE_RECORD> *pListTables)
{
    TTF_TABLE_RECORD result = {};

    qint32 nNumberOfTables = pListTables->count();

    for (qint32 i = 0; i < nNumberOfTables; i++) {
        if (pListTables->at(i).tag == tag) {
            result = pListTables->at(i);
            break;
        }
    }

    return result;
}

bool XTTF::isTablePresent(quint32 tag, QList<TTF_TABLE_RECORD> *pListTables)
{
    bool bResult = false;

    qint32 nNumberOfTables = pListTables->count();

    for (qint32 i = 0; i < nNumberOfTables; i++) {
        if (pListTables->at(i).tag == tag) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

qint64 XTTF::getTableOffset(quint32 tag, QList<TTF_TABLE_RECORD> *pListTables)
{
    qint64 nResult = -1;

    TTF_TABLE_RECORD record = getTableRecord(tag, pListTables);

    if (record.tag == tag) {
        nResult = record.offset;
    }

    return nResult;
}

qint64 XTTF::getTableSize(quint32 tag, QList<TTF_TABLE_RECORD> *pListTables)
{
    qint64 nResult = 0;

    TTF_TABLE_RECORD record = getTableRecord(tag, pListTables);

    if (record.tag == tag) {
        nResult = record.length;
    }

    return nResult;
}

QMap<quint64, QString> XTTF::getHeaderVersions()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00010000, "TrueType");
    mapResult.insert(0x4F54544F, "OpenType_CFF");
    mapResult.insert(0x74727565, "TrueType_Apple");
    mapResult.insert(0x74797031, "PostScript_Type1");

    return mapResult;
}

QMap<quint64, QString> XTTF::getHeaderVersionsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00010000, "TrueType 1.0");
    mapResult.insert(0x4F54544F, "OpenType with CFF");
    mapResult.insert(0x74727565, "Apple TrueType");
    mapResult.insert(0x74797031, "PostScript Type 1");

    return mapResult;
}

