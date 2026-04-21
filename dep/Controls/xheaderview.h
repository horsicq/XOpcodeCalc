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
#ifndef XHEADERVIEW_H
#define XHEADERVIEW_H

#include <QHeaderView>
#include <QLineEdit>
#include <QObject>

class XHeaderView : public QHeaderView {
    Q_OBJECT

public:
    XHeaderView(QWidget *pParent = nullptr);
    ~XHeaderView();

    QSize sizeHint() const override;
    void setNumberOfFilters(qint32 nNumberOfFilters);
    void clearFilters();
    void updateGeometries() override;
    void adjustPositions();
    QList<QString> getFilters();
    void setFilterEnabled(qint32 nColumn, bool bFilterEnabled);

private slots:
    void _textChanged(const QString &sText);
    void onSectionResized(int i, int nOldSize, int nNewSize);
    void onSectionClicked(int logicalIndex);

signals:
    void filterChanged();

private:
    QList<QLineEdit *> m_listLineEdits;
};

#endif  // XHEADERVIEW_H
