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
#include "xlineedithex.h"

XLineEditHEX::XLineEditHEX(QWidget *pParent) : QLineEdit(pParent)
{
    m_bIsColon = false;
    m_bIsFocused = false;
    m_mode = _MODE_HEX;

    updateFont();

    setAlignment(Qt::AlignHCenter);

    connect(this, SIGNAL(textChanged(QString)), this, SLOT(_setText(QString)));
    // connect(this, SIGNAL(returnPressed()), this, SLOT(_returnPressed()));
    setValidator(&m_validator);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    installEventFilter(this);
}

void XLineEditHEX::setValue_uint8(quint8 nValue, _MODE mode)
{
    if (mode != _MODE_UNKNOWN) {
        m_mode = mode;
    }

    QString sText;

    if (m_mode == _MODE_HEX) {
        m_validator.setMode(XLineEditValidator::MODE_HEX_8);
        sText = QString("%1").arg(nValue, 2, 16, QChar('0'));
    } else if (m_mode == _MODE_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_DEC_8);
        sText = QString("%1").arg(nValue);
    } else if (m_mode == _MODE_SIGN_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_8);
        sText = QString("%1").arg((qint8)nValue);
    } else if (m_mode == _MODE_SIZE) {
        m_validator.setMode(XLineEditValidator::MODE_SIZE);
        sText = XLineEditValidator::_valueToSizeString(nValue);
    } else if (m_mode == _MODE_BIN) {
        m_validator.setMode(XLineEditValidator::MODE_BIN_8);
        sText = XLineEditValidator::_valueToBinString(nValue, 8);
    }

    QLineEdit::setText(sText);
    setValue_Variant(nValue);
}

void XLineEditHEX::setValue_int8(qint8 nValue, _MODE mode)
{
    setValue_uint8((quint8)nValue, mode);
}

void XLineEditHEX::setValue_uint16(quint16 nValue, _MODE mode)
{
    if (mode != _MODE_UNKNOWN) {
        m_mode = mode;
    }

    QString sText;

    if (m_mode == _MODE_HEX) {
        m_validator.setMode(XLineEditValidator::MODE_HEX_16);
        sText = QString("%1").arg(nValue, 4, 16, QChar('0'));
    } else if (m_mode == _MODE_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_DEC_16);
        sText = QString("%1").arg(nValue);
    } else if (m_mode == _MODE_SIGN_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_16);
        sText = QString("%1").arg((qint16)nValue);
    } else if (m_mode == _MODE_SIZE) {
        m_validator.setMode(XLineEditValidator::MODE_SIZE);
        sText = XLineEditValidator::_valueToSizeString(nValue);
    } else if (m_mode == _MODE_BIN) {
        m_validator.setMode(XLineEditValidator::MODE_BIN_16);
        sText = XLineEditValidator::_valueToBinString(nValue, 16);
    }

    QLineEdit::setText(sText);
    setValue_Variant(nValue);
}

void XLineEditHEX::setValue_int16(qint16 nValue, _MODE mode)
{
    setValue_uint16((quint16)nValue, mode);
}

void XLineEditHEX::setValue_uint32(quint32 nValue, _MODE mode)
{
    if (mode != _MODE_UNKNOWN) {
        m_mode = mode;
    }

    QString sText;

    if (m_mode == _MODE_HEX) {
        m_validator.setMode(XLineEditValidator::MODE_HEX_32);
        if (m_bIsColon) {
            quint16 nHigh = (quint16)(nValue >> 16);
            quint16 nLow = (quint16)(nValue);
            QString sHigh = QString("%1").arg(nHigh, 4, 16, QChar('0'));
            QString sLow = QString("%1").arg(nLow, 4, 16, QChar('0'));
            sText = QString("%1:%2").arg(sHigh, sLow);
        } else {
            sText = QString("%1").arg(nValue, 8, 16, QChar('0'));
        }
    } else if (m_mode == _MODE_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_DEC_32);
        sText = QString("%1").arg(nValue);
    } else if (m_mode == _MODE_SIGN_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_32);
        sText = QString("%1").arg((qint32)nValue);
    } else if (m_mode == _MODE_SIZE) {
        m_validator.setMode(XLineEditValidator::MODE_SIZE);
        sText = XLineEditValidator::_valueToSizeString(nValue);
    } else if (m_mode == _MODE_BIN) {
        m_validator.setMode(XLineEditValidator::MODE_BIN_32);
        sText = XLineEditValidator::_valueToBinString(nValue, 32);
    }

    QLineEdit::setText(sText);
    setValue_Variant(nValue);
}

