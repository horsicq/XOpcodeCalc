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
#ifndef XPYC_H
#define XPYC_H

#include "xbinary.h"

class XPYC : public XBinary {
    Q_OBJECT

public:
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_HEADER,
        STRUCTID_CODEOBJECT
    };

    struct INFO {
        quint16 nMagicValue;
        quint16 nMagicMarker;
        QString sVersion;
        quint32 nFlags;
        bool bHashBased;
        quint32 nTimestamp;
        quint32 nSourceSize;
        QByteArray baHash;
    };

    // Marshal format type codes
    enum MARSHAL_TYPE {
        TYPE_NULL = '0',
        TYPE_NONE = 'N',
        TYPE_FALSE = 'F',
        TYPE_TRUE = 'T',
        TYPE_STOPITER = 'S',
        TYPE_ELLIPSIS = '.',
        TYPE_INT = 'i',
        TYPE_INT64 = 'I',
        TYPE_FLOAT = 'f',
        TYPE_BINARY_FLOAT = 'g',
        TYPE_COMPLEX = 'x',
        TYPE_BINARY_COMPLEX = 'y',
        TYPE_LONG = 'l',
        TYPE_STRING = 's',
        TYPE_INTERNED = 't',
        TYPE_REF = 'r',
        TYPE_TUPLE = '(',
        TYPE_LIST = '[',
        TYPE_DICT = '{',
        TYPE_CODE = 'c',
        TYPE_UNICODE = 'u',
        TYPE_UNKNOWN = '?',
        TYPE_SET = '<',
        TYPE_FROZENSET = '>',
        TYPE_ASCII = 'a',
        TYPE_ASCII_INTERNED = 'A',
        TYPE_SMALL_TUPLE = ')',
        TYPE_SHORT_ASCII = 'z',
        TYPE_SHORT_ASCII_INTERNED = 'Z'
    };

    struct MARSHAL_OBJECT {
        quint8 nType;
        QVariant vValue;
        QList<MARSHAL_OBJECT> listItems;
        bool bValid;
    };

    struct CODE_OBJECT {
        QString sName;                     // co_name
        QString sFileName;                 // co_filename
        qint32 nFirstLineNo;               // co_firstlineno
        qint32 nArgCount;                  // co_argcount
        qint32 nPosOnlyArgCount;           // co_posonlyargcount (3.8+)
        qint32 nKwOnlyArgCount;            // co_kwonlyargcount
        qint32 nNLocals;                   // co_nlocals
        qint32 nStackSize;                 // co_stacksize
        qint32 nFlags;                     // co_flags
        QByteArray baCode;                 // co_code (bytecode instructions)
        QList<MARSHAL_OBJECT> listConsts;  // co_consts (constants tuple)
        QList<QString> listNames;          // co_names
        QList<QString> listVarNames;       // co_varnames
        QList<QString> listFreeVars;       // co_freevars
        QList<QString> listCellVars;       // co_cellvars
        bool bValid;                       // parsing success flag
    };

    explicit XPYC(QIODevice *pDevice = nullptr);
    virtual ~XPYC();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);

    virtual QString getArch() override;
    virtual MODE getMode() override;
    virtual ENDIAN getEndian() override;
    virtual FT getFileType() override;
    virtual QString getVersion() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct = nullptr) override;
    virtual QString getMIMEString() override;
    virtual QString structIDToString(quint32 nID) override;

    INFO getInternalInfo(PDSTRUCT *pPdStruct = nullptr);
    CODE_OBJECT getCodeObject(PDSTRUCT *pPdStruct = nullptr);
    bool isConstPresent(const QString &sConstValue, PDSTRUCT *pPdStruct = nullptr);
    static bool isConstPresent(const CODE_OBJECT *pCodeObject, const QString &sConstValue, PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct = nullptr) override;

private:
    static QString _magicToVersion(quint16 nMagicValue);
    static bool _isMagicKnown(quint16 nMagicValue);
    static void _parseVersionNumbers(const QString &sVersion, qint32 *pnMajor, qint32 *pnMinor);

    MARSHAL_OBJECT _readMarshalObject(qint64 *pnOffset, PDSTRUCT *pPdStruct);
    QString _readMarshalString(qint64 *pnOffset);
    qint32 _readMarshalInt(qint64 *pnOffset);
    QList<MARSHAL_OBJECT> _readMarshalTuple(qint64 *pnOffset, PDSTRUCT *pPdStruct);
};

#endif  // XPYC_H

