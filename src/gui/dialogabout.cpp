/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#include "dialogabout.h"

#include "ui_dialogabout.h"

namespace {

QString centeredParagraph(const QString &text)
{
    return QStringLiteral("<p align=\"center\"><span style=\" font-weight:600;\">%1</span></p>").arg(text);
}

QString centeredLink(const QString &label, const QString &caption, const QString &url)
{
    return QStringLiteral(
               "<p align=\"center\"><span style=\" font-weight:600;\">%1: </span><a href=\"%2\"><span style=\" text-decoration: underline; color:#ff0000;\">%3</span></a></p>")
        .arg(label, url, caption);
}

QString centeredDonateLink(const QString &label, const QString &method, const QString &value)
{
    return QStringLiteral(
               "<p align=\"center\"><span style=\" font-weight:600;\">%1(%2): </span><a href=\"%3\"><span style=\" text-decoration: underline; color:#ff0000;\">%3</span></a></p>")
        .arg(label, method, value);
}

}  // namespace

XAboutWidget::DATA DialogAbout::createAboutData()
{
    XAboutWidget::DATA data = {};

    data.sInfo += centeredParagraph(XOptions::getTitle(X_APPLICATIONDISPLAYNAME, X_APPLICATIONVERSION));
    data.sInfo += centeredParagraph(QStringLiteral("Copyright (C) 2019-2026 hors"));
    data.sInfo += centeredLink(QObject::tr("Bugreports"), QStringLiteral("horsicq@gmail.com"), QStringLiteral("mailto:horsicq@gmail.com"));
    data.sInfo += centeredLink(QObject::tr("Website"), QStringLiteral("http://ntinfo.biz"), QStringLiteral("http://ntinfo.biz"));
    data.sInfo += centeredDonateLink(QObject::tr("Donate"), QStringLiteral("Paypal"), QStringLiteral("ntinfo.re@gmail.com"));
    data.sInfo += centeredDonateLink(QObject::tr("Donate"), QStringLiteral("BTC"), QStringLiteral("3DqddVBX9PKqMvNPXZ3gPHBNNRtD9CnmJo"));
    data.sInfo += centeredLink(QObject::tr("Source code"),
                               QStringLiteral("https://github.com/horsicq/XOpcodeCalc"),
                               QStringLiteral("https://github.com/horsicq/XOpcodeCalc"));

    data.sLibraries += centeredLink(QStringLiteral("QT Library %1").arg(QT_VERSION_STR),
                                    QStringLiteral("http://qt-project.org"),
                                    QStringLiteral("http://qt-project.org"));

    data.sLogoPath = QStringLiteral(":/images/main.png");
    data.sUpdatesLink = QStringLiteral("https://github.com/horsicq/XOpcodeCalc/releases");
    data.sThanksLink = QStringLiteral("https://github.com/horsicq/XOpcodeCalc/blob/master/THANKS.md");

    return data;
}

DialogAbout::DialogAbout(QWidget *pParent) : QDialog(pParent), ui(new Ui::DialogAbout)
{
    ui->setupUi(this);
    ui->widgetAbout->setData(createAboutData());
}

DialogAbout::~DialogAbout()
{
    delete ui;
}

void DialogAbout::on_pushButtonOK_clicked()
{
    accept();
}
