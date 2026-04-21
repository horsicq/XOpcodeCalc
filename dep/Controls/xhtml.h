/* Copyright (c) 2021-2026 hors<horsicq@gmail.com>
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
#ifndef XHTML_H
#define XHTML_H

#include <QObject>
#include <QString>

class XHtml : public QObject {
    Q_OBJECT

public:
    struct TABLECELL {
        QString sText;
        bool bBold;
        Qt::Alignment alignment;
    };

    explicit XHtml(QObject *pParent = nullptr);

    void appendText(const QString &sText);
    void appendHtml(const QString &sHtml);
    void appendParagraph(const QString &sText);
    void appendHeading(qint32 nLevel, const QString &sText);
    void appendLineBreak();
    void appendHorizontalRule();
    void addTableBegin(qint32 nBorder = 1, qint32 nCellPadding = 5);
    void addTableRow(const QList<TABLECELL> &listTableCells);
    void addTableEnd();
    void addListBegin(bool bOrdered = false);
    void addListItem(const QString &sText);
    void addListEnd();
    void clear();
    QString toString() const;
    static QString makeLink(const QString &sText, const QString &sLink);
    static QString escapeHtml(const QString &sText);
    static QString makeBold(const QString &sText);
    static QString makeItalic(const QString &sText);

private:
    static QString alignmentToString(Qt::Alignment alignment);

private:
    QString m_sString;
    bool m_bInList;
};

#endif  // XHTML_H
