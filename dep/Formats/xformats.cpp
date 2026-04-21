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
#include "xformats.h"

XFormats::XFormats(QObject *pParent) : XThreadObject(pParent)
{
    m_mode = MODE_UNKNOWN;
    m_fileFormat = XBinary::FT_UNKNOWN;
    m_pDevice = nullptr;
    m_pPdStruct = nullptr;
}

XBinary *XFormats::getClass(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    if (XBinary::checkFileType(XBinary::FT_BINARY, fileType)) return new XBinary(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_COM, fileType)) return new XCOM(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_MSDOS, fileType)) return new XMSDOS(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_NE, fileType)) return new XNE(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_LE, fileType) || XBinary::checkFileType(XBinary::FT_LX, fileType)) return new XLE(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_PE, fileType)) return new XPE(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_ELF, fileType)) return new XELF(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_MACHO, fileType)) return new XMACH(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_AMIGAHUNK, fileType)) return new XAmigaHunk(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_ATARIST, fileType)) return new XAtariST(pDevice, bIsImage, nModuleAddress);
    else if (XBinary::checkFileType(XBinary::FT_PNG, fileType)) return new XPNG(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_JPEG, fileType)) return new XJpeg(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ICO, fileType)) return new XIcon(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_BMP, fileType)) return new XBMP(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_GIF, fileType)) return new XGif(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ICC, fileType)) return new XICC(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_TIFF, fileType)) return new XTiff(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_MP4, fileType)) return new XMP4(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_MP3, fileType)) return new XMP3(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_WAV, fileType)) return new XWAV(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_XM, fileType)) return new XXM(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_AVI, fileType)) return new XAVI(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_WEBP, fileType)) return new XWEBP(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_RIFF, fileType)) return new XRiff(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_JAVACLASS, fileType)) return new XJavaClass(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_PYC, fileType)) return new XPYC(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_TTF, fileType)) return new XTTF(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_DJVU, fileType)) return new XDJVU(pDevice);
    // DER is a generic ASN.1 container; we map it under DOCUMENT if requested
    else if (XBinary::checkFileType(XBinary::FT_DER, fileType)) return new XDER(pDevice);
#ifdef USE_DEX
    else if (XBinary::checkFileType(XBinary::FT_ANDROIDXML, fileType) || XBinary::checkFileType(XBinary::FT_ANDROIDASRC, fileType)) return new XAndroidBinary(pDevice);
#endif
    else if (XBinary::checkFileType(XBinary::FT_TEXT, fileType)) return new XText(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_UTF8, fileType)) return new XText(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_UNICODE, fileType)) return new XText(pDevice);
#ifdef USE_DEX
    else if (XBinary::checkFileType(XBinary::FT_DEX, fileType)) return new XDEX(pDevice);
#endif
#ifdef USE_PDF
    else if (XBinary::checkFileType(XBinary::FT_PDF, fileType)) return new XPDF(pDevice);
#endif
#ifdef USE_ARCHIVE
    else if (XBinary::checkFileType(XBinary::FT_ZIP, fileType)) return new XZip(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_JAR, fileType)) return new XJAR(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_APK, fileType)) return new XAPK(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_IPA, fileType)) return new XIPA(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_APKS, fileType)) return new XAPKS(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_DMG, fileType)) return new XDMG(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_7Z, fileType)) return new XSevenZip(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_CAB, fileType)) return new XCab(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_RAR, fileType)) return new XRar(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_MACHOFAT, fileType)) return new XMACHOFat(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_NPM, fileType)) return new XNPM(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_TAR_GZ, fileType) || XBinary::checkFileType(XBinary::FT_TAR_BZIP2, fileType) ||
             XBinary::checkFileType(XBinary::FT_TAR_LZIP, fileType) || XBinary::checkFileType(XBinary::FT_TAR_LZMA, fileType) ||
             XBinary::checkFileType(XBinary::FT_TAR_LZOP, fileType) || XBinary::checkFileType(XBinary::FT_TAR_XZ, fileType) ||
             XBinary::checkFileType(XBinary::FT_TAR_Z, fileType) || XBinary::checkFileType(XBinary::FT_TAR_ZSTD, fileType)) {
        XTARCOMPRESSED::COMPRESSION_TYPE compressionType = XTARCOMPRESSED::detectCompressionType(pDevice);
        XArchive *pArchive = (XArchive *)XTARCOMPRESSED::getCompressionClassInstance(compressionType, pDevice);

        if (pArchive) {
            return pArchive;
        }

        if (XBinary::checkFileType(XBinary::FT_TAR_BZIP2, fileType)) return new XTAR_BZIP2(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_LZIP, fileType)) return new XTAR_LZIP(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_LZMA, fileType)) return new XTAR_LZMA(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_LZOP, fileType)) return new XTAR_LZOP(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_XZ, fileType)) return new XTAR_XZ(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_Z, fileType)) return new XTAR_COMPRESS(pDevice);
        if (XBinary::checkFileType(XBinary::FT_TAR_ZSTD, fileType)) return new XTAR_ZSTD(pDevice);

        return new XTAR_GZ(pDevice);
    } else if (XBinary::checkFileType(XBinary::FT_GZIP, fileType)) return new XGzip(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ZLIB, fileType)) return new XZlib(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_LHA, fileType)) return new XLHA(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ARJ, fileType)) return new XARJ(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ACE, fileType)) return new XACE(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_CFBF, fileType)) return new XCFBF(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_SZDD, fileType)) return new XSZDD(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_BZIP2, fileType)) return new XBZIP2(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_BROTLI, fileType)) return new XBrotli(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_LZIP, fileType)) return new XLzip(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_TAR, fileType)) return new XTAR(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_XZ, fileType)) return new XXZ(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_AR, fileType)) return new X_Ar(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_CPIO, fileType)) return new XCPIO(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_ISO9660, fileType)) return new XISO9660(pDevice);
    else if (XBinary::checkFileType(XBinary::FT_UDF, fileType)) return new XUDF(pDevice);

    else if (XBinary::checkFileType(XBinary::FT_DOS4G, fileType) || XBinary::checkFileType(XBinary::FT_DOS16M, fileType)) return new XDOS16(pDevice);
#endif
    else {
#ifdef QT_DEBUG
        qDebug() << "XFormats::getClass: Unknown file type" << XBinary::fileTypeIdToString(fileType);
#endif
        XBinary *pBinary = new XBinary(pDevice, bIsImage, nModuleAddress);

        return pBinary;
    }
}

bool XFormats::isValid(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    bResult = pBinary->isValid(pPdStruct);
    delete pBinary;

    return bResult;
}

XBinary::_MEMORY_MAP XFormats::getMemoryMap(XBinary::FT fileType, XBinary::MAPMODE mapMode, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress,
                                            XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    result = pBinary->getMemoryMap(mapMode, pPdStruct);
    delete pBinary;

    return result;
}

XBinary::_MEMORY_MAP XFormats::getMemoryMap(const QString &sFileName, XBinary::MAPMODE mapMode, bool bIsImage, XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        result = getMemoryMap(XFormats::getPrefFileType(&file, true), mapMode, &file, bIsImage, nModuleAddress, pPdStruct);

        file.close();
    }

    return result;
}

QList<XBinary::MAPMODE> XFormats::getMapModesList(XBinary::FT fileType)
{
    QList<XBinary::MAPMODE> listResult = {};

    XBinary *pBinary = XFormats::getClass(fileType, nullptr);
    listResult = pBinary->getMapModesList();
    delete pBinary;

    return listResult;
}

