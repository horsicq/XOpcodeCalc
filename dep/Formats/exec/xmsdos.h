/* Copyright (c) 2017-2026 hors<horsicq@gmail.com>
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
#ifndef XMSDOS_H
#define XMSDOS_H

#include "xbinary.h"
#include "xmsdos_def.h"

class XMSDOS : public XBinary {
    Q_OBJECT

public:
    struct MS_RICH_RECORD  // For PE and LE
    {
        quint16 nId;
        quint16 nVersion;
        quint32 nCount;
    };

    enum TYPE {
        TYPE_UNKNOWN = 0,
        TYPE_EXE
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_IMAGE_DOS_HEADER,
        STRUCTID_IMAGE_DOS_HEADEREX,
    };

    explicit XMSDOS(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1, PDSTRUCT *pPdStruct = nullptr);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

    virtual bool isExecutable();

    quint16 get_magic();
    qint32 get_lfanew();

    qint64 getDosHeaderOffset();
    qint64 getDosHeaderSize();
    qint64 getDosHeaderExOffset();
    qint64 getDosHeaderExSize();

    XMSDOS_DEF::IMAGE_DOS_HEADER getDosHeader();
    XMSDOS_DEF::IMAGE_DOS_HEADEREX getDosHeaderEx();
    void setDosHeader(XMSDOS_DEF::IMAGE_DOS_HEADER *pDosHeader);
    void setDosHeaderEx(XMSDOS_DEF::IMAGE_DOS_HEADEREX *pDosHeaderEx);

    XMSDOS_DEF::IMAGE_DOS_HEADEREX _read_IMAGE_DOS_HEADEREX(qint64 nOffset);

    void set_e_magic(quint16 nValue);
    void set_e_cblp(quint16 nValue);
    void set_e_cp(quint16 nValue);
    void set_e_crlc(quint16 nValue);
    void set_e_cparhdr(quint16 nValue);
    void set_e_minalloc(quint16 nValue);
    void set_e_maxalloc(quint16 nValue);
    void set_e_ss(quint16 nValue);
    void set_e_sp(quint16 nValue);
    void set_e_csum(quint16 nValue);
    void set_e_ip(quint16 nValue);
    void set_e_cs(quint16 nValue);
    void set_e_lfarlc(quint16 nValue);
    void set_e_ovno(quint16 nValue);
    void set_e_res(qint32 nPosition, quint16 nValue);
    void set_e_oemid(quint16 nValue);
    void set_e_oeminfo(quint16 nValue);
    void set_e_res2(qint32 nPosition, quint16 nValue);
    void set_e_lfanew(quint32 nValue);

    quint16 get_e_magic();
    quint16 get_e_cblp();
    quint16 get_e_cp();
    quint16 get_e_crlc();
    quint16 get_e_cparhdr();
    quint16 get_e_minalloc();
    quint16 get_e_maxalloc();
    quint16 get_e_ss();
    quint16 get_e_sp();
    quint16 get_e_csum();
    quint16 get_e_ip();
    quint16 get_e_cs();
    quint16 get_e_lfarlc();
    quint16 get_e_ovno();
    quint16 get_e_res(qint32 nPosition);
    quint16 get_e_oemid();
    quint16 get_e_oeminfo();
    quint16 get_e_res2(qint32 nPosition);
    quint32 get_e_lfanew();

    virtual QList<MAPMODE> getMapModesList();
    virtual _MEMORY_MAP getMemoryMap(XBinary::MAPMODE mapMode = XBinary::MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    virtual qint64 getImageSize();
    qint64 getModuleAddress();

    static QMap<quint64, QString> getImageMagics();
    static QMap<quint64, QString> getImageMagicsS();

    bool isLE();
    bool isLX();
    bool isNE();
    bool isPE();

    // rich for PE and LE
    bool isRichSignaturePresent();
    QList<XMSDOS::MS_RICH_RECORD> getRichSignatureRecords(PDSTRUCT *pPdStruct = nullptr);
    qint32 getNumberOfRichIDs(PDSTRUCT *pPdStruct = nullptr);
    qint32 getNumberOfRichIDs(QList<MS_RICH_RECORD> *pListRichSignatureRecords);
    bool isRichVersionPresent(quint32 nVersion, PDSTRUCT *pPdStruct = nullptr);
    bool isRichVersionPresent(quint32 nVersion, QList<MS_RICH_RECORD> *pListRichSignatureRecords);
    quint32 getRichVersion(QList<MS_RICH_RECORD> *pListRichSignatureRecords, qint32 nPosition);
    quint32 getRichID(QList<MS_RICH_RECORD> *pListRichSignatureRecords, qint32 nPosition);
    quint32 getRichCount(QList<MS_RICH_RECORD> *pListRichSignatureRecords, qint32 nPosition);

    // dos stub for LE, NE and PE
    qint64 getDosStubSize();
    qint64 getDosStubOffset();
    QByteArray getDosStub();
    bool isDosStubPresent();
    virtual bool removeDosStub();
    virtual bool addDosStub(const QString &sFileName);

    virtual MODE getMode();
    virtual QString getArch();
    virtual ENDIAN getEndian();
    virtual FT getFileType();
    virtual qint32 getType();
    virtual OSNAME getOsName();
    virtual QString typeIdToString(qint32 nType);
    virtual QString getMIMEString();

    virtual QString structIDToString(quint32 nID);
    virtual QList<XFHEADER> getXFHeaders(const XFSTRUCT &xfStruct, PDSTRUCT *pPdStruct) override;
    virtual QList<XFRECORD> getXFRecords(FT fileType, quint32 nStructID, const XLOC &xLoc) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);

    // File parts enumeration
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;
};

#endif  // XMSDOS_H
