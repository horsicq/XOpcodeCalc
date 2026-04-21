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
#include "xcom.h"

XBinary::XCONVERT _TABLE_XCOM_STRUCTID[] = {
    {XCOM::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XCOM::STRUCTID_HEADER, "HEADER", QString("Header")},
};

XCOM::XCOM(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XBinary(pDevice, bIsImage, nModuleAddress)
{
    XBinary::setBaseAddress(XCOM_DEF::ADDRESS_BEGIN);
}

XCOM::~XCOM()
{
}

bool XCOM::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    // mb TODO mb
    if (getSize() <= (XCOM_DEF::IMAGESIZE - XCOM_DEF::ADDRESS_BEGIN)) {
        bResult = true;
    }

    return bResult;
}

bool XCOM::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XCOM xcom(pDevice, bIsImage, nModuleAddress);

    return xcom.isValid();
}

bool XCOM::isExecutable()
{
    return true;
}

XBinary::MODE XCOM::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XCOM xcom(pDevice, bIsImage, nModuleAddress);

    return xcom.getMode();
}

QList<XBinary::MAPMODE> XCOM::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XCOM::getMemoryMap(XBinary::MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    XBinary::PDSTRUCT pdStructEmpty = {};

    if (!pPdStruct) {
        pdStructEmpty = XBinary::createPdStruct();
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nModuleAddress = getModuleAddress();
    result.nEntryPointAddress = result.nModuleAddress;
    result.nBinarySize = nTotalSize;
    result.nImageSize = getImageSize();
    result.fileType = getFileType();
    result.mode = getMode();
    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();

    qint64 nCodeSize = qMin(nTotalSize, (qint64)(XCOM_DEF::IMAGESIZE - XCOM_DEF::ADDRESS_BEGIN));

    _MEMORY_RECORD record = {};
    record.nAddress = 0;

    record.nOffset = -1;
    record.nSize = XCOM_DEF::ADDRESS_BEGIN;
    record.nIndex++;
    record.bIsVirtual = true;

    result.listRecords.append(record);

    _MEMORY_RECORD recordMain = {};
    recordMain.nAddress = XCOM_DEF::ADDRESS_BEGIN;
    recordMain.nOffset = 0;
    recordMain.nSize = nCodeSize;
    recordMain.nIndex++;

    result.listRecords.append(recordMain);

    qint64 nVirtualSize = (qint64)(XCOM_DEF::IMAGESIZE - XCOM_DEF::ADDRESS_BEGIN) - nTotalSize;

    if (nVirtualSize > 0) {
        _MEMORY_RECORD record = {};
        record.nAddress = XCOM_DEF::ADDRESS_BEGIN + nCodeSize;

        record.nOffset = -1;
        record.nSize = nVirtualSize;
        record.nIndex++;
        record.bIsVirtual = true;

        result.listRecords.append(record);
    }

    _handleOverlay(&result);

    return result;
}

QString XCOM::getArch()
{
    return QString("8086");
}

XBinary::MODE XCOM::getMode()
{
    return MODE_16;
}

XBinary::ENDIAN XCOM::getEndian()
{
    return ENDIAN_LITTLE;
}

qint64 XCOM::getImageSize()
{
    return 0x10000;
}

XBinary::FT XCOM::getFileType()
{
    return FT_COM;
}

QString XCOM::getVersion()
{
    return QString("");
}

qint32 XCOM::getType()
{
    return TYPE_EXECUTABLE;
}

XBinary::OSNAME XCOM::getOsName()
{
    return OSNAME_MSDOS;
}

QString XCOM::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_EXECUTABLE: sResult = QString("EXE"); break;
    }

    return sResult;
}

QString XCOM::getMIMEString()
{
    return "application/x-dosexec";
}

QString XCOM::getFileFormatExt()
{
    return "com";
}

QString XCOM::getFileFormatExtsString()
{
    return "COM files (*.com);;";
}

qint64 XCOM::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return getSize();
}

QString XCOM::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XCOM_STRUCTID, sizeof(_TABLE_XCOM_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XCOM::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        _dataHeadersOptions.nID = STRUCTID_HEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_HEADER) {
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XCOM::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = getSize();

                dataHeader.listRecords.append(getDataRecord(0, dataHeader.nSize, "Data", VT_BYTE_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));

                listResult.append(dataHeader);
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XCOM::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<FPART> listResult;

    qint64 nTotalSize = getSize();

    if (nFileParts & FILEPART_HEADER) {
        FPART record = {};

        record.filePart = FILEPART_HEADER;
        record.nFileOffset = 0;
        record.nFileSize = nTotalSize;
        record.nVirtualAddress = XCOM_DEF::ADDRESS_BEGIN;
        record.sName = tr("Header");

        listResult.append(record);
    }

    return listResult;
}
