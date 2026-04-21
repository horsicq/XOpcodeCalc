/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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

#ifndef XFTABLEVIEW_H
#define XFTABLEVIEW_H

#include <QMenu>
#include <QAction>
#include "xtableview.h"
#include "xfmodel_header.h"
#include "xfmodel_table.h"

class XFTableView : public XTableView {
    Q_OBJECT

public:
    explicit XFTableView(QWidget *pParent = nullptr);
    virtual ~XFTableView();

    void setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader);
    void clear();
    void adjust();

    void setShowOffset(bool bShowOffset);
    void setShowPresentation(bool bShowPresentation);

signals:
    void fieldSelected(qint32 nFieldIndex, quint64 nValue, const XBinary::XFRECORD &xfRecord);
    void fieldDoubleClicked(qint32 nFieldIndex, quint64 nValue, const XBinary::XFRECORD &xfRecord);
    void rowSelected(qint32 nRow);

protected:
    virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous) override;

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onCustomContextMenuRequested(const QPoint &pos);

private:
    XBinary *m_pXBinary;
    XBinary::XFHEADER m_xfHeader;
    XFModel_header *m_pHeaderModel;
    XFModel_table *m_pTableModel;
};

#endif  // XFTABLEVIEW_H