XADDR XFormats::getEntryPointAddress(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    // TODO pMemoryMap !!!
    XADDR nResult = 0;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    nResult = pBinary->getEntryPointAddress();
    delete pBinary;

    return nResult;
}

qint64 XFormats::getEntryPointOffset(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    qint64 nResult = 0;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    nResult = pBinary->_getEntryPointOffset();
    delete pBinary;

    return nResult;
}

bool XFormats::isBigEndian(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    bool bResult = false;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    bResult = pBinary->isBigEndian();
    delete pBinary;

    return bResult;
}

QList<XBinary::FPART> XFormats::getHighlights(XBinary::FT fileType, QIODevice *pDevice, XBinary::HLTYPE hlType, bool bIsImage, XADDR nModuleAddress,
                                              XBinary::PDSTRUCT *pPdStruct)
{
    QList<XBinary::FPART> listResult;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    listResult = pBinary->getHighlights(hlType, pPdStruct);
    delete pBinary;

    return listResult;
}

bool XFormats::isSigned(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    bool bResult = false;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    bResult = pBinary->isSigned();
    delete pBinary;

    return bResult;
}

XBinary::OFFSETSIZE XFormats::getSignOffsetSize(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XBinary::OFFSETSIZE osResult = {};

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    osResult = pBinary->getSignOffsetSize();
    delete pBinary;

    return osResult;
}

XBinary::OFFSETSIZE XFormats::getSignOffsetSize(const QString &sFileName)
{
    XBinary::OFFSETSIZE result = {};

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        result = getSignOffsetSize(XFormats::getPrefFileType(&file, true), &file);

        file.close();
    }

    return result;
}

bool XFormats::isSigned(const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        bResult = isSigned(XFormats::getPrefFileType(&file, true), &file);

        file.close();
    }

    return bResult;
}

QList<XBinary::SYMBOL_RECORD> XFormats::getSymbolRecords(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, XBinary::SYMBOL_TYPE symBolType)
{
    QList<XBinary::SYMBOL_RECORD> listResult;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    XBinary::_MEMORY_MAP memoryMap = pBinary->getMemoryMap();
    listResult = pBinary->getSymbolRecords(&memoryMap, symBolType);
    delete pBinary;

    return listResult;
}

QSet<XBinary::FT> XFormats::getFileTypes(QIODevice *pDevice, bool bExtra, XBinary::PDSTRUCT *pPdStruct)
{
    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    return _getFileTypes(pDevice, bExtra, pPdStruct);
}

bool XFormats::saveAllPEIconsToDirectory(QIODevice *pDevice, const QString &sDirectoryName)
{
    bool bResult = false;

    XPE pe(pDevice);

    if (pe.isValid()) {
        QList<XPE::RESOURCE_RECORD> listResources = pe.getResources(10000);
        QList<XPE::RESOURCE_RECORD> listIcons = pe.getResourceRecords(XPE_DEF::S_RT_GROUP_ICON, -1, &listResources);

        qint32 nNumberOfRecords = listIcons.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            XPE::RESOURCE_RECORD resourceRecord = listIcons.at(i);

            QString sFileName = sDirectoryName + QDir::separator() + QString("%1.ico").arg(XBinary::convertFileNameSymbols(XPE::resourceRecordToString(resourceRecord)));

            XFormats::savePE_ICOToFile(pDevice, &listResources, resourceRecord, sFileName);
        }
    }

    return bResult;
}

bool XFormats::saveAllPECursorsToDirectory(QIODevice *pDevice, const QString &sDirectoryName)
{
    bool bResult = true;

    XPE pe(pDevice);

    if (pe.isValid()) {
        QList<XPE::RESOURCE_RECORD> listResources = pe.getResources(10000);
        QList<XPE::RESOURCE_RECORD> listIcons = pe.getResourceRecords(XPE_DEF::S_RT_GROUP_CURSOR, -1, &listResources);

        qint32 nNumberOfRecords = listIcons.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            XPE::RESOURCE_RECORD resourceRecord = listIcons.at(i);

            QString sFileName = sDirectoryName + QDir::separator() + QString("%1.cur").arg(XBinary::convertFileNameSymbols(XPE::resourceRecordToString(resourceRecord)));

            if (!XFormats::savePE_ICOToFile(pDevice, &listResources, resourceRecord, sFileName)) {
                bResult = false;
            }
        }
    }

    return bResult;
}

bool XFormats::savePE_ICOToFile(QIODevice *pDevice, QList<XPE::RESOURCE_RECORD> *pListResourceRecords, XPE::RESOURCE_RECORD resourceRecord, const QString &sFileName)
{
    bool bResult = false;

    qint32 nChunkType = XPE_DEF::S_RT_ICON;
    qint32 idType = 1;

    if (resourceRecord.irin[0].nID == XPE_DEF::S_RT_GROUP_CURSOR) {
        nChunkType = XPE_DEF::S_RT_CURSOR;
        idType = 2;
    }

    XPE xpe(pDevice);

    if (xpe.isValid()) {
        if (resourceRecord.nSize) {
            SubDevice sd(pDevice, resourceRecord.nOffset, resourceRecord.nSize);

            if (sd.open(QIODevice::ReadOnly)) {
                XIcon icon(&sd);

                if (icon.isValid()) {
                    QList<XPE::RESOURCE_RECORD> listChunkRecords;
                    qint64 nTotalDataSize = 0;

                    QList<XIcon::GRPICONDIRENTRY> listDirectories = icon.getIconGPRDirectories();

                    qint32 nNumberOfRecords = listDirectories.count();

                    for (qint32 i = 0; i < nNumberOfRecords; i++) {
                        XPE::RESOURCE_RECORD _resourceRecord = xpe.getResourceRecord(nChunkType, listDirectories.at(i).nID, pListResourceRecords);

                        listChunkRecords.append(_resourceRecord);

                        nTotalDataSize += _resourceRecord.nSize;
                    }

                    QFile file;
                    file.setFileName(sFileName);

                    if (file.open(QIODevice::ReadWrite)) {
                        file.resize(sizeof(XIcon::ICONDIR) + sizeof(XIcon::ICONDIRENTRY) * nNumberOfRecords + nTotalDataSize);

                        XBinary binaryNew(&file);

                        binaryNew.write_uint16(offsetof(XIcon::ICONDIR, idReserved), 0);
                        binaryNew.write_uint16(offsetof(XIcon::ICONDIR, idType), idType);
                        binaryNew.write_uint16(offsetof(XIcon::ICONDIR, idCount), nNumberOfRecords);

                        qint64 nCurrentTableOffset = sizeof(XIcon::ICONDIR);
                        qint64 nCurrentDataOffset = sizeof(XIcon::ICONDIR) + sizeof(XIcon::ICONDIRENTRY) * nNumberOfRecords;

                        for (qint32 i = 0; i < nNumberOfRecords; i++) {
                            binaryNew.write_uint8(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, bWidth), listDirectories.at(i).bWidth);
                            binaryNew.write_uint8(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, bHeight), listDirectories.at(i).bHeight);
                            binaryNew.write_uint8(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, bColorCount), listDirectories.at(i).bColorCount);
                            binaryNew.write_uint8(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, bReserved), listDirectories.at(i).bReserved);
                            binaryNew.write_uint16(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, wPlanes), listDirectories.at(i).wPlanes);
                            binaryNew.write_uint16(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, wBitCount), listDirectories.at(i).wBitCount);
                            binaryNew.write_uint32(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, dwBytesInRes), listDirectories.at(i).dwBytesInRes);
                            binaryNew.write_uint32(nCurrentTableOffset + offsetof(XIcon::ICONDIRENTRY, dwImageOffset), nCurrentDataOffset);

                            XBinary::copyDeviceMemory(pDevice, listChunkRecords.at(i).nOffset, &file, nCurrentDataOffset, listDirectories.at(i).dwBytesInRes);

                            nCurrentTableOffset += sizeof(XIcon::ICONDIRENTRY);
                            nCurrentDataOffset += listDirectories.at(i).dwBytesInRes;
                        }

                        file.close();
                    }
                }

                sd.close();
            }
        }
    }

    return bResult;
}

