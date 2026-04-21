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
#include "xpyc.h"

namespace {
struct MAGIC_RECORD {
    quint16 nMagic;
    const char *pszVersion;
};

// Conversion table for structure IDs to strings
XBinary::XCONVERT _TABLE_XPYC_STRUCTID[] = {{XPYC::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
                                            {XPYC::STRUCTID_HEADER, "HEADER", QString("Header")},
                                            {XPYC::STRUCTID_CODEOBJECT, "CODEOBJECT", QString("Code Object")}};

// Magic number records sorted by value for binary search
static const MAGIC_RECORD g_records[] = {{2012, "1.5.2"},  {3000, "3000"},   {3111, "3.0a4"},  {3131, "3.0b1"},  {3141, "3.1a1"},
                                         {3151, "3.1a1"},  {3160, "3.2a1"},  {3170, "3.2a2"},  {3180, "3.2a3"},  {3190, "3.3a1"},
                                         {3200, "3.3a1"},  {3210, "3.3a1"},  {3220, "3.3a2"},  {3230, "3.3a4"},  {3250, "3.4a1"},
                                         {3260, "3.4a1"},  {3270, "3.4a1"},  {3280, "3.4a1"},  {3290, "3.4a4"},  {3300, "3.4a4"},
                                         {3310, "3.4rc2"}, {3320, "3.5a1"},  {3330, "3.5b1"},  {3340, "3.5b2"},  {3350, "3.5b3"},
                                         {3351, "3.5.2"},  {3360, "3.6a0"},  {3361, "3.6a1"},  {3370, "3.6a2"},  {3371, "3.6a2"},
                                         {3372, "3.6a2"},  {3373, "3.6b1"},  {3375, "3.6b1"},  {3376, "3.6b1"},  {3377, "3.6b1"},
                                         {3378, "3.6b2"},  {3379, "3.6rc1"}, {3390, "3.7a1"},  {3391, "3.7a2"},  {3392, "3.7a4"},
                                         {3393, "3.7b1"},  {3394, "3.7b5"},  {3400, "3.8a1"},  {3401, "3.8a1"},  {3410, "3.8a1"},
                                         {3411, "3.8b2"},  {3412, "3.8b2"},  {3413, "3.8b4"},  {3420, "3.9a0"},  {3421, "3.9a0"},
                                         {3422, "3.9a0"},  {3423, "3.9a2"},  {3424, "3.9a2"},  {3425, "3.9a2"},  {3430, "3.10a1"},
                                         {3431, "3.10a1"}, {3432, "3.10a2"}, {3433, "3.10a2"}, {3434, "3.10a6"}, {3435, "3.10a7"},
                                         {3436, "3.10b1"}, {3437, "3.10b1"}, {3438, "3.10b1"}, {3439, "3.10b1"}, {3450, "3.11a1"},
                                         {3451, "3.11a1"}, {3452, "3.11a1"}, {3453, "3.11a1"}, {3454, "3.11a1"}, {3455, "3.11a1"},
                                         {3456, "3.11a1"}, {3457, "3.11a1"}, {3458, "3.11a1"}, {3459, "3.11a1"}, {3460, "3.11a1"},
                                         {3461, "3.11a1"}, {3462, "3.11a2"}, {3463, "3.11a3"}, {3464, "3.11a3"}, {3465, "3.11a3"},
                                         {3466, "3.11a4"}, {3467, "3.11a4"}, {3468, "3.11a4"}, {3469, "3.11a4"}, {3470, "3.11a4"},
                                         {3471, "3.11a4"}, {3472, "3.11a4"}, {3473, "3.11a4"}, {3474, "3.11a4"}, {3475, "3.11a5"},
                                         {3476, "3.11a5"}, {3477, "3.11a5"}, {3478, "3.11a5"}, {3479, "3.11a5"}, {3480, "3.11a5"},
                                         {3481, "3.11a5"}, {3482, "3.11a5"}, {3483, "3.11a5"}, {3484, "3.11a5"}, {3485, "3.11a5"},
                                         {3486, "3.11a6"}, {3487, "3.11a6"}, {3488, "3.11a6"}, {3489, "3.11a6"}, {3490, "3.11a6"},
                                         {3491, "3.11a6"}, {3492, "3.11a7"}, {3493, "3.11a7"}, {3494, "3.11a7"}, {3495, "3.11b4"},
                                         {3500, "3.12a1"}, {3501, "3.12a1"}, {3502, "3.12a1"}, {3503, "3.12a1"}, {3504, "3.12a1"},
                                         {3505, "3.12a1"}, {3506, "3.12a1"}, {3507, "3.12a1"}, {3508, "3.12a1"}, {3509, "3.12a1"},
                                         {3510, "3.12a2"}, {3511, "3.12a2"}, {3512, "3.12a2"}, {3513, "3.12a4"}, {3514, "3.12a4"},
                                         {3515, "3.12a5"}, {3516, "3.12a5"}, {3517, "3.12a5"}, {3518, "3.12a6"}, {3519, "3.12a6"},
                                         {3520, "3.12a6"}, {3521, "3.12a7"}, {3522, "3.12a7"}, {3523, "3.12a7"}, {3524, "3.12a7"},
                                         {3525, "3.12b1"}, {3526, "3.12b1"}, {3527, "3.12b1"}, {3528, "3.12b1"}, {3529, "3.12b1"},
                                         {3530, "3.12b1"}, {3531, "3.12b1"}, {3550, "3.13a1"}, {3551, "3.13a1"}, {3552, "3.13a1"},
                                         {3553, "3.13a1"}, {3554, "3.13a1"}, {3555, "3.13a1"}, {3556, "3.13a1"}, {3557, "3.13a1"},
                                         {3558, "3.13a1"}, {3559, "3.13a1"}, {3560, "3.13a1"}, {3561, "3.13a1"}, {3562, "3.13a1"},
                                         {3563, "3.13a1"}, {3564, "3.13a1"}, {3565, "3.13a1"}, {3566, "3.13a1"}, {3567, "3.13a1"},
                                         {3568, "3.13a1"}, {3569, "3.13a5"}, {3570, "3.13a6"}, {3571, "3.13b1"}, {3600, "3.14 will start with"},
                                         {5042, "1.6"},    {5082, "2.0.1"},  {6020, "2.1.2"},  {6071, "2.2"},    {6201, "2.3a0"},
                                         {6202, "2.3a0"},  {6204, "2.4a0"},  {6205, "2.4a3"},  {6206, "2.4b1"},  {6207, "2.5a0"},
                                         {6208, "2.5a0"},  {6209, "2.5a0"},  {6210, "2.5b3"},  {6211, "2.5b3"},  {6212, "2.5c1"},
                                         {6213, "2.5c2"},  {6215, "2.6a0"},  {6216, "2.6a1"},  {6217, "2.7a0"},  {6218, "2.7a0"},
                                         {6219, "2.7a0"},  {6220, "2.7a0"},  {6221, "2.7a0"}};

static qint32 getRecordsCount()
{
    return (qint32)(sizeof(g_records) / sizeof(g_records[0]));
}
}  // namespace

XPYC::XPYC(QIODevice *pDevice) : XBinary(pDevice)
{
}

XPYC::~XPYC()
{
}

bool XPYC::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() >= 12) {
        quint16 nMagicMarker = read_uint16(2);

        if (nMagicMarker == 0x0A0D) {
            quint16 nMagicValue = read_uint16(0);

            if (_isMagicKnown(nMagicValue)) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XPYC::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XPYC xpyc(pDevice);

    return xpyc.isValid();
}

QString XPYC::getArch()
{
    return "Python VM";
}

XBinary::MODE XPYC::getMode()
{
    return MODE_DATA;
}

XBinary::ENDIAN XPYC::getEndian()
{
    return ENDIAN_LITTLE;
}

XBinary::FT XPYC::getFileType()
{
    return FT_PYC;
}

QString XPYC::getVersion()
{
    INFO info = getInternalInfo();

    return info.sVersion;
}

QString XPYC::getFileFormatExt()
{
    return "pyc";
}

QString XPYC::getFileFormatExtsString()
{
    return "Python Compiled (*.pyc)";
}

qint64 XPYC::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    if (isValid(pPdStruct)) {
        INFO info = getInternalInfo(pPdStruct);
        qint32 nMajor = 0;
        qint32 nMinor = 0;
        _parseVersionNumbers(info.sVersion, &nMajor, &nMinor);

        // Header (magic + marker)
        nResult = 4;

        // Metadata size
        if ((nMajor > 3) || ((nMajor == 3) && (nMinor >= 7))) {
            // New layout: flags (4 bytes) + optional hash (8 bytes) + timestamp/source size (4 bytes each)
            nResult += 4;  // Flags
            if (info.bHashBased) {
                nResult += 8;  // Hash
                nResult += 4;  // Source size
            } else {
                nResult += 4;  // Timestamp
                nResult += 4;  // Source size
            }
        } else {
            // Old layout: timestamp (4 bytes) + source size (4 bytes)
            nResult += 8;
        }

        // Code object size (rest of file)
        // PYC format includes the entire file, so return total file size
        nResult = getSize();
    }

    return nResult;
}

