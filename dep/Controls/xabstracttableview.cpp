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
#include "xabstracttableview.h"

XAbstractTableView::XAbstractTableView(QWidget *pParent) : XShortcutstScrollArea(pParent)
{
    m_bIsActive = false;
    //    m_bIsBlinkingCursorEnable = false;
    m_bMouseResizeColumn = false;
    m_bMouseSelection = false;
    m_nViewPosStart = 0;
    m_nCharWidth = 0;
    m_nCharHeight = 0;
    m_nLinesProPage = 0;
    m_nLineHeight = 0;
    m_nTotalScrollCount = 0;
    m_nViewWidth = 0;
    m_nViewHeight = 0;
    m_nTableWidth = 0;
    m_nSelectionInitOffset = -1;
    m_nSelectionInitSize = 0;
    m_nNumberOfRows = 0;
    //    m_nCursorDelta = 0;
    m_nXViewPos = 0;
    m_nHeaderHeight = 20;
    m_nLineDelta = 0;
    m_nSideDelta = 0;
    m_state = {};
    m_bBlink = false;
    m_bLastColumnStretch = false;
    m_bHeaderVisible = false;
    m_bColumnFixed = false;
    m_bVerticalLinesVisible = false;
    m_bHorisontalLinesVisible = false;
    m_bIsMapEnable = false;
    m_nMapWidth = 0;
    m_nMapCount = 1;

    m_nResizeColumnNumber = 0;

    m_bHeaderClickButton = false;
    m_nHeaderClickColumnNumber = 0;

    m_nCurrentBlockViewPos = 0;
    m_nCurrentBlockViewSize = 0;

    m_bIsSelectionEnable = true;
    m_nMaxSelectionViewSize = 0;
    m_bIsContextMenuEnable = true;
    m_bPaintStatisticsEnabled = false;
    m_paintStatistics = {};

    setContextMenuPolicy(Qt::CustomContextMenu);

    setHeaderVisible(true);
    setColumnFixed(false);
    setVerticalLinesVisible(true);
    setLineDelta(0);
    setSideDelta(3);
    // TODO Cursor off default !!! // TODO Check

    // installEventFilter(this);  // mb TODO move to setActive
}

XAbstractTableView::~XAbstractTableView()
{
}

void XAbstractTableView::setActive(bool bIsActive)
{
    bool bChanged = (m_bIsActive != bIsActive);

    if (bChanged) {
        if (bIsActive) {
            connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_customContextMenu(QPoint)));
            connect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll()));
            connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll()));
        } else {
            // TODO Check
            disconnect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(_customContextMenu(QPoint)));
            disconnect(verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(verticalScroll()));
            disconnect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll()));

            verticalScrollBar()->setRange(0, 0);
            horizontalScrollBar()->setRange(0, 0);
        }

        //        if (m_bIsBlinkingCursorEnable) {
        //            setBlinkingCursor(bIsActive);
        //        }

        setMouseTracking(bIsActive);  // Important

        m_bIsActive = bIsActive;
    }
}

bool XAbstractTableView::isActive()
{
    return m_bIsActive;
}

void XAbstractTableView::clear()
{
#ifdef QT_DEBUG
    qDebug("void XAbstractTableView::clear()");
#endif
    setActive(false);
    // TODO
    update();  // Check viewport()->update();
}

void XAbstractTableView::addColumn(const QString &sTitle, qint32 nWidth, bool bClickable, bool bEnable)
{
    COLUMN column = {};

    column.bEnable = bEnable;
    column.nWidth = nWidth;
    column.sTitle = sTitle;
    column.bClickable = bClickable;

    m_listColumns.append(column);
}

void XAbstractTableView::setColumnTitle(qint32 nNumber, const QString &sTitle)
{
    if (nNumber < m_listColumns.count()) {
        m_listColumns[nNumber].sTitle = sTitle;
    }
}

void XAbstractTableView::setColumnEnabled(qint32 nNumber, bool bState)
{
    if (nNumber < m_listColumns.count()) {
        m_listColumns[nNumber].bEnable = bState;
    }
}

void XAbstractTableView::setColumnWidth(qint32 nNumber, qint32 nWidth)
{
    if (nNumber < m_listColumns.count()) {
        m_listColumns[nNumber].nWidth = nWidth;
    }
}

qint32 XAbstractTableView::getColumnWidth(qint32 nNumber)
{
    qint32 nResult = 0;

    if (nNumber < m_listColumns.count()) {
        nResult = m_listColumns[nNumber].nWidth;
    }

    return nResult;
}

