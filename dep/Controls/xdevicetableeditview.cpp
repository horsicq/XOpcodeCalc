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
#include "xdevicetableeditview.h"

XDeviceTableEditView::XDeviceTableEditView(QWidget *pParent) : XDeviceTableView(pParent)
{
}

void XDeviceTableEditView::setViewWidgetState(VIEWWIDGET viewWidget, bool bState)
{
    if (bState) {
        m_stViewWidgetState.insert(viewWidget);
    } else {
        m_stViewWidgetState.remove(viewWidget);
    }

    emit viewWidgetsStateChanged();
}

bool XDeviceTableEditView::getViewWidgetState(VIEWWIDGET viewWidget)
{
    return m_stViewWidgetState.contains(viewWidget);
}

void XDeviceTableEditView::dumpMemory(const QString &sDumpName, qint64 nOffset, qint64 nSize)
{
    QString sSaveFileName = XBinary::getResultFileName(getDevice(), QString("%1.bin").arg(sDumpName));
    QString sFileName = QFileDialog::getSaveFileName(this, tr("Save dump"), sSaveFileName, QString("%1 (*.bin)").arg(tr("Raw data")));

    if (!sFileName.isEmpty()) {
        DumpProcess dumpProcess;
        XDialogProcess dd(this, &dumpProcess);
        dd.setGlobal(getShortcuts(), getGlobalOptions());
        dumpProcess.setData(getDevice(), nOffset, nSize, sFileName, DumpProcess::DT_DUMP_DEVICE_OFFSET, dd.getPdStruct());
        dd.start();
        dd.showDialogDelay();
    }
}

QList<XDeviceTableEditView::HIGHLIGHTREGION> XDeviceTableEditView::getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation, XBinary::LT locationType)
{
    QList<HIGHLIGHTREGION> listResult;

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pList->at(i).locationType == locationType) {
            if ((nLocation >= pList->at(i).nLocation) && (nLocation < (pList->at(i).nLocation + pList->at(i).nSize))) {
                listResult.append(pList->at(i));
            }
        }
    }

    return listResult;
}

QList<XDeviceTableEditView::HIGHLIGHTREGION> XDeviceTableEditView::_convertBookmarksToHighlightRegion(QVector<XInfoDB::BOOKMARKRECORD> *pList)
{
    QList<HIGHLIGHTREGION> listResult;

    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        HIGHLIGHTREGION record = {};
        record.bIsValid = true;
        record.nLocation = pList->at(i).nLocation;
        record.locationType = pList->at(i).locationType;
        record.nSize = pList->at(i).nSize;
        record.colBackground = XOptions::stringToColor(pList->at(i).sColorBackground);
        record.colBackgroundSelected = getColorSelected(record.colBackground);
        record.sComment = pList->at(i).sComment;

        listResult.append(record);
    }

    return listResult;
}

void XDeviceTableEditView::_editHex()
{
    if (!isReadonly()) {
        DEVICESTATE state = getDeviceState();

        DialogHexEdit dialogHexEdit(this);
        dialogHexEdit.setGlobal(getShortcuts(), getGlobalOptions());

        //        connect(&dialogHexEdit,SIGNAL(changed()),this,SLOT(_setEdited()));

        dialogHexEdit.setData(getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);

        dialogHexEdit.exec();

        _setEdited(state.nSelectionDeviceOffset, state.nSelectionSize);
    }
}

void XDeviceTableEditView::_editPatch()
{
    if (!isReadonly()) {
        QString sJsonFileName =
            QFileDialog::getOpenFileName(this, tr("Open file") + QString("..."), XBinary::getDeviceDirectory(getDevice()), QString("%1 (*.patch.json)").arg(tr("Patch")));

        if (sJsonFileName != "") {
            DumpProcess dumpProcess;
            XDialogProcess dd(this, &dumpProcess);
            dd.setGlobal(getShortcuts(), getGlobalOptions());
            dumpProcess.setData(getDevice(), DumpProcess::DT_PATCH_DEVICE_OFFSET, sJsonFileName, dd.getPdStruct());
            dd.start();
            dd.showDialogDelay();

            reload(true);
        }
    }
}

