/* Copyright (c) 2024-2026 hors<horsicq@gmail.com>
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
#include "xtableview.h"
#include <QtConcurrent>

XTableView::XTableView(QWidget *pParent) : QTableView(pParent)
{
    m_pOldModel = nullptr;
    m_pModel = nullptr;
    m_pHeaderView = new XHeaderView(this);
    m_pSortFilterProxyModel = new XSortFilterProxyModel(this);
    m_bIsXmodel = false;
    m_bIsCustomFilter = false;
    m_bIsCustomSort = false;
    m_pXModel = nullptr;
    m_bIsStop = false;

    setHorizontalHeader(m_pHeaderView);

    connect(m_pHeaderView, SIGNAL(filterChanged()), this, SLOT(onFilterChanged()));
    connect(m_pHeaderView, SIGNAL(sortIndicatorChanged(int, Qt::SortOrder)), this, SLOT(onSortChanged(int, Qt::SortOrder)));
    connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(horizontalScroll()));
    connect(this, SIGNAL(invalidateSignal()), m_pSortFilterProxyModel, SLOT(invalidate()));

    m_pFilterTimer = new QTimer(this);
    m_pFilterTimer->setSingleShot(true);
    connect(m_pFilterTimer, SIGNAL(timeout()), this, SLOT(onFilterApply()));

    setSortingEnabled(true);
    setWordWrap(false);
    verticalHeader()->setDefaultSectionSize(verticalHeader()->minimumSectionSize());
}

XTableView::~XTableView()
{
#ifdef QT_DEBUG
    qDebug("XTableView::~XTableView(): Closing window");
#endif

    m_bIsStop = true;
    m_watcher.waitForFinished();
}

void XTableView::setCustomModel(QAbstractItemModel *pModel, bool bFilterEnabled)
{
    // TODO Stretch last section
    m_pOldModel = m_pModel;

    if (m_pOldModel) {
        // #ifdef QT_CONCURRENT_LIB
        // #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        //         QtConcurrent::run(&XTableView::deleteOldModel, this, &m_pOldModel);
        // #else
        //         QtConcurrent::run(this, &XTableView::deleteOldModel, &m_pOldModel);
        // #endif
        // #else
        //         deleteOldModel(&m_pOldModel);
        // #endif
        // m_pOldModel->beginResetModel();
#ifdef QT_DEBUG
        // get elapsed time
        QElapsedTimer timer;
        timer.start();
        qDebug("m_pOldModel->clear() START");
        // QStandartItemModel 433543
        // 470701 ms
#endif
        // m_pOldModel->clear();
        deleteOldModel(&m_pOldModel);
#ifdef QT_DEBUG
        qDebug("setCustomModel Elapsed time: %lld ms", timer.elapsed());
#endif
        // m_pOldModel->endResetModel();
    }

    m_pModel = pModel;

    if (bFilterEnabled) {
        m_pHeaderView->setNumberOfFilters(pModel->columnCount());
        m_pSortFilterProxyModel->setSourceModel(pModel);
        setModel(m_pSortFilterProxyModel);
    } else {
        setModel(pModel);
    }

    adjust();
}

void XTableView::clear()
{
    setModel(nullptr);
}

void XTableView::deleteOldModel(QAbstractItemModel **ppOldModel)
{
    delete (*ppOldModel);

    (*ppOldModel) = nullptr;
}

void XTableView::handleFilter()
{
    QList<QString> listFilters = m_listCurrentFilters;

    qint32 nNumberOfRows = m_pModel->rowCount();
    qint32 nNumberOfFilters = listFilters.count();

    // Clear all hidden states in bulk
    if (m_bIsXmodel) {
        m_pXModel->clearRowHidden();
    }

    // Pre-compute active filter columns to skip empty filters in inner loop
    QVector<qint32> vecActiveColumns;

    for (qint32 j = 0; j < nNumberOfFilters; j++) {
        if (!listFilters.at(j).isEmpty()) {
            vecActiveColumns.append(j);
        }
    }

    qint32 nActiveCount = vecActiveColumns.count();

    for (qint32 i = 0; (i < nNumberOfRows) && (!m_bIsStop); i++) {
        bool bHidden = false;

        for (qint32 k = 0; k < nActiveCount; k++) {
            qint32 nColumn = vecActiveColumns.at(k);
            QModelIndex index = m_pModel->index(i, nColumn);

            if (index.isValid()) {
                QString sValue = m_pModel->data(index).toString();

                if (!sValue.contains(listFilters.at(nColumn), Qt::CaseInsensitive)) {
                    bHidden = true;
                    break;
                }
            }
        }

        // Only set hidden=true; non-hidden rows already cleared by clearRowHidden()
        if (m_bIsXmodel && bHidden) {
            m_pXModel->setRowHidden(i, true);
        }
    }

    if (!m_bIsStop) {
#ifdef QT_DEBUG
        qDebug("XTableView::handleFilter(): Stop at invalid signal");
#endif
        m_pSortFilterProxyModel->blockSignals(true);
        m_pSortFilterProxyModel->invalidate();
        m_pSortFilterProxyModel->blockSignals(false);
        reset();
    } else {
#ifdef QT_DEBUG
        qDebug("XTableView::handleFilter() is stopped");
#endif
    }
}

XSortFilterProxyModel *XTableView::getProxyModel()
{
    return m_pSortFilterProxyModel;
}

void XTableView::setFilterEnabled(qint32 nColumn, bool bFilterEnabled)
{
    m_pHeaderView->setFilterEnabled(nColumn, bFilterEnabled);
}

void XTableView::setColumnFilterString(qint32 nColumn, const QString &sFilter)
{
    m_pSortFilterProxyModel->setColumnFilter(nColumn, sFilter);
    m_listCurrentFilters = m_pSortFilterProxyModel->getFilters();

    if (m_bIsCustomFilter) {
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;

        handleFilter();
    } else {
        m_pSortFilterProxyModel->invalidate();
    }
}

void XTableView::adjust()
{
    m_pXModel = dynamic_cast<XModel *>(m_pModel);

    if (m_pXModel) {
        qint32 nNumberOfColumns = m_pXModel->columnCount();

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 nSymbolSize = m_pXModel->getColumnSymbolSize(i);

            if (nSymbolSize != -1) {
                qint32 nWidth = XOptions::getControlWidth(this, nSymbolSize);
                setColumnWidth(i, nWidth);
            }
        }
        m_bIsXmodel = true;
        m_bIsCustomFilter = m_pXModel->isCustomFilter();
        m_bIsCustomSort = m_pXModel->isCustomSort();
    } else {
        m_bIsXmodel = false;
        m_bIsCustomFilter = false;
        m_bIsCustomSort = false;
    }
}

void XTableView::onFilterChanged()
{
    m_pFilterTimer->start(300);
}

void XTableView::onFilterApply()
{
#ifdef QT_DEBUG
    // Elapsed time
    QElapsedTimer timer;
    timer.start();
    qDebug("XTableView::onFilterApply(): START");
#endif

    QList<QString> listFilters = m_pHeaderView->getFilters();

    m_pSortFilterProxyModel->setFilters(listFilters);
    m_listCurrentFilters = listFilters;

    if (m_bIsCustomFilter) {
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture<void> future = QtConcurrent::run(&XTableView::handleFilter, this);
#else
        QFuture<void> future = QtConcurrent::run(this, &XTableView::handleFilter);
#endif
        m_watcher.setFuture(future);
    } else {
        emit invalidateSignal();
        // m_pSortFilterProxyModel->invalidate();
    }

#ifdef QT_DEBUG
    qDebug("XTableView::onFilterChanged(): Elapsed time: %lld ms", timer.elapsed());
    // 16266 ms
    // 16342
    //
    //
    qDebug("XTableView::onFilterChanged(): STOP");
#endif
}

void XTableView::onSortChanged(int column, Qt::SortOrder order)
{
    if (m_bIsCustomFilter) {
        m_bIsStop = true;
        m_watcher.waitForFinished();
        m_bIsStop = false;
    }

    if (m_bIsCustomSort) {
        m_pSortFilterProxyModel->sort(column, order);
    } else {
        m_pSortFilterProxyModel->sort(column, order);
    }

    if (m_bIsCustomFilter) {
        handleFilter();
    }
}

void XTableView::horizontalScroll()
{
    m_pHeaderView->adjustPositions();
}
