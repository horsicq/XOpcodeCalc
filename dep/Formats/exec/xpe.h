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
#ifndef XPE_H
#define XPE_H

// #include "xicon.h"
#include "xmsdos.h"
#include "xpe_def.h"

#if defined(_MSC_VER)  // For WinTrust TODO Check minGW
// #include <wincrypt.h>
#include <windows.h>
// #include <wintrust.h>
#include <Softpub.h>
#endif

class XPE : public XMSDOS {
    Q_OBJECT

public:
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_IMAGE_DOS_HEADER,
        STRUCTID_IMAGE_NT_HEADERS32,
        STRUCTID_IMAGE_NT_HEADERS64,
        STRUCTID_IMAGE_FILE_HEADER,
        STRUCTID_IMAGE_OPTIONAL_HEADER32,
        STRUCTID_IMAGE_OPTIONAL_HEADER64,
        STRUCTID_IMAGE_SECTION_HEADER,
        STRUCTID_IMAGE_DATA_DIRECTORY,
        STRUCTID_IMAGE_RESOURCE_DIRECTORY,
        STRUCTID_IMAGE_EXPORT_DIRECTORY,
        STRUCTID_IMAGE_IMPORT_DESCRIPTOR,
        STRUCTID_IMAGE_DELAYLOAD_DESCRIPTOR,
        STRUCTID_IMAGE_BOUND_IMPORT_DESCRIPTOR,
        STRUCTID_IMAGE_DEBUG_DIRECTORY,
        STRUCTID_IMAGE_TLS_DIRECTORY32,
        STRUCTID_IMAGE_TLS_DIRECTORY64,
        STRUCTID_IMAGE_LOAD_CONFIG_DIRECTORY32,
        STRUCTID_IMAGE_LOAD_CONFIG_DIRECTORY64,
        STRUCTID_IMAGE_RUNTIME_FUNCTION_ENTRY,
        STRUCTID_WIN_CERT_RECORD,
        STRUCTID_IMAGE_COR20_HEADER,
        STRUCTID_NET_METADATA,
        STRUCTID_NET_METADATA_STREAM,
        STRUCTID_NET_RESOURCES,
        STRUCTID_NET_STRONGNAMESIGNATURE,
        STRUCTID_NET_CODEMANAGERTABLE,
        STRUCTID_NET_VTABLEFIXUPS,
        STRUCTID_NET_EXPORTADDRESSTABLEJUMPS,
        STRUCTID_NET_MANAGEDNATIVEHEADER,
        STRUCTID_IMAGE_RESOURCE_DIRECTORY_ENTRY,
        STRUCTID_IMAGE_RESOURCE_DATA_ENTRY,
        STRUCTID_IMAGE_BASE_RELOCATION,
    };

    struct SECTION_RECORD {
        QString sName;
        qint64 nOffset;
        qint64 nRVA;
        qint64 nSize;
        qint64 nCharacteristics;
    };

    struct SECTIONRVA_RECORD {
        qint64 nRVA;
        qint64 nSize;
        qint64 nCharacteristics;
    };

    struct IMPORT_RECORD {
        qint64 nOffset;
        qint64 nRVA;
        QString sLibrary;
        QString sFunction;
    };

    struct IMPORT_POSITION {
        qint64 nThunkRVA;
        qint64 nThunkOffset;
        XADDR nThunkValue;
        QString sName;
        quint16 nHint;
        qint64 nOrdinal;
        QString sFunction;
    };

    struct IMPORT_HEADER {
        QString sName;
        quint32 nFirstThunk;  // For patch only!
        QList<IMPORT_POSITION> listPositions;
    };

    struct DELAYIMPORT_POSITION {
        qint64 nNameThunkRVA;
        qint64 nNameThunkOffset;
        quint64 nNameThunkValue;
        QString sName;
        quint16 nHint;
        qint64 nOrdinal;
        QString sFunction;
        qint64 nAddressThunkRVA;
        qint64 nAddressThunkOffset;
        quint64 nAddressThunkValue;
        qint64 nBoundThunkRVA;
        qint64 nBoundThunkOffset;
        quint64 nBoundThunkValue;
    };

    struct EXPORT_RECORD {
        quint32 nOrdinal;
        QString sFunctionName;
        qint64 nLibraryBase;
        QString sLibraryName;
    };

    struct EXPORT_POSITION {
        quint16 nOrdinal;
        quint32 nRVA;
        XADDR nAddress;
        quint32 nNameRVA;
        QString sFunctionName;
    };

    struct EXPORT_HEADER {
        XPE_DEF::IMAGE_EXPORT_DIRECTORY directory;
        QString sName;
        QList<EXPORT_POSITION> listPositions;
    };

    struct TLS_HEADER {
        quint64 StartAddressOfRawData;
        quint64 EndAddressOfRawData;
        quint64 AddressOfIndex;
        quint64 AddressOfCallBacks;
        quint32 SizeOfZeroFill;
        quint32 Characteristics;
    };

    struct RESOURCES_ID_NAME {
        bool bIsName;
        quint32 nID;
        QString sName;
        quint32 nNameOffset;
    };

    struct RESOURCE_RECORD {
        RESOURCES_ID_NAME irin[3];
        qint64 nIRDEOffset;
        XADDR nAddress;
        qint64 nRVA;
        qint64 nOffset;
        qint64 nSize;
    };

    struct RESOURCE_POSITION {
        qint64 nOffset;
        bool bIsValid;
        quint32 nLevel;
        bool bIsDataDirectory;
        RESOURCES_ID_NAME rin;
        XPE_DEF::IMAGE_RESOURCE_DIRECTORY directory;
        XPE_DEF::IMAGE_RESOURCE_DIRECTORY_ENTRY dirEntry;
        XPE_DEF::IMAGE_RESOURCE_DATA_ENTRY dataEntry;
        XADDR nDataAddress;
        qint64 nDataOffset;
        QList<RESOURCE_POSITION> listPositions;
    };

    struct RESOURCE_HEADER {
        qint64 nOffset;
        XPE_DEF::IMAGE_RESOURCE_DIRECTORY directory;
        QList<RESOURCE_POSITION> listPositions;
    };

    struct RESOURCE_STRINGTABLE_RECORD {
        quint32 nID;
        quint32 nLanguage;
        QString sString;
        XADDR nAddress;
        quint64 nOffset;
        quint32 nSize;
    };

    struct RELOCS_POSITION {
        qint16 nTypeOffset;
        quint32 nType;
        XADDR nAddress;
    };

    struct RELOCS_HEADER {
        qint64 nOffset;
        XPE_DEF::IMAGE_BASE_RELOCATION baseRelocation;
        qint32 nCount;
    };

    // TODO remove obsolete
    struct DUMP_OPTIONS {
        quint32 nImageBase;
        quint32 nAddressOfEntryPoint;
        // TODO Check
        //        bool bClearHeader;
        //        bool bCheckImport;
        //        QMap<qint64,QString> mapImport;
        //        QMap<qint64,quint64> mapInitImportOffsets;
        //        bool bIs64;
        //        bool bSaveIAT;
    };

    struct RESOURCES_VERSION {
        qint64 nFixedFileInfoOffset;
        XPE_DEF::tagVS_FIXEDFILEINFO fileInfo;
        QList<QString> listRecords;  // TODO rename
        // TODO VarFileInfo
    };

    struct CLI_METADATA_HEADER {
        quint32 nSignature;
        quint16 nMajorVersion;
        quint16 nMinorVersion;
        quint32 nReserved;
        quint32 nVersionStringLength;
        QString sVersion;
        quint16 nFlags;
        quint16 nStreams;
    };

    struct CLI_METADATA_STREAM {
        qint64 nOffset;
        qint64 nSize;
        QString sName;
    };

    struct CLI_METADATA {
        CLI_METADATA_HEADER header;
        QList<CLI_METADATA_STREAM> listStreams;
        OFFSETSIZE osMetadata;
        quint32 nTables_Reserved1;
        quint8 cTables_MajorVersion;
        quint8 cTables_MinorVersion;
        quint8 cTables_HeapOffsetSizes;
        quint8 cTables_Reserved2;
        quint64 nTables_Valid;
        quint64 nTables_Sorted;
        quint32 nTables_Valid_NumberOfRows;        // TODO remove
        quint32 Tables_TablesNumberOfIndexes[64];  // TODO const
        qint64 Tables_TablesOffsets[64];           // TODO const
        qint64 Tables_TableElementSizes[64];       // TODO const
        OFFSETSIZE osStrings;
        OFFSETSIZE osUS;
        OFFSETSIZE osBlob;
        OFFSETSIZE osGUID;
        QByteArray baMetadata;
        QByteArray baStrings;
        QByteArray baUS;
        QByteArray baBlob;
        QByteArray baGUID;
        qint64 nEntryPoint;
        qint64 nEntryPointSize;
        // QList<QString> listAnsiStrings;
        // QList<QString> listUnicodeStrings;
        // QList<QString> listGUIDs;
        qint32 nStringIndexSize;
        qint32 nGUIDIndexSize;
        qint32 nBLOBIndexSize;
        qint32 nResolutionScopeSize;
        qint32 nTypeDefOrRefSize;
        qint32 nMethodDefOrRefSize;
        qint32 nMemberRefParentSize;
        qint32 nHasConstantSize;
        qint32 nHasCustomAttributeSize;
        qint32 nCustomAttributeTypeSize;
        qint32 nHasFieldMarshalSize;
        qint32 nHasDeclSecuritySize;
        qint32 nHasSemanticsSize;
        qint32 nMemberForwardedSize;
        quint32 indexSize[64];
    };

    struct CLI_INFO {
        bool bValid;
        bool bHidden;
        qint64 nHeaderOffset;
        XPE_DEF::IMAGE_COR20_HEADER header;
        qint64 nMetaDataOffset;
        CLI_METADATA metaData;
    };

    struct IMAGE_IMPORT_DESCRIPTOR_EX {
        union {
            quint32 Characteristics;
            quint32 OriginalFirstThunk;
        };
        quint32 TimeDateStamp;
        quint32 ForwarderChain;
        quint32 Name;
        quint32 FirstThunk;
        QString sLibrary;  // Extra
    };

    struct BOUND_IMPORT_POSITION {
        XPE_DEF::IMAGE_BOUND_IMPORT_DESCRIPTOR descriptor;
        QString sName;
    };

    struct CERT_TAG {
        bool bIsValid;
        quint32 nTag;
        qint64 nOffset;
        qint32 nHeaderSize;
        qint32 nSize;
    };

    struct CERT_RECORD {
        CERT_TAG certTag;
        QVariant varValue;
        QList<CERT_RECORD> listRecords;
    };

    struct CERT {
        qint64 nOffset;
        XPE_DEF::WIN_CERT_RECORD record;
        bool bIsValid;
        CERT_RECORD certRecord;
    };

    enum TYPE {
        // mb TODO CEDLL
        TYPE_UNKNOWN = 0,
        TYPE_APPLICATION,
        TYPE_XBOX_APPLICATION,
        TYPE_EFI_APPLICATION,
        TYPE_GUI,
        TYPE_CE_GUI,
        TYPE_CONSOLE,
        TYPE_DLL,
        TYPE_DRIVER,
        TYPE_NATIVE,
        TYPE_BOOTAPPLICATION,
        //        TYPE_SERVICEDRIVER,
        TYPE_EFI_RUNTIMEDRIVER,
        TYPE_EFI_BOOTSERVICEDRIVER
        //        TYPE_XBOX,
        //        TYPE_OS2,
        //        TYPE_POSIX,
        //        TYPE_CE
        // TODO more from subsystems
    };

    explicit XPE(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1, PDSTRUCT *pPdStruct = nullptr);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

    virtual MODE getMode();
    virtual QString getArch();
    virtual ENDIAN getEndian();

    quint16 _getMachine(quint16 nMachine);

    virtual FT getFileType();
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);

    virtual bool isSigned();
    virtual OFFSETSIZE getSignOffsetSize();
    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);

    virtual FILEFORMATINFO getFileFormatInfo(PDSTRUCT *pPdStruct);

    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;

    qint64 getNtHeadersOffset();
    quint32 getNtHeaders_Signature();
    void setNtHeaders_Signature(quint32 nValue);
    qint64 getFileHeaderOffset();
    qint64 getFileHeaderSize();

    XPE_DEF::IMAGE_FILE_HEADER getFileHeader();
    void setFileHeader(XPE_DEF::IMAGE_FILE_HEADER *pFileHeader);

    XPE_DEF::IMAGE_FILE_HEADER _read_IMAGE_FILE_HEADER(qint64 nOffset);

    quint16 getFileHeader_Machine();
    quint16 getFileHeader_NumberOfSections();
    quint32 getFileHeader_TimeDateStamp();
    quint32 getFileHeader_PointerToSymbolTable();
    quint32 getFileHeader_NumberOfSymbols();
    quint16 getFileHeader_SizeOfOptionalHeader();
    quint16 getFileHeader_Characteristics();

    void setFileHeader_Machine(quint16 nValue);
    void setFileHeader_NumberOfSections(quint16 nValue);
    void setFileHeader_TimeDateStamp(quint32 nValue);
    void setFileHeader_PointerToSymbolTable(quint32 nValue);
    void setFileHeader_NumberOfSymbols(quint32 nValue);
    void setFileHeader_SizeOfOptionalHeader(quint16 nValue);
    void setFileHeader_Characteristics(quint16 nValue);

    qint64 getOptionalHeaderOffset();
    qint64 getOptionalHeaderSize();

    XPE_DEF::IMAGE_OPTIONAL_HEADER32 getOptionalHeader32();
    XPE_DEF::IMAGE_OPTIONAL_HEADER64 getOptionalHeader64();

    void setOptionalHeader32(XPE_DEF::IMAGE_OPTIONAL_HEADER32 *pOptionalHeader32);
    void setOptionalHeader64(XPE_DEF::IMAGE_OPTIONAL_HEADER64 *pOptionalHeader64);

    XPE_DEF::IMAGE_OPTIONAL_HEADER32S getOptionalHeader32S();
    XPE_DEF::IMAGE_OPTIONAL_HEADER64S getOptionalHeader64S();

    void setOptionalHeader32S(XPE_DEF::IMAGE_OPTIONAL_HEADER32S *pOptionalHeader32S);
    void setOptionalHeader64S(XPE_DEF::IMAGE_OPTIONAL_HEADER64S *pOptionalHeader64S);

    quint16 getOptionalHeader_Magic();
    quint8 getOptionalHeader_MajorLinkerVersion();
    quint8 getOptionalHeader_MinorLinkerVersion();
    quint32 getOptionalHeader_SizeOfCode();
    quint32 getOptionalHeader_SizeOfInitializedData();
    quint32 getOptionalHeader_SizeOfUninitializedData();
    quint32 getOptionalHeader_AddressOfEntryPoint();
    quint32 getOptionalHeader_BaseOfCode();
    quint32 getOptionalHeader_BaseOfData();
    quint64 getOptionalHeader_ImageBase();
    quint32 getOptionalHeader_SectionAlignment();
    quint32 getOptionalHeader_FileAlignment();
    quint16 getOptionalHeader_MajorOperatingSystemVersion();
    quint16 getOptionalHeader_MinorOperatingSystemVersion();
    quint16 getOptionalHeader_MajorImageVersion();
    quint16 getOptionalHeader_MinorImageVersion();
    quint16 getOptionalHeader_MajorSubsystemVersion();
    quint16 getOptionalHeader_MinorSubsystemVersion();
    quint32 getOptionalHeader_Win32VersionValue();
    quint32 getOptionalHeader_SizeOfImage();
    quint32 getOptionalHeader_SizeOfHeaders();
    quint32 getOptionalHeader_CheckSum();
    quint16 getOptionalHeader_Subsystem();
    quint16 getOptionalHeader_DllCharacteristics();
    qint64 getOptionalHeader_SizeOfStackReserve();
    qint64 getOptionalHeader_SizeOfStackCommit();
    qint64 getOptionalHeader_SizeOfHeapReserve();
    qint64 getOptionalHeader_SizeOfHeapCommit();
    quint32 getOptionalHeader_LoaderFlags();
    quint32 getOptionalHeader_NumberOfRvaAndSizes();

    void setOptionalHeader_Magic(quint16 nValue);
    void setOptionalHeader_MajorLinkerVersion(quint8 nValue);
    void setOptionalHeader_MinorLinkerVersion(quint8 nValue);
    void setOptionalHeader_SizeOfCode(quint32 nValue);
    void setOptionalHeader_SizeOfInitializedData(quint32 nValue);
    void setOptionalHeader_SizeOfUninitializedData(quint32 nValue);
    void setOptionalHeader_AddressOfEntryPoint(quint32 nValue);
    void setOptionalHeader_BaseOfCode(quint32 nValue);
    void setOptionalHeader_BaseOfData(quint32 nValue);
    void setOptionalHeader_ImageBase(quint64 nValue);
    void setOptionalHeader_SectionAlignment(quint32 nValue);
    void setOptionalHeader_FileAlignment(quint32 nValue);
    void setOptionalHeader_MajorOperatingSystemVersion(quint16 nValue);
    void setOptionalHeader_MinorOperatingSystemVersion(quint16 nValue);
    void setOptionalHeader_MajorImageVersion(quint16 nValue);
    void setOptionalHeader_MinorImageVersion(quint16 nValue);
    void setOptionalHeader_MajorSubsystemVersion(quint16 nValue);
    void setOptionalHeader_MinorSubsystemVersion(quint16 nValue);
    void setOptionalHeader_Win32VersionValue(quint32 nValue);
    void setOptionalHeader_SizeOfImage(quint32 nValue);
    void setOptionalHeader_SizeOfHeaders(quint32 nValue);
    void setOptionalHeader_CheckSum(quint32 nValue);
    void setOptionalHeader_Subsystem(quint16 nValue);
    void setOptionalHeader_DllCharacteristics(quint16 nValue);
    void setOptionalHeader_SizeOfStackReserve(quint64 nValue);
    void setOptionalHeader_SizeOfStackCommit(quint64 nValue);
    void setOptionalHeader_SizeOfHeapReserve(quint64 nValue);
    void setOptionalHeader_SizeOfHeapCommit(quint64 nValue);
    void setOptionalHeader_LoaderFlags(quint32 nValue);
    void setOptionalHeader_NumberOfRvaAndSizes(quint32 nValue);

    quint32 getOperatingSystemVersion();
    void setOperatingSystemVersion(quint32 nValue);

    XPE_DEF::IMAGE_DATA_DIRECTORY read_IMAGE_DATA_DIRECTORY(qint64 nOffset);
    void write_IMAGE_DATA_DIRECTORY(qint64 nOffset, XPE_DEF::IMAGE_DATA_DIRECTORY *pDataDirectory);

    XPE_DEF::IMAGE_DATA_DIRECTORY getOptionalHeader_DataDirectory(quint32 nNumber);
    void setOptionalHeader_DataDirectory(quint32 nNumber, XPE_DEF::IMAGE_DATA_DIRECTORY *pDataDirectory);
    void setOptionalHeader_DataDirectory_VirtualAddress(quint32 nNumber, quint32 nValue);
    void setOptionalHeader_DataDirectory_Size(quint32 nNumber, quint32 nValue);

    void clearOptionalHeader_DataDirectory(quint32 nNumber);
    bool isOptionalHeader_DataDirectoryPresent(quint32 nNumber);

    QList<XPE_DEF::IMAGE_DATA_DIRECTORY> getDirectories();  // TODO pdStruct
    void setDirectories(QList<XPE_DEF::IMAGE_DATA_DIRECTORY> *pListDirectories);

    qint64 getDataDirectoryOffset(quint32 nNumber);
    qint64 getDataDirectoryOffset(_MEMORY_MAP *pMemoryMap, quint32 nNumber);

    qint64 getDataDirectoryHeaderOffset(quint32 nNumber);
    qint64 getDataDirectoryHeaderSize();

    QByteArray getDataDirectory(quint32 nNumber);
    QByteArray getDataDirectory(_MEMORY_MAP *pMemoryMap, quint32 nNumber);

    OFFSETSIZE getStringTable();

    qint64 getSectionsTableOffset();
    qint64 getSectionHeaderOffset(quint32 nNumber);
    qint64 getSectionHeaderSize();

    bool isSectionsTablePresent();

    XPE_DEF::IMAGE_SECTION_HEADER getSectionHeader(quint32 nNumber);
    void setSectionHeader(quint32 nNumber, XPE_DEF::IMAGE_SECTION_HEADER *pSectionHeader);

    XPE_DEF::IMAGE_SECTION_HEADER read_IMAGE_SECTION_HEADER(qint64 nOffset);

    QList<XPE_DEF::IMAGE_SECTION_HEADER> getSectionHeaders(PDSTRUCT *pPdStruct = nullptr);
    // TODO with __getSectionOffsetAndSize TODO Check !!!
    QList<SECTION_RECORD> getSectionRecords(QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders, PDSTRUCT *pPdStruct = nullptr);
    static QList<QString> getSectionNames(QList<XPE::SECTION_RECORD> *pListSectionRecords, PDSTRUCT *pPdStruct = nullptr);

    QList<SECTIONRVA_RECORD> getSectionRVARecords();

    QString convertSectionName(const QString &sName, OFFSETSIZE *pOsStringTable);

    QString getSection_NameAsString(quint32 nNumber);
    quint32 getSection_VirtualSize(quint32 nNumber);
    quint32 getSection_VirtualAddress(quint32 nNumber);
    quint32 getSection_SizeOfRawData(quint32 nNumber);
    quint32 getSection_PointerToRawData(quint32 nNumber);
    quint32 getSection_PointerToRelocations(quint32 nNumber);
    quint32 getSection_PointerToLinenumbers(quint32 nNumber);
    quint16 getSection_NumberOfRelocations(quint32 nNumber);
    quint16 getSection_NumberOfLinenumbers(quint32 nNumber);
    quint32 getSection_Characteristics(quint32 nNumber);

    void setSection_NameAsString(quint32 nNumber, const QString &sName);
    void setSection_VirtualSize(quint32 nNumber, quint32 nValue);
    void setSection_VirtualAddress(quint32 nNumber, quint32 nValue);
    void setSection_SizeOfRawData(quint32 nNumber, quint32 nValue);
    void setSection_PointerToRawData(quint32 nNumber, quint32 nValue);
    void setSection_PointerToRelocations(quint32 nNumber, quint32 nValue);
    void setSection_PointerToLinenumbers(quint32 nNumber, quint32 nValue);
    void setSection_NumberOfRelocations(quint32 nNumber, quint16 nValue);
    void setSection_NumberOfLinenumbers(quint32 nNumber, quint16 nValue);
    void setSection_Characteristics(quint32 nNumber, quint32 nValue);

    QString getSection_NameAsString(quint32 nNumber, QList<QString> *pListSectionNameStrings);
    quint32 getSection_VirtualSize(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_VirtualAddress(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_SizeOfRawData(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_PointerToRawData(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_PointerToRelocations(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_PointerToLinenumbers(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint16 getSection_NumberOfRelocations(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint16 getSection_NumberOfLinenumbers(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);
    quint32 getSection_Characteristics(quint32 nNumber, QList<XPE_DEF::IMAGE_SECTION_HEADER> *pListSectionHeaders);

    bool isSectionNamePresent(const QString &sSectionName);
    static bool isSectionNamePresent(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);

    static SECTION_RECORD getSectionRecordByName(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);

    qint32 getSectionNumber(const QString &sSectionName);
    static qint32 getSectionNumber(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);

    static QString sectionCharacteristicToString(quint32 nValue);  // TODO remove

    bool isImportPresent();
    bool isIATPresent();

    QList<IMPORT_RECORD> getImportRecords(PDSTRUCT *pPdStruct = nullptr);
    QList<IMPORT_RECORD> getImportRecords(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    quint64 getImportHash64(QList<IMPORT_RECORD> *pListImportRecords, PDSTRUCT *pPdStruct = nullptr);
    quint32 getImportHash32(QList<IMPORT_RECORD> *pListImportRecords, PDSTRUCT *pPdStruct = nullptr);

    qint64 getImportDescriptorOffset(quint32 nNumber);
    qint64 getImportDescriptorSize();

    QList<XPE_DEF::IMAGE_IMPORT_DESCRIPTOR> getImportDescriptors();
    QList<XPE_DEF::IMAGE_IMPORT_DESCRIPTOR> getImportDescriptors(_MEMORY_MAP *pMemoryMap);
    QList<IMAGE_IMPORT_DESCRIPTOR_EX> getImportDescriptorsEx();
    QList<IMAGE_IMPORT_DESCRIPTOR_EX> getImportDescriptorsEx(_MEMORY_MAP *pMemoryMap);

    XPE_DEF::IMAGE_IMPORT_DESCRIPTOR getImportDescriptor(quint32 nNumber);
    void setImportDescriptor(quint32 nNumber, XPE_DEF::IMAGE_IMPORT_DESCRIPTOR *pImportDescriptor);

    void setImportDescriptor_OriginalFirstThunk(quint32 nNumber, quint32 nValue);
    void setImportDescriptor_TimeDateStamp(quint32 nNumber, quint32 nValue);
    void setImportDescriptor_ForwarderChain(quint32 nNumber, quint32 nValue);
    void setImportDescriptor_Name(quint32 nNumber, quint32 nValue);
    void setImportDescriptor_FirstThunk(quint32 nNumber, quint32 nValue);

    QList<IMPORT_HEADER> getImports(PDSTRUCT *pPdStruct = nullptr);
    QList<IMPORT_HEADER> getImports(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);
    XPE_DEF::IMAGE_DATA_DIRECTORY getIAT(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    QList<QString> getImportNames(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    QList<IMPORT_POSITION> _getImportPositions(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nThunksRVA, qint64 nRVA, PDSTRUCT *pPdStruct = nullptr);
    QList<IMPORT_POSITION> getImportPositions(qint32 nIndex, PDSTRUCT *pPdStruct = nullptr);

    QList<quint32> getImportPositionHashes(bool bLibraryName = false);
    QList<quint32> getImportPositionHashes(QList<IMPORT_HEADER> *pListImport, bool bLibraryName = false);
    static bool isImportPositionHashPresent(QList<quint32> *pListImportHashes, qint32 nIndex, quint32 nHash, PDSTRUCT *pPdStruct = nullptr);

    bool isImportLibraryPresent(const QString &sLibrary, PDSTRUCT *pPdStruct = nullptr);
    static bool isImportLibraryPresent(const QString &sLibrary, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct = nullptr);

    bool isImportLibraryPresentI(const QString &sLibrary, PDSTRUCT *pPdStruct = nullptr);
    static bool isImportLibraryPresentI(const QString &sLibrary, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct = nullptr);

    bool isImportFunctionPresentI(const QString &sLibrary, const QString &sFunction, PDSTRUCT *pPdStruct = nullptr);
    static bool isImportFunctionPresentI(const QString &sLibrary, const QString &sFunction, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct = nullptr);
    static bool isFunctionPresent(const QString &sFunction, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct = nullptr);

    bool setImports(QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct);
    bool setImports(QIODevice *pDevice, bool bIsImage, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct);
    bool setImports(const QString &sFileName, bool bIsImage, QList<IMPORT_HEADER> *pListImportHeaders, PDSTRUCT *pPdStruct);

    QString getImportFunctionName(quint32 nImport, quint32 nFunctionNumber, QList<IMPORT_HEADER> *pListImportHeaders);

    RESOURCE_HEADER getResourceHeader();
    RESOURCE_HEADER getResourceHeader(_MEMORY_MAP *pMemoryMap);
    QList<RESOURCE_RECORD> getResources(qint32 nLimit, PDSTRUCT *pPdStruct = nullptr);
    QList<RESOURCE_RECORD> getResources(_MEMORY_MAP *pMemoryMap, qint32 nLimit = 10000, PDSTRUCT *pPdStruct = nullptr);

    static RESOURCE_RECORD getResourceRecord(quint32 nID1, quint32 nID2, QList<RESOURCE_RECORD> *pListResourceRecords);                    // TODO pdstruct
    static RESOURCE_RECORD getResourceRecord(quint32 nID1, const QString &sName2, QList<RESOURCE_RECORD> *pListResourceRecords);           // TODO pdstruct
    static RESOURCE_RECORD getResourceRecord(const QString &sName1, quint32 nID2, QList<RESOURCE_RECORD> *pListResourceRecords);           // TODO pdstruct
    static RESOURCE_RECORD getResourceRecord(const QString &sName1, const QString &sName2, QList<RESOURCE_RECORD> *pListResourceRecords);  // TODO pdstruct

    static QList<RESOURCE_RECORD> getResourceRecords(quint32 nID1, quint32 nID2, QList<RESOURCE_RECORD> *pListResourceRecords);

    static bool isResourcePresent(quint32 nID1, quint32 nID2, QList<RESOURCE_RECORD> *pListResourceRecords);
    static bool isResourcePresent(quint32 nID1, const QString &sName2, QList<RESOURCE_RECORD> *pListResourceRecords);
    static bool isResourcePresent(const QString &sName1, quint32 nID2, QList<RESOURCE_RECORD> *pListResourceRecords);
    static bool isResourcePresent(const QString &sName1, const QString &sName2, QList<RESOURCE_RECORD> *pListResourceRecords);

    bool isResourceStringTablePresent();
    bool isResourceStringTablePresent(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    QList<XPE::RESOURCE_STRINGTABLE_RECORD> getResourceStringTableRecords();
    QList<XPE::RESOURCE_STRINGTABLE_RECORD> getResourceStringTableRecords(QList<XPE::RESOURCE_RECORD> *pListResourceRecords, XBinary::_MEMORY_MAP *pMemoryMap);

    bool isResourceManifestPresent();
    bool isResourceManifestPresent(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    QString getResourceManifest();
    QString getResourceManifest(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    bool isResourceVersionPresent();
    bool isResourceVersionPresent(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    RESOURCES_VERSION getResourcesVersion();
    XPE_DEF::S_VS_VERSION_INFO readVS_VERSION_INFO(qint64 nOffset);

    RESOURCES_VERSION getResourcesVersion(QList<XPE::RESOURCE_RECORD> *pListResourceRecords, PDSTRUCT *pPdStruct = nullptr);

    QString getFileVersion();
    QString getFileVersion(RESOURCES_VERSION *pResourceVersion);
    QString getFileVersionMS(RESOURCES_VERSION *pResourceVersion);

    void setFixedFileInfo_dwSignature(quint32 nValue);
    void setFixedFileInfo_dwStrucVersion(quint32 nValue);
    void setFixedFileInfo_dwFileVersionMS(quint32 nValue);
    void setFixedFileInfo_dwFileVersionLS(quint32 nValue);
    void setFixedFileInfo_dwProductVersionMS(quint32 nValue);
    void setFixedFileInfo_dwProductVersionLS(quint32 nValue);
    void setFixedFileInfo_dwFileFlagsMask(quint32 nValue);
    void setFixedFileInfo_dwFileFlags(quint32 nValue);
    void setFixedFileInfo_dwFileOS(quint32 nValue);
    void setFixedFileInfo_dwFileType(quint32 nValue);
    void setFixedFileInfo_dwFileSubtype(quint32 nValue);
    void setFixedFileInfo_dwFileDateMS(quint32 nValue);
    void setFixedFileInfo_dwFileDateLS(quint32 nValue);

    QString getResourcesVersionValue(const QString &sKey);
    static QString getResourcesVersionValue(const QString &sKey, XPE::RESOURCES_VERSION *pResourcesVersion);

    quint32 getResourceIdByNumber(quint32 nNumber);
    quint32 getResourceIdByNumber(quint32 nNumber, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    QString getResourceNameByNumber(quint32 nNumber);
    QString getResourceNameByNumber(quint32 nNumber, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    qint64 getResourceOffsetByNumber(quint32 nNumber);
    qint64 getResourceOffsetByNumber(quint32 nNumber, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    qint64 getResourceSizeByNumber(quint32 nNumber);
    qint64 getResourceSizeByNumber(quint32 nNumber, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    quint32 getResourceTypeByNumber(quint32 nNumber);
    quint32 getResourceTypeByNumber(quint32 nNumber, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    qint64 getResourceNameOffset(const QString &sName);
    qint64 getResourceNameOffset(const QString &sName, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    qint64 getResourceGroupNameOffset(const QString &sName);
    qint64 getResourceGroupNameOffset(const QString &sName, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    qint64 getResourceGroupIdOffset(quint32 nID);
    qint64 getResourceGroupIdOffset(quint32 nID, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    bool isResourceNamePresent(const QString &sName);
    bool isResourceNamePresent(const QString &sName, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    bool isResourceGroupNamePresent(const QString &sName);
    bool isResourceGroupNamePresent(const QString &sName, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    bool isResourceGroupIdPresent(quint32 nID);
    bool isResourceGroupIdPresent(quint32 nID, QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    bool isResourceGroupIconsPresent();
    bool isResourceGroupIconsPresent(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);
    bool isResourceGroupCursorsPresent();
    bool isResourceGroupCursorsPresent(QList<XPE::RESOURCE_RECORD> *pListResourceRecords);

    static QString resourceRecordToString(const XPE::RESOURCE_RECORD &resourceRecord);

    XADDR getModuleAddress();

    virtual QList<MAPMODE> getMapModesList();
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual XADDR getBaseAddress();
    virtual void setBaseAddress(XADDR nBaseAddress);
    virtual void setEntryPointOffset(qint64 nEntryPointOffset);
    XPE_DEF::IMAGE_IMPORT_DESCRIPTOR read_IMAGE_IMPORT_DESCRIPTOR(qint64 nOffset);
    void write_IMAGE_IMPORT_DESCRIPTOR(qint64 nOffset, XPE_DEF::IMAGE_IMPORT_DESCRIPTOR idd);

    XPE_DEF::S_IMAGE_DELAYLOAD_DESCRIPTOR _read_IMAGE_DELAYLOAD_DESCRIPTOR(qint64 nOffset);

    bool isExportPresent();

    EXPORT_HEADER getExport(bool bValidOnly = false, PDSTRUCT *pPdStruct = nullptr);
    EXPORT_HEADER getExport(_MEMORY_MAP *pMemoryMap, bool bValidOnly = false, PDSTRUCT *pPdStruct = nullptr);
    QList<QString> getExportFunctionsList(PDSTRUCT *pPdStruct = nullptr);
    static QList<QString> getExportFunctionsList(EXPORT_HEADER *pExportHeader, PDSTRUCT *pPdStruct = nullptr);
    QList<XADDR> getExportFunctionAddressesList(PDSTRUCT *pPdStruct = nullptr);
    QList<XADDR> getExportFunctionAddressesList(_MEMORY_MAP *pMemoryMap, XPE_DEF::IMAGE_EXPORT_DIRECTORY *pIED, PDSTRUCT *pPdStruct = nullptr);
    QList<quint16> getExportNameOrdinalsList(_MEMORY_MAP *pMemoryMap, XPE_DEF::IMAGE_EXPORT_DIRECTORY *pIED, PDSTRUCT *pPdStruct = nullptr);
    QList<XADDR> getExportNamesList(_MEMORY_MAP *pMemoryMap, XPE_DEF::IMAGE_EXPORT_DIRECTORY *pIED, PDSTRUCT *pPdStruct = nullptr);

    XPE_DEF::IMAGE_EXPORT_DIRECTORY read_IMAGE_EXPORT_DIRECTORY(qint64 nOffset);
    void write_IMAGE_EXPORT_DIRECTORY(qint64 nOffset, XPE_DEF::IMAGE_EXPORT_DIRECTORY *pIED);

    XPE_DEF::IMAGE_EXPORT_DIRECTORY getExportDirectory();

    void setExportDirectory(XPE_DEF::IMAGE_EXPORT_DIRECTORY *pExportDirectory);

    void setExportDirectory_Characteristics(quint32 nValue);
    void setExportDirectory_TimeDateStamp(quint32 nValue);
    void setExportDirectory_MajorVersion(quint16 nValue);
    void setExportDirectory_MinorVersion(quint16 nValue);
    void setExportDirectory_Name(quint32 nValue);
    void setExportDirectory_Base(quint32 nValue);
    void setExportDirectory_NumberOfFunctions(quint32 nValue);
    void setExportDirectory_NumberOfNames(quint32 nValue);
    void setExportDirectory_AddressOfFunctions(quint32 nValue);
    void setExportDirectory_AddressOfNames(quint32 nValue);
    void setExportDirectory_AddressOfNameOrdinals(quint32 nValue);

    QByteArray getHeaders();

    OFFSETSIZE __getSectionOffsetAndSize(quint32 nSection);  // TODO move to Xbinary

    QByteArray getSection(quint32 nSection);  // TODO move to Xbinary

    bool addImportSection(QMap<qint64, QString> *pMapIAT, PDSTRUCT *pPdStruct);
    bool addImportSection(QIODevice *pDevice, bool bIsImage, QMap<qint64, QString> *pMapIAT, PDSTRUCT *pPdStruct);
    bool addImportSection(const QString &sFileName, bool bIsImage, QMap<qint64, QString> *pMapIAT, PDSTRUCT *pPdStruct);

    static QList<XPE::IMPORT_HEADER> mapIATToList(QMap<qint64, QString> *pMapIAT, bool bIs64);

    quint32 calculateCheckSum(PDSTRUCT *pPdStruct = nullptr);

    bool addSection(XPE_DEF::IMAGE_SECTION_HEADER *pSectionHeader, char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct);
    bool addSection(const QString &sFileName, bool bIsImage, XPE_DEF::IMAGE_SECTION_HEADER *pSectionHeader, char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct);
    bool addSection(QIODevice *pDevice, bool bIsImage, XPE_DEF::IMAGE_SECTION_HEADER *pSectionHeader, char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct);

    bool removeLastSection(PDSTRUCT *pPdStruct);
    static bool removeLastSection(QIODevice *pDevice, bool bIsImage, PDSTRUCT *pPdStruct);
    static bool removeLastSection(const QString &sFileName, bool bIsImage, PDSTRUCT *pPdStruct);
    // TODO copy Overlay function -> XBinary
    XPE_DEF::IMAGE_RESOURCE_DIRECTORY_ENTRY read_IMAGE_RESOURCE_DIRECTORY_ENTRY(qint64 nOffset);
    XPE_DEF::IMAGE_RESOURCE_DIRECTORY read_IMAGE_RESOURCE_DIRECTORY(qint64 nOffset);
    XPE_DEF::IMAGE_RESOURCE_DATA_ENTRY read_IMAGE_RESOURCE_DATA_ENTRY(qint64 nOffset);
    XPE::RESOURCES_ID_NAME getResourcesIDName(qint64 nResourceOffset, quint32 nValue);

    static QString resourceIdNameToString(const RESOURCES_ID_NAME &resourceIdName, qint32 nNumber);

    QList<qint64> getRelocsAsRVAList();
    QList<RELOCS_HEADER> getRelocsHeaders(PDSTRUCT *pPdStruct = nullptr);
    QList<RELOCS_POSITION> getRelocsPositions(qint64 nOffset);

    XPE_DEF::IMAGE_BASE_RELOCATION _readIMAGE_BASE_RELOCATION(qint64 nOffset);

    quint32 getRelocsVirtualAddress(qint64 nOffset);
    quint32 getRelocsSizeOfBlock(qint64 nOffset);
    void setRelocsVirtualAddress(qint64 nOffset, quint32 nValue);
    void setRelocsSizeOfBlock(qint64 nOffset, quint32 nValue);

    bool addRelocsSection(QList<XADDR> *pList, PDSTRUCT *pPdStruct);
    bool addRelocsSection(QIODevice *pDevice, bool bIsImage, QList<XADDR> *pListRelocs, PDSTRUCT *pPdStruct);
    bool addRelocsSection(const QString &sFileName, bool bIsImage, QList<XADDR> *pListRelocs, PDSTRUCT *pPdStruct);
    static QByteArray relocsAsRVAListToByteArray(QList<XADDR> *pListRelocs, bool bIs64);

    bool isResourcesPresent();
    bool isRelocsPresent();
    bool isDebugPresent();
    bool isTLSPresent();
    bool isSignPresent();
    bool isExceptionPresent();
    bool isLoadConfigPresent();
    bool isBoundImportPresent();
    bool isDelayImportPresent();

    qint64 getTLSHeaderOffset();
    qint64 getTLSHeaderSize();

    XPE_DEF::S_IMAGE_TLS_DIRECTORY32 getTLSDirectory32();
    XPE_DEF::S_IMAGE_TLS_DIRECTORY64 getTLSDirectory64();

    quint64 getTLS_StartAddressOfRawData();
    quint64 getTLS_EndAddressOfRawData();
    quint64 getTLS_AddressOfIndex();
    quint64 getTLS_AddressOfCallBacks();
    quint32 getTLS_SizeOfZeroFill();
    quint32 getTLS_Characteristics();

    void setTLS_StartAddressOfRawData(quint64 nValue);
    void setTLS_EndAddressOfRawData(quint64 nValue);
    void setTLS_AddressOfIndex(quint64 nValue);
    void setTLS_AddressOfCallBacks(quint64 nValue);
    void setTLS_SizeOfZeroFill(quint32 nValue);
    void setTLS_Characteristics(quint32 nValue);

    QList<XADDR> getTLS_CallbacksList();
    QList<XADDR> getTLS_CallbacksList(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    bool isTLSCallbacksPresent();
    bool isTLSCallbacksPresent(XBinary::_MEMORY_MAP *pMemoryMap);

    TLS_HEADER getTLSHeader();

    static const QString PREFIX_ImageNtHeadersSignatures;
    static const QString PREFIX_ImageMagics;
    static const QString PREFIX_ImageFileHeaderMachines;
    static const QString PREFIX_ImageFileHeaderCharacteristics;
    static const QString PREFIX_ImageOptionalHeaderMagic;
    static const QString PREFIX_ImageOptionalHeaderSubsystem;
    static const QString PREFIX_ImageOptionalHeaderDllCharacteristics;
    static const QString PREFIX_ImageOptionalHeaderDataDirectory;
    static const QString PREFIX_ImageSectionHeaderFlags;
    static const QString PREFIX_ImageSectionHeaderAligns;
    static const QString PREFIX_ResourceTypes;
    static const QString PREFIX_ImageRelBased;
    static const QString PREFIX_ComImageFlags;
    static const QString PREFIX_DebugTypes;
    static const QString PREFIX_ResourcesFixedFileInfoSignatures;
    static const QString PREFIX_ResourcesFixedFileInfoFileFlags;
    static const QString PREFIX_ResourcesFixedFileInfoFileOses;
    static const QString PREFIX_ResourcesFixedFileInfoFileTypes;
    static const QString PREFIX_NetMetadataFlags;

    static QMap<quint64, QString> getImageNtHeadersSignatures();
    static QMap<quint64, QString> getImageNtHeadersSignaturesS();
    static QMap<quint64, QString> getImageMagics();
    static QMap<quint64, QString> getImageMagicsS();
    static QMap<quint64, QString> getImageFileHeaderMachines();
    static QMap<quint64, QString> getImageFileHeaderMachinesS();
    static QMap<quint64, QString> getImageFileHeaderCharacteristics();
    static QMap<quint64, QString> getImageFileHeaderCharacteristicsS();
    static QMap<quint64, QString> getImageOptionalHeaderMagic();
    static QMap<quint64, QString> getImageOptionalHeaderMagicS();
    static QMap<quint64, QString> getImageOptionalHeaderSubsystem();
    static QMap<quint64, QString> getImageOptionalHeaderSubsystemS();
    static QMap<quint64, QString> getImageOptionalHeaderDllCharacteristics();
    static QMap<quint64, QString> getImageOptionalHeaderDllCharacteristicsS();
    static QMap<quint64, QString> getImageOptionalHeaderDataDirectory();
    static QMap<quint64, QString> getImageOptionalHeaderDataDirectoryS();
    static QMap<quint64, QString> getImageSectionHeaderFlags();
    static QMap<quint64, QString> getImageSectionHeaderFlagsS();
    static QMap<quint64, QString> getImageSectionHeaderAligns();
    static QMap<quint64, QString> getImageSectionHeaderAlignsS();
    static QMap<quint64, QString> getResourceTypes();
    static QMap<quint64, QString> getResourceTypesS();
    static QMap<quint64, QString> getImageRelBased();
    static QMap<quint64, QString> getImageRelBasedS();
    static QMap<quint64, QString> getComImageFlags();
    static QMap<quint64, QString> getComImageFlagsS();
    static QMap<quint64, QString> getDebugTypes();
    static QMap<quint64, QString> getDebugTypesS();
    static QMap<quint64, QString> getNetMetadataFlags();
    static QMap<quint64, QString> getNetMetadataFlagsS();
    static QMap<quint64, QString> getOperatingSystemVersions(OSNAME osName = OSNAME_WINDOWS);
    static QMap<quint64, QString> getOperatingSystemVersionsS(OSNAME osName = OSNAME_WINDOWS);
    static QMap<quint64, QString> getMajorOperatingSystemVersion();
    static QMap<quint64, QString> getMajorOperatingSystemVersionS();
    static QMap<quint64, QString> getMinorOperatingSystemVersion(quint16 nMajorOperatingSystemVersion);
    static QMap<quint64, QString> getMinorOperatingSystemVersionS(quint16 nMajorOperatingSystemVersion);

    static QMap<quint64, QString> getResourcesFixedFileInfoSignatures();
    static QMap<quint64, QString> getResourcesFixedFileInfoSignaturesS();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileFlags();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileFlagsS();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileOses();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileOsesS();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileTypes();
    static QMap<quint64, QString> getResourcesFixedFileInfoFileTypesS();

    qint64 calculateHeadersSize();
    qint64 _calculateHeadersSize(qint64 nSectionsTableOffset, quint32 nNumberOfSections);

    bool isDll();
    static bool isDll(const QString &sFileName);
    bool isConsole();
    static bool isConsole(const QString &sFileName);
    bool isDriver();
    bool isNETPresent();

    CLI_INFO getCliInfo(bool bFindHidden, PDSTRUCT *pPdStruct = nullptr);
    CLI_INFO getCliInfo(bool bFindHidden, XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    QList<QString> getAnsiStrings(CLI_INFO *pCliInfo, PDSTRUCT *pPdStruct = nullptr);
    QList<QString> getUnicodeStrings(CLI_INFO *pCliInfo, PDSTRUCT *pPdStruct = nullptr);

    bool isNetGlobalCctorPresent(CLI_INFO *pCliInfo, PDSTRUCT *pPdStruct = nullptr);
    bool isNetTypePresent(CLI_INFO *pCliInfo, const QString &sTypeNamespace, const QString &sTypeName, PDSTRUCT *pPdStruct = nullptr);
    bool isNetMethodPresent(CLI_INFO *pCliInfo, QString sTypeNamespace, QString sTypeName, QString sMethodName, PDSTRUCT *pPdStruct = nullptr);
    bool isNetFieldPresent(CLI_INFO *pCliInfo, QString sTypeNamespace, QString sTypeName, QString sFieldName, PDSTRUCT *pPdStruct = nullptr);

    XPE_DEF::S_METADATA_MODULE getMetadataModule(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_MEMBERREF getMetadataMemberRef(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_TYPEDEF getMetadataTypeDef(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_TYPEREF getMetadataTypeRef(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_MODULEREF getMetadataModuleRef(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_METHODDEF getMetadataMethodDef(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_METHODPTR getMetadataMethodPtr(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_PARAM getMetadataParam(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_TYPESPEC getMetadataTypeSpec(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_FIELD getMetadataField(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_METHODIMPL getMetadataMethodImpl(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_ASSEMBLY getMetadataAssembly(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_CONSTANT getMetadataConstant(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_CUSTOMATTRIBUTE getMetadataCustomAttribute(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_FIELDMARSHAL getMetadataFieldMarshal(CLI_INFO *pCliInfo, qint32 nNumber);
    XPE_DEF::S_METADATA_DECLSECURITY getMetadataDeclSecurity(CLI_INFO *pCliInfo, qint32 nNumber);

    QString getMetadataModuleName(CLI_INFO *pCliInfo, qint32 nNumber);
    QString getMetadataAssemblyName(CLI_INFO *pCliInfo, qint32 nNumber);

    XPE_DEF::S_METADATA_METHODDEFORREF getMetadataMethodDefOrRef(CLI_INFO *pCliInfo, quint32 nValue);

    QString getMetadataMemberRefParentName(CLI_INFO *pCliInfo, const XPE_DEF::S_METADATA_MEMBERREF &memberRef);

    static QString mdtIdToString(quint32 nID);

    OFFSETSIZE getNet_MetadataOffsetSize();

    CLI_METADATA_HEADER _read_MetadataHeader(qint64 nOffset);
    void setMetadataHeader_Signature(quint32 nValue);
    void setMetadataHeader_MajorVersion(quint16 nValue);
    void setMetadataHeader_MinorVersion(quint16 nValue);
    void setMetadataHeader_Reserved(quint32 nValue);
    void setMetadataHeader_VersionStringLength(quint32 nValue);
    void setMetadataHeader_Version(const QString &sValue);
    void setMetadataHeader_Flags(quint16 nValue);
    void setMetadataHeader_Streams(quint16 nValue);

    bool isDataDirectoryValid(XPE_DEF::IMAGE_DATA_DIRECTORY *pDataDirectory);
    bool isDataDirectoryValid(XPE_DEF::IMAGE_DATA_DIRECTORY *pDataDirectory, XBinary::_MEMORY_MAP *pMemoryMap);

    bool isNetMetadataPresent(PDSTRUCT *pPdStruct);
    bool isNetMetadataPresent(CLI_INFO *pCliInfo, XBinary::_MEMORY_MAP *pMemoryMap);

    quint32 getNetId();

    qint64 findSignatureInBlob_NET(const QString &sSignature, _MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);
    bool isSignatureInBlobPresent_NET(const QString &sSignature, _MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    qint32 getEntryPointSection();
    qint32 getEntryPointSection(XBinary::_MEMORY_MAP *pMemoryMap);
    qint32 getImportSection();
    qint32 getExportSection();
    qint32 getTLSSection();
    qint32 getIATSection();
    qint32 getResourcesSection();
    qint32 getRelocsSection();
    qint32 getImageDirectoryEntrySection(XBinary::_MEMORY_MAP *pMemoryMap, qint32 nImageDirectoryEntry);
    qint32 getNormalCodeSection();
    qint32 getNormalCodeSection(XBinary::_MEMORY_MAP *pMemoryMap);
    qint32 getNormalDataSection();
    qint32 getNormalDataSection(XBinary::_MEMORY_MAP *pMemoryMap);
    qint32 getConstDataSection();
    qint32 getConstDataSection(XBinary::_MEMORY_MAP *pMemoryMap);

    // TODO .INIT section Check

    struct REBUILD_OPTIONS {
        bool bOptimize;
        bool bClearHeader;
        //        bool bRemoveLastSection;
        bool bSetEntryPoint;
        quint32 nEntryPoint;
        //        bool bAddImportSection;
        QMap<qint64, QString> mapIAT;
        QMap<XADDR, quint64> mapPatches;
        //        bool bAddRelocsSection;
        QList<XADDR> listRelocsRVAs;
        bool bRenameSections;
        QString sSectionName;
        bool bFixChecksum;
    };  // Obsolete TODO remove

    bool rebuildDump(const QString &sResultFile, REBUILD_OPTIONS *pRebuildOptions, PDSTRUCT *pPdStruct);                                    // Obsolete TODO remove
    static bool rebuildDump(const QString &sInputFile, const QString &sResultFile, REBUILD_OPTIONS *pRebuildOptions, PDSTRUCT *pPdStruct);  // Obsolete TODO remove

    static bool fixCheckSum(const QString &sFileName, bool bIsImage);

    struct FIXDUMP_OPTIONS {
        bool bOptimizeSize;
        bool bCleanHeader;
        bool bFixSections;
        bool bSetFileAlignment;
        quint32 nFileAlignment;
        bool bSetSectionAlignment;
        quint32 nSectionAlignment;
        bool bSetEntryPoint;
        XADDR nEntryPoint;
        bool bSetImageBase;
        XADDR nImageBase;
        bool bAddImportSection;
        QString sImportSectionName;
        XPE_DEF::IMAGE_DATA_DIRECTORY ddIAT;
        // TODO functions
    };

    bool fixDump(const QString &sResultFile, const FIXDUMP_OPTIONS &fixDumpOptions, PDSTRUCT *pPdStruct = nullptr);
    FIXDUMP_OPTIONS getFixDumpOptions(PDSTRUCT *pPdStruct = nullptr);

    void fixCheckSum();

    static QList<XPE_DEF::IMAGE_SECTION_HEADER> splitSection(QByteArray *pbaData, XPE_DEF::IMAGE_SECTION_HEADER sectionHeaderOriginal, quint32 nBlockSize);

    struct HEADER_OPTIONS  // TODO Check
    {
        quint16 nMachine;
        quint16 nCharacteristics;
        quint16 nMagic;
        quint64 nImagebase;
        quint16 nSubsystem;
        quint16 nDllcharacteristics;
        quint8 nMajorOperationSystemVersion;
        quint8 nMinorOperationSystemVersion;
        quint32 nFileAlignment;
        quint32 nSectionAlignment;
        quint32 nAddressOfEntryPoint;
        quint32 nResourceRVA;
        quint32 nResourceSize;
    };

    static QByteArray createHeaderStub(HEADER_OPTIONS *pHeaderOptions);

    XPE_DEF::S_IMAGE_LOAD_CONFIG_DIRECTORY32 getLoadConfigDirectory32();  // TODO check size
    XPE_DEF::S_IMAGE_LOAD_CONFIG_DIRECTORY64 getLoadConfigDirectory64();  // TODO check size

    qint64 getLoadConfigDirectoryOffset();
    qint64 getLoadConfigDirectorySize();

    quint32 getLoadConfig_Size();
    quint32 getLoadConfig_TimeDateStamp();
    quint16 getLoadConfig_MajorVersion();
    quint16 getLoadConfig_MinorVersion();
    quint32 getLoadConfig_GlobalFlagsClear();
    quint32 getLoadConfig_GlobalFlagsSet();
    quint32 getLoadConfig_CriticalSectionDefaultTimeout();
    quint64 getLoadConfig_DeCommitFreeBlockThreshold();
    quint64 getLoadConfig_DeCommitTotalFreeThreshold();
    quint64 getLoadConfig_LockPrefixTable();
    quint64 getLoadConfig_MaximumAllocationSize();
    quint64 getLoadConfig_VirtualMemoryThreshold();
    quint64 getLoadConfig_ProcessAffinityMask();
    quint16 getLoadConfig_CSDVersion();
    quint16 getLoadConfig_DependentLoadFlags();
    quint64 getLoadConfig_EditList();
    quint64 getLoadConfig_SecurityCookie();
    quint64 getLoadConfig_SEHandlerTable();
    quint64 getLoadConfig_SEHandlerCount();
    quint64 getLoadConfig_GuardCFCheckFunctionPointer();
    quint64 getLoadConfig_GuardCFDispatchFunctionPointer();
    quint64 getLoadConfig_GuardCFFunctionTable();
    quint64 getLoadConfig_GuardCFFunctionCount();
    quint32 getLoadConfig_GuardFlags();
    quint16 getLoadConfig_CodeIntegrity_Flags();
    quint16 getLoadConfig_CodeIntegrity_Catalog();
    quint32 getLoadConfig_CodeIntegrity_CatalogOffset();
    quint32 getLoadConfig_CodeIntegrity_Reserved();
    quint64 getLoadConfig_GuardAddressTakenIatEntryTable();
    quint64 getLoadConfig_GuardAddressTakenIatEntryCount();
    quint64 getLoadConfig_GuardLongJumpTargetTable();
    quint64 getLoadConfig_GuardLongJumpTargetCount();
    quint64 getLoadConfig_DynamicValueRelocTable();
    quint64 getLoadConfig_CHPEMetadataPointer();
    quint64 getLoadConfig_GuardRFFailureRoutine();
    quint64 getLoadConfig_GuardRFFailureRoutineFunctionPointer();
    quint32 getLoadConfig_DynamicValueRelocTableOffset();
    quint16 getLoadConfig_DynamicValueRelocTableSection();
    quint16 getLoadConfig_Reserved2();
    quint64 getLoadConfig_GuardRFVerifyStackPointerFunctionPointer();
    quint32 getLoadConfig_HotPatchTableOffset();
    quint32 getLoadConfig_Reserved3();
    quint64 getLoadConfig_EnclaveConfigurationPointer();
    quint64 getLoadConfig_VolatileMetadataPointer();
    quint64 getLoadConfig_GuardEHContinuationTable();
    quint64 getLoadConfig_GuardEHContinuationCount();
    quint64 getLoadConfig_GuardXFGCheckFunctionPointer();
    quint64 getLoadConfig_GuardXFGDispatchFunctionPointer();
    quint64 getLoadConfig_GuardXFGTableDispatchFunctionPointer();
    quint64 getLoadConfig_CastGuardOsDeterminedFailureMode();
    quint64 getLoadConfig_GuardMemcpyFunctionPointer();

    void setLoadConfig_Size(quint32 nValue);
    void setLoadConfig_TimeDateStamp(quint32 nValue);
    void setLoadConfig_MajorVersion(quint16 nValue);
    void setLoadConfig_MinorVersion(quint16 nValue);
    void setLoadConfig_GlobalFlagsClear(quint32 nValue);
    void setLoadConfig_GlobalFlagsSet(quint32 nValue);
    void setLoadConfig_CriticalSectionDefaultTimeout(quint32 nValue);
    void setLoadConfig_DeCommitFreeBlockThreshold(quint64 nValue);
    void setLoadConfig_DeCommitTotalFreeThreshold(quint64 nValue);
    void setLoadConfig_LockPrefixTable(quint64 nValue);
    void setLoadConfig_MaximumAllocationSize(quint64 nValue);
    void setLoadConfig_VirtualMemoryThreshold(quint64 nValue);
    void setLoadConfig_ProcessAffinityMask(quint64 nValue);
    void setLoadConfig_CSDVersion(quint16 nValue);
    void setLoadConfig_DependentLoadFlags(quint16 nValue);
    void setLoadConfig_EditList(quint64 nValue);
    void setLoadConfig_SecurityCookie(quint64 nValue);
    void setLoadConfig_SEHandlerTable(quint64 nValue);
    void setLoadConfig_SEHandlerCount(quint64 nValue);
    void setLoadConfig_GuardCFCheckFunctionPointer(quint64 nValue);
    void setLoadConfig_GuardCFDispatchFunctionPointer(quint64 nValue);
    void setLoadConfig_GuardCFFunctionTable(quint64 nValue);
    void setLoadConfig_GuardCFFunctionCount(quint64 nValue);
    void setLoadConfig_GuardFlags(quint32 nValue);
    void setLoadConfig_CodeIntegrity_Flags(quint16 nValue);
    void setLoadConfig_CodeIntegrity_Catalog(quint16 nValue);
    void setLoadConfig_CodeIntegrity_CatalogOffset(quint32 nValue);
    void setLoadConfig_CodeIntegrity_Reserved(quint32 nValue);
    void setLoadConfig_GuardAddressTakenIatEntryTable(quint64 nValue);
    void setLoadConfig_GuardAddressTakenIatEntryCount(quint64 nValue);
    void setLoadConfig_GuardLongJumpTargetTable(quint64 nValue);
    void setLoadConfig_GuardLongJumpTargetCount(quint64 nValue);
    void setLoadConfig_DynamicValueRelocTable(quint64 nValue);
    void setLoadConfig_CHPEMetadataPointer(quint64 nValue);
    void setLoadConfig_GuardRFFailureRoutine(quint64 nValue);
    void setLoadConfig_GuardRFFailureRoutineFunctionPointer(quint64 nValue);
    void setLoadConfig_DynamicValueRelocTableOffset(quint32 nValue);
    void setLoadConfig_DynamicValueRelocTableSection(quint16 nValue);
    void setLoadConfig_Reserved2(quint16 nValue);
    void setLoadConfig_GuardRFVerifyStackPointerFunctionPointer(quint64 nValue);
    void setLoadConfig_HotPatchTableOffset(quint32 nValue);
    void setLoadConfig_Reserved3(quint32 nValue);
    void setLoadConfig_EnclaveConfigurationPointer(quint64 nValue);
    void setLoadConfig_VolatileMetadataPointer(quint64 nValue);
    void setLoadConfig_GuardEHContinuationTable(quint64 nValue);
    void setLoadConfig_GuardEHContinuationCount(quint64 nValue);
    void setLoadConfig_GuardXFGCheckFunctionPointer(quint64 nValue);
    void setLoadConfig_GuardXFGDispatchFunctionPointer(quint64 nValue);
    void setLoadConfig_GuardXFGTableDispatchFunctionPointer(quint64 nValue);
    void setLoadConfig_CastGuardOsDeterminedFailureMode(quint64 nValue);
    void setLoadConfig_GuardMemcpyFunctionPointer(quint64 nValue);

    XPE_DEF::S_IMAGE_RUNTIME_FUNCTION_ENTRY _read_IMAGE_RUNTIME_FUNCTION_ENTRY(qint64 nOffset);

    XPE_DEF::S_IMAGE_RUNTIME_FUNCTION_ENTRY _getException(qint32 nNumber);

    void setException_BeginAddress(qint32 nNumber, quint32 nValue);
    void setException_EndAddress(qint32 nNumber, quint32 nValue);
    void setException_UnwindInfoAddress(qint32 nNumber, quint32 nValue);

    qint64 getExceptionRecordOffset(qint32 nNumber);
    qint64 getExceptionRecordSize();

    QList<XPE_DEF::S_IMAGE_RUNTIME_FUNCTION_ENTRY> getExceptionsList();
    QList<XPE_DEF::S_IMAGE_RUNTIME_FUNCTION_ENTRY> getExceptionsList(XBinary::_MEMORY_MAP *pMemoryMap);

    XPE_DEF::S_IMAGE_DEBUG_DIRECTORY _read_IMAGE_DEBUG_DIRECTORY(qint64 nOffset);

    QList<XPE_DEF::S_IMAGE_DEBUG_DIRECTORY> getDebugList(PDSTRUCT *pPdStruct = nullptr);
    QList<XPE_DEF::S_IMAGE_DEBUG_DIRECTORY> getDebugList(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct = nullptr);

    qint64 getDebugHeaderOffset(quint32 nNumber);
    qint64 getDebugHeaderSize();

    XPE_DEF::S_IMAGE_DEBUG_DIRECTORY getDebugHeader(quint32 nNumber);
    // TODO Get
    void setDebugHeader_Characteristics(quint32 nNumber, quint32 nValue);
    void setDebugHeader_TimeDateStamp(quint32 nNumber, quint32 nValue);
    void setDebugHeader_MajorVersion(quint32 nNumber, quint16 nValue);
    void setDebugHeader_MinorVersion(quint32 nNumber, quint16 nValue);
    void setDebugHeader_Type(quint32 nNumber, quint32 nValue);
    void setDebugHeader_SizeOfData(quint32 nNumber, quint32 nValue);
    void setDebugHeader_AddressOfRawData(quint32 nNumber, quint32 nValue);
    void setDebugHeader_PointerToRawData(quint32 nNumber, quint32 nValue);

    qint64 getDelayImportRecordOffset(qint32 nNumber);
    qint64 getDelayImportRecordSize();

    QList<XPE_DEF::S_IMAGE_DELAYLOAD_DESCRIPTOR> getDelayImportsList();
    QList<XPE_DEF::S_IMAGE_DELAYLOAD_DESCRIPTOR> getDelayImportsList(XBinary::_MEMORY_MAP *pMemoryMap);
    // TODO Get
    void setDelayImport_AllAttributes(quint32 nNumber, quint32 nValue);
    void setDelayImport_DllNameRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_ModuleHandleRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_ImportAddressTableRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_ImportNameTableRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_BoundImportAddressTableRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_UnloadInformationTableRVA(quint32 nNumber, quint32 nValue);
    void setDelayImport_TimeDateStamp(quint32 nNumber, quint32 nValue);

    QList<DELAYIMPORT_POSITION> getDelayImportPositions(qint32 nIndex);
    QList<DELAYIMPORT_POSITION> getDelayImportPositions(XBinary::_MEMORY_MAP *pMemoryMap, qint32 nIndex);

    QList<BOUND_IMPORT_POSITION> getBoundImportPositions();
    QList<BOUND_IMPORT_POSITION> getBoundImportPositions(XBinary::_MEMORY_MAP *pMemoryMap);

    XPE_DEF::IMAGE_BOUND_IMPORT_DESCRIPTOR _read_IMAGE_BOUND_IMPORT_DESCRIPTOR(qint64 nOffset);

    qint64 getBoundImportRecordOffset(qint32 nNumber);
    qint64 getBoundImportRecordSize();

    void setBoundImport_TimeDateStamp(quint32 nNumber, quint32 nValue);
    void setBoundImport_OffsetModuleName(quint32 nNumber, quint16 nValue);
    void setBoundImport_NumberOfModuleForwarderRefs(quint32 nNumber, quint16 nValue);

    qint32 getNumberOfImports();
    QString getImportLibraryName(quint32 nNumber);
    QString getImportLibraryName(quint32 nNumber, QList<XPE::IMPORT_HEADER> *pListImport);

    qint32 getNumberOfImportThunks(quint32 nNumber);
    qint32 getNumberOfImportThunks(quint32 nNumber, QList<XPE::IMPORT_HEADER> *pListImport);

    qint64 getNetHeaderOffset();
    qint64 getNetHeaderSize();

    XPE_DEF::IMAGE_COR20_HEADER _read_IMAGE_COR20_HEADER(qint64 nOffset);

    XPE_DEF::IMAGE_COR20_HEADER getNetHeader();

    quint32 getNetHeader_cb();
    quint16 getNetHeader_MajorRuntimeVersion();
    quint16 getNetHeader_MinorRuntimeVersion();
    quint32 getNetHeader_MetaData_Address();
    quint32 getNetHeader_MetaData_Size();
    quint32 getNetHeader_Flags();
    quint32 getNetHeader_EntryPoint();
    quint32 getNetHeader_Resources_Address();
    quint32 getNetHeader_Resources_Size();
    quint32 getNetHeader_StrongNameSignature_Address();
    quint32 getNetHeader_StrongNameSignature_Size();
    quint32 getNetHeader_CodeManagerTable_Address();
    quint32 getNetHeader_CodeManagerTable_Size();
    quint32 getNetHeader_VTableFixups_Address();
    quint32 getNetHeader_VTableFixups_Size();
    quint32 getNetHeader_ExportAddressTableJumps_Address();
    quint32 getNetHeader_ExportAddressTableJumps_Size();
    quint32 getNetHeader_ManagedNativeHeader_Address();
    quint32 getNetHeader_ManagedNativeHeader_Size();

    void setNetHeader_cb(quint32 nValue);
    void setNetHeader_MajorRuntimeVersion(quint16 nValue);
    void setNetHeader_MinorRuntimeVersion(quint16 nValue);
    void setNetHeader_MetaData_Address(quint32 nValue);
    void setNetHeader_MetaData_Size(quint32 nValue);
    void setNetHeader_Flags(quint32 nValue);
    void setNetHeader_EntryPoint(quint32 nValue);
    void setNetHeader_Resources_Address(quint32 nValue);
    void setNetHeader_Resources_Size(quint32 nValue);
    void setNetHeader_StrongNameSignature_Address(quint32 nValue);
    void setNetHeader_StrongNameSignature_Size(quint32 nValue);
    void setNetHeader_CodeManagerTable_Address(quint32 nValue);
    void setNetHeader_CodeManagerTable_Size(quint32 nValue);
    void setNetHeader_VTableFixups_Address(quint32 nValue);
    void setNetHeader_VTableFixups_Size(quint32 nValue);
    void setNetHeader_ExportAddressTableJumps_Address(quint32 nValue);
    void setNetHeader_ExportAddressTableJumps_Size(quint32 nValue);
    void setNetHeader_ManagedNativeHeader_Address(quint32 nValue);
    void setNetHeader_ManagedNativeHeader_Size(quint32 nValue);

    struct CLI_METADATA_RECORD {
        quint32 nNumber;
        QString sId;
        quint32 nCount;
        bool bIsSorted;
        qint64 nTableOffset;
        qint64 nTableSize;
    };

    QList<CLI_METADATA_RECORD> getCliMetadataRecords(CLI_INFO *pCliInfo, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<SYMBOL_RECORD> getSymbolRecords(XBinary::_MEMORY_MAP *pMemoryMap, SYMBOL_TYPE symbolType = SYMBOL_TYPE_ALL);

    virtual bool removeDosStub();
    virtual bool addDosStub(const QString &sFileName);
    bool _resizeDosStubSize(qint64 nNewStubSize);

    XPE_DEF::WIN_CERT_RECORD read_WIN_CERT_RECORD(qint64 nOffset);
    QList<CERT> getCertList();
    QList<CERT> getCertList(qint64 nOffset, qint64 nSize);
    static QList<CERT> getCertList(QIODevice *pDevice, qint64 nOffset, qint64 nSize);

    static QString certListToString(QList<CERT> *pCertList);
    static QString certRecordToString(CERT_RECORD certRecord, qint32 nLevel);

    CERT_TAG read_CertTag(qint64 nOffset, quint32 nTag);

    void getCertRecord(CERT *pCert, qint64 nOffset, qint64 nSize, CERT_RECORD *pCertRecord);

    static QString certTagToString(quint32 nTag);
    static QString objectIdToString(const QString &sObjectID);

    QString getCertHash(HASH hash);

    struct XCERT_INFO {
        bool bIsValid;
        QString sProgramName;
        QString sPublisher;
        QString sMoreInfo;
        QString sStatus;
        QString sSerialNumber;
        QString sIssuer;
        QString sSubject;
        QString sAlgorithm;
        QString sTSSerialNumber;
        QString sTSIssuer;
        QString sTSSubject;
    };

    static XCERT_INFO getCertInfo(const QString &sFileName);

    virtual QList<FMT_MSG> checkFileFormat(bool bDeep, PDSTRUCT *pPdStruct);

#if defined(_MSC_VER)
    enum CERTNAMESTRING {
        CERTNAMESTRING_UNKNOWN = 0,
        CERTNAMESTRING_ISSUER,
        CERTNAMESTRING_SUBJECT
    };

    static QString getCertNameString(PCCERT_CONTEXT pCertContext, CERTNAMESTRING certNameString);
#endif

    quint64 getImageFileHeader(XPE_DEF::IMAGE_FILE_HEADER *pHeader, const QString &sString);
    quint64 getImageOptionalHeader32(XPE_DEF::IMAGE_OPTIONAL_HEADER32 *pHeader, const QString &sString);
    quint64 getImageOptionalHeader64(XPE_DEF::IMAGE_OPTIONAL_HEADER64 *pHeader, QString sString);

    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr);

    virtual QString structIDToString(quint32 nID);
    virtual QList<XFHEADER> getXFHeaders(const XFSTRUCT &xfStruct, PDSTRUCT *pPdStruct) override;
    virtual QList<XFRECORD> getXFRecords(FT fileType, quint32 nStructID, const XLOC &xLoc) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);

    virtual XADDR _getEntryPointAddress();
    virtual quint32 hlTypeToFParts(HLTYPE hlType);

private:
    quint16 _checkSum(qint64 nStartValue, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);
    RESOURCE_POSITION _getResourcePosition(_MEMORY_MAP *pMemoryMap, qint64 nBaseAddress, qint64 nResourceOffset, qint64 nOffset, quint32 nLevel);
    qint64 _fixHeadersSize();
    // qint64 _getMinSectionOffset();  // TODO move to XBinary
    void _fixFileOffsets(qint64 nDelta);
    quint32 __getResourcesVersion(RESOURCES_VERSION *pResourcesVersionResult, qint64 nOffset, qint64 nSize, const QString &sPrefix, qint32 nLevel);
};

#endif  // XPE_H
