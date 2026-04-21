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
#include "xdevicetableview.h"

XDeviceTableView::XDeviceTableView(QWidget *pParent) : XAbstractTableView(pParent)
{
    m_pXInfoDB = nullptr;
    m_searchData = {};
    m_locationMode = XBinaryView::LOCMODE_ADDRESS;
    m_nLocationBase = 16;
    m_nVisitedIndex = 0;

    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChangedSlot()));
    setXInfoDB(&m_emptyXInfoDB);
}

XDeviceTableView::~XDeviceTableView()
{
    // TODO Check
}

void XDeviceTableView::setXInfoDB(XInfoDB *pXInfoDB)
{
    if (m_pXInfoDB != pXInfoDB) {
        m_pXInfoDB = pXInfoDB;

        if (pXInfoDB) {
            connect(m_pXInfoDB, SIGNAL(reloadViewSignal()), this, SLOT(reloadView()));
        }
    }
}

XInfoDB *XDeviceTableView::getXInfoDB()
{
    return m_pXInfoDB;
}

void XDeviceTableView::setData(QIODevice *pDevice, const XBinaryView::OPTIONS &options)
{
    m_binaryView.setData(pDevice, options);

    m_listVisited.clear();

    if (pDevice) {
        XDeviceTableView::adjustScrollCount();
        //    setReadonly(!(pDevice->isWritable()));
        setActive(true);
    } else {
        setActive(false);
    }
}

void XDeviceTableView::reset()
{
    m_binaryView.reset();
}

QIODevice *XDeviceTableView::getDevice()
{
    return m_binaryView.getDevice();
}

XBinaryView *XDeviceTableView::getBinaryView()
{
    return &m_binaryView;
}

