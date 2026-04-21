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
#ifndef XELF_H
#define XELF_H

#include "xbinary.h"
#include "xelf_def.h"

class XELF : public XBinary {
    Q_OBJECT

public:
    enum DS {
        DS_UNKNOWN,
        DS_INTERPRETER,
        DS_LIBRARIES,
        DS_RUNPATH,
        DS_NOTES,
        DS_DYNAMICTAGS,
        DS_STRINGTABLE,
        DS_SYMBOLTABLE,
        DS_RELA,
        DS_REL
    };

    struct NOTE {
        qint64 nOffset;
        qint64 nSize;
        quint32 nType;
        QString sName;
        qint64 nDataOffset;
        qint64 nDataSize;
    };

    struct TAG_STRUCT {
        qint64 nOffset;
        qint64 nTag;
        qint64 nValue;
    };

    struct SECTION_RECORD {
        QString sName;
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
        qint64 nFlags;
    };

    enum TYPE {
        TYPE_UNKNOWN = 0,
        TYPE_REL,
        TYPE_EXEC,
        TYPE_DYN,
        TYPE_CORE,
        TYPE_NUM
    };

    XELF(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    ~XELF();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1, PDSTRUCT *pPdStruct = nullptr);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);
    virtual ENDIAN getEndian();

    virtual bool isExecutable();

    qint64 getEhdrOffset();
    qint64 getEhdr32Size();
    qint64 getEhdr64Size();

    quint32 getIdent_Magic();
    void setIdent_Magic(quint32 nValue);
    quint8 getIdent_mag(qint32 nMag);
    void setIdent_mag(quint8 nValue, qint32 nMag);
    quint32 getIdent_mag_LE();
    void setIdent_mag_LE(quint32 nValue);

    quint8 getIdent_class();
    void setIdent_class(quint8 nValue);
    quint8 getIdent_data();
    void setIdent_data(quint8 nValue);
    quint8 getIdent_version();
    void setIdent_version(quint8 nValue);

    quint8 getIdent_osabi();
    void setIdent_osabi(quint8 nValue);
    quint8 getIdent_abiversion();
    void setIdent_abiversion(quint8 nValue);

    quint8 getIdent_pad(qint32 nPad);
    void setIdent_pad(quint8 nValue, qint32 nPad);

    quint16 getHdr32_type();
    void setHdr32_type(quint16 nValue);
    quint16 getHdr32_machine();
    void setHdr32_machine(quint16 nValue);
    quint32 getHdr32_version();
    void setHdr32_version(quint32 nValue);
    quint32 getHdr32_entry();
    void setHdr32_entry(quint32 nValue);
    quint32 getHdr32_phoff();
    void setHdr32_phoff(quint32 nValue);
    quint32 getHdr32_shoff();
    void setHdr32_shoff(quint32 nValue);
    quint32 getHdr32_flags();
    void setHdr32_flags(quint32 nValue);
    quint16 getHdr32_ehsize();
    void setHdr32_ehsize(quint16 nValue);
    quint16 getHdr32_phentsize();
    void setHdr32_phentsize(quint16 nValue);
    quint16 getHdr32_phnum();
    void setHdr32_phnum(quint16 nValue);
    quint16 getHdr32_shentsize();
    void setHdr32_shentsize(quint16 nValue);
    quint16 getHdr32_shnum();
    void setHdr32_shnum(quint16 nValue);
    quint16 getHdr32_shstrndx();
    void setHdr32_shstrndx(quint16 nValue);

    quint16 getHdr64_type();
    void setHdr64_type(quint16 nValue);
    quint16 getHdr64_machine();
    void setHdr64_machine(quint16 nValue);
    quint32 getHdr64_version();
    void setHdr64_version(quint32 nValue);
    quint64 getHdr64_entry();
    void setHdr64_entry(quint64 nValue);
    quint64 getHdr64_phoff();
    void setHdr64_phoff(quint64 nValue);
    quint64 getHdr64_shoff();
    void setHdr64_shoff(quint64 nValue);
    quint32 getHdr64_flags();
    void setHdr64_flags(quint32 nValue);
    quint16 getHdr64_ehsize();
    void setHdr64_ehsize(quint16 nValue);
    quint16 getHdr64_phentsize();
    void setHdr64_phentsize(quint16 nValue);
    quint16 getHdr64_phnum();
    void setHdr64_phnum(quint16 nValue);
    quint16 getHdr64_shentsize();
    void setHdr64_shentsize(quint16 nValue);
    quint16 getHdr64_shnum();
    void setHdr64_shnum(quint16 nValue);
    quint16 getHdr64_shstrndx();
    void setHdr64_shstrndx(quint16 nValue);

    XELF_DEF::Elf_Ehdr getHdr();

    static QMap<quint64, QString> getHeaderVersionList();

    static QMap<quint64, QString> getIndentMag();
    static QMap<quint64, QString> getIndentMagS();
    static QMap<quint64, QString> getIndentClasses();
    static QMap<quint64, QString> getIndentClassesS();
    static QMap<quint64, QString> getIndentDatas();
    static QMap<quint64, QString> getIndentDatasS();
    static QMap<quint64, QString> getIndentVersions();
    static QMap<quint64, QString> getIndentVersionsS();
    static QMap<quint64, QString> getIndentOsabis();
    static QMap<quint64, QString> getIndentOsabisS();

    static QMap<quint64, QString> getTypes();
    static QMap<quint64, QString> getTypesS();
    static QMap<quint64, QString> getMachines();
    static QMap<quint64, QString> getMachinesS();

    static QMap<quint64, QString> getSectionTypes(const QString &sArch);
    static QMap<quint64, QString> getSectionTypesS(const QString &sArch);
    static QMap<quint64, QString> getSectionFlags();
    static QMap<quint64, QString> getSectionFlagsS();

    static QMap<quint64, QString> getProgramTypes(qint32 nIndent, const QString &sArch);
    static QMap<quint64, QString> getProgramTypesS(qint32 nIndent, const QString &sArch);
    static QMap<quint64, QString> getProgramFlags();
    static QMap<quint64, QString> getProgramFlagsS();

    static QMap<quint64, QString> getDynamicTags(const QString &sArch);
    static QMap<quint64, QString> getDynamicTagsS(const QString &sArch);

    // TODO more
    static QMap<quint64, QString> getRelTypes_x86();
    static QMap<quint64, QString> getRelTypesS_x86();
    static QMap<quint64, QString> getRelTypes_x64();
    static QMap<quint64, QString> getRelTypesS_x64();
    static QMap<quint64, QString> getRelTypes_SPARC();
    static QMap<quint64, QString> getRelTypesS_SPARC();

    static QMap<quint64, QString> getStBinds();
    static QMap<quint64, QString> getStBindsS();
    static QMap<quint64, QString> getStTypes();
    static QMap<quint64, QString> getStTypesS();

    quint32 getSectionStringTable();
    quint32 getSectionStringTable(bool bIs64);

    OFFSETSIZE getSectionOffsetSize(quint32 nSection);

    QMap<quint32, QString> getStringsFromSection(quint32 nSection);
    QMap<quint32, QString> getStringsFromSectionData(QByteArray *pbaData);
    QString getStringFromSection(quint32 nIndex, quint32 nSection);
    QMap<quint32, QString> getStringsFromMainSection();
    QString getStringFromMainSection(quint32 nIndex);
    QByteArray getSection(quint32 nIndex);
    bool isSectionValid(quint32 nIndex);

    QList<XELF_DEF::Elf32_Shdr> getElf32_ShdrList(qint32 nLimit);  // TODO pdStruct
    QList<XELF_DEF::Elf64_Shdr> getElf64_ShdrList(qint32 nLimit);  // TODO pdStruct
    QList<XELF_DEF::Elf_Shdr> getElf_ShdrList(qint32 nLimit);      // TODO pdStruct

    XELF_DEF::Elf32_Shdr getElf32_Shdr(quint32 nIndex);
    XELF_DEF::Elf64_Shdr getElf64_Shdr(quint32 nIndex);

    XELF_DEF::Elf32_Shdr _readElf32_Shdr(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf64_Shdr _readElf64_Shdr(qint64 nOffset, bool bIsBigEndian);

    quint32 getElf32_Shdr_name(quint32 nIndex);
    quint32 getElf32_Shdr_type(quint32 nIndex);
    quint32 getElf32_Shdr_flags(quint32 nIndex);
    quint32 getElf32_Shdr_addr(quint32 nIndex);
    quint32 getElf32_Shdr_offset(quint32 nIndex);
    quint32 getElf32_Shdr_size(quint32 nIndex);
    quint32 getElf32_Shdr_link(quint32 nIndex);
    quint32 getElf32_Shdr_info(quint32 nIndex);
    quint32 getElf32_Shdr_addralign(quint32 nIndex);
    quint32 getElf32_Shdr_entsize(quint32 nIndex);

    void setElf32_Shdr_name(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_type(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_flags(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_addr(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_offset(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_size(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_link(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_info(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_addralign(quint32 nIndex, quint32 nValue);
    void setElf32_Shdr_entsize(quint32 nIndex, quint32 nValue);

    quint32 getElf64_Shdr_name(quint32 nIndex);
    quint32 getElf64_Shdr_type(quint32 nIndex);
    quint64 getElf64_Shdr_flags(quint32 nIndex);
    quint64 getElf64_Shdr_addr(quint32 nIndex);
    quint64 getElf64_Shdr_offset(quint32 nIndex);
    quint64 getElf64_Shdr_size(quint32 nIndex);
    quint32 getElf64_Shdr_link(quint32 nIndex);
    quint32 getElf64_Shdr_info(quint32 nIndex);
    quint64 getElf64_Shdr_addralign(quint32 nIndex);
    quint64 getElf64_Shdr_entsize(quint32 nIndex);

    void setElf64_Shdr_name(quint32 nIndex, quint32 nValue);
    void setElf64_Shdr_type(quint32 nIndex, quint32 nValue);
    void setElf64_Shdr_flags(quint32 nIndex, quint64 nValue);
    void setElf64_Shdr_addr(quint32 nIndex, quint64 nValue);
    void setElf64_Shdr_offset(quint32 nIndex, quint64 nValue);
    void setElf64_Shdr_size(quint32 nIndex, quint64 nValue);
    void setElf64_Shdr_link(quint32 nIndex, quint32 nValue);
    void setElf64_Shdr_info(quint32 nIndex, quint32 nValue);
    void setElf64_Shdr_addralign(quint32 nIndex, quint64 nValue);
    void setElf64_Shdr_entsize(quint32 nIndex, quint64 nValue);

    static quint32 getElf_Shdr_name(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint32 getElf_Shdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_addr(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_size(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint32 getElf_Shdr_link(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint32 getElf_Shdr_info(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_addralign(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    static quint64 getElf_Shdr_entsize(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);

    qint64 getShdrOffset(quint32 nIndex);
    qint64 getShdrSize();

    QList<XELF_DEF::Elf32_Phdr> getElf32_PhdrList(qint32 nLimit);
    QList<XELF_DEF::Elf64_Phdr> getElf64_PhdrList(qint32 nLimit);
    QList<XELF_DEF::Elf_Phdr> getElf_PhdrList(qint32 nLimit);

    XELF_DEF::Elf32_Phdr getElf32_Phdr(quint32 nIndex);
    XELF_DEF::Elf64_Phdr getElf64_Phdr(quint32 nIndex);

    XELF_DEF::Elf32_Phdr _readElf32_Phdr(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf64_Phdr _readElf64_Phdr(qint64 nOffset, bool bIsBigEndian);

    quint32 getElf32_Phdr_type(quint32 nIndex);
    quint32 getElf32_Phdr_offset(quint32 nIndex);
    quint32 getElf32_Phdr_vaddr(quint32 nIndex);
    quint32 getElf32_Phdr_paddr(quint32 nIndex);
    quint32 getElf32_Phdr_filesz(quint32 nIndex);
    quint32 getElf32_Phdr_memsz(quint32 nIndex);
    quint32 getElf32_Phdr_flags(quint32 nIndex);
    quint32 getElf32_Phdr_align(quint32 nIndex);

    void setElf32_Phdr_type(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_offset(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_vaddr(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_paddr(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_filesz(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_memsz(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_flags(quint32 nIndex, quint32 nValue);
    void setElf32_Phdr_align(quint32 nIndex, quint32 nValue);

    quint32 getElf64_Phdr_type(quint32 nIndex);
    quint64 getElf64_Phdr_offset(quint32 nIndex);
    quint64 getElf64_Phdr_vaddr(quint32 nIndex);
    quint64 getElf64_Phdr_paddr(quint32 nIndex);
    quint64 getElf64_Phdr_filesz(quint32 nIndex);
    quint64 getElf64_Phdr_memsz(quint32 nIndex);
    quint32 getElf64_Phdr_flags(quint32 nIndex);
    quint64 getElf64_Phdr_align(quint32 nIndex);

    void setElf64_Phdr_type(quint32 nIndex, quint32 nValue);
    void setElf64_Phdr_offset(quint32 nIndex, quint64 nValue);
    void setElf64_Phdr_vaddr(quint32 nIndex, quint64 nValue);
    void setElf64_Phdr_paddr(quint32 nIndex, quint64 nValue);
    void setElf64_Phdr_filesz(quint32 nIndex, quint64 nValue);
    void setElf64_Phdr_memsz(quint32 nIndex, quint64 nValue);
    void setElf64_Phdr_flags(quint32 nIndex, quint32 nValue);
    void setElf64_Phdr_align(quint32 nIndex, quint64 nValue);

    quint32 getElf_Phdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_vaddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_paddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_filesz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_memsz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint32 getElf_Phdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    quint64 getElf_Phdr_align(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);

    qint64 getPhdrOffset(quint32 nIndex);
    qint64 getPhdrSize();

    qint32 getSectionIndexByName(const QString &sSectionName);  // mb move to XBinary
    qint32 getSectionIndexByName(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);
    QByteArray getSectionByName(const QString &sSectionName);  // mb move to XBinary

    OS_STRING getProgramInterpreterName();
    OS_STRING getProgramInterpreterName(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    OS_STRING getProgramInterpreterName(QList<SECTION_RECORD> *pListSectionRecords);

    QList<QString> getCommentStrings(QList<SECTION_RECORD> *pListSectionRecords);
    QList<QString> getCommentStrings(qint32 nSection);

    QList<NOTE> getNotes();
    QList<NOTE> getNotes(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    QList<NOTE> getNotes(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    QList<NOTE> _getNotes(qint64 nOffset, qint64 nSize, bool bIsBigEndian);
    NOTE _readNote(qint64 nOffset, qint64 nSize, bool bIsBigEndian);

    bool isNotePresent(const QString &sName);
    static bool isNotePresent(QList<NOTE> *pListNotes, const QString &sName);
    static NOTE getNote(QList<NOTE> *pListNotes, const QString &sName);

    bool isNotePresent(quint32 nType);
    static bool isNotePresent(QList<NOTE> *pListNotes, quint32 nType);
    static NOTE getNote(QList<NOTE> *pListNotes, quint32 nType);

    bool isNotePresent(quint32 nType, const QString &sName);
    static bool isNotePresent(QList<NOTE> *pListNotes, quint32 nType, const QString &sName);
    static NOTE getNote(QList<NOTE> *pListNotes, quint32 nType, const QString &sName);

    QList<TAG_STRUCT> getTagStructs();
    QList<TAG_STRUCT> getTagStructs(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, _MEMORY_MAP *pMemoryMap);
    QList<TAG_STRUCT> _getTagStructs(qint64 nOffset, qint64 nSize, bool bIs64, bool bIsBigEndian);
    static QList<TAG_STRUCT> _getTagStructs(QList<TAG_STRUCT> *pListTagStructs, qint64 nTag);

    qint64 getDynamicArraySize();

    qint64 getDynamicArrayTag(qint64 nOffset);
    qint64 getDynamicArrayValue(qint64 nOffset);

    void setDynamicArrayTag(qint64 nOffset, qint64 nValue);
    void setDynamicArrayValue(qint64 nOffset, qint64 nValue);

    OFFSETSIZE getStringTable();
    OFFSETSIZE getStringTable(_MEMORY_MAP *pMemoryMap, QList<TAG_STRUCT> *pListTagStructs);

    QList<QString> getLibraries();
    QList<QString> getLibraries(_MEMORY_MAP *pMemoryMap, QList<TAG_STRUCT> *pListTagStructs);

    OS_STRING getRunPath();
    OS_STRING getRunPath(_MEMORY_MAP *pMemoryMap, QList<TAG_STRUCT> *pListTagStructs);

    virtual QList<MAPMODE> getMapModesList();
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    virtual qint64 getEntryPointOffset(_MEMORY_MAP *pMemoryMap);

    static QList<SECTION_RECORD> getSectionRecords(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, bool bIsImage, QByteArray *pbaSectionTable);
    bool isSectionNamePresent(const QString &sSectionName);
    static bool isSectionNamePresent(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);  // TODO pdStruct
    qint32 getSectionNumber(const QString &sSectionName);                                                       // TODO pdStruct
    static qint32 getSectionNumber(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords);

    virtual MODE getMode();
    virtual QString getArch();
    virtual qint32 getType();
    virtual FT getFileType();
    virtual QString typeIdToString(qint32 nType);

    virtual FILEFORMATINFO getFileFormatInfo(PDSTRUCT *pPdStruct);

    QList<XELF_DEF::Elf_Phdr> _getPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, quint32 nType);
    QList<XELF_DEF::Elf_Shdr> _getSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, quint32 nType);
    static QList<SECTION_RECORD> _getSectionRecords(QList<SECTION_RECORD> *pListSectionRecords, const QString &sName);

    QList<DATASET> getDatasetsFromSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders);
    QList<DATASET> getDatasetsFromPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders);
    QList<DATASET> getDatasetsFromTagStructs(_MEMORY_MAP *pMemoryMap, QList<TAG_STRUCT> *pListTagStructs);

    QList<XELF_DEF::Elf32_Sym> getElf32_SymList(qint64 nOffset, qint64 nSize);
    QList<XELF_DEF::Elf64_Sym> getElf64_SymList(qint64 nOffset, qint64 nSize);
    QList<XELF_DEF::Elf_Sym> getElf_SymList(qint64 nOffset, qint64 nSize);

    XELF_DEF::Elf32_Sym _readElf32_Sym(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf64_Sym _readElf64_Sym(qint64 nOffset, bool bIsBigEndian);

    void setElf32_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Sym_st_value(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Sym_st_size(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Sym_st_info(qint64 nOffset, quint8 nValue);
    void setElf32_Sym_st_other(qint64 nOffset, quint8 nValue);
    void setElf32_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian);

    void setElf64_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf64_Sym_st_info(qint64 nOffset, quint8 nValue);
    void setElf64_Sym_st_other(qint64 nOffset, quint8 nValue);
    void setElf64_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian);
    void setElf64_Sym_st_value(qint64 nOffset, quint64 nValue, bool bIsBigEndian);
    void setElf64_Sym_st_size(qint64 nOffset, quint64 nValue, bool bIsBigEndian);

    qint64 getSymSize();
    qint64 getSymTableSize(qint64 nOffset);
    qint32 getNumberOfSymbols(qint64 nOffset, PDSTRUCT *pPdStruct = nullptr);
    qint32 getNumberOfSymbols(qint64 nOffset, bool bIsBigEndian, bool bIs64, PDSTRUCT *pPdStruct);

    XELF_DEF::Elf32_Rel _readElf32_Rel(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf64_Rel _readElf64_Rel(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf32_Rela _readElf32_Rela(qint64 nOffset, bool bIsBigEndian);
    XELF_DEF::Elf64_Rela _readElf64_Rela(qint64 nOffset, bool bIsBigEndian);

    QList<XELF_DEF::Elf32_Rel> getElf32_RelList(qint64 nOffset, qint64 nSize);
    QList<XELF_DEF::Elf64_Rel> getElf64_RelList(qint64 nOffset, qint64 nSize);

    QList<XELF_DEF::Elf32_Rela> getElf32_RelaList(qint64 nOffset, qint64 nSize);
    QList<XELF_DEF::Elf64_Rela> getElf64_RelaList(qint64 nOffset, qint64 nSize);

    void setElf32_Rel_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Rel_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian);

    void setElf64_Rel_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian);
    void setElf64_Rel_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian);

    void setElf32_Rela_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Rela_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian);
    void setElf32_Rela_r_addend(qint64 nOffset, quint32 nValue, bool bIsBigEndian);

    void setElf64_Rela_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian);
    void setElf64_Rela_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian);
    void setElf64_Rela_r_addend(qint64 nOffset, quint64 nValue, bool bIsBigEndian);

    quint32 getNumberOfSections();
    quint16 getNumberOfPrograms();

    bool isSectionsTablePresent();
    bool isProgramsTablePresent();

    virtual QString getFileFormatExt();
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);

    virtual QString getMIMEString();

    struct FIXDUMP_OPTIONS {
        bool bOptimizeSize;
        bool bFixSegments;
        bool bSetEntryPoint;
        XADDR nEntryPoint;
    };

    bool fixDump(const QString &sResultFile, const FIXDUMP_OPTIONS &fixDumpOptions, PDSTRUCT *pPdStruct = nullptr);
    FIXDUMP_OPTIONS getFixDumpOptions(PDSTRUCT *pPdStruct = nullptr);

    QByteArray getHeaders();

    static const QString PREFIX_ET;
    static const QString PREFIX_EM;
    static const QString PREFIX_EV;
    static const QString PREFIX_ELFCLASS;
    static const QString PREFIX_ELFDATA;
    static const QString PREFIX_ELFOSABI;
    static const QString PREFIX_SHT;
    static const QString PREFIX_SHF;
    static const QString PREFIX_PT;
    static const QString PREFIX_PF;
    static const QString PREFIX_DT;
    static const QString PREFIX_R;
    static const QString PREFIX_STB;
    static const QString PREFIX_STT;

    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;

};

#endif  // XELF_H