void XLineEditHEX::setValue_int32(qint32 nValue, _MODE mode)
{
    setValue_uint32((quint32)nValue, mode);
}

void XLineEditHEX::setValue_uint64(quint64 nValue, _MODE mode)
{
    if (mode != _MODE_UNKNOWN) {
        m_mode = mode;
    }

    QString sText;

    if (m_mode == _MODE_HEX) {
        m_validator.setMode(XLineEditValidator::MODE_HEX_64);
        if (m_bIsColon) {
            quint32 nHigh = (quint32)(nValue >> 32);
            quint32 nLow = (quint32)(nValue);
            QString sHigh = QString("%1").arg(nHigh, 8, 16, QChar('0'));
            QString sLow = QString("%1").arg(nLow, 8, 16, QChar('0'));
            sText = QString("%1:%2").arg(sHigh, sLow);
        } else {
            sText = QString("%1").arg(nValue, 16, 16, QChar('0'));
        }
    } else if (m_mode == _MODE_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_DEC_64);
        sText = QString("%1").arg(nValue);
    } else if (m_mode == _MODE_SIGN_DEC) {
        m_validator.setMode(XLineEditValidator::MODE_SIGN_DEC_64);
        sText = QString("%1").arg((qint64)nValue);
    } else if (m_mode == _MODE_SIZE) {
        m_validator.setMode(XLineEditValidator::MODE_SIZE);
        sText = XLineEditValidator::_valueToSizeString(nValue);
    } else if (m_mode == _MODE_BIN) {
        m_validator.setMode(XLineEditValidator::MODE_BIN_64);
        sText = XLineEditValidator::_valueToBinString(nValue, 64);
    }

    QLineEdit::setText(sText);
    setValue_Variant(nValue);
}

void XLineEditHEX::setValue_int64(qint64 nValue, _MODE mode)
{
    setValue_uint64((quint64)nValue, mode);
}

