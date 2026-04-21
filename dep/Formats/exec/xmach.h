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
#ifndef XMACH_H
#define XMACH_H

#include "xbinary.h"
#include "xmach_def.h"

// https://github.com/phracker/MacOSX-SDKs
// https://github.com/joseluisq/macosx-sdks
class XMACH : public XBinary {
    Q_OBJECT

public:
    struct COMMAND_RECORD {
        qint64 nStructOffset;
        quint32 nId;
        qint64 nSize;
    };

    struct LIBRARY_RECORD {
        qint64 nStructOffset;
        qint64 nStructSize;
        QString sName;
        QString sFullName;
        quint32 name;
        quint32 timestamp;
        quint32 current_version;
        quint32 compatibility_version;
        qint64 nMaxStringSize;
    };

    struct FVM_LIBRARY_RECORD {
        qint64 nStructOffset;
        qint64 nStructSize;
        QString sName;
        QString sFullName;
        quint32 name;
        quint32 minor_version;
        quint32 header_addr;
        qint64 nMaxStringSize;
    };

    struct SEGMENT_RECORD {
        qint64 nStructOffset;
        bool bIs64;
        union {
            XMACH_DEF::segment_command segment32;
            XMACH_DEF::segment_command_64 segment64;
        } s;
    };

    struct SECTION_RECORD {
        qint64 nStructOffset;
        bool bIs64;
        union {
            XMACH_DEF::section section32;
            XMACH_DEF::section_64 section64;
        } s;
    };

    struct NLIST_RECORD {
        qint64 nStructOffset;
        bool bIs64;
        union {
            XMACH_DEF::nlist nlist32;
            XMACH_DEF::nlist_64 nlist64;
        } s;
    };

    struct MODTAB_RECORD {
        qint64 nStructOffset;
        bool bIs64;
        union {
            XMACH_DEF::dylib_module module32;
            XMACH_DEF::dylib_module_64 module64;
        } s;
    };

    struct TOC_RECORD {
        qint64 nStructOffset;
        XMACH_DEF::dylib_table_of_contents toc;
    };

    struct RELOC_RECORD {
        qint64 nStructOffset;
        XMACH_DEF::relocation_info reloc;
    };

    struct VALUE32_RECORD {
        qint64 nStructOffset;
        quint32 nValue;
    };

    struct REFERENCE_RECORD {
        qint64 nStructOffset;
        XMACH_DEF::dylib_reference reference;
    };

    struct DICE_RECORD {
        qint64 nStructOffset;
        XMACH_DEF::data_in_code_entry dice;
    };

    struct FUNCTION_RECORD {
        quint64 nValue;
        qint64 nDataOffset;
        qint64 nDataSize;
        qint64 nFunctionOffset;
        qint64 nFunctionAddress;
    };

    enum TYPE {
        TYPE_UNKNOWN = 0,
        TYPE_OBJECT,
        TYPE_EXECUTE,
        TYPE_FVMLIB,
        TYPE_CORE,
        TYPE_PRELOAD,
        TYPE_DYLIB,
        TYPE_DYLINKER,
        TYPE_BUNDLE,
        TYPE_DYLIB_STUB,
        TYPE_DSYM,
        TYPE_KEXT_BUNDLE,
        TYPE_FILESET,
        TYPE_GPU_EXECUTE,
        TYPE_GPU_DYLIB,
    };

    struct EXPORT_RECORD {
        QString sName;
        qint64 nOffset;
        quint32 nFlags;
    };

    struct XCODE_VERSION_RECORD {
        QString sVersion;
        QString sReleaseDate;
        QString sMinMacOSToRun;
        QString sMacOSSDK;
        QString siOSSDK;
        QString sWatchOSSDK;
        QString sTvOSSDK;
        QString sVisionOSSDK;
    };

    struct SDK_VERSION_RECORD {
        QString sSDKVersion;
        QString sSDKBuild;
    };

    struct XCODE_TOOLCHAIN_RECORD_1 {
        QString sVersion;
        QString sGccVersion;
        QString sGdbVersion;
    };

    struct XCODE_TOOLCHAIN_RECORD_3 {
        QString sVersion;
        QString sCctools;
        QString sLd64;
        QString sGcc40;
        QString sGcc42;
        QString sLLVMGcc42;
        QString sLLVM;
        QString sAppleLLVMClang;
    };

    struct XCODE_TOOLCHAIN_RECORD_5 {
        QString sVersion;
        QString sCctools;
        QString sLd64;
        QString sLLVM;
        QString sClangVersion;
        QString sSwiftVersion;
    };