QSet<XBinary::FT> XFormats::getFileTypes(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bExtra, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> result;

    if (nOffset >= 0) {
        SubDevice sd(pDevice, nOffset, nSize);

        if (sd.open(QIODevice::ReadOnly)) {
            result = getFileTypes(&sd, bExtra, pPdStruct);

            sd.close();
        }
    }

    return result;
}

QSet<XBinary::FT> XFormats::getFileTypes(const QString &sFileName, bool bExtra, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        stResult = getFileTypes(&file, bExtra, pPdStruct);

        file.close();
    }

    return stResult;
}

QSet<XBinary::FT> XFormats::getFileTypes(QByteArray *pbaData, bool bExtra)
{
    QSet<XBinary::FT> stResult;

    QBuffer buffer;

    buffer.setBuffer(pbaData);

    if (buffer.open(QIODevice::ReadOnly)) {
        stResult = getFileTypes(&buffer, bExtra);

        buffer.close();
    }

    return stResult;
}

XBinary::FT XFormats::getPrefFileType(QIODevice *pDevice, bool bExtra, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stFileTypes = getFileTypes(pDevice, bExtra, pPdStruct);

    return XBinary::_getPrefFileType(&stFileTypes);
}

XBinary::FILEFORMATINFO XFormats::getFileFormatInfo(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct,
                                                    qint64 nOffset, qint64 nSize)
{
    XBinary::FILEFORMATINFO result = {};

    QIODevice *_pDevice = nullptr;
    SubDevice *pSubDevice = nullptr;

    if (nOffset != 0) {
        pSubDevice = new SubDevice(pDevice, nOffset, nSize);
        pSubDevice->open(QIODevice::ReadOnly);
        _pDevice = pSubDevice;
    } else {
        _pDevice = pDevice;
    }

    XBinary *pBinary = XFormats::getClass(fileType, _pDevice, bIsImage, nModuleAddress);
    result = pBinary->getFileFormatInfo(pPdStruct);
    delete pBinary;

    if (pSubDevice) {
        pSubDevice->close();
        delete pSubDevice;
    }

    return result;
}

qint64 XFormats::getFileFormatSize(XBinary::FT fileType, QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct, qint64 nOffset,
                                   qint64 nSize)
{
    qint64 nResult = 0;

    QIODevice *_pDevice = nullptr;
    SubDevice *pSubDevice = nullptr;

    if (nOffset != 0) {
        pSubDevice = new SubDevice(pDevice, nOffset, nSize);
        pSubDevice->open(QIODevice::ReadOnly);
        _pDevice = pSubDevice;
    } else {
        _pDevice = pDevice;
    }

    XBinary *pBinary = XFormats::getClass(fileType, _pDevice, bIsImage, nModuleAddress);
    nResult = pBinary->getFileFormatSize(pPdStruct);
    delete pBinary;

    if (pSubDevice) {
        pSubDevice->close();
        delete pSubDevice;
    }

    return nResult;
}

QList<XBinary::DATA_HEADER> XFormats::getDataHeaders(XBinary::FT fileType, QIODevice *pDevice, const XBinary::DATA_HEADERS_OPTIONS &dataHeadersOptions, bool bIsImage,
                                                     XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct)
{
    QList<XBinary::DATA_HEADER> listResult;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    listResult = pBinary->getDataHeaders(dataHeadersOptions, pPdStruct);
    delete pBinary;

    return listResult;
}

