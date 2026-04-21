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
#include "xjavaclass.h"

XBinary::XCONVERT _TABLE_XJAVACLASS_STRUCTID[] = {
    {XJavaClass::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XJavaClass::STRUCTID_MAGIC, "MAGIC", QString("Magic")},
    {XJavaClass::STRUCTID_VERSION, "VERSION", QString("Version")},
    {XJavaClass::STRUCTID_CONSTANT_POOL, "CONSTANT_POOL", QString("Constant pool")},
    {XJavaClass::STRUCTID_ACCESS_FLAGS, "ACCESS_FLAGS", QString("Access flags")},
    {XJavaClass::STRUCTID_THIS_CLASS, "THIS_CLASS", QString("This class")},
    {XJavaClass::STRUCTID_SUPER_CLASS, "SUPER_CLASS", QString("Super class")},
    {XJavaClass::STRUCTID_INTERFACES, "INTERFACES", QString("Interfaces")},
    {XJavaClass::STRUCTID_FIELDS, "FIELDS", QString("Fields")},
    {XJavaClass::STRUCTID_METHODS, "METHODS", QString("Methods")},
    {XJavaClass::STRUCTID_ATTRIBUTES, "ATTRIBUTES", QString("Attributes")},
};

XJavaClass::XJavaClass(QIODevice *pDevice) : XBinary(pDevice)
{
}

XJavaClass::~XJavaClass()
{
}

bool XJavaClass::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() >= 24) {
        if (read_uint32(0, true) == 0xCAFEBABE) {
            if (read_uint32(4, true) > 10) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XJavaClass::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XJavaClass xjavaclass(pDevice);

    return xjavaclass.isValid();
}

QString XJavaClass::getArch()
{
    return "JVM";
}

QString XJavaClass::getVersion()
{
    QString sResult;

    quint16 nMinor = read_uint16(4, true);
    quint16 nMajor = read_uint16(6, true);

    if (nMajor) {
        sResult = _getJDKVersion(nMajor, nMinor);
    }

    return sResult;
}

QString XJavaClass::getFileFormatExt()
{
    return "class";
}

QString XJavaClass::getFileFormatExtsString()
{
    return "Java Class (*.class)";
}

qint64 XJavaClass::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    INFO info = _getInfo(pPdStruct);
    nResult = info.nSize;

    return nResult;
}

QString XJavaClass::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XJAVACLASS_STRUCTID, sizeof(_TABLE_XJAVACLASS_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::MAPMODE> XJavaClass::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);
    listResult.append(MAPMODE_DATA);

    return listResult;
}

