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
#include "xder.h"

XDER::XDER(QIODevice *pDevice) : XBinary(pDevice)
{
}

XDER::~XDER()
{
}

bool XDER::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    HEADER header = _getHeader(nullptr);

    return header.bIsValid;
}

bool XDER::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XDER der(pDevice);

    return der.isValid();
}

XBinary::FT XDER::getFileType()
{
    return FT_DER;
}

QString XDER::getVersion()
{
    return QString();
}

QString XDER::getFileFormatExt()
{
    return "der";
}

QString XDER::getFileFormatExtsString()
{
    return "DER (*.der *.cer *.crt *.p7b *.p7c *.p12 *.pfx *.spc)";  // typical DER-encoded containers
}

qint64 XDER::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    HEADER header = _getHeader(nullptr);

    if (header.bIsValid) {
        return header.nHeaderSize + header.nContentSize;
    }

    return 0;
}

QString XDER::getMIMEString()
{
    return "application/pkix-cert";  // commonly used for DER certs
}

XDER::HEADER XDER::getHeader(PDSTRUCT *pPdStruct)
{
    return _getHeader(pPdStruct);
}

XDER::HEADER XDER::_getHeader(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    HEADER result = {};

    qint64 nSize = getSize();
    if (nSize < 2) {
        result.bIsValid = false;
        return result;
    }

    qint64 nOffset = 0;
    quint8 nTag = read_uint8(nOffset);

    // Typical DER starts with SEQUENCE (0x30). Accept universal/constructed.
    if ((nTag & 0x1F) != 0x10) {  // 0x10 == SEQUENCE tag number
        // Still allow other top-level tags (e.g., OCTET STRING, 0x04), but require definite length.
        // We'll not restrict too much: just require valid length and fit in file.
    }

    // Read ASN.1 definite length using shared XBinary helper
    XBinary::PACKED_UINT packedLen = read_acn1_integer(nOffset + 1, nSize - 1);
    if (!packedLen.bIsValid || packedLen.nValue < 0) {
        result.bIsValid = false;
        return result;
    }

    qint64 nLen = (qint64)packedLen.nValue;
    qint64 nHeaderSize = 1 + packedLen.nByteSize;
    qint64 nContentOffset = nOffset + nHeaderSize;
    qint64 nEnd = nContentOffset + nLen;

    if (nEnd <= nSize) {
        result.nTag = nTag;
        result.nHeaderSize = nHeaderSize;
        result.nContentOffset = nContentOffset;
        result.nContentSize = nLen;
        result.bIsValid = true;
    } else {
        result.bIsValid = false;
    }

    return result;
}