QList<XBinary::XFHEADER> XFormats::getXFHeaders(QIODevice *pDevice, const QString &sTag, bool bIsImage, XADDR nModuleAddress, XBinary::PDSTRUCT *pPdStruct)
{
    QList<XBinary::XFHEADER> listResult;

    // Tag format: Offset::FileType::StructID::Type[::Count]
    // With '#' separator:
    // TAG                  -> no parent (bIsParent=false)
    // TAG#                 -> with parent (bIsParent=true)
    // TAG1#TAG2[#TAG3...]  -> TAG1 with parent, filter results matching TAG2, TAG3, etc.
    // Filter tags can be partial: 5594::ZIP::LOCALFILEHEADER (offset+fileType+structID)
    // or full: 5594::ZIP::LOCALFILEHEADER::TABLE::6

    QStringList listSegments = sTag.split("#");

    if (listSegments.isEmpty()) {
        return listResult;
    }

    QString sRootTag = listSegments.at(0);
    bool bIsParent = false;
    QStringList listFilterTags;
    qint32 nSegmentCount = listSegments.count();

    if (nSegmentCount == 1) {
        // No '#' found -> no parent flag
        bIsParent = false;
    } else if ((nSegmentCount == 2) && listSegments.at(1).isEmpty()) {
        // Trailing '#' -> parent flag, no filter
        bIsParent = true;
    } else {
        // Multiple segments -> parent flag + filter on remaining segments
        bIsParent = true;
        for (qint32 i = 1; i < nSegmentCount; i++) {
            if (!listSegments.at(i).isEmpty()) {
                listFilterTags.append(listSegments.at(i));
            }
        }
    }

    // Parse root tag: Offset::FileTypeFt::StructFt::TypeName[::Count]
    QStringList listParts = sRootTag.split("::");

    if (listParts.count() >= 4) {
        QString sOffset = listParts.at(0);
        QString sFileTypeFt = listParts.at(1);
        QString sStructFt = listParts.at(2);
        QString sTypeName = listParts.at(3);

        XBinary::FT fileType = XBinary::ftStringToFileTypeId(sFileTypeFt);

        bool bOk = false;
        qint64 nOffset = sOffset.toLongLong(&bOk, 16);

        if ((fileType != XBinary::FT_UNKNOWN) && bOk) {
            XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);

            if (pBinary) {
                quint32 nStructID = pBinary->ftStringToStructID(sStructFt);

                XBinary::XFTYPE xfType = XBinary::XFTYPE_UNKNOWN;
                if (sTypeName == "HEADER") {
                    xfType = XBinary::XFTYPE_HEADER;
                } else if (sTypeName == "TABLE") {
                    xfType = XBinary::XFTYPE_TABLE;
                }

                XBinary::_MEMORY_MAP memoryMap = pBinary->getMemoryMap(XBinary::MAPMODE_UNKNOWN, pPdStruct);

                XBinary::XLOC xLoc = {};
                xLoc.locType = XBinary::LT_OFFSET;
                xLoc.nLocation = nOffset;

                XBinary::XFSTRUCT xfStruct = {};
                xfStruct.pMemoryMap = &memoryMap;
                xfStruct.fileType = fileType;
                xfStruct.nStructID = nStructID;
                xfStruct.xLoc = xLoc;
                xfStruct.xfType = xfType;
                xfStruct.bIsParent = bIsParent;
                xfStruct.nCount = -1;

                if (listParts.count() >= 5) {
                    bool bCountOk = false;
                    qint32 nCount = listParts.at(4).toInt(&bCountOk);
                    if (bCountOk) {
                        xfStruct.nCount = nCount;
                    }
                }

                QList<XBinary::XFHEADER> listAllHeaders = pBinary->getXFHeaders(xfStruct, pPdStruct);

                if (listFilterTags.isEmpty()) {
                    listResult = listAllHeaders;
                } else {
                    // Pre-parse filter criteria
                    qint32 nFilterCount = listFilterTags.count();
                    QList<qint32> listFilterPartCounts;
                    QList<XBinary::FT> listFilterFileTypes;
                    QList<quint32> listFilterStructIDs;
                    QList<XBinary::XFTYPE> listFilterXfTypes;
                    QList<qint64> listFilterOffsets;

                    for (qint32 i = 0; i < nFilterCount; i++) {
                        QStringList listFilterParts = listFilterTags.at(i).split("::");
                        qint32 nFilterPartCount = listFilterParts.count();

                        qint64 nFilterOffset = 0;
                        XBinary::FT filterFileType = XBinary::FT_UNKNOWN;
                        quint32 nFilterStructID = 0;
                        XBinary::XFTYPE filterXfType = XBinary::XFTYPE_UNKNOWN;

                        if (nFilterPartCount >= 1) {
                            bool bFilterOk = false;
                            nFilterOffset = listFilterParts.at(0).toLongLong(&bFilterOk, 16);
                            if (!bFilterOk) {
                                nFilterPartCount = 0;
                            }
                        }
                        if (nFilterPartCount >= 2) {
                            filterFileType = XBinary::ftStringToFileTypeId(listFilterParts.at(1));
                        }
                        if (nFilterPartCount >= 3) {
                            nFilterStructID = pBinary->ftStringToStructID(listFilterParts.at(2));
                        }
                        if (nFilterPartCount >= 4) {
                            if (listFilterParts.at(3) == "HEADER") {
                                filterXfType = XBinary::XFTYPE_HEADER;
                            } else if (listFilterParts.at(3) == "TABLE") {
                                filterXfType = XBinary::XFTYPE_TABLE;
                            }
                        }

                        listFilterPartCounts.append(nFilterPartCount);
                        listFilterFileTypes.append(filterFileType);
                        listFilterStructIDs.append(nFilterStructID);
                        listFilterXfTypes.append(filterXfType);
                        listFilterOffsets.append(nFilterOffset);
                    }

                    // Filter headers against pre-parsed criteria
                    qint32 nAllCount = listAllHeaders.count();

                    for (qint32 i = 0; i < nAllCount; i++) {
                        const XBinary::XFHEADER &header = listAllHeaders.at(i);

                        for (qint32 j = 0; j < nFilterCount; j++) {
                            bool bMatch = true;
                            qint32 nFPC = listFilterPartCounts.at(j);

                            if (bMatch && (nFPC >= 1)) {
                                if (header.xLoc.nLocation != listFilterOffsets.at(j)) {
                                    bMatch = false;
                                }
                            }
                            if (bMatch && (nFPC >= 2)) {
                                if (header.fileType != listFilterFileTypes.at(j)) {
                                    bMatch = false;
                                }
                            }
                            if (bMatch && (nFPC >= 3)) {
                                if (header.structID != listFilterStructIDs.at(j)) {
                                    bMatch = false;
                                }
                            }
                            if (bMatch && (nFPC >= 4)) {
                                if (header.xfType != listFilterXfTypes.at(j)) {
                                    bMatch = false;
                                }
                            }

                            if (bMatch) {
                                listResult.append(header);
                                break;
                            }
                        }
                    }
                }

                delete pBinary;
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XFormats::getFileParts(XBinary::FT fileType, QIODevice *pDevice, quint32 nFileParts, qint32 nLimit, bool bIsImage, XADDR nModuleAddress,
                                             XBinary::PDSTRUCT *pPdStruct)
{
    QList<XBinary::FPART> listResult;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    listResult = pBinary->getFileParts(nFileParts, nLimit, pPdStruct);
    delete pBinary;

    return listResult;
}

QList<XBinary::ARCHIVERECORD> XFormats::getArchiveRecords(XBinary::FT fileType, QIODevice *pDevice, qint32 nLimit, bool bIsImage, XADDR nModuleAddress,
                                                          XBinary::PDSTRUCT *pPdStruct)
{
    QList<XBinary::ARCHIVERECORD> listResult;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    listResult = pBinary->getArchiveRecords(nLimit, pPdStruct);
    delete pBinary;

    return listResult;
}

qint32 XFormats::getDataRecordValues(XBinary::FT fileType, QIODevice *pDevice, const XBinary::DATA_RECORDS_OPTIONS &dataRecordsOptions,
                                     QList<XBinary::DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles, bool bIsImage, XADDR nModuleAddress,
                                     XBinary::PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    XBinary *pBinary = XFormats::getClass(fileType, pDevice, bIsImage, nModuleAddress);
    nResult = pBinary->getDataRecordValues(dataRecordsOptions, pListDataRecords, pListTitles, pPdStruct);
    delete pBinary;

    return nResult;
}

QString XFormats::getFileFormatExtsString(XBinary::FT fileType)
{
    QString sResult;

    XBinary *pBinary = XFormats::getClass(fileType, nullptr);
    sResult = pBinary->getFileFormatExtsString();
    delete pBinary;

    return sResult;
}

bool XFormats::isArchive(XBinary::FT fileType)
{
    bool bResult = false;

    XBinary *pBinary = XFormats::getClass(fileType, nullptr);
    bResult = pBinary->isArchive();
    delete pBinary;

    return bResult;
}

bool XFormats::isExecutable(XBinary::FT fileType)
{
    bool bResult = false;

    XBinary *pBinary = XFormats::getClass(fileType, nullptr);
    bResult = pBinary->isExecutable();
    delete pBinary;

    return bResult;
}

QString XFormats::getXFHeaderStructName(const XBinary::XFHEADER &header)
{
    QString sResult;

    XBinary *pBinary = XFormats::getClass(header.fileType, nullptr);

    if (pBinary) {
        sResult = pBinary->structIDToString(header.structID);
        delete pBinary;
    }

    return sResult;
}
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypes(QIODevice *pDevice, XArchive::RECORD *pRecord, bool bExtra)
{
    QSet<XBinary::FT> stResult;

    // QByteArray baData = XArchives::decompress(pDevice, pRecord, 0, 0, 0x200);

    // stResult = XFormats::getFileTypes(&baData, bExtra);

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesZIP(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)  // TODO

    QSet<XBinary::FT> stResult;

    // XBinary::FT fileType = XZip::_getFileType(pDevice, pListRecords, true);
    stResult.insert(XBinary::FT_ZIP);

    if (XAPK::isValid(pListRecords, pPdStruct)) {
        stResult.insert(XBinary::FT_APK);
        stResult.insert(XBinary::FT_JAR);
    } else if (XIPA::isValid(pListRecords, pPdStruct)) {
        stResult.insert(XBinary::FT_IPA);
    } else if (XJAR::isValid(pListRecords, pPdStruct)) {
        stResult.insert(XBinary::FT_JAR);
    }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesZIP(QIODevice *pDevice, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)  // TODO

    QSet<XBinary::FT> stResult;

    XZip xzip(pDevice);

    if (xzip.isValid(pPdStruct)) {
        stResult.insert(XBinary::FT_ZIP);

        qint64 nECDOffset = xzip.findECDOffset(pPdStruct);

        if (xzip.isAPK(nECDOffset, pPdStruct)) {
            stResult.insert(XBinary::FT_APK);
            stResult.insert(XBinary::FT_JAR);
        } else if (xzip.isIPA(nECDOffset, pPdStruct)) {
            stResult.insert(XBinary::FT_IPA);
        } else if (xzip.isJAR(nECDOffset, pPdStruct)) {
            stResult.insert(XBinary::FT_JAR);
        }
    }

    // XBinary::FT fileType = XZip::_getFileType(pDevice, pListRecords, true);
    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesTGZ(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)  // TODO

    QSet<XBinary::FT> stResult;

    stResult.insert(XBinary::FT_TAR_GZ);

    if (XNPM::isValid(pListRecords, pPdStruct)) {
        stResult.insert(XBinary::FT_NPM);
    }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesTBZIP2(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QSet<XBinary::FT> stResult;

    // if (pListRecords->count()) {
    //     const XArchive::RECORD &record = pListRecords->at(0);
    //     QByteArray baData = XArchives::decompress(pDevice, &record, nullptr, 0, 0x200);

    //     QSet<XBinary::FT> stFileTypes = getFileTypes(&baData, true);

    //     if (stFileTypes.contains(XBinary::FT_TAR)) {
    //         stResult.insert(XBinary::FT_TAR_BZIP2);
    //     }
    // }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesTXZ(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QSet<XBinary::FT> stResult;

    // if (pListRecords->count()) {
    //     const XArchive::RECORD &record = pListRecords->at(0);
    //     QByteArray baData = XArchives::decompress(pDevice, &record, nullptr, 0, 0x200);

    //     QSet<XBinary::FT> stFileTypes = getFileTypes(&baData, true);

    //     if (stFileTypes.contains(XBinary::FT_TAR)) {
    //         stResult.insert(XBinary::FT_TAR_XZ);
    //     }
    // }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesTLZIP(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    QSet<XBinary::FT> stResult;

    // if (pListRecords->count()) {
    //     const XArchive::RECORD &record = pListRecords->at(0);
    //     QByteArray baData = XArchives::decompress(pDevice, &record, nullptr, 0, 0x200);

    //     QSet<XBinary::FT> stFileTypes = getFileTypes(&baData, true);

    //     if (stFileTypes.contains(XBinary::FT_TAR)) {
    //         stResult.insert(XBinary::FT_TAR_LZIP);
    //     }
    // }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesAR(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)  // TODO

    QSet<XBinary::FT> stResult;

    stResult.insert(XBinary::FT_AR);

    if (XDEB::isValid(pListRecords, pPdStruct)) {
        stResult.insert(XBinary::FT_DEB);
    }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesGZIP(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stResult;

    // if (pListRecords->count()) {
    //     const XArchive::RECORD &record = pListRecords->at(0);
    //     QByteArray baData = XArchives::decompress(pDevice, &record, pPdStruct, 0, 0x200);

    //     QSet<XBinary::FT> _ft = getFileTypes(&baData, true);

    //     if (_ft.contains(XBinary::FT_TAR)) {
    //         XTAR_GZ xtarGz(pDevice);
    //         QList<XArchive::RECORD> listArchiveRecords = xtarGz.getRecords(20000, pPdStruct);

    //         stResult += getFileTypesTGZ(pDevice, &listArchiveRecords, pPdStruct);
    //     }
    // }

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesBZIP2(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stResult;

    stResult += getFileTypesTBZIP2(pDevice, pListRecords, pPdStruct);

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesXZ(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stResult;

    stResult += getFileTypesTXZ(pDevice, pListRecords, pPdStruct);

    return stResult;
}
#endif
#ifdef USE_ARCHIVE
QSet<XBinary::FT> XFormats::getFileTypesLZIP(QIODevice *pDevice, QList<XArchive::RECORD> *pListRecords, XBinary::PDSTRUCT *pPdStruct)
{
    QSet<XBinary::FT> stResult;

    // stResult += getFileTypesTLZIP(pDevice, pListRecords, pPdStruct);

    return stResult;
}
#endif

#ifdef USE_ARCHIVE
static XBinary::FT _compressionTypeToTarFT(XTARCOMPRESSED::COMPRESSION_TYPE compressionType)
{
    XBinary::FT result = XBinary::FT_UNKNOWN;

    if (compressionType == XTARCOMPRESSED::COMPRESSION_GZIP) {
        result = XBinary::FT_TAR_GZ;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_BZIP2) {
        result = XBinary::FT_TAR_BZIP2;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_XZ) {
        result = XBinary::FT_TAR_XZ;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_LZMA) {
        result = XBinary::FT_TAR_LZMA;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_ZSTD) {
        result = XBinary::FT_TAR_ZSTD;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_COMPRESS) {
        result = XBinary::FT_TAR_Z;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_LZIP) {
        result = XBinary::FT_TAR_LZIP;
    } else if (compressionType == XTARCOMPRESSED::COMPRESSION_LZOP) {
        result = XBinary::FT_TAR_LZOP;
    }

    return result;
}
#endif

QSet<XBinary::FT> XFormats::_getFileTypes(QIODevice *pDevice, bool bExtra, XBinary::PDSTRUCT *pPdStruct)
{
#ifdef QT_DEBUG
    qDebug("XFormats::_getFileTypes(): Timer started");
    QElapsedTimer timer;
    timer.start();
#endif
    QSet<XBinary::FT> stResult;

    QString sFileTypes = pDevice->property("filetypes").toString();

    if (sFileTypes.isEmpty()) {
        // No cached file types, proceed with detection
        stResult.insert(XBinary::FT_BINARY);

        if (XMSDOS::isValid(pDevice, false, -1, pPdStruct)) {
            stResult.insert(XBinary::FT_MSDOS);

            if (XPE::isValid(pDevice, false, -1, pPdStruct)) {
                stResult.insert(XBinary::FT_PE);

                XPE xpe(pDevice, false, -1);

                if (xpe.isValid(pPdStruct)) {
                    stResult.insert(xpe.getFileType());
                }
            } else if (XNE::isValid(pDevice, false, -1, pPdStruct)) {
                stResult.insert(XBinary::FT_NE);
            } else if (XLE::isValid(pDevice, false, -1, pPdStruct)) {
                stResult.insert(XBinary::FT_LE);

                XLE xle(pDevice, false, -1);

                if (xle.isValid(pPdStruct)) {
                    stResult.insert(xle.getFileType());
                }
            }
#ifdef USE_ARCHIVE
            if (XDOS16::isValid(pDevice, pPdStruct)) {
                XDOS16 xdos16(pDevice);
                if (xdos16.isValid(pPdStruct)) {
                    stResult.insert(xdos16.getFileType());
                }
            }
#endif
        } else if (XELF::isValid(pDevice, false, -1, pPdStruct)) {
            stResult.insert(XBinary::FT_ELF);

            XELF xelf(pDevice, false, -1);

            if (xelf.isValid(pPdStruct)) {
                stResult.insert(xelf.getFileType());
            }
        } else if (XMACH::isValid(pDevice, false, -1, pPdStruct)) {
            stResult.insert(XBinary::FT_MACHO);

            XMACH xmach(pDevice);

            if (xmach.isValid(pPdStruct)) {
                stResult.insert(xmach.getFileType());
            }
        }

        if (stResult.size() <= 1) {
            if (XAmigaHunk::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_AMIGAHUNK);
            }
        }

        if (stResult.size() <= 1) {
            if (XAtariST::isValid(pDevice, false, -1, pPdStruct)) {
                stResult.insert(XBinary::FT_ATARIST);
            }
        }

#ifdef USE_PDF
        if (stResult.size() <= 1) {
            if (XPDF::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_PDF);
            }
        }
#endif
#ifdef USE_DEX
        if (stResult.size() <= 1) {
            if (XDEX::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_DEX);
            }
        }
#endif
#ifdef USE_ARCHIVE
        if (stResult.size() <= 1) {
            if (XZip::isValid(pDevice, pPdStruct)) {
                XZip xzip(pDevice);
                if (xzip.isValid(pPdStruct)) {
                    stResult.insert(XBinary::FT_ARCHIVE);
                    stResult.insert(XBinary::FT_ZIP);
                    qint64 nECDOffset = xzip.findECDOffset(pPdStruct);

                    if (xzip.isAPK(nECDOffset, pPdStruct)) {
                        stResult.insert(XBinary::FT_APK);
                        stResult.insert(XBinary::FT_JAR);
                    } else if (xzip.isIPA(nECDOffset, pPdStruct)) {
                        stResult.insert(XBinary::FT_IPA);
                    } else if (xzip.isJAR(nECDOffset, pPdStruct)) {
                        stResult.insert(XBinary::FT_JAR);
                    }
                }
            } else if (X_Ar::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_AR);
                // TODO DEB
            } else if (XGzip::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_GZIP);
            } else if (XBZIP2::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_BZIP2);
            } else if (XXZ::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_XZ);
            } else if (XLzip::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_LZIP);
            } else if (XTAR::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_TAR);
            } else if (XSevenZip::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_7Z);
            } else if (XRar::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_RAR);
            }
        }

        if (bExtra && (stResult.size() <= 1)) {
            if (XZlib::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ZLIB);
            } else if (XLHA::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_LHA);
            } else if (XCab::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_CAB);
            } else if (XLzo::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_LZO);
            } else if (XCompressZ::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_COMPRESS);
            } else if (XCPIO::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_CPIO);
            } else if (XMiniDump::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_MINIDUMP);
            } else if (XDMG::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_DMG);
            } else if (XARJ::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ARJ);
            } else if (XACE::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ACE);
            } else if (XSEAARC::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ARC);
            } else if (XSZDD::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_SZDD);
            } else if (XZstd::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ZSTD);
            } else if (XCFBF::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_CFBF);
            } else if (XISO9660::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_ISO9660);
            } else if (XUDF::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_UDF);
            } else if (XMACHOFat::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_MACHOFAT);
            }

            if (XFREEARC::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_ARCHIVE);
                stResult.insert(XBinary::FT_FREEARC);
            }
        }
