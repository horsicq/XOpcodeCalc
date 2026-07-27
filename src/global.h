/* Copyright (c) 2019-2026 hors<horsicq@gmail.com>
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
#ifndef GLOBAL_H
#define GLOBAL_H

// The opcode mode follows the build's pointer width so the calculator matches
// the register width of the target process on every architecture.
#if defined(_WIN64) || defined(__LP64__) || (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8)
#define OPCODE64
#else
#define OPCODE32
#endif

#include <QtGlobal>

#define X_APPLICATIONDISPLAYNAME "XOpcodeCalc"
#define X_APPLICATIONNAME "xocalc"
#define X_APPLICATIONVERSION "1.0.0"
#define X_ORGANIZATIONNAME "NTInfo"
#define X_ORGANIZATIONDOMAIN "ntinfo.biz"
#define X_OPTIONSFILE "xocalc.ini"

#endif  // GLOBAL_H
