/* Copyright (c) 2020-2026 hors<horsicq@gmail.com>
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
#ifndef XOPTIONSWIDGET_H
#define XOPTIONSWIDGET_H

#include "xshortcutswidget.h"

#ifdef USE_XSIMD
#include "xsimd.h"
#endif

namespace Ui {
class XOptionsWidget;
}

class XOptionsWidget : public XShortcutsWidget {
    Q_OBJECT

public:
    explicit XOptionsWidget(QWidget *pParent = nullptr);
    ~XOptionsWidget();

    virtual void adjustView();
    virtual void setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions);

    void setOptions(XOptions *pOptions, const QString &sApplicationDisplayName);
    void addListRecord(const QString &sTitle, qint32 nIndex);
    void addPage(QWidget *pWidget, const QString &sTitle);
    void setCurrentPage(qint32 nPage);  // TODO Check -> remove
    void setCurrentPage(XOptions::GROUPID groupId);
    virtual void reloadData(bool bSaveSelection);

public slots:
    void save();
    void reload();

private slots:
    void on_listWidgetOptions_currentRowChanged(int nCurrentRow);
    void on_checkBoxFileContext_toggled(bool bChecked);
    void on_toolButtonViewFontControls_clicked();
    void on_toolButtonViewFontTables_clicked();
    void on_toolButtonViewFontTrees_clicked();
    void on_toolButtonHexFont_clicked();
    void on_toolButtonDisasmFont_clicked();
    void on_toolButtonViewFontTextEdits_clicked();
    void on_pushButtonDefault_clicked();
    void on_pushButtonOK_clicked();
    void on_pushButtonCancel_clicked();
    void on_checkBoxFileSetEnvVar_toggled(bool bChecked);
    void on_checkBoxSSE2_toggled(bool bChecked);
    void on_checkBoxAVX2_toggled(bool bChecked);

signals:
    void saveSignal();
    void reloadSignal();

protected:
    virtual void registerShortcuts(bool bState);

private:
    Ui::XOptionsWidget *ui;
    QWidget *m_pParent;
    XOptions *m_pOptions;
    QString m_sApplicationDisplayName;
#ifdef Q_OS_WIN
    XOptions::USERROLE m_userRole;
#endif
};

#endif  // XOPTIONSWIDGET_H
