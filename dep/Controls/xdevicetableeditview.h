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
#ifndef XDEVICETABLEEDITVIEW_H
#define XDEVICETABLEEDITVIEW_H

#include "dialoghexedit.h"
#include "dialogsearchstrings.h"
#include "xdevicetableview.h"
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
#include "dialogdiehexviewer.h"
#endif
#include "dialogdatainspector.h"
#include "dialogxdataconvertor.h"
#include "dialogshowdata.h"
#include "dumpprocess.h"
#include "xdialogprocess.h"
#include "dialogresize.h"
#include "dialogremove.h"
#include "dialogsearchvalues.h"
#include "dialogvisualization.h"
#include "dialogbookmarks.h"

class XDeviceTableEditView : public XDeviceTableView {
    Q_OBJECT

public:
    struct HIGHLIGHTREGION {
        bool bIsValid;
        quint64 nLocation;
        XBinary::LT locationType;
        qint64 nSize;
        QColor colText;
        QColor colBackground;
        QColor colBackgroundSelected;
        QString sComment;
    };

    enum VIEWWIDGET {
        VIEWWIDGET_DATAINSPECTOR,
        VIEWWIDGET_DATACONVERTOR,
        VIEWWIDGET_MULTISEARCH,
        VIEWWIDGET_BOOKMARKS,
        VIEWWIDGET_STRINGS,
        VIEWWIDGET_VISUALIZATION,
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
        VIEWWIDGET_SCRIPTS,
#endif
    };

    XDeviceTableEditView(QWidget *pParent = nullptr);

    static QList<HIGHLIGHTREGION> _convertBookmarksToHighlightRegion(QVector<XInfoDB::BOOKMARKRECORD> *pList);
    static QList<HIGHLIGHTREGION> getHighlightRegion(QList<HIGHLIGHTREGION> *pList, quint64 nLocation, XBinary::LT locationType);

    void setViewWidgetState(VIEWWIDGET viewWidget, bool bState);
    bool getViewWidgetState(VIEWWIDGET viewWidget);
    void dumpMemory(const QString &sDumpName, qint64 nOffset = 0, qint64 nSize = -1);

public slots:
    void _editHex();
    void _editPatch();
    void _editRemove();
    void _editResize();
    void _strings();
    void _visualization();
    void _dataInspector();
    void _dataConvertor();
    void _multisearch();
    void _bookmarkNew();
    void _bookmarkList();
    void _copyDataSlot();
    void _dumpToFileSlot();
    void _hexSignatureSlot();
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
    void _scripts();
#endif

signals:
    void viewWidgetsStateChanged();
    void closeWidget_DataInspector();
    void closeWidget_DataConvertor();
    void closeWidget_Multisearch();
    void closeWidget_Strings();
    void closeWidget_Visualization();
    void closeWidget_Bookmarks();
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
    void closeWidget_Scripts();
#endif

private:
    QSet<VIEWWIDGET> m_stViewWidgetState;
};

#endif  // XDEVICETABLEEDITVIEW_H
