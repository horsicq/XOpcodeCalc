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
#ifndef XUPDATE_H
#define XUPDATE_H

#include "xthreadobject.h"
#include "xgithub.h"
#include "xzip.h"

#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QTemporaryDir>

class XUpdate : public XThreadObject {
    Q_OBJECT

public:
    struct RECORD {
        QString sLocalPath;
        QString sReleaseURL;
        QString sZIPName;
    };

    explicit XUpdate(QObject *pParent = nullptr);

    // sURL: GitHub release tag URL, e.g. https://github.com/user/repo/releases/tag/tag
    // ZIP asset name is derived from the tag name as "{tag}.zip"
    void addRecord(const QString &sLocalPath, const QString &sURL);

    virtual void process() override;

private:
    struct GITHUB_RELEASE_INFO {
        bool bValid = false;
        QString sUserName;
        QString sRepoName;
        QString sTag;
    };

    static GITHUB_RELEASE_INFO _parseReleaseURL(const QString &sURL);
    static QDate _parseDate(const QString &sContent);
    static QString _getArchiveRootPrefix(const QList<XArchive::RECORD> &listRecords);
    static bool _findZIPRecord(const XGitHub::RELEASE_HEADER &releaseHeader, const QString &sZIPName, XGitHub::RELEASE_RECORD *pRecord);
    bool _downloadAndUnpack(const RECORD &record, const XGitHub::RELEASE_HEADER &releaseHeader, bool bClearDestination);

    QList<RECORD> m_listRecords;
};

#endif  // XUPDATE_H
