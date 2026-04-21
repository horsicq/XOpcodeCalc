/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
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
#include "xatarist.h"

XBinary::XCONVERT _TABLE_XAtariST_STRUCTID[] = {
    {XAtariST::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XAtariST::STRUCTID_HEADER, "HEADER", QString("HEADER")},
};

XAtariST::XAtariST(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XBinary(pDevice, bIsImage, nModuleAddress)
{
}

XAtariST::~XAtariST()
{
}

bool XAtariST::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getSize() >= (qint64)sizeof(XATARIST_DEF::HEADER)) {
        quint16 nMagic = read_uint16(0, true);  // Big-endian

        if (nMagic == XATARIST_DEF::MAGIC) {
            bResult = true;
        }
    }

    return bResult;
}

bool XAtariST::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XAtariST xatarist(pDevice, bIsImage, nModuleAddress);

    return xatarist.isValid();
}

bool XAtariST::isExecutable()
{
    return true;
}

XBinary::MODE XAtariST::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XAtariST xatarist(pDevice, bIsImage, nModuleAddress);

    return xatarist.getMode();
}

XATARIST_DEF::HEADER XAtariST::getHeader()
{
    XATARIST_DEF::HEADER result = {};

    result.nMagic = read_uint16(0, true);
    result.nTextSize = read_uint32(2, true);
    result.nDataSize = read_uint32(6, true);
    result.nBssSize = read_uint32(10, true);
    result.nSymbolSize = read_uint32(14, true);
    result.nReserved = read_uint32(18, true);
    result.nFlags = read_uint32(22, true);
    result.nRelocation = read_uint16(26, true);

    return result;
}

QList<XBinary::MAPMODE> XAtariST::getMapModesList()
{
    QList<XBinary::MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

QList<XBinary::FPART> XAtariST::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<FPART> listResult;

    XATARIST_DEF::HEADER header = getHeader();

    qint64 nTotalSize = getSize();
    qint64 nHeaderSize = sizeof(XATARIST_DEF::HEADER);
    qint64 nCurrentOffset = nHeaderSize;

    if (nFileParts & FILEPART_HEADER) {
        FPART partHeader = {};
        partHeader.filePart = FILEPART_HEADER;
        partHeader.nFileOffset = 0;
        partHeader.nFileSize = nHeaderSize;
        partHeader.nVirtualAddress = -1;
        partHeader.sName = tr("Header");
        listResult.append(partHeader);
    }

    if (nFileParts & FILEPART_SEGMENT) {
        // Text segment
        if ((header.nTextSize > 0) && (nCurrentOffset < nTotalSize)) {
            FPART partText = {};
            partText.filePart = FILEPART_SEGMENT;
            partText.nFileOffset = nCurrentOffset;
            partText.nFileSize = qMin((qint64)header.nTextSize, nTotalSize - nCurrentOffset);
            partText.nVirtualAddress = getModuleAddress();
            partText.sName = tr("Text");
            listResult.append(partText);
            nCurrentOffset += partText.nFileSize;
        }

        // Data segment
        if ((header.nDataSize > 0) && (nCurrentOffset < nTotalSize)) {
            FPART partData = {};
            partData.filePart = FILEPART_SEGMENT;
            partData.nFileOffset = nCurrentOffset;
            partData.nFileSize = qMin((qint64)header.nDataSize, nTotalSize - nCurrentOffset);
            partData.nVirtualAddress = getModuleAddress() + header.nTextSize;
            partData.sName = tr("Data");
            listResult.append(partData);
            nCurrentOffset += partData.nFileSize;
        }

        // BSS segment
        if (header.nBssSize > 0) {
            FPART partBss = {};
            partBss.filePart = FILEPART_SEGMENT;
            partBss.nFileOffset = -1;
            partBss.nFileSize = header.nBssSize;
            partBss.nVirtualAddress = getModuleAddress() + header.nTextSize + header.nDataSize;
            partBss.sName = tr("BSS");
            listResult.append(partBss);
        }
    }

    if (nFileParts & FILEPART_OVERLAY) {
        qint64 nCoveredEnd = 0;
        qint32 nNumberOfParts = listResult.count();

        for (qint32 i = 0; i < nNumberOfParts; i++) {
            const FPART &part = listResult.at(i);
            qint64 nPartEnd = part.nFileOffset + part.nFileSize;

            if (nPartEnd > nCoveredEnd) {
                nCoveredEnd = nPartEnd;
            }
        }

        if (nCoveredEnd < nTotalSize) {
            FPART partOverlay = {};
            partOverlay.filePart = FILEPART_OVERLAY;
            partOverlay.nFileOffset = nCoveredEnd;
            partOverlay.nFileSize = nTotalSize - nCoveredEnd;
            partOverlay.nVirtualAddress = -1;
            partOverlay.sName = tr("Overlay");
            listResult.append(partOverlay);
        }
    }

    return listResult;
}

bool XAtariST::_initMemoryMap(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    XATARIST_DEF::HEADER header = getHeader();

    qint64 nTotalSize = getSize();

    pMemoryMap->nModuleAddress = getModuleAddress();
    pMemoryMap->nEntryPointAddress = pMemoryMap->nModuleAddress;
    pMemoryMap->nBinarySize = nTotalSize;
    pMemoryMap->nImageSize = header.nTextSize + header.nDataSize + header.nBssSize;
    pMemoryMap->fileType = getFileType();
    pMemoryMap->mode = getMode();
    pMemoryMap->sArch = getArch();
    pMemoryMap->endian = getEndian();
    pMemoryMap->sType = getTypeAsString();

    return true;
}

XBinary::_MEMORY_MAP XAtariST::getMemoryMap(XBinary::MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_REGIONS;
    }

    if (mapMode == MAPMODE_REGIONS) {
        return _getMemoryMap(FILEPART_SEGMENT | FILEPART_OVERLAY, pPdStruct);
    } else {
        return _getMemoryMap(FILEPART_SEGMENT | FILEPART_OVERLAY, pPdStruct);
    }
}

QString XAtariST::getArch()
{
    return QString("68K");
}

XBinary::MODE XAtariST::getMode()
{
    return MODE_32;
}

XBinary::ENDIAN XAtariST::getEndian()
{
    return ENDIAN_BIG;
}

qint64 XAtariST::getImageSize()
{
    XATARIST_DEF::HEADER header = getHeader();

    return header.nTextSize + header.nDataSize + header.nBssSize;
}

XBinary::FT XAtariST::getFileType()
{
    return FT_ATARIST;
}

qint32 XAtariST::getType()
{
    return TYPE_EXECUTABLE;
}

XBinary::OSNAME XAtariST::getOsName()
{
    return OSNAME_ATARIST;
}

QString XAtariST::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XAtariST_STRUCTID, sizeof(_TABLE_XAtariST_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QString XAtariST::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_EXECUTABLE: sResult = QString("Executable"); break;
    }

    return sResult;
}

QString XAtariST::getFileFormatExtsString()
{
    return QString("tos,ttp,prg");
}
