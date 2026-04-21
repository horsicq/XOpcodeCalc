/* Copyright (c) 2026 hors<horsicq@gmail.com>
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
#ifndef XCOLORSTRING_H
#define XCOLORSTRING_H

#include <QMap>
#include <QSet>
#include <QString>
#include <QVariant>
#include <QVector>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <WinCon.h>
#endif

// Forward declarations for standalone usage
#ifndef XOPTIONS_H
namespace XOptions {
struct COLOR_RECORD {
    QString sColorMain;
    QString sColorBackground;
};
}  // namespace XOptions
#else
#include "xoptions.h"
#endif

class XColorString {
public:
    struct RGB_COLOR {
        qint32 nRed;
        qint32 nGreen;
        qint32 nBlue;
        bool bValid;
    };
    struct PART {
        QString sText;
        XOptions::COLOR_RECORD colorRecord;
    };

    struct RULE {
        quint32 nGroupID;
        QString sString;
        XOptions::COLOR_RECORD colorRecord;
        bool bIsCaseSensitive;
    };

    struct CONSOLE_STATE {
        quint32 nOriginalMode;
        quint32 nCurrentMode;
        bool bIsValid;
        bool bIsEscapeMode;
        bool bIsWinNativeMode;
    };

    XColorString();
    virtual ~XColorString();

    static CONSOLE_STATE initConsole();
    static void finishConsole(const CONSOLE_STATE &consoleState);

    void addPart(const QString &sText, const QString &sColorMain = "", const QString &sColorBackground = "");
    void addSpace();
    void addString(quint32 nGroupID, const QString &sString);
    void addRule(quint32 nGroupID, const QString &sString, const XOptions::COLOR_RECORD &colorRecord, bool bIsCaseSensitive);
    void addRule(quint32 nGroupID, const QString &sString, const QString &sColorMain, const QString &sColorBackground, bool bIsCaseSensitive);
    QString toPlainText();
    void printConsole(CONSOLE_STATE *pConsoleState);

    static qint32 colorToAnsiCode(const RGB_COLOR &color, bool bBackground = false);
    static QString colorNameToHex(const QString &sColorName);
    static RGB_COLOR hexToColor(const QString &sHex);
    static RGB_COLOR parseColor(const QString &sColor);

private:
    QVector<PART> m_vecParts;
    QList<RULE> m_lstRules;
};

#endif  // XCOLORSTRING_H
