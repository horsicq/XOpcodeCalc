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
#include "xoptionswidget.h"

#include "ui_xoptionswidget.h"

XOptionsWidget::XOptionsWidget(QWidget *pParent) : XShortcutsWidget(pParent), ui(new Ui::XOptionsWidget)
{
    ui->setupUi(this);

    m_pParent = pParent;
    m_pOptions = nullptr;
    m_sApplicationDisplayName = "";

    connect(this, SIGNAL(saveSignal()), this, SLOT(save()), Qt::DirectConnection);
    connect(this, SIGNAL(reloadSignal()), this, SLOT(reload()), Qt::DirectConnection);

#ifdef Q_OS_WIN
    m_userRole = XOptions::USERROLE_NORMAL;
#endif
}

XOptionsWidget::~XOptionsWidget()
{
    delete ui;
}

void XOptionsWidget::adjustView()
{
    getGlobalOptions()->adjustWidget(this, XOptions::ID_VIEW_FONT_CONTROLS);
    XShortcutsWidget::adjustViewChildren(this);
}

void XOptionsWidget::setGlobal(XShortcuts *pShortcuts, XOptions *pXOptions)
{
    XShortcutsWidget::setGlobal(pShortcuts, pXOptions);
    XShortcutsWidget::setGlobalChildren(this, pShortcuts, pXOptions);
}

void XOptionsWidget::setOptions(XOptions *pOptions, const QString &sApplicationDisplayName)
{
    m_pOptions = pOptions;
    m_sApplicationDisplayName = sApplicationDisplayName;

    if (m_pOptions->isGroupIDPresent(XOptions::GROUPID_VIEW)) {
        addListRecord(tr("Appearance"), 0);
        ui->pageView->setProperty("GROUPID", XOptions::GROUPID_VIEW);
    }

    if (m_pOptions->isGroupIDPresent(XOptions::GROUPID_FILE)) {
        addListRecord(tr("File"), 1);
        ui->pageFile->setProperty("GROUPID", XOptions::GROUPID_FILE);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_CONTROLS) || m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TABLEVIEWS) ||
        m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TREEVIEWS) || m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TEXTEDITS)) {
        addListRecord(tr("Fonts"), 2);
        ui->pageFile->setProperty("GROUPID", XOptions::GROUPID_FONTS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_READBUFFERSIZE) || m_pOptions->isIDPresent(XOptions::ID_FEATURE_FILEBUFFERSIZE) ||
        m_pOptions->isIDPresent(XOptions::ID_FEATURE_SSE2) || m_pOptions->isIDPresent(XOptions::ID_FEATURE_AVX2)) {
        addListRecord(tr("Features"), 3);
        ui->pageFile->setProperty("GROUPID", XOptions::GROUPID_FEATURES);
    }

    reload();
}

void XOptionsWidget::addListRecord(const QString &sTitle, qint32 nIndex)
{
    QListWidgetItem *pItem = new QListWidgetItem;

    pItem->setText(sTitle);
    pItem->setData(Qt::UserRole, nIndex);

    ui->listWidgetOptions->addItem(pItem);

    XOptions::adjustListWidgetSize(ui->listWidgetOptions);
}

void XOptionsWidget::addPage(QWidget *pWidget, const QString &sTitle)
{
    qint32 nIndex = ui->stackedWidgetOptions->addWidget(pWidget);

    addListRecord(sTitle, nIndex);

    connect(this, SIGNAL(saveSignal()), pWidget, SLOT(save()), Qt::DirectConnection);
    connect(this, SIGNAL(reloadSignal()), pWidget, SLOT(reload()), Qt::DirectConnection);
}

void XOptionsWidget::setCurrentPage(qint32 nPage)
{
    if (nPage < ui->listWidgetOptions->count()) {
        ui->listWidgetOptions->setCurrentRow(nPage);
    }
}

void XOptionsWidget::setCurrentPage(XOptions::GROUPID groupId)
{
    if (groupId != XOptions::GROUPID_UNKNOWN) {
        qint32 nNumberOfPages = ui->stackedWidgetOptions->count();
        qint32 nNumberOfLists = ui->listWidgetOptions->count();

        for (qint32 i = 0; i < nNumberOfPages; i++) {
            if (ui->stackedWidgetOptions->widget(i)->property("GROUPID").toUInt() == groupId) {
                for (qint32 j = 0; j < nNumberOfLists; j++) {
                    qint32 nIndex = ui->listWidgetOptions->item(j)->data(Qt::UserRole).toInt();

                    if (nIndex == i) {
                        ui->listWidgetOptions->setCurrentRow(j);

                        break;
                    }
                }

                break;
            }
        }
    }
}

