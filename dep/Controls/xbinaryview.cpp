/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xbinaryview.h"

XBinaryView::XBinaryView(QObject *pParent) : QObject(pParent)
{
    m_pDevice = nullptr;
    m_options = {};
    m_nViewSize = 0;
}

XBinaryView::~XBinaryView()
{
}

void XBinaryView::setData(QIODevice *pDevice, const OPTIONS &options)
{
    m_pDevice = pDevice;
    m_options = options;

    m_disasmCore.setMode(m_options.disasmMode);

    m_memoryMap = XFormats::getMemoryMap(m_options.fileType, XBinary::MAPMODE_UNKNOWN, m_pDevice);

    XVPOS nViewPos = 0;

    bool bAll = false;

    if (pDevice) {
        if (m_options.nTotalSize <= 0) {
            m_options.nTotalSize = pDevice->size() - m_options.nStartOffset;
        }

        if ((m_options.nStartOffset == 0) && (m_options.nTotalSize == pDevice->size())) {
            bAll = true;
        }
    }

    m_listViewStruct.clear();

    qint32 nNumberOfRecords = m_memoryMap.listRecords.count();
    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        VIEWSTRUCT record = {};
        record.nAddress = m_memoryMap.listRecords.at(i).nAddress;
        record.nOffset = m_memoryMap.listRecords.at(i).nOffset;
        record.nSize = m_memoryMap.listRecords.at(i).nSize;
        // record.nScrollStart = nScrollStart;
        record.nViewPos = nViewPos;
        // record.nScrollCount = record.nSize;

        bool bAdd = true;
        // // TODO XInfoDB

        if (m_memoryMap.listRecords.at(i).bIsInvisible) {
            bAdd = false;
        }

        if (!m_options.bShowVirtual) {
            if (m_memoryMap.listRecords.at(i).bIsVirtual) {
                bAdd = false;
            }
        }

        // Add if m_nStartOffset and qint64 m_nTotalSize in this viewStruct
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset >= m_options.nStartOffset && (record.nOffset + record.nSize <= m_options.nStartOffset + m_options.nTotalSize)) {
                    bAdd = true;
                } else {
                    bAdd = false;
                }
            }
        }

        // Add if m_nStartOffset and qint64 m_nTotalSize partially in this viewStruct, correct nAddress, nOffset, nSize
        if (!bAll) {
            if (record.nOffset != -1) {
                if (record.nOffset < m_options.nStartOffset && (record.nOffset + record.nSize > m_options.nStartOffset)) {
                    record.nAddress += m_options.nStartOffset - record.nOffset;
                    record.nOffset = m_options.nStartOffset;
                    record.nSize -= m_options.nStartOffset - record.nOffset;
                    bAdd = true;
                }
                if (record.nOffset < m_options.nStartOffset + m_options.nTotalSize && (record.nOffset + record.nSize > m_options.nStartOffset + m_options.nTotalSize)) {
                    record.nSize = m_options.nStartOffset + m_options.nTotalSize - record.nOffset;
                    bAdd = true;
                }
            }
        }

        if (bAdd) {
            nViewPos += record.nSize;

            m_listViewStruct.append(record);
        }
    }

    m_nViewSize = nViewPos;
}

void XBinaryView::reset()
{
    m_pDevice = nullptr;
    m_options = {};
    m_nViewSize = 0;
    m_listViewStruct.clear();
}

QIODevice *XBinaryView::getDevice()
{
    return m_pDevice;
}

XBinaryView::VIEWSTRUCT XBinaryView::_getViewStructByViewPos(XVPOS nViewPos)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nViewPos <= nViewPos) && (nViewPos < (m_listViewStruct.at(i).nViewPos + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XBinaryView::VIEWSTRUCT XBinaryView::_getViewStructByAddress(XADDR nAddress)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nAddress != (XADDR)-1) && (m_listViewStruct.at(i).nAddress <= nAddress) &&
            (nAddress < (m_listViewStruct.at(i).nAddress + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XBinaryView::VIEWSTRUCT XBinaryView::_getViewStructByOffset(qint64 nOffset)
{
    VIEWSTRUCT result = {};

    qint32 nNumberOfRecords = m_listViewStruct.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((m_listViewStruct.at(i).nOffset != -1) && (m_listViewStruct.at(i).nOffset <= nOffset) &&
            (nOffset < (m_listViewStruct.at(i).nOffset + m_listViewStruct.at(i).nSize))) {
            result = m_listViewStruct.at(i);
            break;
        }
    }

    return result;
}

XVPOS XBinaryView::deviceOffsetToViewPos(qint64 nOffset)
{
    XVPOS nResult = 0;

    VIEWSTRUCT viewStruct = _getViewStructByOffset(nOffset);

    if (viewStruct.nSize) {
        nResult = viewStruct.nViewPos + (nOffset - viewStruct.nOffset);
    }

    return nResult;
}

qint64 XBinaryView::viewPosToDeviceOffset(XVPOS nViewPos)
{
    qint64 nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nOffset + (nViewPos - viewStruct.nViewPos);
    }

    return nResult;
}

XADDR XBinaryView::viewPosToAddress(XVPOS nViewPos)
{
    XADDR nResult = -1;

    VIEWSTRUCT viewStruct = _getViewStructByViewPos(nViewPos);

    if (viewStruct.nSize && (viewStruct.nOffset != -1)) {
        nResult = viewStruct.nAddress + (nViewPos - viewStruct.nViewPos);
    }

    return nResult;
}

bool XBinaryView::isViewPosValid(XVPOS nViewPos)
{
    bool bResult = false;

    if ((nViewPos >= 0) && (nViewPos < (XVPOS)m_nViewSize)) {
        bResult = true;
    }

    return bResult;
}

bool XBinaryView::isEnd(XVPOS nOffset)
{
    return (nOffset == m_nViewSize);
}

qint64 XBinaryView::getViewSizeByViewPos(XVPOS nViewPos)
{
    Q_UNUSED(nViewPos)

    return 1;
}

XVPOS XBinaryView::addressToViewPos(XADDR nAddress)
{
    qint64 nResult = 0;

    qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nAddress);

    nResult = deviceOffsetToViewPos(nOffset);

    return nResult;
}

XVPOS XBinaryView::locationToViewPos(XADDR nLocation, XBinary::LT locationType)
{
    XVPOS nResult = 0;

    if (locationType == XBinary::LT_ADDRESS) {
        qint64 nOffset = XBinary::addressToOffset(getMemoryMap(), nLocation);
        nResult = deviceOffsetToViewPos(nOffset);
    } else if (locationType == XBinary::LT_RELADDRESS) {
        qint64 nOffset = XBinary::relAddressToOffset(getMemoryMap(), nLocation);
        nResult = deviceOffsetToViewPos(nOffset);
    } else if (locationType == XBinary::LT_OFFSET) {
        nResult = deviceOffsetToViewPos(nLocation);
    }

    return nResult;
}

qint64 XBinaryView::getViewSize()
{
    return m_nViewSize;
}

XBinary::_MEMORY_MAP *XBinaryView::getMemoryMap()
{
    return &m_memoryMap;
}

XDisasmCore *XBinaryView::getDisasmCore()
{
    return &m_disasmCore;
}

XBinaryView::OPTIONS *XBinaryView::getOptions()
{
    return &m_options;
}
