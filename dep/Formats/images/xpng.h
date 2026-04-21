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
#ifndef XPNG_H
#define XPNG_H

#include "xbinary.h"

#ifdef USE_ARCHIVE
#include "xdeflatedecoder.h"
#endif

class XPNG : public XBinary {
    Q_OBJECT

public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum COLOR_TYPE {
        COLOR_TYPE_GRAYSCALE = 0,        // Grayscale
        COLOR_TYPE_RGB = 2,              // RGB
        COLOR_TYPE_PALETTE = 3,          // Palette
        COLOR_TYPE_GRAYSCALE_ALPHA = 4,  // Grayscale with alpha
        COLOR_TYPE_RGBA = 6              // RGBA
    };

    struct CHUNK {
        QString sName;
        qint64 nDataOffset;
        qint64 nDataSize;
        quint32 nCRC;
    };

    struct IHDR {
        quint32 nWidth;       // Image width in pixels
        quint32 nHeight;      // Image height in pixels
        quint8 nBitDepth;     // Bits per sample (1, 2, 4, 8, 16)
        quint8 nColorType;    // Color type (0, 2, 3, 4, 6)
        quint8 nCompression;  // Compression method (0 = deflate)
        quint8 nFilter;       // Filter method (0 = adaptive filtering)
        quint8 nInterlace;    // Interlace method (0 = no interlace, 1 = Adam7)
    };

    // pHYs chunk: physical pixel dimensions
    struct pHYs {
        quint32 nPixelsPerUnitX;  // Pixels per unit, X axis
        quint32 nPixelsPerUnitY;  // Pixels per unit, Y axis
        quint8 nUnitSpecifier;    // Unit specifier (0 = unknown, 1 = meter)
    };

    // bKGD chunk: background color. Layout depends on color type
    struct bKGD {
        quint16 nGray;         // For grayscale images (length=2)
        quint16 nRed;          // For truecolor images (length=6)
        quint16 nGreen;        // For truecolor images (length=6)
        quint16 nBlue;         // For truecolor images (length=6)
        quint8 nPaletteIndex;  // For indexed-color images (length=1)
        quint8 nType;          // 0=unknown, 1=grayscale, 2=truecolor, 3=indexed
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_SIGNATURE,
        STRUCTID_CHUNK,
        STRUCTID_IHDR,
        STRUCTID_PLTE,
        STRUCTID_IDAT,
        STRUCTID_IEND,
        STRUCTID_cHRM,
        STRUCTID_gAMA,
        STRUCTID_iCCP,
        STRUCTID_sBIT,
        STRUCTID_sRGB,
        STRUCTID_tEXt,
        STRUCTID_zTXt,
        STRUCTID_iTXt,
        STRUCTID_bKGD,
        STRUCTID_hIST,
        STRUCTID_tRNS,
        STRUCTID_pHYs,
        STRUCTID_sPLT,
        STRUCTID_tIME
    };

public:
    explicit XPNG(QIODevice *pDevice = nullptr);
    ~XPNG();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct) override;
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QString getMIMEString() override;
    virtual ENDIAN getEndian() override;
    virtual QString getInfo(PDSTRUCT *pPdStruct = nullptr) override;

    static bool createPNG(QIODevice *pDevice, quint32 nWidth, quint32 nHeight, const QByteArray &baImageData, COLOR_TYPE colorType, quint8 nBitDepth = 8);
    static bool createPNGIndexed(QIODevice *pDevice, quint32 nWidth, quint32 nHeight, const QByteArray &baImageData, const QByteArray &baPalette, quint8 nBitDepth = 8);

    IHDR getIHDR();
    pHYs getpHYs();
    bKGD getbKGD();

    virtual QString structIDToString(quint32 nID) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

    virtual qint32 readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
                                void *pUserData, PDSTRUCT *pPdStruct) override;

private:
CHUNK _readChunk(qint64 nOffset);
    static bool _writeChunk(QIODevice *pDevice, const QString &sChunkType, const QByteArray &data);
    static QByteArray _compressData(const QByteArray &data);
    static QByteArray _convertImageData(const char *pData, qint32 nDataSize, quint32 nWidth, quint32 nHeight, COLOR_TYPE colorType, quint8 nBitDepth);

};

#endif  // XPNG_H

