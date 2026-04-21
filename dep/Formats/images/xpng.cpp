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
#include "xpng.h"
#include <QBuffer>
#include <zlib.h>

XBinary::XCONVERT _TABLE_XPNG_STRUCTID[] = {
    {XPNG::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XPNG::STRUCTID_SIGNATURE, "Signature", QObject::tr("Signature")},
    {XPNG::STRUCTID_CHUNK, "Chunk", QObject::tr("Chunk")},
    {XPNG::STRUCTID_IHDR, "IHDR", QString("IHDR")},
    {XPNG::STRUCTID_PLTE, "PLTE", QString("PLTE")},
    {XPNG::STRUCTID_IDAT, "IDAT", QString("IDAT")},
    {XPNG::STRUCTID_IEND, "IEND", QString("IEND")},
    {XPNG::STRUCTID_cHRM, "cHRM", QString("cHRM")},
    {XPNG::STRUCTID_gAMA, "gAMA", QString("gAMA")},
    {XPNG::STRUCTID_iCCP, "iCCP", QString("iCCP")},
    {XPNG::STRUCTID_sBIT, "sBIT", QString("sBIT")},
    {XPNG::STRUCTID_sRGB, "sRGB", QString("sRGB")},
    {XPNG::STRUCTID_tEXt, "tEXt", QString("tEXt")},
    {XPNG::STRUCTID_zTXt, "zTXt", QString("zTXt")},
    {XPNG::STRUCTID_iTXt, "iTXt", QString("iTXt")},
    {XPNG::STRUCTID_bKGD, "bKGD", QString("bKGD")},
    {XPNG::STRUCTID_hIST, "hIST", QString("hIST")},
    {XPNG::STRUCTID_tRNS, "tRNS", QString("tRNS")},
    {XPNG::STRUCTID_pHYs, "pHYs", QString("pHYs")},
    {XPNG::STRUCTID_sPLT, "sPLT", QString("sPLT")},
    {XPNG::STRUCTID_tIME, "tIME", QString("tIME")},
};

XPNG::XPNG(QIODevice *pDevice) : XBinary(pDevice)
{
}

XPNG::~XPNG()
{
}

bool XPNG::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() >= 20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        bIsValid = compareSignature(&memoryMap, "89'PNG\r\n'1A0A", 0, pPdStruct);
    }

    return bIsValid;
}

bool XPNG::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XPNG xpng(pDevice);

    return xpng.isValid();
}

XBinary::FT XPNG::getFileType()
{
    return FT_PNG;
}

// QString XPNG::getFileFormatString()
// {
//     QString sResult;

//     QString sResolution = QString("%1x%2").arg(read_uint32(16, true)).arg(read_uint32(20, true));

//     sResult = QString("PNG(%1)").arg(sResolution);

//     return sResult;
// }

QString XPNG::getFileFormatExt()
{
    return "png";
}

QString XPNG::getFileFormatExtsString()
{
    return "PNG (*.png)";
}

qint64 XPNG::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;
    qint64 nOffset = 8;

    while (isPdStructNotCanceled(pPdStruct)) {
        CHUNK chunk = _readChunk(nOffset);

        nOffset += (12 + chunk.nDataSize);

        if (chunk.sName == "IEND") {
            nResult = nOffset;

            break;
        }

        if (chunk.nCRC == 0) {  // mb TODO more checks mb ANSI names
            break;
        }
    }

    return nResult;
}

QString XPNG::getMIMEString()
{
    return "image/png";
}

