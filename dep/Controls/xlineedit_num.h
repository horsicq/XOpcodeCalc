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
#ifndef XLINEEDIT_NUM_H
#define XLINEEDIT_NUM_H

#include <QLineEdit>
#include <QObject>

class XLineEdit_num : public QLineEdit {
public:
    enum MODE {
        MODE_UNKNOWN = 0,
        MODE_HEX,
        MODE_DEC,
        MODE_SIGN_DEC
    };

    enum BITS {
        BITS_UNKNOWN = 0,
        BITS_8 = 8,
        BITS_16 = 16,
        BITS_32 = 32,
        BITS_64 = 64
    };

    XLineEdit_num();

private:
    QVariant m_vValue;
};

#endif  // XLINEEDIT_NUM_H