void XOptionsWidget::reloadData(bool bSaveSelection)
{
    Q_UNUSED(bSaveSelection)
    reload();
}

void XOptionsWidget::save()
{
    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_STAYONTOP)) {
        m_pOptions->getCheckBox(ui->checkBoxViewStayOnTop, XOptions::ID_VIEW_STAYONTOP);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_SINGLEAPPLICATION)) {
        m_pOptions->getCheckBox(ui->checkBoxViewSingleApplication, XOptions::ID_VIEW_SINGLEAPPLICATION);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_STYLE)) {
        m_pOptions->getComboBox(ui->comboBoxViewStyle, XOptions::ID_VIEW_STYLE);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_QSS)) {
        m_pOptions->getComboBox(ui->comboBoxViewQss, XOptions::ID_VIEW_QSS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_LANG)) {
        m_pOptions->getComboBox(ui->comboBoxViewLanguage, XOptions::ID_VIEW_LANG);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_SHOWLOGO)) {
        m_pOptions->getCheckBox(ui->checkBoxViewShowLogo, XOptions::ID_VIEW_SHOWLOGO);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_CONTROLS)) {
        m_pOptions->getLineEdit(ui->lineEditViewFontControls, XOptions::ID_VIEW_FONT_CONTROLS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TABLEVIEWS)) {
        m_pOptions->getLineEdit(ui->lineEditViewFontTables, XOptions::ID_VIEW_FONT_TABLEVIEWS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TREEVIEWS)) {
        m_pOptions->getLineEdit(ui->lineEditViewFontTrees, XOptions::ID_VIEW_FONT_TREEVIEWS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TEXTEDITS)) {
        m_pOptions->getLineEdit(ui->lineEditViewFontTextEdits, XOptions::ID_VIEW_FONT_TEXTEDITS);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_HEX_FONT)) {
        m_pOptions->getLineEdit(ui->lineEditHexFont, XOptions::ID_HEX_FONT);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_DISASM_FONT)) {
        m_pOptions->getLineEdit(ui->lineEditDisasmFont, XOptions::ID_DISASM_FONT);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVELASTDIRECTORY)) {
        m_pOptions->getCheckBox(ui->checkBoxFileSaveLastDirectory, XOptions::ID_FILE_SAVELASTDIRECTORY);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVEBACKUP)) {
        m_pOptions->getCheckBox(ui->checkBoxFileSaveBackup, XOptions::ID_FILE_SAVEBACKUP);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVERECENTFILES)) {
        m_pOptions->getCheckBox(ui->checkBoxFileSaveHistory, XOptions::ID_FILE_SAVERECENTFILES);
    }
#ifdef Q_OS_WIN
    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SETENV)) {
        m_pOptions->getCheckBox(ui->checkBoxFileSetEnvVar, XOptions::ID_FILE_SETENV);

        QString appDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
        QString formattedDir = QDir::toNativeSeparators(appDir);

        if (ui->checkBoxFileSetEnvVar->isChecked()) {
            m_pOptions->appendToUserPathVariable(formattedDir);
        } else {
            m_pOptions->removeFromUserPathVariable(formattedDir);
        }
    }
#endif

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_READBUFFERSIZE)) {
        m_pOptions->getComboBox(ui->comboBoxReadBufferSize, XOptions::ID_FEATURE_READBUFFERSIZE);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_FILEBUFFERSIZE)) {
        m_pOptions->getComboBox(ui->comboBoxFileBufferSize, XOptions::ID_FEATURE_FILEBUFFERSIZE);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_SSE2)) {
        m_pOptions->getCheckBox(ui->checkBoxSSE2, XOptions::ID_FEATURE_SSE2);
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_AVX2)) {
        m_pOptions->getCheckBox(ui->checkBoxAVX2, XOptions::ID_FEATURE_AVX2);
    }

    m_pOptions->save();
}