void XAbstractTableView::paintEvent(QPaintEvent *pEvent)
{
#ifdef QT_DEBUG
//    qDebug("XAbstractTableView::paintEvent");
//    QElapsedTimer timer;
//    timer.start();
#endif
    QElapsedTimer paintTimer;
    if (m_bPaintStatisticsEnabled) {
        paintTimer.start();
    }

    QPainter painter(this->viewport());
    painter.setFont(m_fontText);
    painter.setPen(viewport()->palette().color(QPalette::WindowText));
    painter.setBackgroundMode(Qt::TransparentMode);

    if (isActive()) {
        startPainting(&painter);

        // Cache frequently accessed values
        const qint32 nTopLeftY = pEvent->rect().topLeft().y();
        const qint32 nTopLeftX = pEvent->rect().topLeft().x() - m_nXViewPos;
        qint32 nScreenWidth = pEvent->rect().width();
        const qint32 nHeight = pEvent->rect().height();
        const qint32 nHeaderHeight = (m_bHeaderVisible) ? (m_nHeaderHeight) : (0);
        const QColor colorBase = viewport()->palette().color(QPalette::Base);
        const qint32 nNumberOfColumns = m_listColumns.count();

        if (isMapEnable()) {
            nScreenWidth -= getMapWidth();
        }

        if (nNumberOfColumns) {
            qint32 nX = nTopLeftX;

            // Single pass: paint columns, cells, headers, and lines together
            for (qint32 i = 0; i < nNumberOfColumns; i++) {
                if (m_listColumns.at(i).bEnable) {
                    const qint32 nColumnWidth = m_listColumns.at(i).nWidth;
                    const qint32 nContentTop = nTopLeftY + nHeaderHeight;
                    const qint32 nContentHeight = nHeight - nHeaderHeight;

                    // Fill background
                    painter.fillRect(nX, nContentTop, nColumnWidth, nContentHeight, colorBase);

                    // Paint column background
                    paintColumn(&painter, i, nX, nContentTop, nColumnWidth, nContentHeight);

                    // Paint cells
                    for (qint32 j = 0; j < m_nLinesProPage; j++) {
                        paintCell(&painter, j, i, nX, nContentTop + (j * m_nLineHeight), nColumnWidth, m_nLineHeight);
                    }

                    // Paint header if visible
                    if (nHeaderHeight > 0) {
                        QStyleOptionButton styleOptionButton;

                        if ((m_bHeaderClickButton) && (m_nHeaderClickColumnNumber == i)) {
                            styleOptionButton.state = QStyle::State_Raised;
                        } else {
                            styleOptionButton.state = QStyle::State_Enabled;
                        }

                        styleOptionButton.rect = QRect(nX, nTopLeftY, nColumnWidth, nHeaderHeight);

                        m_pushButtonHeader.style()->drawControl(QStyle::CE_PushButton, &styleOptionButton, &painter, &m_pushButtonHeader);

                        paintTitle(&painter, i, nX, nTopLeftY, nColumnWidth, nHeaderHeight, m_listColumns.at(i).sTitle);
                    }

                    // Draw vertical line
                    if (m_bVerticalLinesVisible) {
                        painter.drawLine(nX + nColumnWidth, nContentTop, nX + nColumnWidth, nTopLeftY + nHeight);
                    }

                    // Draw horizontal lines
                    if (m_bHorisontalLinesVisible) {
                        for (qint32 j = 0; j < m_nLinesProPage; j++) {
                            painter.drawLine(nX, nContentTop + ((j + 1) * m_nLineHeight), nX + nColumnWidth, nContentTop + (j + 1) * m_nLineHeight);
                        }
                    }

                    nX += nColumnWidth;
                }
            }

            // Fill remaining area
            if (nScreenWidth - nX > 0) {
                painter.fillRect(nX, nTopLeftY, nScreenWidth - nX, nHeight, colorBase);
            }

            // Paint map if enabled
            if (isMapEnable()) {
                const qint32 nMapWidth = getMapWidth();
                const qint32 nMapX = nScreenWidth;
                const qint32 nContentTop = nTopLeftY + nHeaderHeight;
                const qint32 nContentHeight = nHeight - nHeaderHeight;

                painter.fillRect(nMapX, nContentTop, nMapWidth, nContentHeight, colorBase);

                paintMap(&painter, nMapX, nContentTop, nMapWidth, nContentHeight);

                if (nHeaderHeight > 0) {
                    QStyleOptionButton styleOptionButton;
                    styleOptionButton.state = QStyle::State_Enabled;
                    styleOptionButton.rect = QRect(nMapX, nTopLeftY, nMapWidth, nHeaderHeight);

                    m_pushButtonHeader.style()->drawControl(QStyle::CE_PushButton, &styleOptionButton, &painter, &m_pushButtonHeader);
                }
            }
        }

        endPainting(&painter);
    }

    if (m_bPaintStatisticsEnabled) {
        qint64 nElapsed = paintTimer.elapsed();
        m_paintStatistics.nPaintCount++;
        m_paintStatistics.nTotalPaintTime += nElapsed;
        m_paintStatistics.nLastPaintTime = nElapsed;

        if (m_paintStatistics.nPaintCount == 1) {
            m_paintStatistics.nMinPaintTime = nElapsed;
            m_paintStatistics.nMaxPaintTime = nElapsed;
        } else {
            if (nElapsed < m_paintStatistics.nMinPaintTime) {
                m_paintStatistics.nMinPaintTime = nElapsed;
            }
            if (nElapsed > m_paintStatistics.nMaxPaintTime) {
                m_paintStatistics.nMaxPaintTime = nElapsed;
            }
        }
    }

#ifdef QT_DEBUG
//    qDebug("Elapsed XAbstractTableView::paintEvent %lld", timer.elapsed());
#endif
}

