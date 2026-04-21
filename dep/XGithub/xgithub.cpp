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
#include "xgithub.h"

#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

namespace {
QString findCurlProgram()
{
    QString sProgram = QStandardPaths::findExecutable(QStringLiteral("curl.exe"));

    if (sProgram.isEmpty()) {
        sProgram = QStandardPaths::findExecutable(QStringLiteral("curl"));
    }

    return sProgram;
}

bool executeCurl(const QString &sProgram, const QStringList &listArguments, QByteArray *pStdOut, QString *pError)
{
    QProcess process;
    process.start(sProgram, listArguments);

    if (!process.waitForStarted(5000)) {
        if (pError) {
            *pError = QStringLiteral("Cannot start curl: %1").arg(process.errorString());
        }

        return false;
    }

    if (!process.waitForFinished(35000)) {
        process.kill();
        process.waitForFinished();

        if (pError) {
            *pError = QStringLiteral("curl timeout");
        }

        return false;
    }

    QByteArray baStdOut = process.readAllStandardOutput();
    QByteArray baStdErr = process.readAllStandardError();

    if (pStdOut) {
        *pStdOut = baStdOut;
    }

    if ((process.exitStatus() != QProcess::NormalExit) || (process.exitCode() != 0)) {
        if (pError) {
            *pError = QString::fromLocal8Bit(baStdErr).trimmed();

            if (pError->isEmpty()) {
                *pError = QStringLiteral("curl failed with exit code %1").arg(process.exitCode());
            }
        }

        return false;
    }

    return true;
}
}  // namespace

XGitHub::XGitHub(const QString &sUserName, const QString &sRepoName, QObject *pParent) : QObject(pParent)
{
    this->m_sUserName = sUserName;
    this->m_sRepoName = sRepoName;

    m_bIsStop = false;
}

XGitHub::~XGitHub()
{
    m_bIsStop = true;

    QSetIterator<QNetworkReply *> i(m_stReplies);

    while (i.hasNext()) {
        QNetworkReply *pReply = i.next();

        pReply->abort();
    }
    // TODO Check
    // TODO wait
}

XGitHub::RELEASE_HEADER XGitHub::getTagRelease(QString sTag)
{
    QString sURL = QString("https://api.github.com/repos/%1/%2/releases/tags/%3").arg(m_sUserName, m_sRepoName, sTag);

    return _getRelease(sURL);
}

XGitHub::RELEASE_HEADER XGitHub::getLatestRelease(bool bPrerelease)
{
    QString sURL;

    if (!bPrerelease) {
        sURL = QString("https://api.github.com/repos/%1/%2/releases/latest").arg(m_sUserName, m_sRepoName);
    } else {
        sURL = QString("https://api.github.com/repos/%1/%2/releases").arg(m_sUserName, m_sRepoName);
    }

    return _getRelease(sURL);
}

QList<QString> XGitHub::getDownloadLinks(QString sString)
{
    QList<QString> listResult;

    qint32 nCount = sString.count("](");

    for (qint32 i = 0; i < nCount; i++) {
        QString sLink = sString.section("](", i + 1, i + 1);
        sLink = sLink.section(")", 0, 0);

        listResult.append(sLink);
    }

    return listResult;
}

XGitHub::RELEASE_HEADER XGitHub::_handleReleaseJson(QJsonObject jsonObject)
{
    RELEASE_HEADER result = {};

    result.bValid = true;
    result.sName = jsonObject["name"].toString();
    result.sTag = jsonObject["tag_name"].toString();
    result.sBody = jsonObject["body"].toString();
    result.dt = QDateTime::fromString(jsonObject["published_at"].toString(), "yyyy-MM-ddThh:mm:ssZ");

    QJsonArray jsonArray = jsonObject["assets"].toArray();

    qint32 nCount = jsonArray.count();

    for (qint32 i = 0; i < nCount; i++) {
        RELEASE_RECORD record = {};

        QJsonObject _object = jsonArray.at(i).toObject();

        record.sSrc = _object["browser_download_url"].toString();
        record.nSize = _object["size"].toInt();
        record.sName = _object["name"].toString();
        record.dt = QDateTime::fromString(_object["updated_at"].toString(), "yyyy-MM-ddThh:mm:ssZ");

        result.listRecords.append(record);
    }

    return result;
}

