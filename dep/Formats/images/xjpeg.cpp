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
#include "xjpeg.h"

static XBinary::XCONVERT _TABLE_XJPEG_STRUCTID[] = {
    {XJpeg::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XJpeg::STRUCTID_SIGNATURE, "Signature", QString("Signature")},
    {XJpeg::STRUCTID_CHUNK, "CHUNK", QString("Chunk")},
};

XJpeg::XJpeg(QIODevice *pDevice) : XBinary(pDevice)
{
}

XJpeg::~XJpeg()
{
}

bool XJpeg::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);
        bIsValid = compareSignature(&memoryMap, "FFD8FFE0....'JFIF'00", 0, pPdStruct) || compareSignature(&memoryMap, "FFD8FFE1....'Exif'00", 0, pPdStruct) ||
                   compareSignature(&memoryMap, "FFD8FFDB", 0, pPdStruct);
    }

    return bIsValid;
}

bool XJpeg::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XJpeg xjpeg(pDevice);

    return xjpeg.isValid();
}

XBinary::_MEMORY_MAP XJpeg::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    XBinary::_MEMORY_MAP result = {};
    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    QList<XJpeg::CHUNK> listChunks = getChunks(pPdStruct);

    qint32 nNumberOfChunks = listChunks.count();

    for (qint32 i = 0; (i < nNumberOfChunks) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        _MEMORY_RECORD record = {};

        record.nIndex = nIndex++;

        if (listChunks.at(i).bEntropyCodedData) {
            record.filePart = FILEPART_DATA;
            record.sName = tr("Data");
        } else {
            record.filePart = FILEPART_OBJECT;
            record.sName = XBinary::valueToHex(listChunks.at(i).nId);
        }

        record.nOffset = listChunks.at(i).nDataOffset;
        record.nSize = listChunks.at(i).nDataSize;
        record.nAddress = -1;

        result.listRecords.append(record);
    }

    _handleOverlay(&result);

    return result;
}

XBinary::FT XJpeg::getFileType()
{
    return FT_JPEG;
}

QString XJpeg::getFileFormatExt()
{
    return "jpeg";
}

QString XJpeg::getFileFormatExtsString()
{
    return "JPEG (*.jpeg *.jpg)";
}

qint64 XJpeg::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XJpeg::getVersion()
{
    QString sResult;

    QString sIdent = read_ansiString(6, 5);

    if (sIdent == "JFIF") {
        quint8 nMajor = read_uint8(0x0B);
        quint8 nMinor = read_uint8(0x0C);

        sResult = QString("%1.%2").arg(QString::number(nMajor), QString::number(nMinor));
    }

    return sResult;
}

QList<XJpeg::CHUNK> XJpeg::getChunks(PDSTRUCT *pPdStruct)
{
    QList<CHUNK> listResult;

    qint64 nOffset = 0;

    while (XBinary::isPdStructNotCanceled(pPdStruct)) {
        if (nOffset == -1) {
            break;
        }

        CHUNK chunk = _readChunk(nOffset);

        if (!chunk.bValid) {
            break;
        }

        listResult.append(chunk);

        nOffset = chunk.nDataOffset + chunk.nDataSize;

        if (chunk.nId == 0xDA) {
            CHUNK chunkData = {};
            chunkData.bValid = true;
            chunkData.bEntropyCodedData = true;
            chunkData.nDataOffset = nOffset;

            while (true) {
                nOffset = find_uint8(nOffset, -1, 0xFF, pPdStruct);

                if (nOffset == -1) {
                    break;
                }

                if (read_uint8(nOffset + 1) != 0x00) {
                    break;
                } else {
                    nOffset++;
                }
            }

            chunkData.nDataSize = nOffset - chunkData.nDataOffset;

            listResult.append(chunkData);
        }

        if (chunk.nId == 0xD9) {  // END of image
            break;
        }
    }

    return listResult;
}

QList<XJpeg::CHUNK> XJpeg::_getChunksById(QList<CHUNK> *pListChunks, quint8 nId)
{
    QList<XJpeg::CHUNK> listResult;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nId == nId) {
            listResult.append(pListChunks->at(i));
        }
    }

    return listResult;
}

QString XJpeg::getComment(QList<CHUNK> *pListChunks, PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    QString sResult;

    QList<XJpeg::CHUNK> listComments = _getChunksById(pListChunks, 0xFE);  // COMMENT

    qint32 nNumberOfRecords = listComments.count();

    for (qint32 i = 0; (i < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        sResult += read_ansiString(listComments.at(i).nDataOffset + 4, listComments.at(i).nDataSize - 4);
    }

    if (sResult.size() > 100) {
        sResult.resize(100);
    }

    sResult = sResult.remove("\r").remove("\n");

    return sResult;
}

QString XJpeg::getComment()
{
    QList<CHUNK> listChunks = getChunks();

    return getComment(&listChunks);
}

QString XJpeg::getDqtMD5(QList<CHUNK> *pListChunks)
{
    QString sResult;

    QList<XJpeg::CHUNK> listComments = _getChunksById(pListChunks, 0xDB);  // DQT

    qint32 nNumberOfRecords = listComments.count();

    QCryptographicHash crypto(QCryptographicHash::Md5);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        QByteArray baData = read_array(listComments.at(i).nDataOffset + 4, listComments.at(i).nDataSize - 4);

        crypto.addData(baData);
    }

    sResult = crypto.result().toHex();

    return sResult;
}

