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
#ifndef XAMIGAHUNK_DEF_H
#define XAMIGAHUNK_DEF_H

#include <QtGlobal>

namespace XAMIGAHUNK_DEF {
const quint64 IMAGE_BASE = 0x00000000;

const quint16 HUNK_UNIT = 0x03E7;          // 999
const quint16 HUNK_NAME = 0x03E8;          // 1000
const quint16 HUNK_CODE = 0x03E9;          // 1001
const quint16 HUNK_DATA = 0x03EA;          // 1002
const quint16 HUNK_BSS = 0x03EB;           // 1003
const quint16 HUNK_RELOC32 = 0x03EC;       // 1004
const quint16 HUNK_RELOC16 = 0x03ED;       // 1005
const quint16 HUNK_RELOC8 = 0x03EE;        // 1006
const quint16 HUNK_EXT = 0x03EF;           // 1007
const quint16 HUNK_SYMBOL = 0x03F0;        // 1008
const quint16 HUNK_DEBUG = 0x03F1;         // 1009
const quint16 HUNK_END = 0x03F2;           // 1010
const quint16 HUNK_HEADER = 0x03F3;        // 1011
const quint16 HUNK_OVERLAY = 0x03F5;       // 1013
const quint16 HUNK_BREAK = 0x03F6;         // 1014
const quint16 HUNK_DREL32 = 0x03F7;        // 1015
const quint16 HUNK_DREL16 = 0x03F8;        // 1016
const quint16 HUNK_DREL8 = 0x03F9;         // 1017
const quint16 HUNK_LIB = 0x03FA;           // 1018
const quint16 HUNK_INDEX = 0x03FB;         // 1019
const quint16 HUNK_RELOC32SHORT = 0x03FC;  // 1020
const quint16 HUNK_RELRELOC32 = 0x03FD;    // 1021
const quint16 HUNK_ABSRELOC16 = 0x03FE;    // 1022
const quint16 HUNK_DREL32EXE = 0x03FF;     // 1023
const quint16 HUNK_PPC_CODE = 0x04E9;      // 1257
const quint16 HUNK_RELRELOC26 = 0x04EC;    // 1260

}  // namespace XAMIGAHUNK_DEF

#endif  // XAMIGAHUNK_DEF_H
