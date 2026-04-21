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

#ifndef XFMODEL_H
#define XFMODEL_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QXmlStreamWriter>
#include "xmodel.h"
#include "xbinary.h"

class XFModel : public XModel {
    Q_OBJECT

public:
    enum COLUMN {
        COLUMN_NAME = 0,
        COLUMN_OFFSET,
        COLUMN_SIZE,
        COLUMN_TYPE,
        COLUMN_VALUE,
        COLUMN_COMMENT,
        __COLUMN_SIZE
    };

    enum PRESENTATION_TYPE {
        PT_DATAST_LIST = 0,
        PT_DATAST_FLAGS,
        PT_DOSTIME,
        PT_DOSDATE,
        PT_UNIXTIME,
        PT_FILETIME,
        PT_VERSION,
        PT_COUNT,
        PT_SIZE
    };

    explicit XFModel(QObject *pParent);
    virtual ~XFModel() override;

    virtual void setData(XBinary *pXBinary, const XBinary::XFHEADER &xfHeader);

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    enum EXPORT_FORMAT {
        EXPORT_PLAINTEXT = 0,
        EXPORT_CSV,
        EXPORT_TSV,
        EXPORT_JSON,
        EXPORT_XML
    };

    void printToConsole(const QString &sTitle = QString()) const;
    QString modelToString(const QString &sTitle = QString()) const;

    static QString exportToString(const QAbstractItemModel *pModel, EXPORT_FORMAT exportFormat);
    static bool exportToFile(const QAbstractItemModel *pModel, EXPORT_FORMAT exportFormat, const QString &sFileName);
    static QString exportFormatToFilter(EXPORT_FORMAT exportFormat);
    static QString exportAllFilters();
    static EXPORT_FORMAT filterToExportFormat(const QString &sFilter);

    static QString presentationToString(PRESENTATION_TYPE presentationType, quint64 nValue, const XBinary::XFRECORD &xfRecord, const XBinary::XFDATAST &xfDataSt);

protected:
    QString valueToString(const XBinary::XFRECORD &xfRecord, const QVariant &varValue) const;
    QString flagsToString(quint64 nFlags) const;
    QString commentForField(qint32 nFieldIndex, const QVariant &varValue) const;

    static QList<qint32> calculateColumnWidths(const QAbstractItemModel *pModel);
    static QList<bool> calculateColumnAlignRight(const QAbstractItemModel *pModel);
    static QString formatRow(const QStringList &listValues, const QList<qint32> &listWidths, const QList<bool> &listAlignRight = QList<bool>());
    static QString separatorLine(const QList<qint32> &listWidths);

    XBinary *m_pXBinary;
    XBinary::XFHEADER m_xfHeader;
};

#endif  // XFMODEL_H
