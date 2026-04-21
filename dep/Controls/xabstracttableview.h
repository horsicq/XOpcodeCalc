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
#ifndef XABSTRACTTABLEVIEW_H
#define XABSTRACTTABLEVIEW_H

#include <QApplication>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QShortcut>
#include <QStyleOptionButton>
#include <QTimer>
#include <QElapsedTimer>
#include <QMutex>

#include "xshortcutstscrollarea.h"

class XAbstractTableView : public XShortcutstScrollArea {
    Q_OBJECT

public:
    struct COLUMN {
        bool bEnable;
        qreal nLeft;
        qreal nWidth;
        QString sTitle;
        bool bClickable;
    };

    enum PT {
        PT_UNKNOWN = 0,
        PT_HEADER,
        PT_CELL,
        PT_MAP,
        PT_MAPHEADER,
        // TODO Check more
    };

    struct CURSOR_POSITION {
        bool bIsValid;
        PT ptype;
        qint32 nY;
        qint32 nX;
        qint32 nRow;
        qint32 nColumn;
        qint32 nAreaTop;
        qint32 nAreaLeft;
        qint32 nPercentage;
        bool bResizeColumn;
    };

    struct STATE {
        //        qint64 nCursorViewPos;
        QVariant varCursorExtraInfo;
        XVPOS nSelectionViewPos;
        qint64 nSelectionViewSize;
        CURSOR_POSITION cursorPosition;
    };

    struct OS {
        XVPOS nViewPos;
        qint64 nSize;
        QVariant varData;
    };

    enum XCOLOR {
        XCOLOR_NORMALTEXT = 0,
        XCOLOR_NORMALTEXTBACKGROUND
    };

    struct PAINT_STATISTICS {
        qint64 nPaintCount;
        qint64 nTotalPaintTime;
        qint64 nMinPaintTime;
        qint64 nMaxPaintTime;
        qint64 nLastPaintTime;
    };

    explicit XAbstractTableView(QWidget *pParent = nullptr);
    ~XAbstractTableView();

    void setActive(bool bIsActive);
    bool isActive();
    //    void setEnabled(bool bEnabled);
    void clear();

    void addColumn(const QString &sTitle, qint32 nWidth = 0, bool bClickable = false, bool bEnable = true);
    void setColumnTitle(qint32 nNumber, const QString &sTitle);
    void setColumnEnabled(qint32 nNumber, bool bState);
    void setColumnWidth(qint32 nNumber, qint32 nWidth);
    qint32 getColumnWidth(qint32 nNumber);

    void reload(bool bUpdateData = false);
    virtual void reloadData(bool bSaveSelection) override;

    void setTextFont(const QFont &font);
    QFont getTextFont();

    bool setTextFontFromOptions(XOptions::ID id);
    void setTotalScrollCount(qint64 nValue);
    quint64 getTotalScrollCount();

    void setLineDelta(qint32 nValue);
    void setSideDelta(qint32 nValue);
    qint32 getLinesProPage();

    void setViewPosStart(qint64 nValue);
    XVPOS getViewPosStart();

    qreal getCharWidth();

    CURSOR_POSITION getCursorPosition(const QPoint &pos);

    bool isViewPosSelected(XVPOS nViewPos);
    QPainter *getBoldTextPointer();
    qint32 getLineDelta();
    qint32 getSideDelta();
    STATE getState();
    void setState(STATE state);

    //    qint64 getCursorViewPos();
    //    void setCursorViewPos(qint64 nViewPos, qint32 nColumn = -1, QVariant varCursorExtraInfo = QVariant());
    void adjust(bool bUpdateData = false);

    //    void setCursorData(QRect rectSquare, QRect rectText, const QString &sText, qint32 nDelta);
    //    void resetCursorData();

    //    qint32 getCursorDelta();

    qint64 getMaxScrollValue();
    void setLastColumnStretch(bool bState);
    void setHeaderVisible(bool bState);
    void setColumnFixed(bool bState);
    void setVerticalLinesVisible(bool bState);
    void setHorisontalLinesVisible(bool bState);

    qint64 getSelectionInitOffset();
    qint64 getSelectionInitSize();
    void setCurrentBlock(qint64 nViewPos, qint64 nSize);
    bool isViewPosInCurrentBlock(qint64 nViewPos);
    qreal getLineHeight();
    qreal getHeaderHeight();
    void setHeaderHeight(qreal nHeight);
    void setSelectionEnable(bool bState);
    void setContextMenuEnable(bool bState);
    bool isContextMenuEnable();
    //    void setBlinkingCursor(bool bState);
    //    void setBlinkingCursorEnable(bool bState);
    void _verticalScroll();
    bool isSelectionEnable();
    void setMaxSelectionViewSize(qint64 nMaxSelectionViewSize);
    void setMapEnable(bool bState);
    bool isMapEnable();
    void setMapWidth(qint32 nWidth);
    qreal getMapWidth();
    void setMapCount(qint32 nMapCount);
    qint32 getMapCount();