void XDeviceTableView::setLocationMode(XBinaryView::LOCMODE locationMode)
{
    m_locationMode = locationMode;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

XBinaryView::LOCMODE XDeviceTableView::getlocationMode()
{
    return m_locationMode;
}

void XDeviceTableView::setLocationBase(qint32 nBase)
{
    m_nLocationBase = nBase;

    adjust(true);
    viewport()->update();
    emit selectionChanged();
}

qint32 XDeviceTableView::getLocationBase()
{
    return m_nLocationBase;
}

void XDeviceTableView::adjustScrollCount()
{
    // TODO
}

bool XDeviceTableView::isViewPosValid(XVPOS nViewPos)
{
    return getBinaryView()->isViewPosValid(nViewPos);
}

bool XDeviceTableView::isEnd(XVPOS nViewPos)
{
    return getBinaryView()->isEnd(nViewPos);
}

XDeviceTableView::DEVICESTATE XDeviceTableView::getDeviceState()
{
    DEVICESTATE result = {};
    STATE state = getState();

    result.nSelectionDeviceOffset = getBinaryView()->viewPosToDeviceOffset(state.nSelectionViewPos);
    //    result.nCursorOffset = state.nCursorViewPos;
    result.nSelectionSize = state.nSelectionViewSize;
    result.nStartDeviceOffset = getBinaryView()->viewPosToDeviceOffset(getViewPosStart());

    return result;
}

void XDeviceTableView::setDeviceState(const DEVICESTATE &deviceState)
{
    _goToViewPos(getBinaryView()->deviceOffsetToViewPos(deviceState.nStartDeviceOffset));
    _initSetSelection(getBinaryView()->deviceOffsetToViewPos(deviceState.nSelectionDeviceOffset), deviceState.nSelectionSize);
    //    setCursorViewPos(deviceState.nCursorOffset);

    adjust();
    viewport()->update();
}

void XDeviceTableView::setDeviceSelection(qint64 nOffset, qint64 nSize)
{
    if (isSelectionEnable()) {
        qint64 nViewPos = getBinaryView()->deviceOffsetToViewPos(nOffset);

        _initSetSelection(nViewPos, nSize);

        adjust();
        viewport()->update();
    }
}

bool XDeviceTableView::isPrevVisitedAvailable()
{
    return m_nVisitedIndex > 0;
}

bool XDeviceTableView::isNextVisitedAvailable()
{
    return m_nVisitedIndex < (m_listVisited.count() - 1);
}

void XDeviceTableView::goToNextVisited()
{
    if (isNextVisitedAvailable()) {
        m_nVisitedIndex++;
        qint64 nViewPos = m_listVisited.at(m_nVisitedIndex);

        if (_goToViewPos(nViewPos)) {
            _initSetSelection(nViewPos, getBinaryView()->getViewSizeByViewPos(nViewPos));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::goToPrevVisited()
{
    if (isPrevVisitedAvailable()) {
        m_nVisitedIndex--;
        qint64 nViewPos = m_listVisited.at(m_nVisitedIndex);

        if (_goToViewPos(nViewPos)) {
            _initSetSelection(nViewPos, getBinaryView()->getViewSizeByViewPos(nViewPos));
        }
    }

    emit visitedStateChanged();
}

void XDeviceTableView::addVisited(qint64 nViewPos)
{
    // #ifdef QT_DEBUG
    //     qDebug("Add visited %s", XBinary::valueToHex(nViewPos).toLatin1().data());
    // #endif

    if ((m_listVisited.empty()) || (m_listVisited.last() != nViewPos)) {
        qint32 nNumberOfVisited = m_listVisited.count();

        for (qint32 i = nNumberOfVisited - 1; i > m_nVisitedIndex; i--) {
            m_listVisited.removeAt(i);
        }

        m_listVisited.append(nViewPos);

        if (m_listVisited.count() > N_MAX_VISITED) {
            m_listVisited.removeFirst();
        }

        m_nVisitedIndex = m_listVisited.count() - 1;

        emit visitedStateChanged();
    }
}

void XDeviceTableView::clearVisited()
{
    m_listVisited.clear();
}

void XDeviceTableView::setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    goToLocation(nLocation, (XBinary::LT)nLocationType);
    setLocationOffset(nLocation, (XBinary::LT)nLocationType, nSize);
}

qint64 XDeviceTableView::write_array(qint64 nOffset, char *pData, qint64 nDataSize)
{
    qint64 nResult = 0;

    if (getDevice()) {
        if (saveBackup()) {
            nResult = XBinary::write_array(getDevice(), nOffset, pData, nDataSize);
        }
    }

    return nResult;
}

QByteArray XDeviceTableView::read_array(qint64 nOffset, qint32 nSize)
{
    QByteArray baResult;

    if (getDevice()) {
        baResult = XBinary::read_array(getDevice(), nOffset, nSize);
    }

    return baResult;
}

void XDeviceTableView::goToAddress(XADDR nAddress, bool bShort, bool bAprox, bool bSaveVisited)
{
    goToLocation(nAddress, XBinary::LT_ADDRESS, bShort, bAprox, bSaveVisited);
}

void XDeviceTableView::goToOffset(qint64 nOffset, bool bShort, bool bAprox, bool bSaveVisited)
{
    goToLocation(nOffset, XBinary::LT_OFFSET, bShort, bAprox, bSaveVisited);
}

void XDeviceTableView::goToLocation(XADDR nLocation, XBinary::LT locationType, bool bShort, bool bAprox, bool bSaveVisited)
{
    if (nLocation != (XADDR)-1) {
        qint64 nViewPos = getBinaryView()->locationToViewPos(nLocation, locationType);

        if (bSaveVisited) {
            addVisited(getState().nSelectionViewPos);
        }

        if (_goToViewPos(nViewPos, false, bShort, bAprox)) {
            if (bSaveVisited) {
                addVisited(nViewPos);
            }

            qint64 nViewSize = getBinaryView()->getViewSizeByViewPos(nViewPos);

            _initSetSelection(nViewPos, nViewSize);
            // TODO
        }
        // mb TODO reload
    }
}

void XDeviceTableView::setLocationOffset(XADDR nLocation, XBinary::LT locationType, qint64 nSize)
{
    qint64 nOffset = 0;

    // TODO move to Binary
    if (locationType == XBinary::LT_ADDRESS) {
        nOffset = XBinary::addressToOffset(getBinaryView()->getMemoryMap(), nLocation);
    } else if (locationType == XBinary::LT_RELADDRESS) {
        nOffset = XBinary::relAddressToOffset(getBinaryView()->getMemoryMap(), nLocation);
    } else if (locationType == XBinary::LT_OFFSET) {
        nOffset = nLocation;
    }

    setDeviceSelection(nOffset, nSize);
}

void XDeviceTableView::setSelectionAddress(XADDR nAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::addressToOffset(getBinaryView()->getMemoryMap(), nAddress);

    setSelectionOffset(nOffset, nSize);
}

void XDeviceTableView::setSelectionRelAddress(XADDR nRelAddress, qint64 nSize)
{
    qint64 nOffset = XBinary::relAddressToOffset(getBinaryView()->getMemoryMap(), nRelAddress);

    setSelectionOffset(nOffset, nSize);
}

void XDeviceTableView::setSelectionOffset(qint64 nOffset, qint64 nSize)
{
    if (nOffset != -1) {
        qint64 nViewPos = getBinaryView()->deviceOffsetToViewPos(nOffset);
        qint64 nViewSize = nSize;

        _initSetSelection(nViewPos, nViewSize);
        viewport()->update();  // TODO Check
    }
}

bool XDeviceTableView::isEdited()
{
    bool bResult = XBinary::isBackupPresent(XBinary::getBackupDevice(getDevice()));

    return bResult;
}

bool XDeviceTableView::saveBackup()
{
    bool bResult = true;

    if ((getGlobalOptions()->isSaveBackup()) && (!isEdited())) {
        // Save backup
        bResult = XBinary::saveBackup(XBinary::getBackupDevice(getDevice()));
    }

    return bResult;
}

void XDeviceTableView::setEdited(qint64 nDeviceOffset, qint64 nDeviceSize)
{
    //    QFile *pFile=dynamic_cast<QFile *>(getDevice());

    //    if(pFile)
    //    {
    //        pFile->flush();
    //    }

    updateData();

    if (m_pXInfoDB) {
        m_pXInfoDB->setEdited(nDeviceOffset, nDeviceSize);
    }

    viewport()->update();
}

void XDeviceTableView::adjustAfterAnalysis()
{
    adjustScrollCount();
    clearVisited();

    reload(true);
}

void XDeviceTableView::_goToAddressSlot()
{
    DEVICESTATE state = getDeviceState();
    XADDR nAddress = 0;

    DialogGoToAddress::TYPE type = DialogGoToAddress::TYPE_VIRTUALADDRESS;

    if (m_locationMode == XBinaryView::LOCMODE_RELADDRESS) {
        nAddress = XBinary::offsetToRelAddress(getBinaryView()->getMemoryMap(), state.nSelectionDeviceOffset);
    } else {
        nAddress = XBinary::offsetToAddress(getBinaryView()->getMemoryMap(), state.nSelectionDeviceOffset);
    }

    DialogGoToAddress da(this, getBinaryView()->getMemoryMap(), type, nAddress);
    da.setGlobal(getShortcuts(), getGlobalOptions());

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToAddress(da.getValue(), false, false, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToOffsetSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogGoToAddress da(this, getBinaryView()->getMemoryMap(), DialogGoToAddress::TYPE_OFFSET, state.nSelectionDeviceOffset);
    da.setGlobal(getShortcuts(), getGlobalOptions());

    if (da.exec() == QDialog::Accepted)  // TODO use status
    {
        goToOffset(da.getValue(), true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToSelectionStart()
{
    DEVICESTATE state = getDeviceState();

    if (state.nSelectionSize) {
        goToOffset(state.nSelectionDeviceOffset, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_goToSelectionEnd()
{
    DEVICESTATE state = getDeviceState();

    if (state.nSelectionSize) {
        qint64 nOffset = state.nSelectionDeviceOffset + state.nSelectionSize;

        if (isEnd(nOffset)) {
            nOffset--;
        }

        // mb TODO go to end alignment
        goToOffset(nOffset, true);
        setFocus();
        viewport()->update();
    }
}

void XDeviceTableView::_findStringSlot()
{
    _findSlot(XBinary::SEARCHMODE_STRING);
}

void XDeviceTableView::_findSignatureSlot()
{
    _findSlot(XBinary::SEARCHMODE_SIGNATURE);
}

void XDeviceTableView::_findValueSlot()
{
    _findSlot(XBinary::SEARCHMODE_VALUE);
}

void XDeviceTableView::_findSlot(XBinary::SEARCHMODE mode)
{
    DEVICESTATE state = getDeviceState();

    m_searchData = {};
    m_searchData.nResultOffset = -1;
    m_searchData.nCurrentOffset = state.nSelectionDeviceOffset;

    DialogSearch::OPTIONS options = {};
    options.bShowBegin = true;

    DialogSearch dialogSearch(this, getDevice(), &m_searchData, mode, options);

    if (dialogSearch.exec() == QDialog::Accepted)  // TODO use status
    {
        SearchProcess searchProcess;
        XDialogProcess dsp(this, &searchProcess);
        dsp.setGlobal(getShortcuts(), getGlobalOptions());
        searchProcess.setData(getDevice(), &m_searchData, dsp.getPdStruct());
        dsp.start();
        dsp.showDialogDelay();

        if (m_searchData.nResultOffset != -1) {
            qint64 nViewPos = getBinaryView()->deviceOffsetToViewPos(m_searchData.nResultOffset);
            qint64 nViewSize = m_searchData.nResultSize;

            _goToViewPos(nViewPos);
            _initSetSelection(nViewPos, nViewSize);
            setFocus();
            viewport()->update();
        } else {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_findNextSlot()
{
    if (m_searchData.bIsInit) {
        m_searchData.nCurrentOffset = m_searchData.nResultOffset + 1;
        m_searchData.startFrom = XBinary::SF_CURRENTOFFSET;

        SearchProcess searchProcess;
        XDialogProcess dsp(this, &searchProcess);
        dsp.setGlobal(getShortcuts(), getGlobalOptions());
        searchProcess.setData(getDevice(), &m_searchData, dsp.getPdStruct());
        dsp.start();
        dsp.showDialogDelay();

        if (dsp.isSuccess())  // TODO use status
        {
            qint64 nViewPos = getBinaryView()->deviceOffsetToViewPos(m_searchData.nResultOffset);
            qint64 nViewSize = m_searchData.nResultSize;

            _goToViewPos(nViewPos);
            _initSetSelection(nViewPos, nViewSize);
            setFocus();
            viewport()->update();
        } else if (m_searchData.valueType != XBinary::VT_UNKNOWN) {
            emit errorMessage(tr("Nothing found"));
        }
    }
}

void XDeviceTableView::_selectAllSlot()
{
    _initSetSelection(0, getBinaryView()->getViewSize());
    viewport()->update();
}

void XDeviceTableView::_copyAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToAddress(getBinaryView()->getMemoryMap(), state.nSelectionDeviceOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyRelAddressSlot()
{
    DEVICESTATE state = getDeviceState();

    XADDR nAddress = XBinary::offsetToRelAddress(getBinaryView()->getMemoryMap(), state.nSelectionDeviceOffset);

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, nAddress));
}

void XDeviceTableView::_copyOffsetSlot()
{
    DEVICESTATE state = getDeviceState();

    QApplication::clipboard()->setText(XBinary::valueToHex(XBinary::MODE_UNKNOWN, state.nSelectionDeviceOffset));
}

void XDeviceTableView::_setEdited(qint64 nDeviceOffset, qint64 nDeviceSize)
{
    setEdited(nDeviceOffset, nDeviceSize);

    emit dataChanged(nDeviceOffset, nDeviceSize);  // TODO initOffset
}

void XDeviceTableView::goToAddressSlot(XADDR nAddress, qint64 nSize)
{
    goToAddress(nAddress, true, true, true);

    if (nSize) {
        setSelectionAddress(nAddress, nSize);
    }

    reload(true);
}

void XDeviceTableView::goToAddressSlot(XADDR nAddress)
{
    goToAddressSlot(nAddress, 0);
}

void XDeviceTableView::reloadView()
{
    updateData();

    viewport()->update();
}

void XDeviceTableView::selectionChangedSlot()
{
    XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

    emit currentLocationChanged(deviceState.nSelectionDeviceOffset, XBinary::LT_OFFSET, deviceState.nSelectionSize);
}

void XDeviceTableView::changeLocationMode()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        XBinaryView::LOCMODE mode = (XBinaryView::LOCMODE)pAction->property("mode").toUInt();

        setLocationMode(mode);
    }
}

void XDeviceTableView::changeLocationBase()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        qint32 nBase = (XBinaryView::LOCMODE)pAction->property("base").toInt();

        setLocationBase(nBase);
    }
}

void XDeviceTableView::currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize)
{
    setLocation(nLocation, nLocationType, nSize);

    reload(true);
}
