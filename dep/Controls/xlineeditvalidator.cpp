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
#include "xlineeditvalidator.h"

XLineEditValidator::XLineEditValidator(QObject *pParent) : QValidator(pParent)
{
    m_mode = MODE_HEX_32;
    m_nMaxValue = 0x7FFFFFFFFFFFFFFF;
}

void XLineEditValidator::setMode(XLineEditValidator::MODE mode)
{
    m_mode = mode;
}

void XLineEditValidator::setMaxValue(qint64 nValue)
{
    m_nMaxValue = nValue;
}

XLineEditValidator::MODE XLineEditValidator::getMode()
{
    return m_mode;
}

QValidator::State XLineEditValidator::validate(QString &sInput, int &nPos) const
{
    Q_UNUSED(nPos)

    QValidator::State result = Acceptable;

    if (!sInput.isEmpty()) {
        if ((m_mode == MODE_HEX_8) || (m_mode == MODE_DEC_8) || (m_mode == MODE_SIGN_DEC_8) || (m_mode == MODE_BIN_8) || (m_mode == MODE_HEX_16) ||
            (m_mode == MODE_DEC_16) || (m_mode == MODE_SIGN_DEC_16) || (m_mode == MODE_BIN_16) || (m_mode == MODE_HEX_32) || (m_mode == MODE_DEC_32) ||
            (m_mode == MODE_SIGN_DEC_32) || (m_mode == MODE_BIN_32) || (m_mode == MODE_HEX_64) || (m_mode == MODE_DEC_64) || (m_mode == MODE_SIGN_DEC_64) ||
            (m_mode == MODE_BIN_64)) {
            qint64 nMax = 0;
            qint64 nMin = 0;
            qint32 nLenght = 0;
            // TODO Dec Lenght !!!

            // TODO optimize!
            if ((m_mode == MODE_HEX_8) || (m_mode == MODE_DEC_8) || (m_mode == MODE_BIN_8)) {
                nMax = UCHAR_MAX;
                nMin = 0;
            } else if (m_mode == MODE_SIGN_DEC_8) {
                nMax = SCHAR_MAX;
                nMin = SCHAR_MIN;
            } else if ((m_mode == MODE_HEX_16) || (m_mode == MODE_DEC_16) || (m_mode == MODE_BIN_16)) {
                nMax = USHRT_MAX;
                nMin = 0;
            } else if (m_mode == MODE_SIGN_DEC_16) {
                nMax = SHRT_MAX;
                nMin = SHRT_MIN;
            } else if ((m_mode == MODE_HEX_32) || (m_mode == MODE_DEC_32) || (m_mode == MODE_BIN_32)) {
                nMax = UINT_MAX;
                nMin = 0;
            } else if (m_mode == MODE_SIGN_DEC_32) {
                nMax = INT_MAX;
                nMin = INT_MIN;
            } else if ((m_mode == MODE_HEX_64) || (m_mode == MODE_DEC_64) || (m_mode == MODE_BIN_64)) {
                nMax = ULLONG_MAX;
                nMin = 0;
            } else if (m_mode == MODE_SIGN_DEC_64) {
                // TODO Check
#ifdef LLONG_MAX
                nMax = LLONG_MAX;
                nMin = LLONG_MIN;
#endif
#ifdef LONG_LONG_MAX
                nMax = LONG_LONG_MAX;
                nMin = LONG_LONG_MIN;
#endif
            }

            nMax = qMin(m_nMaxValue, nMax);

            if (m_mode == MODE_HEX_8) {
                nLenght = 2;
            } else if (m_mode == MODE_HEX_16) {
                nLenght = 4;
            } else if (m_mode == MODE_HEX_32) {
                nLenght = 8;
            } else if (m_mode == MODE_HEX_64) {
                nLenght = 16;
            } else if (m_mode == MODE_BIN_8) {
                nLenght = 8;
            } else if (m_mode == MODE_BIN_16) {
                nLenght = 16;
            } else if (m_mode == MODE_BIN_32) {
                nLenght = 32;
            } else if (m_mode == MODE_BIN_64) {
                nLenght = 64;
            }

            if ((m_mode == MODE_HEX_8) || (m_mode == MODE_HEX_16) || (m_mode == MODE_HEX_32) || (m_mode == MODE_HEX_64)) {
                result = Invalid;

                bool bSuccess = false;
                quint64 nValue = sInput.toULongLong(&bSuccess, 16);

                if (bSuccess && (sInput.length() <= nLenght)) {
                    if (m_mode != MODE_HEX_64) {
                        if ((qint64)nValue <= nMax) {
                            result = Acceptable;
                        }
                    } else {
                        result = Acceptable;
                    }
                }
            } else if ((m_mode == MODE_DEC_8) || (m_mode == MODE_DEC_16) || (m_mode == MODE_DEC_32) || (m_mode == MODE_DEC_64)) {
                result = Invalid;

                bool bSuccess = false;
                quint64 nValue = sInput.toULongLong(&bSuccess, 10);

                if (bSuccess) {
                    if (m_mode != MODE_DEC_64) {
                        if ((qint64)nValue <= nMax) {
                            result = Acceptable;
                        }
                    } else {
                        result = Acceptable;
                    }
                }
            } else if ((m_mode == MODE_SIGN_DEC_8) || (m_mode == MODE_SIGN_DEC_16) || (m_mode == MODE_SIGN_DEC_32) || (m_mode == MODE_SIGN_DEC_64)) {
                result = Invalid;

                bool bSuccess = false;
                qint64 nValue = sInput.toLongLong(&bSuccess, 10);

                if (bSuccess && (nValue <= nMax) && (nValue >= nMin)) {
                    result = Acceptable;
                } else if (sInput == "-") {
                    result = Intermediate;
                }
            } else if ((m_mode == MODE_BIN_8) || (m_mode == MODE_BIN_16) || (m_mode == MODE_BIN_32) || (m_mode == MODE_BIN_64)) {
                result = Invalid;

                bool bSuccess = false;
                quint64 nValue = binStringToValue(sInput, &bSuccess);

                if (bSuccess && (sInput.length() <= nLenght)) {
                    if (m_mode != MODE_BIN_64) {
                        if ((qint64)nValue <= nMax) {
                            result = Acceptable;
                        }
                    } else {
                        result = Acceptable;
                    }
                }
            }
        } else if (m_mode == MODE_DOUBLE) {
            QDoubleValidator val;
            val.setLocale(QLocale::C);
            val.setNotation(QDoubleValidator::StandardNotation);
            result = val.validate(sInput, nPos);
        } else if (m_mode == MODE_UUID) {
            // TODO
            // NNNNNNNN-NNNN-NNNN-NNNN-NNNNNNNNNNNN
            // As 5 uints
        }
    }

    return result;
}