void XOptionsWidget::reload()
{
    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_STAYONTOP)) {
        m_pOptions->setCheckBox(ui->checkBoxViewStayOnTop, XOptions::ID_VIEW_STAYONTOP);
    } else {
        ui->checkBoxViewStayOnTop->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_SINGLEAPPLICATION)) {
        m_pOptions->setCheckBox(ui->checkBoxViewSingleApplication, XOptions::ID_VIEW_SINGLEAPPLICATION);
    } else {
        ui->checkBoxViewSingleApplication->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_STYLE)) {
        m_pOptions->setComboBox(ui->comboBoxViewStyle, XOptions::ID_VIEW_STYLE);
    } else {
        ui->groupBoxViewStyle->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_QSS)) {
        m_pOptions->setComboBox(ui->comboBoxViewQss, XOptions::ID_VIEW_QSS);
    } else {
        ui->groupBoxViewQss->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_LANG)) {
        m_pOptions->setComboBox(ui->comboBoxViewLanguage, XOptions::ID_VIEW_LANG);
    } else {
        ui->groupBoxViewLanguage->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_SHOWLOGO)) {
        m_pOptions->setCheckBox(ui->checkBoxViewShowLogo, XOptions::ID_VIEW_SHOWLOGO);
    } else {
        ui->checkBoxViewShowLogo->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_CONTROLS)) {
        m_pOptions->setLineEdit(ui->lineEditViewFontControls, XOptions::ID_VIEW_FONT_CONTROLS);
    } else {
        ui->groupBoxViewFontControls->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TEXTEDITS)) {
        m_pOptions->setLineEdit(ui->lineEditViewFontTextEdits, XOptions::ID_VIEW_FONT_TEXTEDITS);
    } else {
        ui->groupBoxViewFontTextEdits->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TABLEVIEWS)) {
        m_pOptions->setLineEdit(ui->lineEditViewFontTables, XOptions::ID_VIEW_FONT_TABLEVIEWS);
    } else {
        ui->groupBoxViewFontTables->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_VIEW_FONT_TREEVIEWS)) {
        m_pOptions->setLineEdit(ui->lineEditViewFontTrees, XOptions::ID_VIEW_FONT_TREEVIEWS);
    } else {
        ui->groupBoxViewFontTrees->hide();
    }
    if (m_pOptions->isIDPresent(XOptions::ID_HEX_FONT)) {
        m_pOptions->setLineEdit(ui->lineEditHexFont, XOptions::ID_HEX_FONT);
    } else {
        ui->groupBoxHexFont->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_DISASM_FONT)) {
        m_pOptions->setLineEdit(ui->lineEditDisasmFont, XOptions::ID_DISASM_FONT);
    } else {
        ui->groupBoxDisasmFont->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVELASTDIRECTORY)) {
        m_pOptions->setCheckBox(ui->checkBoxFileSaveLastDirectory, XOptions::ID_FILE_SAVELASTDIRECTORY);
    } else {
        ui->checkBoxFileSaveLastDirectory->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVEBACKUP)) {
        m_pOptions->setCheckBox(ui->checkBoxFileSaveBackup, XOptions::ID_FILE_SAVEBACKUP);
    } else {
        ui->checkBoxFileSaveBackup->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_SAVERECENTFILES)) {
        m_pOptions->setCheckBox(ui->checkBoxFileSaveHistory, XOptions::ID_FILE_SAVERECENTFILES);
    } else {
        ui->checkBoxFileSaveHistory->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FILE_CONTEXT)) {
#ifdef Q_OS_WIN
        // bool bAdmin = m_pOptions->checkContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), XOptions::USERROLE_ADMIN);
        // bool bUser = m_pOptions->checkContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), XOptions::USERROLE_NORMAL);

        // if (bAdmin &&
        //     (!bUser)) {
        //     m_userRole = XOptions::USERROLE_ADMIN;
        // }
        ui->checkBoxFileContext->setChecked(m_pOptions->checkContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), m_userRole));

        // Check if application directory is present in system PATH
        QString appDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
        QString formattedDir = QDir::toNativeSeparators(appDir);

        ui->checkBoxFileSetEnvVar->setChecked(m_pOptions->isPathInUserEnvironment(formattedDir));
#endif
    } else {
        ui->checkBoxFileContext->hide();
        ui->checkBoxFileSetEnvVar->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_READBUFFERSIZE)) {
        m_pOptions->setComboBox(ui->comboBoxReadBufferSize, XOptions::ID_FEATURE_READBUFFERSIZE);
    } else {
        ui->comboBoxReadBufferSize->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_FILEBUFFERSIZE)) {
        m_pOptions->setComboBox(ui->comboBoxFileBufferSize, XOptions::ID_FEATURE_FILEBUFFERSIZE);
    } else {
        ui->comboBoxFileBufferSize->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_SSE2)) {
        m_pOptions->setCheckBox(ui->checkBoxSSE2, XOptions::ID_FEATURE_SSE2);
#ifdef USE_XSIMD
        if (!xsimd_is_sse2_present()) {
            ui->checkBoxSSE2->hide();
        }
#endif
    } else {
        ui->checkBoxSSE2->hide();
    }

    if (m_pOptions->isIDPresent(XOptions::ID_FEATURE_AVX2)) {
        m_pOptions->setCheckBox(ui->checkBoxAVX2, XOptions::ID_FEATURE_AVX2);
#ifdef USE_XSIMD
        if (!xsimd_is_avx2_present()) {
            ui->checkBoxAVX2->hide();
        }
#endif
    } else {
        ui->checkBoxAVX2->hide();
    }
}