QString XPYC::getMIMEString()
{
    return "application/x-python-code";
}

QString XPYC::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XPYC_STRUCTID, sizeof(_TABLE_XPYC_STRUCTID) / sizeof(XBinary::XCONVERT));
}

XPYC::INFO XPYC::getInternalInfo(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    INFO info = {};

    if (getSize() < 4) {
        return info;
    }

    info.nMagicValue = read_uint16(0);
    info.nMagicMarker = read_uint16(2);
    info.sVersion = _magicToVersion(info.nMagicValue);
    info.nFlags = 0;
    info.bHashBased = false;
    info.nTimestamp = 0;
    info.nSourceSize = 0;
    info.baHash.clear();

    qint32 nMajor = 0;
    qint32 nMinor = 0;
    _parseVersionNumbers(info.sVersion, &nMajor, &nMinor);

    qint64 nOffset = 4;

    if (getSize() >= (nOffset + 4)) {
        quint32 nWord = read_uint32(nOffset);
        bool bUseNewLayout = false;

        if ((nMajor > 3) || ((nMajor == 3) && (nMinor >= 7))) {
            bUseNewLayout = true;
        }

        if (bUseNewLayout) {
            info.nFlags = nWord;
            info.bHashBased = ((info.nFlags & 0x00000001) != 0);
            nOffset += 4;

            if (info.bHashBased) {
                if (getSize() >= (nOffset + 8)) {
                    info.baHash = read_array(nOffset, 8);
                    nOffset += 8;
                }

                if (getSize() >= (nOffset + 4)) {
                    info.nSourceSize = read_uint32(nOffset);
                }
            } else {
                if (getSize() >= (nOffset + 4)) {
                    info.nTimestamp = read_uint32(nOffset);
                    nOffset += 4;
                }

                if (getSize() >= (nOffset + 4)) {
                    info.nSourceSize = read_uint32(nOffset);
                }
            }
        } else {
            info.nTimestamp = nWord;

            if (getSize() >= (nOffset + 8)) {
                info.nSourceSize = read_uint32(nOffset + 4);
            }
        }
    }

    return info;
}

