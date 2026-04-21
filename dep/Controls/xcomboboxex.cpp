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
#include "xcomboboxex.h"

XComboBoxEx::XComboBoxEx(QWidget *pParent) : QComboBox(pParent)
{
    m_bIsReadonly = false;
    m_cbtype = CBTYPE_LIST;
    m_nMask = 0;

    SubclassOfQStyledItemDelegate *pDelegate = new SubclassOfQStyledItemDelegate(this);
    setItemDelegate(pDelegate);

    connect(this, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &XComboBoxEx::currentIndexChangedSlot);
    connect(&m_model, &QStandardItemModel::itemChanged, this, &XComboBoxEx::itemChangedSlot);
}

void XComboBoxEx::setData(const QMap<quint64, QString> &mapData, CBTYPE cbtype, quint64 nMask, const QString &sTitle)
{
    m_cbtype = cbtype;
    m_nMask = nMask;
    m_mapData = mapData;

    m_model.clear();
    m_model.setColumnCount(1);
    m_model.setRowCount(mapData.count() + 1);

    if ((cbtype == CBTYPE_LIST) || (cbtype == CBTYPE_ELIST)) {
        m_model.setItem(0, 0, new QStandardItem(""));
    } else if ((cbtype == CBTYPE_FLAGS) || (cbtype == CBTYPE_CUSTOM_FLAGS)) {
        QString sTitleLocal = sTitle.isEmpty() ? tr("Flags") : sTitle;
        m_model.setItem(0, 0, new QStandardItem(sTitleLocal));
    }

    qint32 nIndex = 1;
    QMapIterator<quint64, QString> iter(mapData);

    while (iter.hasNext()) {
        iter.next();

        QStandardItem *pItem = new QStandardItem(iter.value());
        pItem->setData(iter.key(), Qt::UserRole);

        if (cbtype == CBTYPE_FLAGS) {
            pItem->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            pItem->setData(Qt::Unchecked, Qt::CheckStateRole);
        }

        m_model.setItem(nIndex, 0, pItem);
        nIndex++;
    }

    setModel(&m_model);
}

void XComboBoxEx::setValue(QVariant varValue)
{
    m_varValue = varValue;

    if ((m_cbtype == CBTYPE_LIST) || (m_cbtype == CBTYPE_ELIST)) {
        quint64 nValue = varValue.toULongLong();

        if (m_cbtype == CBTYPE_ELIST) {
            nValue &= m_nMask;
        }

        qint32 nIndex = findItemIndex(nValue);
        setCurrentIndex((nIndex != -1) ? nIndex : 0);
    } else if (m_cbtype == CBTYPE_FLAGS) {
        quint64 nValue = varValue.toULongLong();
        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 nI = 1; nI < nNumberOfItems; nI++) {
            quint64 nItemValue = m_model.item(nI, 0)->data(Qt::UserRole).toULongLong();
            Qt::CheckState state = (nItemValue & nValue) ? Qt::Checked : Qt::Unchecked;
            m_model.item(nI, 0)->setData(state, Qt::CheckStateRole);
        }
    }
}

QVariant XComboBoxEx::getValue()
{
    return m_varValue;
}

void XComboBoxEx::setReadOnly(bool bIsReadOnly)
{
    m_bIsReadonly = bIsReadOnly;

    if (m_cbtype == CBTYPE_FLAGS) {
        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 nI = 0; nI < nNumberOfItems; nI++) {
            Qt::ItemFlags flags = bIsReadOnly ? Qt::ItemIsEnabled : (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            m_model.item(nI, 0)->setFlags(flags);
        }
    }
}

QString XComboBoxEx::getDescription()
{
    QString sResult;

    if (m_cbtype == CBTYPE_LIST) {
        sResult = m_mapData.value(m_varValue.toULongLong());
    } else if (m_cbtype == CBTYPE_ELIST) {
        sResult = m_mapData.value(m_varValue.toULongLong() & m_nMask);
    } else if (m_cbtype == CBTYPE_FLAGS) {
        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 nI = 1; nI < nNumberOfItems; nI++) {
            if (m_model.item(nI, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                if (!sResult.isEmpty()) {
                    sResult += " | ";
                }
                sResult += m_mapData.value(m_model.item(nI, 0)->data(Qt::UserRole).toULongLong());
            }
        }
    }

    return sResult;
}

