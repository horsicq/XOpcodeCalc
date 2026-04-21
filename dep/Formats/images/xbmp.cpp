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
#include "xbmp.h"

XBinary::XCONVERT _TABLE_XBMP_STRUCTID[] = {
    {XBMP::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XBMP::STRUCTID_BMPFILEHEADER, "BMPFILEHEADER", QString("BMP File Header")},
    {XBMP::STRUCTID_BMPINFOHEADER, "BMPINFOHEADER", QString("BMP Info Header")},
};

XBMP::XBMP(QIODevice *pDevice) : XBinary(pDevice)
{
}

XBMP::~XBMP()
{
}

bool XBMP::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    if (compareSignature(&memoryMap, "'BM'..................000000", 0, pPdStruct)) {
        quint32 nSize = read_uint32(2);

        if ((nSize > 0) && (nSize <= getSize())) {
            quint32 nDBISize = read_uint32(0x0E);

            if (nDBISize == 40) {
                bResult = true;
            } else if (nDBISize == 108) {
                bResult = true;
            } else if (nDBISize == 124) {
                bResult = true;
            }
        }
    }

    // TODO
    //    BM Windows 3.1x, 95, NT, ... etc.
    //    BA OS/2 struct bitmap array
    //    CI OS/2 struct color icon
    //    CP OS/2 const color pointer
    //    IC OS/2 struct icon
    //    PT OS/2 pointer

    return bResult;
}

bool XBMP::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XBMP xbmp(pDevice);

    return xbmp.isValid();
}

QString XBMP::getMIMEString()
{
    return "image/bmp";
}

QString XBMP::getArch()
{
    return "";
}

XBinary::ENDIAN XBMP::getEndian()
{
    return XBinary::ENDIAN_LITTLE;
}

XBinary::MODE XBMP::getMode()
{
    return XBinary::MODE_DATA;
}

XBinary::FT XBMP::getFileType()
{
    return FT_BMP;
}

QString XBMP::getFileFormatExt()
{
    return "bmp";
}

QString XBMP::getFileFormatExtsString()
{
    return "BMP (*.bmp)";
}

qint64 XBMP::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    qint64 nResult = 0;

    BMPFILEHEADER fileHeader = getFileHeader();

    if (fileHeader.bfSize > 0 && fileHeader.bfSize <= (quint64)getSize()) {
        nResult = fileHeader.bfSize;
    }

    return nResult;
}

XBinary::_MEMORY_MAP XBMP::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    _MEMORY_MAP result = {};
    result.fileType = getFileType();
    result.mode = getMode();
    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = typeIdToString(getType());
    result.nBinarySize = getSize();
    result.nImageSize = getSize();
    result.nModuleAddress = getModuleAddress();
    result.bIsImage = false;

    BMPFILEHEADER fileHeader = getFileHeader();
    BMPINFOHEADER infoHeader = getInfoHeader();

    // Add Header
    _MEMORY_RECORD headerRecord = {};
    headerRecord.nOffset = 0;
    headerRecord.nAddress = -1;
    headerRecord.nSize = 14 + infoHeader.biSize;
    headerRecord.filePart = FILEPART_HEADER;
    headerRecord.sName = "Header";
    headerRecord.nIndex = 0;
    result.listRecords.append(headerRecord);

    // Add Bitmap Data (Object)
    _MEMORY_RECORD objectRecord = {};
    objectRecord.nOffset = fileHeader.bfOffBits;
    objectRecord.nAddress = -1;
    objectRecord.nSize = fileHeader.bfSize - fileHeader.bfOffBits;
    objectRecord.filePart = FILEPART_DATA;
    objectRecord.sName = "Bitmap Data";
    objectRecord.nIndex = 1;
    result.listRecords.append(objectRecord);

    _handleOverlay(&result);

    return result;
}

QString XBMP::getVersion()
{
    QString sResult;

    quint32 nDBISize = read_uint32(0x0E);

    if (nDBISize == 40) {
        sResult = "Windows NT, 3.1x";
    } else if (nDBISize == 108) {
        sResult = "Windows NT 4.0, 95";
    } else if (nDBISize == 124) {
        sResult = "Windows NT 5.0, 98";
    }

    return sResult;
}

QString XBMP::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XBMP_STRUCTID, sizeof(_TABLE_XBMP_STRUCTID) / sizeof(XBinary::XCONVERT));
}