void XAbstractTableView::reload(bool bUpdateData)
{
    adjust(bUpdateData);
    viewport()->update();
}

void XAbstractTableView::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection);

    reload(true);
}

void XAbstractTableView::setTextFont(const QFont &font)
{
    const QFontMetricsF fm(font);
    m_nCharWidth = fm.maxWidth();  // mb TODO + nDelta TODO real
    m_nCharHeight = fm.height();

    m_fontText = font;

    adjustColumns();
    adjust();
    viewport()->update();
}

QFont XAbstractTableView::getTextFont()
{
    return m_fontText;
}

bool XAbstractTableView::setTextFontFromOptions(XOptions::ID id)
{
    bool bResult = false;

    QFont _font;
    QString sFont = getGlobalOptions()->getValue(id).toString();

    if (sFont != "") {
        if (_font.fromString(sFont)) {
            setTextFont(_font);
            bResult = true;
        }
    }

    return bResult;
}

void XAbstractTableView::setTotalScrollCount(qint64 nValue)
{
    qint32 nScrollValue = 0;

    // TODO fix scroll for the large files
    // mb flag for large files
    // TODO nValue - 1
    if (nValue > getMaxScrollValue()) {
        nScrollValue = (qint32)getMaxScrollValue();
    } else {
        nScrollValue = (qint32)nValue;
    }

    verticalScrollBar()->setRange(0, nScrollValue);

    m_nTotalScrollCount = nValue;
}

quint64 XAbstractTableView::getTotalScrollCount()
{
    return m_nTotalScrollCount;
}

void XAbstractTableView::setLineDelta(qint32 nValue)
{
    m_nLineDelta = nValue;
}

void XAbstractTableView::setSideDelta(qint32 nValue)
{
    m_nSideDelta = nValue;
}

qint32 XAbstractTableView::getLinesProPage()
{
    return m_nLinesProPage;
}

void XAbstractTableView::setViewPosStart(qint64 nValue)
{
    m_nViewPosStart = nValue;
}

XVPOS XAbstractTableView::getViewPosStart()
{
    return m_nViewPosStart;
}

qreal XAbstractTableView::getCharWidth()
{
    return m_nCharWidth;
}