#endif

        if (bExtra && (stResult.size() <= 1)) {
            if (XPNG::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_PNG);
            } else if (XJpeg::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_JPEG);
            } else if (XGif::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_GIF);
            } else if (XBMP::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_BMP);
            } else if (XTiff::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_TIFF);
            } else if (XIcon::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                XIcon xicon(pDevice);
                stResult.insert(xicon.getFileType());  // FT_ICO or FT_CUR
            } else if (XICC::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_ICC);
            }
        }

        if (bExtra && (stResult.size() <= 1)) {
            if (XMP3::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_AUDIO);
                stResult.insert(XBinary::FT_MP3);
            } else if (XMP4::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_VIDEO);
                stResult.insert(XBinary::FT_MP4);
            } else if (XXM::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_AUDIO);
                stResult.insert(XBinary::FT_XM);
            } else if (XAVI::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_RIFF);
                stResult.insert(XBinary::FT_VIDEO);
                stResult.insert(XBinary::FT_AVI);
            } else if (XWEBP::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_RIFF);
                stResult.insert(XBinary::FT_IMAGE);
                stResult.insert(XBinary::FT_WEBP);
            } else if (XWAV::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_RIFF);
                stResult.insert(XBinary::FT_AUDIO);
                stResult.insert(XBinary::FT_WAV);
            } else if (XRiff::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_RIFF);
            }
        }

        if (bExtra && (stResult.size() <= 1)) {
#ifdef USE_DEX
            if (XAndroidBinary::isValid(pDevice, pPdStruct)) {
                XAndroidBinary xandroid(pDevice);
                stResult.insert(xandroid.getFileType());  // FT_ANDROIDXML or FT_ANDROIDASRC
            } else
#endif
            if (XDER::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_DOCUMENT);
                stResult.insert(XBinary::FT_DER);
            } else if (XJavaClass::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_JAVACLASS);
            } else if (XPYC::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_PYC);
            } else if (XTTF::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_TTF);
            } else if (XDJVU::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_DJVU);
            }
        }

        if (bExtra) {
            if (XText::isValid(pDevice, pPdStruct)) {
                stResult.insert(XBinary::FT_TEXT);
                XText xtext(pDevice);
                XText::TEXT_TYPE textType = xtext.detectTextType();

                if (textType == XText::TEXT_TYPE_UTF8 || textType == XText::TEXT_TYPE_UTF8_BOM) {
                    stResult.insert(XBinary::FT_UTF8);
                } else if (textType == XText::TEXT_TYPE_UTF16_LE || textType == XText::TEXT_TYPE_UTF32_LE) {
                    stResult.insert(XBinary::FT_UNICODE);
                    stResult.insert(XBinary::FT_UNICODE_LE);
                } else if (textType == XText::TEXT_TYPE_UTF16_BE || textType == XText::TEXT_TYPE_UTF32_BE) {
                    stResult.insert(XBinary::FT_UNICODE);
                    stResult.insert(XBinary::FT_UNICODE_BE);
                } else {
                    stResult.insert(XBinary::FT_PLAINTEXT);
                }

                // Fix: GIF can trigger text detection
                if (stResult.contains(XBinary::FT_GIF) && stResult.contains(XBinary::FT_TEXT)) {
                    stResult.remove(XBinary::FT_GIF);
                }
            }
        }

        if ((stResult.count() <= 1) || (stResult.contains(XBinary::FT_PLAINTEXT))) {
            if (XCOM::isValid(pDevice, false, -1, pPdStruct)) {
                if (XBinary::getDeviceFileSuffix(pDevice).toUpper() == "COM") {
                    stResult.insert(XBinary::FT_COM);
                }
            }
        }
    } else {
        // Cached file types available, return them
        stResult = XBinary::stringToFileTypes(sFileTypes);
    }