void XDeviceTableEditView::_editRemove()
{
    if (!isReadonly()) {
        if (XBinary::isResizeEnable(getDevice())) {
            DEVICESTATE state = getDeviceState();

            DialogRemove::DATA _data = {};
            _data.nOffset = state.nSelectionDeviceOffset;
            _data.nSize = state.nSelectionSize;
            _data.nMaxSize = getDevice()->size();

            DialogRemove dialogRemove(this, &_data);
            dialogRemove.setGlobal(getShortcuts(), getGlobalOptions());

            if (dialogRemove.exec() == QDialog::Accepted) {
                qint64 nOldSize = _data.nMaxSize;
                qint64 nNewSize = nOldSize - _data.nSize;

                if (nOldSize != nNewSize) {
                    if (saveBackup()) {
                        // mb TODO Process move memory
                        if (XBinary::moveMemory(getDevice(), _data.nOffset + _data.nSize, _data.nOffset, _data.nSize)) {
                            if (XBinary::resize(getDevice(), nNewSize)) {
                                // mb TODO correct bookmarks
                                adjustScrollCount();
                                reload(true);
                                emit deviceSizeChanged(nOldSize, nNewSize);
                                emit dataChanged(_data.nOffset, nNewSize - _data.nOffset);  // TODO initOffset
                            }
                        }
                    }
                }
            }
        }
    }
}

void XDeviceTableEditView::_editResize()
{
    if (!isReadonly()) {
        if (XBinary::isResizeEnable(getDevice())) {
            DialogResize::DATA _data = {};
            _data.nOldSize = getDevice()->size();
            _data.nNewSize = _data.nOldSize;

            DialogResize dialogResize(this, &_data);
            dialogResize.setGlobal(getShortcuts(), getGlobalOptions());

            if (dialogResize.exec() == QDialog::Accepted) {
                if (_data.nOldSize != _data.nNewSize) {
                    if (saveBackup()) {
                        if (XBinary::resize(getDevice(), _data.nNewSize)) {
                            // mb TODO correct bookmarks
                            adjustScrollCount();
                            reload(true);
                            if (_data.nNewSize > _data.nOldSize) {
                                emit deviceSizeChanged(_data.nOldSize, _data.nNewSize);
                                emit dataChanged(_data.nOldSize, _data.nNewSize - _data.nOldSize);  // TODO initOffset
                            } else if (_data.nOldSize > _data.nNewSize) {
                                emit deviceSizeChanged(_data.nOldSize, _data.nNewSize);
                                emit dataChanged(_data.nNewSize, _data.nOldSize - _data.nNewSize);  // TODO initOffset
                            }
                        } else {
                            emit errorMessage(tr("Cannot resize"));
                        }
                    }
                }
            }
        }
    }
}

void XDeviceTableEditView::_strings()
{
    if (!getViewWidgetState(VIEWWIDGET_STRINGS)) {
        setViewWidgetState(VIEWWIDGET_STRINGS, true);

        DialogSearchStrings dialogSearchStrings(this);

        connect(&dialogSearchStrings, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));

        connect(this, SIGNAL(closeWidget_Strings()), &dialogSearchStrings, SLOT(close()));

        dialogSearchStrings.setGlobal(getShortcuts(), getGlobalOptions());

        SearchStringsWidget::OPTIONS stringsOptions = {};
        stringsOptions.bAnsi = true;
        // stringsOptions.bUTF8 = false;
        stringsOptions.bUnicode = true;
        stringsOptions.bNullTerminated = false;

        dialogSearchStrings.setData(getDevice(), XBinary::FT_UNKNOWN, stringsOptions, true);

        XOptions::_adjustStayOnTop(&dialogSearchStrings, true);

        dialogSearchStrings.exec();

        setViewWidgetState(VIEWWIDGET_STRINGS, false);
    } else {
        emit closeWidget_Strings();
    }
}

