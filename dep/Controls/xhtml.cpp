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
#include "xhtml.h"

XHtml::XHtml(QObject *pParent) : QObject(pParent), m_bInList(false)
{
}

void XHtml::appendText(const QString &sText)
{
    m_sString.append(escapeHtml(sText));
}

void XHtml::appendHtml(const QString &sHtml)
{
    m_sString.append(sHtml);
}

void XHtml::appendParagraph(const QString &sText)
{
    m_sString.append(QString("<p>%1</p>").arg(escapeHtml(sText)));
}

void XHtml::appendHeading(qint32 nLevel, const QString &sText)
{
    if (nLevel < 1) nLevel = 1;
    if (nLevel > 6) nLevel = 6;

    m_sString.append(QString("<h%1>%2</h%1>").arg(nLevel).arg(escapeHtml(sText)));
}

void XHtml::appendLineBreak()
{
    m_sString.append("<br>");
}

void XHtml::appendHorizontalRule()
{
    m_sString.append("<hr>");
}

void XHtml::addTableBegin(qint32 nBorder, qint32 nCellPadding)
{
    m_sString.append(QString("<table border=\"%1\" style=\"border-style:solid;\" cellspacing=\"-1\" cellpadding=\"%2\">").arg(nBorder).arg(nCellPadding));
}

void XHtml::addTableRow(const QList<TABLECELL> &listTableCells)
{
    qint32 nNumbersOfCell = listTableCells.count();

    if (nNumbersOfCell) {
        m_sString.append("<tr>");

        for (qint32 i = 0; i < nNumbersOfCell; i++) {
            QString sAlign = alignmentToString(listTableCells.at(i).alignment);
            m_sString.append(QString("<td align=\"%1\">").arg(sAlign));

            QString sCellText = escapeHtml(listTableCells.at(i).sText);

            if (listTableCells.at(i).bBold) {
                sCellText = QString("<b>%1</b>").arg(sCellText);
            }

            m_sString.append(sCellText);
            m_sString.append("</td>");
        }

        m_sString.append("</tr>");
    }
}

void XHtml::addTableEnd()
{
    m_sString.append("</table>");
}

void XHtml::addListBegin(bool bOrdered)
{
    if (bOrdered) {
        m_sString.append("<ol>");
    } else {
        m_sString.append("<ul>");
    }
    m_bInList = true;
}

void XHtml::addListItem(const QString &sText)
{
    m_sString.append(QString("<li>%1</li>").arg(escapeHtml(sText)));
}

void XHtml::addListEnd()
{
    if (m_bInList) {
        if (m_sString.contains("<ol>")) {
            m_sString.append("</ol>");
        } else {
            m_sString.append("</ul>");
        }
        m_bInList = false;
    }
}

void XHtml::clear()
{
    m_sString.clear();
    m_bInList = false;
}

QString XHtml::toString() const
{
    return m_sString;
}

QString XHtml::makeLink(const QString &sText, const QString &sLink)
{
    QString sResult;

    if (sLink != "") {
        sResult = QString("<a href=\"%1\">%2</a>").arg(escapeHtml(sLink), escapeHtml(sText));
    } else {
        sResult = escapeHtml(sText);
    }

    return sResult;
}

QString XHtml::escapeHtml(const QString &sText)
{
    QString sResult = sText;
    sResult.replace("&", "&amp;");
    sResult.replace("<", "&lt;");
    sResult.replace(">", "&gt;");
    sResult.replace("\"", "&quot;");
    sResult.replace("'", "&#39;");
    return sResult;
}

QString XHtml::makeBold(const QString &sText)
{
    return QString("<b>%1</b>").arg(escapeHtml(sText));
}

QString XHtml::makeItalic(const QString &sText)
{
    return QString("<i>%1</i>").arg(escapeHtml(sText));
}

QString XHtml::alignmentToString(Qt::Alignment alignment)
{
    if (alignment & Qt::AlignLeft) {
        return "left";
    } else if (alignment & Qt::AlignRight) {
        return "right";
    } else if (alignment & Qt::AlignHCenter) {
        return "center";
    } else if (alignment & Qt::AlignJustify) {
        return "justify";
    }

    return "center";  // Default alignment
}