#ifdef QT_DEBUG
    qint64 nElapsed = timer.elapsed();
    qDebug("XFormats::_getFileTypes(): time=%lld ms", nElapsed);  // Calculated in milisecounds
    qDebug("XFormats::_getFileTypes(): Timer stopped");
#endif

    return stResult;
}

void XFormats::setData(MODE mode, XBinary::FT fileFormat, QIODevice *pDevice, QString sFolderName, XBinary::PDSTRUCT *pPdStruct)
{
    m_mode = mode;
    m_fileFormat = fileFormat;
    m_pDevice = pDevice;
    m_sFolderName = sFolderName;
    m_pPdStruct = pPdStruct;
}

void XFormats::process()
{
    if (m_mode == MODE_UNPACKDEVICETOFOLDER) {
        if (m_pDevice) {
            if (!unpackDeviceToFolder(m_fileFormat, m_pDevice, m_sFolderName, m_pPdStruct)) {
                emit errorMessage(tr("Cannot unpack"));
            }
        }
    }
}

bool XFormats::unpackDeviceToFolder(XBinary::FT fileType, QIODevice *pDevice, QString sFolderName, XBinary::PDSTRUCT *pPdStruct)
{
    if (fileType == XBinary::FT_UNKNOWN) {
        fileType = XFormats::getPrefFileType(pDevice, true);
    }

    bool bResult = false;

    bool bDirectory = XBinary::isDirectoryExists(sFolderName);

    if (!bDirectory) {
        bDirectory = XBinary::createDirectory(sFolderName);
    }

    if (bDirectory) {
        XBinary *pBinary = getClass(fileType, pDevice);

        if (pBinary) {
            QMap<XBinary::UNPACK_PROP, QVariant> mapProperties;
            bResult = pBinary->unpackToFolder(sFolderName, mapProperties, pPdStruct);
            delete pBinary;
        }
    }

    return bResult;
}

