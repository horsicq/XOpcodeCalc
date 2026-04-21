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
#ifndef XTEXT_H
#define XTEXT_H

#include "xbinary.h"

class XText : public XBinary {
    Q_OBJECT

public:
    enum TEXT_TYPE {
        TEXT_TYPE_UNKNOWN = 0,
        TEXT_TYPE_PLAINTEXT,
        TEXT_TYPE_UTF8,
        TEXT_TYPE_UTF8_BOM,
        TEXT_TYPE_UTF16_LE,
        TEXT_TYPE_UTF16_BE,
        TEXT_TYPE_UTF32_LE,
        TEXT_TYPE_UTF32_BE,
        TEXT_TYPE_ASCII,
        TEXT_TYPE_ANSI
    };

    enum LINE_ENDING {
        LINE_ENDING_UNKNOWN = 0,
        LINE_ENDING_LF,    // Unix/Linux (\n)
        LINE_ENDING_CRLF,  // Windows (\r\n)
        LINE_ENDING_CR,    // Mac Classic (\r)
        LINE_ENDING_MIXED  // Mixed line endings
    };

    struct TEXT_INFO {
        TEXT_TYPE textType;
        LINE_ENDING lineEnding;
        QString sEncoding;
        qint64 nLineCount;
        qint64 nCharacterCount;
        qint64 nWordCount;
        bool bHasBOM;
        qint64 nBOMSize;
    };

    explicit XText(QIODevice *pDevice = nullptr);
    ~XText();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual FT getFileType() override;
    virtual QString getFileFormatExt() override;
    virtual QString getFileFormatExtsString() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct) override;
    virtual QString getVersion() override;

    TEXT_INFO getTextInfo(PDSTRUCT *pPdStruct = nullptr);
    TEXT_TYPE detectTextType(PDSTRUCT *pPdStruct = nullptr);
    LINE_ENDING detectLineEnding(PDSTRUCT *pPdStruct = nullptr);

    QString getText(qint64 nOffset = 0, qint64 nSize = -1);
    QString getTextByLines(qint64 nStartLine, qint64 nLineCount = 1);
    QStringList getLines(qint64 nStartLine = 0, qint64 nLineCount = -1);

    qint64 getLineCount();
    qint64 getCharacterCount();
    qint64 getWordCount();

    bool hasBOM();
    qint64 getBOMSize();
    QString getEncodingName();

    static QString textTypeToString(TEXT_TYPE textType);
    static QString lineEndingToString(LINE_ENDING lineEnding);

    virtual QString getMIMEString();

private:
    TEXT_TYPE _detectBOM();
    bool _isValidUTF8(const QByteArray &data, qint64 nMaxCheck = 8192);
    bool _isValidUTF16(const QByteArray &data, bool bBigEndian, qint64 nMaxCheck = 8192);
    bool _isPrintableASCII(const QByteArray &data, qint64 nMaxCheck = 8192);
    LINE_ENDING _detectLineEndingInData(const QByteArray &data);
    qint64 _countLines(const QByteArray &data);
    qint64 _countWords(const QString &text);
};

#endif  // XTEXT_H