QString XPNG::getInfo(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QString sResult;

    if (isValid()) {
        IHDR ihdr = getIHDR();

        if (ihdr.nWidth && ihdr.nHeight) {
            QString sSchema;

            switch (ihdr.nColorType) {
                case COLOR_TYPE_GRAYSCALE: sSchema = QString("Grayscale"); break;
                case COLOR_TYPE_RGB: sSchema = QString("RGB"); break;
                case COLOR_TYPE_PALETTE: sSchema = QString("Palette"); break;
                case COLOR_TYPE_GRAYSCALE_ALPHA: sSchema = QString("Grayscale+Alpha"); break;
                case COLOR_TYPE_RGBA: sSchema = QString("RGBA"); break;
                default: sSchema = QString("Unknown(%1)").arg(ihdr.nColorType); break;
            }

            sResult = QString("%1x%2, %3 bits, %4").arg(ihdr.nWidth).arg(ihdr.nHeight).arg(ihdr.nBitDepth).arg(sSchema);

            // Append pHYs info if available
            XPNG::pHYs phys = getpHYs();
            if (phys.nPixelsPerUnitX || phys.nPixelsPerUnitY) {
                QString sUnit = (phys.nUnitSpecifier == 1) ? QString("meter") : QString("unknown");
                sResult += QString(", pHYs: %1x%2 %3").arg(phys.nPixelsPerUnitX).arg(phys.nPixelsPerUnitY).arg(sUnit);
            }

            // Append bKGD info if available
            XPNG::bKGD bkgd = getbKGD();
            if (bkgd.nType == 1) {
                sResult += QString(", bKGD: gray=%1").arg(bkgd.nGray);
            } else if (bkgd.nType == 2) {
                sResult += QString(", bKGD: rgb=(%1,%2,%3)").arg(bkgd.nRed).arg(bkgd.nGreen).arg(bkgd.nBlue);
            } else if (bkgd.nType == 3) {
                sResult += QString(", bKGD: paletteIndex=%1").arg((quint32)bkgd.nPaletteIndex);
            }
        }
    }

    return sResult;
}

XBinary::ENDIAN XPNG::getEndian()
{
    return ENDIAN_BIG;  // PNG is always big-endian
}

XBinary::_MEMORY_MAP XPNG::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_REGIONS;  // Default mode
    }

    if (mapMode == MAPMODE_REGIONS) {
        result = _getMemoryMap(FILEPART_HEADER | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
    }

    return result;
}

XPNG::CHUNK XPNG::_readChunk(qint64 nOffset)
{
    CHUNK result = {};

    result.nDataSize = read_uint32(nOffset, true);
    result.nDataOffset = nOffset + 8;
    result.sName = read_ansiString(nOffset + 4, 4);
    result.nCRC = read_uint32(nOffset + 8 + result.nDataSize, true);

    return result;
}

bool XPNG::createPNG(QIODevice *pDevice, quint32 nWidth, quint32 nHeight, const QByteArray &baImageData, COLOR_TYPE colorType, quint8 nBitDepth)
{
    if (!pDevice || !pDevice->isWritable()) {
        return false;
    }

    // Create IHDR structure
    IHDR ihdr = {};
    ihdr.nWidth = nWidth;
    ihdr.nHeight = nHeight;
    ihdr.nBitDepth = nBitDepth;
    ihdr.nColorType = (quint8)colorType;
    ihdr.nCompression = 0;  // Always 0 (deflate)
    ihdr.nFilter = 0;       // Always 0 (adaptive filtering)
    ihdr.nInterlace = 0;    // 0 = no interlace

    QByteArray _baImageData = _convertImageData(baImageData.data(), baImageData.size(), nWidth, nHeight, colorType, nBitDepth);

    // Write PNG signature
    const char pngSignature[8] = {'\x89', 'P', 'N', 'G', '\r', '\n', '\x1a', '\n'};
    if (pDevice->write(pngSignature, 8) != 8) {
        return false;
    }

    // Create IHDR chunk data using the struct
    QByteArray ihdrData;

    // Convert to big-endian and append
    quint32 nWidthBE = qToBigEndian(ihdr.nWidth);
    quint32 nHeightBE = qToBigEndian(ihdr.nHeight);

    ihdrData.append((char *)&nWidthBE, 4);   // Width (big-endian)
    ihdrData.append((char *)&nHeightBE, 4);  // Height (big-endian)
    ihdrData.append(ihdr.nBitDepth);         // Bit depth
    ihdrData.append(ihdr.nColorType);        // Color type
    ihdrData.append(ihdr.nCompression);      // Compression method
    ihdrData.append(ihdr.nFilter);           // Filter method
    ihdrData.append(ihdr.nInterlace);        // Interlace method

    // Write IHDR chunk
    if (!_writeChunk(pDevice, "IHDR", ihdrData)) {
        return false;
    }

    // Compress image data
    QByteArray compressedData = _compressData(_baImageData);
    if (compressedData.isEmpty()) {
        return false;
    }

    // Write IDAT chunk
    if (!_writeChunk(pDevice, "IDAT", compressedData)) {
        return false;
    }

    // Write IEND chunk
    if (!_writeChunk(pDevice, "IEND", QByteArray())) {
        return false;
    }

    return true;
}

