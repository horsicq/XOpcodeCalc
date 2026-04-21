/* Copyright (c) 2017-2026 hors<horsicq@gmail.com>
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
#ifndef XLINEEDITVALIDATOR_H
#define XLINEEDITVALIDATOR_H

#include <QIntValidator>
#include <QDoubleValidator>

class XLineEditValidator : public QValidator {
    Q_OBJECT

public:
    enum MODE {
        MODE_TEXT = 0,
        MODE_DEC_8,
        MODE_DEC_16,
        MODE_DEC_32,
        MODE_DEC_64,
        MODE_SIGN_DEC_8,
        MODE_SIGN_DEC_16,
        MODE_SIGN_DEC_32,
        MODE_SIGN_DEC_64,
        MODE_BIN_8,
        MODE_BIN_16,
        MODE_BIN_32,
        MODE_BIN_64,
        MODE_HEX_8,
        MODE_HEX_16,
        MODE_HEX_32,
        MODE_HEX_64,
        MODE_UUID,
        MODE_SIZE,
        MODE_DOUBLE
    };

    explicit XLineEditValidator(QObject *pParent = nullptr);

    void setMode(MODE mode);
    MODE getMode();
    virtual State validate(QString &sInput, int &nPos) const;
    void setMaxValue(qint64 nValue);

    static quint64 binStringToValue(const QString &sString, bool *pbSuccess = 0);
    static QString value8ToBinString(quint8 nValue);
    static QString value16ToBinString(quint16 nValue);
    static QString value32ToBinString(quint32 nValue);
    static QString value64ToBinString(quint64 nValue);
    static QString _valueToBinString(quint64 nValue, qint32 nBits);
    static QString _valueToSizeString(quint64 nValue);
    static qint32 getNumberOfBits(MODE mode);

private:
    MODE m_mode;
    qint64 m_nMaxValue;
};

#endif  // XLINEEDITVALIDATOR_H