QString XPYC::_readMarshalString(qint64 *pnOffset)
{
    if (!pnOffset) return QString();

    qint64 nOffset = *pnOffset;
    qint64 nTotalSize = getSize();

    if ((nOffset + 1) > nTotalSize) {
        return QString();
    }

    quint8 nType = read_uint8(nOffset);
    quint8 nActualType = nType & 0x7F;  // Strip REF flag (0x80)
    nOffset += 1;

    qint32 nLength = 0;

    // Handle different string type codes
    if ((nActualType == TYPE_SHORT_ASCII) || (nActualType == TYPE_SHORT_ASCII_INTERNED)) {
        // Short ASCII: length is 1 byte
        if ((nOffset + 1) > nTotalSize) {
            return QString();
        }
        nLength = read_uint8(nOffset);
        nOffset += 1;
    } else if ((nActualType == TYPE_STRING) || (nActualType == TYPE_INTERNED) || (nActualType == TYPE_UNICODE) || (nActualType == TYPE_ASCII) ||
               (nActualType == TYPE_ASCII_INTERNED)) {
        // Regular string: length is 4 bytes (little-endian)
        if ((nOffset + 4) > nTotalSize) {
            return QString();
        }
        nLength = read_int32(nOffset, false);
        nOffset += 4;
    } else {
        return QString();
    }

    if (nLength < 0 || nLength > 1024 * 1024) {
        return QString();  // Sanity check
    }

    if ((nOffset + nLength) > nTotalSize) {
        return QString();
    }

    QString sResult;

    if ((nActualType == TYPE_UNICODE) || (nActualType == TYPE_STRING)) {
        // UTF-8 encoded string
        QByteArray baData = read_array(nOffset, nLength);
        sResult = QString::fromUtf8(baData);
    } else {
        // ASCII string
        sResult = read_ansiString(nOffset, nLength);
    }

    nOffset += nLength;
    *pnOffset = nOffset;

    return sResult;
}

qint32 XPYC::_readMarshalInt(qint64 *pnOffset)
{
    if (!pnOffset) return 0;

    qint64 nOffset = *pnOffset;
    qint64 nTotalSize = getSize();

    if ((nOffset + 1) > nTotalSize) {
        return 0;
    }

    quint8 nType = read_uint8(nOffset);
    // qDebug("_readMarshalInt at offset %lld: type=0x%02X", nOffset, nType);
    nOffset += 1;

    qint32 nResult = 0;

    if (nType == TYPE_INT) {
        // 4-byte integer (little-endian)
        if ((nOffset + 4) > nTotalSize) {
            *pnOffset = nOffset;
            return 0;
        }
        nResult = read_int32(nOffset, false);
        nOffset += 4;
    } else if (nType == TYPE_INT64) {
        // 8-byte integer (little-endian) - truncate to 32-bit
        if ((nOffset + 8) > nTotalSize) {
            *pnOffset = nOffset;
            return 0;
        }
        qint64 nValue = read_int64(nOffset, false);
        nResult = static_cast<qint32>(nValue);
        nOffset += 8;
    } else if (nType == TYPE_NONE) {
        nResult = 0;
    } else if (nType == TYPE_TRUE) {
        nResult = 1;
    } else if (nType == TYPE_FALSE) {
        nResult = 0;
    }

    *pnOffset = nOffset;
    return nResult;
}