    struct FOUNDATION_VERSION_RECORD {
        quint32 nMajor;
        quint32 nMinor;
        quint32 nPatch;
        QString sMacOSVersion;
    };

    struct IOS_FOUNDATION_VERSION_RECORD {
        quint32 nMajor;
        quint32 nMinor;
        quint32 nPatch;
        QString siOSVersion;
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_mach_header,
        STRUCTID_mach_header_64,
        STRUCTID_load_command,
    };

    XMACH(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    ~XMACH();

    // TODO isSigned
    // TODO getSignOS
    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1, PDSTRUCT *pPdStruct = nullptr);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    ENDIAN getEndian();

    virtual bool isExecutable();

    qint64 getHeaderOffset();
    qint64 getHeader32Size();
    qint64 getHeader64Size();

    quint32 getHeader_magic();
    qint32 getHeader_cputype();
    qint32 getHeader_cpusubtype();
    quint32 getHeader_filetype();
    quint32 getHeader_ncmds();
    quint32 getHeader_sizeofcmds();
    quint32 getHeader_flags();
    quint32 getHeader_reserved();

    void setHeader_magic(quint32 nValue);
    void setHeader_cputype(qint32 nValue);
    void setHeader_cpusubtype(qint32 nValue);
    void setHeader_filetype(quint32 nValue);
    void setHeader_ncmds(quint32 nValue);
    void setHeader_sizeofcmds(quint32 nValue);
    void setHeader_flags(quint32 nValue);
    void setHeader_reserved(quint32 nValue);

    qint64 getHeaderSize();

    XMACH_DEF::mach_header _read_mach_header(qint64 nOffset);
    XMACH_DEF::mach_header_64 _read_mach_header_64(qint64 nOffset);

    static QMap<quint64, QString> getHeaderMagics();
    static QMap<quint64, QString> getHeaderMagicsS();
    static QMap<quint64, QString> getHeaderCpuTypes();
    static QMap<quint64, QString> getHeaderCpuTypesS();
    static QMap<quint64, QString> getHeaderCpuSubTypes(quint32 nCpuType);
    static QMap<quint64, QString> getHeaderCpuSubTypesS(quint32 nCpuType);
    static QMap<quint64, QString> getHeaderFileTypes();
    static QMap<quint64, QString> getHeaderFileTypesS();
    static QMap<quint64, QString> getHeaderFlags();
    static QMap<quint64, QString> getHeaderFlagsS();
    static QMap<quint64, QString> getLoadCommandTypes();
    static QMap<quint64, QString> getLoadCommandTypesS();
    static QMap<quint64, QString> getVMProtections();
    static QMap<quint64, QString> getVMProtectionsS();
    static QMap<quint64, QString> getSectionFlagsTypes();
    static QMap<quint64, QString> getSectionFlagsTypesS();
    static QMap<quint64, QString> getSectionAttributesUsr();
    static QMap<quint64, QString> getSectionAttributesUsrS();
    static QMap<quint64, QString> getSectionAttributesSys();
    static QMap<quint64, QString> getSectionAttributesSysS();
    static QMap<quint64, QString> getDICEKinds();
    static QMap<quint64, QString> getDICEKindsS();
    static QMap<quint64, QString> getPlatform();
    static QMap<quint64, QString> getPlatformS();
    static QMap<quint64, QString> getBuildTool();
    static QMap<quint64, QString> getBuildToolS();
    static QMap<quint64, QString> getDyldChainedImport();
    static QMap<quint64, QString> getDyldChainedImportS();

    static QString getSDKVersionFromFoundation(quint32 nFoundationVersion, XBinary::OSNAME osName);
    static XBinary::OSNAME getExactOSName(XBinary::OSNAME osName, const QString &sVersion);
    static QString getXcodeVersionFromSDK(const QString &sSDKVersion, XBinary::OSNAME osName);
    static QString getClangVersionFromSDK(const QString &sSDKVersion, XBinary::OSNAME osName);
    static QString getSwiftVersionFromSDK(const QString &sSDKVersion, XBinary::OSNAME osName);

    COMMAND_RECORD _readLoadCommand(qint64 nOffset, bool bIsBigEndian);

    void _setCommand_cmd(qint64 nOffset, quint32 nValue);
    void _setCommand_cmdsize(qint64 nOffset, quint32 nValue);

