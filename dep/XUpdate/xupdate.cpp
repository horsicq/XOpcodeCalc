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
#include "xupdate.h"

namespace {
bool movePathRecursive(const QString &sSourcePath, const QString &sDestinationPath)
{
    QFileInfo sourceInfo(sSourcePath);

    if (!sourceInfo.exists()) {
        return false;
    }

    if (sourceInfo.isDir()) {
        QFileInfo destinationInfo(sDestinationPath);

        if (destinationInfo.exists() && (!destinationInfo.isDir())) {
            QFile::remove(sDestinationPath);
        }

        if (!QDir().mkpath(sDestinationPath)) {
            return false;
        }

        QDir sourceDir(sSourcePath);
        QFileInfoList listEntries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System);

        for (const QFileInfo &entryInfo : listEntries) {
            QString sChildDestination = QDir(sDestinationPath).filePath(entryInfo.fileName());

            if (!movePathRecursive(entryInfo.absoluteFilePath(), sChildDestination)) {
                return false;
            }
        }

        return QDir().rmdir(sSourcePath);
    }

    QFileInfo destinationInfo(sDestinationPath);

    if (destinationInfo.exists() && destinationInfo.isDir()) {
        QDir(sDestinationPath).removeRecursively();
    } else if (destinationInfo.exists()) {
        QFile::remove(sDestinationPath);
    }

    QFileInfo destinationParentInfo(sDestinationPath);
    if (!QDir().mkpath(destinationParentInfo.absolutePath())) {
        return false;
    }

    if (QFile::rename(sSourcePath, sDestinationPath)) {
        return true;
    }

    if (QFile::copy(sSourcePath, sDestinationPath)) {
        return QFile::remove(sSourcePath);
    }

    return false;
}

bool moveDirectoryContents(const QString &sSourceDirectory, const QString &sDestinationDirectory)
{
    QDir sourceDir(sSourceDirectory);

    if (!sourceDir.exists()) {
        return false;
    }

    if (!QDir().mkpath(sDestinationDirectory)) {
        return false;
    }

    QFileInfoList listEntries = sourceDir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries | QDir::Hidden | QDir::System);

    for (const QFileInfo &entryInfo : listEntries) {
        QString sDestinationPath = QDir(sDestinationDirectory).filePath(entryInfo.fileName());

        if (!movePathRecursive(entryInfo.absoluteFilePath(), sDestinationPath)) {
            return false;
        }
    }

    return true;
}
}  // namespace

XUpdate::XUpdate(QObject *pParent) : XThreadObject(pParent)
{
}

void XUpdate::addRecord(const QString &sLocalPath, const QString &sURL)
{
    RECORD record;
    record.sLocalPath = QDir::cleanPath(sLocalPath);
    record.sReleaseURL = sURL;

    GITHUB_RELEASE_INFO releaseInfo = _parseReleaseURL(sURL);
    if (releaseInfo.bValid) {
        record.sZIPName = releaseInfo.sTag + QStringLiteral(".zip");
    }

    m_listRecords.append(record);
}

