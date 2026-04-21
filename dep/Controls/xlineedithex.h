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
#ifndef XLINEEDITHEX_H
#define XLINEEDITHEX_H

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QLineEdit>
#include <QMenu>
#include <QShortcut>
#include <QVariant>

#include "xlineeditvalidator.h"
#include "dialogbits8.h"

// TODO setMode
class XLineEditHEX : public QLineEdit {
    Q_OBJECT

public:
    // struct OPTIONS {
    //     bool bDemangle;              // TODO Check
    //     bool bShowHexAddress;        // TODO Check
    //     bool bShowHexOffset;         // TODO Check
    //     bool bShowHexRelAddress;     // TODO Check
    //     bool bShowDisasmAddress;     // TODO Check
    //     bool bShowDisasmOffset;      // TODO Check
    //     bool bShowDisasmRelAddress;  // TODO Check
    // };

    enum _MODE {
        _MODE_UNKNOWN = 0,
        _MODE_TEXT,
        _MODE_UUID,
        _MODE_SIZE,
        _MODE_DOUBLE,
        _MODE_HEX,
        _MODE_DEC,
        _MODE_SIGN_DEC,
        _MODE_BIN
        // TODO more
    };

    explicit XLineEditHEX(QWidget *pParent = nullptr);

    void setValue_uint8(quint8 nValue, _MODE mode = _MODE_HEX);
    void setValue_int8(qint8 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint16(quint16 nValue, _MODE mode = _MODE_HEX);
    void setValue_int16(qint16 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint32(quint32 nValue, _MODE mode = _MODE_HEX);
    void setValue_int32(qint32 nValue, _MODE mode = _MODE_HEX);
    void setValue_uint64(quint64 nValue, _MODE mode = _MODE_HEX);
    void setValue_int64(qint64 nValue, _MODE mode = _MODE_HEX);
    void setValidatorModeValue(XLineEditValidator::MODE mode, QVariant varValue);
    void setMode(_MODE mode);
    //    void setValueOS(quint64 nValue, HEXValidator::MODE validatorMode = HEXValidator::MODE_HEX);
    void setValue32_64(quint64 nValue, _MODE mode = _MODE_HEX);
    void setValue_String(const QString &sText, qint32 nMaxLength = 0);
    void setValue_UUID(const QString &sText);
    void setValue_double(double dValue);
    void setValue_Variant(const QVariant &variant);
    //    quint64 getIntValue();
    //    qint64 getSignValue();
    QVariant _getValue();

    void setMaxValue(qint64 nValue);

    quint8 getValue_uint8();
    qint8 getValue_int8();
    quint16 getValue_uint16();
    qint16 getValue_int16();
    quint32 getValue_uint32();
    qint32 getValue_int32();
    quint64 getValue_uint64();
    qint64 getValue_int64();

    //    static MODE getModeFromValue(quint64 nValue);
    static QString getFormatString(XLineEditValidator::MODE mode, qint64 nValue);
    static qint32 getWidthFromMode(QWidget *pWidget, XLineEditValidator::MODE mode);
    static qint32 getSymbolWidth(QWidget *pWidget);
    void setColon(bool bIsColon);
    bool isFocused();
    void setValidatorMode(XLineEditValidator::MODE validatorMode);
    XLineEditValidator::MODE getValidatorMode();

protected:
    //    void keyPressEvent(QKeyEvent* keyEvent);
    bool eventFilter(QObject *pObject, QEvent *pEvent);

private slots:
    void _setText(const QString &sText);
    void _returnPressed();
    void customContextMenu(const QPoint &nPos);
    void updateFont();

public slots:
    void _copy();
    void _copyValue();
    void _copySignValue();
    void _clearValue();
    void _bits();

signals:
    void valueChanged(QVariant varValue);
    void focused(bool bState);

private:
    QVariant m_vValue;
    XLineEditValidator m_validator;
    bool m_bIsColon;
    bool m_bIsFocused;
    _MODE m_mode;
};

#define PXLineEditHEX XLineEditHEX *

#endif  // XLINEEDITHEX_H