bool XPNG::createPNGIndexed(QIODevice *pDevice, quint32 nWidth, quint32 nHeight, const QByteArray &baImageData, const QByteArray &baPalette, quint8 nBitDepth)
{
    if (!pDevice || !pDevice->isWritable()) {
        return false;
    }

    if (baPalette.isEmpty() || ((baPalette.size() % 3) != 0)) {
        return false;
    }

    // Create IHDR structure
    IHDR ihdr = {};
    ihdr.nWidth = nWidth;
    ihdr.nHeight = nHeight;
    ihdr.nBitDepth = nBitDepth;
    ihdr.nColorType = (quint8)COLOR_TYPE_PALETTE;
    ihdr.nCompression = 0;
    ihdr.nFilter = 0;
    ihdr.nInterlace = 0;

    QByteArray _baImageData = _convertImageData(baImageData.data(), baImageData.size(), nWidth, nHeight, COLOR_TYPE_PALETTE, nBitDepth);

    // Write PNG signature
    const char pngSignature[8] = {'\x89', 'P', 'N', 'G', '\r', '\n', '\x1a', '\n'};

    if (pDevice->write(pngSignature, 8) != 8) {
        return false;
    }

    // Create IHDR chunk data
    QByteArray ihdrData;

    quint32 nWidthBE = qToBigEndian(ihdr.nWidth);
    quint32 nHeightBE = qToBigEndian(ihdr.nHeight);

    ihdrData.append((char *)&nWidthBE, 4);
    ihdrData.append((char *)&nHeightBE, 4);
    ihdrData.append(ihdr.nBitDepth);
    ihdrData.append(ihdr.nColorType);
    ihdrData.append(ihdr.nCompression);
    ihdrData.append(ihdr.nFilter);
    ihdrData.append(ihdr.nInterlace);

    if (!_writeChunk(pDevice, "IHDR", ihdrData)) {
        return false;
    }

    // Write PLTE chunk (palette data in RGB format, 3 bytes per entry)
    if (!_writeChunk(pDevice, "PLTE", baPalette)) {
        return false;
    }

    // Compress image data
    QByteArray compressedData = _compressData(_baImageData);

    if (compressedData.isEmpty()) {
        return false;
    }

    // Write IDAT chunk
    if (!_writeChunk(pDevice, "IDAT", compressedData)) {
        return false;
    }

    // Write IEND chunk
    if (!_writeChunk(pDevice, "IEND", QByteArray())) {
        return false;
    }

    return true;
}

bool XPNG::_writeChunk(QIODevice *pDevice, const QString &sChunkType, const QByteArray &data)
{
    if (!pDevice || sChunkType.length() != 4) {
        return false;
    }

    // Write data length (big-endian)
    quint32 nDataLength = qToBigEndian((quint32)data.size());
    if (pDevice->write((char *)&nDataLength, 4) != 4) {
        return false;
    }

    // Write chunk type
    QByteArray chunkTypeBytes = sChunkType.toLatin1();
    if (pDevice->write(chunkTypeBytes) != 4) {
        return false;
    }

    // Write data
    if (!data.isEmpty() && pDevice->write(data) != data.size()) {
        return false;
    }

    // Calculate and write CRC
    QByteArray crcData = chunkTypeBytes + data;
    quint32 nCRC = qToBigEndian(_getCRC32(crcData, 0xFFFFFFFF, XBinary::_getCRC32Table_EDB88320()));
    if (pDevice->write((char *)&nCRC, 4) != 4) {
        return false;
    }

    return true;
}

