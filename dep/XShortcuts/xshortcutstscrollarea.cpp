/* Copyright (c) 2021-2026 hors<horsicq@gmail.com>
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
#include "xshortcutstscrollarea.h"

XShortcutstScrollArea::XShortcutstScrollArea(QWidget *pParent) : QScrollArea(pParent)
{
    m_color[TCLOLOR_SELECTED] = getColorSelected(viewport());
    m_color[TCLOLOR_BREAKPOINT] = Qt::red;              // mb TODO
    m_color[TCLOLOR_ANALYSED] = QColor(100, 0, 0, 10);  // TODO

    installEventFilter(this);
}

XShortcutstScrollArea::~XShortcutstScrollArea()
{
#ifdef QT_DEBUG
    // if (g_pXOptions == &g_xOptionsEmpty) {
    //     qDebug("NO OPTIONS: %s", this->objectName().toLatin1().data());
    // }
#endif
}

void XShortcutstScrollArea::setGlobalChildren(QWidget *pWidget, XShortcuts *pShortcuts, XOptions *pXOptions)
{
    QList<XShortcutstScrollArea *> listAreas = pWidget->findChildren<XShortcutstScrollArea *>();

    qint32 nNumberOfAreas = listAreas.count();

    for (qint32 i = 0; i < nNumberOfAreas; i++) {
        XShortcutstScrollArea *pChild = dynamic_cast<XShortcutstScrollArea *>(listAreas.at(i));

        if (pChild) {
            pChild->setGlobal(pShortcuts, pXOptions);
        }
    }
}

void XShortcutstScrollArea::adjustViewChildren(QWidget *pWidget)
{
    QList<XShortcutstScrollArea *> listAreas = pWidget->findChildren<XShortcutstScrollArea *>();

    qint32 nNumberOfAreas = listAreas.count();

    for (qint32 i = 0; i < nNumberOfAreas; i++) {
        XShortcutstScrollArea *pChild = dynamic_cast<XShortcutstScrollArea *>(listAreas.at(i));

        if (pChild) {
            pChild->adjustView();
        }
    }
}

bool XShortcutstScrollArea::eventFilter(QObject *pObj, QEvent *pEvent)
{
    Q_UNUSED(pObj)

    if (pEvent->type() == QEvent::FocusIn) {
        // qDebug("QEvent::FocusIn");
        setActive(true);
        reloadShortcuts();
    } else if (pEvent->type() == QEvent::FocusOut) {
        // qDebug("QEvent::FocusOut");
        setActive(false);
        registerShortcuts(false);
    }

    return QAbstractScrollArea::eventFilter(pObj, pEvent);
}

QColor XShortcutstScrollArea::getColor(TCLOLOR tcolor)
{
    return m_color[tcolor];
}

void XShortcutstScrollArea::setColor(TCLOLOR tcolor, QColor color)
{
    m_color[tcolor] = color;
}

QColor XShortcutstScrollArea::getColorSelected(QColor color)
{
    color.setAlpha(50);
    color = color.darker(150);

    return color;
}

QColor XShortcutstScrollArea::getColorSelected(QWidget *pWidget)
{
    QColor colorBackground = pWidget->palette().window().color();
    return getColorSelected(colorBackground);
}