    QList<COMMAND_RECORD> getCommandRecords(quint32 nCommandID = 0, PDSTRUCT *pPdStruct = nullptr);
    QList<COMMAND_RECORD> _getCommandRecords(qint64 nDataOffset, qint64 nDataSize, qint32 nLimit, bool bIs64, bool bIsBigEndian, quint32 nCommandID = 0,
                                             PDSTRUCT *pPdStruct = nullptr);
    static QList<COMMAND_RECORD> getCommandRecords(quint32 nCommandID, QList<COMMAND_RECORD> *pListCommandRecords, PDSTRUCT *pPdStruct = nullptr);

    bool isCommandPresent(quint32 nCommandID, qint32 nIndex = 0);
    bool isCommandPresent(quint32 nCommandID, qint32 nIndex, QList<COMMAND_RECORD> *pListCommandRecords);
    bool isCommandPresent(quint32 nCommandID, QList<COMMAND_RECORD> *pListCommandRecords);

    quint32 getCommandId(qint32 nIndex, QList<COMMAND_RECORD> *pListCommandRecords);

    QByteArray getCommandData(quint32 nCommandID, qint32 nIndex = 0);
    bool setCommandData(quint32 nCommandID, const QByteArray &baData, qint32 nIndex = 0);
    QByteArray getCommandData(quint32 nCommandID, qint32 nIndex, QList<COMMAND_RECORD> *pListCommandRecords);
    bool setCommandData(quint32 nCommandID, const QByteArray &baData, qint32 nIndex, QList<COMMAND_RECORD> *pListCommandRecords);

    qint64 getCommandRecordOffset(quint32 nCommandID, qint32 nIndex);
    qint64 getCommandRecordOffset(quint32 nCommandID, qint32 nIndex, QList<COMMAND_RECORD> *pListCommandRecords);

    qint64 getCommandHeaderSize();

    qint64 getAddressOfEntryPoint(_MEMORY_MAP *pMemoryMap);

