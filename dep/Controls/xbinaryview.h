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
#ifndef XBINARYVIEW_H
#define XBINARYVIEW_H

#include "xbinary.h"
#include "xoptions.h"
#include "xinfodb.h"
#include "xcapstone.h"

class XBinaryView : public QObject {
    Q_OBJECT

public:
    enum LOCMODE {
        LOCMODE_OFFSET = 0,
        LOCMODE_ADDRESS,
        LOCMODE_RELADDRESS,
        LOCMODE_THIS
    };

    struct OPTIONS {
        qint64 nStartOffset;
        qint64 nTotalSize;
        XADDR nInitAddress;
        XADDR nEntryPointAddress;  // TODO move to xdb todo use bool
        XBinary::FT fileType;
        XBinary::DM disasmMode;
        bool bAprox;
        bool bMenu_Hex;
        bool bMenu_Disasm;
        bool bMenu_MemoryMap;
        bool bMenu_MainHex;
        bool bHideReadOnly;
        bool bIsImage;
        XADDR nModuleAddress;
        bool bShowVirtual;
        qint64 nStartSelectionOffset;  // -1 no selection
        qint64 nSizeOfSelection;       // TODO Check
        QString sTitle;                // For dialogs
        LOCMODE addressMode;
    };

    struct VIEWSTRUCT {
        XVPOS nViewPos;
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
    };

    explicit XBinaryView(QObject *pParent = nullptr);
    virtual ~XBinaryView();

    void setData(QIODevice *pDevice, const OPTIONS &options);
    void reset();
    QIODevice *getDevice();

    XVPOS deviceOffsetToViewPos(qint64 nOffset);
    qint64 viewPosToDeviceOffset(XVPOS nViewPos);
    XADDR viewPosToAddress(XVPOS nViewPos);
    bool isViewPosValid(XVPOS nViewPos);
    bool isEnd(XVPOS nViewPos);
    qint64 getViewSizeByViewPos(XVPOS nViewPos);
    XVPOS addressToViewPos(XADDR nAddress);
    XVPOS locationToViewPos(XADDR nLocation, XBinary::LT locationType);
    VIEWSTRUCT _getViewStructByOffset(qint64 nOffset);
    VIEWSTRUCT _getViewStructByAddress(XADDR nAddress);
    // VIEWSTRUCT _getViewStructByScroll(qint64 nValue);
    VIEWSTRUCT _getViewStructByViewPos(XVPOS nViewPos);

    qint64 getViewSize();
    XBinary::_MEMORY_MAP *getMemoryMap();
    XDisasmCore *getDisasmCore();
    OPTIONS *getOptions();

private:
    QIODevice *m_pDevice;
    QList<VIEWSTRUCT> m_listViewStruct;
    XDisasmCore m_disasmCore;
    XBinary::_MEMORY_MAP m_memoryMap;
    OPTIONS m_options;
    qint64 m_nViewSize;
};

#endif  // XBINARYVIEW_H
