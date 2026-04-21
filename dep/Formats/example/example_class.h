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
#ifndef EXAMPLE_CLASS_H
#define EXAMPLE_CLASS_H

#include <QObject>
#include <xbinary.h>

class EXAMPLE_CLASS : public XBinary {
    Q_OBJECT
public:
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_ORIGINALHEADERNAME,
        STRUCTID_DATA_STRUCT1,
        STRUCTID_DATA_STRUCT2,
        // Add more structure IDs as needed
    };

    struct ORIGINALHEADERNAME {
        qint64 field1;
        qint32 field2;
    };

    struct DATA_STRUCT1 {
        qint32 fieldA;
        qint32 fieldB;
    };

    struct DATA_STRUCT2 {
        qint8 nFieldX;
        qint32 nFieldY;
        qint64 nFieldZ;
    };

    // Add format data structs

    EXAMPLE_CLASS(QIODevice *pDevice = nullptr);
    ~EXAMPLE_CLASS();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType();
    virtual MODE getMode();
    virtual QString getMIMEString();
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);
    virtual ENDIAN getEndian();
    virtual QString getArch();
    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);
    virtual bool isSigned();
    virtual OSNAME getOsName();
    virtual QString getOsVersion();
    virtual QString getVersion();
    virtual QString getInfo();
    virtual bool isEncrypted();
    virtual QList<MAPMODE> getMapModesList();
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<HREGION> getHData(PDSTRUCT *pPdStruct = nullptr);

    virtual QString structIDToString(quint32 nID);
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);
    virtual qint32 readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<QVariant> *pListValues,
                                PDSTRUCT *pPdStruct);

    ORIGINALHEADERNAME _read_ORIGINALHEADERNAME(qint64 nOffset);
    DATA_STRUCT1 _read_DATA_STRUCT1(qint64 nOffset);
    DATA_STRUCT2 _read_DATA_STRUCT2(qint64 nOffset);
};

#endif  // EXAMPLE_CLASS_H
