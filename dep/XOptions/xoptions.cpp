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
#include "xoptions.h"

static const XOptions::CONSOLE_OPTION g_consoleOptions[] = {
    {XOptions::CONSOLE_OPTION_ID_RECURSIVESCAN, "r", "recursivescan", "Scan directories recursively"},
    {XOptions::CONSOLE_OPTION_ID_DEEPSCAN, "d", "deepscan", "Enable deep scanning for thorough analysis"},
    {XOptions::CONSOLE_OPTION_ID_HEURISTICSCAN, "u", "heuristicscan", "Enable heuristic scanning methods"},
    {XOptions::CONSOLE_OPTION_ID_VERBOSE, "b", "verbose", "Show verbose output with detailed information"},
    {XOptions::CONSOLE_OPTION_ID_AGGRESSIVESCAN, "g", "aggressivecscan", "Enable aggressive scanning mode"},
    {XOptions::CONSOLE_OPTION_ID_ALLTYPES, "a", "alltypes", "Scan all file types"},
    {XOptions::CONSOLE_OPTION_ID_FORMAT, "f", "format", "Format the output result"},
    {XOptions::CONSOLE_OPTION_ID_PROFILING, "l", "profiling", "Profile signatures during scan"},
    {XOptions::CONSOLE_OPTION_ID_MESSAGES, "M", "messages", "Display scan messages and warnings"},
    {XOptions::CONSOLE_OPTION_ID_HIDEUNKNOWN, "U", "hideunknown", "Hide unknown file types from results"},
    {XOptions::CONSOLE_OPTION_ID_ENTROPY, "e", "entropy", "Display file entropy information"},
    {XOptions::CONSOLE_OPTION_ID_INFO, "i", "info", "Display file information"},
    {XOptions::CONSOLE_OPTION_ID_XML, "x", "xml", "Output results in XML format"},
    {XOptions::CONSOLE_OPTION_ID_JSON, "j", "json", "Output results in JSON format"},
    {XOptions::CONSOLE_OPTION_ID_CSV, "c", "csv", "Output results in CSV format"},
    {XOptions::CONSOLE_OPTION_ID_TSV, "t", "tsv", "Output results in TSV format"},
    {XOptions::CONSOLE_OPTION_ID_PLAINTEXT, "p", "plaintext", "Output results as plain text"},
    {XOptions::CONSOLE_OPTION_ID_DATABASE, "D", "database", "Set database path"},
    {XOptions::CONSOLE_OPTION_ID_EXTRADATABASE, "E", "extradatabase", "Set extra database path"},
    {XOptions::CONSOLE_OPTION_ID_CUSTOMDATABASE, "C", "customdatabase", "Set custom database path"},
    {XOptions::CONSOLE_OPTION_ID_SHOWDATABASE, "s", "showdatabase", "Show database information"},
    {XOptions::CONSOLE_OPTION_ID_STRUCT, "S", "struct", "Show special file information using specified structure (e.g., 'Hash' or 'Hash#MD5')"},
    {XOptions::CONSOLE_OPTION_ID_SHOWSTRUCTS, "w", "showstructs", "Display all available special structures for the file"},
    {XOptions::CONSOLE_OPTION_ID_TEST, "", "test", "Test signatures in specified directory"},
    {XOptions::CONSOLE_OPTION_ID_ADDTEST, "", "addtest", "Add test case with filename, detect string, and directory"},
    {XOptions::CONSOLE_OPTION_ID_SORT, "", "sort", "Sort scan results"},
    {XOptions::CONSOLE_OPTION_ID_NOHIGHLIGHT, "", "nohighlight", "Do not highlight scan results"},
    {XOptions::CONSOLE_OPTION_ID_USECACHE, "", "usecache", "Use database cache for faster loading"},
    {XOptions::CONSOLE_OPTION_ID_OVERLAYSCAN, "O", "overlayscan", "Scan file overlay"},
    {XOptions::CONSOLE_OPTION_ID_RESOURCESSCAN, "R", "resourcesscan", "Scan file resources"},
    {XOptions::CONSOLE_OPTION_ID_ARCHIVESSCAN, "A", "archivesscan", "Scan file archives"},
    {XOptions::CONSOLE_OPTION_ID_FILETYPE, "F", "filetype", "Force file type (e.g. PE, ELF, DEX)"},
};

XOptions::XOptions(QObject *pParent) : QObject(pParent)
{
#ifdef QT_GUI_LIB
    m_pRecentFilesMenu = nullptr;
    m_pCodePagesMenu = nullptr;
#endif
    m_bIsNeedRestart = false;
    m_nMaxRecentFilesCount = N_MAX_RECENT_FILES_COUNT;
    m_sName = QString("%1.ini").arg(qApp->applicationName());  // default name
}

void XOptions::resetToDefault()
{
    const qint32 nCount = m_listValueIDs.count();
    if (!nCount) {
        return;
    }

    const QMap<ID, QVariant>::const_iterator itEnd = m_mapDefaultValues.constEnd();

    for (qint32 i = 0; i < nCount; ++i) {
        const ID id = m_listValueIDs.at(i);

        // Skip non-user (runtime) entries
        if ((id == ID_NU_LASTDIRECTORY) || (id == ID_NU_RECENTFILES)) {
            continue;
        }

        QVariant varDefault;  // invalid by default
        QMap<ID, QVariant>::const_iterator it = m_mapDefaultValues.constFind(id);
        if (it != itEnd) {
            varDefault = it.value();
        }

        // Avoid unnecessary insert if value is already equal
        if (m_mapValues.value(id) != varDefault) {
            m_mapValues.insert(id, varDefault);
        }
    }
}

void XOptions::setValueIDs(const QList<ID> &listVariantIDs)
{
    m_listValueIDs = listVariantIDs;
}

QList<XOptions::ID> XOptions::getValueIDs() const
{
    return m_listValueIDs;
}

void XOptions::setDefaultValues(QMap<XOptions::ID, QVariant> mapDefaultValues)
{
    m_mapDefaultValues = mapDefaultValues;
}

void XOptions::addID(ID id, QVariant varDefaultValue)
{
    m_listValueIDs.append(id);

    if (varDefaultValue.isValid()) {
        m_mapDefaultValues.insert(id, varDefaultValue);
    }
}

void XOptions::removeID(ID id)
{
    m_listValueIDs.removeOne(id);
    m_mapDefaultValues.remove(id);
    m_mapValues.remove(id);
}

XOptions::GROUPID XOptions::getGroupID(ID id)
{
    GROUPID result = GROUPID_UNKNOWN;

    switch (id) {
        case ID_VIEW_STAYONTOP:
        case ID_VIEW_STYLE:
        case ID_VIEW_QSS:
        case ID_VIEW_QSS_DATABASE_UPDATE_URL:
        case ID_VIEW_LANG:
        case ID_VIEW_SINGLEAPPLICATION:
        case ID_VIEW_SHOWLOGO:
        case ID_VIEW_FONT:
        case ID_VIEW_FONT_CONTROLS:
        case ID_VIEW_FONT_TABLEVIEWS:
        case ID_VIEW_FONT_TREEVIEWS:
        case ID_VIEW_FONT_TEXTEDITS:
        case ID_VIEW_ADVANCED:
        case ID_VIEW_SELECTSTYLE: result = GROUPID_VIEW; break;
        case ID_FILE_SAVELASTDIRECTORY:
        case ID_FILE_SAVERECENTFILES:
        case ID_FILE_SAVEBACKUP:
        case ID_FILE_CONTEXT:
        case ID_FILE_PATH: result = GROUPID_FILE; break;
        case ID_SCAN_SCANAFTEROPEN:
        case ID_SCAN_FLAG_RECURSIVE:
        case ID_SCAN_FLAG_OVERLAY:
        case ID_SCAN_FLAG_RESOURCES:
        case ID_SCAN_FLAG_ARCHIVES:
        case ID_SCAN_FLAG_DEEP:
        case ID_SCAN_FLAG_HEURISTIC:
        case ID_SCAN_FLAG_AGGRESSIVE:
        case ID_SCAN_FLAG_VERBOSE:
        case ID_SCAN_FLAG_ALLTYPES:
        case ID_SCAN_FORMATRESULT:
        case ID_SCAN_LOG_PROFILING:
        case ID_SCAN_HIGHLIGHT:
        case ID_SCAN_SORT:
        case ID_SCAN_HIDEUNKNOWN:
        case ID_SCAN_ENGINE:
        case ID_SCAN_ENGINE_EMPTY:
        case ID_SCAN_ENGINE_DIE_ENABLED:
        case ID_SCAN_ENGINE_NFD_ENABLED:
        case ID_SCAN_ENGINE_PEID_ENABLED:
        case ID_SCAN_ENGINE_YARA_ENABLED:
        case ID_SCAN_DIE_DATABASE_MAIN_PATH:
        case ID_SCAN_DIE_DATABASE_MAIN_UPDATE_URL:
        case ID_SCAN_DIE_DATABASE_EXTRA_PATH:
        case ID_SCAN_DIE_DATABASE_EXTRA_UPDATE_URL:
        case ID_SCAN_DIE_DATABASE_CUSTOM_PATH:
        case ID_SCAN_DIE_DATABASE_EXTRA_ENABLED:
        case ID_SCAN_DIE_DATABASE_CUSTOM_ENABLED:
        case ID_SCAN_YARA_DATABASE_PATH:
        case ID_SCAN_YARA_DATABASE_UPDATE_URL:
        case ID_SCAN_PEID_DATABASE_PATH:
        case ID_SCAN_PEID_DATABASE_UPDATE_URL:
        case ID_SCAN_DIRECTORY_PATH:
        case ID_SCAN_TARGET_ALL:
        case ID_SCAN_TARGET_ALLFILETYPES:
        case ID_SCAN_TARGET_ALLTYPES:
        case ID_SCAN_TARGET_FILETYPES:
        case ID_SCAN_TARGET_TYPES:
        case ID_SCAN_COLOR_INSTALLER:
        case ID_SCAN_COLOR_SFX:
        case ID_SCAN_COLOR_ARCHIVE:
        case ID_SCAN_COLOR_PROTECTION:
        case ID_SCAN_COLOR_PETOOL:
        case ID_SCAN_COLOR_APKTOOL:
        case ID_SCAN_COLOR_OS:
        case ID_SCAN_COLOR_VM:
        case ID_SCAN_COLOR_PLATFORM:
        case ID_SCAN_COLOR_DOSEXTENDER:
        case ID_SCAN_COLOR_FORMAT:
        case ID_SCAN_COLOR_SIGNTOOL:
        case ID_SCAN_COLOR_CERTIFICATE:
        case ID_SCAN_COLOR_LICENSING:
        case ID_SCAN_COLOR_LANGUAGE:
        case ID_SCAN_COLOR_CORRUPTEDDATA:
        case ID_SCAN_COLOR_PERSONALDATA:
        case ID_SCAN_COLOR_AUTHOR:
        case ID_SCAN_COLOR_VIRUS:
        case ID_SCAN_COLOR_TROJAN:
        case ID_SCAN_COLOR_MALWARE:
        case ID_SCAN_COLOR_DEBUG:
        case ID_SCAN_COLOR_DEBUGDATA:
        case ID_SCAN_COLOR_GAMEENGINE:
        case ID_SCAN_COLOR_COMPILER:
        case ID_SCAN_COLOR_COMPRESSOR:
        case ID_SCAN_COLOR_CONVERTER:
        case ID_SCAN_COLOR_CREATOR:
        case ID_SCAN_COLOR_DATA:
        case ID_SCAN_COLOR_DATABASE:
        case ID_SCAN_COLOR_DOCUMENT:
        case ID_SCAN_COLOR_GENERIC:
        case ID_SCAN_COLOR_IMAGE:
        case ID_SCAN_COLOR_INSTALLERDATA:
        case ID_SCAN_COLOR_LIBRARY:
        case ID_SCAN_COLOR_LINKER:
        case ID_SCAN_COLOR_LOADER:
        case ID_SCAN_COLOR_OBFUSCATOR:
        case ID_SCAN_COLOR_OVERLAY:
        case ID_SCAN_COLOR_PACKAGE:
        case ID_SCAN_COLOR_PLAYER:
        case ID_SCAN_COLOR_PRODUCER:
        case ID_SCAN_COLOR_PROTECTORDATA:
        case ID_SCAN_COLOR_ROM:
        case ID_SCAN_COLOR_SFXDATA:
        case ID_SCAN_COLOR_SOURCECODE:
        case ID_SCAN_COLOR_STUB:
        case ID_SCAN_COLOR_TOOL: result = GROUPID_SCAN; break;
        case ID_SIGNATURES_PATH: result = GROUPID_SIGNATURES; break;
        case ID_INFO_DATABASE_PATH:
        case ID_INFO_DATABASE_UPDATE_URL: result = GROUPID_INFO; break;
        case ID_DISASM_FONT:
        case ID_DISASM_SYNTAX:
        case ID_DISASM_LOCATIONCOLON:
        case ID_DISASM_UPPERCASE:
        case ID_DISASM_HIGHLIGHT:
        case ID_DISASM_COLOR_ARROWS:
        case ID_DISASM_COLOR_ARROWS_SELECTED:
        case ID_DISASM_COLOR_REGS:
        case ID_DISASM_COLOR_NUMBERS:
        case ID_DISASM_COLOR_OPCODE:
        case ID_DISASM_COLOR_REFS:
        case ID_DISASM_COLOR_X86_REGS_GENERAL:
        case ID_DISASM_COLOR_X86_REGS_STACK:
        case ID_DISASM_COLOR_X86_REGS_SEGMENT:
        case ID_DISASM_COLOR_X86_REGS_DEBUG:
        case ID_DISASM_COLOR_X86_REGS_IP:
        case ID_DISASM_COLOR_X86_REGS_FLAGS:
        case ID_DISASM_COLOR_X86_REGS_FPU:
        case ID_DISASM_COLOR_X86_REGS_XMM:
        case ID_DISASM_COLOR_X86_REGS_YMM:
        case ID_DISASM_COLOR_X86_REGS_ZMM:
        case ID_DISASM_COLOR_X86_OPCODE_CALL:
        case ID_DISASM_COLOR_X86_OPCODE_RET:
        case ID_DISASM_COLOR_X86_OPCODE_COND_JMP:
        case ID_DISASM_COLOR_X86_OPCODE_PUSH:
        case ID_DISASM_COLOR_X86_OPCODE_POP:
        case ID_DISASM_COLOR_X86_OPCODE_NOP:
        case ID_DISASM_COLOR_X86_OPCODE_JMP:
        case ID_DISASM_COLOR_X86_OPCODE_INT3:
        case ID_DISASM_COLOR_X86_OPCODE_SYSCALL:
        case ID_DISASM_COLOR_ARM_REGS_GENERAL:
        case ID_DISASM_COLOR_ARM_REGS_STACK:
        case ID_DISASM_COLOR_ARM_OPCODE_BRANCH:
        case ID_DISASM_COLOR_ARM_OPCODE_BRANCHLINK:
        case ID_DISASM_COLOR_ARM_OPCODE_RET:
        case ID_DISASM_COLOR_ARM_OPCODE_PUSH:
        case ID_DISASM_COLOR_ARM_OPCODE_POP:
        case ID_DISASM_COLOR_ARM_OPCODE_NOP: result = GROUPID_DISASM; break;
        case ID_DEBUGGER_COLOR_BREAKPOINT: result = GROUPID_DEBUGGER; break;
        case ID_HEX_FONT:
        case ID_HEX_LOCATIONCOLON: result = GROUPID_HEX; break;
        case ID_STACK_FONT:
        case ID_STACK_LOCATIONCOLON: result = GROUPID_STACK; break;
        case ID_REGISTERS_FONT: result = GROUPID_REGISTERS; break;
        case ID_IODRIVER_FILENAME:
        case ID_IODRIVER_SERVICENAME: result = GROUPID_IODRIVER; break;
        default:
#ifdef QT_DEBUG
            qDebug("Unknown GroupID");
#endif
            result = GROUPID_UNKNOWN;
    }

    return result;
}