XUpdate::GITHUB_RELEASE_INFO XUpdate::_parseReleaseURL(const QString &sURL)
{
    GITHUB_RELEASE_INFO result = {};

    static const QRegularExpression reReleaseUrl(
        QStringLiteral("^https?://github\\.com/([^/]+)/([^/]+)/releases/tag/([^/?#]+)(?:[/?#].*)?$"),
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match = reReleaseUrl.match(sURL.trimmed());

    if (match.hasMatch()) {
        result.bValid = true;
        result.sUserName = match.captured(1);
        result.sRepoName = match.captured(2);
        result.sTag = match.captured(3);
    }

    return result;
}

QDate XUpdate::_parseDate(const QString &sContent)
{
    for (const QString &sLine : sContent.split('\n')) {
        QString sL = sLine.trimmed();
        static const QRegularExpression reDate(QStringLiteral("^date\\s*=\\s*['\"]?(\\d{4}-\\d{2}-\\d{2})['\"]?$"));
        QRegularExpressionMatch match = reDate.match(sL);

        if (match.hasMatch()) {
            return QDate::fromString(match.captured(1), QStringLiteral("yyyy-MM-dd"));
        }
    }

    return QDate();
}

QString XUpdate::_getArchiveRootPrefix(const QList<XArchive::RECORD> &listRecords)
{
    QString sRootName;

    for (const XArchive::RECORD &record : listRecords) {
        QString sRecordName = record.spInfo.sRecordName;

        if (sRecordName.isEmpty()) {
            continue;
        }

        qint32 nSeparatorIndex = sRecordName.indexOf('/');

        if (nSeparatorIndex <= 0) {
            return QString();
        }

        QString sCurrentRootName = sRecordName.left(nSeparatorIndex);

        if (sRootName.isEmpty()) {
            sRootName = sCurrentRootName;
        } else if (sRootName != sCurrentRootName) {
            return QString();
        }
    }

    if (sRootName.isEmpty()) {
        return QString();
    }

    return sRootName + QStringLiteral("/");
}

bool XUpdate::_findZIPRecord(const XGitHub::RELEASE_HEADER &releaseHeader, const QString &sZIPName, XGitHub::RELEASE_RECORD *pRecord)
{
    bool bFound = false;
    XGitHub::RELEASE_RECORD firstZIPRecord = {};

    for (const XGitHub::RELEASE_RECORD &record : releaseHeader.listRecords) {
        if ((!sZIPName.isEmpty()) && (record.sName.compare(sZIPName, Qt::CaseInsensitive) == 0)) {
            if (pRecord) {
                *pRecord = record;
            }

            return true;
        }

        if ((!bFound) && record.sName.endsWith(QStringLiteral(".zip"), Qt::CaseInsensitive)) {
            firstZIPRecord = record;
            bFound = true;
        }
    }

    if (bFound && pRecord) {
        *pRecord = firstZIPRecord;
    }

    return bFound;
}

bool XUpdate::_downloadAndUnpack(const RECORD &record, const XGitHub::RELEASE_HEADER &releaseHeader, bool bClearDestination)
{
    XGitHub::RELEASE_RECORD zipRecord = {};

    if (!_findZIPRecord(releaseHeader, record.sZIPName, &zipRecord)) {
        emit errorMessage(tr("Cannot find ZIP asset %1 in release %2.").arg(record.sZIPName, releaseHeader.sTag));
        return false;
    }

    QString sDisplayName = QFileInfo(record.sLocalPath).fileName();
    if (sDisplayName.isEmpty()) {
        sDisplayName = zipRecord.sName;
    }

    QString sTempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString sTempZip = QDir(sTempDir).filePath(zipRecord.sName);

    emit infoMessage(tr("Downloading %1") + QString("...").arg(zipRecord.sName));

    if (!XGitHub::downloadFile(zipRecord.sSrc, sTempZip)) {
        emit errorMessage(tr("Failed to download") + QString(": %1").arg(zipRecord.sSrc));
        return false;
    }

    emit infoMessage(tr("Extracting %1") + QString("...").arg(zipRecord.sName));

    if (bClearDestination) {
        QDir(record.sLocalPath).removeRecursively();
    }
    QDir().mkpath(record.sLocalPath);

    bool bSuccess = false;

    QFile file(sTempZip);
    if (file.open(QIODevice::ReadOnly)) {
        XZip xzip(&file);
        if (xzip.isValid()) {
            XBinary::PDSTRUCT pdStruct = XBinary::createPdStruct();
            QList<XArchive::RECORD> listRecords = xzip.getRecords(-1, &pdStruct);
            QString sRootPrefix = _getArchiveRootPrefix(listRecords);
            QString sRootDirectoryName = sRootPrefix;

            if (sRootDirectoryName.endsWith('/')) {
                sRootDirectoryName.chop(1);
            }

            QTemporaryDir unpackDir(QDir(sTempDir).filePath(QStringLiteral("xupdate_unpack_XXXXXX")));

            if (unpackDir.isValid()) {
                bSuccess = xzip.unpackToFolder(unpackDir.path(), QMap<XBinary::UNPACK_PROP, QVariant>(), &pdStruct);

                if (bSuccess) {
                    QString sSourcePath = unpackDir.path();

                    if (!sRootDirectoryName.isEmpty()) {
                        QString sRootPath = QDir(unpackDir.path()).filePath(sRootDirectoryName);

                        if (QDir(sRootPath).exists()) {
                            sSourcePath = sRootPath;
                        }
                    }

                    bSuccess = moveDirectoryContents(sSourcePath, record.sLocalPath);
                }
            } else {
                emit errorMessage(tr("Cannot create temporary unpack directory."));
            }
        } else {
            emit errorMessage(tr("Invalid ZIP archive") + QString(": %1").arg(sTempZip));
        }
        file.close();
    } else {
        emit errorMessage(tr("Cannot open downloaded ZIP") + QString(": %1").arg(sTempZip));
    }

    QFile::remove(sTempZip);

    if (bSuccess) {
        emit infoMessage((QString("%1 ") + tr("updated successfully.")).arg(sDisplayName));
    } else {
        emit errorMessage(tr("Failed to extract %1.").arg(sDisplayName));
    }

    return bSuccess;
}

void XUpdate::process()
{
    for (int i = 0; i < m_listRecords.count(); i++) {
        const RECORD &record = m_listRecords.at(i);
        QString sFolderName = QFileInfo(record.sLocalPath).fileName();
        if (sFolderName.isEmpty()) {
            sFolderName = record.sZIPName;
        }

        GITHUB_RELEASE_INFO releaseInfo = _parseReleaseURL(record.sReleaseURL);

        if (!releaseInfo.bValid) {
            emit errorMessage(tr("Unsupported GitHub release URL") + QString(": %1").arg(record.sReleaseURL));
            continue;
        }

        emit infoMessage(tr("Checking %1") + QString("...").arg(sFolderName));

        XGitHub github(releaseInfo.sUserName, releaseInfo.sRepoName);
        connect(&github, &XGitHub::errorMessage, this, &XUpdate::errorMessage);
        XGitHub::RELEASE_HEADER releaseHeader = github.getTagRelease(releaseInfo.sTag);

        if (!releaseHeader.bValid) {
            emit errorMessage(tr("Cannot fetch release metadata for %1.").arg(sFolderName));
            continue;
        }

        QDate remoteDate = _parseDate(releaseHeader.sBody);

        if (!remoteDate.isValid()) {
            emit errorMessage(tr("Invalid release body for %1. Expected date=yyyy-MM-dd.").arg(sFolderName));
            continue;
        }

        QString sLocalInfoPath = QDir(record.sLocalPath).filePath(QStringLiteral("info.ini"));

        if (!QFile::exists(sLocalInfoPath)) {
            emit infoMessage(tr("No local info.ini for %1. Bootstrapping from GitHub release %2.")
                                 .arg(sFolderName, releaseInfo.sTag));
            _downloadAndUnpack(record, releaseHeader, false);
            continue;
        }

        QFile localFile(sLocalInfoPath);
        if (!localFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            emit warningMessage(tr("Cannot read local info.ini for %1, skipping.").arg(sFolderName));
            continue;
        }

        QDate localDate = _parseDate(QString::fromUtf8(localFile.readAll()));
        localFile.close();

        if (!localDate.isValid() || remoteDate > localDate) {
            emit infoMessage((tr("Updating") + QString(" %1: ") + tr("local") + QString("=%2, ") + tr("remote") + QString("=%3"))
                                 .arg(sFolderName)
                                 .arg(localDate.toString(QStringLiteral("yyyy-MM-dd")))
                                 .arg(remoteDate.toString(QStringLiteral("yyyy-MM-dd"))));
            _downloadAndUnpack(record, releaseHeader, true);
        } else {
            emit infoMessage((QString("%1 ") + tr("is up to date") + QString(" (%2)."))
                                 .arg(sFolderName)
                                 .arg(localDate.toString(QStringLiteral("yyyy-MM-dd"))));
        }
    }
}
