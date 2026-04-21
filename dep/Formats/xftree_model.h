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

#ifndef XFTREE_MODEL_H
#define XFTREE_MODEL_H

#include <QAbstractItemModel>
#include "xbinary.h"

class XFTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum COLUMN {
        COLUMN_NAME = 0,
        COLUMN_TYPE,
        COLUMN_OFFSET,
        COLUMN_INFO,
        __COLUMN_SIZE
    };

    struct TREEITEM {
        XBinary::XFHEADER xfHeader;
        TREEITEM *pParent;
        QList<TREEITEM *> listChildren;
        qint32 nRow;
    };

    explicit XFTreeModel(QObject *pParent = nullptr);
    virtual ~XFTreeModel() override;

    void setData(XBinary *pXBinary, const QList<XBinary::XFHEADER> &listHeaders);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &child) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    TREEITEM *rootItem() const;
    TREEITEM *itemFromIndex(const QModelIndex &index) const;

    static QString treeToString(XFTreeModel *pModel, const QString &sTitle = QString());
    static void printToConsole(XFTreeModel *pModel, const QString &sTitle = QString());

    QString toString(XBinary::FORMATTYPE formatType = XBinary::FORMATTYPE_UNKNOWN);
    QString toFormattedString();
    QString toXML() const;
    QString toJSON() const;
    QString toCSV() const;
    QString toTSV() const;

private:
    void clear();
    void buildTree(const QList<XBinary::XFHEADER> &listHeaders);
    static void appendTreeLines(QStringList *pListLines, XBinary *pXBinary, TREEITEM *pItem, const QString &sPrefix);
    static void _toFormattedString(QString *pString, XBinary *pXBinary, TREEITEM *pItem, qint32 nLevel);
    static void appendXMLLines(QStringList *pListLines, XBinary *pXBinary, TREEITEM *pItem, const QString &sIndent);
    static void appendJSONLines(QStringList *pListLines, XBinary *pXBinary, TREEITEM *pItem, const QString &sIndent, bool bLast);
    static void appendSVLines(QStringList *pListLines, XBinary *pXBinary, TREEITEM *pItem, QChar cSep);
    static QString getItemName(XBinary *pXBinary, TREEITEM *pItem);
    static QString getItemString(XBinary *pXBinary, TREEITEM *pItem);
    static QString getItemType(TREEITEM *pItem);
    static QString getItemFileType(TREEITEM *pItem);
    static QString getItemOffset(TREEITEM *pItem);
    static QString getItemSize(TREEITEM *pItem);
    static QString getItemRows(TREEITEM *pItem);
    static QString xmlEscape(const QString &s);
    static QString jsonEscape(const QString &s);
    static QString svQuote(const QString &s, QChar cSep);

    TREEITEM *m_pRootItem;
    XBinary *m_pXBinary;
};

#endif  // XFTREE_MODEL_H