XAbstractTableView::CURSOR_POSITION XAbstractTableView::getCursorPosition(const QPoint &pos)
{
    CURSOR_POSITION result = {};
    result.nY = pos.y();
    result.nX = pos.x() + m_nXViewPos;

    qint32 nHeaderHeight = (m_bHeaderVisible) ? (m_nHeaderHeight) : (0);

    qint32 nCurrentOffset = 0;
    qint32 nNumberOfColumns = m_listColumns.count();

    for (qint32 i = 0; i < nNumberOfColumns; i++) {
        if (m_listColumns.at(i).bEnable) {
            if ((result.nX >= nCurrentOffset) && (result.nX < (nCurrentOffset + m_listColumns.at(i).nWidth))) {
                result.bIsValid = true;
                result.nColumn = i;

                if (result.nY < nHeaderHeight) {
                    result.ptype = PT_HEADER;
                } else {
                    result.ptype = PT_CELL;
                    result.nRow = (result.nY - nHeaderHeight) / m_nLineHeight;
                    result.nAreaTop = (result.nY - nHeaderHeight) % (qint32)m_nLineHeight;
                    result.nAreaLeft = result.nX - nCurrentOffset;
                }

                if (result.nX >= (nCurrentOffset + m_listColumns.at(i).nWidth - m_nSideDelta)) {
                    if (!m_bColumnFixed) {
                        if (m_bVerticalLinesVisible) {
                            result.bResizeColumn = true;
                        } else {
                            if (result.nY < nHeaderHeight) {
                                result.bResizeColumn = true;
                            }
                        }

                        if ((result.nColumn == (nNumberOfColumns - 1)) && (m_bLastColumnStretch)) {
                            result.bResizeColumn = false;
                        }
                    }
                }

                break;
            }

            nCurrentOffset += m_listColumns.at(i).nWidth;
        }
    }

    if (isMapEnable()) {
        if ((result.nX >= (m_nViewWidth - m_nMapWidth)) && (result.nX < m_nViewWidth)) {
            result.bIsValid = true;

            if (result.nY < nHeaderHeight) {
                result.ptype = PT_MAPHEADER;
            } else {
                result.ptype = PT_MAP;
                result.nPercentage = ((result.nY - nHeaderHeight) * getMapCount()) / (m_nViewHeight - nHeaderHeight);
            }
        }
    }

    return result;
}

bool XAbstractTableView::isViewPosSelected(XVPOS nViewPos)
{
    bool bResult = false;

    if ((nViewPos >= m_state.nSelectionViewPos) && (nViewPos < (m_state.nSelectionViewPos + m_state.nSelectionViewSize))) {
        bResult = true;
    }

    return bResult;
}

qint32 XAbstractTableView::getLineDelta()
{
    return m_nLineDelta;
}

qint32 XAbstractTableView::getSideDelta()
{
    return m_nSideDelta;
}

XAbstractTableView::STATE XAbstractTableView::getState()
{
    return m_state;
}

void XAbstractTableView::setState(STATE state)
{
    m_state = state;

    emit selectionChanged();
}

// qint64 XAbstractTableView::getCursorViewPos()
//{
//     return m_state.nCursorViewPos;
// }

// void XAbstractTableView::setCursorViewPos(qint64 nViewPos, qint32 nColumn, QVariant varCursorExtraInfo)
//{
//     if (nColumn != -1) {
//         m_state.cursorPosition.nColumn = nColumn;
//     }

//    m_state.varCursorExtraInfo = varCursorExtraInfo;

//    if (m_state.nCursorViewPos != nViewPos) {
//        m_state.nCursorViewPos = nViewPos;

//        emit cursorViewPosChanged(nViewPos);
//    }
//}

void XAbstractTableView::_initSelection(XVPOS nViewPos, qint64 nSize)
{
    // #ifdef QT_DEBUG
    //     qDebug("_initSelection %llx, %llx", nViewPos, nSize);
    // #endif
    if (isViewPosValid(nViewPos) || isEnd(nViewPos)) {
        m_nSelectionInitOffset = nViewPos;
        m_nSelectionInitSize = nSize;
        m_state.nSelectionViewPos = nViewPos;
        m_state.nSelectionViewSize = 0;

        if (m_nMaxSelectionViewSize) {
            m_nSelectionInitSize = qMin(m_nSelectionInitSize, m_nMaxSelectionViewSize);
        }
    }
}

void XAbstractTableView::_setSelection(XVPOS nViewPos, qint64 nSize)
{
    // #ifdef QT_DEBUG
    //     qDebug("_setSelection %llx, %llx", nViewPos, nSize);
    // #endif
    if (isViewPosValid(nViewPos) || isEnd(nViewPos)) {
        if (nViewPos > m_nSelectionInitOffset) {
            m_state.nSelectionViewPos = m_nSelectionInitOffset;
            m_state.nSelectionViewSize = nViewPos - m_nSelectionInitOffset + nSize;
        } else {
            m_state.nSelectionViewPos = nViewPos;
            m_state.nSelectionViewSize = m_nSelectionInitOffset - nViewPos + m_nSelectionInitSize;
        }

        if (m_nMaxSelectionViewSize) {
            m_state.nSelectionViewSize = qMin(m_state.nSelectionViewSize, m_nMaxSelectionViewSize - (qint64)m_state.nSelectionViewPos);
        }

        if (m_state.nSelectionViewSize == 0) {
            m_state.nSelectionViewSize = 1;
        }

        emit selectionChanged();
    }
}