void XLineEditHEX::setValidatorModeValue(XLineEditValidator::MODE mode, QVariant varValue)
{
    if (mode == XLineEditValidator::MODE_HEX_8) {
        setValue_uint8((quint8)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        setValue_uint16((quint16)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        setValue_uint32((quint32)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        setValue_uint64((quint64)varValue.toULongLong(), _MODE_HEX);
    } else if (mode == XLineEditValidator::MODE_DEC_8) {
        setValue_int8((quint8)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_16) {
        setValue_int16((quint16)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_32) {
        setValue_int32((quint32)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_DEC_64) {
        setValue_int64((quint64)varValue.toULongLong(), _MODE_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_8) {
        setValue_int8((qint8)varValue.toLongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_16) {
        setValue_int16((qint16)varValue.toLongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_32) {
        setValue_int32((qint32)varValue.toLongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_SIGN_DEC_64) {
        setValue_int64((qint64)varValue.toLongLong(), _MODE_SIGN_DEC);
    } else if (mode == XLineEditValidator::MODE_BIN_8) {
        setValue_uint8((quint8)varValue.toULongLong(), _MODE_BIN);
    } else if (mode == XLineEditValidator::MODE_BIN_16) {
        setValue_uint16((quint16)varValue.toULongLong(), _MODE_BIN);
    } else if (mode == XLineEditValidator::MODE_BIN_32) {
        setValue_uint32((quint32)varValue.toULongLong(), _MODE_BIN);
    } else if (mode == XLineEditValidator::MODE_BIN_64) {
        setValue_uint64((quint64)varValue.toULongLong(), _MODE_BIN);
    }
}

void XLineEditHEX::setMode(_MODE mode)
{
    XLineEditValidator::MODE validatorMode = getValidatorMode();

    qint32 nNumberOfBits = XLineEditValidator::getNumberOfBits(validatorMode);

    // TODO
    if (mode == _MODE_HEX) {
        if (nNumberOfBits == 8) {
            validatorMode = XLineEditValidator::MODE_HEX_8;
        } else if (nNumberOfBits == 16) {
            validatorMode = XLineEditValidator::MODE_HEX_16;
        } else if (nNumberOfBits == 32) {
            validatorMode = XLineEditValidator::MODE_HEX_32;
        } else if (nNumberOfBits == 64) {
            validatorMode = XLineEditValidator::MODE_HEX_64;
        }
    } else if (mode == _MODE_DEC) {
        if (nNumberOfBits == 8) {
            validatorMode = XLineEditValidator::MODE_DEC_8;
        } else if (nNumberOfBits == 16) {
            validatorMode = XLineEditValidator::MODE_DEC_16;
        } else if (nNumberOfBits == 32) {
            validatorMode = XLineEditValidator::MODE_DEC_32;
        } else if (nNumberOfBits == 64) {
            validatorMode = XLineEditValidator::MODE_DEC_64;
        }
    } else if (mode == _MODE_SIGN_DEC) {
        if (nNumberOfBits == 8) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_8;
        } else if (nNumberOfBits == 16) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_16;
        } else if (nNumberOfBits == 32) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_32;
        } else if (nNumberOfBits == 64) {
            validatorMode = XLineEditValidator::MODE_SIGN_DEC_64;
        }
    } else if (mode == _MODE_BIN) {
        if (nNumberOfBits == 8) {
            validatorMode = XLineEditValidator::MODE_BIN_8;
        } else if (nNumberOfBits == 16) {
            validatorMode = XLineEditValidator::MODE_BIN_16;
        } else if (nNumberOfBits == 32) {
            validatorMode = XLineEditValidator::MODE_BIN_32;
        } else if (nNumberOfBits == 64) {
            validatorMode = XLineEditValidator::MODE_BIN_64;
        }
    }

    QVariant varValue = _getValue();

    setValidatorModeValue(validatorMode, varValue);
}

// void XLineEditHEX::setValueOS(quint64 nValue, HEXValidator::MODE validatorMode)
//{
//     MODE mode = MODE_32;

//    if (sizeof(void *) == 8) {
//        mode = MODE_64;
//    }

//    setModeValue(mode, nValue, validatorMode);
//}

void XLineEditHEX::setValue32_64(quint64 nValue, _MODE mode)
{
    if (nValue >= 0xFFFFFFFF) {
        setValue_uint64((quint64)nValue, mode);
    } else {
        setValue_uint32((quint32)nValue, mode);
    }
}

void XLineEditHEX::setValue_String(const QString &sText, qint32 nMaxLength)
{
    m_mode = _MODE_TEXT;
    m_validator.setMode(XLineEditValidator::MODE_TEXT);

    if (nMaxLength) {
        setMaxLength(nMaxLength);
    }

    QLineEdit::setText(sText);
    setValue_Variant(sText);
}

void XLineEditHEX::setValue_UUID(const QString &sText)
{
    m_mode = _MODE_UUID;
    m_validator.setMode(XLineEditValidator::MODE_UUID);

    setInputMask(">NNNNNNNN-NNNN-NNNN-NNNN-NNNNNNNNNNNN;0");  // TODO Check

    QLineEdit::setText(sText);
    setValue_Variant(sText);
}

void XLineEditHEX::setValue_double(double dValue)
{
    m_mode = _MODE_DOUBLE;
    m_validator.setMode(XLineEditValidator::MODE_DOUBLE);

    QLineEdit::setText(QString::number(dValue, 'f', 2));
    setValue_Variant(dValue);
}

void XLineEditHEX::setValue_Variant(const QVariant &variant)
{
    if (m_vValue != variant) {
        m_vValue = variant;
        updateFont();

        emit valueChanged(variant);
    }
}

// quint64 XLineEditHEX::getIntValue()
//{
//     quint64 nResult = 0;

//    QString sText = text();

//    if (m_mode == _MODE_HEX) {
//        if (g_bIsColon) {
//            if (sText.contains(":")) {
//                sText = sText.remove(":");
//            }
//        }

//        nResult = sText.toULongLong(nullptr, 16);
//    } else if (m_mode == _MODE_DEC) {
//        nResult = sText.toULongLong(nullptr, 10);
//    } else if (m_mode == _MODE_SIGN_DEC) {
//        nResult = sText.toLongLong(nullptr, 10);
//    }

//    return nResult;
//}

// qint64 XLineEditHEX::getSignValue()
//{
//     qint64 nResult = 0;

//    QString sText = text();

//    if (m_mode == _MODE_HEX) {
//        if (g_bIsColon) {
//            if (sText.contains(":")) {
//                sText = sText.remove(":");
//            }
//        }

//        nResult = sText.toULongLong(nullptr, 16);
//        // TODO
//    } else if (m_mode == _MODE_DEC) {
//        nResult = sText.toULongLong(nullptr, 10);
//    } else if (m_mode == _MODE_SIGN_DEC) {
//        nResult = sText.toLongLong(nullptr, 10);
//    }

//    return nResult;
//}

QVariant XLineEditHEX::_getValue()
{
    return m_vValue;
}

void XLineEditHEX::setMaxValue(qint64 nValue)
{
    m_validator.setMaxValue(nValue);
}

quint8 XLineEditHEX::getValue_uint8()
{
    return (quint8)m_vValue.toUInt();
}

qint8 XLineEditHEX::getValue_int8()
{
    return (qint8)m_vValue.toUInt();
}

quint16 XLineEditHEX::getValue_uint16()
{
    return (quint16)m_vValue.toUInt();
}

qint16 XLineEditHEX::getValue_int16()
{
    return (qint16)m_vValue.toUInt();
}

quint32 XLineEditHEX::getValue_uint32()
{
    return m_vValue.toUInt();
}

qint32 XLineEditHEX::getValue_int32()
{
    return (qint32)m_vValue.toUInt();
}

quint64 XLineEditHEX::getValue_uint64()
{
    return m_vValue.toULongLong();
}

qint64 XLineEditHEX::getValue_int64()
{
    return (qint64)m_vValue.toULongLong();
}

// XLineEditHEX::MODE XLineEditHEX::getModeFromValue(quint64 nValue)
//{
//     MODE result = MODE_64;

//    if (((quint64)nValue) > 0xFFFFFFFF) {
//        result = MODE_64;
//    } else if (((quint64)nValue) > 0xFFFF) {
//        result = MODE_32;
//    } else if (((quint64)nValue) > 0xFF) {
//        result = MODE_16;
//    } else {
//        result = MODE_8;
//    }

//    return result;
//}

QString XLineEditHEX::getFormatString(XLineEditValidator::MODE mode, qint64 nValue)
{
    QString sResult;

    qint32 nMod = 8;

    if (mode == XLineEditValidator::MODE_HEX_8) {
        nMod = 2;
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        nMod = 4;
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        nMod = 8;
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        nMod = 16;  // TODO Check 14 symbols width?
    }

    sResult = QString("%1").arg(nValue, nMod, 16, QChar('0'));

    if (nValue < 0) {
        sResult = sResult.right(nMod);
    }

    return sResult;
}

qint32 XLineEditHEX::getWidthFromMode(QWidget *pWidget, XLineEditValidator::MODE mode)
{
    qint32 nSymbolWidth = getSymbolWidth(pWidget);
    qint32 nResult = nSymbolWidth * 8;

    // TODO Check
    if (mode == XLineEditValidator::MODE_HEX_8) {
        nResult = nSymbolWidth * 4;
    } else if (mode == XLineEditValidator::MODE_HEX_16) {
        nResult = nSymbolWidth * 6;
    } else if (mode == XLineEditValidator::MODE_HEX_32) {
        nResult = nSymbolWidth * 8;
    } else if (mode == XLineEditValidator::MODE_HEX_64) {
        nResult = nSymbolWidth * 12;
    }

    return nResult;
}

qint32 XLineEditHEX::getSymbolWidth(QWidget *pWidget)
{
    QFontMetrics fm(pWidget->font());

    return fm.boundingRect("W").width();  // TODO Check
}

void XLineEditHEX::setColon(bool bIsColon)
{
    m_bIsColon = bIsColon;
}

bool XLineEditHEX::isFocused()
{
    return m_bIsFocused;
}

void XLineEditHEX::setValidatorMode(XLineEditValidator::MODE validatorMode)
{
    QVariant variant = _getValue();
    setValidatorModeValue(validatorMode, variant);
}

XLineEditValidator::MODE XLineEditHEX::getValidatorMode()
{
    return m_validator.getMode();
}

bool XLineEditHEX::eventFilter(QObject *pObject, QEvent *pEvent)
{
    if (pEvent->type() == QEvent::FocusIn) {
        m_bIsFocused = true;

        emit focused(true);
    } else if (pEvent->type() == QEvent::FocusOut) {
        m_bIsFocused = false;

        emit focused(false);
    }

    return QLineEdit::eventFilter(pObject, pEvent);
}

// void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)
//{
//     qDebug("void XLineEditHEX::keyPressEvent(QKeyEvent *keyEvent)");
// }

void XLineEditHEX::_setText(const QString &sText)
{
    // TODO fix strings like 00011111111
    if (m_mode == _MODE_TEXT) {
        emit valueChanged(text());
    } else if (m_mode == _MODE_UUID) {
        emit valueChanged(displayText());
    } else if (m_mode == _MODE_HEX) {
        quint64 nCurrentValue = sText.toULongLong(nullptr, 16);

        if (m_vValue.toULongLong() != nCurrentValue) {
            m_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    } else if (m_mode == _MODE_DEC) {
        quint64 nCurrentValue = sText.toULongLong(nullptr, 10);

        if (m_vValue.toULongLong() != nCurrentValue) {
            m_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    } else if (m_mode == _MODE_SIGN_DEC) {
        qint64 nCurrentValue = sText.toLongLong(nullptr, 10);
        qint64 _nValue = m_vValue.toLongLong();

        if (_nValue != nCurrentValue) {
            m_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    } else if (m_mode == _MODE_BIN) {
        quint64 nCurrentValue = XLineEditValidator::binStringToValue(sText);

        if (m_vValue.toULongLong() != nCurrentValue) {
            m_vValue = nCurrentValue;
            updateFont();

            emit valueChanged(nCurrentValue);
        }
    }

    qint32 nCursorPosition = QLineEdit::cursorPosition();
    QLineEdit::setText(sText);
    QLineEdit::setCursorPosition(nCursorPosition);
}

void XLineEditHEX::_returnPressed()
{
    _setText(text());
}

void XLineEditHEX::customContextMenu(const QPoint &nPos)
{
    QMenu contextMenu(this);  // TODO

    QAction actionCopy(QString("%1: \"%2\"").arg(tr("Copy"), text()), this);
    connect(&actionCopy, SIGNAL(triggered()), this, SLOT(_copy()));
    contextMenu.addAction(&actionCopy);

    QAction actionCopyValue(QString("%1: \"%2\"").arg(tr("Copy"), QString::number(_getValue().toULongLong())), this);
    QAction actionCopySignValue(this);
    QAction actionBits(tr("Bits"), this);

    if ((m_mode == _MODE_HEX) || (m_mode == _MODE_DEC) || (m_mode == _MODE_SIGN_DEC) || (m_mode == _MODE_SIZE) || (m_mode == _MODE_BIN)) {
        connect(&actionCopyValue, SIGNAL(triggered()), this, SLOT(_copyValue()));
        contextMenu.addAction(&actionCopyValue);

        if (_getValue().toULongLong() != (quint64)_getValue().toLongLong()) {
            actionCopySignValue.setText(QString("%1: \"%2\"").arg(tr("Copy"), QString::number(_getValue().toLongLong())));
            connect(&actionCopySignValue, SIGNAL(triggered()), this, SLOT(_copySignValue()));
            contextMenu.addAction(&actionCopySignValue);
        }
    }

    if ((m_mode == _MODE_HEX) || (m_mode == _MODE_DEC) || (m_mode == _MODE_SIGN_DEC)) {
        contextMenu.addSeparator();
        connect(&actionBits, SIGNAL(triggered()), this, SLOT(_bits()));
        contextMenu.addAction(&actionBits);
    }

    QAction actionClearValue(tr("Clear"), this);

    if (!isReadOnly()) {
        contextMenu.addSeparator();

        connect(&actionClearValue, SIGNAL(triggered()), this, SLOT(_clearValue()));
        contextMenu.addAction(&actionClearValue);
    }

    contextMenu.exec(mapToGlobal(nPos));
}

void XLineEditHEX::updateFont()
{
    QFont _font = font();

    bool bIsBold = false;

    if ((m_mode == _MODE_DEC) || (m_mode == _MODE_SIGN_DEC) || (m_mode == _MODE_HEX) || (m_mode == _MODE_BIN)) {
        if (m_vValue.toLongLong() != 0) {
            bIsBold = true;
        }
    }

    if (bIsBold) {
        _font.setBold(bIsBold);
    }

    // TODO another modes

    setFont(_font);
}

void XLineEditHEX::_copy()
{
    QApplication::clipboard()->setText(text());
}

void XLineEditHEX::_copyValue()
{
    QApplication::clipboard()->setText(QString::number(_getValue().toULongLong()));
}

void XLineEditHEX::_copySignValue()
{
    QApplication::clipboard()->setText(QString::number(_getValue().toLongLong()));
}

void XLineEditHEX::_clearValue()
{
    XLineEditValidator::MODE mode = m_validator.getMode();

    // TODO UUID
    if (mode == XLineEditValidator::MODE_TEXT) {
        setValue_String("");
    } else {
        setValidatorModeValue(m_validator.getMode(), 0);
    }

    emit textEdited(text());
}

void XLineEditHEX::_bits()
{
    XLineEditValidator::MODE validatorMode = getValidatorMode();

    qint32 nNumberOfBits = XLineEditValidator::getNumberOfBits(validatorMode);

    if (nNumberOfBits == 8) {
        DialogBits8 dialog(this, true);
        dialog.setValue_uint8((quint8)_getValue().toULongLong());
        dialog.setReadonly(isReadOnly());

        if (dialog.exec() == QDialog::Accepted) {
            setValue_uint8(dialog.getValue_uint8(), _MODE_UNKNOWN);
        }
    } else if (nNumberOfBits == 16) {
        DialogBits8 dialog(this, true);
        dialog.setValue_uint16((quint16)_getValue().toULongLong());
        dialog.setReadonly(isReadOnly());

        if (dialog.exec() == QDialog::Accepted) {
            setValue_uint16(dialog.getValue_uint16(), _MODE_UNKNOWN);
        }
    } else if (nNumberOfBits == 32) {
        DialogBits8 dialog(this, true);
        dialog.setValue_uint32((quint32)_getValue().toULongLong());
        dialog.setReadonly(isReadOnly());

        if (dialog.exec() == QDialog::Accepted) {
            setValue_uint32(dialog.getValue_uint32(), _MODE_UNKNOWN);
        }
    } else if (nNumberOfBits == 64) {
        DialogBits8 dialog(this, true);
        dialog.setValue_uint64((quint64)_getValue().toULongLong());
        dialog.setReadonly(isReadOnly());

        if (dialog.exec() == QDialog::Accepted) {
            setValue_uint64(dialog.getValue_uint64(), _MODE_UNKNOWN);
        }
    }
}
