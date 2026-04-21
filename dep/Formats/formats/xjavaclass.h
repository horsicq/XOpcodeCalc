/* Copyright (c) 2024-2026 hors<horsicq@gmail.com>
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
#ifndef XJAVACLASS_H
#define XJAVACLASS_H

#include "xbinary.h"

class XJavaClass : public XBinary {
    Q_OBJECT

public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_MAGIC,
        STRUCTID_VERSION,
        STRUCTID_CONSTANT_POOL,
        STRUCTID_ACCESS_FLAGS,
        STRUCTID_THIS_CLASS,
        STRUCTID_SUPER_CLASS,
        STRUCTID_INTERFACES,
        STRUCTID_FIELDS,
        STRUCTID_METHODS,
        STRUCTID_ATTRIBUTES
    };

    struct cp_info {
        qint64 nOffset;
        quint8 nTag;
        QVariant varInfo;
    };

    struct attribute_info {
        quint16 nAttributeNameIndex;
        quint32 nAttributeLength;
        QByteArray baInfo;
    };

    struct record_info {
        quint16 nAccessFlags;
        quint16 nNameIndex;
        quint16 nDescriptorIndex;
        quint16 nAttributesCount;
        QList<attribute_info> listAttributes;
    };

    enum CONSTANT {
        CONSTANT_Utf8 = 1,
        CONSTANT_Integer = 3,
        CONSTANT_Float = 4,
        CONSTANT_Long = 5,
        CONSTANT_Double = 6,
        CONSTANT_Class = 7,
        CONSTANT_String = 8,
        CONSTANT_Fieldref = 9,
        CONSTANT_Methodref = 10,
        CONSTANT_InterfaceMethodref = 11,
        CONSTANT_NameAndType = 12,
        CONSTANT_MethodHandle = 15,
        CONSTANT_MethodType = 16,
        CONSTANT_InvokeDynamic = 18,
        CONSTANT_Module = 19,
        CONSTANT_Package = 20
    };

    struct INFO {
        quint64 nSize;
        quint16 nMinorVersion;
        quint16 nMajorVersion;
        quint16 nConstantPoolCount;
        QList<cp_info> listCP;
        quint16 nAccessFlags;
        quint16 nThisClass;
        quint16 nSuperClass;
        quint16 nInterfacesCount;
        QList<quint16> listInterfaces;
        quint16 nFieldsCount;
        QList<record_info> listFields;
        quint16 nMethodsCount;
        QList<record_info> listMethods;
        quint16 nAttributesCount;
        QList<attribute_info> listAttributes;
    };

    explicit XJavaClass(QIODevice *pDevice = nullptr);
    virtual ~XJavaClass();

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

    virtual QList<MAPMODE> getMapModesList() override;
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

    INFO _getInfo(PDSTRUCT *pPdStruct = nullptr);

    static QString _getJDKVersion(quint16 nMajor, quint16 nMinor);

private:
qint32 _read_attribute_info(qint64 nOffset, attribute_info *pAttributeInfo);
    qint32 _read_record_info(qint64 nOffset, record_info *pRecordInfo);

};

#endif  // XJAVACLASS_H

