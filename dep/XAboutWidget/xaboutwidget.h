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
#ifndef XABOUTWIDGET_H
#define XABOUTWIDGET_H

#include <QClipboard>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#if (QT_VERSION_MAJOR > 4)
#include <QJsonDocument>
#include <QJsonObject>
#endif
#include <QListWidgetItem>
#include <QMessageBox>
#include <QUrl>
#include "xshortcutswidget.h"
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#elif (QT_VERSION_MAJOR >= 6)
#include <QRandomGenerator>
#endif
#ifdef QT_NETWORK_LIB
#include <QNetworkAccessManager>
#include <QNetworkReply>
#endif
// TODO XGitHub API for check versions

namespace Ui {
class XAboutWidget;
}

class XAboutWidget : public XShortcutsWidget {
    Q_OBJECT

public:
    struct DATA {
        QString sInfo;
        QString sUpdatesLink;
        QString sServerVersionLink;
        QString sLibraries;
        QString sLogoPath;
        QString sThanksLink;
    };

    explicit XAboutWidget(QWidget *pParent = nullptr);
    ~XAboutWidget();
    virtual void adjustView();
    virtual void reloadData(bool bSaveSelection);

    void setData(const DATA &data);

private slots:
    void on_pushButtonCheckUpdates_clicked();
    void on_labelInfo_linkActivated(const QString &sLink);
    void on_pushButtonFollowGitHub_clicked();
    void on_pushButtonFollowX_clicked();
    void on_pushButtonFollowYouTube_clicked();
    void on_pushButtonThanks_clicked();
    void on_pushButtonTelegram_clicked();
    void on_pushButtonDiscord_clicked();

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XAboutWidget *ui;
    DATA m_data;
};

#endif  // XABOUTWIDGET_H
