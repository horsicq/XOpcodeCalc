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
#ifndef XDEVICETABLEVIEW_H
#define XDEVICETABLEVIEW_H

#include "dialoggotoaddress.h"
#include "dialoghexsignature.h"
#include "dialogsearch.h"
#include "xabstracttableview.h"
#include "xformats.h"
#include "xdialogprocess.h"
#include "searchprocess.h"
#include "xbinaryview.h"

class XDeviceTableView : public XAbstractTableView {
    Q_OBJECT

public:
    struct DEVICESTATE {
        quint64 nSelectionDeviceOffset;
        qint64 nSelectionSize;
        //        qint64 nCursorOffset;
        qint64 nStartDeviceOffset;
    };

    XDeviceTableView(QWidget *pParent = nullptr);
    ~XDeviceTableView();

    void setXInfoDB(XInfoDB *pXInfoDB);
    XInfoDB *getXInfoDB();
    void setData(QIODevice *pDevice, const XBinaryView::OPTIONS &options);
    void reset();
    QIODevice *getDevice();
    XBinaryView *getBinaryView();
    void setLocationMode(XBinaryView::LOCMODE locationMode);
    XBinaryView::LOCMODE getlocationMode();
    void setLocationBase(qint32 nBase);
    qint32 getLocationBase();
    qint64 write_array(qint64 nOffset, char *pData, qint64 nDataSize);
    QByteArray read_array(qint64 nOffset, qint32 nSize);
    void goToAddress(XADDR nAddress, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void goToOffset(qint64 nOffset, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void goToLocation(XADDR nLocation, XBinary::LT locationType, bool bShort = false, bool bAprox = false, bool bSaveVisited = false);
    void setLocationOffset(XADDR nLocation, XBinary::LT locationType, qint64 nSize);
    void setSelectionAddress(XADDR nAddress, qint64 nSize);        // TODO remove
    void setSelectionRelAddress(XADDR nRelAddress, qint64 nSize);  // TODO remove
    void setSelectionOffset(qint64 nOffset, qint64 nSize);         // TODO remove
    bool isEdited();
    bool saveBackup();
    void adjustAfterAnalysis();  // TODO Check mb remove
    virtual DEVICESTATE getDeviceState();
    virtual void setDeviceState(const DEVICESTATE &deviceState);

    void setDeviceSelection(qint64 nOffset, qint64 nSize);
    bool isPrevVisitedAvailable();
    bool isNextVisitedAvailable();
    void goToNextVisited();
    void goToPrevVisited();
    void addVisited(qint64 nViewPos);
    void clearVisited();

    virtual void setLocation(quint64 nLocation, qint32 nLocationType, qint64 nSize);

public slots:
    void setEdited(qint64 nDeviceOffset, qint64 nDeviceSize);
    void _goToAddressSlot();
    void _goToOffsetSlot();

protected:
    virtual void adjustScrollCount();
    virtual bool isViewPosValid(XVPOS nViewPos);
    virtual bool isEnd(XVPOS nViewPos);

signals:
    void visitedStateChanged();
    void deviceSizeChanged(qint64 nOldSize, qint64 nNewSize);

protected slots:
    void _goToSelectionStart();
    void _goToSelectionEnd();
    void _findStringSlot();
    void _findSignatureSlot();
    void _findValueSlot();
    void _findSlot(XBinary::SEARCHMODE mode);
    void _findNextSlot();
    void _selectAllSlot();
    void _copyAddressSlot();
    void _copyRelAddressSlot();
    void _copyOffsetSlot();
    void _setEdited(qint64 nDeviceOffset, qint64 nDeviceSize);
    void goToAddressSlot(XADDR nAddress, qint64 nSize);
    void goToAddressSlot(XADDR nAddress);
    void reloadView();
    void selectionChangedSlot();
    void changeLocationMode();
    void changeLocationBase();

public slots:
    void currentLocationChangedSlot(quint64 nLocation, qint32 nLocationType, qint64 nSize);

private:
    static const qint32 N_MAX_VISITED = 100;
    XInfoDB m_emptyXInfoDB;
    XInfoDB *m_pXInfoDB;
    XBinary::SEARCHDATA m_searchData;
    XBinaryView::LOCMODE m_locationMode;
    qint32 m_nLocationBase;
    QList<qint64> m_listVisited;
    qint32 m_nVisitedIndex;

    XBinaryView m_binaryView;
};

#endif  // XDEVICETABLEVIEW_H