QByteArray XPNG::_compressData(const QByteArray &data)
{
    QByteArray result;
#ifdef USE_ARCHIVE
    // Use XDeflateDecoder for compression

    if (data.isEmpty()) {
        return result;
    }

    // Create input and output devices
    QBuffer inputBuffer;
    inputBuffer.setData(data);
    inputBuffer.open(QIODevice::ReadOnly);

    QBuffer outputBuffer;
    outputBuffer.open(QIODevice::WriteOnly);

    // Setup compression state
    XBinary::DATAPROCESS_STATE compressState = {};
    compressState.pDeviceInput = &inputBuffer;
    compressState.pDeviceOutput = &outputBuffer;
    compressState.nInputOffset = 0;
    compressState.nInputLimit = data.size();
    compressState.nProcessedOffset = 0;
    compressState.nProcessedLimit = -1;
    compressState.bReadError = false;
    compressState.bWriteError = false;
    compressState.nCountInput = 0;
    compressState.nCountOutput = 0;

    // Compress using XDeflateDecoder (actually compresses despite the class name)
    bool success = XDeflateDecoder::compress_zlib(&compressState);

    inputBuffer.close();
    outputBuffer.close();

    if (success && !compressState.bReadError && !compressState.bWriteError) {
        result = outputBuffer.data();
    }
#endif
    return result;
}

QByteArray XPNG::_convertImageData(const char *pData, qint32 nDataSize, quint32 nWidth, quint32 nHeight, COLOR_TYPE colorType, quint8 nBitDepth)
{
    QByteArray baResult;

    // Calculate samples per pixel based on color type
    qint32 nSamplesPerPixel = 0;
    switch (colorType) {
        case COLOR_TYPE_GRAYSCALE: nSamplesPerPixel = 1; break;
        case COLOR_TYPE_RGB: nSamplesPerPixel = 3; break;
        case COLOR_TYPE_PALETTE: nSamplesPerPixel = 1; break;
        case COLOR_TYPE_GRAYSCALE_ALPHA: nSamplesPerPixel = 2; break;
        case COLOR_TYPE_RGBA: nSamplesPerPixel = 4; break;
    }

    if (nSamplesPerPixel > 0) {
        qint32 nBitsPerRow = nWidth * nSamplesPerPixel * nBitDepth;
        qint32 nBytesPerRow = (nBitsPerRow + 7) / 8;
        qint32 nFilteredRowSize = nBytesPerRow + 1;  // +1 for filter byte
        qint32 nTotalSize = nFilteredRowSize * nHeight;

        baResult.resize(nTotalSize);
        baResult.fill(0);  // Initialize with zeros (filter byte 0 = None)

        for (quint32 y = 0; y < nHeight; y++) {
            qint32 nOutputRowOffset = y * nFilteredRowSize;
            qint32 nInputRowOffset = y * nBytesPerRow;

            // Filter byte is already 0 (None) from fill
            qint32 nCopySize = qMin(nBytesPerRow, nDataSize - nInputRowOffset);

            if (nCopySize > 0) {
                memcpy(baResult.data() + nOutputRowOffset + 1, pData + nInputRowOffset, nCopySize);
            }
        }
    }

    return baResult;
}

XPNG::IHDR XPNG::getIHDR()
{
    IHDR result = {};

    if (isValid()) {
        // IHDR is always the first chunk after the PNG signature (at offset 8)
        CHUNK ihdrChunk = _readChunk(8);

        if (ihdrChunk.sName == "IHDR" && ihdrChunk.nDataSize == 13) {
            result.nWidth = read_uint32(ihdrChunk.nDataOffset, true);       // Big-endian
            result.nHeight = read_uint32(ihdrChunk.nDataOffset + 4, true);  // Big-endian
            result.nBitDepth = read_uint8(ihdrChunk.nDataOffset + 8);
            result.nColorType = read_uint8(ihdrChunk.nDataOffset + 9);
            result.nCompression = read_uint8(ihdrChunk.nDataOffset + 10);
            result.nFilter = read_uint8(ihdrChunk.nDataOffset + 11);
            result.nInterlace = read_uint8(ihdrChunk.nDataOffset + 12);
        }
    }

    return result;
}

