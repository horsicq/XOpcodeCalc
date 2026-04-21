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
#include "xriff.h"

static XBinary::XCONVERT _TABLE_XRIFF_STRUCTID[] = {{XRiff::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
                                                    {XRiff::STRUCTID_HEADER, "HEADER", QString("HEADER")},
                                                    {XRiff::STRUCTID_CHUNK, "CHUNK", QString("CHUNK")}};

XRiff::XRiff(QIODevice *pDevice) : XBinary(pDevice)
{
}

XRiff::~XRiff()
{
}

bool XRiff::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() > 0x20) {
        QString sTag = read_ansiString(0, 4);

        bool bIsValid = false;
        bool bIsBigEndian = false;

        if (sTag == "RIFF") {
            bIsValid = true;
        } else if ((sTag == "RIFX") || (sTag == "AIFF")) {
            bIsValid = true;
            bIsBigEndian = true;
        }

        if (bIsValid) {
            quint32 nSize = read_uint32(4, bIsBigEndian);
            qint64 _nSize = getSize();

            if ((nSize > 0) && (nSize <= _nSize)) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XRiff::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XRiff xriff(pDevice);

    return xriff.isValid();
}

QString XRiff::getFileFormatExt()
{
    // TODO
    return read_ansiString(12, 4).trimmed().toLower();
}

QString XRiff::getFileFormatExtsString()
{
    return "RIFF (*.riff *.wav *.avi *.aiff *.aifc *.aif *.aifx *.rmi *.rmf *.rmv)";  // TODO
}

qint64 XRiff::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XRiff::getMIMEString()
{
    QString sResult;

    QString sTag = read_ansiString(0, 4);

    if (sTag == "RIFF") {
        sResult = "audio/x-wav";
    } else if (sTag == "RIFX") {
        sResult = "audio/x-aiff";
    } else if (sTag == "AIFF") {
        sResult = "audio/x-aiff";
    }

    return sResult;
}

XBinary::_MEMORY_MAP XRiff::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    // Delegate to generic helper using our file parts implementation
    return _getMemoryMap(FILEPART_HEADER | FILEPART_REGION | FILEPART_OVERLAY, pPdStruct);
}

XBinary::FT XRiff::getFileType()
{
    FT result = FT_RIFF;

    QString sTag = read_ansiString(0, 4);
    QString sSubTag = read_ansiString(12, 4);

    // TODO
    if ((sTag == "RIFF") || (sTag == "RIFX")) {
        if (sSubTag == "AVI ") {
            result = FT_AVI;
        } else if (sSubTag == "WEBP") {
            result = FT_WEBP;
        } else if (sSubTag == "WAVE") {
            result = FT_WAV;
        }
    }

    return result;
}

XBinary::ENDIAN XRiff::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    QString sTag = read_ansiString(0, 4);

    if (sTag == "RIFF") {
        result = ENDIAN_LITTLE;
    } else if ((sTag == "RIFX") || (sTag == "AIFF")) {
        result = ENDIAN_BIG;
    }

    return result;
}

QList<XBinary::MAPMODE> XRiff::getMapModesList()
{
    QList<MAPMODE> list;
    list.append(MAPMODE_REGIONS);
    return list;
}

QList<XBinary::FPART> XRiff::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    QList<FPART> list;

    if (getSize() < 12) return list;

    bool bBE = (getEndian() == ENDIAN_BIG);

    if (nFileParts & FILEPART_HEADER) {
        FPART f = {};
        f.filePart = FILEPART_HEADER;
        f.nFileOffset = 0;
        f.nFileSize = 12;  // RIFF header: 'RIFF'/'RIFX' + size + form type
        f.nVirtualAddress = -1;
        f.sName = tr("Header");
        list.append(f);
    }

    if (nFileParts & FILEPART_REGION) {
        // Top-level RIFF chunk spans from 0 to 8+size
        quint32 nSize = read_uint32(4, bBE);
        qint64 nChunkTotal = qMin<qint64>(getSize(), (qint64)nSize + 8);
        FPART f = {};
        f.filePart = FILEPART_REGION;
        f.nFileOffset = 0;
        f.nFileSize = nChunkTotal;
        f.nVirtualAddress = -1;
        f.sName = read_ansiString(0, 4);
        list.append(f);
    }

    if (nFileParts & FILEPART_OVERLAY) {
        // Anything beyond the stated RIFF size
        bool bBE2 = bBE;
        quint32 nSize = read_uint32(4, bBE2);
        qint64 nEnd = qMin<qint64>(getSize(), (qint64)nSize + 8);
        if (nEnd < getSize()) {
            FPART ov = {};
            ov.filePart = FILEPART_OVERLAY;
            ov.nFileOffset = nEnd;
            ov.nFileSize = getSize() - nEnd;
            ov.nVirtualAddress = -1;
            ov.sName = tr("Overlay");
            list.append(ov);
        }
    }

    return list;
}

QList<XBinary::DATA_HEADER> XRiff::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    QList<DATA_HEADER> list;

    if (!(dataHeadersOptions.nID) || (dataHeadersOptions.nID == STRUCTID_HEADER)) {
        DATA_HEADERS_OPTIONS opt = dataHeadersOptions;
        opt.nID = STRUCTID_HEADER;
        opt.dhMode = DHMODE_HEADER;
        DATA_HEADER h = _initDataHeader(opt, tr("Header"));
        h.locType = LT_OFFSET;
        h.nLocation = 0;
        h.nSize = 12;
        h.listRecords.append(getDataRecord(0, 4, "Tag", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
        h.listRecords.append(getDataRecord(4, 4, "Size", VT_UINT32, DRF_SIZE, getEndian()));
        h.listRecords.append(getDataRecord(8, 4, "Form", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
        list.append(h);
    }

    if (!(dataHeadersOptions.nID) || (dataHeadersOptions.nID == STRUCTID_CHUNK)) {
        // Present a single table row for the outer chunk
        DATA_HEADERS_OPTIONS opt = dataHeadersOptions;
        opt.nID = STRUCTID_CHUNK;
        opt.dhMode = DHMODE_TABLE;
        DATA_HEADER t = _initDataHeader(opt, tr("Chunks"));
        t.locType = LT_OFFSET;
        t.nLocation = 0;
        bool bBE = (getEndian() == ENDIAN_BIG);
        quint32 nSize = read_uint32(4, bBE);
        t.nSize = qMin<qint64>(getSize(), (qint64)nSize + 8);
        t.listRecords.append(getDataRecord(0, 4, "Tag", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
        t.listRecords.append(getDataRecord(4, 4, "Size", VT_UINT32, DRF_SIZE, getEndian()));
        t.listRecords.append(getDataRecord(8, 4, "Form", VT_ANSI, DRF_UNKNOWN, ENDIAN_LITTLE));
        list.append(t);
    }

    return list;
}

QString XRiff::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XRIFF_STRUCTID, sizeof(_TABLE_XRIFF_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<QString> XRiff::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'RIFF'");
    listResult.append("'RIFX'");
    listResult.append("'AIFF'");

    return listResult;
}

XBinary *XRiff::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XRiff(pDevice);
}

