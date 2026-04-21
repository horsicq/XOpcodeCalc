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
#ifndef XTIFF_H
#define XTIFF_H

#include "xbinary.h"

class XTiff : public XBinary {
    Q_OBJECT

#pragma pack(push)
#pragma pack(1)
    struct IFD_ENTRY {
        quint16 nTag;
        quint16 nType;
        quint32 nCount;
        quint32 nOffset;
    };
#pragma pack(pop)

public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_SIGNATURE,
        STRUCTID_IFD_TABLE,
        STRUCTID_IFD_ENTRY
    };
    struct CHUNK {
        quint32 nTag;
        qint64 nOffset;
        qint64 nSize;
    };

    explicit XTiff(QIODevice *pDevice = nullptr);
    ~XTiff();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual FT getFileType() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct) override;
    virtual ENDIAN getEndian() override;

    QList<CHUNK> getChunks(PDSTRUCT *pPdStruct = nullptr);
    static QList<CHUNK> _getChunksByTag(QList<CHUNK> *pListChunks, quint16 nTag);

    static QString getExifCameraName(QIODevice *pDevice, OFFSETSIZE osExif, QList<CHUNK> *pListChunks);

    static QList<XTiff::CHUNK> getExifChunks(QIODevice *pDevice, OFFSETSIZE osExif, PDSTRUCT *pPdStruct = nullptr);
    virtual QString getMIMEString() override;

    // Inspection
    virtual QString structIDToString(quint32 nID) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

private:
qint32 getBaseTypeSize(quint16 nType);

};

#endif  // XTIFF_H

