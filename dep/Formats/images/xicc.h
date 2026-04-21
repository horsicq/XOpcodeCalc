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
#ifndef XICC_H
#define XICC_H

#include "xbinary.h"

class XICC : public XBinary {
    Q_OBJECT

public:
    struct TAG {
        bool bValid;
        quint32 nSignature;
        quint32 nOffset;
        quint32 nSize;
        QString sTagName;
    };

    struct HEADER {
        quint32 nProfileSize;
        quint32 nCMMType;
        quint32 nVersion;
        quint32 nDeviceClass;
        quint32 nDataColorSpace;
        quint32 nPCS;
        quint8 nDate[12];
        quint32 nPlatform;
        quint32 nFlags;
        quint32 nDeviceManufacturer;
        quint32 nDeviceModel;
        quint64 nDeviceAttributes;
        quint32 nRenderingIntent;
        quint32 nIlluminantX;
        quint32 nIlluminantY;
        quint32 nIlluminantZ;
        quint32 nCreator;
        quint8 nReserved[44];
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_HEADER,  // Header
        STRUCTID_TAG,     // Tag
        STRUCTID_REGION,  // Region
    };

    explicit XICC(QIODevice *pDevice = nullptr);
    ~XICC();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual FT getFileType() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct) override;
    virtual QString getVersion() override;

    HEADER getHeader();
    QList<TAG> getTags(PDSTRUCT *pPdStruct = nullptr);
    static QList<TAG> _getTagsBySignature(QList<TAG> *pListTags, quint32 nSignature);
    QString getTagName(quint32 nSignature);

    QString getDescription(QList<TAG> *pListTags, PDSTRUCT *pPdStruct = nullptr);
    QString getDescription();
    QString getCopyright(QList<TAG> *pListTags, PDSTRUCT *pPdStruct = nullptr);
    QString getCopyright();
    QString getDeviceManufacturer();
    QString getDeviceModel();
    QString getColorSpace();
    QString getDeviceClass();

    virtual QString getMIMEString() override;

    virtual QString structIDToString(quint32 nID) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

    // virtual qint32 readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
    //                             void *pUserData, PDSTRUCT *pPdStruct);

private:
    TAG _readTag(qint64 nOffset);
    QString _readTextType(qint64 nOffset);
    QString _readMultiLocalizedUnicodeType(qint64 nOffset);
    QString _fourCCToString(quint32 nValue);
};

#endif  // XICC_H