bool XOptions::isIDPresent(ID id)
{
    bool bResult = false;

    qint32 nNumberOfRecords = m_listValueIDs.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (m_listValueIDs.at(i) == id) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

bool XOptions::isGroupIDPresent(GROUPID groupID)
{
    bool bResult = false;

    qint32 nNumberOfRecords = m_listValueIDs.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (getGroupID(m_listValueIDs.at(i)) == groupID) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

bool XOptions::isNative()
{
#ifdef X_BUILD_INSTALL
    return true;
#else
    return !isPortable();
#endif
}

bool XOptions::isPortable()
{
#ifdef QT_DEBUG
    return true;
#endif

    QString sApplicationDirPath = QDir::cleanPath(qApp->applicationDirPath());
    QString sPortableFileName = sApplicationDirPath + QDir::separator() + "portable";
    return QFileInfo(sPortableFileName).exists();
}

bool XOptions::isAppImage()
{
#ifdef X_BUILD_APPIMAGE
    return true;
#else
    return false;
#endif
}

void XOptions::setName(const QString &sValue)
{
    m_sName = sValue;
#ifdef QT_DEBUG
#ifdef Q_OS_WIN
#ifndef Q_OS_WIN64
    m_sName += ".win32.debug.ini";
#else
    m_sName += ".win64.debug.ini";
#endif
#endif
#ifdef Q_OS_LINUX
    m_sName += ".linux.debug.ini";
#endif
#ifdef Q_OS_MACOS
    m_sName += ".macos.debug.ini";
#endif
#endif
}

void XOptions::load()
{
    QSettings *pSettings = nullptr;

    bool bIsNative = isNative();

    if (bIsNative) {
        pSettings = new QSettings;
    } else {
        pSettings = new QSettings(qApp->applicationDirPath() + QDir::separator() + QString("%1").arg(m_sName), QSettings::IniFormat);
    }

#ifdef QT_DEBUG
    if (pSettings) {
        qDebug("XOptions::load(): Loaded file from %s", pSettings->fileName().toUtf8().data());
    }
#endif

    qint32 nNumberOfIDs = m_listValueIDs.count();

    bool bSaveLastDirectory = false;
    bool bLastDirectory = false;
    bool bSaveRecentFiles = false;
    bool bRecentFiles = false;

    for (qint32 i = 0; i < nNumberOfIDs; i++) {
        if (m_listValueIDs.at(i) == ID_FILE_SAVELASTDIRECTORY) {
            bSaveLastDirectory = true;
        } else if (m_listValueIDs.at(i) == ID_NU_LASTDIRECTORY) {
            bLastDirectory = true;
        } else if (m_listValueIDs.at(i) == ID_FILE_SAVERECENTFILES) {
            bSaveRecentFiles = true;
        } else if (m_listValueIDs.at(i) == ID_NU_RECENTFILES) {
            bRecentFiles = true;
        }
    }

    if (bSaveLastDirectory && (!bLastDirectory)) {
        m_listValueIDs.append(ID_NU_LASTDIRECTORY);
    }

    if (bSaveRecentFiles && (!bRecentFiles)) {
        m_listValueIDs.append(ID_NU_RECENTFILES);
    }

    nNumberOfIDs = m_listValueIDs.count();

    for (qint32 i = 0; i < nNumberOfIDs; i++) {
        ID id = m_listValueIDs.at(i);
        QString sName = idToString(id);

        QVariant varDefault;

        if (m_mapDefaultValues.contains(id)) {
            varDefault = m_mapDefaultValues.value(id);
        } else {
            // TODO: remove,use addID
            switch (id) {
                case ID_ROOTPATH: varDefault = ""; break;
                case ID_DATAPATH: varDefault = "$data/data"; break;
                case ID_JSON: varDefault = ""; break;
                case ID_STRUCTSPATH: varDefault = "$data/structs"; break;
                case ID_STRUCTS_PATH: varDefault = "$data/structs"; break;
                case ID_AUTHUSER: varDefault = ""; break;
                case ID_AUTHTOKEN: varDefault = ""; break;
                case ID_NU_RECENTFILES: varDefault = QList<QVariant>(); break;
                case ID_NU_LASTDIRECTORY: varDefault = ""; break;
                case ID_FEATURE_READBUFFERSIZE: varDefault = 4 * 1024; break;
                case ID_FEATURE_FILEBUFFERSIZE: varDefault = 64 * 1024 * 1024; break;
                default: varDefault = "";
            }
        }

        QVariant variant = pSettings->value(sName, varDefault);

        if (!variant.toString().contains("$data")) {
            if ((id == ID_SCAN_DIE_DATABASE_MAIN_PATH) || (id == ID_SCAN_DIE_DATABASE_EXTRA_PATH) || (id == ID_SCAN_DIE_DATABASE_CUSTOM_PATH) ||
                (id == ID_SCAN_YARA_DATABASE_PATH) || (id == ID_SCAN_PEID_DATABASE_PATH)) {
                if ((!QDir(variant.toString()).exists()) && (!QFile(variant.toString()).exists())) {
                    variant = varDefault;
                }
            } else if ((id == ID_DATAPATH) || (id == ID_SIGNATURES_PATH) || (id == ID_STRUCTS_PATH) || (id == ID_STRUCTSPATH) || (id == ID_INFO_DATABASE_PATH) ||
                       (id == ID_SCAN_DIRECTORY_PATH)) {
                if (!QDir(variant.toString()).exists()) {
                    variant = varDefault;
                }
            }
        }

        m_mapValues.insert(id, variant);
    }

    QString sLastDirectory = m_mapValues.value(ID_NU_LASTDIRECTORY).toString();

    if (sLastDirectory != "") {
        if (!QDir(sLastDirectory).exists()) {
            m_mapValues.insert(ID_NU_LASTDIRECTORY, "");
        }
    }

#ifdef USE_XSIMD
    if (xsimd_is_sse2_present()) {
        xsimd_set_sse2(m_mapValues.value(ID_FEATURE_SSE2).toBool());
    }
    if (xsimd_is_avx2_present()) {
        xsimd_set_avx2(m_mapValues.value(ID_FEATURE_AVX2).toBool());
    }
#endif

    delete pSettings;
}

void XOptions::save()
{
    QSettings *pSettings = nullptr;

    bool bIsNative = isNative();

    if (bIsNative) {
        pSettings = new QSettings;
    } else {
        pSettings = new QSettings(qApp->applicationDirPath() + QDir::separator() + QString("%1").arg(m_sName), QSettings::IniFormat);
    }

#ifdef QT_DEBUG
    if (pSettings) {
        qDebug("XOptions::save(): Saved file to %s", pSettings->fileName().toUtf8().data());
    }
#endif

    qint32 nNumberOfIDs = m_listValueIDs.count();

    for (qint32 i = 0; i < nNumberOfIDs; i++) {
        ID id = m_listValueIDs.at(i);
        QString sName = idToString(id);
        pSettings->setValue(sName, m_mapValues.value(id));

        if ((id == ID_FILE_SAVELASTDIRECTORY) && (m_mapValues.value(id).toBool() == false)) {
            pSettings->setValue(idToString(ID_NU_LASTDIRECTORY), "");
        } else if ((id == ID_FILE_SAVERECENTFILES) && (m_mapValues.value(id).toBool() == false)) {
            clearRecentFiles();
        }
    }

    delete pSettings;
}

QVariant XOptions::getValue(XOptions::ID id)
{
    return m_mapValues.value(id);
}

void XOptions::setValue(XOptions::ID id, QVariant varValue)
{
    if ((id == ID_VIEW_STYLE) || (id == ID_VIEW_LANG) || (id == ID_VIEW_QSS)) {
        QVariant varOld = m_mapValues.value(id);

        if (varValue != varOld) {
            m_bIsNeedRestart = true;
        }
    }

    m_mapValues.insert(id, varValue);
}

void XOptions::clearValue(XOptions::ID id)
{
    m_mapValues.insert(id, "");
}

bool XOptions::isValuePresent(ID id)
{
    return m_mapValues.contains(id);
}

QVariant XOptions::getDefaultValue(ID id)
{
    return m_mapDefaultValues.value(id);
}

QCommandLineOption XOptions::getCommandLineOption(CONSOLE_OPTION_ID nId)
{
    if ((nId > CONSOLE_OPTION_ID_UNKNOWN) && (nId <= CONSOLE_OPTION_ID_FILETYPE)) {
        const CONSOLE_OPTION *pOption = &g_consoleOptions[nId - 1];

        QStringList listOptions;

        if (pOption->pszShort[0] != '\0') {
            listOptions << pOption->pszShort;
        }

        listOptions << pOption->pszLong;

        if ((nId == CONSOLE_OPTION_ID_DATABASE) || (nId == CONSOLE_OPTION_ID_EXTRADATABASE) || (nId == CONSOLE_OPTION_ID_CUSTOMDATABASE)) {
            return QCommandLineOption(listOptions, pOption->pszDescription, "path");
        } else if (nId == CONSOLE_OPTION_ID_STRUCT) {
            return QCommandLineOption(listOptions, pOption->pszDescription, "struct");
        } else if (nId == CONSOLE_OPTION_ID_TEST) {
            return QCommandLineOption(listOptions, pOption->pszDescription, "directory");
        } else if (nId == CONSOLE_OPTION_ID_ADDTEST) {
            return QCommandLineOption(listOptions, pOption->pszDescription, "filename", "");
        } else if (nId == CONSOLE_OPTION_ID_FILETYPE) {
            return QCommandLineOption(listOptions, pOption->pszDescription, "filetype");
        } else {
            return QCommandLineOption(listOptions, pOption->pszDescription);
        }
    }

    return QCommandLineOption(QStringList() << "error", "Invalid option ID");
}

QString XOptions::idToString(ID id)
{
    QString sResult;

    switch (id) {
        // TODO: remove
        case ID_ROOTPATH: sResult = QString("RootPath"); break;
        case ID_DATAPATH: sResult = QString("DataPath"); break;
        case ID_JSON: sResult = QString("Json"); break;
        case ID_STRUCTSPATH: sResult = QString("StructsPath"); break;
        case ID_AUTHUSER: sResult = QString("AuthUser"); break;
        case ID_AUTHTOKEN: sResult = QString("AuthToken"); break;
        // new
        case ID_VIEW_STAYONTOP: sResult = QString("View/StayOnTop"); break;
        case ID_VIEW_STYLE: sResult = QString("View/Style"); break;
        case ID_VIEW_QSS: sResult = QString("View/Qss"); break;
        case ID_VIEW_QSS_DATABASE_UPDATE_URL: sResult = QString("View/Qss/Database/UpdateUrl"); break;
        case ID_VIEW_LANG: sResult = QString("View/Lang"); break;
        case ID_VIEW_SINGLEAPPLICATION: sResult = QString("View/SingleApplication"); break;
        case ID_VIEW_SHOWLOGO: sResult = QString("View/ShowLogo"); break;
        case ID_VIEW_FONT: sResult = QString("View/Font"); break;
        case ID_VIEW_FONT_CONTROLS: sResult = QString("View/Font/Controls"); break;
        case ID_VIEW_FONT_TABLEVIEWS: sResult = QString("View/Font/TableViews"); break;
        case ID_VIEW_FONT_TREEVIEWS: sResult = QString("View/Font/TreeViews"); break;
        case ID_VIEW_FONT_TEXTEDITS: sResult = QString("View/Font/TextEdits"); break;
        case ID_VIEW_ADVANCED: sResult = QString("View/Advanced"); break;
        case ID_VIEW_SELECTSTYLE: sResult = QString("View/SelectStyle"); break;
        case ID_FILE_SAVELASTDIRECTORY: sResult = QString("File/SaveLastDirectory"); break;
        case ID_FILE_SAVERECENTFILES: sResult = QString("File/SaveRecentFiles"); break;
        case ID_FILE_SAVEBACKUP: sResult = QString("File/SaveBackup"); break;
        case ID_FILE_CONTEXT: sResult = QString("File/Context"); break;
        case ID_FILE_PATH: sResult = QString("File/Path"); break;
        case ID_FEATURE_READBUFFERSIZE: sResult = QString("Feature/ReadBufferSize"); break;
        case ID_FEATURE_FILEBUFFERSIZE: sResult = QString("Feature/FileBufferSize"); break;
        case ID_FEATURE_SSE2: sResult = QString("Feature/SSE2"); break;
        case ID_FEATURE_AVX2: sResult = QString("Feature/AVX2"); break;
        case ID_SCAN_SCANAFTEROPEN: sResult = QString("Scan/ScanAfterOpen"); break;
        case ID_SCAN_FLAG_RECURSIVE: sResult = QString("Scan/Flag/Recursive"); break;
        case ID_SCAN_FLAG_RESOURCES: sResult = QString("Scan/Flag/Resources"); break;
        case ID_SCAN_FLAG_ARCHIVES: sResult = QString("Scan/Flag/Archives"); break;
        case ID_SCAN_FLAG_OVERLAY: sResult = QString("Scan/Flag/Overlay"); break;
        case ID_SCAN_FLAG_DEEP: sResult = QString("Scan/Flag/Deep"); break;
        case ID_SCAN_FLAG_HEURISTIC: sResult = QString("Scan/Flag/Heuristic"); break;
        case ID_SCAN_FLAG_AGGRESSIVE: sResult = QString("Scan/Flag/Aggressive"); break;
        case ID_SCAN_FLAG_VERBOSE: sResult = QString("Scan/Flag/Verbose"); break;
        case ID_SCAN_FLAG_ALLTYPES: sResult = QString("Scan/Flag/AllTypes"); break;
        case ID_SCAN_USECACHE: sResult = QString("Scan/UseCache"); break;
        case ID_SCAN_FORMATRESULT: sResult = QString("Scan/FormatResult"); break;
        case ID_SCAN_LOG_PROFILING: sResult = QString("Scan/Log/Profiling"); break;
        case ID_SCAN_HIGHLIGHT: sResult = QString("Scan/Highlight"); break;
        case ID_SCAN_SORT: sResult = QString("Scan/Sort"); break;
        case ID_SCAN_HIDEUNKNOWN: sResult = QString("Scan/HideUnknown"); break;
        case ID_SCAN_ENGINE:
        case ID_SCAN_ENGINE_EMPTY: sResult = QString("Scan/Engine"); break;
        case ID_SCAN_ENGINE_DIE_ENABLED: sResult = QString("Scan/Engine/Die/Enabled"); break;
        case ID_SCAN_ENGINE_NFD_ENABLED: sResult = QString("Scan/Engine/Nfd/Enabled"); break;
        case ID_SCAN_ENGINE_PEID_ENABLED: sResult = QString("Scan/Engine/Peid/Enabled"); break;
        case ID_SCAN_ENGINE_YARA_ENABLED: sResult = QString("Scan/Engine/Yara/Enabled"); break;
        case ID_SCAN_DIE_DATABASE_MAIN_PATH: sResult = QString("Scan/Die/Database/Main/Path"); break;
        case ID_SCAN_DIE_DATABASE_MAIN_UPDATE_URL: sResult = QString("Scan/Die/Database/Main/UpdateUrl"); break;
        case ID_SCAN_DIE_DATABASE_EXTRA_PATH: sResult = QString("Scan/Die/Database/Extra/Path"); break;
        case ID_SCAN_DIE_DATABASE_EXTRA_UPDATE_URL: sResult = QString("Scan/Die/Database/Extra/UpdateUrl"); break;
        case ID_SCAN_DIE_DATABASE_CUSTOM_PATH: sResult = QString("Scan/Die/Database/Custom/Path"); break;
        case ID_SCAN_DIE_DATABASE_EXTRA_ENABLED: sResult = QString("Scan/Die/Database/Extra/Enabled"); break;
        case ID_SCAN_DIE_DATABASE_CUSTOM_ENABLED: sResult = QString("Scan/Die/Database/Custom/Enabled"); break;
        case ID_SCAN_YARA_DATABASE_PATH: sResult = QString("Scan/Yara/Database/Path"); break;
        case ID_SCAN_YARA_DATABASE_UPDATE_URL: sResult = QString("Scan/Yara/Database/UpdateUrl"); break;
        case ID_SCAN_PEID_DATABASE_PATH: sResult = QString("Scan/Peid/Database/Path"); break;
        case ID_SCAN_PEID_DATABASE_UPDATE_URL: sResult = QString("Scan/Peid/Database/UpdateUrl"); break;
        case ID_SCAN_DIRECTORY_PATH: sResult = QString("Scan/Directory/Path"); break;
        case ID_SCAN_TARGET_ALL: sResult = QString("Scan/Target/All"); break;
        case ID_SCAN_TARGET_ALLFILETYPES: sResult = QString("Scan/Target/AllFileTypes"); break;
        case ID_SCAN_TARGET_ALLTYPES: sResult = QString("Scan/Target/AllTypes"); break;
        case ID_SCAN_TARGET_FILETYPES: sResult = QString("Scan/Target/FileTypes"); break;
        case ID_SCAN_TARGET_TYPES: sResult = QString("Scan/Target/Types"); break;
        case ID_SCAN_COLOR_INSTALLER: sResult = QString("Scan/Color/Installer"); break;
        case ID_SCAN_COLOR_SFX: sResult = QString("Scan/Color/Sfx"); break;
        case ID_SCAN_COLOR_ARCHIVE: sResult = QString("Scan/Color/Archive"); break;
        case ID_SCAN_COLOR_PROTECTION: sResult = QString("Scan/Color/Protection"); break;
        case ID_SCAN_COLOR_PETOOL: sResult = QString("Scan/Color/PeTool"); break;
        case ID_SCAN_COLOR_APKTOOL: sResult = QString("Scan/Color/ApkTool"); break;
        case ID_SCAN_COLOR_OS: sResult = QString("Scan/Color/OS"); break;
        case ID_SCAN_COLOR_VM: sResult = QString("Scan/Color/VM"); break;
        case ID_SCAN_COLOR_PLATFORM: sResult = QString("Scan/Color/Platform"); break;
        case ID_SCAN_COLOR_DOSEXTENDER: sResult = QString("Scan/Color/DosExtender"); break;
        case ID_SCAN_COLOR_FORMAT: sResult = QString("Scan/Color/Format"); break;
        case ID_SCAN_COLOR_SIGNTOOL: sResult = QString("Scan/Color/SignTool"); break;
        case ID_SCAN_COLOR_CERTIFICATE: sResult = QString("Scan/Color/Certificate"); break;
        case ID_SCAN_COLOR_LICENSING: sResult = QString("Scan/Color/Licensing"); break;
        case ID_SCAN_COLOR_LANGUAGE: sResult = QString("Scan/Color/Language"); break;
        case ID_SCAN_COLOR_CORRUPTEDDATA: sResult = QString("Scan/Color/CorruptedData"); break;
        case ID_SCAN_COLOR_PERSONALDATA: sResult = QString("Scan/Color/PersonalData"); break;
        case ID_SCAN_COLOR_AUTHOR: sResult = QString("Scan/Color/Author"); break;
        case ID_SCAN_COLOR_VIRUS: sResult = QString("Scan/Color/Virus"); break;
        case ID_SCAN_COLOR_TROJAN: sResult = QString("Scan/Color/Trojan"); break;
        case ID_SCAN_COLOR_MALWARE: sResult = QString("Scan/Color/Malware"); break;
        case ID_SCAN_COLOR_DEBUG: sResult = QString("Scan/Color/Debug"); break;
        case ID_SCAN_COLOR_DEBUGDATA: sResult = QString("Scan/Color/DebugData"); break;
        case ID_SCAN_COLOR_GAMEENGINE: sResult = QString("Scan/Color/GameEngine"); break;
        case ID_SCAN_COLOR_COMPILER: sResult = QString("Scan/Color/Compiler"); break;
        case ID_SCAN_COLOR_COMPRESSOR: sResult = QString("Scan/Color/Compressor"); break;
        case ID_SCAN_COLOR_CONVERTER: sResult = QString("Scan/Color/Converter"); break;
        case ID_SCAN_COLOR_CREATOR: sResult = QString("Scan/Color/Creator"); break;
        case ID_SCAN_COLOR_DATA: sResult = QString("Scan/Color/Data"); break;
        case ID_SCAN_COLOR_DATABASE: sResult = QString("Scan/Color/Database"); break;
        case ID_SCAN_COLOR_DOCUMENT: sResult = QString("Scan/Color/Document"); break;
        case ID_SCAN_COLOR_GENERIC: sResult = QString("Scan/Color/Generic"); break;
        case ID_SCAN_COLOR_IMAGE: sResult = QString("Scan/Color/Image"); break;
        case ID_SCAN_COLOR_INSTALLERDATA: sResult = QString("Scan/Color/InstallerData"); break;
        case ID_SCAN_COLOR_LIBRARY: sResult = QString("Scan/Color/Library"); break;
        case ID_SCAN_COLOR_LINKER: sResult = QString("Scan/Color/Linker"); break;
        case ID_SCAN_COLOR_LOADER: sResult = QString("Scan/Color/Loader"); break;
        case ID_SCAN_COLOR_OBFUSCATOR: sResult = QString("Scan/Color/Obfuscator"); break;
        case ID_SCAN_COLOR_OVERLAY: sResult = QString("Scan/Color/Overlay"); break;
        case ID_SCAN_COLOR_PACKAGE: sResult = QString("Scan/Color/Package"); break;
        case ID_SCAN_COLOR_PLAYER: sResult = QString("Scan/Color/Player"); break;
        case ID_SCAN_COLOR_PRODUCER: sResult = QString("Scan/Color/Producer"); break;
        case ID_SCAN_COLOR_PROTECTORDATA: sResult = QString("Scan/Color/ProtectorData"); break;
        case ID_SCAN_COLOR_ROM: sResult = QString("Scan/Color/ROM"); break;
        case ID_SCAN_COLOR_SFXDATA: sResult = QString("Scan/Color/SfxData"); break;
        case ID_SCAN_COLOR_SOURCECODE: sResult = QString("Scan/Color/SourceCode"); break;
        case ID_SCAN_COLOR_STUB: sResult = QString("Scan/Color/Stub"); break;
        case ID_SCAN_COLOR_TOOL: sResult = QString("Scan/Color/Tool"); break;
        case ID_SIGNATURES_PATH: sResult = QString("Signatures/Path"); break;
        case ID_INFO_DATABASE_PATH: sResult = QString("Info/Path"); break;
        case ID_INFO_DATABASE_UPDATE_URL: sResult = QString("Info/Database/UpdateUrl"); break;
        case ID_ONLINETOOLS_VIRUSTOTAL_APIKEY: sResult = QString("OnlineTools/VirusTotalApi"); break;
        case ID_DISASM_FONT: sResult = QString("Disasm/Font"); break;
        case ID_DISASM_LOCATIONCOLON: sResult = QString("Disasm/LocationColon"); break;
        case ID_DISASM_UPPERCASE: sResult = QString("Disasm/Uppercase"); break;
        case ID_DISASM_HIGHLIGHT: sResult = QString("Disasm/Highlight"); break;
        case ID_DISASM_SYNTAX: sResult = QString("Disasm/Syntax"); break;
        case ID_DISASM_COLOR_ARROWS: sResult = QString("Disasm/Color/Arrows"); break;
        case ID_DISASM_COLOR_ARROWS_SELECTED: sResult = QString("Disasm/Color/Arrows/Selected"); break;
        case ID_DISASM_COLOR_REGS: sResult = QString("Disasm/Color/Regs"); break;
        case ID_DISASM_COLOR_NUMBERS: sResult = QString("Disasm/Color/Numbers"); break;
        case ID_DISASM_COLOR_OPCODE: sResult = QString("Disasm/Color/Opcode"); break;
        case ID_DISASM_COLOR_REFS: sResult = QString("Disasm/Color/Refs"); break;
        case ID_DISASM_COLOR_X86_REGS_GENERAL: sResult = QString("Disasm/Color/x86/Regs/General"); break;
        case ID_DISASM_COLOR_X86_REGS_STACK: sResult = QString("Disasm/Color/x86/Regs/Stack"); break;
        case ID_DISASM_COLOR_X86_REGS_SEGMENT: sResult = QString("Disasm/Color/x86/Regs/Segment"); break;
        case ID_DISASM_COLOR_X86_REGS_DEBUG: sResult = QString("Disasm/Color/x86/Regs/Debug"); break;
        case ID_DISASM_COLOR_X86_REGS_IP: sResult = QString("Disasm/Color/x86/Regs/IP"); break;
        case ID_DISASM_COLOR_X86_REGS_FLAGS: sResult = QString("Disasm/Color/x86/Regs/Flags"); break;
        case ID_DISASM_COLOR_X86_REGS_FPU: sResult = QString("Disasm/Color/x86/Regs/Float"); break;
        case ID_DISASM_COLOR_X86_REGS_XMM: sResult = QString("Disasm/Color/x86/Regs/XMM"); break;
        case ID_DISASM_COLOR_X86_REGS_YMM: sResult = QString("Disasm/Color/x86/Regs/YMM"); break;
        case ID_DISASM_COLOR_X86_REGS_ZMM: sResult = QString("Disasm/Color/x86/Regs/ZMM"); break;
        case ID_DISASM_COLOR_X86_OPCODE_CALL: sResult = QString("Disasm/Color/x86/Opcode/call"); break;
        case ID_DISASM_COLOR_X86_OPCODE_RET: sResult = QString("Disasm/Color/x86/Opcode/ret"); break;
        case ID_DISASM_COLOR_X86_OPCODE_COND_JMP: sResult = QString("Disasm/Color/x86/Opcode/cond_jmp"); break;
        case ID_DISASM_COLOR_X86_OPCODE_PUSH: sResult = QString("Disasm/Color/x86/Opcode/push"); break;
        case ID_DISASM_COLOR_X86_OPCODE_POP: sResult = QString("Disasm/Color/x86/Opcode/pop"); break;
        case ID_DISASM_COLOR_X86_OPCODE_NOP: sResult = QString("Disasm/Color/x86/Opcode/nop"); break;
        case ID_DISASM_COLOR_X86_OPCODE_JMP: sResult = QString("Disasm/Color/x86/Opcode/jmp"); break;
        case ID_DISASM_COLOR_X86_OPCODE_INT3: sResult = QString("Disasm/Color/x86/Opcode/int3"); break;
        case ID_DISASM_COLOR_X86_OPCODE_SYSCALL: sResult = QString("Disasm/Color/x86/Opcode/syscall"); break;
        case ID_DISASM_COLOR_ARM_REGS_GENERAL: sResult = QString("Disasm/Color/arm/Regs/General"); break;
        case ID_DISASM_COLOR_ARM_REGS_STACK: sResult = QString("Disasm/Color/arm/Regs/Stack"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_BRANCH: sResult = QString("Disasm/Color/arm/Opcode/b"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_BRANCHLINK: sResult = QString("Disasm/Color/arm/Opcode/bl"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_RET: sResult = QString("Disasm/Color/arm/Opcode/ret"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_PUSH: sResult = QString("Disasm/Color/arm/Opcode/push"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_POP: sResult = QString("Disasm/Color/arm/Opcode/pop"); break;
        case ID_DISASM_COLOR_ARM_OPCODE_NOP: sResult = QString("Disasm/Color/arm/Opcode/nop"); break;
        case ID_HEX_FONT: sResult = QString("Hex/Font"); break;
        case ID_HEX_LOCATIONCOLON: sResult = QString("Hex/LocationColon"); break;
        case ID_STACK_FONT: sResult = QString("Stack/Font"); break;
        case ID_STACK_LOCATIONCOLON: sResult = QString("Stack/LocationColon"); break;
        case ID_REGISTERS_FONT: sResult = QString("Registers/Font"); break;
        case ID_DEBUGGER_COLOR_BREAKPOINT: sResult = QString("Debugger/Color/Breakpoint"); break;
        case ID_IODRIVER_FILENAME: sResult = QString("IODriver/FileName"); break;
        case ID_IODRIVER_SERVICENAME: sResult = QString("IODriver/ServiceName"); break;
        case ID_STRUCTS_PATH: sResult = QString("Structs/Path"); break;
        case ID_NU_RECENTFILES: sResult = QString("RecentFiles"); break;
        case ID_NU_LASTDIRECTORY: sResult = QString("LastDirectory"); break;
        default: sResult = QString("Unknown");
    }

    return sResult;
}

QString XOptions::getLastDirectory()
{
    QString sResult;

    bool bSaveLastDirectory = getValue(ID_FILE_SAVELASTDIRECTORY).toBool();
    QString sLastDirectory = getValue(ID_NU_LASTDIRECTORY).toString();

    if (bSaveLastDirectory && (sLastDirectory != "") && QDir().exists(sLastDirectory)) {
        sResult = sLastDirectory;
    }

    return sResult;
}

void XOptions::setLastDirectory(const QString &sPathName)
{
    QString _sPathName = sPathName;

    if (getValue(ID_FILE_SAVELASTDIRECTORY).toBool()) {
        QFileInfo fi(_sPathName);

        if (fi.isFile()) {
            _sPathName = fi.absolutePath();
        } else if (fi.isDir()) {
            _sPathName = fi.absoluteFilePath();
        }

        setValue(ID_NU_LASTDIRECTORY, _sPathName);
    }
}

void XOptions::setLastFileName(const QString &sFileName)
{
    QFileInfo fi(sFileName);

    QString sDirectory;

    if (fi.isFile()) {
        sDirectory = fi.absolutePath();
    }

    if (getValue(ID_FILE_SAVELASTDIRECTORY).toBool()) {
        setValue(ID_NU_LASTDIRECTORY, sDirectory);
    }

    if (getValue(ID_FILE_SAVERECENTFILES).toBool()) {
        QString _sFileName = fi.absoluteFilePath();

        if (_sFileName != "") {
            QList<QVariant> listFiles = getValue(ID_NU_RECENTFILES).toList();

            listFiles.removeAll(_sFileName);

            listFiles.append(QVariant(_sFileName));

            if (listFiles.count() > m_nMaxRecentFilesCount) {
                listFiles.removeFirst();
            }

            m_mapValues.insert(ID_NU_RECENTFILES, listFiles);

#ifdef QT_GUI_LIB
            _updateRecentFilesMenu();
#endif
        }
    }
}

void XOptions::clearRecentFiles()
{
    m_mapValues.insert(ID_NU_RECENTFILES, QList<QVariant>());

#ifdef QT_GUI_LIB
    _updateRecentFilesMenu();
#endif
}

void XOptions::openRecentFile()
{
#ifdef QT_GUI_LIB
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        QString sFileName = pAction->data().toString();

        if (QFile::exists(sFileName)) {
            emit openFile(sFileName);
        } else {
            emit errorMessage(QString("%1: %2").arg(tr("Cannot find file"), sFileName));
        }
    }
#endif
}

void XOptions::setCodePageSlot()
{
#ifdef QT_GUI_LIB
    QAction *pAction = qobject_cast<QAction *>(sender());

    if (pAction) {
        QString sCodePage = pAction->data().toString();

        emit setCodePage(sCodePage);
    }
#endif
}

QList<QString> XOptions::getRecentFiles()
{
    QList<QString> listResult;

    QList<QVariant> listFiles = getValue(ID_NU_RECENTFILES).toList();

    qint32 nNumberOfFiles = listFiles.size();

    for (qint32 i = 0; i < nNumberOfFiles; i++) {
        listResult.append(listFiles.at(i).toString());
    }

    return listResult;
}

QString XOptions::getScanEngine()
{
    QString sResult;

    if (isValuePresent(ID_SCAN_ENGINE)) {
        sResult = getValue(ID_SCAN_ENGINE).toString();
    } else if (isValuePresent(ID_SCAN_ENGINE_EMPTY)) {
        sResult = getValue(ID_SCAN_ENGINE_EMPTY).toString();
    }

    return sResult;
}

QString XOptions::getInfoPath()
{
    return getValue(ID_INFO_DATABASE_PATH).toString();
}

QString XOptions::getRootPath()
{
    return getValue(ID_ROOTPATH).toString();
}

QString XOptions::getDataPath()
{
    return getValue(ID_DATAPATH).toString();
}

QString XOptions::getJson()
{
    return getValue(ID_JSON).toString();
}

QString XOptions::getAuthUser()
{
    return getValue(ID_AUTHUSER).toString();
}

QString XOptions::getAuthToken()
{
    return getValue(ID_AUTHTOKEN).toString();
}

QString XOptions::getVirusTotalApiKey()
{
    return getValue(ID_ONLINETOOLS_VIRUSTOTAL_APIKEY).toString();
}

#ifdef QT_GUI_LIB
void XOptions::adjustStayOnTop(QWidget *pWidget)
{
    if (isIDPresent(XOptions::ID_VIEW_STAYONTOP)) {
        _adjustStayOnTop(pWidget, isStayOnTop());
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_adjustStayOnTop(QWidget *pWidget, bool bState)
{
    Qt::WindowFlags wf = pWidget->windowFlags();

    if (bState) {
        wf |= Qt::WindowStaysOnTopHint;
    } else {
        wf &= ~(Qt::WindowStaysOnTopHint);
    }

    pWidget->setWindowFlags(wf);
    pWidget->show();
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::_isAjustStayOnTop(QWidget *pWidget)
{
    bool bResult = false;

    Qt::WindowFlags wf = pWidget->windowFlags();

    if (wf & Qt::WindowStaysOnTopHint) {
        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_adjustApplicationModal(QWidget *pWidget, bool bState)
{
    if (bState) {
        pWidget->setWindowModality(Qt::ApplicationModal);
    } else {
        pWidget->setWindowModality(Qt::NonModal);
    }

    pWidget->show();
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_adjustFullScreen(QWidget *pWidget, bool bState)
{
    if (bState) {
        pWidget->showFullScreen();
    } else {
        pWidget->showNormal();
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_adjustMaximizeScreen(QWidget *pWidget, bool bState)
{
    if (bState) {
        pWidget->showMaximized();
    } else {
        pWidget->showNormal();
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_adjustMinimizeScreen(QWidget *pWidget, bool bState)
{
    if (bState) {
        pWidget->showMinimized();
    } else {
        pWidget->showNormal();
    }
}
#endif
#ifdef QT_GUI_LIB
QFont XOptions::adjustFont(QWidget *pWidget, ID id)
{
    QFont result;
    QString sFont = getValue(id).toString();

    if ((sFont != "") && result.fromString(sFont)) {
        QFont fontOld = pWidget->font();

        result.setBold(fontOld.bold());

        pWidget->setFont(result);
    }

    return result;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustWindow(QWidget *pWidget)
{
    if (isIDPresent(XOptions::ID_VIEW_STAYONTOP)) {
        adjustStayOnTop(pWidget);
    }

    adjustWidget(pWidget, XOptions::ID_VIEW_FONT);
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustWidget(QWidget *pWidget, ID id)
{
    if (isIDPresent(id)) {
        adjustFont(pWidget, id);
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTableWidget(QTableWidget *pWidget, ID id)
{
    if (isIDPresent(id)) {
        QFont _font = adjustFont(pWidget, id);

        const QFontMetricsF fm(_font);

        qint32 nCharHeight = fm.height();

        pWidget->verticalHeader()->setMinimumSectionSize(nCharHeight + 3);
        pWidget->verticalHeader()->setDefaultSectionSize(nCharHeight + 3);
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTableView(QTableView *pWidget, ID id)
{
    if (isIDPresent(id)) {
        QFont _font = adjustFont(pWidget, id);

        const QFontMetricsF fm(_font);

        qint32 nCharHeight = fm.height();

        pWidget->verticalHeader()->setMinimumSectionSize(nCharHeight + 3);
        pWidget->verticalHeader()->setDefaultSectionSize(nCharHeight + 3);
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTreeWidget(QTreeWidget *pWidget, ID id, bool bAdjustSize)
{
    if (isIDPresent(id)) {
        adjustFont(pWidget, id);
    }

    if (bAdjustSize) {
        adjustTreeWidgetSize(pWidget);
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTreeView(QTreeView *pWidget, ID id)
{
    if (isIDPresent(id)) {
        adjustFont(pWidget, id);
    }
}
#endif
#ifdef QT_GUI_LIB
QFont XOptions::getDefaultFont(qint32 nFontSize)
{
    QFont fontResult;

    if (nFontSize == -1) {
#ifdef Q_OS_MACOS
        nFontSize = 13;
#endif
#ifdef Q_OS_WIN
        nFontSize = 9;
#endif
#ifdef Q_OS_LINUX
        nFontSize = 11;
#endif
    }

#ifdef Q_OS_WIN
    fontResult = QFont("Segoe UI", nFontSize);
#endif
#ifdef Q_OS_LINUX
    fontResult = QFont("Ubuntu", nFontSize);
#endif
#ifdef Q_OS_MACOS
    fontResult = QFont(".AppleSystemUIFont", nFontSize);
#endif

    return fontResult;
}
#endif
#ifdef QT_GUI_LIB
QFont XOptions::getMonoFont(qint32 nFontSize)
{
    QFont fontResult;

    if (nFontSize == -1) {
#ifdef Q_OS_MACOS
        nFontSize = 12;
#endif
#ifdef Q_OS_WIN
        nFontSize = 10;
#endif
#ifdef Q_OS_LINUX
        nFontSize = 10;
#endif
    }

    QFontDatabase fontDatabase;
    QStringList listFamilies = fontDatabase.families();

    QString sFontFamily;

#ifdef Q_OS_WIN
    if (listFamilies.contains("Courier New")) {
        sFontFamily = "Courier New";
    } else if (listFamilies.contains("Courier")) {
        sFontFamily = "Courier";
    } else if (listFamilies.contains("Consolas")) {
        sFontFamily = "Consolas";
    }
#endif
#ifdef Q_OS_LINUX
    if (listFamilies.contains("DejaVu Sans Mono")) {
        sFontFamily = "DejaVu Sans Mono";
    } else if (listFamilies.contains("Liberation Mono")) {
        sFontFamily = "Liberation Mono";
    } else if (listFamilies.contains("Courier New")) {
        sFontFamily = "Courier New";
    } else if (listFamilies.contains("Courier")) {
        sFontFamily = "Courier";
    }
#endif
#ifdef Q_OS_MACOS
    if (listFamilies.contains("Menlo")) {
        sFontFamily = "Menlo";
    } else if (listFamilies.contains("Monaco")) {
        sFontFamily = "Monaco";
    } else if (listFamilies.contains("Courier New")) {
        sFontFamily = "Courier New";
    } else if (listFamilies.contains("Courier")) {
        sFontFamily = "Courier";
    }
#endif

    if (!sFontFamily.isEmpty()) {
        fontResult = QFont(sFontFamily, nFontSize);
    } else {
        fontResult.setFamily("monospace");
        fontResult.setStyleHint(QFont::Monospace);
        fontResult.setPointSize(nFontSize);
    }

    return fontResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setMonoFont(QWidget *pWidget, qint32 nSize)
{
    QFont font = pWidget->font();
    QFont fontMono = getMonoFont(nSize);

    font.setFamily(fontMono.family());
    font.setPointSize(fontMono.pointSize());

    pWidget->setFont(font);
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setCheckBox(QCheckBox *pCheckBox, XOptions::ID id)
{
    pCheckBox->setChecked(getValue(id).toBool());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::getCheckBox(QCheckBox *pCheckBox, XOptions::ID id)
{
    setValue(id, pCheckBox->isChecked());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setCheckBox(QGroupBox *pGroupBox, ID id)
{
    pGroupBox->setChecked(getValue(id).toBool());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::getCheckBox(QGroupBox *pGroupBox, ID id)
{
    setValue(id, pGroupBox->isChecked());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setComboBox(QComboBox *pComboBox, XOptions::ID id)
{
    const bool bBlocked1 = pComboBox->blockSignals(true);

    pComboBox->clear();

    QString sValue = getValue(id).toString();

    if (id == ID_VIEW_STYLE) {
        pComboBox->addItem("Default", "");
        QStringList listKeys = QStyleFactory::keys();

        qint32 nNumberOfKeys = listKeys.count();

        for (qint32 i = 0; i < nNumberOfKeys; i++) {
            QString sRecord = listKeys.at(i);
            pComboBox->addItem(sRecord, sRecord);
        }
    } else if (id == ID_VIEW_LANG) {
        pComboBox->addItem("English", "");
        pComboBox->addItem("System", "System");

        QList<QString> listFileNames = getAllFilesFromDirectory(convertPathName("$data/lang"), "*.qm");

        qint32 nNumberOfRecords = listFileNames.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QFileInfo fi(listFileNames.at(i));

            QString sRecord = fi.completeBaseName();

            QLocale locale(sRecord.section("_", 1, -1));
            QString sLocale = locale.nativeLanguageName();

            if (sRecord.count("_") == 2) {
                sLocale += QString("(%1)").arg(locale.nativeCountryName());
            }

            if (sLocale != "") {
                sLocale.replace(0, 1, sLocale[0].toUpper());
            }

            sLocale += QString("[%1]").arg(locale.languageToString(locale.language()));

            pComboBox->addItem(sLocale, sRecord);
        }
    } else if (id == ID_VIEW_QSS) {
        pComboBox->addItem("Default", "");

        QList<QString> listFileNames = getAllFilesFromDirectory(convertPathName("$data/qss"), "*.qss");

        qint32 nNumberOfRecords = listFileNames.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QFileInfo fi(listFileNames.at(i));

            QString sRecord = fi.completeBaseName();

            pComboBox->addItem(sRecord, sRecord);
        }
    } else if (id == ID_SCAN_ENGINE) {
        pComboBox->addItem(tr("Automatic"), "auto");
        pComboBox->addItem(QString("Detect It Easy (DiE)"), "die");
        pComboBox->addItem(QString("Nauz File Detector (NFD)"), "nfd");
        pComboBox->addItem(QString("PEiD"), "peid");
        // TODO: TrID
        // TODO: EXEInfoPE
        // TODO: PE Detective
#ifdef USE_YARA
        pComboBox->addItem(QString("Yara rules"), "yara");
#endif
    } else if (id == ID_SCAN_ENGINE_EMPTY) {
        pComboBox->addItem("", "");
        pComboBox->addItem(QString("Detect It Easy (DiE)"), "die");
        pComboBox->addItem(QString("Nauz File Detector (NFD)"), "nfd");
        pComboBox->addItem(QString("PEiD"), "peid");
#ifdef USE_YARA
        pComboBox->addItem(QString("Yara rules"), "yara");
#endif
    } else if (id == ID_DISASM_SYNTAX) {
        pComboBox->addItem("Default", "");
        pComboBox->addItem(QString("ATT"), "ATT");
        pComboBox->addItem(QString("INTEL"), "INTEL");
        pComboBox->addItem(QString("MASM"), "MASM");
        pComboBox->addItem(QString("MOTOROLA"), "MOTOROLA");
    } else if (id == ID_FEATURE_READBUFFERSIZE) {
        pComboBox->addItem("", 0);
        pComboBox->addItem("1 KiB", 1 * 1024);
        pComboBox->addItem("2 KiB", 2 * 1024);
        pComboBox->addItem("4 KiB", 4 * 1024);
        pComboBox->addItem("8 KiB", 8 * 1024);
        pComboBox->addItem("16 KiB", 16 * 1024);
        pComboBox->addItem("32 KiB", 32 * 1024);
        pComboBox->addItem("64 KiB", 64 * 1024);
        pComboBox->addItem("128 KiB", 128 * 1024);
        pComboBox->addItem("256 KiB", 256 * 1024);
        pComboBox->addItem("512 KiB", 512 * 1024);
    } else if (id == ID_FEATURE_FILEBUFFERSIZE) {
        pComboBox->addItem("", 0);
        pComboBox->addItem("1 MiB", 1 * 1024 * 1024);
        pComboBox->addItem("2 MiB", 2 * 1024 * 1024);
        pComboBox->addItem("4 MiB", 4 * 1024 * 1024);
        pComboBox->addItem("8 MiB", 8 * 1024 * 1024);
        pComboBox->addItem("16 MiB", 16 * 1024 * 1024);
        pComboBox->addItem("32 MiB", 32 * 1024 * 1024);
        pComboBox->addItem("64 MiB", 64 * 1024 * 1024);
        pComboBox->addItem("128 MiB", 128 * 1024 * 1024);
        pComboBox->addItem("256 MiB", 256 * 1024 * 1024);
        pComboBox->addItem("512 MiB", 512 * 1024 * 1024);
        pComboBox->addItem("1 GiB", 1LL * 1024 * 1024 * 1024);
        pComboBox->addItem("2 GiB", 2LL * 1024 * 1024 * 1024);
        pComboBox->addItem("4 GiB", 4LL * 1024 * 1024 * 1024);
    }

    qint32 nNumberOfItems = pComboBox->count();
    qint32 nIndex = -1;

    for (qint32 i = 0; i < nNumberOfItems; i++) {
        if (sValue == pComboBox->itemData(i, Qt::UserRole)) {
            nIndex = i;
        }
    }

    if (nIndex != -1) {
        pComboBox->setCurrentIndex(nIndex);
    }

    pComboBox->blockSignals(bBlocked1);
}
#endif
#ifdef QT_GUI_LIB
void XOptions::getComboBox(QComboBox *pComboBox, XOptions::ID id)
{
    setValue(id, pComboBox->currentData());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setLineEdit(QLineEdit *pLineEdit, XOptions::ID id)
{
    pLineEdit->setText(getValue(id).toString());
}
#endif
#ifdef QT_GUI_LIB
void XOptions::getLineEdit(QLineEdit *pLineEdit, XOptions::ID id)
{
    setValue(id, pLineEdit->text());
}
#endif
bool XOptions::isSaveBackup()
{
    return getValue(XOptions::ID_FILE_SAVEBACKUP).toBool();
}

bool XOptions::isSaveLastDirectory()
{
    return getValue(XOptions::ID_FILE_SAVELASTDIRECTORY).toBool();
}

bool XOptions::isSaveRecentFiles()
{
    return getValue(XOptions::ID_FILE_SAVERECENTFILES).toBool();
}

bool XOptions::isRestartNeeded()
{
    return m_bIsNeedRestart;
}

bool XOptions::isStayOnTop()
{
    return getValue(XOptions::ID_VIEW_STAYONTOP).toBool();
}

bool XOptions::isScanAfterOpen()
{
    return getValue(XOptions::ID_SCAN_SCANAFTEROPEN).toBool();
}

bool XOptions::isSingleApplication()
{
    return getValue(XOptions::ID_VIEW_SINGLEAPPLICATION).toBool();
}

bool XOptions::isShowLogo()
{
    return getValue(XOptions::ID_VIEW_SHOWLOGO).toBool();
}

QString XOptions::getSearchSignaturesPath()
{
    return getValue(XOptions::ID_SIGNATURES_PATH).toString();
}

QString XOptions::getStructsPath()
{
    return getValue(XOptions::ID_STRUCTS_PATH).toString();
}

void XOptions::adjustApplicationInitAttributes()
{
#ifdef QT_GUI_LIB
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
#endif
#endif
}
#ifdef QT_GUI_LIB
QColor XOptions::stringToColor(QString sColor)
{
    QColor result;

    if (sColor != "") {
        result.setNamedColor(sColor);
    }

    return result;
}
#endif
#ifdef QT_GUI_LIB
QString XOptions::colorToString(const QColor &color)
{
    return color.name();
}
#endif
#ifdef QT_GUI_LIB
QIcon XOptions::createIcon(QChar unicode, qint32 nWidth, qint32 nHeight)
{
    return createIcon(unicode.unicode(), nWidth, nHeight);
}

QIcon XOptions::createIcon(quint32 codepoint, qint32 nWidth, qint32 nHeight)
{
    // Create a pixmap with the specified dimensions
    QPixmap pixmap(nWidth, nHeight);
    pixmap.fill(Qt::transparent);

    // Setup painter
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Setup font - try multiple emoji fonts
    QFont font;
    font.setPixelSize(qMin(nWidth, nHeight) * 0.8);  // 80% of size for better fit

    font.setFamily("Segoe UI Emoji");

    painter.setFont(font);

    // Draw the Unicode character centered
    painter.setPen(Qt::black);

    // Convert codepoint to QString properly
    QString text;
    if (codepoint <= 0xFFFF) {
        text = QString(QChar(codepoint));
    } else {
        // Handle surrogate pairs for codepoints > U+FFFF
        text = QString::fromUcs4(&codepoint, 1);
    }

    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);

    painter.end();

    //     // Debug: Save icon to C:\tmp
    //     static bool debugDumpEnabled = true;
    //     if (debugDumpEnabled) {
    //         QString filename = QString("C:/tmp/icon_%1_%2x%3_U%4.png")
    //             .arg(QDateTime::currentMSecsSinceEpoch())
    //             .arg(nWidth)
    //             .arg(nHeight)
    //             .arg(codepoint, 4, 16, QChar('0'));
    //         pixmap.save(filename);
    // #ifdef QT_DEBUG
    //         qDebug() << "Saved icon:" << filename << "text:" << text << "code:" << QString("U+%1").arg(codepoint, 4, 16, QChar('0'));
    // #endif
    //     }

    return QIcon(pixmap);
}

quint32 XOptions::iconTypeToUnicodeSymbol(ICONTYPE iconType)
{
    quint32 result;

    switch (iconType) {
        case ICONTYPE_ACTION: result = 0x26A1; break;          // ⚡ - High Voltage Sign (action/execute)
        case ICONTYPE_ADD: result = 0x2795; break;             // ➕ - Heavy Plus Sign
        case ICONTYPE_ADDRESS: result = 0x1F4CD; break;        // 📍 - Round Pushpin (location/address)
        case ICONTYPE_ALL: result = 0x2261; break;             // ≡ - Identical To (all/everything)
        case ICONTYPE_BACKWARD: result = 0x23EA; break;        // ⏪ - Black Left-Pointing Double Triangle
        case ICONTYPE_BOOKMARK: result = 0x1F516; break;       // 🔖 - Bookmark
        case ICONTYPE_CERTIFICATE: result = 0x1F4DC; break;    // 📜 - Scroll (certificate)
        case ICONTYPE_CODE: result = 0x1F5CB; break;           // 🗋 - Document (code file)
        case ICONTYPE_COPY: result = 0x1F4CB; break;           // 📋 - Clipboard
        case ICONTYPE_DATA: result = 0x1F4CA; break;           // 📊 - Bar Chart (data)
        case ICONTYPE_DEBUG: result = 0x1F41E; break;          // 🐞 - Lady Beetle (debug)
        case ICONTYPE_DEMANGLE: result = 0x1F9F5; break;       // 🧵 - Thread (untangle)
        case ICONTYPE_DIE: result = 0x1F3B2; break;            // 🎲 - Game Die
        case ICONTYPE_DISASM: result = 0x2699; break;          // ⚙ - Gear (disassembly)
        case ICONTYPE_DOTNET: result = 0x1F310; break;         // 🌐 - Globe With Meridians (.NET)
        case ICONTYPE_DUMPTOFILE: result = 0x1F4BE; break;     // 💾 - Floppy Disk (save/dump)
        case ICONTYPE_EDIT: result = 0x270E; break;            // ✎ - Lower Right Pencil
        case ICONTYPE_ENTROPY: result = 0x1F300; break;        // 🌀 - Cyclone (chaos/entropy)
        case ICONTYPE_ENTRY: result = 0x1F6AA; break;          // 🚪 - Door (entry point)
        case ICONTYPE_EXCEPTION: result = 0x26A0; break;       // ⚠ - Warning Sign
        case ICONTYPE_EXIT: result = 0x1F6AA; break;           // 🚪 - Door (exit)
        case ICONTYPE_EXPORT: result = 0x1F4E4; break;         // 📤 - Outbox Tray
        case ICONTYPE_EXTRACTOR: result = 0x1F5C4; break;      // 🗄 - File Cabinet (extract)
        case ICONTYPE_FILE: result = 0x1F4C4; break;           // 📄 - Page Facing Up
        case ICONTYPE_FOLLOW: result = 0x1F517; break;         // 🔗 - Link Symbol
        case ICONTYPE_FORWARD: result = 0x23E9; break;         // ⏩ - Black Right-Pointing Double Triangle
        case ICONTYPE_FUNCTION: result = 0x0192; break;        // ƒ - Latin Small Letter F With Hook
        case ICONTYPE_GENERIC: result = 0x25A0; break;         // ■ - Black Square
        case ICONTYPE_GOTO: result = 0x21E8; break;            // ⇨ - Rightwards White Arrow
        case ICONTYPE_HASH: result = 0x0023; break;            // # - Number Sign
        case ICONTYPE_HEADER: result = 0x1F4C3; break;         // 📃 - Page With Curl
        case ICONTYPE_HEX: result = 0x0023; break;             // # - Number Sign (hex)
        case ICONTYPE_IMPORT: result = 0x1F4E5; break;         // 📥 - Inbox Tray
        case ICONTYPE_INFO: result = 0x2139; break;            // ℹ - Information Source
        case ICONTYPE_LIBRARY: result = 0x1F4DA; break;        // 📚 - Books
        case ICONTYPE_LIST: result = 0x1F4CB; break;           // 📋 - Clipboard (list)
        case ICONTYPE_MANIFEST: result = 0x1F4DC; break;       // 📜 - Scroll (manifest)
        case ICONTYPE_MEMORYMAP: result = 0x1F5FA; break;      // 🗺 - World Map
        case ICONTYPE_METADATA: result = 0x1F3F7; break;       // 🏷 - Label (metadata)
        case ICONTYPE_MIME: result = 0x1F4CE; break;           // 📎 - Paperclip (MIME type)
        case ICONTYPE_NEW: result = 0x2795; break;             // ➕ - Heavy Plus Sign (new/add)
        case ICONTYPE_NEXT: result = 0x25B6; break;            // ▶ - Black Right-Pointing Triangle
        case ICONTYPE_NFD: result = 0x1F4C2; break;            // 📂 - Open File Folder
        case ICONTYPE_NOTE: result = 0x1F4DD; break;           // 📝 - Memo
        case ICONTYPE_OFFSET: result = 0x21F1; break;          // ⇱ - North West Arrow To Corner
        case ICONTYPE_OPEN: result = 0x1F4C2; break;           // 📂 - Open File Folder
        case ICONTYPE_OPTION: result = 0x2699; break;          // ⚙ - Gear (option/setting)
        case ICONTYPE_OVERLAY: result = 0x1F5C4; break;        // 🗄 - File Cabinet (overlay)
        case ICONTYPE_PATCH: result = 0x1FA79; break;          // 🩹 - Adhesive Bandage
        case ICONTYPE_PATH: result = 0x1F6E4; break;           // 🛤 - Railway Track (path)
        case ICONTYPE_REFERENCE: result = 0x1F517; break;      // 🔗 - Link Symbol (reference)
        case ICONTYPE_RELOAD: result = 0x21BB; break;          // ↻ - Clockwise Open Circle Arrow (refresh/reload)
        case ICONTYPE_RELOC: result = 0x1F4CD; break;          // 📍 - Round Pushpin (relocation)
        case ICONTYPE_REMOVE: result = 0x2796; break;          // ➖ - Heavy Minus Sign
        case ICONTYPE_RESIZE: result = 0x21D4; break;          // ⇔ - Left Right Double Arrow
        case ICONTYPE_RESOURCE: result = 0x1F4E6; break;       // 📦 - Package
        case ICONTYPE_SAVE: result = 0x1F5AB; break;           // 🖫 - White Hard Shell Floppy Disk
        case ICONTYPE_SCAN: result = 0x1F50D; break;           // 🔍 - Left-Pointing Magnifying Glass (scan/search)
        case ICONTYPE_SCRIPT: result = 0x1F4DC; break;         // 📜 - Scroll (script)
        case ICONTYPE_SEARCH: result = 0x1F50D; break;         // 🔍 - Left-Pointing Magnifying Glass
        case ICONTYPE_SECTION: result = 0x00A7; break;         // § - Section Sign
        case ICONTYPE_SEGMENT: result = 0x25A8; break;         // ▨ - Square With Upper Right To Lower Left Fill
        case ICONTYPE_SELECT: result = 0x1F5F1; break;         // 🗱 - Ballot Box With Check
        case ICONTYPE_SHORTCUT: result = 0x1F517; break;       // 🔗 - Link Symbol (shortcut)
        case ICONTYPE_SIGNATURE: result = 0x270D; break;       // ✍ - Writing Hand
        case ICONTYPE_SIZE: result = 0x1F4CF; break;           // 📏 - Straight Ruler
        case ICONTYPE_STRING: result = 0x1F4AC; break;         // 💬 - Speech Balloon
        case ICONTYPE_STRUCTS: result = 0x1F9F1; break;        // 🧱 - Brick (structure)
        case ICONTYPE_SYMBOL: result = 0x1F523; break;         // 🔣 - Input Symbol For Symbols
        case ICONTYPE_TABLE: result = 0x1F5C3; break;          // 🗃 - Card File Box
        case ICONTYPE_TLS: result = 0x1F512; break;            // 🔒 - Lock (TLS/security)
        case ICONTYPE_TOOL: result = 0x1F527; break;           // 🔧 - Wrench
        case ICONTYPE_VALUE: result = 0x1F4B0; break;          // 💰 - Money Bag (value)
        case ICONTYPE_VERSION: result = 0x1F4C5; break;        // 📅 - Calendar (version)
        case ICONTYPE_VIRUSTOTAL: result = 0x1F9A0; break;     // 🦠 - Microbe (virus)
        case ICONTYPE_VISUALIZATION: result = 0x1F4CA; break;  // 📊 - Bar Chart
        case ICONTYPE_WEBSITE: result = 0x1F310; break;        // 🌐 - Globe With Meridians
        case ICONTYPE_YARA: result = 0x1F50E; break;           // 🔎 - Right-Pointing Magnifying Glass (Yara scan)
        case ICONTYPE_INSPECTOR: result = 0x1F50D; break;      // 🔍 - Left-Pointing Magnifying Glass
        case ICONTYPE_CONVERTOR: result = 0x21C4; break;       // ⇄ - Rightwards Arrow Over Leftwards Arrow

        case ICONTYPE_NONE:
        default:
            result = 0;  // Null character for unmapped types
            break;
    }

    return result;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustApplicationView(const QString &sTranslationName, XOptions *pOptions)
{
    if (pOptions->isIDPresent(XOptions::ID_VIEW_STYLE)) {
        QString sStyle = pOptions->getValue(XOptions::ID_VIEW_STYLE).toString();

        if (sStyle != "") {
            qApp->setStyle(QStyleFactory::create(sStyle));
        }
    }

    if (pOptions->isIDPresent(XOptions::ID_VIEW_LANG)) {
        QTranslator *pTranslator = new QTranslator;  // Important
        QString sLang = pOptions->getValue(XOptions::ID_VIEW_LANG).toString();
        QString sLangsPath = convertPathName("$data/lang");

        bool bLoad = false;

        if (sLang == "System") {
            QLocale locale = QLocale::system();
            if (locale != QLocale::English) {
                bLoad = pTranslator->load(locale, sTranslationName, "_", sLangsPath, ".qm");
            }
        } else if (sLang != "") {
            bLoad = pTranslator->load(sLang, sLangsPath);
        }

        if (bLoad) {
            qApp->installTranslator(pTranslator);
        }
    }

    if (pOptions->isIDPresent(XOptions::ID_VIEW_QSS)) {
        QString sQss = pOptions->getValue(XOptions::ID_VIEW_QSS).toString();

        if (sQss != "") {
            QString sQssFileName = convertPathName(QString("$data/qss/%1.qss").arg(sQss));

            if (QFile::exists(sQssFileName)) {
                QFile file;
                file.setFileName(sQssFileName);

                if (file.open(QIODevice::ReadOnly)) {
                    QByteArray baQss = file.readAll();
                    qApp->setStyleSheet(baQss.data());
                    file.close();
                }
            }
        }
    }

    if (pOptions->isIDPresent(XOptions::ID_VIEW_FONT)) {
        QString sFont = pOptions->getValue(XOptions::ID_VIEW_FONT).toString();

        if (sFont != "") {
            qApp->setFont(sFont);
        }
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustListWidgetSize(QListWidget *pListWidget, qint32 nMinimumWidth)
{
    qint32 nWidth = nMinimumWidth;

    qint32 nNumberOfRecords = pListWidget->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        QString sText = pListWidget->item(i)->text() + "WWWW";
        nWidth = qMax(QFontMetrics(pListWidget->item(i)->font()).size(Qt::TextSingleLine, sText).width(), nWidth);
    }

    pListWidget->setMinimumWidth(0);
    pListWidget->setMaximumWidth(nWidth);
}
#endif
#ifdef QT_GUI_LIB
qint32 XOptions::_getTreeWidgetItemSize(QTreeWidget *pTreeWidget, QTreeWidgetItem *pTreeWidgetItem, qint32 nIndent, qint32 nLevel)
{
    qint32 nResult = 0;

    QString sText = pTreeWidgetItem->text(0) + "WW";

    QFont _font = pTreeWidget->font();
    QString sFont = _font.toString();

    nResult = QFontMetrics(_font).size(Qt::TextSingleLine, sText).width() + ((nIndent + 16) * nLevel);  // 16 size of icon

    qint32 nNumberOfRecords = pTreeWidgetItem->childCount();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        nResult = qMax(_getTreeWidgetItemSize(pTreeWidget, pTreeWidgetItem->child(i), nIndent, nLevel + 1), nResult);
    }

    return nResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTreeWidgetSize(QTreeWidget *pTreeWidget, qint32 nMinimumWidth)
{
    qint32 nWidth = nMinimumWidth;
    qint32 nIndent = pTreeWidget->indentation();
    qint32 nNumberOfItems = pTreeWidget->topLevelItemCount();

    for (qint32 i = 0; i < nNumberOfItems; i++) {
        nWidth = qMax(_getTreeWidgetItemSize(pTreeWidget, pTreeWidget->topLevelItem(i), nIndent, 1), nWidth);
    }

    pTreeWidget->setMinimumWidth(0);
    pTreeWidget->setMaximumWidth(nWidth);
}
#endif
// #ifdef QT_GUI_LIB
//  void XOptions::adjustApplicationView(QString sApplicationFileName,QString
//  sTranslationName)
//{
//     XOptions xOptions;

//    xOptions.setName(sApplicationFileName);

//    QList<XOptions::ID> listIDs;

//    listIDs.append(XOptions::ID_VIEW_STYLE);
//    listIDs.append(XOptions::ID_VIEW_LANG);
//    listIDs.append(XOptions::ID_VIEW_QSS);

//    xOptions.setValueIDs(listIDs);
//    xOptions.load();

//    xOptions.adjustApplicationView(sTranslationName,&xOptions);
//}
// #endif
#ifdef QT_GUI_LIB
QWidget *XOptions::getMainWidget(QWidget *pWidget)
{
    QWidget *pResult = pWidget;

    if (pResult) {
        while (pResult->parent()) {
            pResult = qobject_cast<QWidget *>(pResult->parent());
        }
    }

    return pResult;
}
#endif
#ifdef QT_GUI_LIB
QString XOptions::getTableModelText(QAbstractItemModel *pModel)
{
    QString sResult;

    if (pModel) {
        qint32 nNumberOfRows = pModel->rowCount();
        qint32 nNumberOfColumns = pModel->columnCount();

        QList<QString> listHeaders;
        QList<QList<QString>> listListStrings;

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            QString sHeader = pModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

            listHeaders.append(sHeader);
        }

        for (qint32 i = 0; i < nNumberOfRows; i++) {
            QList<QString> listStrings;

            for (qint32 j = 0; j < nNumberOfColumns; j++) {
                QString sString = pModel->data(pModel->index(i, j)).toString();

                listStrings.append(sString);
            }

            listListStrings.append(listStrings);
        }

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            if (i != (nNumberOfColumns - 1)) {
                sResult += QString("%1\t").arg(listHeaders.at(i));
            } else {
                sResult += QString("%1\r\n").arg(listHeaders.at(i));
            }
        }

        // mb TODO: csv,tsv,json,xml,json
        qint32 _nNumberOfLines = listListStrings.count();

        for (qint32 i = 0; i < _nNumberOfLines; i++) {
            qint32 _nNumberOfColumns = listListStrings.at(i).count();

            for (qint32 j = 0; j < _nNumberOfColumns; j++) {
                QString sString = listListStrings.at(i).at(j);

                if (j != (_nNumberOfColumns - 1)) {
                    sResult += QString("%1\t").arg(sString);
                } else {
                    sResult += QString("%1\r\n").arg(sString);
                }
            }
        }
    }

    return sResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::_getTreeModelText(QString *psString, QAbstractItemModel *pModel, QModelIndex index, qint32 nLevel)
{
    qint32 nNumberOfRows = pModel->rowCount(index);
    qint32 nNumberOfColumns = pModel->columnCount(index);

    for (qint32 i = 0; i < nNumberOfRows; i++) {
        QString sPrefix;
        sPrefix = sPrefix.leftJustified(4 * nLevel, ' ');

        *psString += sPrefix;

        for (qint32 j = 0; j < nNumberOfColumns; j++) {
            QString sString = pModel->data(pModel->index(i, j, index)).toString();

            if (j != (nNumberOfColumns - 1)) {
                *psString += QString("%1\t").arg(sString);
            } else {
                *psString += QString("%1\r\n").arg(sString);
            }
        }

        if (nNumberOfColumns) {
            _getTreeModelText(psString, pModel, pModel->index(i, 0, index), nLevel + 1);
        }
    }
}
#endif
#ifdef QT_GUI_LIB
QString XOptions::getTreeModelText(QAbstractItemModel *pModel)
{
    QString sResult;

    if (pModel) {
        _getTreeModelText(&sResult, pModel, QModelIndex(), 0);
    }

    return sResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTableModel(QAbstractItemModel *pModel, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sText = getTableModelText(pModel);

        file.resize(0);
        file.write(sText.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTreeModel(QAbstractItemModel *pModel, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sText = getTreeModelText(pModel);

        file.resize(0);
        file.write(sText.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTextEdit(QTextEdit *pTextEdit, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sResult = pTextEdit->toPlainText();

        file.resize(0);
        file.write(sResult.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::savePlainTextEdit(QPlainTextEdit *pPlainTextEdit, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sResult = pPlainTextEdit->toPlainText();

        file.resize(0);
        file.write(sResult.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTableView(QTableView *pTableView, const QString &sFileName)
{
    return saveTableModel(pTableView->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTableWidget(QTableWidget *pTableWidget, const QString &sFileName)
{
    return saveTableModel(pTableWidget->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveListView(QListView *pListView, const QString &sFileName)
{
    return saveTableModel(pListView->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveListWidget(QListWidget *pListWidget, const QString &sFileName)
{
    return saveTableModel(pListWidget->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTreeView(QTreeView *pTreeView, const QString &sFileName)
{
    return saveTreeModel(pTreeView->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTreeWidget(QTreeWidget *pTreeWidget, const QString &sFileName)
{
    return saveTreeModel(pTreeWidget->model(), sFileName);
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTextBrowser(QTextBrowser *pTextBrowser, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sResult = pTextBrowser->toPlainText();

        file.resize(0);
        file.write(sResult.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
bool XOptions::saveTextBrowserHtml(QTextBrowser *pTextBrowser, const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        QString sResult = pTextBrowser->toHtml();

        file.resize(0);
        file.write(sResult.toUtf8().data());
        file.close();

        bResult = true;
    }

    return bResult;
}
#endif
#ifdef QT_GUI_LIB
QMenu *XOptions::createRecentFilesMenu(QWidget *pParent)
{
    m_pRecentFilesMenu = new QMenu(tr("Recent files"), pParent);

    _updateRecentFilesMenu();

    return m_pRecentFilesMenu;
}
#endif
#ifdef QT_GUI_LIB
qint32 XOptions::getCharWidth(QWidget *pWidget)
{
    const QFontMetricsF fm(pWidget->font());

    return fm.maxWidth();
}
#endif
#ifdef QT_GUI_LIB
qint32 XOptions::getCharHeight(QWidget *pWidget)
{
    const QFontMetricsF fm(pWidget->font());

    return fm.height();
}
#endif
#ifdef QT_GUI_LIB
qint32 XOptions::getControlWidth(QWidget *pWidget, qint32 nSymbolSize)
{
    QFontMetrics fm(pWidget->font());

    QString sText = QString(nSymbolSize, '0') + QString(2, ' ');

    return fm.boundingRect(sText).width();
}
#endif
#ifdef QT_GUI_LIB
void XOptions::showInFolder(const QString &sFileName)
{
    // TODO: https://github.com/qt-creator/qt-creator/blob/master/src/plugins/coreplugin/fileutils.cpp#L67
    QFileInfo fi = QFileInfo(sFileName);

#if defined(Q_OS_WIN)
    QStringList slParams;
    if (!fi.isDir()) {
        slParams += QLatin1String("/select,");
    }

    slParams += QDir::toNativeSeparators(fi.canonicalFilePath());

    QProcess::startDetached("explorer.exe", slParams);
#elif defined(Q_OS_MAC)
    QStringList slParams;
    slParams << "-e";
    slParams << "tell application \"Finder\"";
    slParams << "-e";
    slParams << "activate";
    slParams << "-e";
    slParams << "select POSIX file \"" + fi.path() + "\"";
    slParams << "-e";
    slParams << "end tell";
    slParams << "-e";
    slParams << "return";
    QProcess::execute("/usr/bin/osascript", slParams);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(fi.path()));
#endif
}
#endif
#ifdef QT_GUI_LIB
void XOptions::showFolder(const QString &sDirectory)
{
#if defined(Q_OS_WIN)
    QStringList slParams;

    slParams += QDir::toNativeSeparators(sDirectory);

    QProcess::startDetached("explorer.exe", slParams);
#elif defined(Q_OS_MAC)
    QStringList slParams;
    slParams << "-e";
    slParams << "tell application \"Finder\"";
    slParams << "-e";
    slParams << "activate";
    slParams << "-e";
    slParams << "select POSIX file \"" + sDirectory + "\"";  // TODO: Check
    slParams << "-e";
    slParams << "end tell";
    slParams << "-e";
    slParams << "return";
    QProcess::execute("/usr/bin/osascript", slParams);
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(sDirectory));
#endif
}
#endif
#ifdef QT_GUI_LIB
void XOptions::handleFontButton(QWidget *pParent, QLineEdit *pLineEdit)
{
    QFont _font;
    _font.fromString(pLineEdit->text());

    bool bOK = false;
    _font = QFontDialog::getFont(&bOK, _font, pParent);

    if (bOK) {
        pLineEdit->setText(_font.toString());
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setModelTextAlignment(QStandardItemModel *pModel, qint32 nColumn, Qt::Alignment flag)
{
    qint32 nNumberOfRows = pModel->rowCount();

    pModel->setHeaderData(nColumn, Qt::Horizontal, (qint32)flag, Qt::TextAlignmentRole);

    for (qint32 i = 0; i < nNumberOfRows; i++) {
        QStandardItem *pItem = pModel->item(i, nColumn);

        if (pItem) {
            pItem->setTextAlignment(flag);
            // pModel->setData(pModel->index(i, nColumn), (qint32)flag, Qt::TextAlignmentRole);

            QModelIndex index = pModel->index(i, 0);
            qint32 _nNumberOfRows = pModel->rowCount(index);

            for (qint32 j = 0; j < _nNumberOfRows; j++) {
                pModel->setData(pModel->index(j, nColumn, index), (qint32)flag, Qt::TextAlignmentRole);
            }
        }
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setTableViewHeaderWidth(QTableView *pTableView, qint32 nColumn, qint32 nContentWidth)
{
    QFont font = pTableView->font();
    font.setBold(true);

    const QFontMetricsF fm(font);

    QString sTitle = pTableView->model()->headerData(nColumn, Qt::Horizontal, Qt::DisplayRole).toString();

    qreal rWidth = fm.boundingRect(sTitle + "  ").width();

    nContentWidth = qMax(nContentWidth, (qint32)(rWidth));

    pTableView->setColumnWidth(nColumn, nContentWidth);
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setTreeViewHeaderWidth(QTreeView *pTreeView, qint32 nColumn, qint32 nContentWidth)
{
    QFont font = pTreeView->font();
    font.setBold(true);

    const QFontMetricsF fm(font);

    QString sTitle = pTreeView->model()->headerData(nColumn, Qt::Horizontal, Qt::DisplayRole).toString();

    qreal rWidth = fm.boundingRect(sTitle + "  ").width();

    nContentWidth = qMax(nContentWidth, (qint32)(rWidth));

    pTreeView->setColumnWidth(nColumn, nContentWidth);
}
#endif
#ifdef QT_GUI_LIB
void XOptions::setTableWidgetHeaderAlignment(QTableWidget *pTableWidget, qint32 nColumn, Qt::Alignment flag)
{
    pTableWidget->model()->setHeaderData(nColumn, Qt::Horizontal, (qint32)flag, Qt::TextAlignmentRole);
}
#endif

void XOptions::deleteQObjectList(QList<QObject *> *pList)
{
    qint32 nNumberOfRecords = pList->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        delete pList->at(i);
    }
}

QList<QString> XOptions::getAllFilesFromDirectory(const QString &sDirectory, const QString &sExtension)
{
    QDir directory(sDirectory);

    return directory.entryList(QStringList() << sExtension, QDir::Files);
}

bool XOptions::checkNative(const QString &sIniFileName)
{
    Q_UNUSED(sIniFileName)

    return isNative();
}

QString XOptions::getApplicationDataPath()
{
    QString sResult;
    bool bResult = false;

    QString sApplicationDirPath = qApp->applicationDirPath();
    QString sApplicationName = qApp->applicationName();

    if (!bResult) {
        if (sApplicationDirPath.contains("/usr/local/bin")) {
            QString sPrefix = sApplicationDirPath.section("/usr/local/bin", 0, 0);

            sResult += sPrefix + QString("/usr/local/lib/%1").arg(sApplicationName);

            if (QDir(sResult).exists()) {
                bResult = true;
            }
        }
    }

    if (!bResult) {
        if (sApplicationDirPath.contains("/app/bin")) {
            QString sPrefix = sApplicationDirPath.section("/app/bin", 0, 0);

            sResult += sPrefix + QString("/app/lib/%1").arg(sApplicationName);

            if (QDir(sResult).exists()) {
                bResult = true;
            }
        }
    }

    if (!bResult) {
        if (sApplicationDirPath.contains("/tmp/.mount_")) {
            QString sPrefix = sApplicationDirPath.section("/", 0, 2);

            sResult += sPrefix + QString("/usr/lib/%1").arg(sApplicationName);

            if (QDir(sResult).exists()) {
                bResult = true;
            }
        }
    }

    if (!bResult) {
        sResult = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if (QDir(sResult).exists()) {
            bResult = true;
        }
    }

#ifdef Q_OS_MAC
    if (!bResult) {
        sResult = sApplicationDirPath + "/../Resources";
        if (QDir(sResult).exists()) {
            bResult = true;
        }
    }
#endif
#ifdef X_BUILD_APPIMAGE
    if (!bResult) {
        if (sApplicationDirPath.contains("/tmp/.mount_")) {
            QString sPrefix = sApplicationDirPath.section("/", 0, 2);

            sResult += sPrefix + QString("/usr/lib/%1").arg(sApplicationName);

            if (QDir(sResult).exists()) {
                bResult = true;
            }
        }
    }
#endif
#ifdef X_BUILD_FLATPACK
    if (!bResult) {
        sResult = QString("/app/lib/%1").arg(sApplicationName);

        if (QDir(sResult).exists()) {
            bResult = true;
        }
    }
#endif
#ifdef Q_OS_FREEBSD
    if (!bResult) {
        sResult = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).at(1) + QDir::separator() + sApplicationName;

        if (QDir(sResult).exists()) {
            bResult = true;
        }
    }
#endif

    // dataPathAlt0 .. dataPathAltN
    if (!bResult) {
        qint32 nIndex = 0;
        while (true) {
            QString sResult = qApp->property(QString("dataPathAlt%1").arg(nIndex).toUtf8().data()).toString();

            if (QDir(sResult).exists()) {
                bResult = true;
                break;
            }

            nIndex++;
        }
    }

    if (!bResult) {
        sResult = sApplicationDirPath;
    }

    if (!bResult) {
        sResult = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

        if (QDir().mkpath(sResult)) {
            bResult = true;
        } else {
            sResult = sApplicationDirPath;
            bResult = true;
        }
    }

    return sResult;
}

QString XOptions::convertPathName(const QString &sPathName)
{
    QString sResult = sPathName;

    if (sPathName.contains("$data")) {
        bool bSuccess = false;
        QString _sPathName;
        QString sApplicationDirPath = qApp->applicationDirPath();
        QString sApplicationName = qApp->applicationName();

#ifdef Q_OS_MAC
        if (!bSuccess) {
            _sPathName = sPathName;
            QString _sApplicationDirPath = sApplicationDirPath + "/../Resources";

            _sPathName = _sPathName.replace("$data", _sApplicationDirPath);

            bSuccess = isPathExists(_sPathName);
        }
#endif

        if (!bSuccess) {
            _sPathName = sPathName;
            _sPathName = _sPathName.replace("$data", sApplicationDirPath);

            bSuccess = isPathExists(_sPathName);
        }

        if (!bSuccess) {
            _sPathName = sPathName;
            QString sAppData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

            _sPathName = _sPathName.replace("$data", sAppData);

            bSuccess = isPathExists(_sPathName);
        }

        if (!bSuccess) {
            if (sApplicationDirPath.contains("/usr/local/bin")) {
                _sPathName = sPathName;
                QString sPrefix = sApplicationDirPath.section("/usr/local/bin", 0, 0);
                QString sPath = sPrefix + QString("/usr/local/lib/%1").arg(sApplicationName);

                _sPathName = _sPathName.replace("$data", sPath);

                bSuccess = isPathExists(_sPathName);
            }
        }

        if (!bSuccess) {
            if (sApplicationDirPath.contains("/app/bin")) {
                _sPathName = sPathName;
                QString sPrefix = sApplicationDirPath.section("/app/bin", 0, 0);
                QString sPath = sPrefix + QString("/app/lib/%1").arg(sApplicationName);

                _sPathName = _sPathName.replace("$data", sPath);

                bSuccess = isPathExists(_sPathName);
            }
        }

        if (!bSuccess) {
            if (sApplicationDirPath.contains("/tmp/.mount_")) {
                _sPathName = sPathName;
                QString sPrefix = sApplicationDirPath.section("/", 0, 2);

                QString sPath = sPrefix + QString("/app/lib/%1").arg(sApplicationName);

                _sPathName = _sPathName.replace("$data", sPath);

                bSuccess = isPathExists(_sPathName);
            }
        }

        if (!bSuccess) {
            for (qint32 nIndex = 0; nIndex < 10; nIndex++) {
                _sPathName = sPathName;
                QString sPath = qApp->property(QString("dataPathAlt%1").arg(nIndex).toUtf8().data()).toString();

                _sPathName = _sPathName.replace("$data", sPath);

                if (QDir(sResult).exists()) {
                    bSuccess = true;
                    break;
                }
            }
        }

        if (!bSuccess) {
             _sPathName = sPathName;
            QString sPath = QString("/usr/local/lib/%1").arg(sApplicationName);

            _sPathName = _sPathName.replace("$data", sPath);

            bSuccess = isPathExists(_sPathName);
        }

        if (!bSuccess) {
             _sPathName = sPathName;
            QString sPath = QString("/usr/lib/%1").arg(sApplicationName);

            _sPathName = _sPathName.replace("$data", sPath);

            bSuccess = isPathExists(_sPathName);
        }

        if (bSuccess) {
            sResult = _sPathName;
        }
    }

    // QString sApplicationDirPath = qApp->applicationDirPath();
    // QString sApplicationName = qApp->applicationName();

    return sResult;
}

bool XOptions::isPathExists(const QString &sPathName)
{
    QFileInfo fileInfo(sPathName);

    if (fileInfo.isDir()) {
        return QDir(sPathName).exists();
    } else {
        return QFile(sPathName).exists();
    }
}

QString XOptions::getTitle(const QString &sName, const QString &sVersion, bool bShowOS)
{
    QString sResult = QString("%1 v%2").arg(sName, sVersion);

    if (bShowOS) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
        // TODO: Check Windows 11 (DiE currently detectes Windows 11 as Windows 10)
        QString architecture = QSysInfo::buildCpuArchitecture();
        if (architecture == "x86_64" || architecture == "amd64") {
            architecture = "x64";
        } else if (architecture == "arm32" || architecture == "armhf" || architecture == "aarch32") {
            architecture = "ARM32";
        } else if (architecture == "arm64" || architecture == "aarch64") {
            architecture = "ARM64";
        } else if (architecture == "i386" || architecture == "i686") {
            architecture = "x86";
        }
        sResult += QString(" [%1] (%2)").arg(QSysInfo::prettyProductName(), architecture);
#else
        // TODO: OS Name // For Windows Arch GetVersionExA
#endif
    }

    return sResult;
}

bool XOptions::isWritable()
{
    bool bResult = false;
    QSettings *pSettings = nullptr;

    bool bIsNative = isNative();

    if (bIsNative) {
        pSettings = new QSettings;
    } else {
        pSettings = new QSettings(qApp->applicationDirPath() + QDir::separator() + QString("%1").arg(m_sName), QSettings::IniFormat);
    }

    bResult = pSettings->isWritable();

    delete pSettings;

    return bResult;
}

#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)

bool sort_code_page(const qint32 &nValue1, const qint32 &nValue2)
{
    return (qAbs(nValue1) < qAbs(nValue2));
}

QList<QString> XOptions::getCodePages(bool bAll)
{
    QList<QString> listResult;

    listResult.append("");  // Empty

    QList<qint32> list = QTextCodec::availableMibs();

    std::sort(list.begin(), list.end(), sort_code_page);

    qint32 nNumberOfRecords = list.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        qint32 nMIB = list.at(i);

        //        qDebug("%s",QTextCodec::codecForMib(nMIB)->name().data());

        bool bAdd = true;

        if (!bAll) {
            if (nMIB == 106) bAdd = false;                            // UTF8
            else if ((nMIB >= 1013) && (nMIB <= 1019)) bAdd = false;  // Unicode
        }

        if (bAdd) {
            QString sName = QTextCodec::codecForMib(nMIB)->name();

            listResult.append(sName);
        }
    }

    return listResult;
}
#endif
void XOptions::registerCodecs()
{
#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
    {
        codec_cp437 *pCodec = new codec_cp437;

        if (!pCodec) {
            qFatal("Codec failed");
        }
        // TODO: more codecs

        // delete pCodec; // TODO: unregisterCodecs
    }
#endif
}

Qt::GlobalColor XOptions::hexToGlobalColor(const QString &sHex)
{
    if (sHex.isEmpty() || !sHex.startsWith("#") || sHex.length() != 7) {
        return Qt::color0;
    }

    bool bOk;
    qint32 nR = sHex.mid(1, 2).toInt(&bOk, 16);

    if (!bOk) return Qt::color0;

    qint32 nG = sHex.mid(3, 2).toInt(&bOk, 16);

    if (!bOk) return Qt::color0;

    qint32 nB = sHex.mid(5, 2).toInt(&bOk, 16);

    if (!bOk) return Qt::color0;

    struct ColorEntry {
        qint32 nR;
        qint32 nG;
        qint32 nB;
        Qt::GlobalColor color;
    };

    static const ColorEntry table[] = {
        {0, 0, 0, Qt::black},       {255, 255, 255, Qt::white},  {128, 128, 128, Qt::darkGray},  {160, 160, 164, Qt::gray},
        {255, 0, 0, Qt::red},       {0, 255, 0, Qt::green},      {0, 0, 255, Qt::blue},          {0, 255, 255, Qt::cyan},
        {255, 0, 255, Qt::magenta}, {255, 255, 0, Qt::yellow},   {128, 0, 0, Qt::darkRed},       {0, 128, 0, Qt::darkGreen},
        {0, 0, 128, Qt::darkBlue},  {0, 128, 128, Qt::darkCyan}, {128, 0, 128, Qt::darkMagenta}, {128, 128, 0, Qt::darkYellow},
    };

    Qt::GlobalColor bestColor = Qt::black;
    qint32 nBestDist = 0x7FFFFFFF;
    qint32 nCount = sizeof(table) / sizeof(ColorEntry);

    for (qint32 i = 0; i < nCount; i++) {
        qint32 nDR = nR - table[i].nR;
        qint32 nDG = nG - table[i].nG;
        qint32 nDB = nB - table[i].nB;
        qint32 nDist = nDR * nDR + nDG * nDG + nDB * nDB;

        if (nDist < nBestDist) {
            nBestDist = nDist;
            bestColor = table[i].color;
        }
    }

    return bestColor;
}

void XOptions::printConsole(const QString &sString, const QString &sColorText, const QString &sColorBackground)
{
    Qt::GlobalColor colorText = hexToGlobalColor(sColorText);
    Qt::GlobalColor colorBackground = hexToGlobalColor(sColorBackground);

    bool bEscapeMode = false;
    bool bNativeMode = false;

#ifdef Q_OS_WIN
    HANDLE hConsole = 0;
    WORD wOldAttribute = 0;
    DWORD dwMode = 0;

    if (colorText != Qt::transparent || colorBackground != Qt::transparent) {
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        if (hConsole) {
            GetConsoleMode(hConsole, &dwMode);
#ifndef _USING_V110_SDK71_
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
            if (SetConsoleMode(hConsole, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)) {
                bEscapeMode = true;
            } else {
                bNativeMode = true;
            }
#else
            bNativeMode = true;
#endif
        } else {
#ifdef QT_DEBUG
            qWarning("GetStdHandle(STD_OUTPUT_HANDLE) failed");
#endif
        }
    }
#else
    if (colorText != Qt::transparent || colorBackground != Qt::transparent) {
        bEscapeMode = true;
    }
#endif

    if (bEscapeMode) {
        if (colorText != Qt::transparent || colorBackground != Qt::transparent) {
            // Foreground
            int fg = 39, bg = 49;  // Default
            // Map Qt::GlobalColor to ANSI codes
            switch (colorText) {
                case Qt::black: fg = 30; break;
                case Qt::red: fg = 31; break;
                case Qt::green: fg = 32; break;
                case Qt::yellow: fg = 33; break;
                case Qt::blue: fg = 34; break;
                case Qt::magenta: fg = 35; break;
                case Qt::cyan: fg = 36; break;
                case Qt::gray: fg = 90; break;
                case Qt::white: fg = 37; break;
                case Qt::darkRed: fg = 91; break;
                case Qt::darkGreen: fg = 92; break;
                case Qt::darkYellow: fg = 93; break;
                case Qt::darkBlue: fg = 94; break;
                case Qt::darkMagenta: fg = 95; break;
                case Qt::darkCyan: fg = 96; break;
                case Qt::darkGray: fg = 90; break;  // Or 90 as gray
                default: fg = 39; break;
            }
            switch (colorBackground) {
                case Qt::black: bg = 40; break;
                case Qt::red: bg = 41; break;
                case Qt::green: bg = 42; break;
                case Qt::yellow: bg = 43; break;
                case Qt::blue: bg = 44; break;
                case Qt::magenta: bg = 45; break;
                case Qt::cyan: bg = 46; break;
                case Qt::gray: bg = 100; break;
                case Qt::white: bg = 47; break;
                case Qt::darkRed: bg = 101; break;
                case Qt::darkGreen: bg = 102; break;
                case Qt::darkYellow: bg = 103; break;
                case Qt::darkBlue: bg = 104; break;
                case Qt::darkMagenta: bg = 105; break;
                case Qt::darkCyan: bg = 106; break;
                case Qt::darkGray: bg = 100; break;
                default: bg = 49; break;
            }
            printf("\033[%d;%dm", fg, bg);
        }
    } else if (bNativeMode) {
#ifdef Q_OS_WIN
        CONSOLE_SCREEN_BUFFER_INFO csbi = {};

        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            wOldAttribute = csbi.wAttributes;
        }

        WORD wAttribute = 0;

        // Foreground (text) color
        if (colorText == Qt::black) {
            wAttribute |= 0;
        } else if (colorText == Qt::white) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        } else if (colorText == Qt::blue) {
            wAttribute |= FOREGROUND_BLUE;
        } else if (colorText == Qt::red) {
            wAttribute |= FOREGROUND_RED;
        } else if (colorText == Qt::green) {
            wAttribute |= FOREGROUND_GREEN;
        } else if (colorText == Qt::yellow) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_GREEN;
        } else if (colorText == Qt::magenta) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_BLUE;
        } else if (colorText == Qt::cyan) {
            wAttribute |= FOREGROUND_GREEN | FOREGROUND_BLUE;
        } else if (colorText == Qt::darkBlue) {
            wAttribute |= FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkRed) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkGreen) {
            wAttribute |= FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkYellow) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkMagenta) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkCyan) {
            wAttribute |= FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::gray) {
            wAttribute |= FOREGROUND_INTENSITY;
        } else if (colorText == Qt::darkGray) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        } else if (colorText == Qt::transparent) {
            wAttribute |= FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
        }

        // Background color
        if (colorBackground == Qt::black) {
            wAttribute |= 0;
        } else if (colorBackground == Qt::white) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;
        } else if (colorBackground == Qt::blue) {
            wAttribute |= BACKGROUND_BLUE;
        } else if (colorBackground == Qt::red) {
            wAttribute |= BACKGROUND_RED;
        } else if (colorBackground == Qt::green) {
            wAttribute |= BACKGROUND_GREEN;
        } else if (colorBackground == Qt::yellow) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_GREEN;
        } else if (colorBackground == Qt::magenta) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_BLUE;
        } else if (colorBackground == Qt::cyan) {
            wAttribute |= BACKGROUND_GREEN | BACKGROUND_BLUE;
        } else if (colorBackground == Qt::darkBlue) {
            wAttribute |= BACKGROUND_BLUE | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkRed) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkGreen) {
            wAttribute |= BACKGROUND_GREEN | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkYellow) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkMagenta) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkCyan) {
            wAttribute |= BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::gray) {
            wAttribute |= BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::darkGray) {
            wAttribute |= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
        } else if (colorBackground == Qt::transparent) {
            // No background bits set (leave as is)
        }

        if (wAttribute) {
            SetConsoleTextAttribute(hConsole, wAttribute);
        }
#endif
    }

    printf("%s", sString.toUtf8().data());

    if (bEscapeMode) {
        if (colorText != Qt::transparent || colorBackground != Qt::transparent) {
            printf("\033[0m");
        }
#ifdef Q_OS_WIN
        SetConsoleMode(hConsole, dwMode);  // Restore original console mode TODO: optimize
#endif
    } else if (bNativeMode) {
#ifdef Q_OS_WIN
        if (colorText != Qt::transparent || colorBackground != Qt::transparent) {
            if (wOldAttribute) {
                SetConsoleTextAttribute(hConsole, wOldAttribute);
            }
        }
#endif
    }
}

void XOptions::printModel(QAbstractItemModel *pModel)
{
    if (pModel) {
        qint32 nNumberOfRows = pModel->rowCount();
        qint32 nNumberOfColumns = pModel->columnCount();

        QList<qint32> listColumnSymbolSize;
        listColumnSymbolSize.reserve(nNumberOfColumns);
        QChar charSpace(' ');

        for (qint32 i = 0; i < nNumberOfColumns; i++) {
            qint32 nSymbolSize = 0;
            nSymbolSize = qMax(nSymbolSize, pModel->headerData(i, Qt::Horizontal).toString().length());

            for (qint32 j = 0; j < nNumberOfRows; j++) {
                QModelIndex index = pModel->index(j, i);
                QString sData = pModel->data(index, Qt::DisplayRole).toString();

                nSymbolSize = qMax(nSymbolSize, sData.length());
            }

            listColumnSymbolSize.append(nSymbolSize);
        }

        QString sTableLine;

        {
            for (qint32 i = 0; i < nNumberOfColumns; i++) {
                sTableLine += "+";

                for (int j = 0; j < listColumnSymbolSize[i]; j++) {
                    sTableLine += "-";
                }
            }

            sTableLine += "+\n";
        }

        {
            printConsole(sTableLine);

            for (qint32 i = 0; i < nNumberOfColumns; i++) {
                printConsole("|");
                QString sString = pModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
                qint32 nColumnSize = listColumnSymbolSize[i];
                QString sEmpty = QString(nColumnSize - sString.size(), ' ');

                Qt::AlignmentFlag flag = (Qt::AlignmentFlag)(pModel->headerData(i, Qt::Horizontal, Qt::TextAlignmentRole).toInt());

                if (flag & Qt::AlignRight) {
                    sString.prepend(sEmpty);
                } else {
                    sString.append(sEmpty);
                }

                printConsole(sString, "#ff0000");

                if (i == (nNumberOfColumns - 1)) {
                    printConsole("|\n");
                }
            }

            printConsole(sTableLine);
        }

        {
            for (qint32 i = 0; i < nNumberOfRows; i++) {
                for (qint32 j = 0; j < nNumberOfColumns; j++) {
                    printConsole("|");

                    QModelIndex index = pModel->index(i, j);
                    QString sString = pModel->data(index, Qt::DisplayRole).toString();

                    qint32 nColumnSize = listColumnSymbolSize[j];
                    QString sEmpty = QString(nColumnSize - sString.size(), ' ');

                    Qt::AlignmentFlag flag = (Qt::AlignmentFlag)(pModel->data(index, Qt::TextAlignmentRole).toInt());

                    if (flag & Qt::AlignRight) {
                        sString.prepend(sEmpty);
                    } else {
                        sString.append(sEmpty);
                    }

                    printConsole(sString);

                    if (j == (nNumberOfColumns - 1)) {
                        printConsole("|\n");
                    }
                }
            }

            printConsole(sTableLine);
        }
    }
}

#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
#ifdef QT_GUI_LIB
QMenu *XOptions::createCodePagesMenu(QWidget *pParent, bool bAll)
{
    m_pCodePagesMenu = new QMenu(tr("Code pages"), pParent);

    if (m_pCodePagesMenu) {
        m_pCodePagesMenu->clear();  // TODO: Check

        QList<QString> listCodePages = getCodePages(bAll);

        qint32 nNumberOfRecords = listCodePages.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QAction *pAction = new QAction(listCodePages.at(i), m_pCodePagesMenu);
            pAction->setData(listCodePages.at(i));

            connect(pAction, SIGNAL(triggered()), this, SLOT(setCodePageSlot()));

            m_pCodePagesMenu->addAction(pAction);
        }
    }

    return m_pCodePagesMenu;
}
#endif
#endif
#ifdef Q_OS_WIN
QString XOptions::getClassesPrefix(USERROLE userRole)
{
    QString sResult;

    if (userRole == USERROLE_ADMIN) {
        sResult = "HKEY_CLASSES_ROOT";
    } else if (userRole == USERROLE_NORMAL || userRole == USERROLE_GUEST || userRole == USERROLE_CHILD) {
        sResult = "HKEY_CURRENT_USER\\Software\\Classes";
    }

    return sResult;
}
#endif
#ifdef Q_OS_WIN
bool XOptions::isPathInUserEnvironment(const QString &checkPath)
{
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString currentPath = settings.value("Path").toString();

    QString formattedPath = QDir::toNativeSeparators(checkPath);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList pathEntries = currentPath.split(';', Qt::SkipEmptyParts);
#else
    QStringList pathEntries = currentPath.split(';', QString::SkipEmptyParts);
#endif

    return pathEntries.contains(formattedPath, Qt::CaseInsensitive);
}
#endif
#ifdef Q_OS_WIN
void XOptions::appendToUserPathVariable(const QString &newPath)
{
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString currentPath = settings.value("Path").toString();

    QString formattedPath = QDir::toNativeSeparators(newPath);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList pathEntries = currentPath.split(';', Qt::SkipEmptyParts);
#else
    QStringList pathEntries = currentPath.split(';', QString::SkipEmptyParts);
#endif

    if (!pathEntries.contains(formattedPath, Qt::CaseInsensitive)) {
        pathEntries.append(formattedPath);
        settings.setValue("Path", pathEntries.join(';'));

#ifdef QT_GUI_LIB
// SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);
#endif
    }
}
#endif
#ifdef Q_OS_WIN
void XOptions::removeFromUserPathVariable(const QString &targetPath)
{
    QSettings settings("HKEY_CURRENT_USER\\Environment", QSettings::NativeFormat);
    QString currentPath = settings.value("Path").toString();

    QString formattedPath = QDir::toNativeSeparators(targetPath);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QStringList pathEntries = currentPath.split(';', Qt::SkipEmptyParts);
#else
    QStringList pathEntries = currentPath.split(';', QString::SkipEmptyParts);
#endif

    if (pathEntries.contains(formattedPath, Qt::CaseInsensitive)) {
        pathEntries.removeAll(formattedPath);
        settings.setValue("Path", pathEntries.join(';'));

#ifdef QT_GUI_LIB
// SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"Environment", SMTO_ABORTIFHUNG, 5000, nullptr);
#endif
    }
}
#endif

#ifdef Q_OS_WIN
bool XOptions::registerContext(const QString &sApplicationName, const QString &sType, const QString &sApplicationFilePath, USERROLE userRole)
{
    QString _sApplicationFilePath = sApplicationFilePath;

    QSettings settings(getClassesPrefix(userRole) + QString("\\%1\\shell\\%2\\command").arg(sType, sApplicationName), QSettings::NativeFormat);
    settings.setValue(".", "\"" + _sApplicationFilePath.replace("/", "\\") + "\" \"%1\"");

    QSettings settingsIcon(getClassesPrefix(userRole) + QString("\\%1\\shell\\%2").arg(sType, sApplicationName), QSettings::NativeFormat);
    settingsIcon.setValue("Icon", "\"" + _sApplicationFilePath.replace("/", "\\") + "\"");

    // TODO: Check if not send message
    return checkContext(sApplicationName, sType);
}
#endif
#ifdef Q_OS_WIN
bool XOptions::clearContext(const QString &sApplicationName, const QString &sType, USERROLE userRole)
{
    QSettings settings(getClassesPrefix(userRole) + QString("\\%1\\shell\\%2").arg(sType, sApplicationName), QSettings::NativeFormat);
    settings.clear();

    // TODO: Check if not send message
    return !(checkContext(sApplicationName, sType));
}
#endif
#ifdef Q_OS_WIN
bool XOptions::checkContext(const QString &sApplicationName, const QString &sType, USERROLE userRole)
{
    QSettings settings(getClassesPrefix(userRole) + QString("\\%1\\shell").arg(sType), QSettings::NativeFormat);

    return (settings.value(QString("%1/command/Default").arg(sApplicationName)).toString() != "");
}
#endif

void XOptions::setMaxRecentFilesCount(qint32 nValue)
{
    m_nMaxRecentFilesCount = nValue;
}

qint32 XOptions::getMaxRecentFilesCount()
{
    return m_nMaxRecentFilesCount;
}

QString XOptions::getBundleIdToString(BUNDLE bundle)
{
    QString sResult;

    if (bundle == BUNDLE_LINUX_ARCH_X64) {
        sResult = "Linux Arch x64";
    } else if (bundle == BUNDLE_WINDOWS_QT6_X64) {
        sResult = "Windows Qt6 x64";
    } else if (bundle == BUNDLE_LINUX_APPIMAGE_X64) {
        sResult = "Linux AppImage x64";
    } else if (bundle == BUNDLE_LINUX_DEBIAN_X64) {
        sResult = "Linux Debian x64";
    } else if (bundle == BUNDLE_LINUX_DEBIAN_X86) {
        sResult = "Linux Debian x86";
    } else if (bundle == BUNDLE_LINUX_DEBIAN_ARM64) {
        sResult = "Linux Debian ARM64";
    } else if (bundle == BUNDLE_LINUX_UBUNTU_X64) {
        sResult = "Linux Ubuntu x64";
    } else if (bundle == BUNDLE_LINUX_PARROT_X64) {
        sResult = "Linux Parrot x64";
    } else if (bundle == BUNDLE_LINUX_KALI_X64) {
        sResult = "Linux Kali x64";
    } else if (bundle == BUNDLE_WINDOWS_XP_X86) {
        sResult = "Windows XP x86";
    } else if (bundle == BUNDLE_WINDOWS_X86) {
        sResult = "Windows x86";
    } else if (bundle == BUNDLE_WINDOWS_X64) {
        sResult = "Windows x64";
    } else if (bundle == BUNDLE_WINDOWS_ARM64) {
        sResult = "Windows ARM64";
    } else if (bundle == BUNDLE_MACOS_X64) {
        sResult = "MacOS x64";
    } else if (bundle == BUNDLE_MACOS_QT6_ARM64) {
        sResult = "MacOS Qt6 ARM64";
    } else if (bundle == BUNDLE_FEDORA_X64) {
        sResult = "Fedora x64";
    } else if (bundle == BUNDLE_FREEBSD_X64) {
        sResult = "FreeBSD x64";
    } else {
        sResult = tr("Unknown");
    }

    return sResult;
}

#ifdef QT_GUI_LIB
void XOptions::_updateRecentFilesMenu()
{
    if (m_pRecentFilesMenu) {
        m_pRecentFilesMenu->clear();

        QList<QString> listRecentFiles = getRecentFiles();

        qint32 nNumberOfRecentFiles = listRecentFiles.count();

        for (qint32 i = nNumberOfRecentFiles - 1; i >= 0; i--) {
            QAction *pAction = new QAction(listRecentFiles.at(i), m_pRecentFilesMenu);
            pAction->setData(listRecentFiles.at(i));

            connect(pAction, SIGNAL(triggered()), this, SLOT(openRecentFile()));

            m_pRecentFilesMenu->addAction(pAction);
        }

        if (nNumberOfRecentFiles) {
            m_pRecentFilesMenu->addSeparator();

            QAction *pAction = new QAction(tr("Clear"), m_pRecentFilesMenu);

            connect(pAction, SIGNAL(triggered()), this, SLOT(clearRecentFiles()));

            m_pRecentFilesMenu->addAction(pAction);
        }

        m_pRecentFilesMenu->setEnabled(nNumberOfRecentFiles);
    }
}
#endif
#ifdef QT_GUI_LIB
QString XOptions::getImageFilter()
{
    const QList<QByteArray> listImageFormats = QImageWriter::supportedImageFormats();

    QStringList listFilter;

    qint32 nNumberOfImageFormats = listImageFormats.count();

    if (nNumberOfImageFormats) {
        QString sImageFilter = tr("Images") + " (";

        for (qint32 i = 0; i < nNumberOfImageFormats; i++) {
            if (i > 0) {
                sImageFilter += " ";
            }

            sImageFilter += "*.";
            sImageFilter += listImageFormats.at(i);
        }

        sImageFilter += ")";

        listFilter.append(sImageFilter);
    }

    listFilter.append(QString("PDF %1 (*.pdf)").arg(tr("Documents")));
    //    listFilter.append(QString("Postscript %1 (*.ps)").arg(tr("Documents")));

    return listFilter.join(";;");
}
#endif
#ifdef QT_GUI_LIB
QColor XOptions::getColorDialog(QWidget *pParent, const QString &sTitle, QColor &color)
{
    QColor colResult;

    QColorDialog colorDialog(pParent);
    colorDialog.setCurrentColor(color);
    colorDialog.setWindowTitle(sTitle);

    _adjustApplicationModal(&colorDialog, true);
    _adjustStayOnTop(&colorDialog, true);

    if (colorDialog.exec() == QDialog::Accepted) {
        colResult = colorDialog.currentColor();
    }

    return colResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustToolButton(QToolButton *pToolButton, ICONTYPE iconType, Qt::ToolButtonStyle style)
{
    QString sIconName = getIconPath(iconType);

    if (sIconName != "") {
        QIcon icon;
        icon.addFile(sIconName, QSize(), QIcon::Normal, QIcon::Off);
        pToolButton->setIcon(icon);
        pToolButton->setIconSize(QSize(16, 16));
        pToolButton->setToolButtonStyle(style);
    } else {
#ifdef QT_DEBUG
        qDebug("Icon not found: %s", sIconName.toUtf8().data());
#endif
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustTreeWidgetItem(QTreeWidgetItem *pTreeWidgetItem, ICONTYPE iconType)
{
    QString sIconName = getIconPath(iconType);

    if (sIconName != "") {
        QIcon icon;
        icon.addFile(sIconName, QSize(), QIcon::Normal, QIcon::Off);
        pTreeWidgetItem->setIcon(0, icon);
    } else {
#ifdef QT_DEBUG
        qDebug("Icon not found: %s", sIconName.toUtf8().data());
#endif
    }
}
#endif
#ifdef QT_GUI_LIB
QString XOptions::getIconPath(ICONTYPE iconType)
{
    QString sResult;

    if (iconType == ICONTYPE_NONE) {
        sResult = "";
    } else if (iconType == ICONTYPE_GENERIC) {
        sResult = ":/XStyles/icons/BreakpointEnabled.16.16.png";
    } else if (iconType == ICONTYPE_ACTION) {
        sResult = ":/XStyles/icons/Action.16.16.png";
    } else if (iconType == ICONTYPE_HEX) {
        sResult = ":/XStyles/icons/Binary.16.16.png";
    } else if (iconType == ICONTYPE_DISASM) {
        sResult = ":/XStyles/icons/Disasm.16.16.png";
    } else if (iconType == ICONTYPE_ENTROPY) {
        sResult = ":/XStyles/icons/Entropy.16.16.png";
    } else if (iconType == ICONTYPE_STRING) {
        sResult = ":/XStyles/icons/String.16.16.png";
    } else if (iconType == ICONTYPE_SIGNATURE) {
        sResult = ":/XStyles/icons/Signature.16.16.png";
    } else if (iconType == ICONTYPE_SIZE) {
        sResult = ":/XStyles/icons/Size.16.16.png";
    } else if (iconType == ICONTYPE_VALUE) {
        sResult = ":/XStyles/icons/Value.16.16.png";
    } else if (iconType == ICONTYPE_MEMORYMAP) {
        sResult = ":/XStyles/icons/MemoryMap.16.16.png";
    } else if (iconType == ICONTYPE_INFO) {
        sResult = ":/XStyles/icons/Info.16.16.png";
    } else if (iconType == ICONTYPE_HASH) {
        sResult = ":/XStyles/icons/Hash.16.16.png";
    } else if (iconType == ICONTYPE_VISUALIZATION) {
        sResult = ":/XStyles/icons/Image.16.16.png";
    } else if (iconType == ICONTYPE_SEARCH) {
        sResult = ":/XStyles/icons/Search.16.16.png";
    } else if (iconType == ICONTYPE_EXTRACTOR) {
        sResult = ":/XStyles/icons/Extract.16.16.png";
    } else if (iconType == ICONTYPE_FILE) {
        sResult = ":/XStyles/icons/File.16.16.png";
    } else if (iconType == ICONTYPE_SAVE) {
        sResult = ":/XStyles/icons/Save.16.16.png";
    } else if (iconType == ICONTYPE_COPY) {
        sResult = ":/XStyles/icons/Copy.16.16.png";
    } else if (iconType == ICONTYPE_EDIT) {
        sResult = ":/XStyles/icons/Edit.16.16.png";
    } else if (iconType == ICONTYPE_OVERLAY) {
        sResult = ":/XStyles/icons/Overlay.16.16.png";
    } else if (iconType == ICONTYPE_RELOAD) {
        sResult = ":/XStyles/icons/Refresh.16.16.png";
    } else if (iconType == ICONTYPE_SCAN) {
        sResult = ":/XStyles/icons/Search.16.16.png";
    } else if (iconType == ICONTYPE_DUMPTOFILE) {
        sResult = ":/XStyles/icons/Download.16.16.png";
    } else if (iconType == ICONTYPE_ENTRY) {
        sResult = ":/XStyles/icons/Entry.16.16.png";
    } else if (iconType == ICONTYPE_BACKWARD) {
        sResult = ":/XStyles/icons/Backward.16.16.png";
    } else if (iconType == ICONTYPE_FORWARD) {
        sResult = ":/XStyles/icons/Forward.16.16.png";
    } else if (iconType == ICONTYPE_ADD) {
        sResult = ":/XStyles/icons/Add.16.16.png";
    } else if (iconType == ICONTYPE_OPEN) {
        sResult = ":/XStyles/icons/Open.16.16.png";
    } else if (iconType == ICONTYPE_LIST) {
        sResult = ":/XStyles/icons/List.16.16.png";
    } else if (iconType == ICONTYPE_NEW) {
        sResult = ":/XStyles/icons/Add.16.16.png";
    } else if (iconType == ICONTYPE_OPTION) {
        sResult = ":/XStyles/icons/Option.16.16.png";
    } else if (iconType == ICONTYPE_YARA) {
        sResult = ":/XStyles/icons/Yara.16.16.png";
    } else if (iconType == ICONTYPE_MIME) {
        sResult = ":/XStyles/icons/Mime.16.16.png";
    } else if (iconType == ICONTYPE_VIRUSTOTAL) {
        sResult = ":/XStyles/icons/Virustotal.16.16.png";
    } else if (iconType == ICONTYPE_TOOL) {
        sResult = ":/XStyles/icons/Tool.16.16.png";
    } else if (iconType == ICONTYPE_EXIT) {
        sResult = ":/XStyles/icons/Exit.16.16.png";
    } else if (iconType == ICONTYPE_DEMANGLE) {
        sResult = ":/XStyles/icons/Demangle.16.16.png";
    } else if (iconType == ICONTYPE_SHORTCUT) {
        sResult = ":/XStyles/icons/Shortcut.16.16.png";
    } else if (iconType == ICONTYPE_GOTO) {
        sResult = ":/XStyles/icons/Goto.16.16.png";
    } else if (iconType == ICONTYPE_SECTION) {
        sResult = ":/XStyles/icons/Section.16.16.png";
    } else if (iconType == ICONTYPE_SEGMENT) {
        sResult = ":/XStyles/icons/Segment.16.16.png";
    } else if (iconType == ICONTYPE_EXCEPTION) {
        sResult = ":/XStyles/icons/Exception.16.16.png";
    } else if (iconType == ICONTYPE_CERTIFICATE) {
        sResult = ":/XStyles/icons/Certificate.16.16.png";
    } else if (iconType == ICONTYPE_RELOC) {
        sResult = ":/XStyles/icons/Reloc.16.16.png";
    } else if (iconType == ICONTYPE_DEBUG) {
        sResult = ":/XStyles/icons/Debug.16.16.png";
    } else if (iconType == ICONTYPE_HEADER) {
        sResult = ":/XStyles/icons/Header.16.16.png";
    } else if (iconType == ICONTYPE_LIBRARY) {
        sResult = ":/XStyles/icons/Library.16.16.png";
    } else if (iconType == ICONTYPE_SYMBOL) {
        sResult = ":/XStyles/icons/Symbol.16.16.png";
    } else if (iconType == ICONTYPE_TABLE) {
        sResult = ":/XStyles/icons/Table.16.16.png";
    } else if (iconType == ICONTYPE_DOTNET) {
        sResult = ":/XStyles/icons/DotNet.16.16.png";
    } else if (iconType == ICONTYPE_METADATA) {
        sResult = ":/XStyles/icons/Metadata.16.16.png";
    } else if (iconType == ICONTYPE_RESOURCE) {
        sResult = ":/XStyles/icons/Resource.16.16.png";
    } else if (iconType == ICONTYPE_TLS) {
        sResult = ":/XStyles/icons/TLS.16.16.png";
    } else if (iconType == ICONTYPE_SELECT) {
        sResult = ":/XStyles/icons/Select.16.16.png";
    } else if (iconType == ICONTYPE_ADDRESS) {
        sResult = ":/XStyles/icons/Address.16.16.png";
    } else if (iconType == ICONTYPE_OFFSET) {
        sResult = ":/XStyles/icons/Offset.16.16.png";
    } else if (iconType == ICONTYPE_IMPORT) {
        sResult = ":/XStyles/icons/Import.16.16.png";
    } else if (iconType == ICONTYPE_EXPORT) {
        sResult = ":/XStyles/icons/Export.16.16.png";
    } else if (iconType == ICONTYPE_DATA) {
        sResult = ":/XStyles/icons/Data.16.16.png";
    } else if (iconType == ICONTYPE_DIE) {
        sResult = ":/XStyles/icons/DIE.16.16.png";
    } else if (iconType == ICONTYPE_NFD) {
        sResult = ":/XStyles/icons/NFD.16.16.png";
    } else if (iconType == ICONTYPE_VERSION) {
        sResult = ":/XStyles/icons/Version.16.16.png";
    } else if (iconType == ICONTYPE_MANIFEST) {
        sResult = ":/XStyles/icons/Manifest.16.16.png";
    } else if (iconType == ICONTYPE_FOLLOW) {
        sResult = ":/XStyles/icons/Follow.16.16.png";
    } else if (iconType == ICONTYPE_NEXT) {
        sResult = ":/XStyles/icons/Next.16.16.png";
    } else if (iconType == ICONTYPE_ALL) {
        sResult = ":/XStyles/icons/All.16.16.png";
    } else if (iconType == ICONTYPE_PATH) {
        sResult = ":/XStyles/icons/Path.16.16.png";
    } else if (iconType == ICONTYPE_NOTE) {
        sResult = ":/XStyles/icons/Note.16.16.png";
    } else if (iconType == ICONTYPE_FUNCTION) {
        sResult = ":/XStyles/icons/Function.16.16.png";
    } else if (iconType == ICONTYPE_SCRIPT) {
        sResult = ":/XStyles/icons/Script.16.16.png";
    } else if (iconType == ICONTYPE_PATCH) {
        sResult = ":/XStyles/icons/Patch.16.16.png";
    } else if (iconType == ICONTYPE_REMOVE) {
        sResult = ":/XStyles/icons/Remove.16.16.png";
    } else if (iconType == ICONTYPE_RESIZE) {
        sResult = ":/XStyles/icons/Resize.16.16.png";
    } else if (iconType == ICONTYPE_CODE) {
        sResult = ":/XStyles/icons/Code.16.16.png";
    } else if (iconType == ICONTYPE_REFERENCE) {
        sResult = ":/XStyles/icons/Reference.16.16.png";
    } else if (iconType == ICONTYPE_BOOKMARK) {
        sResult = ":/XStyles/icons/Bookmark.16.16.png";
    } else if (iconType == ICONTYPE_INSPECTOR) {
        sResult = ":/XStyles/icons/Inspector.16.16.png";
    } else if (iconType == ICONTYPE_CONVERTOR) {
        sResult = ":/XStyles/icons/Convertor.16.16.png";
    } else if (iconType == ICONTYPE_STRUCTS) {
        sResult = ":/XStyles/icons/Structs.16.16.png";
    } else {
        sResult = ":/XStyles/icons/BreakpointDisabled.16.16.png";
    }

    if (sResult != "") {
        if (!QFile::exists(sResult)) {
            sResult = "";
        }
    }

    return sResult;
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustMenu(QMenu *pParentMenu, QMenu *pMenu, const QString &sText, ICONTYPE iconType)
{
    pMenu->setTitle(sText);

    QString sIconPath = XOptions::getIconPath(iconType);

    if (sIconPath != "") {
        QIcon icon;
        icon.addFile(sIconPath, QSize(), QIcon::Normal, QIcon::Off);
        pMenu->setIcon(icon);
    }

    if (pParentMenu) {
        pParentMenu->addMenu(pMenu);
    }
}
#endif
#ifdef QT_GUI_LIB
void XOptions::adjustAction(QMenu *pParentMenu, QAction *pAction, const QString &sText, const QObject *pRecv, const char *pMethod, ICONTYPE iconType)
{
    connect(pAction, SIGNAL(triggered()), pRecv, pMethod);

    pAction->setText(sText);

    QString sIconPath = XOptions::getIconPath(iconType);

    if (sIconPath != "") {
        QIcon icon;
        icon.addFile(sIconPath, QSize(), QIcon::Normal, QIcon::Off);
        pAction->setIcon(icon);
    }

    if (pParentMenu) {
        pParentMenu->addAction(pAction);
    }
}
#endif

XOptions::BUNDLE XOptions::getBundle()
{
    BUNDLE result = BUNDLE_UNKNOWN;

#ifdef QT_DEBUG
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    QString _sProductType = QSysInfo::productType();
    qDebug("OS: %s", _sProductType.toUtf8().data());
#endif
#endif

#ifdef Q_OS_WIN
#if defined(Q_PROCESSOR_ARM64)
    result = BUNDLE_WINDOWS_ARM64;
#elif QT_VERSION <= QT_VERSION_CHECK(5, 6, 3)
    result = BUNDLE_WINDOWS_XP_X86;
#elif (QT_VERSION_MAJOR >= 6)
#ifdef Q_OS_WIN64
    result = BUNDLE_WINDOWS_QT6_X64;
#else
    result = BUNDLE_WINDOWS_QT6_X86;
#endif
#else
#ifdef Q_OS_WIN64
    result = BUNDLE_WINDOWS_X64;
#else
    result = BUNDLE_WINDOWS_X86;
#endif
#endif
#endif
#ifdef Q_PROCESSOR_X86
#ifdef Q_OS_LINUX
    result = BUNDLE_LINUX_DEBIAN_X86;
#endif
#endif
#ifdef Q_PROCESSOR_X86_64
#ifdef Q_OS_LINUX
#if QT_VERSION == QT_VERSION_CHECK(5, 2, 1)
    result = BUNDLE_LINUX_APPIMAGE_X64;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    QString sProductType = QSysInfo::productType();

    if (sProductType == "ubuntu") {
        result = BUNDLE_LINUX_UBUNTU_X64;
    }
#endif
#endif
#ifdef Q_OS_FRREBSD
    result = BUNDLE_FREEBSD_X64;
#endif
#ifdef Q_OS_MACOS
    // TODO: QSysInfo::currentCpuArchitecture();
#endif
#ifdef Q_OS_FEDORA
    result = BUNDLE_FEDORA_X64;
#endif
#endif

    return result;
}