bool XFormats::extractArchiveRecordsToFolder(QList<XBinary::ARCHIVERECORD> *pListRecords, QIODevice *pDevice, QString sFolderName, XBinary::PDSTRUCT *pPdStruct)
{
#ifdef QT_DEBUG
    qDebug("XFormats::extractArchiveRecordsToFolder: Starting extraction to %s", sFolderName.toLatin1().data());
#endif
    bool bResult = false;

    qint32 nNumberOfRecords = pListRecords->count();
#ifdef QT_DEBUG
    qDebug("XFormats::extractArchiveRecordsToFolder: Number of records=%d", nNumberOfRecords);
#endif

    if (nNumberOfRecords > 0) {
#ifdef USE_ARCHIVE
        XDecompress xDecompress;
        connect(&xDecompress, &XDecompress::errorMessage, this, &XThreadObject::errorMessage);
        connect(&xDecompress, &XDecompress::warningMessage, this, &XThreadObject::warningMessage);
        connect(&xDecompress, &XDecompress::infoMessage, this, &XThreadObject::infoMessage);
#endif

        if (XBinary::createDirectory(sFolderName)) {
#ifdef QT_DEBUG
            qDebug("XFormats::extractArchiveRecordsToFolder: Directory created successfully");
#endif
            bResult = true;
            qint32 nGlobalIndex = XBinary::getFreeIndex(pPdStruct);
            XBinary::setPdStructInit(pPdStruct, nGlobalIndex, nNumberOfRecords);

            QString sCanonicalRoot = QDir::cleanPath(QDir(sFolderName).absolutePath());

            for (qint32 i = 0; (i < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                QString sPrefName = pListRecords->at(i).mapProperties.value(XBinary::FPART_PROP_ORIGINALNAME).toString();
#ifdef QT_DEBUG
                qDebug("XFormats::extractArchiveRecordsToFolder: Processing record %d/%d: %s", i + 1, nNumberOfRecords, sPrefName.toLatin1().data());
#endif

                XBinary::setPdStructStatus(pPdStruct, nGlobalIndex, sPrefName);

                QString sResultFileName = QDir::cleanPath(sFolderName + QDir::separator() + sPrefName);

                if (!sResultFileName.startsWith(sCanonicalRoot + "/")) {
#ifdef QT_DEBUG
                    qDebug("XFormats::extractArchiveRecordsToFolder: Path traversal detected for %s", sPrefName.toLatin1().data());
#endif
                    emit errorMessage(QString("%1: %2").arg(tr("Path traversal detected"), sPrefName));
                    bResult = false;
                    XBinary::setPdStructCurrentIncrement(pPdStruct, nGlobalIndex);
                    continue;
                }

                QFileInfo fi(sResultFileName);
                if (XBinary::createDirectory(fi.absolutePath())) {
                    QFile file;
                    file.setFileName(sResultFileName);

                    if (file.open(QIODevice::ReadWrite)) {
                        const XBinary::ARCHIVERECORD &archiveRecord = pListRecords->at(i);

#ifdef USE_ARCHIVE
                        QMap<XBinary::UNPACK_PROP, QVariant> mapUnpackProperties;

                        if (xDecompress.decompressArchiveRecord(archiveRecord, pDevice, &file, mapUnpackProperties, pPdStruct)) {
                        } else {
#ifdef QT_DEBUG
                            qDebug() << "Cannot decompress" << sPrefName;
#endif
                            emit errorMessage(QString("%1: %2").arg(tr("Cannot decompress"), sPrefName));
                            bResult = false;
                        }
#endif

                        file.close();
#ifdef QT_DEBUG
                        qDebug("XFormats::extractArchiveRecordsToFolder: File closed successfully");
#endif
                    } else {
#ifdef QT_DEBUG
                        qDebug("XFormats::extractArchiveRecordsToFolder: Cannot create file %s", sResultFileName.toLatin1().data());
#endif
                        emit errorMessage(QString("%1: %2").arg(tr("Cannot create"), sResultFileName));
                        bResult = false;
                    }
                } else {
#ifdef QT_DEBUG
                    qDebug("XFormats::extractArchiveRecordsToFolder: Cannot create directory %s", fi.absolutePath().toLatin1().data());
#endif
                    emit errorMessage(QString("%1: %2").arg(tr("Cannot create"), fi.absolutePath()));
                    bResult = false;
                }

                XBinary::setPdStructCurrentIncrement(pPdStruct, nGlobalIndex);
            }

            XBinary::setPdStructFinished(pPdStruct, nGlobalIndex);
#ifdef QT_DEBUG
            qDebug("XFormats::extractArchiveRecordsToFolder: Extraction completed, bResult=%d", bResult);
#endif
        } else {
#ifdef QT_DEBUG
            qDebug("XFormats::extractArchiveRecordsToFolder: Cannot create folder %s", sFolderName.toLatin1().data());
#endif
            emit errorMessage(QString("%1: %2").arg(tr("Cannot create"), sFolderName));
            bResult = false;
        }
    }

#ifdef QT_DEBUG
    qDebug("XFormats::extractArchiveRecordsToFolder: Returning bResult=%d", bResult);
#endif
    return bResult;
}

bool XFormats::packFolderToDevice(XBinary::FT fileType, QIODevice *pDevice, const QMap<XBinary::PACK_PROP, QVariant> &mapProperties, const QString &sFolderName,
                                  XBinary::PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (XBinary::isDirectoryExists(sFolderName)) {
        XBinary *pBinary = getClass(fileType, pDevice);

        if (pBinary) {
            bResult = pBinary->packFolderToDevice(pDevice, mapProperties, sFolderName, pPdStruct);
            delete pBinary;
        }
    }

    return bResult;
}

#ifdef QT_GUI_LIB
XBinary::FT XFormats::setFileTypeComboBox(XBinary::FT fileType, QIODevice *pDevice, QComboBox *pComboBox, XBinary::TL_OPTION tlOption)
{
    const bool bBlocked1 = pComboBox->blockSignals(true);

    QList<XBinary::FT> listFileTypes;

    if (fileType != XBinary::FT_REGION) {
        QSet<XBinary::FT> stFileType = XFormats::getFileTypes(pDevice, true);

        listFileTypes = XBinary::_getFileTypeListFromSet(stFileType, tlOption);
    } else {
        listFileTypes.append(XBinary::FT_REGION);
    }

    pComboBox->clear();

    qint32 nNumberOfListTypes = listFileTypes.count();

    for (qint32 i = 0; i < nNumberOfListTypes; i++) {
        XBinary::FT _fileType = listFileTypes.at(i);
        pComboBox->addItem(XBinary::fileTypeIdToString(_fileType), _fileType);
    }

    if (nNumberOfListTypes) {
        if (fileType == XBinary::FT_UNKNOWN) {
            pComboBox->setCurrentIndex(nNumberOfListTypes - 1);
        } else {
            setComboBoxCurrent(pComboBox, fileType);
        }
    }

    pComboBox->blockSignals(bBlocked1);

    return (XBinary::FT)(pComboBox->currentData().toUInt());
}
#endif
#ifdef QT_GUI_LIB
XBinary::FT XFormats::setFileTypeComboBox(XBinary::FT fileType, const QString &sFileName, QComboBox *pComboBox, XBinary::TL_OPTION tlOption)
{
    XBinary::FT result = XBinary::FT_UNKNOWN;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        result = setFileTypeComboBox(fileType, &file, pComboBox, tlOption);

        file.close();
    }

    return result;
}
#endif
#ifdef QT_GUI_LIB
QVariant XFormats::setComboBoxCurrent(QComboBox *pComboBox, QVariant varValue)
{
    QVariant varResult;

    const bool bBlocked1 = pComboBox->blockSignals(true);

    qint32 nNumberOfItems = pComboBox->count();

    for (qint32 i = 0; i < nNumberOfItems; i++) {
        if (pComboBox->itemData(i) == varValue) {
            pComboBox->setCurrentIndex(i);

            varResult = varValue;

            break;
        }
    }

    pComboBox->blockSignals(bBlocked1);

    return varResult;
}