XPYC::MARSHAL_OBJECT XPYC::_readMarshalObject(qint64 *pnOffset, PDSTRUCT *pPdStruct)
{
    MARSHAL_OBJECT obj = {};
    obj.bValid = false;

    if (!pnOffset) return obj;

    qint64 nOffset = *pnOffset;
    qint64 nTotalSize = getSize();

    if ((nOffset + 1) > nTotalSize) {
        return obj;
    }

    quint8 nType = read_uint8(nOffset);
    quint8 nActualType = nType & 0x7F;  // Strip REF flag (0x80)
    obj.nType = nType;
    nOffset += 1;

    if (nActualType == TYPE_NONE) {
        obj.vValue = QVariant();
        obj.bValid = true;
    } else if (nActualType == TYPE_TRUE) {
        obj.vValue = true;
        obj.bValid = true;
    } else if (nActualType == TYPE_FALSE) {
        obj.vValue = false;
        obj.bValid = true;
    } else if (nActualType == TYPE_INT) {
        if ((nOffset + 4) > nTotalSize) {
            *pnOffset = nOffset;
            return obj;
        }
        qint32 nValue = read_int32(nOffset, false);
        nOffset += 4;
        obj.vValue = nValue;
        obj.bValid = true;
    } else if (nActualType == TYPE_INT64) {
        if ((nOffset + 8) > nTotalSize) {
            *pnOffset = nOffset;
            return obj;
        }
        qint64 nValue = read_int64(nOffset, false);
        nOffset += 8;
        obj.vValue = nValue;
        obj.bValid = true;
    } else if (nActualType == TYPE_FLOAT) {
        // Binary float representation
        if ((nOffset + 8) > nTotalSize) {
            *pnOffset = nOffset;
            return obj;
        }
        double dValue = 0.0;
        QByteArray baFloat = read_array(nOffset, 8);
        if (baFloat.size() == 8) {
            memcpy(&dValue, baFloat.constData(), 8);
        }
        nOffset += 8;
        obj.vValue = dValue;
        obj.bValid = true;
    } else if ((nActualType == TYPE_STRING) || (nActualType == TYPE_INTERNED) || (nActualType == TYPE_UNICODE) || (nActualType == TYPE_ASCII) ||
               (nActualType == TYPE_ASCII_INTERNED) || (nActualType == TYPE_SHORT_ASCII) || (nActualType == TYPE_SHORT_ASCII_INTERNED)) {
        // String type - rewind and use string reader
        nOffset -= 1;
        qint64 nStringOffset = nOffset;
        QString sValue = _readMarshalString(&nStringOffset);
        nOffset = nStringOffset;
        obj.vValue = sValue;
        obj.bValid = true;
    } else if ((nActualType == TYPE_TUPLE) || (nActualType == TYPE_SMALL_TUPLE)) {
        // Tuple type - rewind and use tuple reader
        nOffset -= 1;
        qint64 nTupleOffset = nOffset;
        obj.listItems = _readMarshalTuple(&nTupleOffset, pPdStruct);
        nOffset = nTupleOffset;
        obj.bValid = true;
    } else if (nActualType == TYPE_CODE) {
        // Nested code object - skip for now
        obj.vValue = QString("<code object>");
        obj.bValid = true;
        // TODO: Recursively parse code object
    } else {
        // Unknown or unsupported type
        obj.vValue = QString("<unknown type 0x%1>").arg(nActualType, 2, 16, QChar('0'));
        obj.bValid = false;
    }

    *pnOffset = nOffset;
    return obj;
}

QList<XPYC::MARSHAL_OBJECT> XPYC::_readMarshalTuple(qint64 *pnOffset, PDSTRUCT *pPdStruct)
{
    QList<MARSHAL_OBJECT> listResult;

    if (!pnOffset) return listResult;

    qint64 nOffset = *pnOffset;
    qint64 nTotalSize = getSize();

    if ((nOffset + 1) > nTotalSize) {
        return listResult;
    }

    quint8 nType = read_uint8(nOffset);
    quint8 nActualType = nType & 0x7F;  // Strip REF flag (0x80)
    // qDebug("_readMarshalTuple at offset %lld: type=0x%02X (actual=0x%02X)", *pnOffset, nType, nActualType);
    nOffset += 1;

    if ((nActualType != TYPE_TUPLE) && (nActualType != TYPE_SMALL_TUPLE)) {
        // qDebug("_readMarshalTuple: not a tuple type, returning empty");
        return listResult;
    }

    qint32 nCount = 0;

    if (nActualType == TYPE_SMALL_TUPLE) {
        // Small tuple: count is 1 byte
        if ((nOffset + 1) > nTotalSize) {
            return listResult;
        }
        nCount = read_uint8(nOffset);
        nOffset += 1;
    } else {
        // Regular tuple: count is 4 bytes (little-endian)
        if ((nOffset + 4) > nTotalSize) {
            return listResult;
        }
        nCount = read_int32(nOffset, false);
        nOffset += 4;
    }

    if (nCount < 0 || nCount > 10000) {
        return listResult;  // Sanity check
    }

    // Read each element
    for (qint32 i = 0; (i < nCount) && isPdStructNotCanceled(pPdStruct); i++) {
        qint64 nItemOffset = nOffset;
        MARSHAL_OBJECT obj = _readMarshalObject(&nOffset, pPdStruct);
        // qDebug("_readMarshalTuple: item[%d] at offset %lld, type=0x%02X, valid=%d, new offset=%lld", i, nItemOffset, obj.nType, obj.bValid, nOffset);
        listResult.append(obj);

        if (!obj.bValid) {
            // qDebug("_readMarshalTuple: stopping at invalid object");
            break;  // Stop on error
        }
    }

    *pnOffset = nOffset;
    return listResult;
}