XPNG::pHYs XPNG::getpHYs()
{
    XPNG::pHYs result = {};

    if (!isValid()) {
        return result;
    }

    qint64 nOffset = 8;  // After signature
    qint64 nTotalSize = getSize();

    while (nOffset + 12 <= nTotalSize) {
        CHUNK chunk = _readChunk(nOffset);

        if (chunk.sName == "pHYs") {
            if (chunk.nDataSize >= 9) {
                result.nPixelsPerUnitX = read_uint32(chunk.nDataOffset + 0, true);
                result.nPixelsPerUnitY = read_uint32(chunk.nDataOffset + 4, true);
                result.nUnitSpecifier = read_uint8(chunk.nDataOffset + 8);
            }
            break;
        }

        if (chunk.sName == "IEND") {
            break;
        }

        // Advance to next chunk: length(4) + type(4) + data + crc(4)
        nOffset += (12 + chunk.nDataSize);
    }

    return result;
}

XPNG::bKGD XPNG::getbKGD()
{
    XPNG::bKGD result = {};

    if (!isValid()) {
        return result;
    }

    // We may use IHDR color type to infer expected layout, but chunk itself defines size
    IHDR ihdr = getIHDR();

    qint64 nOffset = 8;  // After signature
    qint64 nTotalSize = getSize();

    while (nOffset + 12 <= nTotalSize) {
        CHUNK chunk = _readChunk(nOffset);

        if (chunk.sName == "bKGD") {
            quint32 nLen = (quint32)chunk.nDataSize;
            if (nLen == 1) {
                result.nPaletteIndex = read_uint8(chunk.nDataOffset + 0);
                result.nType = 3;  // indexed
            } else if (nLen == 2) {
                result.nGray = read_uint16(chunk.nDataOffset + 0, true);
                result.nType = 1;  // grayscale
            } else if (nLen == 6) {
                result.nRed = read_uint16(chunk.nDataOffset + 0, true);
                result.nGreen = read_uint16(chunk.nDataOffset + 2, true);
                result.nBlue = read_uint16(chunk.nDataOffset + 4, true);
                result.nType = 2;  // truecolor
            } else {
                result.nType = 0;  // unknown or unsupported
            }
            break;
        }

        if (chunk.sName == "IEND") {
            break;
        }

        nOffset += (12 + chunk.nDataSize);
    }

    // Validate against color type if present
    if (result.nType != 0) {
        // If mismatch between IHDR color type and bKGD content, still return parsed values; caller can decide.
        Q_UNUSED(ihdr)
    }

    return result;
}