void XDeviceTableEditView::_visualization()
{
    if (!getViewWidgetState(VIEWWIDGET_VISUALIZATION)) {
        setViewWidgetState(VIEWWIDGET_VISUALIZATION, true);

        DialogVisualization dialogVisualization(this);

        connect(&dialogVisualization, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));

        connect(this, SIGNAL(closeWidget_Visualization()), &dialogVisualization, SLOT(close()));

        dialogVisualization.setGlobal(getShortcuts(), getGlobalOptions());

        dialogVisualization.setData(getDevice(), XBinary::FT_UNKNOWN, true);  // TODO options

        XOptions::_adjustStayOnTop(&dialogVisualization, true);

        dialogVisualization.exec();

        setViewWidgetState(VIEWWIDGET_VISUALIZATION, false);
    } else {
        emit closeWidget_Visualization();
    }
}

void XDeviceTableEditView::_dataInspector()
{
    if (!getViewWidgetState(VIEWWIDGET_DATAINSPECTOR)) {
        setViewWidgetState(VIEWWIDGET_DATAINSPECTOR, true);

        XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

        DialogDataInspector dialogDataInspector(this, getDevice(), deviceState.nSelectionDeviceOffset, deviceState.nSelectionSize);
        dialogDataInspector.setGlobal(getShortcuts(), getGlobalOptions());

        connect(this, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), &dialogDataInspector, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
        connect(this, SIGNAL(dataChanged(qint64, qint64)), &dialogDataInspector, SLOT(dataChangedSlot(qint64, qint64)));
        connect(&dialogDataInspector, SIGNAL(dataChanged(qint64, qint64)), this, SLOT(_setEdited(qint64, qint64)));
        connect(this, SIGNAL(closeWidget_DataInspector()), &dialogDataInspector, SLOT(close()));

        XOptions::_adjustStayOnTop(&dialogDataInspector, true);

        dialogDataInspector.exec();

        setViewWidgetState(VIEWWIDGET_DATAINSPECTOR, false);
    } else {
        emit closeWidget_DataInspector();
    }
}

void XDeviceTableEditView::_dataConvertor()
{
    if (!getViewWidgetState(VIEWWIDGET_DATACONVERTOR)) {
        setViewWidgetState(VIEWWIDGET_DATACONVERTOR, true);

        XDeviceTableView::DEVICESTATE deviceState = getDeviceState();

        SubDevice sd(getDevice(), deviceState.nSelectionDeviceOffset, deviceState.nSelectionSize);

        if (sd.open(QIODevice::ReadOnly)) {
            DialogXDataConvertor dialogDataConvertor(this);
            dialogDataConvertor.setGlobal(getShortcuts(), getGlobalOptions());
            dialogDataConvertor.setData(&sd);
            connect(this, SIGNAL(closeWidget_DataConvertor()), &dialogDataConvertor, SLOT(close()));

            dialogDataConvertor.exec();

            setViewWidgetState(VIEWWIDGET_DATACONVERTOR, false);

            sd.close();
        }
    } else {
        emit closeWidget_DataConvertor();
    }
}