XPYC::CODE_OBJECT XPYC::getCodeObject(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    CODE_OBJECT codeObject = {};
    codeObject.bValid = false;
    codeObject.nFirstLineNo = 0;
    codeObject.nArgCount = 0;
    codeObject.nPosOnlyArgCount = 0;
    codeObject.nKwOnlyArgCount = 0;
    codeObject.nNLocals = 0;
    codeObject.nStackSize = 0;
    codeObject.nFlags = 0;

    if (!isValid(pPdStruct)) {
        return codeObject;
    }

    INFO info = getInternalInfo(pPdStruct);
    qint32 nMajor = 0;
    qint32 nMinor = 0;
    _parseVersionNumbers(info.sVersion, &nMajor, &nMinor);

    // qDebug("getCodeObject: Version %d.%d, bHashBased=%d", nMajor, nMinor, info.bHashBased);

    // Calculate offset to marshalled code object
    qint64 nCodeOffset = 4;  // Magic + Marker
    if ((nMajor > 3) || ((nMajor == 3) && (nMinor >= 7))) {
        nCodeOffset += 4;  // Flags
        if (info.bHashBased) {
            nCodeOffset += 8 + 4;  // Hash + source size
        } else {
            nCodeOffset += 4 + 4;  // Timestamp + source size
        }
    } else {
        nCodeOffset += 8;  // Timestamp + source size
    }

    qint64 nTotalSize = getSize();
    // qDebug("getCodeObject: nCodeOffset=%lld, nTotalSize=%lld", nCodeOffset, nTotalSize);

    if (nCodeOffset >= nTotalSize) {
        // qDebug("getCodeObject: nCodeOffset >= nTotalSize, returning invalid");
        return codeObject;
    }

    // Read marshal type byte
    quint8 nMarshalType = read_uint8(nCodeOffset);

    // qDebug("getCodeObject: nMarshalType=0x%02X at offset %lld", nMarshalType, nCodeOffset);

    // Check for code object type (TYPE_CODE = 'c' = 0x63)
    // The high bit (0x80) is the FLAG_REF which indicates this object can be referenced later
    bool bHasRefFlag = (nMarshalType & 0x80) != 0;
    quint8 nActualType = nMarshalType & 0x7F;

    // qDebug("getCodeObject: bHasRefFlag=%d, nActualType=0x%02X", bHasRefFlag, nActualType);

    if (nActualType != 0x63) {
        // Not a code object at the expected position
        // qDebug("getCodeObject: Marshal type is not CODE (0x63), returning invalid");
        return codeObject;
    }

    // Parse code object fields (version-dependent)
    qint64 nOffset = nCodeOffset + 1;
    // qDebug("getCodeObject: Starting parse at offset %lld", nOffset);

    // Python 3.11+ changed to raw integer format for initial fields
    if ((nMajor >= 3) && (nMinor >= 11)) {
        // Read argcount as raw integer
        if ((nOffset + 4) <= nTotalSize) {
            codeObject.nArgCount = read_int32(nOffset, false);
            nOffset += 4;
        }
        // qDebug("getCodeObject: nArgCount=%d (raw int), nOffset=%lld", codeObject.nArgCount, nOffset);

        // Read posonlyargcount as raw integer (Python 3.8+)
        if ((nOffset + 4) <= nTotalSize) {
            codeObject.nPosOnlyArgCount = read_int32(nOffset, false);
            nOffset += 4;
        }
        // qDebug("getCodeObject: nPosOnlyArgCount=%d (raw int), nOffset=%lld", codeObject.nPosOnlyArgCount, nOffset);

        // Read kwonlyargcount as raw integer
        if ((nOffset + 4) <= nTotalSize) {
            codeObject.nKwOnlyArgCount = read_int32(nOffset, false);
            nOffset += 4;
        }
        // qDebug("getCodeObject: nKwOnlyArgCount=%d (raw int), nOffset=%lld", codeObject.nKwOnlyArgCount, nOffset);
    } else {
        // Pre-3.11: Read argcount (all versions)
        codeObject.nArgCount = _readMarshalInt(&nOffset);
        // qDebug("getCodeObject: nArgCount=%d, nOffset=%lld", codeObject.nArgCount, nOffset);

        // Read posonlyargcount (Python 3.8+)
        if ((nMajor >= 3) && (nMinor >= 8)) {
            codeObject.nPosOnlyArgCount = _readMarshalInt(&nOffset);
            // qDebug("getCodeObject: nPosOnlyArgCount=%d, nOffset=%lld", codeObject.nPosOnlyArgCount, nOffset);
        }

        // Read kwonlyargcount (Python 3.0+)
        if (nMajor >= 3) {
            codeObject.nKwOnlyArgCount = _readMarshalInt(&nOffset);
            // qDebug("getCodeObject: nKwOnlyArgCount=%d, nOffset=%lld", codeObject.nKwOnlyArgCount, nOffset);
        }

        // Read nlocals (pre-3.11)
        codeObject.nNLocals = _readMarshalInt(&nOffset);
        // qDebug("getCodeObject: nNLocals=%d, nOffset=%lld", codeObject.nNLocals, nOffset);
    }

    // Python 3.11+ does not have nlocals in marshal format (it's computed)
    // For pre-3.11, nlocals was already read above

    // Python 3.11+ changed code object format - stacksize and flags are raw integers
    if ((nMajor >= 3) && (nMinor >= 11)) {
        // Read stacksize as raw integer (4 bytes, little-endian)
        if ((nOffset + 4) <= nTotalSize) {
            codeObject.nStackSize = read_int32(nOffset, false);
            nOffset += 4;
        }
        // qDebug("getCodeObject: nStackSize=%d (raw int), nOffset=%lld", codeObject.nStackSize, nOffset);

        // Read flags as raw integer (4 bytes, little-endian)
        if ((nOffset + 4) <= nTotalSize) {
            codeObject.nFlags = read_int32(nOffset, false);
            nOffset += 4;
        }
        // qDebug("getCodeObject: nFlags=0x%08X (raw int), nOffset=%lld", codeObject.nFlags, nOffset);
    } else {
        // Pre-3.11: Read stacksize (all versions)
        codeObject.nStackSize = _readMarshalInt(&nOffset);
        // qDebug("getCodeObject: nStackSize=%d, nOffset=%lld", codeObject.nStackSize, nOffset);

        // Read flags (all versions)
        codeObject.nFlags = _readMarshalInt(&nOffset);
        // qDebug("getCodeObject: nFlags=0x%08X, nOffset=%lld", codeObject.nFlags, nOffset);
    }

    // Read code (bytecode) - TYPE_STRING or TYPE_BYTES
    qint64 nCodeBytesOffset = nOffset;
    // qDebug("getCodeObject: Reading code at offset %lld", nOffset);
    if ((nOffset + 1) <= nTotalSize) {
        quint8 nCodeType = read_uint8(nOffset);
        quint8 nActualCodeType = nCodeType & 0x7F;  // Strip REF flag
        // qDebug("getCodeObject: Code type byte=0x%02X (actual type=0x%02X)", nCodeType, nActualCodeType);
        nOffset += 1;

        if ((nActualCodeType == TYPE_STRING) || (nActualCodeType == TYPE_SHORT_ASCII) || (nActualCodeType == TYPE_ASCII)) {
            qint32 nCodeLength = 0;

            if (nActualCodeType == TYPE_SHORT_ASCII) {
                nCodeLength = read_uint8(nOffset);
                nOffset += 1;
            } else {
                nCodeLength = read_int32(nOffset, false);
                nOffset += 4;
            }

            // qDebug("getCodeObject: Code length=%d, reading from offset %lld", nCodeLength, nOffset);
            if ((nCodeLength >= 0) && (nCodeLength <= 1024 * 1024) && ((nOffset + nCodeLength) <= nTotalSize)) {
                codeObject.baCode = read_array(nOffset, nCodeLength);
                nOffset += nCodeLength;
                // qDebug("getCodeObject: Read %d bytes of code, nOffset=%lld", nCodeLength, nOffset);
            }
        }
    }

    // Read consts (tuple of constants)
    // qDebug("getCodeObject: Reading consts tuple at offset %lld", nOffset);
    codeObject.listConsts = _readMarshalTuple(&nOffset, pPdStruct);
    // qDebug("getCodeObject: Read %d consts, nOffset=%lld", codeObject.listConsts.count(), nOffset);

    // Read names (tuple of variable/function names)
    // qDebug("getCodeObject: Reading names tuple at offset %lld", nOffset);
    QList<MARSHAL_OBJECT> listNamesObjects = _readMarshalTuple(&nOffset, pPdStruct);
    // qDebug("getCodeObject: Read %d names, nOffset=%lld", listNamesObjects.count(), nOffset);
    for (qint32 i = 0; i < listNamesObjects.size(); i++) {
        if (listNamesObjects[i].vValue.type() == QVariant::String) {
            codeObject.listNames.append(listNamesObjects[i].vValue.toString());
        }
    }

    // Read varnames (tuple of local variable names)
    QList<MARSHAL_OBJECT> listVarNamesObjects = _readMarshalTuple(&nOffset, pPdStruct);
    for (qint32 i = 0; i < listVarNamesObjects.size(); i++) {
        if (listVarNamesObjects[i].vValue.type() == QVariant::String) {
            codeObject.listVarNames.append(listVarNamesObjects[i].vValue.toString());
        }
    }

    // Read freevars (tuple of free variable names)
    QList<MARSHAL_OBJECT> listFreeVarsObjects = _readMarshalTuple(&nOffset, pPdStruct);
    for (qint32 i = 0; i < listFreeVarsObjects.size(); i++) {
        if (listFreeVarsObjects[i].vValue.type() == QVariant::String) {
            codeObject.listFreeVars.append(listFreeVarsObjects[i].vValue.toString());
        }
    }

    // Read cellvars (tuple of cell variable names)
    QList<MARSHAL_OBJECT> listCellVarsObjects = _readMarshalTuple(&nOffset, pPdStruct);
    for (qint32 i = 0; i < listCellVarsObjects.size(); i++) {
        if (listCellVarsObjects[i].vValue.type() == QVariant::String) {
            codeObject.listCellVars.append(listCellVarsObjects[i].vValue.toString());
        }
    }

    // Read filename (string)
    // qDebug("getCodeObject: Reading filename at offset %lld", nOffset);
    codeObject.sFileName = _readMarshalString(&nOffset);
    // qDebug("getCodeObject: sFileName='%s', nOffset=%lld", codeObject.sFileName.toUtf8().constData(), nOffset);

    // Read name (string)
    // qDebug("getCodeObject: Reading name at offset %lld", nOffset);
    codeObject.sName = _readMarshalString(&nOffset);
    // qDebug("getCodeObject: sName='%s', nOffset=%lld", codeObject.sName.toUtf8().constData(), nOffset);

    // Read firstlineno (integer)
    codeObject.nFirstLineNo = _readMarshalInt(&nOffset);
    // qDebug("getCodeObject: nFirstLineNo=%d, nOffset=%lld", codeObject.nFirstLineNo, nOffset);

    // Read lnotab (line number table) - skip for now
    // This is a complex structure that maps bytecode offsets to line numbers

    // Set as valid if we successfully parsed key fields
    // Name might be empty or a reference in Python 3.11+
    codeObject.bValid = (codeObject.baCode.size() > 0 || codeObject.listConsts.count() > 0 || !codeObject.sName.isEmpty());

    return codeObject;
}

