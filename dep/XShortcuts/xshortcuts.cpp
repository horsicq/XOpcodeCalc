/* Copyright (c) 2021-2026 hors<horsicq@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include "xshortcuts.h"

XShortcuts::XShortcuts(QObject *pParent) : QObject(pParent)
{
    m_bIsNative = false;
}

XShortcuts::~XShortcuts()
{
}

void XShortcuts::setName(const QString &sValue)
{
    this->m_sName = sValue;

#ifdef Q_OS_WIN
    m_sName += ".win.ini";
#endif
#ifdef Q_OS_LINUX
    m_sName += ".linux.ini";
#endif
#ifdef Q_OS_MACOS
    m_sName += ".macos.ini";
#endif
}

void XShortcuts::setNative(bool bValue, const QString &sApplicationDataPath)
{
    m_bIsNative = bValue;
    m_sApplicationDataPath = sApplicationDataPath;

    if (m_sApplicationDataPath.isEmpty()) {
        m_sApplicationDataPath = qApp->applicationDirPath();
    }
}

bool XShortcuts::isNative() const
{
    return m_bIsNative;
}

void XShortcuts::addGroup(GROUPID groupId)
{
    if (groupId == GROUPID_FILE) {
        addId(X_ID_FILE_OPEN);
        addId(X_ID_FILE_NEW);
        addId(X_ID_FILE_SAVE);
        addId(X_ID_FILE_SAVEAS);
        addId(X_ID_FILE_CLOSE);
        addId(X_ID_FILE_PRINT);
        addId(X_ID_FILE_EXIT);
    } else if (groupId == GROUPID_VIEW) {
    } else if (groupId == GROUPID_DEBUGGER) {
        addId(X_ID_DEBUGGER_FILE_OPEN);
        addId(X_ID_DEBUGGER_FILE_CLOSE);
        addId(X_ID_DEBUGGER_FILE_ATTACH);
        addId(X_ID_DEBUGGER_FILE_DETACH);
        addId(X_ID_DEBUGGER_FILE_EXIT);
        addId(X_ID_DEBUGGER_VIEW_CPU);
        addId(X_ID_DEBUGGER_VIEW_LOG);
        addId(X_ID_DEBUGGER_VIEW_BREAKPOINTS);
        addId(X_ID_DEBUGGER_VIEW_MEMORYMAP);
        addId(X_ID_DEBUGGER_VIEW_CALLSTACK);
        addId(X_ID_DEBUGGER_VIEW_THREADS);
        addId(X_ID_DEBUGGER_VIEW_HANDLES);
        addId(X_ID_DEBUGGER_VIEW_MODULES);
        addId(X_ID_DEBUGGER_VIEW_SYMBOLS);
        addId(X_ID_DEBUGGER_TOOLS_SHORTCUTS);
        addId(X_ID_DEBUGGER_TOOLS_OPTIONS);
        addId(X_ID_DEBUGGER_HELP_ABOUT);
        addId(X_ID_DEBUGGER_DEBUG_RUN);
        addId(X_ID_DEBUGGER_DEBUG_PAUSE);
        addId(X_ID_DEBUGGER_DEBUG_STEPINTO);
        addId(X_ID_DEBUGGER_DEBUG_STEPOVER);
        addId(X_ID_DEBUGGER_DEBUG_CLOSE);
        addId(X_ID_DEBUGGER_DEBUG_RESTART);
        addId(X_ID_DEBUGGER_ANIMATE_STEPINTO);
        addId(X_ID_DEBUGGER_ANIMATE_STEPOVER);
        addId(X_ID_DEBUGGER_ANIMATE_STOP);
        addId(X_ID_DEBUGGER_TRACE_STEPINTO);
        addId(X_ID_DEBUGGER_TRACE_STEPOVER);
        addId(X_ID_DEBUGGER_TRACE_STOP);
        addId(X_ID_DEBUGGER_DISASM_BREAKPOINT_TOGGLE);
        addId(X_ID_DEBUGGER_DISASM_BREAKPOINT_REMOVE);
        addId(X_ID_DEBUGGER_DISASM_BREAKPOINT_CONDITIONAL);
        addId(X_ID_DEBUGGER_DISASM_DUMPTOFILE);
        addId(X_ID_DEBUGGER_DISASM_GOTO_ADDRESS);
        addId(X_ID_DEBUGGER_DISASM_HEX_SIGNATURE);
        addId(X_ID_DEBUGGER_DISASM_SIGNATURE);
        addId(X_ID_DEBUGGER_DISASM_FIND_STRING);
        addId(X_ID_DEBUGGER_DISASM_FIND_VALUE);
        addId(X_ID_DEBUGGER_DISASM_FIND_SIGNATURE);
        addId(X_ID_DEBUGGER_DISASM_FIND_NEXT);
        addId(X_ID_DEBUGGER_DISASM_COPY_HEX);
        addId(X_ID_DEBUGGER_DISASM_COPY_ADDRESS);
        addId(X_ID_DEBUGGER_DISASM_SELECT_ALL);
        addId(X_ID_DEBUGGER_DISASM_EDIT_HEX);
        addId(X_ID_DEBUGGER_DISASM_FOLLOWIN_HEX);
        addId(X_ID_DEBUGGER_HEX_DUMPTOFILE);
        addId(X_ID_DEBUGGER_HEX_FIND_STRING);
        addId(X_ID_DEBUGGER_HEX_FIND_VALUE);
        addId(X_ID_DEBUGGER_HEX_FIND_SIGNATURE);
        addId(X_ID_DEBUGGER_HEX_FIND_NEXT);
        addId(X_ID_DEBUGGER_HEX_GOTO_ADDRESS);
        addId(X_ID_DEBUGGER_HEX_SIGNATURE);
        addId(X_ID_DEBUGGER_HEX_COPY_HEX);
        addId(X_ID_DEBUGGER_HEX_COPY_ADDRESS);
        addId(X_ID_DEBUGGER_HEX_SELECT_ALL);
        addId(X_ID_DEBUGGER_HEX_EDIT_HEX);
        addId(X_ID_DEBUGGER_HEX_FOLLOWIN_DISASM);
        addId(X_ID_DEBUGGER_STACK_GOTO_ADDRESS);
        addId(X_ID_DEBUGGER_STACK_COPY_ADDRESS);
        addId(X_ID_DEBUGGER_STACK_COPY_VALUE);
        addId(X_ID_DEBUGGER_STACK_EDIT_HEX);
        addId(X_ID_DEBUGGER_STACK_FOLLOWIN_DISASM);
        addId(X_ID_DEBUGGER_STACK_FOLLOWIN_HEX);
        addId(X_ID_DEBUGGER_REGISTERS_EDIT);
        addId(X_ID_DEBUGGER_REGISTERS_CLEAR);
        addId(X_ID_DEBUGGER_REGISTERS_FOLLOWIN_DISASM);
        addId(X_ID_DEBUGGER_REGISTERS_FOLLOWIN_HEX);
        addId(X_ID_DEBUGGER_REGISTERS_COPY);
        addId(X_ID_DEBUGGER_BREAKPOINT_HARDWARE_0);
        addId(X_ID_DEBUGGER_BREAKPOINT_HARDWARE_1);
        addId(X_ID_DEBUGGER_BREAKPOINT_HARDWARE_2);
        addId(X_ID_DEBUGGER_BREAKPOINT_HARDWARE_3);
        addId(X_ID_DEBUGGER_BREAKPOINT_HARDWARE_SHOW);
    } else if (groupId == GROUPID_DEBUG) {
    } else if (groupId == GROUPID_ACTION) {
        addId(createShortcutsId(groupId, QList<GROUPID>(), BASEID_COPY));
        addId(createShortcutsId(groupId, QList<GROUPID>(), BASEID_SHOW));
    } else if (groupId == GROUPID_STRINGS) {
        addId(X_ID_STRINGS_FOLLOWIN_HEX);
        addId(X_ID_STRINGS_DEMANGLE);
        addId(X_ID_STRINGS_EDIT_STRING);
    } else if (groupId == GROUPID_SIGNATURES) {
        addId(X_ID_SIGNATURES_COPY_NAME);
        addId(X_ID_SIGNATURES_COPY_SIGNATURE);
        addId(X_ID_SIGNATURES_COPY_ADDRESS);
        addId(X_ID_SIGNATURES_COPY_OFFSET);
        addId(X_ID_SIGNATURES_FOLLOWIN_HEX);
    } else if (groupId == GROUPID_FIND) {
        addId(X_ID_FIND_FOLLOWIN_DISASM);
        addId(X_ID_FIND_FOLLOWIN_HEX);
    } else if (groupId == GROUPID_STRUCT) {
    } else if (groupId == GROUPID_HEX) {
        addId(X_ID_HEX_DATA_INSPECTOR);
        addId(X_ID_HEX_DATA_CONVERTOR);
        addId(X_ID_HEX_MULTISEARCH);
        addId(X_ID_HEX_VISUALIZATION);
        addId(X_ID_HEX_DUMPTOFILE);
        addId(X_ID_HEX_GOTO_OFFSET);
        addId(X_ID_HEX_GOTO_ADDRESS);
        addId(X_ID_HEX_GOTO_SELECTION_START);
        addId(X_ID_HEX_GOTO_SELECTION_END);
        addId(X_ID_HEX_SIGNATURE);
        addId(X_ID_HEX_FIND_STRING);
        addId(X_ID_HEX_FIND_SIGNATURE);
        addId(X_ID_HEX_FIND_VALUE);
        addId(X_ID_HEX_FIND_NEXT);
        addId(X_ID_HEX_SELECT_ALL);
        addId(X_ID_HEX_COPY_DATA);
        addId(X_ID_HEX_COPY_OFFSET);
        addId(X_ID_HEX_COPY_ADDRESS);
        addId(X_ID_HEX_FOLLOWIN_DISASM);
        addId(X_ID_HEX_FOLLOWIN_MEMORYMAP);
        addId(X_ID_HEX_FOLLOWIN_HEX);
        addId(X_ID_HEX_EDIT_HEX);
        addId(X_ID_HEX_EDIT_PATCH);
        addId(X_ID_HEX_EDIT_REMOVE);
        addId(X_ID_HEX_EDIT_RESIZE);
        addId(X_ID_HEX_STRUCTS);
        addId(X_ID_HEX_BOOKMARKS_NEW);
        addId(X_ID_HEX_BOOKMARKS_LIST);
        addId(X_ID_HEX_STRINGS);
#if defined(QT_SCRIPT_LIB) || defined(QT_QML_LIB)
        addId(X_ID_HEX_SCRIPTS);
#endif
    } else if (groupId == GROUPID_DISASM) {
        addId(X_ID_DISASM_DUMPTOFILE);
        addId(X_ID_DISASM_GOTO_OFFSET);
        addId(X_ID_DISASM_GOTO_ADDRESS);
        addId(X_ID_DISASM_GOTO_ENTRYPOINT);
        addId(X_ID_DISASM_GOTO_REFERENCES);
        //        addId(X_ID_DISASM_GOTO_XREF);
        addId(X_ID_DISASM_SIGNATURE);
        addId(X_ID_DISASM_HEX_SIGNATURE);
        addId(X_ID_DISASM_FIND_STRING);
        addId(X_ID_DISASM_FIND_SIGNATURE);
        addId(X_ID_DISASM_FIND_VALUE);
        addId(X_ID_DISASM_FIND_NEXT);
        addId(X_ID_DISASM_SELECT_ALL);
        addId(X_ID_DISASM_COPY_DATA);
        addId(X_ID_DISASM_COPY_OFFSET);
        addId(X_ID_DISASM_COPY_ADDRESS);
        addId(X_ID_DISASM_FOLLOWIN_HEX);
        addId(X_ID_DISASM_EDIT_HEX);
        addId(X_ID_DISASM_EDIT_PATCH);
        addId(X_ID_DISASM_ANALYZE_DISASM);
        addId(X_ID_DISASM_ANALYZE_REMOVE);
        addId(X_ID_DISASM_ANALYZE_ALL);
        addId(X_ID_DISASM_ANALYZE_ANALYZE);
        addId(X_ID_DISASM_ANALYZE_SYMBOLS);
        addId(X_ID_DISASM_ANALYZE_FUNCTIONS);
        addId(X_ID_DISASM_ANALYZE_CLEAR);
    } else if (groupId == GROUPID_STACK) {
    } else if (groupId == GROUPID_REGISTERS) {
    } else if (groupId == GROUPID_ARCHIVE) {
        addId(X_ID_ARCHIVE_OPEN);
        addId(X_ID_ARCHIVE_SCAN);
        addId(X_ID_ARCHIVE_HEX);
        addId(X_ID_ARCHIVE_STRINGS);
        addId(X_ID_ARCHIVE_ENTROPY);
        addId(X_ID_ARCHIVE_HASH);
        addId(X_ID_ARCHIVE_COPY_FILENAME);
        addId(X_ID_ARCHIVE_DUMPTOFILE);
    } else if (groupId == GROUPID_TABLE) {
        addId(X_ID_TABLE_DEMANGLE);
        addId(X_ID_TABLE_EDIT);
        addId(X_ID_TABLE_SELECTION_HEX);
        addId(X_ID_TABLE_SELECTION_DISASM);
        addId(X_ID_TABLE_SELECTION_ENTROPY);
        addId(X_ID_TABLE_SELECTION_DUMPTOFILE);
        addId(X_ID_TABLE_FOLLOWIN_DISASM);
        addId(X_ID_TABLE_FOLLOWIN_HEX);
        addId(X_ID_TABLE_EDIT_STRING);
    } else if (groupId == GROUPID_PROCESS) {
        addId(X_ID_PROCESS_STRUCTS);
        addId(X_ID_PROCESS_DUMPTOFILE);
        addId(X_ID_PROCESS_MEMORY_HEX);
        addId(X_ID_PROCESS_MEMORY_STRINGS);
        addId(X_ID_PROCESS_MEMORY_SIGNATURES);
        addId(X_ID_PROCESS_MEMORY_MEMORYMAP);
        addId(X_ID_PROCESS_MEMORY_MODULES);
        addId(X_ID_PROCESS_SHOWIN_VIEWER);
        addId(X_ID_PROCESS_SHOWIN_FOLDER);
        addId(X_ID_PROCESS_COPY_PID);
        addId(X_ID_PROCESS_COPY_NAME);
        addId(X_ID_PROCESS_COPY_FILENAME);
    } else if (groupId == GROUPID_MEMORY) {
    } else if (groupId == GROUPID_MEMORYMAP) {
        addId(X_ID_MEMORYMAP_DUMPTOFILE);
        addId(X_ID_MEMORYMAP_SHOWIN_FOLDER);
    } else if (groupId == GROUPID_MODULES) {
        addId(X_ID_MODULES_DUMPTOFILE);
        addId(X_ID_MODULES_SHOWIN_FOLDER);
    } else if (groupId == GROUPID_COPY) {
    } else if (groupId == GROUPID_GOTO) {
    } else if (groupId == GROUPID_EDIT) {
    } else if (groupId == GROUPID_TOOLS) {
    } else if (groupId == GROUPID_HELP) {
    } else if (groupId == GROUPID_SELECT) {
    } else if (groupId == GROUPID_SELECTION) {
    } else if (groupId == GROUPID_FOLLOWIN) {
    } else if (groupId == GROUPID_SHOWIN) {
    } else if (groupId == GROUPID_BREAKPOINT) {
    } else if (groupId == GROUPID_SCAN) {
        addId(X_ID_SCAN_EDITOR_FIND_STRING);
        addId(X_ID_SCAN_EDITOR_FIND_NEXT);
    }
}

void XShortcuts::addId(quint64 nId)
{
    setShortcut(nId, getDefault(nId));
}

QList<XShortcuts::RECORD> XShortcuts::getRecords()
{
    return m_listRecords;
}

QSettings *XShortcuts::_openSettings() const
{
    if (m_bIsNative) {
        return new QSettings;
    } else if (!m_sName.isEmpty()) {
        return new QSettings(m_sApplicationDataPath + QDir::separator() + m_sName, QSettings::IniFormat);  // TODO more options
    }
    return nullptr;
}

qint32 XShortcuts::_findRecordIndex(quint64 nId) const
{
    const qint32 nCount = m_listRecords.count();
    for (qint32 i = 0; i < nCount; i++) {
        if (m_listRecords.at(i).nId == nId) {
            return i;
        }
    }
    return -1;
}

void XShortcuts::load()
{
    QSettings *pSettings = _openSettings();

#ifdef QT_DEBUG
    if (pSettings) {
        qDebug("XShortcuts::load(): Loaded file from %s", pSettings->fileName().toUtf8().data());
    }
#endif

    if (pSettings) {
        qint32 nNumberOfRecords = m_listRecords.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            quint64 nId = m_listRecords.at(i).nId;
            QKeySequence ksDefault = getDefault(nId);

            QString sString = idToSettingsString(nId);

            setShortcut(nId, QKeySequence::fromString(pSettings->value(sString, ksDefault).toString()));
        }

        delete pSettings;
    }
}

void XShortcuts::save()
{
    QSettings *pSettings = _openSettings();

#ifdef QT_DEBUG
    if (pSettings) {
        qDebug("XShortcuts::save(): Saved file to %s", pSettings->fileName().toUtf8().data());
    }
#endif

    if (pSettings) {
        qint32 nNumberOfRecords = m_listRecords.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            quint64 nId = m_listRecords.at(i).nId;

            QString sString = idToSettingsString(nId);

            pSettings->setValue(sString, m_listRecords.at(i).keySequence.toString());
        }

        delete pSettings;
    }
}

QKeySequence XShortcuts::getShortcut(quint64 nId)
{
#ifdef QT_DEBUG
    if (!isIdPresent(nId)) {
        QString sErrorString = idToSettingsString(nId);
        qDebug("%s", sErrorString.toLatin1().data());
    }
#endif

    qint32 nIndex = _findRecordIndex(nId);
    if (nIndex != -1) {
        return m_listRecords.at(nIndex).keySequence;
    }
    return QKeySequence();
}

bool XShortcuts::isIdPresent(quint64 nId)
{
    return _findRecordIndex(nId) != -1;
}

void XShortcuts::setShortcut(quint64 nId, QKeySequence keySequence)
{
    qint32 nIndex = _findRecordIndex(nId);
    if (nIndex != -1) {
        m_listRecords[nIndex].keySequence = keySequence;
    } else {
        RECORD record = {};
        record.nId = nId;
        record.keySequence = keySequence;
        m_listRecords.append(record);
    }
}

bool XShortcuts::checkShortcut(quint64 nId, QKeySequence keySequence)
{
    bool bResult = true;

    if (keySequence != QKeySequence()) {
        GROUPID idGroup = getGroupId(nId);

        qint32 nNumberOfRecords = m_listRecords.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            quint64 _nId = m_listRecords.at(i).nId;

            if (_nId != nId) {
                if (getGroupId(_nId) == idGroup) {
                    QKeySequence _keySequence = m_listRecords.at(i).keySequence;
                    if (_keySequence == keySequence) {
                        bResult = false;
                        break;
                    }
                }
            }
        }
    }

    return bResult;
}

QString XShortcuts::idToSettingsString(quint64 nId)
{
    QString sResult;

    GROUPID groupId = getGroupId(nId);
    QList<GROUPID> listSubgroupIds = getSubgroupIds(nId);
    BASEID baseId = getBaseId(nId);

    sResult += QString("XShortcuts/%1/").arg(groupIdToSettingsString(groupId));

    qint32 nNumberOfRecords = listSubgroupIds.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        sResult += QString("%1/").arg(groupIdToSettingsString(listSubgroupIds.at(i)));
    }

    sResult += QString("%1").arg(baseIdToSettingsString(baseId));

    return sResult;
}

QKeySequence XShortcuts::getDefault(quint64 nId)
{
    // TODO use defines
    QKeySequence ksResult = QKeySequence();

    GROUPID groupId = getGroupId(nId);

    if (groupId == GROUPID_FILE) {
        if (nId == X_ID_FILE_OPEN) ksResult = Qt::CTRL | Qt::Key_O;
        else if (nId == X_ID_FILE_SAVE) ksResult = QKeySequence();
        else if (nId == X_ID_FILE_SAVEAS) ksResult = QKeySequence();
        else if (nId == X_ID_FILE_CLOSE) ksResult = QKeySequence();
        else if (nId == X_ID_FILE_PRINT) ksResult = QKeySequence();
        else if (nId == X_ID_FILE_EXIT) ksResult = Qt::ALT | Qt::Key_X;
    } else if (groupId == GROUPID_VIEW) {
        if (nId == X_ID_VIEW_FULLSCREEN) ksResult = Qt::CTRL | Qt::Key_E;
    } else if (groupId == GROUPID_DEBUGGER) {
        if (nId == X_ID_DEBUGGER_FILE_OPEN) ksResult = QKeySequence::Open;
        else if (nId == X_ID_DEBUGGER_FILE_CLOSE) ksResult = QKeySequence::Close;
        else if (nId == X_ID_DEBUGGER_FILE_ATTACH) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_FILE_DETACH) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_FILE_EXIT) ksResult = QKeySequence::Quit;
        else if (nId == X_ID_DEBUGGER_VIEW_CPU) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_LOG) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_BREAKPOINTS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_MEMORYMAP) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_CALLSTACK) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_THREADS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_HANDLES) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_MODULES) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_VIEW_SYMBOLS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_TOOLS_SHORTCUTS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_TOOLS_OPTIONS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HELP_ABOUT) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DEBUG_RUN) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F9;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F9;
#endif
        } else if (nId == X_ID_DEBUGGER_DEBUG_PAUSE) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F12;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F12;
#endif
        } else if (nId == X_ID_DEBUGGER_DEBUG_STEPINTO) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F7;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F7;
#endif
        } else if (nId == X_ID_DEBUGGER_DEBUG_STEPOVER) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F8;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F8;
#endif
        } else if (nId == X_ID_DEBUGGER_DEBUG_CLOSE) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F12;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F12;
#endif
        } else if (nId == X_ID_DEBUGGER_DEBUG_RESTART) {
#ifdef Q_OS_WIN
            ksResult = Qt::CTRL | Qt::Key_F2;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::CTRL | Qt::Key_F2;
#endif
        } else if (nId == X_ID_DEBUGGER_DISASM_BREAKPOINT_TOGGLE) {
#ifdef Q_OS_WIN
            ksResult = Qt::Key_F2;
#endif
#ifdef Q_OS_LINUX
            ksResult = Qt::Key_F2;
#endif
        } else if (nId == X_ID_DEBUGGER_DISASM_DUMPTOFILE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_GOTO_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_HEX_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_FIND_STRING) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_FIND_VALUE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_FIND_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_FIND_NEXT) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_COPY_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_COPY_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_SELECT_ALL) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_DISASM_EDIT_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_DUMPTOFILE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_FIND_STRING) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_FIND_VALUE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_FIND_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_FIND_NEXT) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_GOTO_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_COPY_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_COPY_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_SELECT_ALL) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_HEX_EDIT_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_STACK_GOTO_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_STACK_COPY_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_STACK_COPY_VALUE) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_STACK_EDIT_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_REGISTERS_EDIT) ksResult = Qt::Key_Return;  // mb Enter
        else if (nId == X_ID_DEBUGGER_REGISTERS_CLEAR) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_REGISTERS_FOLLOWIN_DISASM) ksResult = QKeySequence();
        else if (nId == X_ID_DEBUGGER_REGISTERS_FOLLOWIN_HEX) ksResult = QKeySequence();
    } else if (groupId == GROUPID_DEBUG) {
    } else if (groupId == GROUPID_ACTION) {
    } else if (groupId == GROUPID_STRINGS) {
        if (nId == X_ID_STRINGS_FOLLOWIN_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_STRINGS_DEMANGLE) ksResult = QKeySequence();
        else if (nId == X_ID_STRINGS_EDIT_STRING) ksResult = QKeySequence();
    } else if (groupId == GROUPID_SIGNATURE) {
        if (nId == X_ID_SIGNATURES_COPY_NAME) ksResult = QKeySequence::Copy;
        else if (nId == X_ID_SIGNATURES_COPY_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_SIGNATURES_COPY_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_SIGNATURES_COPY_OFFSET) ksResult = QKeySequence();
        else if (nId == X_ID_SIGNATURES_FOLLOWIN_HEX) ksResult = QKeySequence();
    } else if (groupId == GROUPID_STRUCT) {
    } else if (groupId == GROUPID_HEX) {
        if (nId == X_ID_HEX_DATA_INSPECTOR) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_DATA_CONVERTOR) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_DUMPTOFILE) ksResult = Qt::CTRL | Qt::Key_D;
        else if (nId == X_ID_HEX_GOTO_OFFSET) ksResult = Qt::Key_G;
        else if (nId == X_ID_HEX_GOTO_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FIND_STRING) ksResult = QKeySequence::Find;
        else if (nId == X_ID_HEX_FIND_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FIND_VALUE) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FIND_NEXT) ksResult = QKeySequence::FindNext;
        else if (nId == X_ID_HEX_SELECT_ALL) ksResult = QKeySequence::SelectAll;
        else if (nId == X_ID_HEX_COPY_DATA) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_COPY_OFFSET) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_COPY_ADDRESS) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FOLLOWIN_DISASM) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FOLLOWIN_MEMORYMAP) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_FOLLOWIN_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_EDIT_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_EDIT_REMOVE) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_EDIT_RESIZE) ksResult = QKeySequence();
        else if (nId == X_ID_HEX_STRUCTS) ksResult = Qt::Key_H;
    } else if (groupId == GROUPID_DISASM) {
        if (nId == X_ID_DISASM_DUMPTOFILE) ksResult = Qt::CTRL | Qt::Key_D;
        else if (nId == X_ID_DISASM_GOTO_OFFSET) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_GOTO_ADDRESS) ksResult = Qt::Key_G;
        else if (nId == X_ID_DISASM_GOTO_ENTRYPOINT) ksResult = Qt::Key_E;
        else if (nId == X_ID_DISASM_GOTO_REFERENCES) ksResult = Qt::Key_X;
        else if (nId == X_ID_DISASM_SIGNATURE) ksResult = Qt::SHIFT | Qt::Key_G;
        else if (nId == X_ID_DISASM_HEX_SIGNATURE) ksResult = Qt::Key_S;
        else if (nId == X_ID_DISASM_FIND_STRING) ksResult = QKeySequence::Find;
        else if (nId == X_ID_DISASM_FIND_SIGNATURE) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_FIND_VALUE) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_FIND_NEXT) ksResult = QKeySequence::FindNext;
        else if (nId == X_ID_DISASM_SELECT_ALL) ksResult = QKeySequence::SelectAll;
        else if (nId == X_ID_DISASM_COPY_DATA) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_COPY_OFFSET) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_COPY_ADDRESS) ksResult = QKeySequence::Copy;
        else if (nId == X_ID_DISASM_FOLLOWIN_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_DISASM_ANALYZE_DISASM) ksResult = Qt::Key_D;
        else if (nId == X_ID_DISASM_ANALYZE_REMOVE) ksResult = Qt::Key_Delete;
    } else if (groupId == GROUPID_STACK) {
    } else if (groupId == GROUPID_REGISTERS) {
    } else if (groupId == GROUPID_ARCHIVE) {
        if (nId == X_ID_ARCHIVE_OPEN) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_SCAN) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_STRINGS) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_ENTROPY) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_HASH) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_COPY_FILENAME) ksResult = QKeySequence();
        else if (nId == X_ID_ARCHIVE_DUMPTOFILE) ksResult = QKeySequence();
    } else if (groupId == GROUPID_TABLE) {
    } else if (groupId == GROUPID_PROCESS) {
        if (nId == X_ID_PROCESS_STRUCTS) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_DUMPTOFILE) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_MEMORY_HEX) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_MEMORY_STRINGS) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_MEMORY_SIGNATURES) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_MEMORY_MEMORYMAP) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_MEMORY_MODULES) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_SHOWIN_VIEWER) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_SHOWIN_FOLDER) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_COPY_PID) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_COPY_NAME) ksResult = QKeySequence();
        else if (nId == X_ID_PROCESS_COPY_FILENAME) ksResult = QKeySequence();
    } else if (groupId == GROUPID_MEMORY) {
    } else if (groupId == GROUPID_MEMORYMAP) {
        if (nId == X_ID_MEMORYMAP_DUMPTOFILE) ksResult = QKeySequence();
        else if (nId == X_ID_MEMORYMAP_SHOWIN_FOLDER) ksResult = QKeySequence();
    } else if (groupId == GROUPID_MODULES) {
        if (nId == X_ID_MODULES_DUMPTOFILE) ksResult = QKeySequence();
        else if (nId == X_ID_MODULES_SHOWIN_FOLDER) ksResult = QKeySequence();
    } else if (groupId == GROUPID_COPY) {
    } else if (groupId == GROUPID_GOTO) {
    } else if (groupId == GROUPID_EDIT) {
    } else if (groupId == GROUPID_FIND) {
    } else if (groupId == GROUPID_TOOLS) {
    } else if (groupId == GROUPID_HELP) {
    } else if (groupId == GROUPID_SELECT) {
    } else if (groupId == GROUPID_SELECTION) {
    } else if (groupId == GROUPID_FOLLOWIN) {
    } else if (groupId == GROUPID_SHOWIN) {
    } else if (groupId == GROUPID_BREAKPOINT) {
    } else if (groupId == GROUPID_SCAN) {
        if (nId == X_ID_SCAN_EDITOR_FIND_STRING) ksResult = QKeySequence::Find;
        else if (nId == X_ID_SCAN_EDITOR_FIND_NEXT) ksResult = QKeySequence::FindNext;
    }

    return ksResult;
}

QString XShortcuts::groupIdToString(GROUPID groupId)
{
    QString sResult;

    if (groupId == GROUPID_ACTION) sResult = tr("Action");
    else if (groupId == GROUPID_FILE) sResult = tr("File");
    else if (groupId == GROUPID_VIEW) sResult = tr("View");
    else if (groupId == GROUPID_STRING) sResult = tr("String");
    else if (groupId == GROUPID_STRINGS) sResult = tr("Strings");
    else if (groupId == GROUPID_SIGNATURE) sResult = tr("Signature");
    else if (groupId == GROUPID_SIGNATURES) sResult = tr("Signatures");
    else if (groupId == GROUPID_STRUCT) sResult = tr("Struct");
    else if (groupId == GROUPID_HEX) sResult = tr("Hex");
    else if (groupId == GROUPID_DATA) sResult = tr("Data");
    else if (groupId == GROUPID_DISASM) sResult = tr("Disasm");
    else if (groupId == GROUPID_DEBUG) sResult = tr("Debug");
    else if (groupId == GROUPID_TRACE) sResult = tr("Trace");
    else if (groupId == GROUPID_ANIMATE) sResult = tr("Animate");
    else if (groupId == GROUPID_DEBUGGER) sResult = tr("Debugger");
    else if (groupId == GROUPID_REGISTERS) sResult = tr("Registers");
    else if (groupId == GROUPID_REGISTER) sResult = tr("Register");
    else if (groupId == GROUPID_STACK) sResult = tr("Stack");
    else if (groupId == GROUPID_ARCHIVE) sResult = tr("Archive");
    else if (groupId == GROUPID_TABLE) sResult = tr("Table");
    else if (groupId == GROUPID_PROCESS) sResult = tr("Process");
    else if (groupId == GROUPID_MEMORY) sResult = tr("Memory");
    else if (groupId == GROUPID_COPY) sResult = tr("Copy");
    else if (groupId == GROUPID_EDIT) sResult = tr("Edit");
    else if (groupId == GROUPID_FIND) sResult = tr("Find");
    else if (groupId == GROUPID_GOTO) sResult = tr("Go to");
    else if (groupId == GROUPID_TOOLS) sResult = tr("Tools");
    else if (groupId == GROUPID_HELP) sResult = tr("Help");
    else if (groupId == GROUPID_SELECT) sResult = tr("Select");
    else if (groupId == GROUPID_SELECTION) sResult = tr("Selection");
    else if (groupId == GROUPID_FOLLOWIN) sResult = tr("Follow in");
    else if (groupId == GROUPID_SHOWIN) sResult = tr("Show in");
    else if (groupId == GROUPID_BREAKPOINT) sResult = tr("Breakpoint");
    else if (groupId == GROUPID_MODULES) sResult = tr("Modules");
    else if (groupId == GROUPID_MEMORYMAP) sResult = tr("Memory map");
    else if (groupId == GROUPID_VALUE) sResult = tr("Value");
    else if (groupId == GROUPID_SCAN) sResult = tr("Scan");
    else if (groupId == GROUPID_EDITOR) sResult = tr("Editor");
    else if (groupId == GROUPID_BOOKMARKS) sResult = tr("Bookmarks");
    else if (groupId == GROUPID_ANALYZE) sResult = tr("Analyze");
    else if (groupId == GROUPID_HARDWARE) sResult = tr("Hardware");
    else if (groupId == GROUPID_LOCATION) sResult = tr("Location");
    else if (groupId == GROUPID_MODE) sResult = tr("Mode");
    else if (groupId == GROUPID_WIDTH) sResult = tr("Width");
    else if (groupId == GROUPID_BASE) sResult = tr("Base");
    else {
#ifdef QT_DEBUG
        qDebug("Unknown groupId");
#endif
    }

    return sResult;
}

QString XShortcuts::baseIdToString(BASEID baseId)
{
    QString sResult;

    if (baseId == BASEID_COPY) sResult = tr("Copy");
    else if (baseId == BASEID_SHOW) sResult = tr("Show");
    else if (baseId == BASEID_OPEN) sResult = tr("Open");
    else if (baseId == BASEID_NEW) sResult = tr("New");
    else if (baseId == BASEID_SAVE) sResult = tr("Save");
    else if (baseId == BASEID_SAVEAS) sResult = tr("Save as");
    else if (baseId == BASEID_CLOSE) sResult = tr("Close");
    else if (baseId == BASEID_PRINT) sResult = tr("Print");
    else if (baseId == BASEID_EXIT) sResult = tr("Exit");
    else if (baseId == BASEID_DUMPTOFILE) sResult = tr("Dump to file");
    else if (baseId == BASEID_ADDRESS) sResult = tr("Address");
    else if (baseId == BASEID_END) sResult = tr("End");
    else if (baseId == BASEID_START) sResult = tr("Start");
    else if (baseId == BASEID_ENTRYPOINT) sResult = tr("Entry point");
    else if (baseId == BASEID_XREF) sResult = QString("XREF");
    else if (baseId == BASEID_OFFSET) sResult = tr("Offset");
    else if (baseId == BASEID_SIZE) sResult = tr("Size");
    else if (baseId == BASEID_STRING) sResult = tr("String");
    else if (baseId == BASEID_STRINGS) sResult = tr("Strings");
    else if (baseId == BASEID_SCRIPTS) sResult = tr("Scripts");
    else if (baseId == BASEID_SIGNATURE) sResult = tr("Signature");
    else if (baseId == BASEID_SIGNATURES) sResult = tr("Signatures");
    else if (baseId == BASEID_HEX) sResult = tr("Hex");
    else if (baseId == BASEID_PATCH) sResult = tr("Patch");
    else if (baseId == BASEID_OPCODE) sResult = tr("Opcode");
    else if (baseId == BASEID_DEMANGLE) sResult = tr("Demangle");
    else if (baseId == BASEID_NAME) sResult = tr("Name");
    else if (baseId == BASEID_NEXT) sResult = tr("Next");
    else if (baseId == BASEID_DATA) sResult = tr("Data");
    else if (baseId == BASEID_VALUE) sResult = tr("Value");
    else if (baseId == BASEID_ALL) sResult = tr("All");
    else if (baseId == BASEID_DISASM) sResult = tr("Disasm");
    else if (baseId == BASEID_MEMORYMAP) sResult = tr("Memory map");
    else if (baseId == BASEID_ATTACH) sResult = tr("Attach");
    else if (baseId == BASEID_DETACH) sResult = tr("Detach");
    else if (baseId == BASEID_CPU) sResult = QString("CPU");
    else if (baseId == BASEID_LOG) sResult = tr("Log");
    else if (baseId == BASEID_BREAKPOINTS) sResult = tr("Breakpoints");
    else if (baseId == BASEID_CALLSTACK) sResult = tr("Callstack");
    else if (baseId == BASEID_THREADS) sResult = tr("Threads");
    else if (baseId == BASEID_HANDLES) sResult = tr("Handles");
    else if (baseId == BASEID_MODULES) sResult = tr("Modules");
    else if (baseId == BASEID_SYMBOLS) sResult = tr("Symbols");
    else if (baseId == BASEID_FUNCTIONS) sResult = tr("Functions");
    else if (baseId == BASEID_CLEAR) sResult = tr("Clear");
    else if (baseId == BASEID_SHORTCUTS) sResult = tr("Shortcuts");
    else if (baseId == BASEID_OPTIONS) sResult = tr("Options");
    else if (baseId == BASEID_ABOUT) sResult = tr("About");
    else if (baseId == BASEID_FILENAME) sResult = tr("File name");
    else if (baseId == BASEID_STRUCTS) sResult = tr("Structs");
    else if (baseId == BASEID_VIEWER) sResult = tr("Viewer");
    else if (baseId == BASEID_FOLDER) sResult = tr("Folder");
    else if (baseId == BASEID_PID) sResult = QString("PID");
    else if (baseId == BASEID_RUN) sResult = tr("Run");
    else if (baseId == BASEID_PAUSE) sResult = tr("Pause");
    else if (baseId == BASEID_STEPINTO) sResult = tr("Step into");
    else if (baseId == BASEID_STEPOVER) sResult = tr("Step over");
    else if (baseId == BASEID_STOP) sResult = tr("Stop");
    else if (baseId == BASEID_RESTART) sResult = tr("Restart");
    else if (baseId == BASEID_TOGGLE) sResult = tr("Toggle");
    else if (baseId == BASEID_SCAN) sResult = tr("Scan");
    else if (baseId == BASEID_ENTROPY) sResult = tr("Entropy");
    else if (baseId == BASEID_HASH) sResult = tr("Hash");
    else if (baseId == BASEID_STACK) sResult = tr("Stack");
    else if (baseId == BASEID_FULLSCREEN) sResult = tr("Full screen");
    else if (baseId == BASEID_REFERENCES) sResult = tr("References");
    else if (baseId == BASEID_BOOKMARK) sResult = tr("Bookmark");
    else if (baseId == BASEID_LIST) sResult = tr("List");
    else if (baseId == BASEID_REMOVE) sResult = tr("Remove");
    else if (baseId == BASEID_RESIZE) sResult = tr("Resize");
    else if (baseId == BASEID_ANALYZE) sResult = tr("Analyze");
    else if (baseId == BASEID_CONDITIONAL) sResult = tr("Conditional");
    else if (baseId == BASEID_EDIT) sResult = tr("Edit");
    else if (baseId == BASEID_INSPECTOR) sResult = tr("Inspector");
    else if (baseId == BASEID_CONVERTOR) sResult = tr("Convertor");
    else if (baseId == BASEID_MULTISEARCH) sResult = tr("Multisearch");
    else if (baseId == BASEID_VISUALIZATION) sResult = tr("Visualization");
    else if (baseId == BASEID_0) sResult = QString("0");
    else if (baseId == BASEID_1) sResult = QString("1");
    else if (baseId == BASEID_2) sResult = QString("2");
    else if (baseId == BASEID_3) sResult = QString("3");
    else {
#ifdef QT_DEBUG
        qDebug("Unknown baseId");
#endif
    }

    return sResult;
}

quint64 XShortcuts::createShortcutsId(GROUPID groupId, const QList<GROUPID> &listSubgroup, BASEID baseId)
{
    quint64 nResult = 0;
    quint64 nSubgroups = 0;
    qint32 nNumberOfRecords = listSubgroup.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        nSubgroups |= (((quint64)1) << (listSubgroup.at(i)));
    }

    nResult = (((quint64)groupId) << (56)) | (((quint64)nSubgroups) << (8)) | ((quint64)baseId);

    //    if (baseId == BASEID_TOGGLE) {
    //        groupId = getGroupId(nResult);
    //    }

    return nResult;
}

XShortcuts::GROUPID XShortcuts::getGroupId(quint64 nShortcutId)
{
    GROUPID result = GROUPID_NONE;

    result = (GROUPID)(nShortcutId >> (56));

    return result;
}

QList<XShortcuts::GROUPID> XShortcuts::getSubgroupIds(quint64 nShortcutId)
{
    QList<GROUPID> listResult;

    quint64 nSubgroups = (nShortcutId >> 8) & (0xFFFFFFFFFFFF);

    for (qint64 i = 0; i < 48; i++) {
        if (nSubgroups & ((qint64)1 << i)) {
            GROUPID groupId = (GROUPID)i;

            listResult.append(groupId);
        }
    }

    return listResult;
}

quint64 XShortcuts::getParentId(quint64 nId)
{
    quint64 nResult = 0;

    BASEID baseId = getBaseId(nId);
    QList<GROUPID> listSubgroups = getSubgroupIds(nId);
    GROUPID groupId = getGroupId(nId);

    qint32 nNumberOfRecords = listSubgroups.count();

    if (baseId != BASEID_UNKNOWN) {
        nResult = nId & 0xFFFFFFFFFFFFFF00;
    } else {
        if (nNumberOfRecords) {
            nResult = ((nId & 0xFFFFFFFFFF0000) >> 8) | (((quint64)groupId & 0xFF) << 56);
        }
    }

    return nResult;
}

XShortcuts::GROUPID XShortcuts::getParentGroupId(quint64 nId)
{
    GROUPID result = GROUPID_NONE;

    BASEID baseId = getBaseId(nId);
    QList<GROUPID> listSubgroups = getSubgroupIds(nId);
    GROUPID groupId = getGroupId(nId);

    qint32 nNumberOfRecords = listSubgroups.count();

    if (baseId != BASEID_UNKNOWN) {
        if (nNumberOfRecords) {
            result = listSubgroups.at(nNumberOfRecords - 1);
        } else {
            result = groupId;
        }
    } else {
        if (nNumberOfRecords > 1) {
            result = listSubgroups.at(nNumberOfRecords - 2);
        } else {
            result = groupId;
        }
    }

    return result;
}

XShortcuts::BASEID XShortcuts::getBaseId(quint64 nShortcutId)
{
    BASEID result = BASEID_UNKNOWN;

    result = (BASEID)(nShortcutId & 0xFF);

    return result;
}

QString XShortcuts::baseIdToSettingsString(BASEID baseId)
{
    QString sResult;

    if (baseId == BASEID_COPY) sResult = QString("Copy");
    else if (baseId == BASEID_SHOW) sResult = QString("Show");
    else if (baseId == BASEID_OPEN) sResult = QString("Open");
    else if (baseId == BASEID_NEW) sResult = QString("New");
    else if (baseId == BASEID_SAVE) sResult = QString("Save");
    else if (baseId == BASEID_SAVEAS) sResult = QString("SaveAs");
    else if (baseId == BASEID_CLOSE) sResult = QString("Close");
    else if (baseId == BASEID_PRINT) sResult = QString("Print");
    else if (baseId == BASEID_EXIT) sResult = QString("Exit");
    else if (baseId == BASEID_DUMPTOFILE) sResult = QString("DumpToFile");
    else if (baseId == BASEID_ADDRESS) sResult = QString("Address");
    else if (baseId == BASEID_END) sResult = QString("End");
    else if (baseId == BASEID_START) sResult = QString("Start");
    else if (baseId == BASEID_ENTRYPOINT) sResult = QString("EntryPoint");
    else if (baseId == BASEID_XREF) sResult = QString("XREF");
    else if (baseId == BASEID_OFFSET) sResult = QString("Offset");
    else if (baseId == BASEID_SIZE) sResult = QString("Size");
    else if (baseId == BASEID_STRING) sResult = QString("String");
    else if (baseId == BASEID_STRINGS) sResult = QString("Strings");
    else if (baseId == BASEID_SCRIPTS) sResult = QString("Scripts");
    else if (baseId == BASEID_SIGNATURE) sResult = QString("Signature");
    else if (baseId == BASEID_SIGNATURES) sResult = QString("Signatures");
    else if (baseId == BASEID_HEX) sResult = QString("Hex");
    else if (baseId == BASEID_PATCH) sResult = QString("Patch");
    else if (baseId == BASEID_OPCODE) sResult = QString("Opcode");
    else if (baseId == BASEID_DEMANGLE) sResult = QString("Demangle");
    else if (baseId == BASEID_NAME) sResult = QString("Name");
    else if (baseId == BASEID_NEXT) sResult = QString("Next");
    else if (baseId == BASEID_DATA) sResult = QString("Data");
    else if (baseId == BASEID_VALUE) sResult = QString("Value");
    else if (baseId == BASEID_ALL) sResult = QString("All");
    else if (baseId == BASEID_DISASM) sResult = QString("Disasm");
    else if (baseId == BASEID_MEMORYMAP) sResult = QString("MemoryMap");
    else if (baseId == BASEID_ATTACH) sResult = QString("Attach");
    else if (baseId == BASEID_DETACH) sResult = QString("Detach");
    else if (baseId == BASEID_CPU) sResult = QString("CPU");
    else if (baseId == BASEID_LOG) sResult = QString("Log");
    else if (baseId == BASEID_BREAKPOINTS) sResult = QString("Breakpoints");
    else if (baseId == BASEID_CALLSTACK) sResult = QString("Callstack");
    else if (baseId == BASEID_THREADS) sResult = QString("Threads");
    else if (baseId == BASEID_HANDLES) sResult = QString("Handles");
    else if (baseId == BASEID_MODULES) sResult = QString("Modules");
    else if (baseId == BASEID_SYMBOLS) sResult = QString("Symbols");
    else if (baseId == BASEID_FUNCTIONS) sResult = QString("Functions");
    else if (baseId == BASEID_CLEAR) sResult = QString("Clear");
    else if (baseId == BASEID_SHORTCUTS) sResult = QString("Shortcuts");
    else if (baseId == BASEID_OPTIONS) sResult = QString("Options");
    else if (baseId == BASEID_ABOUT) sResult = QString("About");
    else if (baseId == BASEID_FILENAME) sResult = QString("FileName");
    else if (baseId == BASEID_STRUCTS) sResult = QString("Structs");
    else if (baseId == BASEID_VIEWER) sResult = QString("Viewer");
    else if (baseId == BASEID_FOLDER) sResult = QString("Folder");
    else if (baseId == BASEID_PID) sResult = QString("PID");
    else if (baseId == BASEID_RUN) sResult = QString("Run");
    else if (baseId == BASEID_PAUSE) sResult = QString("Pause");
    else if (baseId == BASEID_STEPINTO) sResult = QString("StepInto");
    else if (baseId == BASEID_STEPOVER) sResult = QString("StepOver");
    else if (baseId == BASEID_STOP) sResult = QString("Stop");
    else if (baseId == BASEID_RESTART) sResult = QString("Restart");
    else if (baseId == BASEID_TOGGLE) sResult = QString("Toggle");
    else if (baseId == BASEID_SCAN) sResult = QString("Scan");
    else if (baseId == BASEID_ENTROPY) sResult = QString("Entropy");
    else if (baseId == BASEID_HASH) sResult = QString("Hash");
    else if (baseId == BASEID_STACK) sResult = QString("Stack");
    else if (baseId == BASEID_FULLSCREEN) sResult = QString("FullScreen");
    else if (baseId == BASEID_REFERENCES) sResult = QString("References");
    else if (baseId == BASEID_BOOKMARK) sResult = QString("Bookmark");
    else if (baseId == BASEID_LIST) sResult = QString("List");
    else if (baseId == BASEID_REMOVE) sResult = QString("Remove");
    else if (baseId == BASEID_RESIZE) sResult = QString("Resize");
    else if (baseId == BASEID_ANALYZE) sResult = QString("Analyze");
    else if (baseId == BASEID_CONDITIONAL) sResult = QString("Conditional");
    else if (baseId == BASEID_EDIT) sResult = QString("Edit");
    else if (baseId == BASEID_INSPECTOR) sResult = QString("Inspector");
    else if (baseId == BASEID_CONVERTOR) sResult = QString("Convertor");
    else if (baseId == BASEID_MULTISEARCH) sResult = QString("Multisearch");
    else if (baseId == BASEID_VISUALIZATION) sResult = QString("Visualization");
    else if (baseId == BASEID_0) sResult = QString("0");
    else if (baseId == BASEID_1) sResult = QString("1");
    else if (baseId == BASEID_2) sResult = QString("2");
    else if (baseId == BASEID_3) sResult = QString("3");
    else {
#ifdef QT_DEBUG
        qDebug("Unknown baseId");
#endif
    }

    return sResult;
}

QString XShortcuts::groupIdToSettingsString(GROUPID groupId)
{
    QString sResult;

    if (groupId == GROUPID_ACTION) sResult = QString("Action");
    else if (groupId == GROUPID_FILE) sResult = QString("File");
    else if (groupId == GROUPID_VIEW) sResult = QString("View");
    else if (groupId == GROUPID_STRING) sResult = QString("String");
    else if (groupId == GROUPID_STRINGS) sResult = QString("Strings");
    else if (groupId == GROUPID_SIGNATURE) sResult = QString("Signature");
    else if (groupId == GROUPID_SIGNATURES) sResult = QString("Signatures");
    else if (groupId == GROUPID_STRUCT) sResult = QString("Struct");
    else if (groupId == GROUPID_HEX) sResult = QString("Hex");
    else if (groupId == GROUPID_DATA) sResult = QString("Data");
    else if (groupId == GROUPID_DISASM) sResult = QString("Disasm");
    else if (groupId == GROUPID_DEBUG) sResult = QString("Debug");
    else if (groupId == GROUPID_TRACE) sResult = QString("Trace");
    else if (groupId == GROUPID_ANIMATE) sResult = QString("Animate");
    else if (groupId == GROUPID_DEBUGGER) sResult = QString("Debugger");
    else if (groupId == GROUPID_REGISTERS) sResult = QString("Registers");
    else if (groupId == GROUPID_REGISTER) sResult = QString("Register");
    else if (groupId == GROUPID_STACK) sResult = QString("Stack");
    else if (groupId == GROUPID_ARCHIVE) sResult = QString("Archive");
    else if (groupId == GROUPID_TABLE) sResult = QString("Table");
    else if (groupId == GROUPID_PROCESS) sResult = QString("Process");
    else if (groupId == GROUPID_MEMORY) sResult = QString("Memory");
    else if (groupId == GROUPID_COPY) sResult = QString("Copy");
    else if (groupId == GROUPID_EDIT) sResult = QString("Edit");
    else if (groupId == GROUPID_FIND) sResult = QString("Find");
    else if (groupId == GROUPID_GOTO) sResult = QString("GoTo");
    else if (groupId == GROUPID_TOOLS) sResult = QString("Tools");
    else if (groupId == GROUPID_HELP) sResult = QString("Help");
    else if (groupId == GROUPID_SELECT) sResult = QString("Select");
    else if (groupId == GROUPID_SELECTION) sResult = QString("Selection");
    else if (groupId == GROUPID_FOLLOWIN) sResult = QString("FollowIn");
    else if (groupId == GROUPID_SHOWIN) sResult = QString("ShowIn");
    else if (groupId == GROUPID_BREAKPOINT) sResult = QString("Breakpoint");
    else if (groupId == GROUPID_MODULES) sResult = QString("Modules");
    else if (groupId == GROUPID_MEMORYMAP) sResult = QString("MemoryMap");
    else if (groupId == GROUPID_VALUE) sResult = QString("Value");
    else if (groupId == GROUPID_SCAN) sResult = QString("Scan");
    else if (groupId == GROUPID_EDITOR) sResult = QString("Editor");
    else if (groupId == GROUPID_BOOKMARKS) sResult = QString("Bookmarks");
    else if (groupId == GROUPID_ANALYZE) sResult = QString("Analyze");
    else if (groupId == GROUPID_HARDWARE) sResult = QString("Hardware");
    else if (groupId == GROUPID_LOCATION) sResult = QString("Location");
    else if (groupId == GROUPID_MODE) sResult = QString("Mode");
    else if (groupId == GROUPID_WIDTH) sResult = QString("Width");
    else if (groupId == GROUPID_BASE) sResult = QString("Base");

    return sResult;
}

void XShortcuts::adjustRowCopyMenu(QMenu *pParentMenu, QMenu *pMenu, QAbstractItemView *pTableView)
{
    m_listCopyActions.clear();

    adjustMenu(pParentMenu, pMenu, GROUPID_COPY);

    qint32 nRow = pTableView->currentIndex().row();

    if (nRow != -1) {
        QList<QString> listRecords;
        QList<QString> listTitles;

        QModelIndexList listSelected = pTableView->selectionModel()->selectedIndexes();

        qint32 nNumberOfSelected = listSelected.count();

        for (qint32 i = 0; i < nNumberOfSelected; i++) {
            QModelIndex index = pTableView->selectionModel()->selectedIndexes().at(i);
            QString sRecord = pTableView->model()->data(index).toString();
            QString sTitle = pTableView->model()->headerData(i, Qt::Horizontal).toString();

            listRecords.append(sRecord);
            listTitles.append(sTitle);
        }

        for (qint32 i = 0; i < nNumberOfSelected; i++) {
            QString sRecord = listRecords.at(i);
            QString sTitle = listTitles.at(i);

            if (!sTitle.isEmpty()) {
                QAction *pActionRecord = new QAction(pParentMenu);
                XOptions::adjustAction(pMenu, pActionRecord, sTitle, this, SLOT(copyRecord()), XOptions::ICONTYPE_COPY);
                pActionRecord->setProperty("VALUE", sRecord);

                m_listCopyActions.append(pActionRecord);
            }
        }

        pMenu->addSeparator();

        for (qint32 i = 0; i < nNumberOfSelected; i++) {
            QString sRecord = listRecords.at(i);

            if (!sRecord.isEmpty()) {
                // QAction *pActionRecord = new QAction(sRecord, pParent);
                QAction *pActionRecord = new QAction(pParentMenu);
                XOptions::adjustAction(pMenu, pActionRecord, sRecord, this, SLOT(copyRecord()), XOptions::ICONTYPE_COPY);
                pActionRecord->setProperty("VALUE", sRecord);

                m_listCopyActions.append(pActionRecord);
            }
        }
    }
}

void XShortcuts::adjustMenu(QMenu *pParentMenu, QMenu *pMenu, GROUPID groupId)
{
    QString sTitle = groupIdToString(groupId);
    XOptions::ICONTYPE iconType = getIconTypeByGroupId(groupId);

    XOptions::adjustMenu(pParentMenu, pMenu, sTitle, iconType);
}

void XShortcuts::adjustAction(QMenu *pParentMenu, QAction *pAction, quint64 nId, const QObject *pRecv, const char *pMethod, const QString &sText)
{
    pAction->setShortcut(getShortcut(nId));

    QString sTitle = baseIdToString(getBaseId(nId));

    if (sText != "") {
        if (sTitle != "") {
            sTitle += " ";
        }
        sTitle += sText;
    }

    XOptions::ICONTYPE iconType = getIconTypeById(nId);

    XOptions::adjustAction(pParentMenu, pAction, sTitle, pRecv, pMethod, iconType);
}

XOptions::ICONTYPE XShortcuts::getIconTypeById(quint64 nId)
{
    XOptions::ICONTYPE result = XOptions::ICONTYPE_NONE;

    BASEID baseId = getBaseId(nId);

    if (baseId == BASEID_COPY) result = XOptions::ICONTYPE_COPY;
    // else if (baseId == BASEID_SHOW) result = XOptions::ICONTYPE_SHOW;
    else if (baseId == BASEID_OPEN) result = XOptions::ICONTYPE_OPEN;
    else if (baseId == BASEID_NEW) result = XOptions::ICONTYPE_NEW;
    else if (baseId == BASEID_SAVE) result = XOptions::ICONTYPE_SAVE;
    // else if (baseId == BASEID_SAVEAS) result = XOptions::ICONTYPE_SAVEAS;
    // else if (baseId == BASEID_CLOSE) result = XOptions::ICONTYPE_CLOSE;
    // else if (baseId == BASEID_PRINT) result = XOptions::ICONTYPE_PRINT;
    else if (baseId == BASEID_EXIT) result = XOptions::ICONTYPE_EXIT;
    else if (baseId == BASEID_DUMPTOFILE) result = XOptions::ICONTYPE_DUMPTOFILE;
    else if (baseId == BASEID_ADDRESS) result = XOptions::ICONTYPE_ADDRESS;
    // else if (baseId == BASEID_END) result = XOptions::ICONTYPE_END;
    // else if (baseId == BASEID_START) result = XOptions::ICONTYPE_START;
    else if (baseId == BASEID_ENTRYPOINT) result = XOptions::ICONTYPE_ENTRY;
    // else if (baseId == BASEID_XREF) result = XOptions::ICONTYPE_XREF;
    else if (baseId == BASEID_OFFSET) result = XOptions::ICONTYPE_OFFSET;
    // else if (baseId == BASEID_SIZE) result = XOptions::ICONTYPE_SIZE;
    else if (baseId == BASEID_STRING) result = XOptions::ICONTYPE_STRING;
    else if (baseId == BASEID_STRINGS) result = XOptions::ICONTYPE_STRING;
    else if (baseId == BASEID_SCRIPTS) result = XOptions::ICONTYPE_SCRIPT;
    else if (baseId == BASEID_SIGNATURE) result = XOptions::ICONTYPE_SIGNATURE;
    else if (baseId == BASEID_SIGNATURES) result = XOptions::ICONTYPE_SIGNATURE;
    else if (baseId == BASEID_HEX) result = XOptions::ICONTYPE_HEX;
    else if (baseId == BASEID_PATCH) result = XOptions::ICONTYPE_PATCH;
    // else if (baseId == BASEID_OPCODE) result = XOptions::ICONTYPE_OPCODE;
    else if (baseId == BASEID_DEMANGLE) result = XOptions::ICONTYPE_DEMANGLE;
    // else if (baseId == BASEID_NAME) result = XOptions::ICONTYPE_NAME;
    else if (baseId == BASEID_NEXT) result = XOptions::ICONTYPE_NEXT;
    else if (baseId == BASEID_DATA) result = XOptions::ICONTYPE_DATA;
    else if (baseId == BASEID_VALUE) result = XOptions::ICONTYPE_VALUE;
    else if (baseId == BASEID_ALL) result = XOptions::ICONTYPE_ALL;
    else if (baseId == BASEID_DISASM) result = XOptions::ICONTYPE_DISASM;
    else if (baseId == BASEID_MEMORYMAP) result = XOptions::ICONTYPE_MEMORYMAP;
    // else if (baseId == BASEID_ATTACH) result = XOptions::ICONTYPE_ATTACH;
    // else if (baseId == BASEID_DETACH) result = XOptions::ICONTYPE_DETACH;
    // else if (baseId == BASEID_CPU) result = XOptions::ICONTYPE_CPU;
    // else if (baseId == BASEID_LOG) result = XOptions::ICONTYPE_LOG;
    // else if (baseId == BASEID_BREAKPOINTS) result = XOptions::ICONTYPE_BREAKPOINTS;
    // else if (baseId == BASEID_CALLSTACK) result = XOptions::ICONTYPE_CALLSTACK;
    // else if (baseId == BASEID_THREADS) result = XOptions::ICONTYPE_THREADS;
    // else if (baseId == BASEID_HANDLES) result = XOptions::ICONTYPE_HANDLES;
    // else if (baseId == BASEID_MODULES) result = XOptions::ICONTYPE_MODULES;
    else if (baseId == BASEID_SYMBOLS) result = XOptions::ICONTYPE_SYMBOL;
    else if (baseId == BASEID_FUNCTIONS) result = XOptions::ICONTYPE_FUNCTION;
    // else if (baseId == BASEID_CLEAR) result = XOptions::ICONTYPE_CLEAR;
    else if (baseId == BASEID_SHORTCUTS) result = XOptions::ICONTYPE_SHORTCUT;
    else if (baseId == BASEID_OPTIONS) result = XOptions::ICONTYPE_OPTION;
    else if (baseId == BASEID_ABOUT) result = XOptions::ICONTYPE_INFO;
    else if (baseId == BASEID_FILENAME) result = XOptions::ICONTYPE_FILE;
    else if (baseId == BASEID_STRUCTS) result = XOptions::ICONTYPE_STRUCTS;
    // else if (baseId == BASEID_VIEWER) result = XOptions::ICONTYPE_VIEWER;
    // else if (baseId == BASEID_FOLDER) result = XOptions::ICONTYPE_FOLDER;
    // else if (baseId == BASEID_PID) result = XOptions::ICONTYPE_PID;
    // else if (baseId == BASEID_RUN) result = XOptions::ICONTYPE_RUN;
    // else if (baseId == BASEID_PAUSE) result = XOptions::ICONTYPE_PAUSE;
    // else if (baseId == BASEID_STEPINTO) result = XOptions::ICONTYPE_STEPINTO;
    // else if (baseId == BASEID_STEPOVER) result = XOptions::ICONTYPE_STEPOVER;
    // else if (baseId == BASEID_STOP) result = XOptions::ICONTYPE_STOP;
    // else if (baseId == BASEID_RESTART) result = XOptions::ICONTYPE_RESTART;
    // else if (baseId == BASEID_TOGGLE) result = XOptions::ICONTYPE_TOGGLE;
    else if (baseId == BASEID_SCAN) result = XOptions::ICONTYPE_SCAN;
    else if (baseId == BASEID_ENTROPY) result = XOptions::ICONTYPE_ENTROPY;
    else if (baseId == BASEID_HASH) result = XOptions::ICONTYPE_HASH;
    // else if (baseId == BASEID_STACK) result = XOptions::ICONTYPE_STACK;
    // else if (baseId == BASEID_FULLSCREEN) result = XOptions::ICONTYPE_FULLSCREEN;
    else if (baseId == BASEID_REFERENCES) result = XOptions::ICONTYPE_REFERENCE;
    else if (baseId == BASEID_BOOKMARK) result = XOptions::ICONTYPE_BOOKMARK;
    else if (baseId == BASEID_LIST) result = XOptions::ICONTYPE_LIST;
    else if (baseId == BASEID_REMOVE) result = XOptions::ICONTYPE_REMOVE;
    else if (baseId == BASEID_RESIZE) result = XOptions::ICONTYPE_RESIZE;
    // else if (baseId == BASEID_ANALYZE) result = XOptions::ICONTYPE_ANALYZE;
    // else if (baseId == BASEID_CONDITIONAL) result = XOptions::ICONTYPE_CONDITIONAL;
    else if (baseId == BASEID_EDIT) result = XOptions::ICONTYPE_EDIT;
    else if (baseId == BASEID_INSPECTOR) result = XOptions::ICONTYPE_INSPECTOR;
    else if (baseId == BASEID_CONVERTOR) result = XOptions::ICONTYPE_CONVERTOR;
    // else if (baseId == BASEID_DATACONVERTOR) result = XOptions::ICONTYPE_DATACONVERTOR;
    else if (baseId == BASEID_MULTISEARCH) result = XOptions::ICONTYPE_SEARCH;
    else if (baseId == BASEID_VISUALIZATION) result = XOptions::ICONTYPE_VISUALIZATION;
    // else if (baseId == BASEID_0) result = XOptions::ICONTYPE_0;
    // else if (baseId == BASEID_1) result = XOptions::ICONTYPE_1;
    // else if (baseId == BASEID_2) result = XOptions::ICONTYPE_2;
    // else if (baseId == BASEID_3) result = XOptions::ICONTYPE_3;
    else {
        result = XOptions::ICONTYPE_NONE;
    }

    return result;
}

XOptions::ICONTYPE XShortcuts::getIconTypeByGroupId(GROUPID groupId)
{
    XOptions::ICONTYPE result = XOptions::ICONTYPE_NONE;

    if (groupId == GROUPID_ACTION) result = XOptions::ICONTYPE_ACTION;
    else if (groupId == GROUPID_FILE) result = XOptions::ICONTYPE_FILE;
    // else if (groupId == GROUPID_VIEW) result = XOptions::ICONTYPE_VIEW;
    else if (groupId == GROUPID_STRING) result = XOptions::ICONTYPE_STRING;
    else if (groupId == GROUPID_STRINGS) result = XOptions::ICONTYPE_STRING;
    else if (groupId == GROUPID_SIGNATURE) result = XOptions::ICONTYPE_SIGNATURE;
    else if (groupId == GROUPID_SIGNATURES) result = XOptions::ICONTYPE_SIGNATURE;
    // else if (groupId == GROUPID_STRUCT) result = XOptions::ICONTYPE_STRUCT;
    else if (groupId == GROUPID_HEX) result = XOptions::ICONTYPE_HEX;
    else if (groupId == GROUPID_DATA) result = XOptions::ICONTYPE_DATA;
    else if (groupId == GROUPID_DISASM) result = XOptions::ICONTYPE_DISASM;
    // else if (groupId == GROUPID_DEBUG) result = XOptions::ICONTYPE_DEBUG;
    // else if (groupId == GROUPID_TRACE) result = XOptions::ICONTYPE_TRACE;
    // else if (groupId == GROUPID_ANIMATE) result = XOptions::ICONTYPE_ANIMATE;
    // else if (groupId == GROUPID_DEBUGGER) result = XOptions::ICONTYPE_DEBUGGER;
    // else if (groupId == GROUPID_REGISTERS) result = XOptions::ICONTYPE_REGISTERS;
    // else if (groupId == GROUPID_REGISTER) result = XOptions::ICONTYPE_REGISTER;
    // else if (groupId == GROUPID_STACK) result = XOptions::ICONTYPE_STACK;
    // else if (groupId == GROUPID_ARCHIVE) result = XOptions::ICONTYPE_ARCHIVE;
    else if (groupId == GROUPID_TABLE) result = XOptions::ICONTYPE_TABLE;
    // else if (groupId == GROUPID_PROCESS) result = XOptions::ICONTYPE_PROCESS;
    // else if (groupId == GROUPID_MEMORY) result = XOptions::ICONTYPE_MEMORY;
    else if (groupId == GROUPID_COPY) result = XOptions::ICONTYPE_COPY;
    else if (groupId == GROUPID_EDIT) result = XOptions::ICONTYPE_EDIT;
    else if (groupId == GROUPID_FIND) result = XOptions::ICONTYPE_SEARCH;
    else if (groupId == GROUPID_GOTO) result = XOptions::ICONTYPE_GOTO;
    else if (groupId == GROUPID_TOOLS) result = XOptions::ICONTYPE_TOOL;
    else if (groupId == GROUPID_HELP) result = XOptions::ICONTYPE_INFO;
    else if (groupId == GROUPID_SELECT) result = XOptions::ICONTYPE_SELECT;
    else if (groupId == GROUPID_SELECTION) result = XOptions::ICONTYPE_SELECT;
    else if (groupId == GROUPID_FOLLOWIN) result = XOptions::ICONTYPE_FOLLOW;
    // else if (groupId == GROUPID_SHOWIN) result = XOptions::ICONTYPE_SHOWIN;
    // else if (groupId == GROUPID_BREAKPOINT) result = XOptions::ICONTYPE_BREAKPOINT;
    // else if (groupId == GROUPID_MODULES) result = XOptions::ICONTYPE_MODULES;
    else if (groupId == GROUPID_MEMORYMAP) result = XOptions::ICONTYPE_MEMORYMAP;
    else if (groupId == GROUPID_VALUE) result = XOptions::ICONTYPE_VALUE;
    else if (groupId == GROUPID_SCAN) result = XOptions::ICONTYPE_SCAN;
    else if (groupId == GROUPID_EDITOR) result = XOptions::ICONTYPE_EDIT;
    else if (groupId == GROUPID_BOOKMARKS) result = XOptions::ICONTYPE_BOOKMARK;
    // else if (groupId == GROUPID_ANALYZE) result = XOptions::ICONTYPE_ANALYZE;
    // else if (groupId == GROUPID_HARDWARE) result = XOptions::ICONTYPE_HARDWARE;

    return result;
}

void XShortcuts::createMainMenu(QWidget *pWidget, QMenuBar *pMenuBar, const QList<MENUITEM> &listMenuItems)
{
    Q_UNUSED(pWidget);
    Q_UNUSED(pMenuBar);
    Q_UNUSED(listMenuItems);
}

void XShortcuts::_addMenuItem(QList<MENUITEM> *pListMenuItems, quint64 nShortcutId, const QObject *pRecv, const char *pMethod, quint64 nSubgroups)
{
    MENUITEM record = {};

    record.nShortcutId = nShortcutId;
    record.pRecv = pRecv;
    record.pMethod = pMethod;
    record.nSubgroups = nSubgroups;

    pListMenuItems->append(record);
}

void XShortcuts::_addMenuItem_Text(QList<MENUITEM> *pListMenuItems, quint64 nShortcutId, const QObject *pRecv, const char *pMethod, quint64 nSubgroups,
                                   const QString &sText)
{
    MENUITEM record = {};

    record.nShortcutId = nShortcutId;
    record.pRecv = pRecv;
    record.pMethod = pMethod;
    record.nSubgroups = nSubgroups;
    record.sText = sText;

    pListMenuItems->append(record);
}

void XShortcuts::_addMenuItem_Checked(QList<MENUITEM> *pListMenuItems, quint64 nShortcutId, const QObject *pRecv, const char *pMethod, quint64 nSubgroups,
                                      bool bIsChecked)
{
    MENUITEM record = {};

    record.nShortcutId = nShortcutId;
    record.pRecv = pRecv;
    record.pMethod = pMethod;
    record.nSubgroups = nSubgroups;
    record.bIsCheckable = true;
    record.bIsChecked = bIsChecked;

    pListMenuItems->append(record);
}

void XShortcuts::_addMenuItem_CopyRow(QList<MENUITEM> *pListMenuItems, QAbstractItemView *pTableView)
{
    MENUITEM record = {};

    record.bCopyRow = true;
    record.pTableView = pTableView;

    pListMenuItems->append(record);
}

void XShortcuts::_addMenuSeparator(QList<MENUITEM> *pListMenuItems, quint64 nSubgroups)
{
    MENUITEM record = {};

    record.bIsMenuSeparator = true;
    record.nSubgroups = nSubgroups;

    pListMenuItems->append(record);
}

void XShortcuts::adjustContextMenu(QMenu *pMenu, const QList<MENUITEM> *plistMenuItems)
{
    QMap<quint64, QMenu *> mapMenus;
    mapMenus.insert(GROUPID_NONE, pMenu);

    qint32 nNumberOfRecords = plistMenuItems->count();

    for (qint32 j = 0; j < nNumberOfRecords; j++) {
        const MENUITEM &record = plistMenuItems->at(j);

        QMenu *pCurrentMenu = mapMenus.value(0);

        if (record.nSubgroups) {
            QMenu *pParentMenu = nullptr;

            for (qint32 i = 0; i < 8; i++) {
                pParentMenu = pCurrentMenu;

                GROUPID groupId = (GROUPID)(((record.nSubgroups) >> (8 * i)) & 0xFF);

                if (groupId == GROUPID_NONE) {
                    break;
                }

                quint64 nCurrentGroup = record.nSubgroups;

                for (int j = 7; j > i; j--) {
                    nCurrentGroup = nCurrentGroup & ~(((quint64)0xFF) << (8 * j));
                }

                pCurrentMenu = mapMenus.value(nCurrentGroup);

                if (!pCurrentMenu) {
                    pCurrentMenu = new QMenu(0);
                    mapMenus.insert(nCurrentGroup, pCurrentMenu);
                }

                adjustMenu(pParentMenu, pCurrentMenu, groupId);
            }
        }

        if (record.bCopyRow) {
            QMenu *pMenuCopy = new QMenu(pMenu);
            adjustMenu(pCurrentMenu, pMenuCopy, GROUPID_COPY);

            qint32 nRow = record.pTableView->currentIndex().row();

            if (nRow != -1) {
                QList<QString> listRecords;
                QList<QString> listTitles;

                QModelIndexList listSelected = record.pTableView->selectionModel()->selectedIndexes();

                qint32 nNumberOfSelected = listSelected.count();

                for (qint32 i = 0; i < nNumberOfSelected; i++) {
                    QModelIndex index = record.pTableView->selectionModel()->selectedIndexes().at(i);
                    QString sRecord = record.pTableView->model()->data(index).toString();
                    QString sTitle = record.pTableView->model()->headerData(i, Qt::Horizontal).toString();

                    listRecords.append(sRecord);
                    listTitles.append(sTitle);
                }

                for (qint32 i = 0; i < nNumberOfSelected; i++) {
                    QString sRecord = listRecords.at(i);
                    QString sTitle = listTitles.at(i);

                    if (!sTitle.isEmpty()) {
                        QAction *pActionRecord = new QAction(pMenu);
                        XOptions::adjustAction(pMenuCopy, pActionRecord, sTitle, this, SLOT(copyRecord()), XOptions::ICONTYPE_COPY);
                        pActionRecord->setProperty("VALUE", sRecord);
                    }
                }

                pMenuCopy->addSeparator();

                for (qint32 i = 0; i < nNumberOfSelected; i++) {
                    QString sRecord = listRecords.at(i);

                    if (!sRecord.isEmpty()) {
                        QAction *pActionRecord = new QAction(pMenu);

                        XOptions::adjustAction(pMenuCopy, pActionRecord, sRecord, this, SLOT(copyRecord()), XOptions::ICONTYPE_COPY);
                        pActionRecord->setProperty("VALUE", sRecord);
                    }
                }
            }
        } else if (record.bIsMenuSeparator) {
            if (pCurrentMenu) {
                pCurrentMenu->addSeparator();
            }
        } else {
            if (pCurrentMenu) {
                QAction *pAction = new QAction(pMenu);

                if (record.nShortcutId) {
                    adjustAction(pCurrentMenu, pAction, record.nShortcutId, record.pRecv, record.pMethod, record.sText);
                } else {
                    XOptions::adjustAction(pCurrentMenu, pAction, record.sText, record.pRecv, record.pMethod, record.iconType);
                }

                if (record.sPropertyName != "") {
                    pAction->setProperty(record.sPropertyName.toLatin1().data(), record.varProperty);
                }

                if (record.bIsCheckable) {
                    pAction->setCheckable(true);

                    if (record.bIsChecked) {
                        pAction->setChecked(true);
                    }
                }
            }
        }
    }
}

void XShortcuts::registerShortcuts(QList<SHORTCUTITEM> *pListShortcutItems, bool bState)
{
    qint32 nNumberOfRecords = pListShortcutItems->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (bState) {
            if (!pListShortcutItems->at(i).pShortCut) {
                QKeySequence keySequence = getShortcut(pListShortcutItems->at(i).nShortcutId);
                // qDebug("%d) %s", i, keySequence.toString(QKeySequence::NativeText).toLatin1().data());
                (*pListShortcutItems)[i].pShortCut = new QShortcut(keySequence, pListShortcutItems->at(i).pRecv, pListShortcutItems->at(i).pMethod);
            }
        } else {
            delete pListShortcutItems->at(i).pShortCut;
            (*pListShortcutItems)[i].pShortCut = nullptr;
        }
    }
}

void XShortcuts::copyRecord()
{
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        QString sString = pAction->property("VALUE").toString();

        QApplication::clipboard()->setText(sString);
    }
}