QString XPNG::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XPNG_STRUCTID, sizeof(_TABLE_XPNG_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XPNG::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        _dataHeadersOptions.nID = STRUCTID_SIGNATURE;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_SIGNATURE) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPNG::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 8;

                dataHeader.listRecords.append(getDataRecord(0, 8, "Signature", VT_BYTE_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    qint64 nCurrentOffset = 8;  // Start after the PNG signature
                    qint64 nTotalSize = getSize();
                    qint32 nNumberOfChunks = 0;

                    while (XBinary::isPdStructNotCanceled(pPdStruct)) {
                        qint64 nDataSize = read_uint32(nCurrentOffset, true);
                        QString sTag = read_ansiString(nCurrentOffset + 4, 4);

                        if (nCurrentOffset + nDataSize + 12 > nTotalSize) {
                            break;  // Prevent reading beyond the file size
                        }

                        nNumberOfChunks++;

                        // End Tag
                        if (sTag == "IEND") {
                            break;
                        }

                        nCurrentOffset += (12 + nDataSize);
                    }

                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;

                    _dataHeadersOptions.dhMode = XBinary::DHMODE_TABLE;
                    _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;
                    _dataHeadersOptions.nID = STRUCTID_CHUNK;
                    _dataHeadersOptions.locType = LT_OFFSET;
                    _dataHeadersOptions.nLocation = 8;  // Start after the PNG signature;
                    _dataHeadersOptions.nCount = nNumberOfChunks;
                    _dataHeadersOptions.nSize = nCurrentOffset - 8;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_CHUNK) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPNG::structIDToString(dataHeadersOptions.nID));

                quint32 nDataSize = read_uint32(nStartOffset, true);

                dataHeader.nSize = 12 + nDataSize;

                dataHeader.listRecords.append(getDataRecord(0, 4, "Length", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(4, 4, "Type", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(8 + nDataSize, 4, "CRC", XBinary::VT_UINT32, 0, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    qint64 nCurrentOffset = nStartOffset;
                    qint64 nLocation = dataHeadersOptions.nLocation;

                    for (int i = 0; i < dataHeader.nCount; i++) {
                        qint64 nDataSize = read_uint32(nCurrentOffset, true);
                        QString sTag = read_ansiString(nCurrentOffset + 4, 4);

                        if (sTag == "IHDR") {
                            DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;

                            _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
                            _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;
                            _dataHeadersOptions.nID = STRUCTID_IHDR;
                            _dataHeadersOptions.locType = LT_OFFSET;
                            _dataHeadersOptions.nLocation = nLocation;
                            _dataHeadersOptions.nSize = nDataSize + 12;

                            listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                        } else if (sTag == "pHYs") {
                            DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;

                            _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
                            _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;
                            _dataHeadersOptions.nID = STRUCTID_pHYs;
                            _dataHeadersOptions.locType = LT_OFFSET;
                            _dataHeadersOptions.nLocation = nLocation;
                            _dataHeadersOptions.nSize = nDataSize + 12;

                            listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                        } else if (sTag == "bKGD") {
                            DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;

                            _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
                            _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;
                            _dataHeadersOptions.nID = STRUCTID_bKGD;
                            _dataHeadersOptions.locType = LT_OFFSET;
                            _dataHeadersOptions.nLocation = nLocation;
                            _dataHeadersOptions.nSize = nDataSize + 12;

                            listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                        }

                        nCurrentOffset += (12 + nDataSize);
                        nLocation += (12 + nDataSize);
                    }
                }
            } else if (dataHeadersOptions.nID == STRUCTID_IHDR) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPNG::structIDToString(dataHeadersOptions.nID));

                dataHeader.nSize = 25;  // IHDR size is always 25 bytes
                dataHeader.listRecords.append(getDataRecord(0, 4, "Length", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(4, 4, "Type", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(8, 4, "Width", XBinary::VT_UINT32, DRF_COUNT, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(12, 4, "Height", XBinary::VT_UINT32, DRF_COUNT, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(16, 1, "Bit Depth", XBinary::VT_UINT8, DRF_COUNT, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(17, 1, "Color Type", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(18, 1, "Compression", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(19, 1, "Filter", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(20, 1, "Interlace", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(21, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);
            } else if (dataHeadersOptions.nID == STRUCTID_pHYs) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPNG::structIDToString(dataHeadersOptions.nID));

                dataHeader.nSize = 21;  // pHYs size is always 21 bytes (length=9)
                dataHeader.listRecords.append(getDataRecord(0, 4, "Length", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(4, 4, "Type", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(8, 4, "Pixels per Unit X", XBinary::VT_UINT32, DRF_COUNT, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(12, 4, "Pixels per Unit Y", XBinary::VT_UINT32, DRF_COUNT, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(16, 1, "Unit Specifier", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(17, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);
            } else if (dataHeadersOptions.nID == STRUCTID_bKGD) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPNG::structIDToString(dataHeadersOptions.nID));

                // Data part of bKGD can be 1 (indexed), 2 (grayscale), or 6 (truecolor) bytes
                quint32 nLen = read_uint32(nStartOffset, true);
                dataHeader.nSize = 12 + nLen;  // length + type + data + crc

                dataHeader.listRecords.append(getDataRecord(0, 4, "Length", XBinary::VT_UINT32, DRF_SIZE, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(4, 4, "Type", XBinary::VT_CHAR_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                if (nLen == 1) {
                    dataHeader.listRecords.append(getDataRecord(8, 1, "Palette Index", XBinary::VT_UINT8, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(9, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                } else if (nLen == 2) {
                    dataHeader.listRecords.append(getDataRecord(8, 2, "Gray", XBinary::VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(10, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                } else if (nLen == 6) {
                    dataHeader.listRecords.append(getDataRecord(8, 2, "Red", XBinary::VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(10, 2, "Green", XBinary::VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(12, 2, "Blue", XBinary::VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(14, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                } else {
                    // Unknown size, just show raw data
                    dataHeader.listRecords.append(getDataRecord(8, nLen, "Data", XBinary::VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                    dataHeader.listRecords.append(getDataRecord(8 + nLen, 4, "CRC", XBinary::VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);
            } else {
                // mb TODO
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XPNG::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<FPART> listResult;

    if (nFileParts & FILEPART_SIGNATURE) {
        FPART record = {};

        record.filePart = FILEPART_SIGNATURE;
        record.nFileOffset = 0;
        record.nFileSize = 8;
        record.nVirtualAddress = -1;
        record.sName = tr("Signature");

        listResult.append(record);
    }
    qint64 nTotalSize = getSize();

    qint64 nCurrentOffset = 8;  // Start after the PNG signature

    while (XBinary::isPdStructNotCanceled(pPdStruct)) {
        qint64 nDataSize = read_uint32(nCurrentOffset, true);
        QString sTag = read_ansiString(nCurrentOffset + 4, 4);

        if (nCurrentOffset + nDataSize + 12 > nTotalSize) {
            break;  // Prevent reading beyond the file size
        }

        if (nFileParts & FILEPART_REGION) {
            FPART record = {};

            record.filePart = FILEPART_REGION;
            record.nFileOffset = nCurrentOffset;
            record.nFileSize = 12 + nDataSize;
            record.nVirtualAddress = -1;
            record.sName = sTag;  // mb TODO

            listResult.append(record);
        }

        nCurrentOffset += (12 + nDataSize);

        // End Tag
        if (sTag == "IEND") {
            break;
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        if (nCurrentOffset < nTotalSize) {
            FPART record = {};

            record.filePart = FILEPART_OVERLAY;
            record.nFileOffset = nCurrentOffset;
            record.nFileSize = nTotalSize - nCurrentOffset;
            record.nVirtualAddress = -1;
            record.sName = tr("Overlay");

            listResult.append(record);
        }
    }

    return listResult;
}

qint32 XPNG::readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
                          void *pUserData, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(locType)
    Q_UNUSED(nLocation)
    Q_UNUSED(dataRecordsOptions)
    Q_UNUSED(pUserData)

    qint32 nResult = 0;

    if (dataRecordsOptions.dataHeaderFirst.dsID.nID == STRUCTID_CHUNK) {
        nResult = XBinary::readTableRow(nRow, locType, nLocation, dataRecordsOptions, pListDataRecords, pUserData, pPdStruct);

        qint64 nStartOffset = locationToOffset(dataRecordsOptions.pMemoryMap, locType, nLocation);

        quint32 nChunkLenght = read_uint32(nStartOffset, true);

        nResult = nChunkLenght + 12;
    } else {
        nResult = XBinary::readTableRow(nRow, locType, nLocation, dataRecordsOptions, pListDataRecords, pUserData, pPdStruct);
    }

    return nResult;
}

QList<QString> XPNG::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("89504E470D0A1A0A");

    return listResult;
}

XBinary *XPNG::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XPNG(pDevice);
}