void XAbstractTableView::_initSetSelection(XVPOS nViewPos, qint64 nSize)
{
    if (nViewPos != -1) {
        _initSelection(nViewPos, nSize);
        _setSelection(nViewPos, nSize);
    }
}

void XAbstractTableView::verticalScroll()
{
    m_nViewPosStart = getCurrentViewPosFromScroll();

    adjust(true);
    viewport()->update();
}

void XAbstractTableView::horizontalScroll()
{
    adjust();
    viewport()->update();
}

void XAbstractTableView::adjust(bool bUpdateData)
{
    if (isActive()) {
        {
            m_nViewWidth = viewport()->width();
            m_nViewHeight = viewport()->height();

            m_nLineHeight = m_nCharHeight + m_nLineDelta;

            qint32 nHeaderHeight = (m_bHeaderVisible) ? (m_nHeaderHeight) : (0);

            qint32 nLinesProPage = 0;

            if (m_nLineHeight) {
                nLinesProPage = (m_nViewHeight - nHeaderHeight) / m_nLineHeight;
            }

            if (nLinesProPage < 0) {
                nLinesProPage = 0;
            }

            if (m_nLinesProPage != nLinesProPage) {
                bUpdateData = true;
                m_nLinesProPage = nLinesProPage;
            }

            m_nTableWidth = 0;
            qint32 nNumberOfColumns = m_listColumns.count();

            if (m_bLastColumnStretch) {
                if (nNumberOfColumns) {
                    nNumberOfColumns--;
                }
            }

            for (qint32 i = 0; i < nNumberOfColumns; i++) {
                if (m_listColumns.at(i).bEnable) {
                    m_listColumns[i].nLeft = m_nTableWidth;
                    m_nTableWidth += m_listColumns.at(i).nWidth;
                }
            }

            if (isMapEnable()) {
                m_nTableWidth += getMapWidth();
            }

            qint32 nDelta = m_nTableWidth - m_nViewWidth;

            if (m_bLastColumnStretch) {
                //            nDelta+=3; // TODO Check
                // TODO !!!
                qint32 _nNumberOfColumns = m_listColumns.count();

                if (nDelta < 0) {
                    m_listColumns[_nNumberOfColumns - 1].nWidth = -(nDelta);
                } else {
                    m_listColumns[_nNumberOfColumns - 1].nWidth = 0;
                }
            }

            horizontalScrollBar()->setRange(0, nDelta);
            horizontalScrollBar()->setPageStep(m_nViewWidth);

            m_nXViewPos = horizontalScrollBar()->value();

            if (bUpdateData) {
#ifdef QT_DEBUG
                QElapsedTimer timer;
                timer.start();
#endif
                updateData();
#ifdef QT_DEBUG
//                qDebug("updateData %lld", timer.elapsed());
#endif
            }

            //    resetCursor(); // TODO Check
            // TODO
        }
    }
}

void XAbstractTableView::adjustColumns()
{
    // TODO
}

void XAbstractTableView::adjustHeader()
{
}

void XAbstractTableView::_headerClicked(qint32 nNumber)
{
    adjustHeader();

    emit headerClicked(nNumber);
}

void XAbstractTableView::_cellDoubleClicked(qint32 nRow, qint32 nColumn)
{
    emit cellDoubleClicked(nRow, nColumn);
}

qint64 XAbstractTableView::getFixViewPos(XVPOS nViewPos)
{
    return nViewPos;
}

void XAbstractTableView::adjustMap()
{
}

// void XAbstractTableView::setCursorData(QRect rectSquare, QRect rectText, const QString &sText, qint32 nDelta)
//{
//     m_rectCursorSquare = rectSquare;
//     m_rectCursorText = rectText;
//     m_sCursorText = sText;
//     m_nCursorDelta = nDelta;
// }

// void XAbstractTableView::resetCursorData()
//{
//     setCursorData(QRect(), QRect(), "", 0);
// }

// qint32 XAbstractTableView::getCursorDelta()
//{
//     return m_nCursorDelta;
// }

qint64 XAbstractTableView::getMaxScrollValue()
{
    return 0x7FFFFFFF;
}

void XAbstractTableView::setLastColumnStretch(bool bState)
{
    m_bLastColumnStretch = bState;
}