    void enablePaintStatistics(bool bEnable);
    bool isPaintStatisticsEnabled() const;
    PAINT_STATISTICS getPaintStatistics() const;
    void resetPaintStatistics();

    virtual QList<XShortcuts::MENUITEM> getMenuItems();

signals:
    void cursorViewPosChanged(qint64 nViewPos);
    void selectionChanged();
    void errorMessage(const QString &sText);
    void infoMessage(const QString &sText);
    void headerClicked(qint32 nNumber);
    void cellDoubleClicked(qint32 nRow, qint32 nColumn);

protected:
    void _initSelection(XVPOS nViewPos, qint64 nSize);
    void _setSelection(XVPOS nViewPos, qint64 nSize);
    void _initSetSelection(XVPOS nViewPos, qint64 nSize);

private slots:
    void verticalScroll();
    void horizontalScroll();
    void _customContextMenu(const QPoint &pos);
    //    void updateBlink();

protected slots:
    void _copyValueSlot();  // TODO

protected:
    virtual void paintEvent(QPaintEvent *pEvent) override;
    virtual void resizeEvent(QResizeEvent *pEvent) override;
    virtual void mouseMoveEvent(QMouseEvent *pEvent) override;
    virtual void mousePressEvent(QMouseEvent *pEvent) override;
    virtual void mouseReleaseEvent(QMouseEvent *pEvent) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *pEvent) override;
    virtual void keyPressEvent(QKeyEvent *pEvent) override;
    virtual void wheelEvent(QWheelEvent *pEvent) override;
    virtual bool isViewPosValid(XVPOS nViewPos);
    virtual bool isEnd(XVPOS nViewPos);
    virtual OS cursorPositionToOS(const CURSOR_POSITION &cursorPosition);
    virtual void updateData();
    virtual void startPainting(QPainter *pPainter);
    virtual void paintColumn(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight);
    virtual void paintMap(QPainter *pPainter, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight);
    virtual void paintCell(QPainter *pPainter, qint32 nRow, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight);
    virtual void paintTitle(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight, const QString &sTitle);
    virtual void endPainting(QPainter *pPainter);
    virtual bool _goToViewPos(XVPOS nViewPos, bool bSaveCursor = false, bool bShort = false, bool bAprox = false);
    virtual XVPOS getCurrentViewPosFromScroll();
    virtual void setCurrentViewPosToScroll(XVPOS nViewPos);
    virtual void adjustColumns();
    virtual void adjustHeader();
    virtual void _headerClicked(qint32 nNumber);
    virtual void _cellDoubleClicked(qint32 nRow, qint32 nColumn);
    virtual qint64 getFixViewPos(XVPOS nViewPos);
    virtual void adjustMap();

private:
    bool m_bIsActive;
    //    bool m_bIsBlinkingCursorEnable;
    qint64 m_nNumberOfRows;
    QList<COLUMN> m_listColumns;
    qreal m_nHeaderHeight;
    QPushButton m_pushButtonHeader;
    qint32 m_nXViewPos;
    XVPOS m_nViewPosStart;  // TODO move to state
    qreal m_nCharWidth;
    qreal m_nCharHeight;
    qint32 m_nLinesProPage;
    qreal m_nLineHeight;
    QFont m_fontText;
    qint64 m_nTotalScrollCount;
    qint32 m_nViewWidth;
    qint32 m_nViewHeight;
    qint32 m_nTableWidth;
    qreal m_nLineDelta;
    qreal m_nSideDelta;

    STATE m_state;
    bool m_bMouseSelection;
    XVPOS m_nSelectionInitOffset;
    qint64 m_nSelectionInitSize;
    qint64 m_nMaxSelectionViewSize;
    bool m_bMouseResizeColumn;
    qint32 m_nResizeColumnNumber;
    bool m_bHeaderClickButton;
    qint32 m_nHeaderClickColumnNumber;

    //    QTimer m_timerCursor;
    //    QRect m_rectCursorSquare;
    //    QRect m_rectCursorText;
    //    QString m_sCursorText;
    //    qint32 m_nCursorDelta;
    bool m_bBlink;
    bool m_bLastColumnStretch;
    bool m_bHeaderVisible;
    bool m_bColumnFixed;
    bool m_bVerticalLinesVisible;
    bool m_bHorisontalLinesVisible;
    bool m_bIsMapEnable;
    qreal m_nMapWidth;
    qint32 m_nMapCount;
    qint64 m_nCurrentBlockViewPos;
    qint64 m_nCurrentBlockViewSize;
    bool m_bIsSelectionEnable;
    bool m_bIsContextMenuEnable;
    bool m_bPaintStatisticsEnabled;
    PAINT_STATISTICS m_paintStatistics;
};

#endif  // XABSTRACTTABLEVIEW_H