bool XPYC::isConstPresent(const QString &sConstValue, PDSTRUCT *pPdStruct)
{
    CODE_OBJECT codeObject = getCodeObject(pPdStruct);

    if (!codeObject.bValid) {
        return false;
    }

    for (qint32 i = 0; i < codeObject.listConsts.count(); i++) {
        if (codeObject.listConsts[i].vValue.type() == QVariant::String) {
            QString sValue = codeObject.listConsts[i].vValue.toString();
            if (sValue == sConstValue) {
                return true;
            }
        }
    }

    return false;
}

bool XPYC::isConstPresent(const CODE_OBJECT *pCodeObject, const QString &sConstValue, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    if (!pCodeObject || !pCodeObject->bValid) {
        return false;
    }

    for (qint32 i = 0; i < pCodeObject->listConsts.count(); i++) {
        if (pCodeObject->listConsts[i].vValue.type() == QVariant::String) {
            QString sValue = pCodeObject->listConsts[i].vValue.toString();
            if (sValue == sConstValue) {
                return true;
            }
        }
    }

    return false;
}

XBinary::_MEMORY_MAP XPYC::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
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

QList<XBinary::FPART> XPYC::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<FPART> listResult;

    qint64 nTotalSize = getSize();
    qint64 nFormatSize = getFileFormatSize(pPdStruct);
    INFO info = getInternalInfo(pPdStruct);

    // Calculate header size (magic + marker + metadata)
    qint64 nHeaderSize = 4;  // Magic + Marker
    qint32 nMajor = 0;
    qint32 nMinor = 0;
    _parseVersionNumbers(info.sVersion, &nMajor, &nMinor);

    if ((nMajor > 3) || ((nMajor == 3) && (nMinor >= 7))) {
        // New layout: flags (4 bytes) + optional hash (8 bytes) + timestamp/source size (4 bytes each)
        nHeaderSize += 4;  // Flags
        if (info.bHashBased) {
            nHeaderSize += 8 + 4;  // Hash + source size
        } else {
            nHeaderSize += 4 + 4;  // Timestamp + source size
        }
    } else {
        // Old layout: timestamp (4 bytes) + source size (4 bytes)
        nHeaderSize += 8;
    }

    // Header (includes magic, marker, and all metadata)
    if (nFileParts & FILEPART_HEADER) {
        FPART record = {};
        record.filePart = FILEPART_HEADER;
        record.nFileOffset = 0;
        record.nFileSize = qMin(nHeaderSize, nTotalSize);
        record.nVirtualAddress = -1;
        record.sName = tr("Header");

        listResult.append(record);
    }

    // Code object (remaining data up to format size)
    if ((nFileParts & FILEPART_REGION) && (nHeaderSize < nFormatSize)) {
        FPART record = {};
        record.filePart = FILEPART_REGION;
        record.nFileOffset = nHeaderSize;
        record.nFileSize = nFormatSize - nHeaderSize;
        record.nVirtualAddress = -1;
        record.sName = tr("Code Object");

        listResult.append(record);
    }

    if (nFileParts & FILEPART_DATA) {
        FPART record = {};
        record.filePart = FILEPART_DATA;
        record.nFileOffset = 0;
        record.nFileSize = nFormatSize;
        record.nVirtualAddress = -1;
        record.sName = tr("Data");

        listResult.append(record);
    }

    // Overlay (if any data beyond format size)
    if ((nFileParts & FILEPART_OVERLAY) && (nFormatSize < nTotalSize)) {
        FPART record = {};
        record.filePart = FILEPART_OVERLAY;
        record.nFileOffset = nFormatSize;
        record.nFileSize = nTotalSize - nFormatSize;
        record.nVirtualAddress = -1;
        record.sName = tr("Overlay");

        listResult.append(record);
    }

    return listResult;
}