void XAbstractTableView::setHeaderVisible(bool bState)
{
    m_bHeaderVisible = bState;
}

void XAbstractTableView::setColumnFixed(bool bState)
{
    m_bColumnFixed = bState;
}

void XAbstractTableView::setVerticalLinesVisible(bool bState)
{
    m_bVerticalLinesVisible = bState;
}

void XAbstractTableView::setHorisontalLinesVisible(bool bState)
{
    m_bHorisontalLinesVisible = bState;
}

qint64 XAbstractTableView::getSelectionInitOffset()
{
    return m_nSelectionInitOffset;
}

qint64 XAbstractTableView::getSelectionInitSize()
{
    return m_nSelectionInitSize;
}

void XAbstractTableView::setCurrentBlock(qint64 nViewPos, qint64 nSize)
{
    m_nCurrentBlockViewPos = nViewPos;
    m_nCurrentBlockViewSize = nSize;
}

bool XAbstractTableView::isViewPosInCurrentBlock(qint64 nViewPos)
{
    bool bResult = false;

    if (m_nCurrentBlockViewSize) {
        if ((m_nCurrentBlockViewPos <= nViewPos) && (nViewPos < (m_nCurrentBlockViewPos + m_nCurrentBlockViewSize))) {
            bResult = true;
        }
    }

    return bResult;
}

qreal XAbstractTableView::getLineHeight()
{
    return m_nLineHeight;
}

qreal XAbstractTableView::getHeaderHeight()
{
    return m_nHeaderHeight;
}

void XAbstractTableView::setHeaderHeight(qreal nHeight)
{
    m_nHeaderHeight = nHeight;
}

void XAbstractTableView::setSelectionEnable(bool bState)
{
    m_bIsSelectionEnable = bState;
}

void XAbstractTableView::setContextMenuEnable(bool bState)
{
    m_bIsContextMenuEnable = bState;
}

bool XAbstractTableView::isContextMenuEnable()
{
    return m_bIsContextMenuEnable;
}

// void XAbstractTableView::setBlinkingCursor(bool bState)
//{
//     if (bState) {
//         connect(&m_timerCursor, SIGNAL(timeout()), this, SLOT(updateBlink()));
//         m_timerCursor.setInterval(500);  // TODO Consts
//         m_timerCursor.start();
//     } else {
//         m_bBlink = true;

//        disconnect(&m_timerCursor, SIGNAL(timeout()), this, SLOT(updateBlink()));
//        m_timerCursor.stop();
//    }
//}

// void XAbstractTableView::setBlinkingCursorEnable(bool bState)
//{
//     if (m_bIsBlinkingCursorEnable != bState) {
//         m_bIsBlinkingCursorEnable = bState;

//        if (bState) {
//            if (isActive()) {
//                setBlinkingCursor(true);
//            }
//        } else {
//            setBlinkingCursor(false);
//        }
//    }
//}

void XAbstractTableView::_verticalScroll()
{
    verticalScroll();
}

bool XAbstractTableView::isSelectionEnable()
{
    return m_bIsSelectionEnable;
}

void XAbstractTableView::setMaxSelectionViewSize(qint64 nMaxSelectionViewSize)
{
    m_nMaxSelectionViewSize = nMaxSelectionViewSize;
}

void XAbstractTableView::setMapEnable(bool bState)
{
    m_bIsMapEnable = bState;
}

bool XAbstractTableView::isMapEnable()
{
    return m_bIsMapEnable;
}

void XAbstractTableView::setMapWidth(qint32 nWidth)
{
    m_nMapWidth = nWidth;
}

qreal XAbstractTableView::getMapWidth()
{
    return m_nMapWidth;
}

void XAbstractTableView::setMapCount(qint32 nMapCount)
{
    m_nMapCount = nMapCount;
}

qint32 XAbstractTableView::getMapCount()
{
    return m_nMapCount;
}

void XAbstractTableView::_customContextMenu(const QPoint &pos)
{
    if (isContextMenuEnable()) {
        QList<XShortcuts::MENUITEM> listMenuItems = getMenuItems();

        if (listMenuItems.count()) {
            QMenu contextMenu(this);

            getShortcuts()->adjustContextMenu(&contextMenu, &listMenuItems);

            contextMenu.exec(mapToGlobal(pos));
        }
    }
}

// void XAbstractTableView::updateBlink()
//{
//     if (isFocused()) {
//         m_bBlink = (bool)(!m_bBlink);
//         viewport()->update(m_rectCursorSquare);
//     } else {
//         if (!m_bBlink) {
//             m_bBlink = true;
//             viewport()->update(m_rectCursorSquare);
//         }
//     }
// }