void XOptionsWidget::on_listWidgetOptions_currentRowChanged(int nCurrentRow)
{
    if (nCurrentRow < ui->stackedWidgetOptions->count()) {
        qint32 nIndex = ui->listWidgetOptions->item(nCurrentRow)->data(Qt::UserRole).toInt();
        ui->stackedWidgetOptions->setCurrentIndex(nIndex);
    }
}

void XOptionsWidget::on_checkBoxFileContext_toggled(bool bChecked)
{
    if (m_pOptions->isIDPresent(XOptions::ID_FILE_CONTEXT)) {
#ifdef Q_OS_WIN
        if (m_pOptions->checkContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), m_userRole) != bChecked) {
            bool bSuccess = false;

            if (bChecked) {
                bSuccess = m_pOptions->registerContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), qApp->applicationFilePath(),
                                                       m_userRole);
            } else {
                bSuccess = m_pOptions->clearContext(m_sApplicationDisplayName, m_pOptions->getValue(XOptions::ID_FILE_CONTEXT).toString(), m_userRole);
            }

            if (!bSuccess) {
                QMessageBox::critical(this, tr("Error"), tr("Please run the program as an administrator"));

                ui->checkBoxFileContext->setChecked(!bChecked);
            }
        }
#else
        Q_UNUSED(bChecked)
#endif
    }
}

void XOptionsWidget::on_checkBoxFileSetEnvVar_toggled(bool bChecked)
{
#ifdef Q_OS_WIN
    QString appDir = QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();
    QString formattedDir = QDir::toNativeSeparators(appDir);

    bool bIsSet = m_pOptions->isPathInUserEnvironment(formattedDir);

    if (bChecked) {
        if (!bIsSet) {
            m_pOptions->appendToUserPathVariable(formattedDir);
        }
    } else {
        if (bIsSet) {
            m_pOptions->removeFromUserPathVariable(formattedDir);
        }
    }
#else
    Q_UNUSED(bChecked)
#endif
}

void XOptionsWidget::on_toolButtonViewFontControls_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditViewFontControls);
}

void XOptionsWidget::on_toolButtonViewFontTables_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditViewFontTables);
}

void XOptionsWidget::on_toolButtonViewFontTrees_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditViewFontTrees);
}

void XOptionsWidget::on_toolButtonHexFont_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditHexFont);
}

void XOptionsWidget::on_toolButtonDisasmFont_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditDisasmFont);
}

void XOptionsWidget::on_toolButtonViewFontTextEdits_clicked()
{
    XOptions::handleFontButton(this, ui->lineEditViewFontTextEdits);
}

void XOptionsWidget::on_pushButtonDefault_clicked()
{
    m_pOptions->resetToDefault();

    emit reloadSignal();
}

void XOptionsWidget::on_pushButtonOK_clicked()
{
    emit saveSignal();

    if (m_pOptions->isRestartNeeded()) {
        QMessageBox::information(this, tr("Information"), tr("Please restart the application"));
    }

    m_pParent->close();
}

void XOptionsWidget::on_pushButtonCancel_clicked()
{
    m_pParent->close();
}

void XOptionsWidget::registerShortcuts(bool bState)
{
    Q_UNUSED(bState)
}

void XOptionsWidget::on_checkBoxSSE2_toggled(bool bChecked)
{
#ifdef USE_XSIMD
    if (bChecked) {
        if (!xsimd_is_sse2_present()) {
            ui->checkBoxSSE2->setChecked(false);
            return;
        }
        xsimd_set_sse2(1);
    } else {
        xsimd_set_sse2(0);
    }
#else
    Q_UNUSED(bChecked)
#endif
}

void XOptionsWidget::on_checkBoxAVX2_toggled(bool bChecked)
{
#ifdef USE_XSIMD
    if (bChecked) {
        if (!xsimd_is_avx2_present()) {
            ui->checkBoxAVX2->setChecked(false);
            return;
        }
        xsimd_set_avx2(1);
    } else {
        xsimd_set_avx2(0);
    }
#else
    Q_UNUSED(bChecked)
#endif
}
