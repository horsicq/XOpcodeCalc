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
#include "xcolorstring.h"

XColorString::XColorString()
{
}

XColorString::~XColorString()
{
}

XColorString::CONSOLE_STATE XColorString::initConsole()
{
    CONSOLE_STATE state = {};
    state.bIsValid = false;
    state.bIsEscapeMode = false;
    state.bIsWinNativeMode = false;

#ifdef Q_OS_WIN
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    if (hConsole && hConsole != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;

        if (GetConsoleMode(hConsole, &dwMode)) {
            state.nOriginalMode = (quint32)dwMode;
            state.bIsValid = true;

#ifndef _USING_V110_SDK71_
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
            DWORD dwNewMode = dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;

            if (SetConsoleMode(hConsole, dwNewMode)) {
                state.nCurrentMode = (quint32)dwNewMode;
                state.bIsEscapeMode = true;
            } else {
                state.nCurrentMode = state.nOriginalMode;
                state.bIsWinNativeMode = true;
            }
#else
            state.nCurrentMode = state.nOriginalMode;
            state.bIsWinNativeMode = true;
#endif
        }
    }
#else
    state.bIsEscapeMode = true;
#endif

    return state;
}

void XColorString::finishConsole(const CONSOLE_STATE &consoleState)
{
#ifdef Q_OS_WIN
    if (consoleState.bIsValid) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (hConsole && hConsole != INVALID_HANDLE_VALUE) {
            SetConsoleMode(hConsole, (DWORD)consoleState.nOriginalMode);
        }
    }
#else
    Q_UNUSED(consoleState)
#endif
}

void XColorString::addPart(const QString &sText, const QString &sColorMain, const QString &sColorBackground)
{
    PART part = {};
    part.sText = sText;
    part.colorRecord.sColorMain = sColorMain;
    part.colorRecord.sColorBackground = sColorBackground;

    m_vecParts.append(part);
}

void XColorString::addSpace()
{
    addPart(" ");
}

void XColorString::addString(quint32 nGroupID, const QString &sString)
{
    XOptions::COLOR_RECORD colorRecord = {};
    bool bFound = false;

    qint32 nNumberOfRules = m_lstRules.count();

    for (qint32 i = 0; i < nNumberOfRules; i++) {
        if (m_lstRules.at(i).nGroupID == nGroupID) {
            bool bMatch = false;

            if (m_lstRules.at(i).sString.isEmpty()) {
                bMatch = true;
            } else if (m_lstRules.at(i).bIsCaseSensitive) {
                bMatch = (m_lstRules.at(i).sString == sString);
            } else {
                bMatch = (m_lstRules.at(i).sString.toLower() == sString.toLower());
            }

            if (bMatch) {
                colorRecord = m_lstRules.at(i).colorRecord;
                bFound = true;
                break;
            }
        }
    }

    if (bFound) {
        addPart(sString, colorRecord.sColorMain, colorRecord.sColorBackground);
    } else {
        addPart(sString);
    }
}

void XColorString::addRule(quint32 nGroupID, const QString &sString, const XOptions::COLOR_RECORD &colorRecord, bool bIsCaseSensitive)
{
    RULE rule = {};
    rule.nGroupID = nGroupID;
    rule.sString = sString;
    rule.colorRecord = colorRecord;
    rule.bIsCaseSensitive = bIsCaseSensitive;

    m_lstRules.append(rule);
}

void XColorString::addRule(quint32 nGroupID, const QString &sString, const QString &sColorMain, const QString &sColorBackground, bool bIsCaseSensitive)
{
    XOptions::COLOR_RECORD colorRecord = {};
    colorRecord.sColorMain = sColorMain;
    colorRecord.sColorBackground = sColorBackground;

    addRule(nGroupID, sString, colorRecord, bIsCaseSensitive);
}

QString XColorString::toPlainText()
{
    QString sResult;

    qint32 nNumberOfParts = m_vecParts.count();

    for (qint32 i = 0; i < nNumberOfParts; i++) {
        sResult += m_vecParts.at(i).sText;
    }

    return sResult;
}

