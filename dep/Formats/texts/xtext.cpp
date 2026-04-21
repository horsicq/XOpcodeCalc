/* Copyright (c) 2022-2026 hors<horsicq@gmail.com>
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
#include "xtext.h"

XText::XText(QIODevice *pDevice) : XBinary(pDevice)
{
}

XText::~XText()
{
}

bool XText::isValid(PDSTRUCT *pPdStruct)
{
    bool bIsValid = false;

    if (getSize() > 0) {
        // Check if file contains primarily text content
        QByteArray baData = read_array_process(0, qMin(getSize(), qint64(8192)), pPdStruct);

        // Check for BOM markers
        TEXT_TYPE bomType = _detectBOM();
        if (bomType != TEXT_TYPE_UNKNOWN) {
            bIsValid = true;
        }
        // Check if content is valid UTF-8
        else if (_isValidUTF8(baData)) {
            bIsValid = true;
        }
        // Check if content is printable ASCII
        else if (_isPrintableASCII(baData)) {
            bIsValid = true;
        }
        // Check for UTF-16 (with basic heuristics)
        else if (baData.size() >= 2 && (baData.size() % 2 == 0)) {
            if (_isValidUTF16(baData, false) || _isValidUTF16(baData, true)) {
                bIsValid = true;
            }
        }
    }

    return bIsValid;
}

bool XText::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XText xtext(pDevice);
    return xtext.isValid(pPdStruct);
}

XBinary::_MEMORY_MAP XText::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    Q_UNUSED(pPdStruct)

    XBinary::_MEMORY_MAP result = {};
    result.nBinarySize = getSize();

    qint32 nIndex = 0;

    // Add BOM if present
    qint64 nBOMSize = getBOMSize();
    if (nBOMSize > 0) {
        _MEMORY_RECORD recordBOM = {};
        recordBOM.nIndex = nIndex++;
        recordBOM.filePart = FILEPART_HEADER;
        recordBOM.sName = tr("BOM");
        recordBOM.nOffset = 0;
        recordBOM.nSize = nBOMSize;
        recordBOM.nAddress = -1;
        result.listRecords.append(recordBOM);
    }

    // Add text content
    _MEMORY_RECORD recordText = {};
    recordText.nIndex = nIndex++;
    recordText.filePart = FILEPART_DATA;
    recordText.sName = tr("Text content");
    recordText.nOffset = nBOMSize;
    recordText.nSize = getSize() - nBOMSize;
    recordText.nAddress = -1;
    result.listRecords.append(recordText);

    return result;
}

XBinary::FT XText::getFileType()
{
    TEXT_TYPE textType = detectTextType();

    switch (textType) {
        case TEXT_TYPE_UTF8:
        case TEXT_TYPE_UTF8_BOM: return FT_UTF8;
        case TEXT_TYPE_UTF16_LE:
        case TEXT_TYPE_UTF16_BE: return FT_UNICODE;
        case TEXT_TYPE_PLAINTEXT:
        case TEXT_TYPE_ASCII:
        case TEXT_TYPE_ANSI:
        default: return FT_TEXT;
    }
}

QString XText::getFileFormatExt()
{
    return "txt";
}

QString XText::getFileFormatExtsString()
{
    return "Text files (*.txt *.text *.log *.md *.readme)";
}

qint64 XText::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}

QString XText::getVersion()
{
    TEXT_INFO textInfo = getTextInfo();
    return textTypeToString(textInfo.textType);
}

XText::TEXT_INFO XText::getTextInfo(PDSTRUCT *pPdStruct)
{
    TEXT_INFO result = {};

    result.textType = detectTextType(pPdStruct);
    result.lineEnding = detectLineEnding(pPdStruct);
    result.sEncoding = getEncodingName();
    result.bHasBOM = hasBOM();
    result.nBOMSize = getBOMSize();
    result.nLineCount = getLineCount();
    result.nCharacterCount = getCharacterCount();

    // Calculate word count from a sample of text
    QString sampleText = getText(result.nBOMSize, qMin(qint64(32768), getSize() - result.nBOMSize));
    result.nWordCount = _countWords(sampleText);

    return result;
}

XText::TEXT_TYPE XText::detectTextType(PDSTRUCT *pPdStruct)
{
    // First check for BOM
    TEXT_TYPE bomType = _detectBOM();
    if (bomType != TEXT_TYPE_UNKNOWN) {
        return bomType;
    }

    // Read sample data for analysis
    QByteArray baData = read_array_process(0, qMin(getSize(), qint64(8192)), pPdStruct);

    // Check if it's printable ASCII
    if (_isPrintableASCII(baData)) {
        return TEXT_TYPE_ASCII;
    }

    // Check UTF-8
    if (_isValidUTF8(baData)) {
        return TEXT_TYPE_UTF8;
    }

    // Check UTF-16
    if (baData.size() >= 2 && (baData.size() % 2 == 0)) {
        if (_isValidUTF16(baData, false)) {
            return TEXT_TYPE_UTF16_LE;
        }
        if (_isValidUTF16(baData, true)) {
            return TEXT_TYPE_UTF16_BE;
        }
    }

    // Default to plain text
    return TEXT_TYPE_PLAINTEXT;
}

XText::LINE_ENDING XText::detectLineEnding(PDSTRUCT *pPdStruct)
{
    QByteArray baData = read_array_process(0, qMin(getSize(), qint64(8192)), pPdStruct);
    return _detectLineEndingInData(baData);
}

QString XText::getText(qint64 nOffset, qint64 nSize)
{
    if (nSize == -1) {
        nSize = getSize() - nOffset;
    }

    QByteArray baData = read_array(nOffset, nSize);
    TEXT_TYPE textType = detectTextType();

    switch (textType) {
        case TEXT_TYPE_UTF8:
        case TEXT_TYPE_UTF8_BOM: return QString::fromUtf8(baData);
        case TEXT_TYPE_UTF16_LE: return QString::fromUtf16(reinterpret_cast<const char16_t *>(baData.constData()), baData.size() / 2);
        case TEXT_TYPE_UTF16_BE:
            // Convert big-endian to little-endian
            {
                QByteArray converted;
                for (int i = 0; i < baData.size() - 1; i += 2) {
                    converted.append(baData[i + 1]);
                    converted.append(baData[i]);
                }
                return QString::fromUtf16(reinterpret_cast<const char16_t *>(converted.constData()), converted.size() / 2);
            }
        default: return QString::fromLatin1(baData);
    }
}

QString XText::getTextByLines(qint64 nStartLine, qint64 nLineCount)
{
    QStringList lines = getLines(nStartLine, nLineCount);
    return lines.join("\n");
}

QStringList XText::getLines(qint64 nStartLine, qint64 nLineCount)
{
    QString allText = getText();
    // QStringList allLines = allText.split(QRegExp("\\r\\n|\\r|\\n"));

    // if (nLineCount == -1) {
    //     nLineCount = allLines.size() - nStartLine;
    // }

    QStringList result;
    // for (qint64 i = nStartLine; i < qMin(nStartLine + nLineCount, qint64(allLines.size())); i++) {
    //     if (i >= 0 && i < allLines.size()) {
    //         result.append(allLines[i]);
    //     }
    // }

    return result;
}

qint64 XText::getLineCount()
{
    QByteArray baData = read_array(0, getSize());
    return _countLines(baData);
}

qint64 XText::getCharacterCount()
{
    TEXT_TYPE textType = detectTextType();
    qint64 nBOMSize = getBOMSize();

    switch (textType) {
        case TEXT_TYPE_UTF16_LE:
        case TEXT_TYPE_UTF16_BE: return (getSize() - nBOMSize) / 2;
        case TEXT_TYPE_UTF32_LE:
        case TEXT_TYPE_UTF32_BE: return (getSize() - nBOMSize) / 4;
        default:
            // For UTF-8 and single-byte encodings, approximate character count
            QString text = getText();
            return text.length();
    }
}

qint64 XText::getWordCount()
{
    QString text = getText();
    return _countWords(text);
}

bool XText::hasBOM()
{
    return getBOMSize() > 0;
}

qint64 XText::getBOMSize()
{
    TEXT_TYPE bomType = _detectBOM();

    switch (bomType) {
        case TEXT_TYPE_UTF8_BOM: return 3;
        case TEXT_TYPE_UTF16_LE:
        case TEXT_TYPE_UTF16_BE: return 2;
        case TEXT_TYPE_UTF32_LE:
        case TEXT_TYPE_UTF32_BE: return 4;
        default: return 0;
    }
}

QString XText::getEncodingName()
{
    TEXT_TYPE textType = detectTextType();
    return textTypeToString(textType);
}

QString XText::textTypeToString(TEXT_TYPE textType)
{
    switch (textType) {
        case TEXT_TYPE_PLAINTEXT: return "Plain Text";
        case TEXT_TYPE_UTF8: return "UTF-8";
        case TEXT_TYPE_UTF8_BOM: return "UTF-8 with BOM";
        case TEXT_TYPE_UTF16_LE: return "UTF-16 LE";
        case TEXT_TYPE_UTF16_BE: return "UTF-16 BE";
        case TEXT_TYPE_UTF32_LE: return "UTF-32 LE";
        case TEXT_TYPE_UTF32_BE: return "UTF-32 BE";
        case TEXT_TYPE_ASCII: return "ASCII";
        case TEXT_TYPE_ANSI: return "ANSI";
        default: return "Unknown";
    }
}

QString XText::lineEndingToString(LINE_ENDING lineEnding)
{
    switch (lineEnding) {
        case LINE_ENDING_LF: return "LF (Unix)";
        case LINE_ENDING_CRLF: return "CRLF (Windows)";
        case LINE_ENDING_CR: return "CR (Mac)";
        case LINE_ENDING_MIXED: return "Mixed";
        default: return "Unknown";
    }
}

QString XText::getMIMEString()
{
    TEXT_TYPE textType = detectTextType();

    switch (textType) {
        case TEXT_TYPE_UTF8:
        case TEXT_TYPE_UTF8_BOM: return "text/plain; charset=utf-8";
        case TEXT_TYPE_UTF16_LE:
        case TEXT_TYPE_UTF16_BE: return "text/plain; charset=utf-16";
        default: return "text/plain";
    }
}

XText::TEXT_TYPE XText::_detectBOM()
{
    if (getSize() >= 3) {
        QByteArray baBOM = read_array(0, 4);

        // UTF-8 BOM: EF BB BF
        if (baBOM.size() >= 3 && (quint8)baBOM[0] == 0xEF && (quint8)baBOM[1] == 0xBB && (quint8)baBOM[2] == 0xBF) {
            return TEXT_TYPE_UTF8_BOM;
        }

        // UTF-16 LE BOM: FF FE
        if (baBOM.size() >= 2 && (quint8)baBOM[0] == 0xFF && (quint8)baBOM[1] == 0xFE) {
            // Check if it's UTF-32 LE: FF FE 00 00
            if (baBOM.size() >= 4 && (quint8)baBOM[2] == 0x00 && (quint8)baBOM[3] == 0x00) {
                return TEXT_TYPE_UTF32_LE;
            }
            return TEXT_TYPE_UTF16_LE;
        }

        // UTF-16 BE BOM: FE FF
        if (baBOM.size() >= 2 && (quint8)baBOM[0] == 0xFE && (quint8)baBOM[1] == 0xFF) {
            return TEXT_TYPE_UTF16_BE;
        }

        // UTF-32 BE BOM: 00 00 FE FF
        if (baBOM.size() >= 4 && (quint8)baBOM[0] == 0x00 && (quint8)baBOM[1] == 0x00 && (quint8)baBOM[2] == 0xFE && (quint8)baBOM[3] == 0xFF) {
            return TEXT_TYPE_UTF32_BE;
        }
    }

    return TEXT_TYPE_UNKNOWN;
}

bool XText::_isValidUTF8(const QByteArray &data, qint64 nMaxCheck)
{
    qint64 nCheck = qMin(nMaxCheck, qint64(data.size()));

    for (qint64 i = 0; i < nCheck;) {
        quint8 byte = data.at(i);

        if (byte < 0x80) {
            // ASCII character
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte sequence
            if (i + 1 >= nCheck) break;
            if ((data.at(i + 1) & 0xC0) != 0x80) return false;
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte sequence
            if (i + 2 >= nCheck) break;
            if ((data.at(i + 1) & 0xC0) != 0x80 || (data.at(i + 2) & 0xC0) != 0x80) return false;
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte sequence
            if (i + 3 >= nCheck) break;
            if ((data.at(i + 1) & 0xC0) != 0x80 || (data.at(i + 2) & 0xC0) != 0x80 || (data.at(i + 3) & 0xC0) != 0x80) return false;
            i += 4;
        } else {
            return false;
        }
    }

    return true;
}

bool XText::_isValidUTF16(const QByteArray &data, bool bBigEndian, qint64 nMaxCheck)
{
    qint64 nCheck = qMin(nMaxCheck, qint64(data.size()));

    if (nCheck % 2 != 0) return false;

    for (qint64 i = 0; i < nCheck - 1; i += 2) {
        quint16 codeUnit;
        if (bBigEndian) {
            codeUnit = ((quint8)data.at(i) << 8) | (quint8)data.at(i + 1);
        } else {
            codeUnit = ((quint8)data.at(i + 1) << 8) | (quint8)data.at(i);
        }

        // Check for high surrogate
        if (codeUnit >= 0xD800 && codeUnit <= 0xDBFF) {
            // Need low surrogate
            if (i + 3 >= nCheck) return false;

            quint16 lowSurrogate;
            if (bBigEndian) {
                lowSurrogate = ((quint8)data.at(i + 2) << 8) | (quint8)data.at(i + 3);
            } else {
                lowSurrogate = ((quint8)data.at(i + 3) << 8) | (quint8)data.at(i + 2);
            }

            if (lowSurrogate < 0xDC00 || lowSurrogate > 0xDFFF) return false;
            i += 2;  // Skip the low surrogate in next iteration
        } else if (codeUnit >= 0xDC00 && codeUnit <= 0xDFFF) {
            // Unexpected low surrogate
            return false;
        }
    }

    return true;
}

bool XText::_isPrintableASCII(const QByteArray &data, qint64 nMaxCheck)
{
    qint64 nCheck = qMin(nMaxCheck, qint64(data.size()));
    qint32 nPrintableCount = 0;

    for (qint64 i = 0; i < nCheck; i++) {
        quint8 byte = data.at(i);

        // Check for printable ASCII or common whitespace
        if ((byte >= 32 && byte <= 126) || byte == 9 || byte == 10 || byte == 13) {
            nPrintableCount++;
        } else if (byte == 0) {
            // Null bytes are not expected in text files
            return false;
        }
    }

    // Consider it text if at least 90% of characters are printable
    return (nCheck > 0) && (nPrintableCount * 100 / nCheck >= 90);
}

XText::LINE_ENDING XText::_detectLineEndingInData(const QByteArray &data)
{
    bool hasCRLF = data.contains("\r\n");
    bool hasLF = data.contains("\n");
    bool hasCR = data.contains("\r");

    if (hasCRLF && !hasCR && !hasLF) {
        return LINE_ENDING_CRLF;
    } else if (hasLF && !hasCR) {
        return LINE_ENDING_LF;
    } else if (hasCR && !hasLF) {
        return LINE_ENDING_CR;
    } else if ((hasCR || hasLF) && (hasCRLF || (hasCR && hasLF))) {
        return LINE_ENDING_MIXED;
    }

    return LINE_ENDING_UNKNOWN;
}

qint64 XText::_countLines(const QByteArray &data)
{
    qint64 nLines = 0;
    LINE_ENDING lineEnding = _detectLineEndingInData(data);

    switch (lineEnding) {
        case LINE_ENDING_CRLF: nLines = data.count("\r\n"); break;
        case LINE_ENDING_LF: nLines = data.count("\n"); break;
        case LINE_ENDING_CR: nLines = data.count("\r"); break;
        case LINE_ENDING_MIXED:
            // Count all types of line endings
            nLines = data.count("\r\n") + data.count("\n") + data.count("\r");
            nLines -= data.count("\r\n");  // Don't double-count CRLF
            break;
        default: nLines = 0; break;
    }

    // Add 1 if file doesn't end with a line ending
    if (data.size() > 0 && !data.endsWith("\n") && !data.endsWith("\r")) {
        nLines++;
    }

    return nLines;
}

qint64 XText::_countWords(const QString &text)
{
    // return text.split(QRegExp("\\s+"), Qt::SkipEmptyParts).count();
    return 0;
}