void XComboBoxEx::addCustomFlags(const QString &sTitle, const QList<CUSTOM_FLAG> &listCustomFlags)
{
    m_cbtype = CBTYPE_CUSTOM_FLAGS;

    m_model.clear();
    m_model.setColumnCount(1);
    m_model.setItem(0, 0, new QStandardItem(sTitle));

    qint32 nNumberOfRecords = listCustomFlags.count();

    for (qint32 nI = 0; nI < nNumberOfRecords; nI++) {
        const CUSTOM_FLAG &flag = listCustomFlags.at(nI);

        QStandardItem *pItem = new QStandardItem(flag.sString);
        pItem->setData(flag.nValue, Qt::UserRole);
        pItem->setFlags(flag.bIsReadOnly ? Qt::ItemIsUserCheckable : (Qt::ItemIsUserCheckable | Qt::ItemIsEnabled));
        pItem->setData(flag.bIsChecked ? Qt::Checked : Qt::Unchecked, Qt::CheckStateRole);

        m_model.setItem(nI + 1, 0, pItem);
    }

    setModel(&m_model);
}

void XComboBoxEx::setCustomFlag(quint64 nValue)
{
    qint32 nNumberOfRecords = m_model.rowCount();

    for (qint32 nI = 1; nI < nNumberOfRecords; nI++) {
        if (m_model.item(nI, 0)->data(Qt::UserRole).toULongLong() == nValue) {
            setItemData(nI, Qt::Checked, Qt::CheckStateRole);
        }
    }
}

QList<quint64> XComboBoxEx::getCustomFlags()
{
    QList<quint64> listResult;

    qint32 nNumberOfRecords = m_model.rowCount();

    for (qint32 nI = 1; nI < nNumberOfRecords; nI++) {
        if (m_model.item(nI)->data(Qt::CheckStateRole).toUInt() == Qt::Checked) {
            listResult.append(m_model.item(nI)->data(Qt::UserRole).toULongLong());
        }
    }

    return listResult;
}

void XComboBoxEx::_addCustomFlag(QList<CUSTOM_FLAG> *pListCustomFlags, quint64 nValue, const QString &sString, bool bChecked)
{
    CUSTOM_FLAG record = {};
    record.nValue = nValue;
    record.sString = sString;
    record.bIsChecked = bChecked;

    pListCustomFlags->append(record);
}

void XComboBoxEx::setItemEnabled(qint32 nRow, bool bState)
{
    m_model.item(nRow, 0)->setEnabled(bState);
}

qint32 XComboBoxEx::findItemIndex(quint64 nValue) const
{
    qint32 nNumberOfItems = m_model.rowCount();

    for (qint32 nI = 1; nI < nNumberOfItems; nI++) {
        if (m_model.item(nI, 0)->data(Qt::UserRole).toULongLong() == nValue) {
            return nI;
        }
    }

    return -1;
}

void XComboBoxEx::currentIndexChangedSlot(qint32 nIndex)
{
    if (m_cbtype == CBTYPE_FLAGS) {
        if (nIndex) {
            setCurrentIndex(0);
        }
    } else if (m_cbtype == CBTYPE_ELIST) {
        if (!m_bIsReadonly) {
            if (nIndex) {
                quint64 nCurrentValue = itemData(nIndex).toULongLong();
                quint64 nValue = (m_varValue.toULongLong() & ~m_nMask) | nCurrentValue;

                if (nValue != m_varValue.toULongLong()) {
                    m_varValue = nValue;
                    emit valueChanged(m_varValue);
                }
            }
        } else {
            setValue(m_varValue);
        }
    } else if (m_cbtype == CBTYPE_LIST) {
        if (!m_bIsReadonly) {
            if (nIndex) {
                QVariant vCurrentValue = itemData(nIndex);

                if (vCurrentValue != m_varValue) {
                    m_varValue = vCurrentValue;
                    emit valueChanged(m_varValue);
                }
            }
        } else {
            setValue(m_varValue);
        }
    }
}

void XComboBoxEx::itemChangedSlot(QStandardItem *pItem)
{
    Q_UNUSED(pItem)

    if ((m_cbtype == CBTYPE_FLAGS) && count()) {
        quint64 nCurrentValue = m_varValue.toULongLong();
        qint32 nNumberOfItems = m_model.rowCount();

        for (qint32 nI = 1; nI < nNumberOfItems; nI++) {
            quint64 nItemValue = m_model.item(nI, 0)->data(Qt::UserRole).toULongLong();

            if (m_model.item(nI, 0)->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
                nCurrentValue |= nItemValue;
            } else {
                nCurrentValue &= ~nItemValue;
            }
        }

        if (nCurrentValue != m_varValue.toULongLong()) {
            m_varValue = nCurrentValue;
            emit valueChanged(nCurrentValue);
        }
    }
}