#endif
#ifdef QT_GUI_LIB
XBinary::ENDIAN XFormats::setEndiannessComboBox(QComboBox *pComboBox, XBinary::ENDIAN endian)
{
    const bool bBlocked1 = pComboBox->blockSignals(true);

    pComboBox->clear();

    pComboBox->addItem(XBinary::endianToString(XBinary::ENDIAN_LITTLE), XBinary::ENDIAN_LITTLE);
    pComboBox->addItem(XBinary::endianToString(XBinary::ENDIAN_BIG), XBinary::ENDIAN_BIG);

    XBinary::ENDIAN result = (XBinary::ENDIAN)setComboBoxCurrent(pComboBox, endian).toUInt();

    pComboBox->blockSignals(bBlocked1);

    return result;
}
#endif
#ifdef QT_GUI_LIB
qint32 XFormats::setBaseComboBox(QComboBox *pComboBox, qint32 nBase)
{
    const bool bBlocked1 = pComboBox->blockSignals(true);

    pComboBox->clear();

    pComboBox->addItem(QString("10"), 10);
    pComboBox->addItem(QString("16"), 16);

    qint32 nResult = setComboBoxCurrent(pComboBox, nBase).toInt();

    pComboBox->blockSignals(bBlocked1);

    return nResult;
}
#endif
#ifdef QT_GUI_LIB
XBinary::MAPMODE XFormats::getMapModesList(XBinary::FT fileType, QComboBox *pComboBox)
{
    XBinary::MAPMODE result = XBinary::MAPMODE_UNKNOWN;

    const bool bBlocked1 = pComboBox->blockSignals(true);

    pComboBox->clear();

    QList<XBinary::MAPMODE> listMapModes = getMapModesList(fileType);

    qint32 nNumberOfRecords = listMapModes.count();

    if (nNumberOfRecords) {
        result = listMapModes.at(0);

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            XBinary::MAPMODE record = listMapModes.at(i);
            pComboBox->addItem(XBinary::mapModeToString(record), record);
        }
    }

    pComboBox->blockSignals(bBlocked1);

    return result;
}
#endif
#ifdef QT_GUI_LIB
void XFormats::setProgressBar(QProgressBar *pProgressBar, XBinary::PDRECORD pdRecord)
{
    if ((pdRecord.nTotal) || (pdRecord.sStatus != "")) {
        pProgressBar->show();

        if (pdRecord.nTotal) {
            pProgressBar->setMaximum(pdRecord.nTotal);
            pProgressBar->setValue(pdRecord.nCurrent);
        }

        if (pdRecord.sStatus != "") {
            pProgressBar->setFormat(pdRecord.sStatus);
        }
    } else {
        pProgressBar->hide();
    }
}
#endif
#ifdef QT_GUI_LIB
XBinary::DM XFormats::setDisasmModeComboBox(XBinary::DM disasmMode, QComboBox *pComboBox)
{
    XBinary::DM result = disasmMode;

    const bool bBlocked1 = pComboBox->blockSignals(true);

    pComboBox->clear();

    if (disasmMode == XBinary::DM_UNKNOWN) {
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_X86_16), XBinary::DM_X86_16);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_X86_32), XBinary::DM_X86_32);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_X86_64), XBinary::DM_X86_64);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_ARM_LE), XBinary::DM_ARM_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_ARM_BE), XBinary::DM_ARM_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_AARCH64_LE), XBinary::DM_AARCH64_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_AARCH64_BE), XBinary::DM_AARCH64_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_CORTEXM), XBinary::DM_CORTEXM);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_THUMB_LE), XBinary::DM_THUMB_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_THUMB_BE), XBinary::DM_THUMB_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MIPS_LE), XBinary::DM_MIPS_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MIPS_BE), XBinary::DM_MIPS_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MIPS64_LE), XBinary::DM_MIPS64_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MIPS64_BE), XBinary::DM_MIPS64_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_PPC_LE), XBinary::DM_PPC_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_PPC_BE), XBinary::DM_PPC_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_PPC64_LE), XBinary::DM_PPC64_LE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_PPC64_BE), XBinary::DM_PPC64_BE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_SPARC), XBinary::DM_SPARC);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_S390X), XBinary::DM_S390X);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_XCORE), XBinary::DM_XCORE);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K), XBinary::DM_M68K);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K00), XBinary::DM_M68K00);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K10), XBinary::DM_M68K10);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K20), XBinary::DM_M68K20);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K30), XBinary::DM_M68K30);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K40), XBinary::DM_M68K40);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M68K60), XBinary::DM_M68K60);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_TMS320C64X), XBinary::DM_TMS320C64X);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6800), XBinary::DM_M6800);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6801), XBinary::DM_M6801);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6805), XBinary::DM_M6805);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6808), XBinary::DM_M6808);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6809), XBinary::DM_M6809);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_M6811), XBinary::DM_M6811);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_CPU12), XBinary::DM_CPU12);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_HD6301), XBinary::DM_HD6301);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_HD6309), XBinary::DM_HD6309);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_HCS08), XBinary::DM_HCS08);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_EVM), XBinary::DM_EVM);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MOS65XX), XBinary::DM_MOS65XX);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_RISKV32), XBinary::DM_RISKV32);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_RISKV64), XBinary::DM_RISKV64);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_RISKVC), XBinary::DM_RISKVC);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_MOS65XX), XBinary::DM_MOS65XX);
        pComboBox->addItem(XBinary::disasmIdToString(XBinary::DM_WASM), XBinary::DM_WASM);
    } else {
        pComboBox->addItem(XBinary::disasmIdToString(disasmMode), disasmMode);
    }

    pComboBox->blockSignals(bBlocked1);

    return result;
}
#endif