void XAbstractTableView::_copyValueSlot()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        QString sValue = pAction->property("VALUE").toString();

        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(sValue);
    }
}

void XAbstractTableView::resizeEvent(QResizeEvent *pEvent)
{
    if (isActive()) {
        adjust();
        QAbstractScrollArea::resizeEvent(pEvent);
    }
}

void XAbstractTableView::mouseMoveEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());

        if (m_bIsSelectionEnable && m_bMouseSelection) {
            OS os = cursorPositionToOS(cursorPosition);

            if (os.nViewPos != -1) {
                //                m_state.nCursorOffset=os.nOffset;
                //                m_state.varCursorExtraInfo=os.varData;
                //                m_state.cursorPosition=cursorPosition;

                //                emit cursorChanged(os.nOffset);

                _setSelection(os.nViewPos, os.nSize);

                adjust();
                viewport()->update();
            }
        } else if (m_bMouseResizeColumn) {
            qreal nColumnWidth = qMax(m_nSideDelta, cursorPosition.nX - m_listColumns.at(m_nResizeColumnNumber).nLeft);

            m_listColumns[m_nResizeColumnNumber].nWidth = nColumnWidth;

            adjust();
            viewport()->update();
        } else if (pEvent->button() == Qt::NoButton) {
            if (cursorPosition.bResizeColumn) {
                setCursor(Qt::SplitHCursor);
            } else {
                unsetCursor();
            }
        }

        if (m_bHeaderClickButton) {
            m_bHeaderClickButton = ((cursorPosition.ptype == PT_HEADER) && (m_nHeaderClickColumnNumber == cursorPosition.nColumn));

            adjust();
            viewport()->update();
        }

        QAbstractScrollArea::mouseMoveEvent(pEvent);
    }
}

void XAbstractTableView::mousePressEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
            OS os = cursorPositionToOS(cursorPosition);

            if (cursorPosition.ptype == PT_MAPHEADER) {
            } else if (cursorPosition.ptype == PT_MAP) {
                _goToViewPos(os.nViewPos, false, false, true);
            } else if (cursorPosition.bResizeColumn) {
                m_bMouseResizeColumn = true;
                m_nResizeColumnNumber = cursorPosition.nColumn;
                setCursor(Qt::SplitHCursor);
            } else if ((cursorPosition.ptype == PT_HEADER) && (m_listColumns.at(cursorPosition.nColumn).bClickable)) {
                m_bHeaderClickButton = true;
                m_nHeaderClickColumnNumber = cursorPosition.nColumn;
            } else if (os.nViewPos != -1) {
                //                m_state.nCursorViewPos = os.nViewPos;
                //                m_state.varCursorExtraInfo = os.varData;
                m_state.cursorPosition = cursorPosition;
                m_state.varCursorExtraInfo = os.varData;

                if (m_bIsSelectionEnable) {
                    _initSelection(os.nViewPos, os.nSize);
                    m_bMouseSelection = true;
                }

                emit cursorViewPosChanged(os.nViewPos);
            }

            adjust();
            viewport()->update();
        }

        QAbstractScrollArea::mousePressEvent(pEvent);
    }
}

void XAbstractTableView::mouseReleaseEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            if (m_bHeaderClickButton) {
                _headerClicked(m_nHeaderClickColumnNumber);
            } else if (m_bIsSelectionEnable && m_bMouseSelection) {
                CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
                OS os = cursorPositionToOS(cursorPosition);

                _setSelection(os.nViewPos, os.nSize);

                //            if(m_state.nCursorOffset==os.nOffset)
                //            {
                //                _setSelection(os.nOffset+os.nSize-1);
                //            }
            }

            adjust();
            viewport()->update();
        }

        m_bMouseResizeColumn = false;
        m_bHeaderClickButton = false;

        if (m_bIsSelectionEnable) {
            m_bMouseSelection = false;
        }
    }
}

void XAbstractTableView::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
    if (isActive()) {
        if (pEvent->button() == Qt::LeftButton) {
            CURSOR_POSITION cursorPosition = getCursorPosition(pEvent->pos());
            OS os = cursorPositionToOS(cursorPosition);

            if ((cursorPosition.ptype == PT_HEADER) && (m_listColumns.at(cursorPosition.nColumn).bClickable)) {
                // TODO
            } else if (os.nViewPos != -1) {
                _cellDoubleClicked(cursorPosition.nRow, cursorPosition.nColumn);
            }

            //            adjust();
            //            viewport()->update();
        }
    }

    QAbstractScrollArea::mouseDoubleClickEvent(pEvent);
}