    virtual QList<MAPMODE> getMapModesList();
    virtual XADDR getBaseAddress();
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);

    QList<LIBRARY_RECORD> getLibraryRecords(qint32 nType = XMACH_DEF::S_LC_LOAD_DYLIB);
    QList<LIBRARY_RECORD> getLibraryRecords(QList<COMMAND_RECORD> *pListCommandRecords, qint32 nType = XMACH_DEF::S_LC_LOAD_DYLIB);
    static LIBRARY_RECORD getLibraryRecordByName(const QString &sName, QList<LIBRARY_RECORD> *pListLibraryRecords);
    bool isLibraryRecordNamePresent(const QString &sName);
    static bool isLibraryRecordNamePresent(const QString &sName, QList<LIBRARY_RECORD> *pListLibraryRecords);

    LIBRARY_RECORD _readLibraryRecord(qint64 nOffset, bool bIsBigEndian);

    QList<FVM_LIBRARY_RECORD> getFvmLibraryRecords(qint32 nType = XMACH_DEF::S_LC_LOAD_DYLIB);
    QList<FVM_LIBRARY_RECORD> getFvmLibraryRecords(QList<COMMAND_RECORD> *pListCommandRecords, qint32 nType = XMACH_DEF::S_LC_LOADFVMLIB);
    static FVM_LIBRARY_RECORD getFvmLibraryRecordByName(const QString &sName, QList<FVM_LIBRARY_RECORD> *pListLibraryRecords);
    bool isFvmLibraryRecordNamePresent(const QString &sName);
    static bool isFvmLibraryRecordNamePresent(const QString &sName, QList<FVM_LIBRARY_RECORD> *pListLibraryRecords);

    FVM_LIBRARY_RECORD _readFvmLibraryRecord(qint64 nOffset, bool bIsBigEndian);

    void _setLibraryRecord_timestamp(qint64 nOffset, quint32 nValue);
    void _setLibraryRecord_current_version(qint64 nOffset, quint32 nValue);
    void _setLibraryRecord_compatibility_version(qint64 nOffset, quint32 nValue);
    void _setLibraryRecord_name(qint64 nOffset, const QString &sValue);

    void _setFvmLibraryRecord_minor_version(qint64 nOffset, quint32 nValue);
    void _setFvmLibraryRecord_header_addr(qint64 nOffset, quint32 nValue);
    void _setFvmLibraryRecord_name(qint64 nOffset, const QString &sValue);

    QList<SEGMENT_RECORD> getSegmentRecords();
    QList<SEGMENT_RECORD> getSegmentRecords(QList<COMMAND_RECORD> *pListCommandRecords);

    XMACH_DEF::segment_command _read_segment_command(qint64 nOffset);
    XMACH_DEF::segment_command_64 _read_segment_command_64(qint64 nOffset);
    XMACH_DEF::segment_command _read_segment_command(qint64 nOffset, bool bIsBigEndian);
    XMACH_DEF::segment_command_64 _read_segment_command_64(qint64 nOffset, bool bIsBigEndian);

    void _setSegment32_segname(qint64 nOffset, const QString &sValue);
    void _setSegment32_vmaddr(qint64 nOffset, quint32 nValue);
    void _setSegment32_vmsize(qint64 nOffset, quint32 nValue);
    void _setSegment32_fileoff(qint64 nOffset, quint32 nValue);
    void _setSegment32_filesize(qint64 nOffset, quint32 nValue);
    void _setSegment32_maxprot(qint64 nOffset, qint32 nValue);
    void _setSegment32_initprot(qint64 nOffset, qint32 nValue);
    void _setSegment32_nsects(qint64 nOffset, quint32 nValue);
    void _setSegment32_flags(qint64 nOffset, quint32 nValue);

    void _setSegment64_segname(qint64 nOffset, const QString &sValue);
    void _setSegment64_vmaddr(qint64 nOffset, quint64 nValue);
    void _setSegment64_vmsize(qint64 nOffset, quint64 nValue);
    void _setSegment64_fileoff(qint64 nOffset, quint64 nValue);
    void _setSegment64_filesize(qint64 nOffset, quint64 nValue);
    void _setSegment64_maxprot(qint64 nOffset, qint32 nValue);
    void _setSegment64_initprot(qint64 nOffset, qint32 nValue);
    void _setSegment64_nsects(qint64 nOffset, quint32 nValue);
    void _setSegment64_flags(qint64 nOffset, quint32 nValue);

    QList<SECTION_RECORD> getSectionRecords();
    QList<SECTION_RECORD> getSectionRecords(QList<COMMAND_RECORD> *pListCommandRecords);

    XMACH_DEF::section _read_section(qint64 nOffset, bool bIsBigEndian);
    XMACH_DEF::section_64 _read_section_64(qint64 nOffset, bool bIsBigEndian);

    void _setSection32_sectname(qint64 nOffset, const QString &sValue);
    void _setSection32_segname(qint64 nOffset, const QString &sValue);
    void _setSection32_addr(qint64 nOffset, quint32 nValue);
    void _setSection32_size(qint64 nOffset, quint32 nValue);
    void _setSection32_offset(qint64 nOffset, quint32 nValue);
    void _setSection32_align(qint64 nOffset, quint32 nValue);
    void _setSection32_reloff(qint64 nOffset, quint32 nValue);
    void _setSection32_nreloc(qint64 nOffset, quint32 nValue);
    void _setSection32_flags(qint64 nOffset, quint32 nValue);
    void _setSection32_reserved1(qint64 nOffset, quint32 nValue);
    void _setSection32_reserved2(qint64 nOffset, quint32 nValue);

    void _setSection64_sectname(qint64 nOffset, const QString &sValue);
    void _setSection64_segname(qint64 nOffset, const QString &sValue);
    void _setSection64_addr(qint64 nOffset, quint64 nValue);
    void _setSection64_size(qint64 nOffset, quint64 nValue);
    void _setSection64_offset(qint64 nOffset, quint32 nValue);
    void _setSection64_align(qint64 nOffset, quint32 nValue);
    void _setSection64_reloff(qint64 nOffset, quint32 nValue);
    void _setSection64_nreloc(qint64 nOffset, quint32 nValue);
    void _setSection64_flags(qint64 nOffset, quint32 nValue);
    void _setSection64_reserved1(qint64 nOffset, quint32 nValue);
    void _setSection64_reserved2(qint64 nOffset, quint32 nValue);
    void _setSection64_reserved3(qint64 nOffset, quint32 nValue);

    qint64 getSegmentHeaderSize();

    quint32 getNumberOfSections();
    quint32 getNumberOfSections(QList<COMMAND_RECORD> *pListCommandRecords);

    bool isSectionNamePresent(const QString &sName);
    static bool isSectionNamePresent(const QString &sName, QList<SECTION_RECORD> *pListSectionRecords);

    static qint32 getSectionNumber(const QString &sName, QList<SECTION_RECORD> *pListSectionRecords);
    qint32 getSectionNumber(const QString &sName);

    qint64 getSectionHeaderSize();

    static quint32 getSectionFileOffset(quint32 nIndex, QList<SECTION_RECORD> *pListSectionRecords);
    static quint32 getSectionFileSize(quint32 nIndex, QList<SECTION_RECORD> *pListSectionRecords);

    static QList<QString> getSectionNames(QList<SECTION_RECORD> *pListSectionRecords);

    bool isSegmentNamePresent(const QString &sName);
    static bool isSegmentNamePresent(const QString &sName, QList<SEGMENT_RECORD> *pListSegmentRecords);

    static qint32 getSegmentNumber(const QString &sName, QList<SEGMENT_RECORD> *pListSegmentRecords);
    qint32 getSegmentNumber(const QString &sName);

    static quint32 getLibraryCurrentVersion(const QString &sName, QList<LIBRARY_RECORD> *pListLibraryRecords);

    XMACH_DEF::dyld_info_command get_dyld_info_command();

    qint64 get_dyld_info_command_size();

    void _set_dyld_info_command_rebase_off(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_rebase_size(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_bind_off(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_bind_size(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_weak_bind_off(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_weak_bind_size(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_lazy_bind_off(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_lazy_bind_size(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_export_off(qint64 nOffset, quint32 nValue);
    void _set_dyld_info_command_export_size(qint64 nOffset, quint32 nValue);

    QString getModuleUUID();
    void setModuleUUID(const QString &sValue);

    QString getLoadDylinker();
    void setLoadDylinker(const QString &sValue);

    QString getRPath();
    void setRPath(const QString &sValue);

    XMACH_DEF::symtab_command get_symtab_command();

    qint64 get_symtab_command_size();

    void _set_symtab_command_symoff(qint64 nOffset, quint32 nValue);
    void _set_symtab_command_nsyms(qint64 nOffset, quint32 nValue);
    void _set_symtab_command_stroff(qint64 nOffset, quint32 nValue);
    void _set_symtab_command_strsize(qint64 nOffset, quint32 nValue);

    XMACH_DEF::dysymtab_command get_dysymtab_command();

    qint64 get_dysymtab_command_size();

    void _set_dysymtab_command_ilocalsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nlocalsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_iextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_iundefsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nundefsym(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_tocoff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_ntoc(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_modtaboff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nmodtab(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_extrefsymoff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nextrefsyms(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_indirectsymoff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nindirectsyms(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_extreloff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nextrel(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_locreloff(qint64 nOffset, quint32 nValue);
    void _set_dysymtab_command_nlocrel(qint64 nOffset, quint32 nValue);

    XMACH_DEF::version_min_command get_version_min_command();

    qint64 get_version_min_command_size();

    void _set_version_min_command_version(qint64 nOffset, quint32 nValue);
    void _set_version_min_command_sdk(qint64 nOffset, quint32 nValue);

    qint64 get_build_version_command_size();

    void _set_build_version_command_platform(qint64 nOffset, quint32 nValue);
    void _set_build_version_command_minos(qint64 nOffset, quint32 nValue);
    void _set_build_version_command_sdk(qint64 nOffset, quint32 nValue);
    void _set_build_version_command_ntools(qint64 nOffset, quint32 nValue);

    quint64 getSourceVersion();
    void setSourceVersion(quint64 nValue);

    qint64 get_source_version_command_size();

    void _set_encryption_info_command_64_cryptoff(qint64 nOffset, quint32 nValue);
    void _set_encryption_info_command_64_cryptsize(qint64 nOffset, quint32 nValue);
    void _set_encryption_info_command_64_cryptid(qint64 nOffset, quint32 nValue);
    void _set_encryption_info_command_64_pad(qint64 nOffset, quint32 nValue);

    qint64 get_encryption_info_command_size();
    qint64 get_encryption_info_command_64_size();

    void _set_linkedit_data_command_dataoff(qint64 nOffset, quint32 nValue);
    void _set_linkedit_data_command_datasize(qint64 nOffset, quint32 nValue);

    qint64 get_linkedit_data_command_size();

    void _set_entry_point_command_entryoff(qint64 nOffset, quint64 nValue);
    void _set_entry_point_command_stacksize(qint64 nOffset, quint64 nValue);

    qint64 get_entry_point_command_size();

    void _set_unix_thread_command_flavor(qint64 nOffset, quint32 nValue);
    void _set_unix_thread_command_count(qint64 nOffset, quint32 nValue);

    qint64 get_unix_thread_command_size();

    void _set_x86_thread_state32_t_eax(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_ebx(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_ecx(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_edx(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_edi(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_esi(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_ebp(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_esp(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_ss(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_eflags(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_eip(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_cs(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_ds(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_es(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_fs(qint64 nOffset, quint32 nValue);
    void _set_x86_thread_state32_t_gs(qint64 nOffset, quint32 nValue);

    qint64 get_x86_thread_state32_t_size();

    void _set_x86_thread_state64_t_rax(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rbx(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rcx(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rdx(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rdi(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rsi(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rbp(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rsp(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r8(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r9(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r10(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r11(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r12(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r13(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r14(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_r15(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rip(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_rflags(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_cs(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_fs(qint64 nOffset, quint64 nValue);
    void _set_x86_thread_state64_t_gs(qint64 nOffset, quint64 nValue);

    qint64 get_x86_thread_state64_t_size();

    void _set_ppc_thread_state32_t_srr0(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_srr1(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_r(qint64 nOffset, quint32 nValue, qint32 nIndex);
    void _set_ppc_thread_state32_t_ct(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_xer(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_lr(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_ctr(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_mq(qint64 nOffset, quint32 nValue);
    void _set_ppc_thread_state32_t_vrsave(qint64 nOffset, quint32 nValue);

    qint64 get_ppc_thread_state32_t_size();

    void _set_arm_thread_state32_t_r(qint64 nOffset, quint32 nValue, qint32 nIndex);
    void _set_arm_thread_state32_t_sp(qint64 nOffset, quint32 nValue);
    void _set_arm_thread_state32_t_lr(qint64 nOffset, quint32 nValue);
    void _set_arm_thread_state32_t_pc(qint64 nOffset, quint32 nValue);
    void _set_arm_thread_state32_t_cpsr(qint64 nOffset, quint32 nValue);

    qint64 get_arm_thread_state32_t_size();

    void _set_m68k_thread_state32_t_dreg(qint64 nOffset, quint32 nValue, qint32 nIndex);
    void _set_m68k_thread_state32_t_areg(qint64 nOffset, quint32 nValue, qint32 nIndex);
    void _set_m68k_thread_state32_t_pad0(qint64 nOffset, quint16 nValue);
    void _set_m68k_thread_state32_t_sr(qint64 nOffset, quint16 nValue);
    void _set_m68k_thread_state32_t_pc(qint64 nOffset, quint32 nValue);

    qint64 get_m68k_thread_state32_t_size();

    void _set_arm_thread_state64_t_x(qint64 nOffset, quint64 nValue, qint32 nIndex);
    void _set_arm_thread_state64_t_fp(qint64 nOffset, quint64 nValue);
    void _set_arm_thread_state64_t_lr(qint64 nOffset, quint64 nValue);
    void _set_arm_thread_state64_t_sp(qint64 nOffset, quint64 nValue);
    void _set_arm_thread_state64_t_pc(qint64 nOffset, quint64 nValue);
    void _set_arm_thread_state64_t_cpsr(qint64 nOffset, quint64 nValue);
    void _set_arm_thread_state64_t_pad(qint64 nOffset, quint64 nValue);

    qint64 get_arm_thread_state64_t_size();

    void _set_nlist_n_strx(qint64 nOffset, quint32 nValue);
    void _set_nlist_n_type(qint64 nOffset, quint8 nValue);
    void _set_nlist_n_sect(qint64 nOffset, quint8 nValue);
    void _set_nlist_n_desc(qint64 nOffset, quint16 nValue);
    void _set_nlist_n_value(qint64 nOffset, quint32 nValue);

    qint64 get_nlist_size();

    void _set_nlist_64_n_strx(qint64 nOffset, quint32 nValue);
    void _set_nlist_64_n_type(qint64 nOffset, quint8 nValue);
    void _set_nlist_64_n_sect(qint64 nOffset, quint8 nValue);
    void _set_nlist_64_n_desc(qint64 nOffset, quint16 nValue);
    void _set_nlist_64_n_value(qint64 nOffset, quint64 nValue);

    qint64 get_nlist_64_size();

    qint64 get_nlist_MODE_size();

    void _set_data_in_code_entry_offset(qint64 nOffset, quint32 nValue);
    void _set_data_in_code_entry_length(qint64 nOffset, quint16 nValue);
    void _set_data_in_code_entry_kind(qint64 nOffset, quint16 nValue);

    qint64 get_data_in_code_entry_size();

    void _set_dylib_module_module_name(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_iextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_nextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_irefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_nrefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_ilocalsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_nlocalsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_iextrel(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_nextrel(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_iinit_iterm(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_ninit_nterm(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_objc_module_info_addr(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_objc_module_info_size(qint64 nOffset, quint32 nValue);

    qint64 get_dylib_module_size();

    void _set_dylib_module_64_module_name(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_iextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_nextdefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_irefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_nrefsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_ilocalsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_nlocalsym(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_iextrel(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_nextrel(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_iinit_iterm(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_ninit_nterm(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_objc_module_info_size(qint64 nOffset, quint32 nValue);
    void _set_dylib_module_64_objc_module_info_addr(qint64 nOffset, quint32 nValue);

    qint64 get_dylib_module_64_size();

    void _set_dylib_table_of_contents_symbol_index(qint64 nOffset, quint32 nValue);
    void _set_dylib_table_of_contents_module_index(qint64 nOffset, quint32 nValue);

    qint64 get_dylib_table_of_contents_size();

    void _set_relocation_info_r_address(qint64 nOffset, quint32 nValue);
    void _set_relocation_info_value(qint64 nOffset, quint32 nValue);

    qint64 get_relocation_info_size();

    void _set_value32_value(qint64 nOffset, quint32 nValue);

    qint64 get_value32_size();

    qint64 get_dylib_reference_size();

    void _set_SC_SuperBlob_magic(qint64 nOffset, quint32 nValue);
    void _set_SC_SuperBlob_length(qint64 nOffset, quint32 nValue);
    void _set_SC_SuperBlob_count(qint64 nOffset, quint32 nValue);

    qint64 get_SC_SuperBlob_size();

    void _set_BlobIndex_type(qint64 nOffset, quint32 nValue);
    void _set_BlobIndex_offset(qint64 nOffset, quint32 nValue);

    qint64 get_BlobIndex_size();

    XMACH_DEF::dyld_info_command _read_dyld_info_command(qint64 nOffset);
    XMACH_DEF::symtab_command _read_symtab_command(qint64 nOffset);
    XMACH_DEF::dysymtab_command _read_dysymtab_command(qint64 nOffset);
    XMACH_DEF::version_min_command _read_version_min_command(qint64 nOffset);
    XMACH_DEF::build_version_command _read_build_version_command(qint64 nOffset);
    XMACH_DEF::dylinker_command _read_dylinker_command(qint64 nOffset);
    XMACH_DEF::rpath_command _read_rpath_command(qint64 nOffset);
    XMACH_DEF::source_version_command _read_source_version_command(qint64 nOffset);
    XMACH_DEF::encryption_info_command _read_encryption_info_command(qint64 nOffset);
    XMACH_DEF::encryption_info_command_64 _read_encryption_info_command_64(qint64 nOffset);
    XMACH_DEF::entry_point_command _read_entry_point_command(qint64 nOffset);
    XMACH_DEF::x86_thread_state32_t _read_x86_thread_state32_t(qint64 nOffset);
    XMACH_DEF::x86_thread_state64_t _read_x86_thread_state64_t(qint64 nOffset);
    XMACH_DEF::ppc_thread_state32_t _read_ppc_thread_state32_t(qint64 nOffset);
    XMACH_DEF::arm_thread_state32_t _read_arm_thread_state32_t(qint64 nOffset);
    XMACH_DEF::arm_thread_state64_t _read_arm_thread_state64_t(qint64 nOffset);
    XMACH_DEF::m68k_thread_state32_t _read_m68k_thread_state32_t(qint64 nOffset);
    XMACH_DEF::state_hdr_t _read_state_hdr_t(qint64 nOffset);
    XMACH_DEF::linkedit_data_command _read_linkedit_data_command(qint64 nOffset);
    XMACH_DEF::unix_thread_command _read_unix_thread_command(qint64 nOffset);
    XMACH_DEF::nlist _read_nlist(qint64 nOffset);
    XMACH_DEF::nlist_64 _read_nlist_64(qint64 nOffset);
    XMACH_DEF::data_in_code_entry _read_data_in_code_entry(qint64 nOffset);
    XMACH_DEF::dylib_module _read_dylib_module(qint64 nOffset);
    XMACH_DEF::dylib_module_64 _read_dylib_module_64(qint64 nOffset);
    XMACH_DEF::dylib_table_of_contents _read_dylib_table_of_contents(qint64 nOffset);
    XMACH_DEF::dylib_table_of_contents _read_dylib_table_of_contents(qint64 nOffset, bool bIsBigEndian);
    XMACH_DEF::relocation_info _read_relocation_info(qint64 nOffset);
    XMACH_DEF::dylib_reference _read_dylib_reference(qint64 nOffset);
    XMACH_DEF::__SC_SuperBlob _read_SC_SuperBlob(qint64 nOffset);
    XMACH_DEF::CS_BlobIndex _read_CS_BlobIndex(qint64 nOffset);
    XMACH_DEF::CS_CodeDirectory _read_CS_CodeDirectory(qint64 nOffset);
    XMACH_DEF::dyld_chained_fixups_header _read_dyld_chained_fixups_header(qint64 nOffset);
    XMACH_DEF::twolevel_hints_command _read_twolevel_hints_command(qint64 nOffset);
    XMACH_DEF::build_tool_version _read_build_tool_version(qint64 nOffset);
    XMACH_DEF::build_tool_version _read_build_tool_version(qint64 nOffset, bool bIsBigEndian);

    QList<NLIST_RECORD> getNlistRecords();
    QList<NLIST_RECORD> getNlistRecords(QList<COMMAND_RECORD> *pListCommandRecords);

    static NLIST_RECORD searchNlistRecordByValue(QList<NLIST_RECORD> *pList, quint64 nValue, bool bValidName = false);

    QList<TOC_RECORD> get_toc_list();
    QList<MODTAB_RECORD> get_modtab_list();
    QList<REFERENCE_RECORD> get_extrefsyms_list();
    QList<VALUE32_RECORD> get_indirectsyms_list();
    QList<RELOC_RECORD> get_extrel_list();
    QList<RELOC_RECORD> get_locrel_list();
    QList<RELOC_RECORD> getRelocRecords(qint64 nOffset, qint32 nNumberOfRecords);
    QList<VALUE32_RECORD> getValue32Records(qint64 nOffset, qint32 nNumberOfRecords);

    OFFSETSIZE getStringTableOffsetSize();
    OFFSETSIZE getStringTableOffsetSize(QList<COMMAND_RECORD> *pListCommandRecords);

    XMACH_DEF::dyld_info_command get_dyld_info();
    XMACH_DEF::symtab_command get_symtab();
    XMACH_DEF::dysymtab_command get_dysymtab();
    XMACH_DEF::encryption_info_command get_encryption_info();
    XMACH_DEF::encryption_info_command_64 get_encryption_info_64();
    XMACH_DEF::linkedit_data_command get_linkedit_data(quint32 nCommandID);

    QList<FUNCTION_RECORD> getFunctionRecords(qint64 nOffset, qint64 nSize);  // TODO pdStruct

    QList<DICE_RECORD> getDiceRecords(qint64 nOffset, qint64 nSize);

    QString getIndexSymbolName(quint32 nValue);
    QString getIndexSymbolName(quint32 nValue, QList<NLIST_RECORD> *pNlistList, qint64 nStringTableOffset, qint64 nStringTableSize);

    virtual MODE getMode();
    virtual QString getArch();
    static QString _getArch(quint32 nCpuType, quint32 nCpuSubType);
    virtual FT getFileType();
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);

    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);

    virtual FILEFORMATINFO getFileFormatInfo(PDSTRUCT *pPdStruct);

    enum OPCODE_TYPE {
        OPCODE_TYPE_UNKNOWN = 0,
        OPCODE_TYPE_BIND,
        OPCODE_TYPE_WEAK_BIND,
        OPCODE_TYPE_LAZY_BIND,
        OPCODE_TYPE_EXPORT,
        OPCODE_TYPE_REBASE
    };

    virtual XADDR readOpcodes(quint32 nType, char *pData, XADDR nAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus);
    XADDR readOpcodesInterface_rebase(char *pData, XADDR nAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus);
    XADDR readOpcodesInterface_bind(char *pData, XADDR nAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus, bool bNullEnd);
    XADDR readOpcodesInterface_export(char *pData, XADDR nAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus);

    bool handleImport(qint64 nOffset, qint64 nRelOffset, qint64 nSize, QList<EXPORT_RECORD> *pListExportRecords, const QString &sCurrentName, PDSTRUCT *pPdStruct);

    virtual QString structIDToString(quint32 nID);
    virtual QList<XFRECORD> getXFRecords(FT fileType, quint32 nStructID, const XLOC &xLoc);
    virtual QList<XFHEADER> getXFHeaders(const XFSTRUCT &xfStruct, PDSTRUCT *pPdStruct);
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);

    virtual QString getMIMEString();
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;

};

#endif  // XMACH_H
