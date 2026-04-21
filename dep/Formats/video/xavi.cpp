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
#include "xavi.h"

XBinary::XCONVERT _TABLE_XAVI_STRUCTID[] = {{XAVI::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
                                            {XAVI::STRUCTID_HEADER, "HEADER", QString("Header")},
                                            {XAVI::STRUCTID_CHUNK, "CHUNK", QString("Chunk")},
                                            {XAVI::STRUCTID_AVIH, "AVIH", QString("AVI Header")},
                                            {XAVI::STRUCTID_STRH, "STRH", QString("Stream Header")},
                                            {XAVI::STRUCTID_STRF, "STRF", QString("Stream Format")},
                                            {XAVI::STRUCTID_JUNK, "JUNK", QString("Junk")},
                                            {XAVI::STRUCTID_MOVI, "MOVI", QString("Movie Data")},
                                            {XAVI::STRUCTID_IDX1, "IDX1", QString("Index")}};

XAVI::XAVI(QIODevice *pDevice) : XRiff(pDevice)
{
}

XAVI::~XAVI()
{
}

bool XAVI::isValid(PDSTRUCT *pPdStruct)
{
    // First check if it's a valid RIFF file
    if (!XRiff::isValid(pPdStruct)) {
        return false;
    }

    // AVI files have form type "AVI " at offset 8
    QString sForm = read_ansiString(8, 4);

    return (sForm == "AVI ");
}

bool XAVI::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XAVI x(pDevice);
    return x.isValid();
}

QString XAVI::getFileFormatExt()
{
    return "avi";
}

QString XAVI::getFileFormatExtsString()
{
    return "AVI";
}

qint64 XAVI::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

XBinary::FT XAVI::getFileType()
{
    return FT_AVI;
}

QString XAVI::getMIMEString()
{
    return "video/x-msvideo";
}

QString XAVI::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XAVI_STRUCTID, sizeof(_TABLE_XAVI_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QString XAVI::getArch()
{
    return "";  // AVI is a multimedia container, no specific architecture
}

XBinary::MODE XAVI::getMode()
{
    return MODE_DATA;  // AVI files contain data, not executable code
}

XBinary::ENDIAN XAVI::getEndian()
{
    return XRiff::getEndian();  // Delegate to parent class
}

QString XAVI::getVersion()
{
    return "";  // AVI format doesn't have a version number
}

QList<XBinary::MAPMODE> XAVI::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);
    listResult.append(MAPMODE_DATA);

    return listResult;
}

XBinary::_MEMORY_MAP XAVI::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_DATA;  // Default mode
    }

    if (mapMode == MAPMODE_REGIONS) {
        result = _getMemoryMap(FILEPART_HEADER | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
    } else if (mapMode == MAPMODE_DATA) {
        result = _getMemoryMap(FILEPART_DATA | FILEPART_OVERLAY, pPdStruct);
    }

    return result;
}

QList<XBinary::DATA_HEADER> XAVI::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        // Initialize with default headers
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        // Start with RIFF header
        _dataHeadersOptions.nID = STRUCTID_HEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_HEADER) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XAVI::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 12;

                dataHeader.listRecords.append(getDataRecord(0, 4, "RIFF Tag", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(4, 4, "File Size", VT_UINT32, DRF_SIZE, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(8, 4, "AVI Form", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));

                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XAVI::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<FPART> listResult;

    if (nFileParts & FILEPART_HEADER) {
        FPART record = {};

        record.filePart = FILEPART_HEADER;
        record.nFileOffset = 0;
        record.nFileSize = 12;
        record.nVirtualAddress = -1;
        record.sName = tr("RIFF Header");

        listResult.append(record);
    }

    if (nFileParts & FILEPART_REGION) {
        qint64 nTotalSize = getSize();
        quint32 nRiffSize = read_uint32(4, false);  // Little-endian
        qint64 nContentSize = qMin<qint64>(nTotalSize, (qint64)nRiffSize + 8);

        FPART record = {};

        record.filePart = FILEPART_REGION;
        record.nFileOffset = 0;
        record.nFileSize = nContentSize;
        record.nVirtualAddress = -1;
        record.sName = "AVI";

        listResult.append(record);
    }

    if (nFileParts & FILEPART_DATA) {
        qint64 nTotalSize = getSize();

        FPART record = {};

        record.filePart = FILEPART_DATA;
        record.nFileOffset = 0;
        record.nFileSize = nTotalSize;
        record.nVirtualAddress = -1;
        record.sName = tr("Data");

        listResult.append(record);
    }

    if (nFileParts & FILEPART_OVERLAY) {
        quint32 nRiffSize = read_uint32(4, false);  // Little-endian
        qint64 nContentEnd = (qint64)nRiffSize + 8;
        qint64 nTotalSize = getSize();

        if (nContentEnd < nTotalSize) {
            FPART record = {};

            record.filePart = FILEPART_OVERLAY;
            record.nFileOffset = nContentEnd;
            record.nFileSize = nTotalSize - nContentEnd;
            record.nVirtualAddress = -1;
            record.sName = tr("Overlay");

            listResult.append(record);
        }
    }

    return listResult;
}

QList<QString> XAVI::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'RIFF'....'AVI '");

    return listResult;
}

XBinary *XAVI::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XAVI(pDevice);
}

