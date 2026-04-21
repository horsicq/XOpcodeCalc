/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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
#ifndef XDER_H
#define XDER_H

#include "xbinary.h"

// Basic Distinguished Encoding Rules (DER) parser for top-level TLV header
class XDER : public XBinary {
    Q_OBJECT

public:
    struct HEADER {
        quint8 nTag;            // Top-level tag (usually 0x30 for SEQUENCE)
        qint64 nHeaderSize;     // Size of Tag+Length
        qint64 nContentOffset;  // Offset of content (value)
        qint64 nContentSize;    // Content size from length field
        bool bIsValid;          // Whether header is valid and fits into file
    };

    explicit XDER(QIODevice *pDevice = nullptr);
    ~XDER();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);

    virtual FT getFileType();
    virtual QString getVersion();
    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);
    virtual QString getMIMEString();

    HEADER getHeader(PDSTRUCT *pPdStruct = nullptr);

private:
    HEADER _getHeader(PDSTRUCT *pPdStruct);
};

#endif  // XDER_H

