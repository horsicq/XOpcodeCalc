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
#include "xmp4.h"

XBinary::XCONVERT _TABLE_XMP4_STRUCTID[] = {
    {XMP4::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XMP4::STRUCTID_BOX, "BOX", QObject::tr("Box")},
    {XMP4::STRUCTID_HEADER, "HEADER", QObject::tr("Header")},
};

XMP4::XMP4(QIODevice *pDevice) : XBinary(pDevice)
{
}

XMP4::~XMP4()
{
}

bool XMP4::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() > 0x20) {
        _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

        if (compareSignature(&memoryMap, "000000..'ftyp'", 0, pPdStruct)) {
            // TODO more checks !!!
            bResult = true;
        }
    }

    return bResult;
}

bool XMP4::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XMP4 mp4(pDevice);

    return mp4.isValid();
}

QString XMP4::getFileFormatExt()
{
    return "mp4";
}

QString XMP4::getFileFormatExtsString()
{
    return "MP4";
}

qint64 XMP4::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XMP4::getMIMEString()
{
    return "video/mp4";
}

XBinary::_MEMORY_MAP XMP4::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    return _getMemoryMap(FILEPART_HEADER | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
}

XBinary::FT XMP4::getFileType()
{
    return FT_MP4;
}

bool XMP4::isTagValid(const QString &sTagName)
{
    bool bResult = false;

    if ((sTagName == "ftyp") || (sTagName == "pdin") || (sTagName == "moov") || (sTagName == "moof") || (sTagName == "mfra") || (sTagName == "mdat") ||
        (sTagName == "stts") || (sTagName == "stsc") || (sTagName == "stsz") || (sTagName == "meta") || (sTagName == "mvhd") || (sTagName == "trak") ||
        (sTagName == "udta") || (sTagName == "iods") || (sTagName == "uuid") || (sTagName == "free")) {
        bResult = true;
    } else {
#ifdef QT_DEBUG
        qDebug("%s", sTagName.toLatin1().data());
#endif
    }

    return bResult;
}

QList<XBinary::DATA_HEADER> XMP4::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    QList<DATA_HEADER> list;

    if (!(dataHeadersOptions.nID) || (dataHeadersOptions.nID == STRUCTID_BOX)) {
        DATA_HEADERS_OPTIONS opt = dataHeadersOptions;
        opt.nID = STRUCTID_BOX;
        opt.dhMode = DHMODE_TABLE;
        DATA_HEADER t = _initDataHeader(opt, tr("Boxes"));
        t.locType = LT_OFFSET;
        t.nLocation = 0;
        t.nSize = getSize();
        t.listRecords.append(getDataRecord(0, 4, "size", VT_UINT32, DRF_SIZE, ENDIAN_BIG));
        t.listRecords.append(getDataRecord(4, 4, "type", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
        list.append(t);
    }

    return list;
}

QList<XBinary::FPART> XMP4::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    QList<FPART> list;
    const qint64 nTotal = getSize();

    if (nFileParts & FILEPART_HEADER) {
        FPART h = {};
        h.filePart = FILEPART_HEADER;
        h.nFileOffset = 0;
        h.nFileSize = qMin<qint64>(nTotal, 8);  // size(4)+type(4) of first box
        h.nVirtualAddress = -1;
        h.sName = tr("Header");
        list.append(h);
    }

    if (nFileParts & FILEPART_REGION) {
        qint64 nOffset = 0;
        while ((nOffset + 8 <= nTotal) && XBinary::isPdStructNotCanceled(pPdStruct)) {
            quint32 size = read_uint32(nOffset, true);
            QString type = read_ansiString(nOffset + 4, 4);

            if (size == 0) {
                // box extends to EOF
                size = (quint32)(nTotal - nOffset);
            }
            if (size == 1) {
                // 64-bit largesize follows
                if (nOffset + 16 > nTotal) break;
                quint64 largesize = read_uint64(nOffset + 8, true);
                if (largesize < 16) break;
                // Cap to file size
                if ((qint64)largesize > nTotal - nOffset) largesize = (quint64)(nTotal - nOffset);
                FPART f = {};
                f.filePart = FILEPART_REGION;
                f.nFileOffset = nOffset;
                f.nFileSize = (qint64)largesize;
                f.nVirtualAddress = -1;
                f.sName = type;
                list.append(f);
                nOffset += (qint64)largesize;
                continue;
            }

            if (size < 8 || (nOffset + size > nTotal) || !isTagValid(type)) {
                break;
            }

            FPART f = {};
            f.filePart = FILEPART_REGION;
            f.nFileOffset = nOffset;
            f.nFileSize = size;
            f.nVirtualAddress = -1;
            f.sName = type;
            list.append(f);

            nOffset += size;
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        // Overlay is anything past the last parsed box
        qint64 nMax = 0;
        for (int i = 0; i < list.size(); ++i) {
            const FPART &p = list.at(i);
            nMax = qMax(nMax, p.nFileOffset + p.nFileSize);
        }
        if (nMax < nTotal) {
            FPART ov = {};
            ov.filePart = FILEPART_OVERLAY;
            ov.nFileOffset = nMax;
            ov.nFileSize = nTotal - nMax;
            ov.nVirtualAddress = -1;
            ov.sName = tr("Overlay");
            list.append(ov);
        }
    }

    return list;
}

QList<XBinary::MAPMODE> XMP4::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

QString XMP4::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XMP4_STRUCTID, sizeof(_TABLE_XMP4_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<QString> XMP4::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("000000..'ftyp'");

    return listResult;
}

XBinary *XMP4::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XMP4(pDevice);
}