QString XJpeg::getDqtMD5()
{
    QList<CHUNK> listChunks = getChunks();

    return getDqtMD5(&listChunks);
}

bool XJpeg::isChunkPresent(QList<CHUNK> *pListChunks, quint8 nId)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pListChunks->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListChunks->at(i).nId == nId) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XBinary::OFFSETSIZE XJpeg::getExif(QList<CHUNK> *pListChunks)
{
    OFFSETSIZE result = {};

    QList<CHUNK> listExif = _getChunksById(pListChunks, 0xE1);

    if (listExif.count() > 0) {
        CHUNK chunkExif = listExif.at(0);

        if (chunkExif.nDataSize > 10) {
            if (read_ansiString(chunkExif.nDataOffset + 4) == "Exif") {
                result.nOffset = chunkExif.nDataOffset + 10;
                result.nSize = chunkExif.nDataSize - 10;
            }
        }
    }

    return result;
}

bool XJpeg::isExifPresent(OFFSETSIZE osExif)
{
    return osExif.nSize;
}

QString XJpeg::getMIMEString()
{
    return "image/jpeg";
}

QString XJpeg::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XJPEG_STRUCTID, sizeof(_TABLE_XJPEG_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XJpeg::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = getFileType();

        _dataHeadersOptions.nID = STRUCTID_SIGNATURE;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;
        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);
        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_SIGNATURE) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJpeg::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2;  // SOI marker
                dataHeader.listRecords.append(getDataRecord(0, 2, "SOI", VT_UINT16, DRF_UNKNOWN, ENDIAN_BIG));
                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.dhMode = XBinary::DHMODE_TABLE;
                    _dataHeadersOptions.nID = STRUCTID_CHUNK;
                    _dataHeadersOptions.nLocation = dataHeadersOptions.nLocation + 2;
                    _dataHeadersOptions.locType = dataHeadersOptions.locType;
                    _dataHeadersOptions.nSize = getSize() - 2;
                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_CHUNK) {
                // Represent chunks as a table region starting after SOI
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJpeg::structIDToString(dataHeadersOptions.nID));
                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XJpeg::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    QList<FPART> listResult;

    qint64 nTotal = getSize();
    qint64 nMaxOffset = 0;

    if (nFileParts & FILEPART_SIGNATURE) {
        FPART rec = {};
        rec.filePart = FILEPART_SIGNATURE;
        rec.nFileOffset = 0;
        rec.nFileSize = 2;
        rec.nVirtualAddress = -1;
        rec.sName = tr("Signature");
        listResult.append(rec);
    }

    QList<CHUNK> chunks = getChunks(pPdStruct);
    for (int i = 0; i < chunks.size(); ++i) {
        const CHUNK &ch = chunks.at(i);
        if (ch.bEntropyCodedData) {
            if (nFileParts & FILEPART_REGION) {
                FPART rec = {};
                rec.filePart = FILEPART_REGION;
                rec.nFileOffset = ch.nDataOffset;
                rec.nFileSize = ch.nDataSize;
                rec.nVirtualAddress = -1;
                rec.sName = tr("Data");
                listResult.append(rec);
            }
        } else {
            if (nFileParts & FILEPART_OBJECT) {
                FPART rec = {};
                rec.filePart = FILEPART_OBJECT;
                rec.nFileOffset = ch.nDataOffset;
                rec.nFileSize = ch.nDataSize;
                rec.nVirtualAddress = -1;
                rec.sName = valueToHex(ch.nId);
                listResult.append(rec);
            }
        }

        nMaxOffset = qMax(nMaxOffset, ch.nDataOffset + ch.nDataSize);
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

XJpeg::CHUNK XJpeg::_readChunk(qint64 nOffset)
{
    CHUNK result = {};

    quint8 nBegin = read_uint8(nOffset);

    if (nBegin == 0xFF) {
        result.bValid = true;
        result.nId = read_uint8(nOffset + 1);

        result.nDataOffset = nOffset;

        if ((result.nId == 0xD8) || (result.nId == 0xD9)) {
            result.nDataSize = 2;
        } else if (result.nId == 0xDD) {
            result.nDataSize = 2 + 4;
        } else if ((result.nId >= 0xD0) && (result.nId <= 0xD7)) {
            result.nDataSize = 2;
        } else if (result.nId != 0x00) {
            result.nDataSize = 2 + read_uint16(nOffset + 2, true);
        } else {
            result.bValid = false;
        }
    }

    return result;
}

QList<QString> XJpeg::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("FFD8FF");

    return listResult;
}

XBinary *XJpeg::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XJpeg(pDevice);
}

