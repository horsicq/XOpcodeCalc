/* Copyright (c) 2022-2026 hors<horsicq@gmail.com>
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
#include "xaboutwidget.h"

#include "ui_xaboutwidget.h"

XAboutWidget::XAboutWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XAboutWidget)
{
    ui->setupUi(this);
    ui->pushButtonCheckUpdates->setVisible(!XOptions::isNative());

    m_data = {};

    ui->labelDate->setText(__DATE__);
    ui->tabWidgetAbout->setCurrentIndex(0);
}

XAboutWidget::~XAboutWidget()
{
    delete ui;
}

void XAboutWidget::adjustView()
{
}

void XAboutWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
}

void XAboutWidget::setData(const DATA &data)
{
    m_data = data;

    ui->labelInfo->setText(data.sInfo);
    ui->labelLibraries->setText(data.sLibraries);

    QPixmap pixMap = QPixmap(data.sLogoPath);

    pixMap = pixMap.scaledToHeight(height());

    ui->labelLogo->setPixmap(pixMap);
}

void XAboutWidget::on_pushButtonCheckUpdates_clicked()
{
    // TODO GitHub API for checking version
#ifdef QT_NETWORK_LIB
    if (m_data.sServerVersionLink != "") {
        QNetworkAccessManager manager(this);
        QNetworkRequest request(QUrl(m_data.sServerVersionLink));
        QNetworkReply *pReply = manager.get(request);
        QEventLoop loop;
        QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();

        if (pReply->error() == QNetworkReply::NoError) {
            if (pReply->bytesAvailable()) {
                QByteArray baData = pReply->readAll();
                QString sVersion = QString(baData.data());

                if (QCoreApplication::applicationVersion().toDouble() < sVersion.toDouble()) {
                    if (QMessageBox::information(this, tr("Update information"),
                                                 QString("%1\r\n\r\n%2\r\n\r\n%3").arg(tr("New version available"), sVersion, tr("Go to download page?")),
                                                 QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes) {
                        QDesktopServices::openUrl(QUrl(m_data.sUpdatesLink));
                    }
                } else {
                    QMessageBox::information(this, tr("Update information"), tr("No update available"));
                }
            }
        } else {
            QMessageBox::critical(this, tr("Network error"), pReply->errorString());
        }
    } else {
        QDesktopServices::openUrl(QUrl(m_data.sUpdatesLink));
    }
#else
    QDesktopServices::openUrl(QUrl(m_data.sUpdatesLink));
#endif
}

void XAboutWidget::on_labelInfo_linkActivated(const QString &sLink)
{
    if (sLink.startsWith("http", Qt::CaseInsensitive)) {
        QDesktopServices::openUrl(sLink);
    } else {
        QApplication::clipboard()->setText(sLink);

        QMessageBox::information(this, tr("Information"), tr("The value copied to clipboard"));
    }
}

void XAboutWidget::on_pushButtonFollowGitHub_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://github.com/horsicq")));
}

void XAboutWidget::on_pushButtonFollowX_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://x.com/horsicq")));
}

void XAboutWidget::on_pushButtonFollowYouTube_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.youtube.com/@funreverseengineering")));
}

void XAboutWidget::on_pushButtonThanks_clicked()
{
    QDesktopServices::openUrl(QUrl(m_data.sThanksLink));
}

void XAboutWidget::on_pushButtonTelegram_clicked()
{
    QDesktopServices::openUrl(QUrl(QString("https://www.t.me/horsicq")));
}

void XAboutWidget::on_pushButtonDiscord_clicked()
{
    // Currently only possible over the Website to open the Discord profile. Not Desktop-App compatible!
    // User have to signed into Discord on the Website!
    QDesktopServices::openUrl(QUrl(QString("https://discord.com/users/641741884095725569")));
}

void XAboutWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}
