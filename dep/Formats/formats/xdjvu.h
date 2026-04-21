// Copyright (c) 2017-2026 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef XDJVU_H
#define XDJVU_H

#include "xbinary.h"

class XDJVU : public XBinary {
    Q_OBJECT

public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum TYPE {
        TYPE_UNKNOWN = 0,
        TYPE_SINGLE_PAGE,
        TYPE_MULTI_PAGE,
        TYPE_MULTI_FILE,
        TYPE_THUMBNAILS,
        TYPE_SECURE
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_HEADER,
        STRUCTID_CHUNK
    };

    struct CHUNK_RECORD {
        QString sName;
        qint64 nOffset;
        quint32 nSize;
        qint64 nDataOffset;
        qint64 nDataSize;
        qint64 nHeaderSize;
    };

    struct INFO_RECORD {
        quint16 nWidth;
        quint16 nHeight;
        quint8 nMinorVersion;
        quint8 nMajorVersion;
        quint16 nDPI;
        quint8 nGamma;
        quint8 nFlags;
        quint8 nRotation;
        quint8 nReserved;
    };

    struct HEADER {
        QString sSignature;
        TYPE type;
        quint32 nSize;
        bool bIsValid;
        bool bIsSecure;
    };

    explicit XDJVU(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    virtual ~XDJVU();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType() override;
    virtual QString getMIMEString() override;
    virtual QString getArch() override;
    virtual MODE getMode() override;
    virtual ENDIAN getEndian() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct = nullptr) override;
    virtual QString getVersion() override;
    virtual QList<MAPMODE> getMapModesList() override;
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QString structIDToString(quint32 nID) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

    virtual HEADER getHeader();

    bool isSecure();
    quint32 getDocumentSize();

    INFO_RECORD getInfoRecord(qint64 nOffset, PDSTRUCT *pPdStruct = nullptr);

    QString getDocumentInfo(PDSTRUCT *pPdStruct = nullptr);

    static QMap<quint64, QString> getImageTypes();
    static QMap<quint64, QString> getImageTypesS();

    static const QString PREFIX_ImageType;

private:
    HEADER _getHeader();
    QList<CHUNK_RECORD> _getChunkRecords(PDSTRUCT *pPdStruct);
    INFO_RECORD _getInfoRecord(qint64 nOffset, PDSTRUCT *pPdStruct);
    bool _isChunkValid(const QString &sChunkName);

    HEADER m_header;

};

#endif  // XDJVU_H

