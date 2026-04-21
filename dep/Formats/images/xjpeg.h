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
#ifndef XJPEG_H
#define XJPEG_H

#include "xbinary.h"
#include "xtiff.h"

class XJpeg : public XBinary {
    Q_OBJECT

public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_SIGNATURE,
        STRUCTID_CHUNK
    };
    struct CHUNK {
        bool bValid;
        bool bEntropyCodedData;
        quint8 nId;
        qint64 nDataOffset;
        qint64 nDataSize;
    };

    explicit XJpeg(QIODevice *pDevice = nullptr);
    ~XJpeg();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType();
    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);
    virtual QString getVersion();

    QList<CHUNK> getChunks(PDSTRUCT *pPdStruct = nullptr);
    static QList<CHUNK> _getChunksById(QList<CHUNK> *pListChunks, quint8 nId);
    QString getComment(QList<CHUNK> *pListChunks, PDSTRUCT *pPdStruct = nullptr);
    QString getComment();

    QString getDqtMD5(QList<CHUNK> *pListChunks);
    QString getDqtMD5();

    bool isChunkPresent(QList<CHUNK> *pListChunks, quint8 nId);

    OFFSETSIZE getExif(QList<CHUNK> *pListChunks);
    bool isExifPresent(OFFSETSIZE osExif);

    virtual QString getMIMEString();

    // Inspection
    virtual QString structIDToString(quint32 nID);
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr);

private:
CHUNK _readChunk(qint64 nOffset);

};

#endif  // XJPEG_H

