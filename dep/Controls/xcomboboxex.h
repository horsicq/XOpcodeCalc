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
#ifndef XCOMBOBOXEX_H
#define XCOMBOBOXEX_H

#include <QComboBox>
#include <QHash>
#include <QStandardItemModel>
#include <QVariant>

#include "subclassofqstyleditemdelegate.h"

class XComboBoxEx : public QComboBox {
    Q_OBJECT

public:
    enum CBTYPE {
        CBTYPE_LIST = 0,
        CBTYPE_FLAGS,
        CBTYPE_ELIST,  // Extended list
        CBTYPE_CUSTOM_FLAGS
    };

    struct CUSTOM_FLAG {
        quint64 nValue;
        QString sString;
        bool bIsChecked;
        bool bIsReadOnly;
    };

    explicit XComboBoxEx(QWidget *pParent = nullptr);

    void setData(const QMap<quint64, QString> &mapData, CBTYPE cbtype = CBTYPE_LIST, quint64 nMask = 0, const QString &sTitle = "");
    void setValue(QVariant varValue);
    QVariant getValue();
    void setReadOnly(bool bIsReadOnly);
    QString getDescription();
    void addCustomFlags(const QString &sTitle, const QList<CUSTOM_FLAG> &listCustomFlags);
    void setCustomFlag(quint64 nValue);
    QList<quint64> getCustomFlags();
    static void _addCustomFlag(QList<CUSTOM_FLAG> *pListCustomFlags, quint64 nValue, const QString &sString, bool bChecked);
    void setItemEnabled(qint32 nRow, bool bState);

private:
    qint32 findItemIndex(quint64 nValue) const;

private slots:
    void currentIndexChangedSlot(qint32 nIndex);
    void itemChangedSlot(QStandardItem *pItem);

signals:
    void valueChanged(QVariant vValue);

private:
    QVariant m_varValue;
    bool m_bIsReadonly;
    QStandardItemModel m_model;
    CBTYPE m_cbtype;
    quint64 m_nMask;
    QMap<quint64, QString> m_mapData;
};

#define PXComboBoxEx XComboBoxEx *

#endif  // XCOMBOBOXEX_H
