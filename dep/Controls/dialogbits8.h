/* Copyright (c) 2023-2026 hors<horsicq@gmail.com>
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
#ifndef DIALOGBITS8_H
#define DIALOGBITS8_H

#include <QDialog>
#include <QToolButton>
#include "xlineeditvalidator.h"

namespace Ui {
class DialogBits8;
}

class DialogBits8 : public QDialog {
    Q_OBJECT

public:
    explicit DialogBits8(QWidget *pParent, bool bStayOnTop);
    ~DialogBits8();

    void setReadonly(bool bIsReadonly);
    void setValue_uint8(quint8 nValue);
    void setValue_uint16(quint16 nValue);
    void setValue_uint32(quint32 nValue);
    void setValue_uint64(quint64 nValue);
    void reload();
    quint8 getValue_uint8();
    quint16 getValue_uint16();
    quint32 getValue_uint32();
    quint64 getValue_uint64();

private slots:
    void _handleButton(QToolButton *pToolButton);
    void on_lineEditHex_textChanged(const QString &sString);
    void on_lineEditSigned_textChanged(const QString &sString);
    void on_lineEditUnsigned_textChanged(const QString &sString);
    void on_lineEditBin_textChanged(const QString &sString);
    void toggledSlot(bool bState);
    void enableControls(bool bState);
    void on_pushButtonCancel_clicked();

    void on_pushButtonOK_clicked();

private:
    Ui::DialogBits8 *ui;
    quint64 m_nValue;
    bool m_bIsReadonly;
    XLineEditValidator m_validatorHex;
    XLineEditValidator m_validatorSigned;
    XLineEditValidator m_validatorUnsigned;
    XLineEditValidator m_validatorBin;
    QList<QToolButton *> m_listButtons;
    qint32 m_nBits;
};

#endif  // DIALOGBITS8_H