void XGitHub::setCredentials(QString sUser, QString sToken)
{
    m_sAuthUser = sUser;
    m_sAuthToken = sToken;
}

XGitHub::WEBFILE XGitHub::getWebFile(const QString &sUrl)
{
    WEBFILE result = {};
    QString sCurlProgram = findCurlProgram();

    if (!sCurlProgram.isEmpty()) {
        QByteArray baData;
        QString sError;
        QStringList listArguments;

        listArguments << QStringLiteral("--silent") << QStringLiteral("--show-error") << QStringLiteral("--location") << QStringLiteral("--fail")
                      << QStringLiteral("--max-time") << QStringLiteral("30") << QStringLiteral("--user-agent") << QStringLiteral("XGitHub/1.0")
                      << QStringLiteral("--header") << QStringLiteral("Accept: application/vnd.github+json") << sUrl;

        if (executeCurl(sCurlProgram, listArguments, &baData, &sError)) {
            result.sContent = QString::fromUtf8(baData);
            result.bValid = true;
        } else {
            result.bValid = false;
            result.sNetworkError = sError;
        }

        return result;
    }

    QNetworkAccessManager nam;
    nam.setProxy(QNetworkProxy::NoProxy);
    QUrl url(sUrl);
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "XGitHub/1.0");
    req.setRawHeader("Accept", "application/vnd.github+json");
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setTransferTimeout(30000);
    QNetworkReply *pReply = nam.get(req);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(&timer, &QTimer::timeout, pReply, &QNetworkReply::abort);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(30000);

    if (!(pReply->isFinished())) {
        loop.exec();
    }

    timer.stop();

    if (pReply->error() == QNetworkReply::NoError) {
        if (pReply->bytesAvailable()) {
            result.sContent = pReply->readAll();
            result.bValid = true;
        } else {
            QString sRedirectUrl = pReply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();

            if (sRedirectUrl != "") {
                result = getWebFile(sRedirectUrl);
                result.bRedirect = true;
                result.sRedirectUrl = sRedirectUrl;
            }
        }
    } else {
        result.bValid = false;
        result.sNetworkError = pReply->errorString();
    }

    pReply->deleteLater();

    return result;
}

bool XGitHub::downloadFile(const QString &sUrl, const QString &sLocalFilePath)
{
    bool bResult = false;
    QString sCurlProgram = findCurlProgram();

    if (!sCurlProgram.isEmpty()) {
        QString sError;
        QStringList listArguments;

        listArguments << QStringLiteral("--silent") << QStringLiteral("--show-error") << QStringLiteral("--location") << QStringLiteral("--fail")
                      << QStringLiteral("--max-time") << QStringLiteral("30") << QStringLiteral("--user-agent") << QStringLiteral("XGitHub/1.0")
                      << QStringLiteral("--header") << QStringLiteral("Accept: application/octet-stream") << QStringLiteral("--output") << sLocalFilePath
                      << sUrl;

        bResult = executeCurl(sCurlProgram, listArguments, nullptr, &sError);

        if ((!bResult) && QFile::exists(sLocalFilePath)) {
            QFile::remove(sLocalFilePath);
        }

        return bResult;
    }

    QNetworkAccessManager nam;
    nam.setProxy(QNetworkProxy::NoProxy);
    QUrl url(sUrl);
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "XGitHub/1.0");
    req.setRawHeader("Accept", "application/octet-stream");
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setTransferTimeout(30000);
    QNetworkReply *pReply = nam.get(req);
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(&timer, &QTimer::timeout, pReply, &QNetworkReply::abort);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(30000);

    if (!(pReply->isFinished())) {
        loop.exec();
    }

    timer.stop();

    if (pReply->error() == QNetworkReply::NoError) {
        QFile file(sLocalFilePath);

        if (file.open(QIODevice::WriteOnly)) {
            file.write(pReply->readAll());
            file.close();
            bResult = true;
        }
    }

    pReply->deleteLater();

    return bResult;
}

