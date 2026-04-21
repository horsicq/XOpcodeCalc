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
#include "xdatetimeeditx.h"

XDateTimeEditX::XDateTimeEditX(QWidget *pParent) : QDateTimeEdit(pParent)
{
    m_dtType = DT_TYPE_UNKNOWN;

    connect(this, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(_setDateTime(QDateTime)));
}

void XDateTimeEditX::setValue(QVariant vValue, DT_TYPE dtType)
{
    if (m_vValue != vValue) {
        m_vValue = vValue;
        emit valueChanged(vValue);
    }

    if (m_dtType != dtType) {
        m_dtType = dtType;

        if (dtType == DT_TYPE_POSIX) {
            setDisplayFormat("yyyy-MM-dd hh:mm:ss");
            QDateTime dt = QDateTime::fromString("1970-01-01 00:00:00", "yyyy-MM-dd hh:mm:ss");
            setMinimumDateTime(QDateTime(dt));
            setDateTime(dt);
        }
    }

    if (m_dtType == DT_TYPE_POSIX) {
        QDateTime dt;
        dt.setMSecsSinceEpoch((quint64)vValue.toULongLong() * 1000);

        setDateTime(dt);
    }
}

QVariant XDateTimeEditX::getValue()
{
    return m_vValue;
}

void XDateTimeEditX::_setDateTime(const QDateTime &dtValue)
{
    quint64 nCurrentValue = 0;

    if (m_dtType == DT_TYPE_POSIX) {
        nCurrentValue = (quint64)dtValue.toMSecsSinceEpoch() / 1000;
    }

    QVariant vValue = nCurrentValue;

    if (m_vValue != vValue) {
        m_vValue = vValue;

        emit valueChanged(vValue);
    }
}