void XAbstractTableView::keyPressEvent(QKeyEvent *pEvent)
{
    QAbstractScrollArea::keyPressEvent(pEvent);
}

void XAbstractTableView::wheelEvent(QWheelEvent *pEvent)
{
    viewport()->update();
    QAbstractScrollArea::wheelEvent(pEvent);
}

bool XAbstractTableView::isViewPosValid(XVPOS nViewPos)
{
    Q_UNUSED(nViewPos)

    return false;
}

bool XAbstractTableView::isEnd(XVPOS nViewPos)
{
    Q_UNUSED(nViewPos)

    return false;
}

XAbstractTableView::OS XAbstractTableView::cursorPositionToOS(const XAbstractTableView::CURSOR_POSITION &cursorPosition)
{
    Q_UNUSED(cursorPosition)

    OS result = {};

    return result;
}

void XAbstractTableView::updateData()
{
}

void XAbstractTableView::startPainting(QPainter *pPainter)
{
    Q_UNUSED(pPainter)
}

void XAbstractTableView::paintColumn(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nColumn)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintMap(QPainter *pPainter, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintCell(QPainter *pPainter, qint32 nRow, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight)
{
    Q_UNUSED(pPainter)
    Q_UNUSED(nRow)
    Q_UNUSED(nColumn)
    Q_UNUSED(nLeft)
    Q_UNUSED(nTop)
    Q_UNUSED(nWidth)
    Q_UNUSED(nHeight)
}

void XAbstractTableView::paintTitle(QPainter *pPainter, qint32 nColumn, qint32 nLeft, qint32 nTop, qint32 nWidth, qint32 nHeight, const QString &sTitle)
{
    Q_UNUSED(nColumn)

    QRect rect = QRect(nLeft + 4, nTop, nWidth - 8, nHeight);
    pPainter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, sTitle);  // TODO alignment settings
}

void XAbstractTableView::endPainting(QPainter *pPainter)
{
    Q_UNUSED(pPainter)
}

bool XAbstractTableView::_goToViewPos(XVPOS nViewPos, bool bSaveCursor, bool bShort, bool bAprox)
{
    Q_UNUSED(bSaveCursor)

    bool bResult = false;

    if (isViewPosValid(nViewPos)) {
        if (bAprox) {
            nViewPos = getFixViewPos(nViewPos);
        }

        //        qint64 nCursorOffset = 0;

        //        if (bSaveCursor) {
        //            nCursorOffset = getCursorViewPos();
        //        } else {
        //            nCursorOffset = nViewPos;
        //        }

        bool bScroll = true;

        if (bShort && isViewPosInCurrentBlock(nViewPos)) {
            bScroll = false;
        }

        if (bScroll) {
            setCurrentViewPosToScroll(nViewPos);
        }

        //        setCursorViewPos(nCursorOffset);

        bResult = true;
    }

    return bResult;
}

QList<XShortcuts::MENUITEM> XAbstractTableView::getMenuItems()
{
    QList<XShortcuts::MENUITEM> listResult;

    return listResult;
}

XVPOS XAbstractTableView::getCurrentViewPosFromScroll()
{
    return verticalScrollBar()->value();  // TODO Check large files
}

void XAbstractTableView::setCurrentViewPosToScroll(XVPOS nViewPos)
{
    setViewPosStart(nViewPos);
    verticalScrollBar()->setValue((qint32)nViewPos);  // TODO Check large files
    adjust(true);
}

void XAbstractTableView::enablePaintStatistics(bool bEnable)
{
    m_bPaintStatisticsEnabled = bEnable;
    if (bEnable) {
        resetPaintStatistics();
    }
}

bool XAbstractTableView::isPaintStatisticsEnabled() const
{
    return m_bPaintStatisticsEnabled;
}

XAbstractTableView::PAINT_STATISTICS XAbstractTableView::getPaintStatistics() const
{
    return m_paintStatistics;
}

void XAbstractTableView::resetPaintStatistics()
{
    m_paintStatistics.nPaintCount = 0;
    m_paintStatistics.nTotalPaintTime = 0;
    m_paintStatistics.nMinPaintTime = 0;
    m_paintStatistics.nMaxPaintTime = 0;
    m_paintStatistics.nLastPaintTime = 0;
}