void XColorString::printConsole(CONSOLE_STATE *pConsoleState)
{
    if (!pConsoleState) {
        return;
    }

    qint32 nNumberOfParts = m_vecParts.count();

    for (qint32 i = 0; i < nNumberOfParts; i++) {
        QString sColorMain = m_vecParts.at(i).colorRecord.sColorMain;
        QString sColorBackground = m_vecParts.at(i).colorRecord.sColorBackground;

        if (pConsoleState->bIsEscapeMode) {
            if (!sColorMain.isEmpty() || !sColorBackground.isEmpty()) {
                qint32 nFg = 39;
                qint32 nBg = 49;

                RGB_COLOR colorMain = parseColor(sColorMain);
                if (colorMain.bValid) {
                    nFg = colorToAnsiCode(colorMain, false);
                }

                RGB_COLOR colorBg = parseColor(sColorBackground);
                if (colorBg.bValid) {
                    nBg = colorToAnsiCode(colorBg, true);
                }

                printf("\033[%d;%dm", nFg, nBg);
            }
        } else if (pConsoleState->bIsWinNativeMode) {
#ifdef Q_OS_WIN
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

            if (hConsole && hConsole != INVALID_HANDLE_VALUE) {
                WORD wOldAttribute = 0;
                CONSOLE_SCREEN_BUFFER_INFO csbi = {};

                if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
                    wOldAttribute = csbi.wAttributes;
                }

                WORD wAttribute = 0;
                RGB_COLOR colorMain = parseColor(sColorMain);

                if (colorMain.bValid) {
                    if (colorMain.nRed > 127) wAttribute |= FOREGROUND_RED;
                    if (colorMain.nGreen > 127) wAttribute |= FOREGROUND_GREEN;
                    if (colorMain.nBlue > 127) wAttribute |= FOREGROUND_BLUE;
                    if ((colorMain.nRed + colorMain.nGreen + colorMain.nBlue) / 3 > 192) wAttribute |= FOREGROUND_INTENSITY;
                }

                RGB_COLOR colorBg = parseColor(sColorBackground);

                if (colorBg.bValid) {
                    if (colorBg.nRed > 127) wAttribute |= BACKGROUND_RED;
                    if (colorBg.nGreen > 127) wAttribute |= BACKGROUND_GREEN;
                    if (colorBg.nBlue > 127) wAttribute |= BACKGROUND_BLUE;
                    if ((colorBg.nRed + colorBg.nGreen + colorBg.nBlue) / 3 > 192) wAttribute |= BACKGROUND_INTENSITY;
                }

                bool bChanged = false;

                if (wAttribute || colorMain.bValid || colorBg.bValid) {
                    SetConsoleTextAttribute(hConsole, wAttribute ? wAttribute : wOldAttribute);
                    bChanged = true;
                }

                printf("%s", m_vecParts.at(i).sText.toUtf8().data());

                if (bChanged) {
                    SetConsoleTextAttribute(hConsole, wOldAttribute);
                }
            } else {
                // Fallback if console handle is invalid
                printf("%s", m_vecParts.at(i).sText.toUtf8().data());
            }
#else
            printf("%s", m_vecParts.at(i).sText.toUtf8().data());
#endif
        } else {
            printf("%s", m_vecParts.at(i).sText.toUtf8().data());
        }
    }
}

XColorString::RGB_COLOR XColorString::parseColor(const QString &sColor)
{
    RGB_COLOR result = {};
    result.bValid = false;

    if (sColor.isEmpty()) {
        return result;
    }

    QString sColorLower = sColor.toLower().trimmed();

    if (sColorLower.startsWith("#")) {
        return hexToColor(sColorLower);
    }

    static QMap<QString, RGB_COLOR> mapColors;
    if (mapColors.isEmpty()) {
        mapColors["black"] = {0, 0, 0, true};
        mapColors["red"] = {255, 0, 0, true};
        mapColors["green"] = {0, 255, 0, true};
        mapColors["yellow"] = {255, 255, 0, true};
        mapColors["blue"] = {0, 0, 255, true};
        mapColors["magenta"] = {255, 0, 255, true};
        mapColors["cyan"] = {0, 255, 255, true};
        mapColors["white"] = {255, 255, 255, true};
        mapColors["gray"] = {128, 128, 128, true};
        mapColors["darkred"] = {139, 0, 0, true};
        mapColors["darkgreen"] = {0, 100, 0, true};
        mapColors["darkblue"] = {0, 0, 139, true};
        mapColors["orange"] = {255, 165, 0, true};
    }

    if (mapColors.contains(sColorLower)) {
        return mapColors[sColorLower];
    }

    return result;
}

