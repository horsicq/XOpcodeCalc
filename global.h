// Copyright (c) 2019 hors<horsicq@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#ifndef GLOBAL_H
#define GLOBAL_H

// Check Windows
#if defined(_WIN32) || defined(_WIN64)
#if defined(_WIN64)
#define OPCODE64
#else
#define OPCODE32
#endif
#endif

// Check GCC
#if defined(__GNUC__)
#if defined(__x86_64__) || defined(__ppc64__)
#define OPCODE64
#else
#define OPCODE32
#endif
#endif

#include <QtGlobal>

#define X_APPLICATIONNAME "XOpcodeCalc"
#define X_APPLICATIONVERSION "0.02"
#define X_ORGANIZATIONNAME "NTInfo"
#define X_ORGANIZATIONDOMAIN "ntinfo.biz"

namespace XOPCODECALC
{
    struct OPTIONS
    {
        bool bStayOnTop;
    };
}

#endif // GLOBAL_H