XJavaClass::INFO XJavaClass::_getInfo(PDSTRUCT *pPdStruct)
{
    INFO result = {};

    result.nMinorVersion = read_uint16(4, true);
    result.nMajorVersion = read_uint16(6, true);
    result.nConstantPoolCount = read_uint16(8, true);

    qint64 nOffset = 10;

    for (int i = 1; (i < result.nConstantPoolCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        quint8 nTag = read_uint8(nOffset);

        cp_info cpInfo = {};
        cpInfo.nOffset = nOffset;
        cpInfo.nTag = nTag;

        // handle tags
        switch (nTag) {
            case CONSTANT_Utf8: {
                quint16 nLength = read_uint16(nOffset + 1, true);
                QByteArray baData = read_array(nOffset + 3, nLength);
                cpInfo.varInfo = QString(baData);
                nOffset += 3 + nLength;
            } break;
            case CONSTANT_Integer: {
                cpInfo.varInfo = read_uint32(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_Float: {
                cpInfo.varInfo = read_float(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_Long: {
                cpInfo.varInfo = read_uint64(nOffset + 1, true);
                nOffset += 9;
            } break;
            case CONSTANT_Double: {
                cpInfo.varInfo = read_double(nOffset + 1, true);
                nOffset += 9;
            } break;
            case CONSTANT_Class: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 3;
            } break;
            case CONSTANT_String: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 3;
            } break;
            case CONSTANT_Fieldref: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_Methodref: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_InterfaceMethodref: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_NameAndType: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);  // TODO two indexes
                nOffset += 5;
            } break;
            case CONSTANT_MethodHandle: {
                cpInfo.varInfo = read_uint8(nOffset + 1);  // TODO
                nOffset += 4;
            } break;
            case CONSTANT_MethodType: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 3;
            } break;
            case CONSTANT_InvokeDynamic: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 5;
            } break;
            case CONSTANT_Module: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 3;
            } break;
            case CONSTANT_Package: {
                cpInfo.varInfo = read_uint16(nOffset + 1, true);
                nOffset += 3;
            } break;
            default: {
#ifdef QT_DEBUG
                qDebug("Unknown tag: %02X", nTag);
#endif
                break;
            }
        }

        // Print offset, tag, value
        // qDebug("%08X %02X %s", cpInfo.nOffset, cpInfo.nTag, cpInfo.varInfo.toString().toLatin1().data());

        // add to list
        result.listCP.append(cpInfo);

        if (nTag == CONSTANT_Long || nTag == CONSTANT_Double) {
            i++;
        }

        if (nOffset >= getSize()) {
            break;
        }
    }

    result.nAccessFlags = read_uint16(nOffset, true);
    nOffset += 2;
    result.nThisClass = read_uint16(nOffset, true);
    nOffset += 2;
    result.nSuperClass = read_uint16(nOffset, true);
    nOffset += 2;
    result.nInterfacesCount = read_uint16(nOffset, true);
    nOffset += 2;

    for (int i = 0; (i < result.nInterfacesCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        result.listInterfaces.append(read_uint16(nOffset, true));
        nOffset += 2;
    }

    result.nFieldsCount = read_uint16(nOffset, true);
    nOffset += 2;

    for (int i = 0; (i < result.nFieldsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        record_info fieldInfo = {};

        nOffset += _read_record_info(nOffset, &fieldInfo);

        result.listFields.append(fieldInfo);
    }

    result.nMethodsCount = read_uint16(nOffset, true);
    nOffset += 2;

    for (int i = 0; (i < result.nMethodsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        record_info methodInfo = {};

        nOffset += _read_record_info(nOffset, &methodInfo);

        result.listMethods.append(methodInfo);
    }

    result.nAttributesCount = read_uint16(nOffset, true);
    nOffset += 2;

    for (int i = 0; (i < result.nAttributesCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        attribute_info attributeInfo = {};

        nOffset += _read_attribute_info(nOffset, &attributeInfo);

        result.listAttributes.append(attributeInfo);
    }

    result.nSize = nOffset;

    return result;
}

QString XJavaClass::_getJDKVersion(quint16 nMajor, quint16 nMinor)
{
    QString sResult;

    switch (nMajor) {
        case 0x2D: sResult = "JDK 1.1"; break;
        case 0x2E: sResult = "JDK 1.2"; break;
        case 0x2F: sResult = "JDK 1.3"; break;
        case 0x30: sResult = "JDK 1.4"; break;
        case 0x31: sResult = "Java SE 5.0"; break;
        case 0x32: sResult = "Java SE 6"; break;
        case 0x33: sResult = "Java SE 7"; break;
        case 0x34: sResult = "Java SE 8"; break;
        case 0x35: sResult = "Java SE 9"; break;
        case 0x36: sResult = "Java SE 10"; break;
        case 0x37: sResult = "Java SE 11"; break;
        case 0x38: sResult = "Java SE 12"; break;
        case 0x39: sResult = "Java SE 13"; break;
        case 0x3A: sResult = "Java SE 14"; break;
        case 0x3B: sResult = "Java SE 15"; break;
        case 0x3C: sResult = "Java SE 16"; break;
        case 0x3D: sResult = "Java SE 17"; break;
        case 0x3E: sResult = "Java SE 18"; break;
        case 0x3F: sResult = "Java SE 19"; break;
        case 0x40: sResult = "Java SE 20"; break;
        case 0x41: sResult = "Java SE 21"; break;
        case 0x42: sResult = "Java SE 22"; break;
        case 0x43: sResult = "Java SE 23"; break;
        case 0x44: sResult = "Java SE 24"; break;
        case 0x45: sResult = "Java SE 25"; break;
        case 0x46: sResult = "Java SE 26"; break;
        case 0x47: sResult = "Java SE 27"; break;
        case 0x48: sResult = "Java SE 28"; break;
        case 0x49: sResult = "Java SE 29"; break;
        case 0x4A: sResult = "Java SE 30"; break;
    }

    if ((sResult != "") && (nMinor)) {
        sResult += QString(".%1").arg(nMinor);
    }

    return sResult;
}

QString XJavaClass::getMIMEString()
{
    return "application/java-vm";
}

qint32 XJavaClass::_read_attribute_info(qint64 nOffset, attribute_info *pAttributeInfo)
{
    qint32 nResult = 0;

    pAttributeInfo->nAttributeNameIndex = read_uint16(nOffset, true);
    pAttributeInfo->nAttributeLength = read_uint32(nOffset + 2, true);
    pAttributeInfo->baInfo = read_array(nOffset + 6, pAttributeInfo->nAttributeLength);

    nResult = 6 + pAttributeInfo->nAttributeLength;

    return nResult;
}

qint32 XJavaClass::_read_record_info(qint64 nOffset, record_info *pRecordInfo)
{
    qint32 nResult = 0;
    qint64 nOriginalOffset = nOffset;

    pRecordInfo->nAccessFlags = read_uint16(nOffset, true);
    pRecordInfo->nNameIndex = read_uint16(nOffset + 2, true);
    pRecordInfo->nDescriptorIndex = read_uint16(nOffset + 4, true);
    pRecordInfo->nAttributesCount = read_uint16(nOffset + 6, true);

    nOffset += 8;

    for (int i = 0; i < pRecordInfo->nAttributesCount; i++) {
        attribute_info attributeInfo = {};

        nOffset += _read_attribute_info(nOffset, &attributeInfo);

        pRecordInfo->listAttributes.append(attributeInfo);
    }

    nResult = nOffset - nOriginalOffset;

    return nResult;
}

XBinary::_MEMORY_MAP XJavaClass::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_DATA;
    }

    if (mapMode == MAPMODE_REGIONS) {
        result = _getMemoryMap(FILEPART_HEADER | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
    } else if (mapMode == MAPMODE_DATA) {
        result = _getMemoryMap(FILEPART_DATA | FILEPART_OVERLAY, pPdStruct);
    }

    return result;
}

QList<XBinary::DATA_HEADER> XJavaClass::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        _dataHeadersOptions.nID = STRUCTID_MAGIC;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_MAGIC) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 4;

                dataHeader.listRecords.append(getDataRecord(0, 4, "Magic", VT_UINT32, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_VERSION;
                    _dataHeadersOptions.nLocation = 4;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_VERSION) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 4;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Minor version", VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                dataHeader.listRecords.append(getDataRecord(2, 2, "Major version", VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_CONSTANT_POOL;
                    _dataHeadersOptions.nLocation = 8;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_CONSTANT_POOL) {
                INFO info = _getInfo(pPdStruct);

                qint64 nConstantPoolSize = 0;
                if (info.listCP.size() > 0) {
                    qint64 nLastOffset = info.listCP.last().nOffset;
                    quint8 nLastTag = info.listCP.last().nTag;

                    nConstantPoolSize = nLastOffset - 10;

                    switch (nLastTag) {
                        case CONSTANT_Utf8: {
                            quint16 nLength = read_uint16(nLastOffset + 1, true);
                            nConstantPoolSize += 3 + nLength;
                        } break;
                        case CONSTANT_Integer:
                        case CONSTANT_Float:
                        case CONSTANT_Fieldref:
                        case CONSTANT_Methodref:
                        case CONSTANT_InterfaceMethodref:
                        case CONSTANT_NameAndType:
                        case CONSTANT_InvokeDynamic: nConstantPoolSize += 5; break;
                        case CONSTANT_Long:
                        case CONSTANT_Double: nConstantPoolSize += 9; break;
                        case CONSTANT_Class:
                        case CONSTANT_String:
                        case CONSTANT_MethodType:
                        case CONSTANT_Module:
                        case CONSTANT_Package: nConstantPoolSize += 3; break;
                        case CONSTANT_MethodHandle: nConstantPoolSize += 4; break;
                    }
                }

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2 + nConstantPoolSize;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Constant pool count", VT_UINT16, DRF_COUNT, XBinary::ENDIAN_BIG));
                if (nConstantPoolSize > 0) {
                    dataHeader.listRecords.append(getDataRecord(2, nConstantPoolSize, "Constant pool", VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_ACCESS_FLAGS;
                    _dataHeadersOptions.nLocation = 8 + 2 + nConstantPoolSize;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_ACCESS_FLAGS) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Access flags", VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_THIS_CLASS;
                    _dataHeadersOptions.nLocation = nStartOffset + 2;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_THIS_CLASS) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2;

                dataHeader.listRecords.append(getDataRecord(0, 2, "This class", VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_SUPER_CLASS;
                    _dataHeadersOptions.nLocation = nStartOffset + 2;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_SUPER_CLASS) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Super class", VT_UINT16, DRF_UNKNOWN, XBinary::ENDIAN_BIG));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_INTERFACES;
                    _dataHeadersOptions.nLocation = nStartOffset + 2;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_INTERFACES) {
                INFO info = _getInfo(pPdStruct);

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2 + (info.nInterfacesCount * 2);

                dataHeader.listRecords.append(getDataRecord(0, 2, "Interfaces count", VT_UINT16, DRF_COUNT, XBinary::ENDIAN_BIG));
                if (info.nInterfacesCount > 0) {
                    dataHeader.listRecords.append(getDataRecord(2, info.nInterfacesCount * 2, "Interfaces", VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_FIELDS;
                    _dataHeadersOptions.nLocation = nStartOffset + dataHeader.nSize;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_FIELDS) {
                INFO info = _getInfo(pPdStruct);

                qint64 nFieldsSize = 0;
                for (qint32 i = 0; (i < info.nFieldsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                    nFieldsSize += 8;
                    for (qint32 j = 0; j < info.listFields[i].nAttributesCount; j++) {
                        nFieldsSize += 6 + info.listFields[i].listAttributes[j].nAttributeLength;
                    }
                }

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2 + nFieldsSize;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Fields count", VT_UINT16, DRF_COUNT, XBinary::ENDIAN_BIG));
                if (nFieldsSize > 0) {
                    dataHeader.listRecords.append(getDataRecord(2, nFieldsSize, "Fields", VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_METHODS;
                    _dataHeadersOptions.nLocation = nStartOffset + dataHeader.nSize;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_METHODS) {
                INFO info = _getInfo(pPdStruct);

                qint64 nMethodsSize = 0;
                for (qint32 i = 0; (i < info.nMethodsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                    nMethodsSize += 8;
                    for (qint32 j = 0; j < info.listMethods[i].nAttributesCount; j++) {
                        nMethodsSize += 6 + info.listMethods[i].listAttributes[j].nAttributeLength;
                    }
                }

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2 + nMethodsSize;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Methods count", VT_UINT16, DRF_COUNT, XBinary::ENDIAN_BIG));
                if (nMethodsSize > 0) {
                    dataHeader.listRecords.append(getDataRecord(2, nMethodsSize, "Methods", VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                    _dataHeadersOptions.nID = STRUCTID_ATTRIBUTES;
                    _dataHeadersOptions.nLocation = nStartOffset + dataHeader.nSize;

                    listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                }
            } else if (dataHeadersOptions.nID == STRUCTID_ATTRIBUTES) {
                INFO info = _getInfo(pPdStruct);

                qint64 nAttributesSize = 0;
                for (qint32 i = 0; (i < info.nAttributesCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                    nAttributesSize += 6 + info.listAttributes[i].nAttributeLength;
                }

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XJavaClass::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = 2 + nAttributesSize;

                dataHeader.listRecords.append(getDataRecord(0, 2, "Attributes count", VT_UINT16, DRF_COUNT, XBinary::ENDIAN_BIG));
                if (nAttributesSize > 0) {
                    dataHeader.listRecords.append(getDataRecord(2, nAttributesSize, "Attributes", VT_BYTE_ARRAY, DRF_UNKNOWN, XBinary::ENDIAN_BIG));
                }

                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XJavaClass::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<FPART> listResult;

    INFO info = _getInfo(pPdStruct);

    if (nFileParts & FILEPART_HEADER) {
        FPART record = {};

        record.filePart = FILEPART_HEADER;
        record.nFileOffset = 0;
        record.nFileSize = 8;
        record.nVirtualAddress = -1;
        record.sName = tr("Header");

        listResult.append(record);
    }

    if (nFileParts & FILEPART_REGION) {
        qint64 nOffset = 8;

        // Constant pool
        qint64 nConstantPoolSize = 0;
        if (info.listCP.size() > 0) {
            qint64 nLastOffset = info.listCP.last().nOffset;
            quint8 nLastTag = info.listCP.last().nTag;

            nConstantPoolSize = nLastOffset - 10;

            switch (nLastTag) {
                case CONSTANT_Utf8: {
                    quint16 nLength = read_uint16(nLastOffset + 1, true);
                    nConstantPoolSize += 3 + nLength;
                } break;
                case CONSTANT_Integer:
                case CONSTANT_Float:
                case CONSTANT_Fieldref:
                case CONSTANT_Methodref:
                case CONSTANT_InterfaceMethodref:
                case CONSTANT_NameAndType:
                case CONSTANT_InvokeDynamic: nConstantPoolSize += 5; break;
                case CONSTANT_Long:
                case CONSTANT_Double: nConstantPoolSize += 9; break;
                case CONSTANT_Class:
                case CONSTANT_String:
                case CONSTANT_MethodType:
                case CONSTANT_Module:
                case CONSTANT_Package: nConstantPoolSize += 3; break;
                case CONSTANT_MethodHandle: nConstantPoolSize += 4; break;
            }
        }

        if (nConstantPoolSize > 0) {
            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 2 + nConstantPoolSize;
            record.nVirtualAddress = -1;
            record.sName = tr("Constant pool");
            listResult.append(record);
            nOffset += record.nFileSize;
        }

        // Access flags, this class, super class
        {
            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 6;
            record.nVirtualAddress = -1;
            record.sName = tr("Class info");
            listResult.append(record);
            nOffset += 6;
        }

        // Interfaces
        {
            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 2 + (info.nInterfacesCount * 2);
            record.nVirtualAddress = -1;
            record.sName = tr("Interfaces");
            listResult.append(record);
            nOffset += record.nFileSize;
        }

        // Fields
        {
            qint64 nFieldsSize = 0;
            for (qint32 i = 0; (i < info.nFieldsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                nFieldsSize += 8;
                for (qint32 j = 0; j < info.listFields[i].nAttributesCount; j++) {
                    nFieldsSize += 6 + info.listFields[i].listAttributes[j].nAttributeLength;
                }
            }

            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 2 + nFieldsSize;
            record.nVirtualAddress = -1;
            record.sName = tr("Fields");
            listResult.append(record);
            nOffset += record.nFileSize;
        }

        // Methods
        {
            qint64 nMethodsSize = 0;
            for (qint32 i = 0; (i < info.nMethodsCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                nMethodsSize += 8;
                for (qint32 j = 0; j < info.listMethods[i].nAttributesCount; j++) {
                    nMethodsSize += 6 + info.listMethods[i].listAttributes[j].nAttributeLength;
                }
            }

            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 2 + nMethodsSize;
            record.nVirtualAddress = -1;
            record.sName = tr("Methods");
            listResult.append(record);
            nOffset += record.nFileSize;
        }

        // Attributes
        {
            qint64 nAttributesSize = 0;
            for (qint32 i = 0; (i < info.nAttributesCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                nAttributesSize += 6 + info.listAttributes[i].nAttributeLength;
            }

            FPART record = {};
            record.filePart = FILEPART_REGION;
            record.nFileOffset = nOffset;
            record.nFileSize = 2 + nAttributesSize;
            record.nVirtualAddress = -1;
            record.sName = tr("Attributes");
            listResult.append(record);
            nOffset += record.nFileSize;
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        qint64 nTotalSize = getSize();
        qint64 nDataEnd = info.nSize;

        if (nDataEnd < nTotalSize) {
            FPART record = {};

            record.filePart = FILEPART_OVERLAY;
            record.nFileOffset = nDataEnd;
            record.nFileSize = nTotalSize - nDataEnd;
            record.nVirtualAddress = -1;
            record.sName = tr("Overlay");

            listResult.append(record);
        }
    }

    return listResult;
}

XBinary::FT XJavaClass::getFileType()
{
    return FT_JAVACLASS;
}

XBinary::ENDIAN XJavaClass::getEndian()
{
    return ENDIAN_BIG;
}

XBinary::MODE XJavaClass::getMode()
{
    return MODE_32;
}

QList<QString> XJavaClass::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("CAFEBABE");

    return listResult;
}

XBinary *XJavaClass::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XJavaClass(pDevice);
}