qint32 XColorString::colorToAnsiCode(const RGB_COLOR &color, bool bBackground)
{
    if (!color.bValid) {
        return bBackground ? 49 : 39;
    }

    qint32 nBase = bBackground ? 40 : 30;
    qint32 nBrightBase = bBackground ? 100 : 90;

    qint32 nRed = color.nRed;
    qint32 nGreen = color.nGreen;
    qint32 nBlue = color.nBlue;

    bool bIsBright = (nRed + nGreen + nBlue) > 384;

    if (nRed > 200 && nGreen < 100 && nBlue < 100) {
        return bIsBright ? (nBrightBase + 1) : (nBase + 1);
    } else if (nGreen > 200 && nRed < 100 && nBlue < 100) {
        return bIsBright ? (nBrightBase + 2) : (nBase + 2);
    } else if (nBlue > 200 && nRed < 100 && nGreen < 100) {
        return bIsBright ? (nBrightBase + 4) : (nBase + 4);
    } else if (nRed > 200 && nGreen > 200 && nBlue < 100) {
        return bIsBright ? (nBrightBase + 3) : (nBase + 3);
    } else if (nRed > 200 && nBlue > 200 && nGreen < 100) {
        return bIsBright ? (nBrightBase + 5) : (nBase + 5);
    } else if (nGreen > 200 && nBlue > 200 && nRed < 100) {
        return bIsBright ? (nBrightBase + 6) : (nBase + 6);
    } else if (nRed < 50 && nGreen < 50 && nBlue < 50) {
        return nBase;
    } else if (nRed > 200 && nGreen > 200 && nBlue > 200) {
        return bIsBright ? (nBrightBase + 7) : (nBase + 7);
    }

    return bIsBright ? nBrightBase : nBase;
}

QString XColorString::colorNameToHex(const QString &sColorName)
{
    RGB_COLOR color = parseColor(sColorName);

    if (color.bValid) {
        return QString("#%1%2%3").arg(color.nRed, 2, 16, QChar('0')).arg(color.nGreen, 2, 16, QChar('0')).arg(color.nBlue, 2, 16, QChar('0'));
    }

    return QString();
}

XColorString::RGB_COLOR XColorString::hexToColor(const QString &sHex)
{
    RGB_COLOR result = {};
    result.bValid = false;

    if (sHex.isEmpty()) {
        return result;
    }

    QString sColor = sHex.trimmed();
    if (sColor.startsWith("#")) {
        sColor = sColor.mid(1);
    }

    if (sColor.length() == 6) {
        bool bOk1 = false;
        bool bOk2 = false;
        bool bOk3 = false;

        result.nRed = sColor.mid(0, 2).toInt(&bOk1, 16);
        result.nGreen = sColor.mid(2, 2).toInt(&bOk2, 16);
        result.nBlue = sColor.mid(4, 2).toInt(&bOk3, 16);
        result.bValid = bOk1 && bOk2 && bOk3;
    } else if (sColor.length() == 3) {
        bool bOk1 = false;
        bool bOk2 = false;
        bool bOk3 = false;

        result.nRed = sColor.mid(0, 1).toInt(&bOk1, 16) * 17;
        result.nGreen = sColor.mid(1, 1).toInt(&bOk2, 16) * 17;
        result.nBlue = sColor.mid(2, 1).toInt(&bOk3, 16) * 17;
        result.bValid = bOk1 && bOk2 && bOk3;
    }

    return result;
}