XGitHub::RELEASE_HEADER XGitHub::_getRelease(const QString &sUrl)
{
    XGitHub::RELEASE_HEADER result = {};
    QString sCurlProgram = findCurlProgram();

    if (!sCurlProgram.isEmpty()) {
        QByteArray baData;
        QString sError;
        QStringList listArguments;

        listArguments << QStringLiteral("--silent") << QStringLiteral("--show-error") << QStringLiteral("--location") << QStringLiteral("--fail")
                      << QStringLiteral("--max-time") << QStringLiteral("30") << QStringLiteral("--user-agent") << QStringLiteral("XGitHub/1.0")
                      << QStringLiteral("--header") << QStringLiteral("Accept: application/vnd.github+json");

        if (!m_sAuthUser.isEmpty()) {
            listArguments << QStringLiteral("--user") << QStringLiteral("%1:%2").arg(m_sAuthUser, m_sAuthToken);
        }

        listArguments << sUrl;

        if (executeCurl(sCurlProgram, listArguments, &baData, &sError)) {
            QJsonDocument document = QJsonDocument::fromJson(baData);

            if (document.isArray()) {
                QJsonArray jsArray = document.array();

                if (jsArray.count()) {
                    result = _handleReleaseJson(jsArray.at(0).toObject());
                }
            } else {
                result = _handleReleaseJson(document.object());
            }
        } else {
            emit errorMessage(sError);
            result.bNetworkError = true;
        }

        return result;
    }

    QNetworkAccessManager nam;
    nam.setProxy(QNetworkProxy::NoProxy);
    QNetworkRequest req;
    req.setUrl(QUrl(QString(sUrl)));
    req.setRawHeader("User-Agent", "XGitHub/1.0");
    req.setRawHeader("Accept", "application/vnd.github+json");
    req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    req.setTransferTimeout(30000);

    // Add credentials if supplied
    if (!m_sAuthUser.isEmpty()) {
        QString auth = m_sAuthUser + ":" + m_sAuthToken;
        auth = "Basic " + auth.toLocal8Bit().toBase64();
        req.setRawHeader("Authorization", auth.toLocal8Bit());
    }

    QNetworkReply *pReply = nam.get(req);

    m_stReplies.insert(pReply);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(pReply, SIGNAL(finished()), &loop, SLOT(quit()));
    QObject::connect(&timer, &QTimer::timeout, pReply, &QNetworkReply::abort);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start(30000);

    if (!(pReply->isFinished())) {
        loop.exec();
    }

    timer.stop();

    if (!m_bIsStop) {
        if (!pReply->error()) {
            QByteArray baData = pReply->readAll();
            QJsonDocument document = QJsonDocument::fromJson(baData);

#ifdef QT_DEBUG
            QString strJson(document.toJson(QJsonDocument::Indented));
            qDebug(strJson.toLatin1().data());
#endif

            if (document.isArray()) {
                QJsonArray jsArray = document.array();

                if (jsArray.count()) {
                    result = _handleReleaseJson(jsArray.at(0).toObject());
                }
            } else {
                result = _handleReleaseJson(document.object());
            }
        } else {
            QString sErrorString = pReply->errorString();

            if (pReply->error() == QNetworkReply::OperationCanceledError) {
                sErrorString = QStringLiteral("Request timeout: %1").arg(sUrl);
            }

            if (sErrorString.contains("server replied: rate limit exceeded\n")) {
                sErrorString += "Github has the limit is 60 requests per hour for unauthenticated users (and 5000 for authenticated users).\n\n";
                sErrorString += "TRY AGAIN IN ONE HOUR!";
            }

            emit errorMessage(sErrorString);

#ifdef QT_DEBUG
            qDebug(sErrorString.toLatin1().data());
#endif

            result.bNetworkError = true;
        }
    }

    m_stReplies.remove(pReply);
    pReply->deleteLater();

    return result;
}