void XDeviceTableEditView::_multisearch()
{
    if (!getViewWidgetState(VIEWWIDGET_MULTISEARCH)) {
        setViewWidgetState(VIEWWIDGET_MULTISEARCH, true);

        SearchValuesWidget::OPTIONS options = {};
        //        options.fileType = XBinary::FT_REGION;
        options.fileType = XBinary::FT_UNKNOWN;

        DialogSearchValues dialogSearchValues(this);
        dialogSearchValues.setGlobal(getShortcuts(), getGlobalOptions());
        dialogSearchValues.setData(getDevice(), options);

        connect(&dialogSearchValues, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
        connect(this, SIGNAL(closeWidget_Multisearch()), &dialogSearchValues, SLOT(close()));

        XOptions::_adjustStayOnTop(&dialogSearchValues, true);

        dialogSearchValues.exec();

        setViewWidgetState(VIEWWIDGET_MULTISEARCH, false);
    } else {
        emit closeWidget_Multisearch();
    }
}

void XDeviceTableEditView::_bookmarkNew()
{
    if (getXInfoDB()) {
        DEVICESTATE state = getDeviceState();

        QString sComment =
            QString("%1 - %2").arg(QString::number(state.nSelectionDeviceOffset, 16), QString::number(state.nSelectionDeviceOffset + state.nSelectionSize, 16));

        XInfoDB::BOOKMARKRECORD record = {};
        record.sUUID = XBinary::generateUUID();
        record.sColorBackground = QColor(Qt::yellow).name();
        record.nLocation = state.nSelectionDeviceOffset;
        record.locationType = XBinary::LT_OFFSET;
        record.nSize = state.nSelectionSize;
        record.sComment = sComment;

        getXInfoDB()->addBookmarkRecord(record);

        getXInfoDB()->reloadView();
    }
}

void XDeviceTableEditView::_bookmarkList()
{
    if (getXInfoDB()) {
        if (!getViewWidgetState(VIEWWIDGET_BOOKMARKS)) {
            setViewWidgetState(VIEWWIDGET_BOOKMARKS, true);

            quint64 nLocation = 0;
            XIODevice *pSubDevice = dynamic_cast<XIODevice *>(getDevice());

            if (pSubDevice) {
                nLocation = pSubDevice->getInitLocation();
            }

            DialogBookmarks dialogBookmarks(this);

            dialogBookmarks.setData(getXInfoDB(), nLocation, -1, getDevice()->size());

            connect(&dialogBookmarks, SIGNAL(currentLocationChanged(quint64, qint32, qint64)), this, SLOT(currentLocationChangedSlot(quint64, qint32, qint64)));
            connect(this, SIGNAL(closeWidget_Bookmarks()), &dialogBookmarks, SLOT(close()));

            XOptions::_adjustStayOnTop(&dialogBookmarks, true);
            dialogBookmarks.exec();

            setViewWidgetState(VIEWWIDGET_BOOKMARKS, false);
        } else {
            emit closeWidget_Bookmarks();
        }
    }
}

void XDeviceTableEditView::_copyDataSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogShowData dialogShowData(this, getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);
    dialogShowData.setGlobal(getShortcuts(), getGlobalOptions());

    dialogShowData.exec();
}

void XDeviceTableEditView::_dumpToFileSlot()
{
    DEVICESTATE state = getDeviceState();

    dumpMemory(tr("Dump"), state.nSelectionDeviceOffset, state.nSelectionSize);
}

void XDeviceTableEditView::_hexSignatureSlot()
{
    DEVICESTATE state = getDeviceState();

    DialogHexSignature dhs(this, getDevice(), state.nSelectionDeviceOffset, state.nSelectionSize);
    dhs.setGlobal(getShortcuts(), getGlobalOptions());

    dhs.exec();
}
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
void XDeviceTableEditView::_scripts()
{
    if (!getViewWidgetState(VIEWWIDGET_SCRIPTS)) {
        setViewWidgetState(VIEWWIDGET_SCRIPTS, true);

        DialogDieHexViewer dialogDieHexViewer(this);

        connect(&dialogDieHexViewer, SIGNAL(currentAddressChanged(XADDR, qint64)), this, SLOT(goToAddressSlot(XADDR, qint64)));
        connect(this, SIGNAL(closeWidget_Scripts()), &dialogDieHexViewer, SLOT(close()));

        dialogDieHexViewer.setGlobal(getShortcuts(), getGlobalOptions());

        XOptions::_adjustStayOnTop(&dialogDieHexViewer, true);

        dialogDieHexViewer.exec();

        setViewWidgetState(VIEWWIDGET_SCRIPTS, false);
    } else {
        emit closeWidget_Scripts();
    }
}
#endif
