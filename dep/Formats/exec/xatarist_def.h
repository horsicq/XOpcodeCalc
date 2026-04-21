/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
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
#ifndef XATARIST_DEF_H
#define XATARIST_DEF_H

#include <QtGlobal>

namespace XATARIST_DEF {
const quint16 MAGIC = 0x601A;

struct HEADER {
    quint16 nMagic;       // 0x601A
    quint32 nTextSize;    // Size of text segment
    quint32 nDataSize;    // Size of data segment
    quint32 nBssSize;     // Size of BSS segment
    quint32 nSymbolSize;  // Size of symbol table
    quint32 nReserved;    // Reserved (0)
    quint32 nFlags;       // Flags
    quint16 nRelocation;  // Relocation flag
};
}  // namespace XATARIST_DEF

#endif  // XATARIST_DEF_H