quint64 XLineEditValidator::binStringToValue(const QString &sString, bool *pbSuccess)
{
    quint64 nResult = 0;

    bool bSuccess = false;
    if (pbSuccess == nullptr) {
        pbSuccess = &bSuccess;
    }

    *pbSuccess = true;

    qint32 nSize = sString.size();

    quint64 nMask = 1;

    for (qint32 i = nSize - 1; i >= 0; i--) {
        if (sString.at(i) == QChar('1')) {
            nResult += nMask;
        } else if (sString.at(i) != QChar('0')) {
            *pbSuccess = false;
            break;
        }

        nMask <<= 1;
    }

    return nResult;
}

QString XLineEditValidator::value8ToBinString(quint8 nValue)
{
    return _valueToBinString(nValue, 8);
}

QString XLineEditValidator::value16ToBinString(quint16 nValue)
{
    return _valueToBinString(nValue, 16);
}

QString XLineEditValidator::value32ToBinString(quint32 nValue)
{
    return _valueToBinString(nValue, 32);
}

QString XLineEditValidator::value64ToBinString(quint64 nValue)
{
    return _valueToBinString(nValue, 64);
}

QString XLineEditValidator::_valueToBinString(quint64 nValue, qint32 nBits)
{
    QString sResult;

    for (qint32 i = 0; i < nBits; i++) {
        if (nValue & 0x1) {
            sResult += "1";
        } else {
            sResult += "0";
        }
        nValue = nValue >> 1;
    }

    std::reverse(sResult.begin(), sResult.end());

    return sResult;
}

QString XLineEditValidator::_valueToSizeString(quint64 nValue)
{
    QString sResult;

    quint64 nBase = 1024;
    QString sValue;
    QString sUnit;

    if (nValue < nBase) {
        sValue = QString::number(nValue);
        sUnit = tr("Bytes");
    } else if (nValue < (nBase * nBase)) {
        sValue = QString::number((double)nValue / nBase, 'f', 2);
        sUnit = tr("KiB");
    } else if (nValue < (nBase * nBase * nBase)) {
        sValue = QString::number((double)nValue / (nBase * nBase), 'f', 2);
        sUnit = tr("MiB");
    } else if (nValue < (nBase * nBase * nBase * nBase)) {
        sValue = QString::number((double)nValue / (nBase * nBase * nBase), 'f', 2);
        sUnit = tr("GiB");
    } else {
        sValue = QString::number((double)nValue / (nBase * nBase * nBase * nBase), 'f', 2);
        sUnit = tr("TiB");
    }

    sResult = QString("%1 %2").arg(sValue, sUnit);

    return sResult;
}

qint32 XLineEditValidator::getNumberOfBits(MODE mode)
{
    qint32 nResult = 0;

    if ((mode == MODE_DEC_8) || (mode == MODE_SIGN_DEC_8) || (mode == MODE_BIN_8) || (mode == MODE_HEX_8)) {
        nResult = 8;
    } else if ((mode == MODE_DEC_16) || (mode == MODE_SIGN_DEC_16) || (mode == MODE_BIN_16) || (mode == MODE_HEX_16)) {
        nResult = 16;
    } else if ((mode == MODE_DEC_32) || (mode == MODE_SIGN_DEC_32) || (mode == MODE_BIN_32) || (mode == MODE_HEX_32)) {
        nResult = 32;
    } else if ((mode == MODE_DEC_64) || (mode == MODE_SIGN_DEC_64) || (mode == MODE_BIN_64) || (mode == MODE_HEX_64)) {
        nResult = 64;
    }

    return nResult;
}