XBMP::BMPINFOHEADER XBMP::getInfoHeader()
{
    BMPINFOHEADER info = {};
    info.biSize = read_uint32(14, false);
    info.biWidth = read_int32(18, false);
    info.biHeight = read_int32(22, false);
    info.biPlanes = read_uint16(26, false);
    info.biBitCount = read_uint16(28, false);
    info.biCompression = read_uint32(30, false);
    info.biSizeImage = read_uint32(34, false);
    info.biXPelsPerMeter = read_int32(38, false);
    info.biYPelsPerMeter = read_int32(42, false);
    info.biClrUsed = read_uint32(46, false);
    info.biClrImportant = read_uint32(50, false);
    return info;
}

XBMP::BMPFILEHEADER XBMP::getFileHeader()
{
    BMPFILEHEADER header = {};
    header.bfType = read_uint16(0, false);
    header.bfSize = read_uint32(2, false);
    header.bfReserved1 = read_uint16(6, false);
    header.bfReserved2 = read_uint16(8, false);
    header.bfOffBits = read_uint32(10, false);
    return header;
}

QList<XBinary::DATA_HEADER> XBMP::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = FT_BMP;

        _dataHeadersOptions.nID = STRUCTID_BMPFILEHEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;
        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);
        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_BMPFILEHEADER) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XBMP::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 14;  // BITMAPFILEHEADER
                dataHeader.listRecords.append(getDataRecord(0, 2, "bfType", VT_CHAR_ARRAY, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(2, 4, "bfSize", VT_UINT32, DRF_SIZE, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(6, 2, "bfReserved1", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(8, 2, "bfReserved2", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                dataHeader.listRecords.append(getDataRecord(10, 4, "bfOffBits", VT_UINT32, DRF_OFFSET, ENDIAN_LITTLE));
                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_BMPINFOHEADER;
                    _dataHeadersOptions.nLocation = dataHeadersOptions.nLocation + 14;
                    _dataHeadersOptions.locType = dataHeadersOptions.locType;
                    _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_BMPINFOHEADER) {
                // The first field contains the header size and defines the variant
                quint32 biSize = read_uint32(nStartOffset, false);
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XBMP::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = biSize;
                dataHeader.listRecords.append(getDataRecord(0, 4, "biSize", VT_UINT32, DRF_SIZE, ENDIAN_LITTLE));
                if (biSize >= 40) {
                    dataHeader.listRecords.append(getDataRecord(4, 4, "biWidth", VT_INT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(8, 4, "biHeight", VT_INT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(12, 2, "biPlanes", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(14, 2, "biBitCount", VT_UINT16, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(16, 4, "biCompression", VT_UINT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(20, 4, "biSizeImage", VT_UINT32, DRF_SIZE, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(24, 4, "biXPelsPerMeter", VT_INT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(28, 4, "biYPelsPerMeter", VT_INT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(32, 4, "biClrUsed", VT_UINT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                    dataHeader.listRecords.append(getDataRecord(36, 4, "biClrImportant", VT_UINT32, DRF_UNKNOWN, ENDIAN_LITTLE));
                }
                // For BI_BITFIELDS (if biSize >= 52/56/108/124), masks follow; we keep header generic here
                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XBMP::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<FPART> listResult;

    BMPFILEHEADER fh = getFileHeader();
    BMPINFOHEADER ih = getInfoHeader();

    qint64 nTotal = getSize();
    qint64 nMaxOffset = 14 + ih.biSize;

    if (nFileParts & FILEPART_HEADER) {
        FPART rec = {};
        rec.filePart = FILEPART_HEADER;
        rec.nFileOffset = 0;
        rec.nFileSize = 14 + ih.biSize;
        rec.nVirtualAddress = -1;
        rec.sName = tr("Header");
        listResult.append(rec);
    }

    if ((fh.bfOffBits > 0) && (fh.bfOffBits < nTotal) && (fh.bfSize <= (quint64)nTotal)) {
        if (nFileParts & FILEPART_DATA) {
            FPART rec = {};
            rec.filePart = FILEPART_DATA;
            rec.nFileOffset = fh.bfOffBits;
            rec.nFileSize = qMin<qint64>(fh.bfSize - fh.bfOffBits, nTotal - fh.bfOffBits);
            rec.nVirtualAddress = -1;
            rec.sName = tr("Bitmap Data");
            listResult.append(rec);
        }

        nMaxOffset = qMax(nMaxOffset, (qint64)(fh.bfSize));
    }

    if (nFileParts & FILEPART_OVERLAY) {
        if (nMaxOffset < nTotal) {
            FPART rec = {};
            rec.filePart = FILEPART_OVERLAY;
            rec.nFileOffset = nMaxOffset;
            rec.nFileSize = nTotal - nMaxOffset;
            rec.nVirtualAddress = -1;
            rec.sName = tr("Overlay");
            listResult.append(rec);
        }
    }

    return listResult;
}

QList<QString> XBMP::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'BM'");

    return listResult;
}

XBinary *XBMP::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XBMP(pDevice);
}