QList<XBinary::DATA_HEADER> XPYC::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        // Initialize with default headers
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        // Start with header
        _dataHeadersOptions.nID = STRUCTID_HEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_HEADER) {
                INFO info = getInternalInfo(pPdStruct);
                qint32 nMajor = 0;
                qint32 nMinor = 0;
                _parseVersionNumbers(info.sVersion, &nMajor, &nMinor);

                bool bNewLayout = ((nMajor > 3) || ((nMajor == 3) && (nMinor >= 7)));

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPYC::structIDToString(dataHeadersOptions.nID));

                // Calculate header size
                dataHeader.nSize = 4;  // Magic + Marker
                if (bNewLayout) {
                    dataHeader.nSize += 4;  // Flags
                    if (info.bHashBased) {
                        dataHeader.nSize += 8 + 4;  // Hash + source size
                    } else {
                        dataHeader.nSize += 4 + 4;  // Timestamp + source size
                    }
                } else {
                    dataHeader.nSize += 8;  // Timestamp + source size
                }

                // Add all header fields
                dataHeader.listRecords.append(getDataRecord(nStartOffset + 0, 2, "Magic Value", VT_UINT16, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(nStartOffset + 2, 2, "Magic Marker", VT_UINT16, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));

                qint64 nOffset = 4;
                if (bNewLayout) {
                    dataHeader.listRecords.append(getDataRecord(nStartOffset + nOffset, 4, "Flags", VT_UINT32, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                    nOffset += 4;

                    if (info.bHashBased) {
                        dataHeader.listRecords.append(
                            getDataRecord(nStartOffset + nOffset, 8, "Hash", VT_BYTE_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                        nOffset += 8;
                        dataHeader.listRecords.append(
                            getDataRecord(nStartOffset + nOffset, 4, "Source Size", VT_UINT32, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                    } else {
                        dataHeader.listRecords.append(
                            getDataRecord(nStartOffset + nOffset, 4, "Timestamp", VT_UINT32, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                        nOffset += 4;
                        dataHeader.listRecords.append(
                            getDataRecord(nStartOffset + nOffset, 4, "Source Size", VT_UINT32, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                    }
                } else {
                    dataHeader.listRecords.append(getDataRecord(nStartOffset + nOffset, 4, "Timestamp", VT_UINT32, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                    nOffset += 4;
                    dataHeader.listRecords.append(getDataRecord(nStartOffset + nOffset, 4, "Source Size", VT_UINT32, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                }

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    // Add marshalled code object section
                    qint64 nHeaderSize = dataHeader.nSize;
                    qint64 nFormatSize = getFileFormatSize(pPdStruct);

                    if (nHeaderSize < nFormatSize) {
                        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
                        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
                        _dataHeadersOptions.nID = STRUCTID_CODEOBJECT;
                        _dataHeadersOptions.nLocation = nHeaderSize;
                        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

                        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
                    }
                }
            } else if (dataHeadersOptions.nID == STRUCTID_CODEOBJECT) {
                qint64 nFormatSize = getFileFormatSize(pPdStruct);

                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XPYC::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = nFormatSize - nStartOffset;

                // Marshalled code object - binary data
                dataHeader.listRecords.append(
                    getDataRecord(nStartOffset, dataHeader.nSize, "Marshalled Code Object", VT_BYTE_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));

                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QString XPYC::_magicToVersion(quint16 nMagicValue)
{
    QString sResult;

    qint32 nCount = getRecordsCount();

    // Binary search for efficiency
    qint32 nLeft = 0;
    qint32 nRight = nCount - 1;

    while (nLeft <= nRight) {
        qint32 nMid = nLeft + (nRight - nLeft) / 2;

        if (g_records[nMid].nMagic == nMagicValue) {
            sResult = QString::fromLatin1(g_records[nMid].pszVersion);
            break;
        } else if (g_records[nMid].nMagic < nMagicValue) {
            nLeft = nMid + 1;
        } else {
            nRight = nMid - 1;
        }
    }

    return sResult;
}

bool XPYC::_isMagicKnown(quint16 nMagicValue)
{
    qint32 nCount = getRecordsCount();

    // Binary search for efficiency
    qint32 nLeft = 0;
    qint32 nRight = nCount - 1;

    while (nLeft <= nRight) {
        qint32 nMid = nLeft + (nRight - nLeft) / 2;

        if (g_records[nMid].nMagic == nMagicValue) {
            return true;
        } else if (g_records[nMid].nMagic < nMagicValue) {
            nLeft = nMid + 1;
        } else {
            nRight = nMid - 1;
        }
    }

    return false;
}

void XPYC::_parseVersionNumbers(const QString &sVersion, qint32 *pnMajor, qint32 *pnMinor)
{
    if (pnMajor) {
        *pnMajor = 0;
    }

    if (pnMinor) {
        *pnMinor = 0;
    }

    qint32 nLength = sVersion.length();
    qint32 nIndex = 0;

    qint32 nCurrentMajor = 0;
    qint32 nCurrentMinor = 0;

    while ((nIndex < nLength) && sVersion.at(nIndex).isDigit()) {
        int nDigit = sVersion.at(nIndex).digitValue();

        if (nDigit >= 0) {
            nCurrentMajor = nCurrentMajor * 10 + nDigit;
        }

        nIndex++;
    }

    bool bHasDot = false;

    if ((nIndex < nLength) && (sVersion.at(nIndex) == '.')) {
        bHasDot = true;
        nIndex++;

        while ((nIndex < nLength) && sVersion.at(nIndex).isDigit()) {
            int nDigit = sVersion.at(nIndex).digitValue();

            if (nDigit >= 0) {
                nCurrentMinor = nCurrentMinor * 10 + nDigit;
            }

            nIndex++;
        }
    }

    if ((!bHasDot) && (sVersion == "3000")) {
        nCurrentMajor = 3;
        nCurrentMinor = 0;
    }

    if (pnMajor) {
        *pnMajor = nCurrentMajor;
    }

    if (pnMinor) {
        *pnMinor = nCurrentMinor;
    }
}

