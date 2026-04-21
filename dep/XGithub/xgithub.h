/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
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
#ifndef XGITHUB_H
#define XGITHUB_H

#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkProxy>
#include <QNetworkReply>

class XGitHub : public QObject {
    Q_OBJECT

public:
    struct RELEASE_RECORD {
        QString sName;
        QString sSrc;
        qint64 nSize;
        QDateTime dt;
    };

    struct RELEASE_HEADER {
        bool bValid;
        bool bNetworkError;
        QString sName;
        QString sTag;
        QString sBody;
        QDateTime dt;
        QList<RELEASE_RECORD> listRecords;
    };

    explicit XGitHub(const QString &sUserName, const QString &sRepoName, QObject *pParent = nullptr);
    ~XGitHub();

    RELEASE_HEADER getLatestRelease(bool bPrerelease);
    RELEASE_HEADER getTagRelease(QString sTag);
    static QList<QString> getDownloadLinks(QString sString);
    void setCredentials(QString sUser, QString sToken);

    struct WEBFILE {
        bool bValid;
        QString sContent;
        QString sNetworkError;
        bool bRedirect;
        QString sRedirectUrl;
    };

    static WEBFILE getWebFile(const QString &sUrl);
    static bool downloadFile(const QString &sUrl, const QString &sLocalFilePath);

private:
    RELEASE_HEADER _handleReleaseJson(QJsonObject jsonObject);
    RELEASE_HEADER _getRelease(const QString &sUrl);

signals:
    void errorMessage(QString sText);

private:
    QString m_sUserName;
    QString m_sRepoName;
    QString m_sAuthUser;
    QString m_sAuthToken;
    bool m_bIsStop;
    QSet<QNetworkReply *> m_stReplies;
    QNetworkAccessManager m_naManager;
};

#endif  // XGITHUB_H
