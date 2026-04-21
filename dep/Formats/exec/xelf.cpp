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
#include "xelf.h"

// clang-format off
XBinary::XIDSTRING _TABLE_XELF_Types[] = {
    {0,      "NONE"},
    {1,      "REL"},
    {2,      "EXEC"},
    {3,      "DYN"},
    {4,      "CORE"},
    {5,      "NUM"},
    {0xff00, "LOPROC"},
    {0xffff, "HIPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_Machines[] = {
    {0,      "NONE"},
    {1,      "M32"},
    {2,      "SPARC"},
    {3,      "386"},
    {4,      "68K"},
    {5,      "88K"},
    {6,      "486"},
    {7,      "860"},
    {8,      "MIPS"},
    {9,      "S370"},
    {10,     "MIPS_RS3_LE"},
    {11,     "RS6000"},
    {15,     "PARISC"},
    {16,     "nCUBE"},
    {17,     "VPP500"},
    {18,     "SPARC32PLUS"},
    {19,     "960"},
    {20,     "PPC"},
    {21,     "PPC64"},
    {22,     "S390"},
    {23,     "SPU"},
    {36,     "V800"},
    {37,     "FR20"},
    {38,     "RH32"},
    {39,     "RCE"},
    {40,     "ARM"},
    {41,     "ALPHA"},
    {42,     "SH"},
    {43,     "SPARCV9"},
    {44,     "TRICORE"},
    {45,     "ARC"},
    {46,     "H8_300"},
    {47,     "H8_300H"},
    {48,     "H8S"},
    {49,     "H8_500"},
    {50,     "IA_64"},
    {51,     "MIPS_X"},
    {52,     "COLDFIRE"},
    {53,     "68HC12"},
    {54,     "MMA"},
    {55,     "PCP"},
    {56,     "NCPU"},
    {57,     "NDR1"},
    {58,     "STARCORE"},
    {59,     "ME16"},
    {60,     "ST100"},
    {61,     "TINYJ"},
    {62,     "AMD64"},
    {63,     "PDSP"},
    {66,     "FX66"},
    {67,     "ST9PLUS"},
    {68,     "ST7"},
    {69,     "68HC16"},
    {70,     "68HC11"},
    {71,     "68HC08"},
    {72,     "68HC05"},
    {73,     "SVX"},
    {74,     "ST19"},
    {75,     "VAX"},
    {76,     "CRIS"},
    {77,     "JAVELIN"},
    {78,     "FIREPATH"},
    {79,     "ZSP"},
    {80,     "MMIX"},
    {81,     "HUANY"},
    {82,     "PRISM"},
    {83,     "AVR"},
    {84,     "FR30"},
    {85,     "D10V"},
    {86,     "D30V"},
    {87,     "V850"},
    {88,     "M32R"},
    {89,     "MN10300"},
    {90,     "MN10200"},
    {91,     "PJ"},
    {92,     "OPENRISC"},
    {93,     "ARC_A5"},
    {94,     "XTENSA"},
    {95,     "VIDEOCORE"},
    {96,     "TMM_GPP"},
    {97,     "NS32K"},
    {98,     "TPC"},
    {99,     "SNP1K"},
    {100,    "ST200"},
    {101,    "IP2K"},
    {102,    "MAX"},
    {103,    "CR"},
    {104,    "F2MC16"},
    {105,    "MSP430"},
    {106,    "BLACKFIN"},
    {107,    "SE_C33"},
    {108,    "SEP"},
    {109,    "ARCA"},
    {110,    "UNICORE"},
    {111,    "EXCESS"},
    {112,    "DXP"},
    {113,    "ALTERA_NIOS2"},
    {114,    "CRX"},
    {115,    "XGATE"},
    {116,    "C166"},
    {117,    "M16C"},
    {118,    "DSPIC30F"},
    {119,    "CE"},
    {120,    "M32C"},
    {140,    "TI_C6000"},
    {183,    "AARCH64"},
    {243,    "RISC_V"},
    {258,    "LOONGARCH"},
    {0x5441, "FRV"},
    {0x18ad, "AVR32"},
    {0x9026, "ALPHA"},
    {0x9080, "CYGNUS_V850"},
    {0x9041, "CYGNUS_M32R"},
    {0xA390, "S390_OLD"},
    {0xbeef, "CYGNUS_MN10300"},
};

XBinary::XIDSTRING _TABLE_XELF_Versions[] = {
    {1, "CURRENT"},
};

XBinary::XIDSTRING _TABLE_XELF_IndentMag[] = {
    {0x464C457F, "ELFMAG"},
};

XBinary::XIDSTRING _TABLE_XELF_IndentClasses[] = {
    {0, "NONE"},
    {1, "32"},
    {2, "64"},
};

XBinary::XIDSTRING _TABLE_XELF_IndentDatas[] = {
    {0, "NONE"},
    {1, "2LSB"},
    {2, "2MSB"},
};

XBinary::XIDSTRING _TABLE_XELF_IndentOsabis[] = {
    {0,   "SYSV"},
    {1,   "HPUX"},
    {2,   "NETBSD"},
    {3,   "LINUX"},
    {4,   "HURD"},
    {5,   "86OPEN"},
    {6,   "SOLARIS"},
    {7,   "AIX"},
    {8,   "IRIX"},
    {9,   "FREEBSD"},
    {10,  "TRU64"},
    {11,  "MODESTO"},
    {12,  "OPENBSD"},
    {13,  "OPENVMS"},
    {14,  "NSK"},
    {15,  "AROS"},
    {16,  "FENIXOS"},
    {17,  "CLOUDABI"},
    {18,  "OPENVOS"},
    {64,  "ARM_AEABI"},
    {97,  "ARM"},
    {255, "STANDALONE"},
};

XBinary::XIDSTRING _TABLE_XELF_SectionTypes[] = {
    {0,          "NULL"},
    {1,          "PROGBITS"},
    {2,          "SYMTAB"},
    {3,          "STRTAB"},
    {4,          "RELA"},
    {5,          "HASH"},
    {6,          "DYNAMIC"},
    {7,          "NOTE"},
    {8,          "NOBITS"},
    {9,          "REL"},
    {10,         "SHLIB"},
    {11,         "DYNSYM"},
    {14,         "INIT_ARRAY"},
    {15,         "FINI_ARRAY"},
    {16,         "PREINIT_ARRAY"},
    {17,         "GROUP"},
    {18,         "SYMTAB_SHNDX"},
    {19,         "NUM"},
    {0x60000000, "LOOS"},
    {0x6ffffff5, "GNU_ATTRIBUTES"},
    {0x6ffffff6, "GNU_HASH"},
    {0x6ffffffa, "SUNW_move"},
    {0x6ffffffc, "SUNW_syminfo"},
    {0x6ffffffd, "GNU_verdef"},
    {0x6ffffffe, "GNU_verneed"},
    {0x6fffffff, "GNU_versym"},
    {0x70000000, "LOPROC"},
    {0x7fffffff, "HIPROC"},
    {0x80000000, "LOUSER"},
    {0xffffffff, "HIUSER"},
};

XBinary::XIDSTRING _TABLE_XELF_SectionTypes_ARM[] = {
    {0x70000001, "ARM_EXIDX"},
    {0x70000002, "ARM_PREEMPTMAP"},
    {0x70000003, "ARM_ATTRIBUTES"},
    {0x70000004, "ARM_DEBUGOVERLAY"},
    {0x70000005, "ARM_OVERLAYSECTION"},
};

XBinary::XIDSTRING _TABLE_XELF_SectionTypes_MIPS[] = {
    {0x70000001, "MIPS_MSYM"},
    {0x70000002, "MIPS_CONFLICT"},
    {0x70000003, "MIPS_GPTAB"},
    {0x70000004, "MIPS_UCODE"},
    {0x70000005, "MIPS_DEBUG"},
    {0x70000006, "MIPS_REGINFO"},
    {0x70000007, "MIPS_PACKAGE"},
    {0x70000008, "MIPS_PACKSYM"},
    {0x70000009, "MIPS_RELD"},
    {0x7000000b, "MIPS_IFACE"},
    {0x7000000c, "MIPS_CONTENT"},
    {0x7000000d, "MIPS_OPTIONS"},
    {0x70000010, "MIPS_SHDR"},
    {0x70000011, "MIPS_FDESC"},
    {0x70000012, "MIPS_EXTSYM"},
    {0x70000013, "MIPS_DENSE"},
    {0x70000014, "MIPS_PDESC"},
    {0x70000015, "MIPS_LOCSYM"},
    {0x70000016, "MIPS_AUXSYM"},
    {0x70000017, "MIPS_OPTSYM"},
    {0x70000018, "MIPS_LOCSTR"},
    {0x70000019, "MIPS_LINE"},
    {0x7000001a, "MIPS_RFDESC"},
    {0x7000001b, "MIPS_DELTASYM"},
    {0x7000001c, "MIPS_DELTAINST"},
    {0x7000001d, "MIPS_DELTACLASS"},
    {0x7000001e, "MIPS_DWARF"},
    {0x7000001f, "MIPS_DELTADECL"},
    {0x70000020, "MIPS_SYMBOL_LIB"},
    {0x70000021, "MIPS_EVENTS"},
    {0x70000022, "MIPS_TRANSLATE"},
    {0x70000023, "MIPS_PIXIE"},
    {0x70000024, "MIPS_XLATE"},
    {0x70000025, "MIPS_XLATE_DEBUG"},
    {0x70000026, "MIPS_WHIRL"},
    {0x70000027, "MIPS_EH_REGION"},
    {0x70000028, "MIPS_XLATE_OLD"},
    {0x70000029, "MIPS_PDR_EXCEPTION"},
    {0x7000002a, "MIPS_ABIFLAGS"},
    {0x7000002b, "MIPS_XHASH"},
};

XBinary::XIDSTRING _TABLE_XELF_SectionTypes_AMD64[] = {
    {0x70000001, "AMD64_UNWIND"},
};

XBinary::XIDSTRING _TABLE_XELF_SectionFlags[] = {
    {0x00000001, "WRITE"},
    {0x00000002, "ALLOC"},
    {0x00000004, "EXECINSTR"},
    {0x00000010, "MERGE"},
    {0x00000020, "STRINGS"},
    {0x00000040, "INFO_LINK"},
    {0x00000080, "LINK_ORDER"},
    {0x00000100, "OS_NONCONFORMING"},
    {0x00000200, "GROUP"},
    {0x00000400, "TLS"},
    {0x00000800, "COMPRESSED"},
    {0x0ff00000, "MASKOS"},
    {0xf0000000, "MASKPROC"},
    {0x40000000, "ORDERED"},
    {0x80000000, "EXCLUDE"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_Base[] = {
    {0,          "NULL"},
    {1,          "LOAD"},
    {2,          "DYNAMIC"},
    {3,          "INTERP"},
    {4,          "NOTE"},
    {5,          "SHLIB"},
    {6,          "PHDR"},
    {7,          "TLS"},
    {8,          "NUM"},
    {0x60000000, "LOOS"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_Solaris[] = {
    {0x6464e550, "SUNW_UNWIND"},
    {0x6474e550, "SUNW_EH_FRAME"},
    {0x6ffffffa, "SUNWBSS"},
    {0x6ffffffb, "SUNWSTACK"},
    {0x6ffffffc, "SUNWDTRACE"},
    {0x6ffffffd, "SUNWCAP"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_GNU[] = {
    {0x6474e550, "GNU_EH_FRAME"},
    {0x6474e551, "GNU_STACK"},
    {0x6474e552, "GNU_RELRO"},
    {0x6474e553, "GNU_PROPERTY"},
    {0x65041580, "PAX_FLAGS"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_OpenBSD[] = {
    {0x65A3DBE6, "OPENBSD_RANDOMIZE"},
    {0x65A3DBE7, "OPENBSD_WXNEEDED"},
    {0x65A41BE6, "OPENBSD_BOOTDATA"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_Cont[] = {
    {0x6ffffffa, "LOSUNW"},
    {0x6fffffff, "HIOS"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_ARM[] = {
    {0x70000001, "ARM_EXIDX"},
    {0x70000002, "ARM_PREEMPTMAP"},
    {0x70000003, "ARM_ATTRIBUTES"},
    {0x70000004, "ARM_DEBUGOVERLAY"},
    {0x70000005, "ARM_OVERLAYSECTION"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_MIPS[] = {
    {0x70000000, "MIPS_REGINFO"},
    {0x70000001, "MIPS_RTPROC"},
    {0x70000002, "MIPS_OPTIONS"},
    {0x70000003, "MIPS_ABIFLAGS"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_AARCH64[] = {
    {0x70000000, "AARCH64_ARCHEXT"},
    {0x70000001, "AARCH64_UNWIND"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_S390[] = {
    {0x70000000, "S390_PGSTE"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_LOPROC[] = {
    {0x70000000, "LOPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramTypes_Tail[] = {
    {0x7fffffff, "HIPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_ProgramFlags[] = {
    {0x00000001, "X"},
    {0x00000002, "W"},
    {0x00000004, "R"},
    {0x0ff00000, "MASKOS"},
    {0xf0000000, "MASKPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_DynamicTags_Base[] = {
    {0,          "NULL"},
    {1,          "NEEDED"},
    {2,          "PLTRELSZ"},
    {3,          "PLTGOT"},
    {4,          "HASH"},
    {5,          "STRTAB"},
    {6,          "SYMTAB"},
    {7,          "RELA"},
    {8,          "RELASZ"},
    {9,          "RELAENT"},
    {10,         "STRSZ"},
    {11,         "SYMENT"},
    {12,         "INIT"},
    {13,         "FINI"},
    {14,         "SONAME"},
    {15,         "RPATH"},
    {16,         "SYMBOLIC"},
    {17,         "REL"},
    {18,         "RELSZ"},
    {19,         "RELENT"},
    {20,         "PLTREL"},
    {21,         "DEBUG"},
    {22,         "TEXTREL"},
    {23,         "JMPREL"},
    {24,         "BIND_NOW"},
    {25,         "INIT_ARRAY"},
    {26,         "FINI_ARRAY"},
    {27,         "INIT_ARRAYSZ"},
    {28,         "FINI_ARRAYSZ"},
    {29,         "RUNPATH"},
    {30,         "FLAGS"},
    {32,         "PREINIT_ARRAY"},
    {33,         "PREINIT_ARRAYSZ"},
    {34,         "NUM"},
    {0x6000000d, "LOOS"},
    {0x6000000e, "SUNW_RTLDINF"},
    {0x6ffff000, "HIOS"},
    {0x6ffffd00, "VALRNGLO"},
    {0x6ffffdf8, "CHECKSUM"},
    {0x6ffffdf9, "PLTPADSZ"},
    {0x6ffffdfa, "MOVEENT"},
    {0x6ffffdfb, "MOVESZ"},
    {0x6ffffdfc, "FEATURE_1"},
    {0x6ffffdfd, "POSFLAG_1"},
    {0x6ffffdfe, "SYMINSZ"},
    {0x6ffffdff, "SYMINENT"},
    {0x6ffffdff, "VALRNGHI"},
    {0x6ffffe00, "ADDRRNGLO"},
    {0x6ffffef5, "GNU_HASH"},
    {0x6ffffefa, "CONFIG"},
    {0x6ffffefb, "DEPAUDIT"},
    {0x6ffffefc, "AUDIT"},
    {0x6ffffefd, "PLTPAD"},
    {0x6ffffefe, "MOVETAB"},
    {0x6ffffeff, "SYMINFO"},
    {0x6ffffeff, "ADDRRNGHI"},
    {0x6ffffff0, "VERSYM"},
    {0x6ffffff9, "RELACOUNT"},
    {0x6ffffffa, "RELCOUNT"},
    {0x6ffffffb, "FLAGS_1"},
    {0x6ffffffc, "VERDEF"},
    {0x6ffffffd, "VERDEFNUM"},
    {0x6ffffffe, "VERNEED"},
    {0x6fffffff, "VERNEEDNUM"},
    {0x70000000, "LOPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_DynamicTags_MIPS[] = {
    {0x70000001, "MIPS_RLD_VERSION"},
    {0x70000002, "MIPS_TIME_STAMP"},
    {0x70000003, "MIPS_ICHECKSUM"},
    {0x70000004, "MIPS_IVERSION"},
    {0x70000005, "MIPS_FLAGS"},
    {0x70000006, "MIPS_BASE_ADDRESS"},
    {0x70000008, "MIPS_CONFLICT"},
    {0x70000009, "MIPS_LIBLIST"},
    {0x7000000a, "MIPS_LOCAL_GOTNO"},
    {0x7000000b, "MIPS_CONFLICTNO"},
    {0x70000010, "MIPS_LIBLISTNO"},
    {0x70000011, "MIPS_SYMTABNO"},
    {0x70000012, "MIPS_UNREFEXTNO"},
    {0x70000013, "MIPS_GOTSYM"},
    {0x70000014, "MIPS_HIPAGENO"},
    {0x70000016, "MIPS_RLD_MAP"},
    {0x70000017, "MIPS_DELTA_CLASS"},
    {0x70000018, "MIPS_DELTA_CLASS_NO"},
    {0x70000019, "MIPS_DELTA_INSTANCE"},
    {0x7000001A, "MIPS_DELTA_INSTANCE_NO"},
    {0x7000001B, "MIPS_DELTA_RELOC"},
    {0x7000001C, "MIPS_DELTA_RELOC_NO"},
    {0x7000001D, "MIPS_DELTA_SYM"},
    {0x7000001E, "MIPS_DELTA_SYM_NO"},
    {0x70000020, "MIPS_DELTA_CLASSSYM"},
    {0x70000021, "MIPS_DELTA_CLASSSYM_NO"},
    {0x70000022, "MIPS_CXX_FLAGS"},
    {0x70000023, "MIPS_PIXIE_INIT"},
    {0x70000024, "MIPS_SYMBOL_LIB"},
    {0x70000025, "MIPS_LOCALPAGE_GOTIDX"},
    {0x70000026, "MIPS_LOCAL_GOTIDX"},
    {0x70000027, "MIPS_HIDDEN_GOTIDX"},
    {0x70000028, "MIPS_PROTECTED_GOTIDX"},
    {0x70000029, "MIPS_OPTIONS"},
    {0x7000002A, "MIPS_INTERFACE"},
    {0x7000002B, "MIPS_DYNSTR_ALIGN"},
    {0x7000002C, "MIPS_INTERFACE_SIZE"},
    {0x7000002D, "MIPS_RLD_TEXT_RESOLVE_ADDR"},
    {0x7000002E, "MIPS_PERF_SUFFIX"},
    {0x7000002F, "MIPS_COMPACT_SIZE"},
    {0x70000030, "MIPS_GP_VALUE"},
    {0x70000031, "MIPS_AUX_DYNAMIC"},
    {0x70000032, "MIPS_PLTGOT"},
    {0x70000033, "MIPS_RLD_OBJ_UPDATE"},
    {0x70000034, "MIPS_RWPLT"},
    {0x70000035, "MIPS_RLD_VERSION"},
    {0x36,       "MIPS_NUM"},
};

XBinary::XIDSTRING _TABLE_XELF_DynamicTags_SPARC[] = {
    {0x70000001, "SPARC_REGISTER"},
};

XBinary::XIDSTRING _TABLE_XELF_DynamicTags_Tail[] = {
    {0x7ffffffd, "AUXILIARY"},
    {0x7ffffffe, "USED"},
    {0x7fffffff, "HIPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_RelTypes_x86[] = {
    {0,  "386_NONE"},
    {1,  "386_32"},
    {2,  "386_PC32"},
    {3,  "386_GOT32"},
    {4,  "386_PLT32"},
    {5,  "386_COPY"},
    {6,  "386_GLOB_DAT"},
    {7,  "386_JMP_SLOT"},
    {8,  "386_RELATIVE"},
    {9,  "386_GOTOFF"},
    {10, "386_GOTPC"},
    {11, "386_32PLT"},
    {20, "386_16"},
    {21, "386_PC16"},
    {22, "386_8"},
    {23, "386_PC8"},
    {38, "386_SIZE32"},
};

XBinary::XIDSTRING _TABLE_XELF_RelTypes_x64[] = {
    {0,  "X86_64_NONE"},
    {1,  "X86_64_64"},
    {2,  "386_PC32"},
    {3,  "386_GOT32"},
    {4,  "X86_64_PLT32"},
    {5,  "X86_64_COPY"},
    {6,  "X86_64_GLOB_DAT"},
    {7,  "X86_64_JMP_SLOT"},
    {8,  "X86_64_RELATIVE"},
    {9,  "X86_64_GOTPCREL"},
    {10, "X86_64_32"},
    {11, "X86_64_32S"},
    {12, "X86_64_16"},
    {13, "X86_64_PC16"},
    {14, "X86_64_8"},
    {15, "X86_64_PC8"},
    {24, "X86_64_PC64"},
};

XBinary::XIDSTRING _TABLE_XELF_RelTypes_SPARC[] = {
    {0,  "SPARC_NONE"},
    {1,  "SPARC_8"},
    {2,  "SPARC_16"},
    {3,  "SPARC_32"},
    {4,  "SPARC_DISP8"},
    {5,  "SPARC_DISP16"},
    {6,  "SPARC_DISP32"},
    {7,  "SPARC_WDISP30"},
    {8,  "SPARC_WDISP22"},
    {9,  "SPARC_HI22"},
    {10, "SPARC_22"},
    {11, "SPARC_13"},
    {12, "SPARC_LO10"},
    {13, "SPARC_GOT10"},
    {14, "SPARC_GOT13"},
    {15, "SPARC_GOT22"},
    {16, "SPARC_PC10"},
    {17, "SPARC_PC22"},
    {18, "SPARC_WPLT30"},
    {19, "SPARC_COPY"},
    {20, "SPARC_GLOB_DAT"},
    {21, "SPARC_JMP_SLOT"},
    {22, "SPARC_RELATIVE"},
    {23, "SPARC_UA32"},
    {24, "SPARC_PLT32"},
    {25, "SPARC_HIPLT22"},
    {26, "SPARC_LOPLT10"},
    {27, "SPARC_PCPLT32"},
    {28, "SPARC_PCPLT22"},
    {29, "SPARC_PCPLT10"},
    {30, "SPARC_10"},
    {31, "SPARC_11"},
    {32, "SPARC_64"},
    {33, "SPARC_OLO10"},
    {34, "SPARC_HH22"},
    {35, "SPARC_HM10"},
    {36, "SPARC_LM22"},
    {37, "SPARC_PC_HH22"},
    {38, "SPARC_PC_HM10"},
    {39, "SPARC_PC_LM22"},
    {40, "SPARC_WDISP16"},
    {41, "SPARC_WDISP19"},
    {43, "SPARC_7"},
    {44, "SPARC_5"},
    {45, "SPARC_6"},
    {46, "SPARC_DISP64"},
    {47, "SPARC_PLT64"},
    {48, "SPARC_HIX22"},
    {49, "SPARC_LOX10"},
    {50, "SPARC_H44"},
    {51, "SPARC_M44"},
    {52, "SPARC_L44"},
    {53, "SPARC_REGISTER"},
    {54, "SPARC_UA64"},
    {55, "SPARC_UA16"},
    {80, "SPARC_GOTDATA_HIX22"},
    {81, "SPARC_GOTDATA_LOX10"},
    {82, "SPARC_GOTDATA_OP_HIX22"},
    {83, "SPARC_GOTDATA_OP_LOX10"},
    {84, "SPARC_GOTDATA_OP"},
    {85, "SPARC_SIZE32"},
    {87, "SPARC_SIZE64"},
};

XBinary::XIDSTRING _TABLE_XELF_StBinds[] = {
    {0,  "LOCAL"},
    {1,  "GLOBAL"},
    {2,  "WEAK"},
    {10, "LOOS"},
    {12, "HIOS"},
    {13, "LOPROC"},
    {15, "HIPROC"},
};

XBinary::XIDSTRING _TABLE_XELF_StTypes[] = {
    {0,  "NOTYPE"},
    {1,  "OBJECT"},
    {2,  "FUNC"},
    {3,  "SECTION"},
    {4,  "FILE"},
    {5,  "COMMON"},
    {10, "LOOS"},
    {12, "HIOS"},
    {13, "LOPROC"},
    {15, "HIPROC"},
};
// clang-format on

XELF::XELF(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XBinary(pDevice, bIsImage, nModuleAddress)
{
}

XELF::~XELF()
{
}

bool XELF::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    if (getIdent_Magic() == XELF_DEF::S_ELFMAG) {
        quint8 nClass = getIdent_class();

        if ((nClass == XELF_DEF::S_ELFCLASS32) || (nClass == XELF_DEF::S_ELFCLASS64)) {
            bResult = true;
        }
    }

    return bResult;
}

bool XELF::isExecutable()
{
    return true;
}

bool XELF::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XELF xelf(pDevice, bIsImage, nModuleAddress);

    return xelf.isValid(pPdStruct);
}

XBinary::MODE XELF::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XELF xelf(pDevice, bIsImage, nModuleAddress);

    return xelf.getMode();
}

XBinary::ENDIAN XELF::getEndian()
{
    ENDIAN result = ENDIAN_UNKNOWN;

    quint8 nData = getIdent_data();

    if (nData == XELF_DEF::S_ELFDATA2LSB) {
        result = ENDIAN_LITTLE;
    } else if (nData == XELF_DEF::S_ELFDATA2MSB) {
        result = ENDIAN_BIG;
    }

    return result;
}

qint64 XELF::getEhdrOffset()
{
    return 0;
}

qint64 XELF::getEhdr32Size()
{
    return sizeof(XELF_DEF::Elf32_Ehdr);
}

qint64 XELF::getEhdr64Size()
{
    return sizeof(XELF_DEF::Elf64_Ehdr);
}

quint32 XELF::getIdent_Magic()
{
    return read_uint32(XELF_DEF::S_EI_MAG0);
}

void XELF::setIdent_Magic(quint32 nValue)
{
    write_uint32(XELF_DEF::S_EI_MAG0, nValue);
}

quint8 XELF::getIdent_mag(qint32 nMag)
{
    quint8 nValue = 0;

    if (nMag < 4) {
        nValue = read_uint8(nMag);
    }

    return nValue;
}

void XELF::setIdent_mag(quint8 nValue, qint32 nMag)
{
    if (nMag < 4) {
        write_uint8(nMag, nValue);
    }
}

quint32 XELF::getIdent_mag_LE()
{
    return read_uint32(0, false);
}

void XELF::setIdent_mag_LE(quint32 nValue)
{
    write_uint32(0, nValue, false);
}

quint8 XELF::getIdent_class()
{
    return read_uint8(XELF_DEF::S_EI_CLASS);
}

void XELF::setIdent_class(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_CLASS, nValue);
}

quint8 XELF::getIdent_data()
{
    return read_uint8(XELF_DEF::S_EI_DATA);
}

void XELF::setIdent_data(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_DATA, nValue);
}

quint8 XELF::getIdent_version()
{
    return read_uint8(XELF_DEF::S_EI_VERSION);
}

void XELF::setIdent_version(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_VERSION, nValue);
}

quint8 XELF::getIdent_osabi()
{
    return read_uint8(XELF_DEF::S_EI_OSABI);
}

void XELF::setIdent_osabi(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_OSABI, nValue);
}

quint8 XELF::getIdent_abiversion()
{
    return read_uint8(XELF_DEF::S_EI_ABIVERSION);
}

void XELF::setIdent_abiversion(quint8 nValue)
{
    write_uint8(XELF_DEF::S_EI_ABIVERSION, nValue);
}

quint8 XELF::getIdent_pad(qint32 nPad)
{
    quint8 nValue = 0;

    if (nPad < 7) {
        nValue = read_uint8(XELF_DEF::S_EI_ABIVERSION + 1 + nPad);
    }

    return nValue;
}

void XELF::setIdent_pad(quint8 nValue, qint32 nPad)
{
    if (nPad < 7) {
        write_uint8(XELF_DEF::S_EI_ABIVERSION + 1 + nPad, nValue);
    }
}

quint16 XELF::getHdr32_type()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), isBigEndian());
}

void XELF::setHdr32_type(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), nValue, isBigEndian());
}

quint16 XELF::getHdr32_machine()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), isBigEndian());
}

void XELF::setHdr32_machine(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), nValue, isBigEndian());
}

quint32 XELF::getHdr32_version()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), isBigEndian());
}

void XELF::setHdr32_version(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), nValue, isBigEndian());
}

quint32 XELF::getHdr32_entry()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), isBigEndian());
}

void XELF::setHdr32_entry(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), nValue, isBigEndian());
}

quint32 XELF::getHdr32_phoff()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), isBigEndian());
}

void XELF::setHdr32_phoff(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), nValue, isBigEndian());
}

quint32 XELF::getHdr32_shoff()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), isBigEndian());
}

void XELF::setHdr32_shoff(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), nValue, isBigEndian());
}

quint32 XELF::getHdr32_flags()
{
    return read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), isBigEndian());
}

void XELF::setHdr32_flags(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), nValue, isBigEndian());
}

quint16 XELF::getHdr32_ehsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), isBigEndian());
}

void XELF::setHdr32_ehsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_phentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), isBigEndian());
}

void XELF::setHdr32_phentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_phnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), isBigEndian());
}

void XELF::setHdr32_phnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), isBigEndian());
}

void XELF::setHdr32_shentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), isBigEndian());
}

void XELF::setHdr32_shnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), nValue, isBigEndian());
}

quint16 XELF::getHdr32_shstrndx()
{
    return read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), isBigEndian());
}

void XELF::setHdr32_shstrndx(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), nValue, isBigEndian());
}

quint16 XELF::getHdr64_type()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), isBigEndian());
}

void XELF::setHdr64_type(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), nValue, isBigEndian());
}

quint16 XELF::getHdr64_machine()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), isBigEndian());
}

void XELF::setHdr64_machine(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), nValue, isBigEndian());
}

quint32 XELF::getHdr64_version()
{
    return read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), isBigEndian());
}

void XELF::setHdr64_version(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), nValue, isBigEndian());
}

quint64 XELF::getHdr64_entry()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), isBigEndian());
}

void XELF::setHdr64_entry(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), nValue, isBigEndian());
}

quint64 XELF::getHdr64_phoff()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), isBigEndian());
}

void XELF::setHdr64_phoff(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), nValue, isBigEndian());
}

quint64 XELF::getHdr64_shoff()
{
    return read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), isBigEndian());
}

void XELF::setHdr64_shoff(quint64 nValue)
{
    write_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), nValue, isBigEndian());
}

quint32 XELF::getHdr64_flags()
{
    return read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), isBigEndian());
}

void XELF::setHdr64_flags(quint32 nValue)
{
    write_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), nValue, isBigEndian());
}

quint16 XELF::getHdr64_ehsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), isBigEndian());
}

void XELF::setHdr64_ehsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_phentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), isBigEndian());
}

void XELF::setHdr64_phentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_phnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), isBigEndian());
}

void XELF::setHdr64_phnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shentsize()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), isBigEndian());
}

void XELF::setHdr64_shentsize(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shnum()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), isBigEndian());
}

void XELF::setHdr64_shnum(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), nValue, isBigEndian());
}

quint16 XELF::getHdr64_shstrndx()
{
    return read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), isBigEndian());
}

void XELF::setHdr64_shstrndx(quint16 nValue)
{
    write_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), nValue, isBigEndian());
}

XELF_DEF::Elf_Ehdr XELF::getHdr()
{
    XELF_DEF::Elf_Ehdr result = {};

    bool bIsbigEndian = isBigEndian();

    if (!is64()) {
        result.e_type = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_type), bIsbigEndian);
        result.e_machine = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_machine), bIsbigEndian);
        result.e_version = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_version), bIsbigEndian);
        result.e_entry = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_entry), bIsbigEndian);
        result.e_phoff = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_phoff), bIsbigEndian);
        result.e_shoff = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_shoff), bIsbigEndian);
        result.e_flags = read_uint32(offsetof(XELF_DEF::Elf32_Ehdr, e_flags), bIsbigEndian);
        result.e_ehsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_ehsize), bIsbigEndian);
        result.e_phentsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phentsize), bIsbigEndian);
        result.e_phnum = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_phnum), bIsbigEndian);
        result.e_shentsize = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shentsize), bIsbigEndian);
        result.e_shnum = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shnum), bIsbigEndian);
        result.e_shstrndx = read_uint16(offsetof(XELF_DEF::Elf32_Ehdr, e_shstrndx), bIsbigEndian);
    } else {
        result.e_type = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_type), bIsbigEndian);
        result.e_machine = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_machine), bIsbigEndian);
        result.e_version = read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_version), bIsbigEndian);
        result.e_entry = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_entry), bIsbigEndian);
        result.e_phoff = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_phoff), bIsbigEndian);
        result.e_shoff = read_uint64(offsetof(XELF_DEF::Elf64_Ehdr, e_shoff), bIsbigEndian);
        result.e_flags = read_uint32(offsetof(XELF_DEF::Elf64_Ehdr, e_flags), bIsbigEndian);
        result.e_ehsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_ehsize), bIsbigEndian);
        result.e_phentsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phentsize), bIsbigEndian);
        result.e_phnum = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_phnum), bIsbigEndian);
        result.e_shentsize = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shentsize), bIsbigEndian);
        result.e_shnum = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shnum), bIsbigEndian);
        result.e_shstrndx = read_uint16(offsetof(XELF_DEF::Elf64_Ehdr, e_shstrndx), bIsbigEndian);
    }

    return result;
}

const QString XELF::PREFIX_ET = "ET";
const QString XELF::PREFIX_EM = "EM";
const QString XELF::PREFIX_EV = "EV";
const QString XELF::PREFIX_ELFCLASS = "ELFCLASS";
const QString XELF::PREFIX_ELFDATA = "ELFDATA";
const QString XELF::PREFIX_ELFOSABI = "ELFOSABI";
const QString XELF::PREFIX_SHT = "SHT";
const QString XELF::PREFIX_SHF = "SHF";
const QString XELF::PREFIX_PT = "PT";
const QString XELF::PREFIX_PF = "PF";
const QString XELF::PREFIX_DT = "DT";
const QString XELF::PREFIX_R = "R";
const QString XELF::PREFIX_STB = "STB";
const QString XELF::PREFIX_STT = "STT";

QMap<quint64, QString> XELF::getTypes()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_Types, sizeof(_TABLE_XELF_Types) / sizeof(XBinary::XIDSTRING), PREFIX_ET);
}

QMap<quint64, QString> XELF::getTypesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_Types, sizeof(_TABLE_XELF_Types) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getMachines()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_Machines, sizeof(_TABLE_XELF_Machines) / sizeof(XBinary::XIDSTRING), PREFIX_EM);
}

QMap<quint64, QString> XELF::getMachinesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_Machines, sizeof(_TABLE_XELF_Machines) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getHeaderVersionList()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_Versions, sizeof(_TABLE_XELF_Versions) / sizeof(XBinary::XIDSTRING), PREFIX_EV);
}

QMap<quint64, QString> XELF::getIndentMag()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_IndentMag, sizeof(_TABLE_XELF_IndentMag) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getIndentMagS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_IndentMag, sizeof(_TABLE_XELF_IndentMag) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getIndentClasses()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_IndentClasses, sizeof(_TABLE_XELF_IndentClasses) / sizeof(XBinary::XIDSTRING), PREFIX_ELFCLASS);
}

QMap<quint64, QString> XELF::getIndentClassesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_IndentClasses, sizeof(_TABLE_XELF_IndentClasses) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getIndentDatas()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_IndentDatas, sizeof(_TABLE_XELF_IndentDatas) / sizeof(XBinary::XIDSTRING), PREFIX_ELFDATA);
}

QMap<quint64, QString> XELF::getIndentDatasS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_IndentDatas, sizeof(_TABLE_XELF_IndentDatas) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getIndentVersions()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_Versions, sizeof(_TABLE_XELF_Versions) / sizeof(XBinary::XIDSTRING), PREFIX_EV);
}

QMap<quint64, QString> XELF::getIndentVersionsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_Versions, sizeof(_TABLE_XELF_Versions) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getIndentOsabis()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_IndentOsabis, sizeof(_TABLE_XELF_IndentOsabis) / sizeof(XBinary::XIDSTRING), PREFIX_ELFOSABI);
}

QMap<quint64, QString> XELF::getIndentOsabisS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_IndentOsabis, sizeof(_TABLE_XELF_IndentOsabis) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getSectionTypes(const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_SectionTypes, sizeof(_TABLE_XELF_SectionTypes) / sizeof(XBinary::XIDSTRING), PREFIX_SHT);

    XBinary::XIDSTRING *pArchTable = nullptr;
    qint32 nArchCount = 0;

    if (sArch == "ARM") {
        pArchTable = _TABLE_XELF_SectionTypes_ARM;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_ARM) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_SectionTypes_MIPS;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_MIPS) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "AMD64") {
        pArchTable = _TABLE_XELF_SectionTypes_AMD64;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_AMD64) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, PREFIX_SHT + "_" + pArchTable[i].sString);
    }

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionTypesS(const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMap(_TABLE_XELF_SectionTypes, sizeof(_TABLE_XELF_SectionTypes) / sizeof(XBinary::XIDSTRING));

    XBinary::XIDSTRING *pArchTable = nullptr;
    qint32 nArchCount = 0;

    if (sArch == "ARM") {
        pArchTable = _TABLE_XELF_SectionTypes_ARM;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_ARM) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_SectionTypes_MIPS;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_MIPS) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "AMD64") {
        pArchTable = _TABLE_XELF_SectionTypes_AMD64;
        nArchCount = sizeof(_TABLE_XELF_SectionTypes_AMD64) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, pArchTable[i].sString);
    }

    return mapResult;
}

QMap<quint64, QString> XELF::getSectionFlags()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_SectionFlags, sizeof(_TABLE_XELF_SectionFlags) / sizeof(XBinary::XIDSTRING), PREFIX_SHF);
}

QMap<quint64, QString> XELF::getSectionFlagsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_SectionFlags, sizeof(_TABLE_XELF_SectionFlags) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getProgramTypes(qint32 nIndent, const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_ProgramTypes_Base, sizeof(_TABLE_XELF_ProgramTypes_Base) / sizeof(XBinary::XIDSTRING), PREFIX_PT);

    XBinary::XIDSTRING *pOsabiTable = _TABLE_XELF_ProgramTypes_GNU;
    qint32 nOsabiCount = sizeof(_TABLE_XELF_ProgramTypes_GNU) / sizeof(XBinary::XIDSTRING);

    if (nIndent == XELF_DEF::S_ELFOSABI_SOLARIS) {
        pOsabiTable = _TABLE_XELF_ProgramTypes_Solaris;
        nOsabiCount = sizeof(_TABLE_XELF_ProgramTypes_Solaris) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nOsabiCount; i++) {
        mapResult.insert(pOsabiTable[i].nID, PREFIX_PT + "_" + pOsabiTable[i].sString);
    }

    if (nIndent == XELF_DEF::S_ELFOSABI_OPENBSD) {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_OpenBSD) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_OpenBSD[i].nID, PREFIX_PT + "_" + _TABLE_XELF_ProgramTypes_OpenBSD[i].sString);
        }
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_Cont) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_Cont[i].nID, PREFIX_PT + "_" + _TABLE_XELF_ProgramTypes_Cont[i].sString);
        }
    }

    XBinary::XIDSTRING *pArchTable = _TABLE_XELF_ProgramTypes_LOPROC;
    qint32 nArchCount = sizeof(_TABLE_XELF_ProgramTypes_LOPROC) / sizeof(XBinary::XIDSTRING);

    if (sArch == "ARM") {
        pArchTable = _TABLE_XELF_ProgramTypes_ARM;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_ARM) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_ProgramTypes_MIPS;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_MIPS) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "AARCH64") {
        pArchTable = _TABLE_XELF_ProgramTypes_AARCH64;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_AARCH64) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "S390") {
        pArchTable = _TABLE_XELF_ProgramTypes_S390;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_S390) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, PREFIX_PT + "_" + pArchTable[i].sString);
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_Tail) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_Tail[i].nID, PREFIX_PT + "_" + _TABLE_XELF_ProgramTypes_Tail[i].sString);
        }
    }

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramTypesS(qint32 nIndent, const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMap(_TABLE_XELF_ProgramTypes_Base, sizeof(_TABLE_XELF_ProgramTypes_Base) / sizeof(XBinary::XIDSTRING));

    XBinary::XIDSTRING *pOsabiTable = _TABLE_XELF_ProgramTypes_GNU;
    qint32 nOsabiCount = sizeof(_TABLE_XELF_ProgramTypes_GNU) / sizeof(XBinary::XIDSTRING);

    if (nIndent == XELF_DEF::S_ELFOSABI_SOLARIS) {
        pOsabiTable = _TABLE_XELF_ProgramTypes_Solaris;
        nOsabiCount = sizeof(_TABLE_XELF_ProgramTypes_Solaris) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nOsabiCount; i++) {
        mapResult.insert(pOsabiTable[i].nID, pOsabiTable[i].sString);
    }

    if (nIndent == XELF_DEF::S_ELFOSABI_OPENBSD) {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_OpenBSD) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_OpenBSD[i].nID, _TABLE_XELF_ProgramTypes_OpenBSD[i].sString);
        }
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_Cont) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_Cont[i].nID, _TABLE_XELF_ProgramTypes_Cont[i].sString);
        }
    }

    XBinary::XIDSTRING *pArchTable = _TABLE_XELF_ProgramTypes_LOPROC;
    qint32 nArchCount = sizeof(_TABLE_XELF_ProgramTypes_LOPROC) / sizeof(XBinary::XIDSTRING);

    if (sArch == "ARM") {
        pArchTable = _TABLE_XELF_ProgramTypes_ARM;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_ARM) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_ProgramTypes_MIPS;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_MIPS) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "AARCH64") {
        pArchTable = _TABLE_XELF_ProgramTypes_AARCH64;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_AARCH64) / sizeof(XBinary::XIDSTRING);
    } else if (sArch == "S390") {
        pArchTable = _TABLE_XELF_ProgramTypes_S390;
        nArchCount = sizeof(_TABLE_XELF_ProgramTypes_S390) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, pArchTable[i].sString);
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_ProgramTypes_Tail) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_ProgramTypes_Tail[i].nID, _TABLE_XELF_ProgramTypes_Tail[i].sString);
        }
    }

    return mapResult;
}

QMap<quint64, QString> XELF::getProgramFlags()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_ProgramFlags, sizeof(_TABLE_XELF_ProgramFlags) / sizeof(XBinary::XIDSTRING), PREFIX_PF);
}

QMap<quint64, QString> XELF::getProgramFlagsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_ProgramFlags, sizeof(_TABLE_XELF_ProgramFlags) / sizeof(XBinary::XIDSTRING));
}

quint32 XELF::getSectionStringTable()
{
    return getSectionStringTable(is64());
}

quint32 XELF::getSectionStringTable(bool bIs64)
{
    quint32 nResult = 0;

    if (bIs64) {
        nResult = getHdr64_shstrndx();
    } else {
        nResult = getHdr32_shstrndx();
    }

    if (nResult == 0) {
        quint64 nOffset = 0;

        if (bIs64) {
            nOffset = getHdr64_shoff();
        } else {
            nOffset = getHdr32_shoff();
        }

        if (nOffset) {
            bool bIsBigEndian = isBigEndian();

            if (bIs64) {
                XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_link;
            } else {
                XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_link;
            }
        }
    }

    return nResult;
}

XBinary::OFFSETSIZE XELF::getSectionOffsetSize(quint32 nSection)
{
    OFFSETSIZE osResult = {};

    if (is64()) {
        XELF_DEF::Elf64_Shdr section_header = getElf64_Shdr(nSection);
        osResult.nOffset = section_header.sh_offset;
        osResult.nSize = section_header.sh_size;
    } else {
        XELF_DEF::Elf32_Shdr section_header = getElf32_Shdr(nSection);
        osResult.nOffset = section_header.sh_offset;
        osResult.nSize = section_header.sh_size;
    }

    return osResult;
}

QMap<quint32, QString> XELF::getStringsFromSection(quint32 nSection)
{
    QMap<quint32, QString> mapResult;

    if (nSection != XELF_DEF::S_SHN_UNDEF) {
        QByteArray baSection = getSection(nSection);

        mapResult = getStringsFromSectionData(&baSection);
    }

    return mapResult;
}

QMap<quint32, QString> XELF::getStringsFromSectionData(QByteArray *pbaData)
{
    QMap<quint32, QString> mapResult;

    qint32 nSize = pbaData->size();
    char *pOffset = pbaData->data();
    quint32 nCurrentOffset = 0;

    while (nSize > 0) {
        QString sString(pOffset + nCurrentOffset);

        if (sString.length()) {
            mapResult.insert(nCurrentOffset, sString);
        }

        nCurrentOffset += (quint32)sString.length() + 1;
        nSize -= sString.length() + 1;
    }

    return mapResult;
}

QString XELF::getStringFromSection(quint32 nIndex, quint32 nSection)
{
    QString sResult;

    XBinary::OFFSETSIZE osRegion = getSectionOffsetSize(nSection);

    sResult = getStringFromIndex(osRegion.nOffset, osRegion.nSize, nIndex);

    return sResult;
}

QMap<quint32, QString> XELF::getStringsFromMainSection()
{
    quint32 nSection = getSectionStringTable();

    return getStringsFromSection(nSection);
}

QString XELF::getStringFromMainSection(quint32 nIndex)
{
    quint32 nSection = getSectionStringTable();

    return getStringFromSection(nIndex, nSection);  // TODO optimize
}

QByteArray XELF::getSection(quint32 nIndex)
{
    qint64 nOffset = 0;
    qint64 nSize = 0;

    if (is64()) {
        XELF_DEF::Elf64_Shdr section_header = getElf64_Shdr(nIndex);
        nSize = section_header.sh_size;
        nOffset = (isImage()) ? (section_header.sh_addr) : (section_header.sh_offset);
    } else {
        XELF_DEF::Elf32_Shdr section_header = getElf32_Shdr(nIndex);
        nSize = section_header.sh_size;
        nOffset = (isImage()) ? (section_header.sh_addr) : (section_header.sh_offset);
    }

    return read_array(nOffset, nSize);
}

bool XELF::isSectionValid(quint32 nIndex)
{
    quint32 nNumberOfSections = getNumberOfSections();

    return (nIndex < nNumberOfSections) && (nIndex != (quint32)-1);
}

QList<XELF_DEF::Elf32_Shdr> XELF::getElf32_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf32_Shdr> listResult;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 nOffset = getHdr32_shoff();

    if (nOffset) {
        bool bIsBigEndian = isBigEndian();

        if (nLimit != -1) {
            nNumberOfSections = qMin((quint32)nLimit, nNumberOfSections);
        }

        for (quint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);

            listResult.append(record);

            nOffset += sizeof(XELF_DEF::Elf32_Shdr);
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Shdr> XELF::getElf64_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf64_Shdr> listResult;

    quint32 nNumberOfSections = getNumberOfSections();

    quint64 nOffset = getHdr64_shoff();

    if (nOffset) {
        bool bIsBigEndian = isBigEndian();

        if (nLimit != -1) {
            nNumberOfSections = qMin((quint32)nLimit, nNumberOfSections);
        }

        for (quint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);

            listResult.append(record);

            nOffset += sizeof(XELF_DEF::Elf64_Shdr);
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf_Shdr> XELF::getElf_ShdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf_Shdr> listResult;

    bool bIs64 = is64();

    if (bIs64) {
        QList<XELF_DEF::Elf64_Shdr> listSectionHeaders = getElf64_ShdrList(nLimit);
        qint32 nNumberOfSections = listSectionHeaders.count();

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf_Shdr record = {};

            record.sh_name = listSectionHeaders.at(i).sh_name;
            record.sh_type = listSectionHeaders.at(i).sh_type;
            record.sh_flags = listSectionHeaders.at(i).sh_flags;
            record.sh_addr = listSectionHeaders.at(i).sh_addr;
            record.sh_offset = listSectionHeaders.at(i).sh_offset;
            record.sh_size = listSectionHeaders.at(i).sh_size;
            record.sh_link = listSectionHeaders.at(i).sh_link;
            record.sh_info = listSectionHeaders.at(i).sh_info;
            record.sh_addralign = listSectionHeaders.at(i).sh_addralign;
            record.sh_entsize = listSectionHeaders.at(i).sh_entsize;

            listResult.append(record);
        }
    } else {
        QList<XELF_DEF::Elf32_Shdr> listSectionHeaders = getElf32_ShdrList(nLimit);
        qint32 nNumberOfSections = listSectionHeaders.count();

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            XELF_DEF::Elf_Shdr record = {};

            record.sh_name = listSectionHeaders.at(i).sh_name;
            record.sh_type = listSectionHeaders.at(i).sh_type;
            record.sh_flags = listSectionHeaders.at(i).sh_flags;
            record.sh_addr = listSectionHeaders.at(i).sh_addr;
            record.sh_offset = listSectionHeaders.at(i).sh_offset;
            record.sh_size = listSectionHeaders.at(i).sh_size;
            record.sh_link = listSectionHeaders.at(i).sh_link;
            record.sh_info = listSectionHeaders.at(i).sh_info;
            record.sh_addralign = listSectionHeaders.at(i).sh_addralign;
            record.sh_entsize = listSectionHeaders.at(i).sh_entsize;

            listResult.append(record);
        }
    }

    return listResult;
}

XELF_DEF::Elf32_Shdr XELF::getElf32_Shdr(quint32 nIndex)
{
    XELF_DEF::Elf32_Shdr result = {};

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 nOffset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);

        result = _readElf32_Shdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf64_Shdr XELF::getElf64_Shdr(quint32 nIndex)
{
    XELF_DEF::Elf64_Shdr result = {};

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 nOffset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);

        result = _readElf64_Shdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf32_Shdr XELF::_readElf32_Shdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Shdr result = {};

    result.sh_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), bIsBigEndian);
    result.sh_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), bIsBigEndian);
    result.sh_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), bIsBigEndian);
    result.sh_addr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), bIsBigEndian);
    result.sh_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), bIsBigEndian);
    result.sh_size = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), bIsBigEndian);
    result.sh_link = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), bIsBigEndian);
    result.sh_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), bIsBigEndian);
    result.sh_addralign = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), bIsBigEndian);
    result.sh_entsize = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Shdr XELF::_readElf64_Shdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Shdr result = {};

    result.sh_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), bIsBigEndian);
    result.sh_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), bIsBigEndian);
    result.sh_flags = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), bIsBigEndian);
    result.sh_addr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), bIsBigEndian);
    result.sh_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), bIsBigEndian);
    result.sh_size = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), bIsBigEndian);
    result.sh_link = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), bIsBigEndian);
    result.sh_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), bIsBigEndian);
    result.sh_addralign = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), bIsBigEndian);
    result.sh_entsize = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), bIsBigEndian);

    return result;
}

quint32 XELF::getElf32_Shdr_name(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_addr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_offset(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_size(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_link(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_info(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_addralign(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Shdr_entsize(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), bIsBigEndian);
    }

    return result;
}

void XELF::setElf32_Shdr_name(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_name), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_addr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_offset(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_size(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_size), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_link(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_link), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_info(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_info), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_addralign(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_addralign), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Shdr_entsize(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint32 offset = getHdr32_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Shdr, sh_entsize), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf64_Shdr_name(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_flags(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_addr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_offset(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_size(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_link(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Shdr_info(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_addralign(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Shdr_entsize(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), bIsBigEndian);
    }

    return result;
}

void XELF::setElf64_Shdr_name(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_name), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_flags(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_addr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_offset(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_size(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_size), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_link(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_link), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_info(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_info), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_addralign(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_addralign), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Shdr_entsize(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfSections = getNumberOfSections();
    quint64 offset = getHdr64_shoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfSections) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Shdr, sh_entsize), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf_Shdr_name(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_name;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_type;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_flags;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_addr(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_addr;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_offset;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_size(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_size;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_link(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_link;
    }

    return nResult;
}

quint32 XELF::getElf_Shdr_info(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_info;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_addralign(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_addralign;
    }

    return nResult;
}

quint64 XELF::getElf_Shdr_entsize(quint32 nIndex, QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListSectionHeaders->count()) {
        nResult = pListSectionHeaders->at(nIndex).sh_entsize;
    }

    return nResult;
}

qint64 XELF::getShdrOffset(quint32 nIndex)
{
    quint64 nResult = -1;

    bool bIs64 = is64();
    quint32 nNumberOfSections = 0;

    if (bIs64) {
        nNumberOfSections = getNumberOfSections();

        if (nIndex < nNumberOfSections) {
            nResult = getHdr64_shoff() + nIndex * sizeof(XELF_DEF::Elf64_Shdr);
        }
    } else {
        nNumberOfSections = getNumberOfSections();

        if (nIndex < nNumberOfSections) {
            nResult = getHdr32_shoff() + nIndex * sizeof(XELF_DEF::Elf32_Shdr);
        }
    }

    return nResult;
}

qint64 XELF::getShdrSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Shdr);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Shdr);
    }

    return nResult;
}

QList<XELF_DEF::Elf32_Phdr> XELF::getElf32_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf32_Phdr> result;

    quint32 nNumberOfProgramms = getHdr32_phnum();

    if (nLimit != -1) {
        nNumberOfProgramms = qMin((quint32)nLimit, nNumberOfProgramms);
    }

    quint32 nOffset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    for (quint32 i = 0; i < nNumberOfProgramms; i++) {
        XELF_DEF::Elf32_Phdr record = _readElf32_Phdr(nOffset, bIsBigEndian);

        result.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Phdr);
    }

    return result;
}

QList<XELF_DEF::Elf64_Phdr> XELF::getElf64_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf64_Phdr> result;

    quint32 nNumberOfProgramms = getHdr64_phnum();

    if (nLimit != -1) {
        nNumberOfProgramms = qMin((quint32)nLimit, nNumberOfProgramms);
    }

    quint64 nOffset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    for (quint32 i = 0; i < nNumberOfProgramms; i++) {
        XELF_DEF::Elf64_Phdr record = _readElf64_Phdr(nOffset, bIsBigEndian);

        result.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Phdr);
    }

    return result;
}

QList<XELF_DEF::Elf_Phdr> XELF::getElf_PhdrList(qint32 nLimit)
{
    QList<XELF_DEF::Elf_Phdr> listResult;

    bool bIs64 = is64();

    if (bIs64) {
        QList<XELF_DEF::Elf64_Phdr> listProgramHeaders = getElf64_PhdrList(nLimit);

        qint32 nNumberOfPrograms = listProgramHeaders.count();

        for (qint32 i = 0; i < nNumberOfPrograms; i++) {
            XELF_DEF::Elf_Phdr record = {};

            record.p_type = listProgramHeaders.at(i).p_type;
            record.p_flags = listProgramHeaders.at(i).p_flags;
            record.p_offset = listProgramHeaders.at(i).p_offset;
            record.p_vaddr = listProgramHeaders.at(i).p_vaddr;
            record.p_paddr = listProgramHeaders.at(i).p_paddr;
            record.p_filesz = listProgramHeaders.at(i).p_filesz;
            record.p_memsz = listProgramHeaders.at(i).p_memsz;
            record.p_align = listProgramHeaders.at(i).p_align;

            listResult.append(record);
        }
    } else {
        QList<XELF_DEF::Elf32_Phdr> listProgramHeaders = getElf32_PhdrList(nLimit);

        qint32 nNumberOfPrograms = listProgramHeaders.count();

        for (qint32 i = 0; i < nNumberOfPrograms; i++) {
            XELF_DEF::Elf_Phdr record = {};

            record.p_type = listProgramHeaders.at(i).p_type;
            record.p_flags = listProgramHeaders.at(i).p_flags;
            record.p_offset = listProgramHeaders.at(i).p_offset;
            record.p_vaddr = listProgramHeaders.at(i).p_vaddr;
            record.p_paddr = listProgramHeaders.at(i).p_paddr;
            record.p_filesz = listProgramHeaders.at(i).p_filesz;
            record.p_memsz = listProgramHeaders.at(i).p_memsz;
            record.p_align = listProgramHeaders.at(i).p_align;

            listResult.append(record);
        }
    }

    return listResult;
}

XELF_DEF::Elf32_Phdr XELF::getElf32_Phdr(quint32 nIndex)
{
    XELF_DEF::Elf32_Phdr result = {};

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 nOffset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = _readElf32_Phdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf64_Phdr XELF::getElf64_Phdr(quint32 nIndex)
{
    XELF_DEF::Elf64_Phdr result = {};

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 nOffset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        nOffset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = _readElf64_Phdr(nOffset, bIsBigEndian);
    }

    return result;
}

XELF_DEF::Elf32_Phdr XELF::_readElf32_Phdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Phdr result = {};

    result.p_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_type), bIsBigEndian);
    result.p_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), bIsBigEndian);
    result.p_vaddr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), bIsBigEndian);
    result.p_paddr = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), bIsBigEndian);
    result.p_filesz = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), bIsBigEndian);
    result.p_memsz = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), bIsBigEndian);
    result.p_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), bIsBigEndian);
    result.p_align = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Phdr, p_align), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Phdr XELF::_readElf64_Phdr(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Phdr result = {};

    result.p_type = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_type), bIsBigEndian);
    result.p_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), bIsBigEndian);
    result.p_vaddr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), bIsBigEndian);
    result.p_paddr = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), bIsBigEndian);
    result.p_filesz = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), bIsBigEndian);
    result.p_memsz = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), bIsBigEndian);
    result.p_flags = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), bIsBigEndian);
    result.p_align = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Phdr, p_align), bIsBigEndian);

    return result;
}

quint32 XELF::getElf32_Phdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_type), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_offset(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_vaddr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_paddr(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_filesz(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_memsz(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf32_Phdr_align(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_align), bIsBigEndian);
    }

    return result;
}

void XELF::setElf32_Phdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_offset(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_vaddr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_vaddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_paddr(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_paddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_filesz(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_filesz), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_memsz(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_memsz), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf32_Phdr_align(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr32_phnum();
    quint32 offset = getHdr32_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf32_Phdr, p_align), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf64_Phdr_type(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_type), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_offset(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_vaddr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_paddr(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_filesz(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_memsz(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), bIsBigEndian);
    }

    return result;
}

quint32 XELF::getElf64_Phdr_flags(quint32 nIndex)
{
    quint32 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), bIsBigEndian);
    }

    return result;
}

quint64 XELF::getElf64_Phdr_align(quint32 nIndex)
{
    quint64 result = 0;

    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        result = read_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_align), bIsBigEndian);
    }

    return result;
}

void XELF::setElf64_Phdr_type(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_type), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_offset(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_offset), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_vaddr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_vaddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_paddr(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_paddr), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_filesz(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_filesz), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_memsz(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_memsz), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_flags(quint32 nIndex, quint32 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint32(offset + offsetof(XELF_DEF::Elf64_Phdr, p_flags), nValue, bIsBigEndian);
    }
}

void XELF::setElf64_Phdr_align(quint32 nIndex, quint64 nValue)
{
    quint32 nNumberOfPrograms = getHdr64_phnum();
    quint64 offset = getHdr64_phoff();
    bool bIsBigEndian = isBigEndian();

    if (nIndex < nNumberOfPrograms) {
        offset += nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        write_uint64(offset + offsetof(XELF_DEF::Elf64_Phdr, p_align), nValue, bIsBigEndian);
    }
}

quint32 XELF::getElf_Phdr_type(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_type;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_offset(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_offset;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_vaddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_vaddr;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_paddr(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_paddr;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_filesz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_filesz;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_memsz(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_memsz;
    }

    return nResult;
}

quint32 XELF::getElf_Phdr_flags(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint32 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_flags;
    }

    return nResult;
}

quint64 XELF::getElf_Phdr_align(quint32 nIndex, QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    quint64 nResult = 0;

    if (nIndex < (quint32)pListProgramHeaders->count()) {
        nResult = pListProgramHeaders->at(nIndex).p_align;
    }

    return nResult;
}

qint64 XELF::getPhdrOffset(quint32 nIndex)
{
    quint64 nResult = -1;

    bool bIs64 = is64();
    quint32 nNumberOfPrograms = 0;

    if (bIs64) {
        nNumberOfPrograms = getHdr64_phnum();

        if (nIndex < nNumberOfPrograms) {
            nResult = getHdr64_phoff() + nIndex * sizeof(XELF_DEF::Elf64_Phdr);
        }
    } else {
        nNumberOfPrograms = getHdr32_phnum();

        if (nIndex < nNumberOfPrograms) {
            nResult = getHdr32_phoff() + nIndex * sizeof(XELF_DEF::Elf32_Phdr);
        }
    }

    return nResult;
}

qint64 XELF::getPhdrSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Phdr);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Phdr);
    }

    return nResult;
}

qint32 XELF::getSectionIndexByName(const QString &sSectionName)
{
    qint32 nResult = -1;

    quint32 nNumberOfSections = getNumberOfSections();

    if (is64()) {
        for (quint32 i = 0; i < nNumberOfSections; i++) {
            quint32 nCurrentNameIndex = getElf64_Shdr_name(i);

            if (getStringFromMainSection(nCurrentNameIndex) == sSectionName) {
                nResult = i;
                break;
            }
        }
    } else {
        for (quint32 i = 0; i < nNumberOfSections; i++) {
            quint32 nCurrentNameIndex = getElf32_Shdr_name(i);

            if (getStringFromMainSection(nCurrentNameIndex) == sSectionName) {
                nResult = i;
                break;
            }
        }
    }

    return nResult;
}

qint32 XELF::getSectionIndexByName(const QString &sSectionName, QList<SECTION_RECORD> *pListSectionRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            nResult = i;
            break;
        }
    }

    return nResult;
}

QByteArray XELF::getSectionByName(const QString &sSectionName)
{
    QByteArray baResult;

    quint32 nIndex = getSectionIndexByName(sSectionName);

    if (nIndex != (quint32)-1) {
        baResult = getSection(nIndex);
    }

    return baResult;
}

XBinary::OS_STRING XELF::getProgramInterpreterName()
{
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getProgramInterpreterName(&listProgramHeaders);
}

XBinary::OS_STRING XELF::getProgramInterpreterName(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    OS_STRING result = {};

    QList<XELF_DEF::Elf_Phdr> listInterps = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_INTERP);

    if (listInterps.count()) {
        result = getOsAnsiString(listInterps.at(0).p_offset, listInterps.at(0).p_filesz);
    }

    return result;
}

XBinary::OS_STRING XELF::getProgramInterpreterName(QList<SECTION_RECORD> *pListSectionRecords)
{
    OS_STRING result = {};

    QList<SECTION_RECORD> listInterps = _getSectionRecords(pListSectionRecords, ".interp");

    if (listInterps.count()) {
        result = getOsAnsiString(listInterps.at(0).nOffset, listInterps.at(0).nSize);
    }

    return result;
}

QList<QString> XELF::getCommentStrings(QList<SECTION_RECORD> *pListSectionRecords)
{
    // TODO Optimize
    return getCommentStrings(getSectionIndexByName(".comment", pListSectionRecords));
}

QList<QString> XELF::getCommentStrings(qint32 nSection)
{
    return getStringsFromSection(nSection).values();
}

QList<XELF::NOTE> XELF::getNotes()
{
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getNotes(&listProgramHeaders);
}

// QString XELF::getCompatibleKernelVersion()
//{
//     QString sResult;

//    QByteArray baData=getSectionByName(".note.ABI-tag");
//    bool bIsBigEndian=isBigEndian();
//    NOTE note=getNote(baData,bIsBigEndian);

//    if((note.nType==1)&&(note.name=="GNU"))
//    {
//        quint32 kv[4];
//        kv[0]=_read_uint32(note.desc.data()+0,bIsBigEndian);
//        kv[1]=_read_uint32(note.desc.data()+4,bIsBigEndian);
//        kv[2]=_read_uint32(note.desc.data()+8,bIsBigEndian);
//        kv[3]=_read_uint32(note.desc.data()+12,bIsBigEndian);

//        if(kv[0]==0)
//        {
//            sResult=QString("%1.%2.%3").arg(QString::number(kv[1]),QString::number(kv[2]),QString::number(kv[3]));
//        }
//    }

//    return sResult;
//}

QList<XELF::NOTE> XELF::getNotes(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    QList<NOTE> listResult;

    QList<XELF_DEF::Elf_Phdr> listNotes = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_NOTE);

    bool bIsBigEndian = isBigEndian();

    qint32 nNumberOfNotes = listNotes.count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        // TODO Image
        qint64 nOffset = listNotes.at(i).p_offset;
        qint64 nSize = listNotes.at(i).p_filesz;

        listResult.append(_getNotes(nOffset, nSize, bIsBigEndian));
    }

    return listResult;
}

QList<XELF::NOTE> XELF::getNotes(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    QList<NOTE> listResult;

    QList<XELF_DEF::Elf_Shdr> listNotes = _getSections(pListSectionHeaders, XELF_DEF::S_SHT_NOTE);

    bool bIsBigEndian = isBigEndian();

    qint32 nNumberOfNotes = listNotes.count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        // TODO Image
        qint64 nOffset = listNotes.at(i).sh_offset;
        qint64 nSize = listNotes.at(i).sh_size;

        listResult.append(_getNotes(nOffset, nSize, bIsBigEndian));
    }

    return listResult;
}

QList<XELF::NOTE> XELF::_getNotes(qint64 nOffset, qint64 nSize, bool bIsBigEndian)
{
    QList<NOTE> listResult;

    qint32 nNumberOfRecords = 0;

    while (nSize > 0) {
        NOTE note = _readNote(nOffset, nSize, bIsBigEndian);

        if (note.nSize) {
            listResult.append(note);

            nOffset += note.nSize;
            nSize -= note.nSize;
        } else {
            break;
        }

        nNumberOfRecords++;

        if (nNumberOfRecords > 1000) {
            break;
        }
    }

    return listResult;
}

XELF::NOTE XELF::_readNote(qint64 nOffset, qint64 nSize, bool bIsBigEndian)
{
    NOTE result = {};

    if (nSize >= 6) {
        quint32 nNameLength = read_uint32(nOffset + 0, bIsBigEndian);
        quint32 nDescLength = read_uint32(nOffset + 4, bIsBigEndian);
        quint32 nType = read_uint32(nOffset + 8, bIsBigEndian);

        qint32 nNoteSize = 12 + S_ALIGN_UP(nNameLength, 4) + S_ALIGN_UP(nDescLength, 4);

        if (nSize >= nNoteSize) {
            result.nOffset = nOffset;
            result.nSize = nNoteSize;
            result.nType = nType;

            if (nNameLength >= 1) {
                result.sName = read_ansiString(nOffset + 12, nNameLength - 1);
            }

            result.nDataOffset = nOffset + 12 + S_ALIGN_UP(nNameLength, 4);
            result.nDataSize = qMax((nOffset + nSize) - result.nDataOffset, (qint64)0);
        }
    }

    return result;
}

bool XELF::isNotePresent(const QString &sName)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, sName);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, const QString &sName)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if (pListNotes->at(i).sName == sName) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, const QString &sName)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if (pListNotes->at(i).sName == sName) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

bool XELF::isNotePresent(quint32 nType)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, nType);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, quint32 nType)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if (pListNotes->at(i).nType == nType) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, quint32 nType)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if (pListNotes->at(i).nType == nType) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

bool XELF::isNotePresent(quint32 nType, const QString &sName)
{
    QList<XELF::NOTE> listNotes = getNotes();

    return isNotePresent(&listNotes, nType, sName);
}

bool XELF::isNotePresent(QList<NOTE> *pListNotes, quint32 nType, const QString &sName)
{
    bool bResult = false;

    qint32 nNumberOfNotes = pListNotes->count();

    for (qint32 i = 0; i < nNumberOfNotes; i++) {
        if ((pListNotes->at(i).nType == nType) && (pListNotes->at(i).sName == sName)) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

XELF::NOTE XELF::getNote(QList<NOTE> *pListNotes, quint32 nType, const QString &sName)
{
    NOTE result = {};

    for (qint32 i = 0; i < pListNotes->count(); i++) {
        if ((pListNotes->at(i).nType == nType) && (pListNotes->at(i).sName == sName)) {
            result = pListNotes->at(i);
            break;
        }
    }

    return result;
}

QList<XELF::TAG_STRUCT> XELF::getTagStructs()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);

    return getTagStructs(&listProgramHeaders, &memoryMap);
}

QList<XELF::TAG_STRUCT> XELF::getTagStructs(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, XBinary::_MEMORY_MAP *pMemoryMap)
{
    QList<TAG_STRUCT> listResult;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    QList<XELF_DEF::Elf_Phdr> listTags = _getPrograms(pListProgramHeaders, XELF_DEF::S_PT_DYNAMIC);

    qint32 nNumberOfTags = listTags.count();

    for (qint32 i = 0; i < nNumberOfTags; i++) {
        qint64 nOffset = listTags.at(i).p_offset;  //  Check image
        qint64 nSize = listTags.at(i).p_filesz;

        if (isOffsetAndSizeValid(pMemoryMap, nOffset, nSize)) {
            listResult.append(_getTagStructs(nOffset, nSize, bIs64, bIsBigEndian));
        }
    }

    return listResult;
}

QList<XELF::TAG_STRUCT> XELF::_getTagStructs(qint64 nOffset, qint64 nSize, bool bIs64, bool bIsBigEndian)
{
    QList<TAG_STRUCT> listResult;

    while (nSize > 0) {
        TAG_STRUCT tagStruct = {};
        tagStruct.nOffset = nOffset;

        if (bIs64) {
            tagStruct.nTag = read_int64(nOffset, bIsBigEndian);
            tagStruct.nValue = read_int64(nOffset + 8, bIsBigEndian);
            nOffset += 16;
            nSize -= 16;
        } else {
            tagStruct.nTag = read_int32(nOffset, bIsBigEndian);
            tagStruct.nValue = read_int32(nOffset + 4, bIsBigEndian);
            nOffset += 8;
            nSize -= 8;
        }

        if (tagStruct.nTag) {
            listResult.append(tagStruct);
        } else {
            break;
        }
    }

    return listResult;
}

QList<XELF::TAG_STRUCT> XELF::_getTagStructs(QList<XELF::TAG_STRUCT> *pListTagStructs, qint64 nTag)
{
    QList<TAG_STRUCT> listResult;

    qint32 nNumberOfTags = pListTagStructs->count();

    for (qint32 i = 0; i < nNumberOfTags; i++) {
        if (pListTagStructs->at(i).nTag == nTag) {
            listResult.append(pListTagStructs->at(i));
        }
    }

    return listResult;
}

qint64 XELF::getDynamicArraySize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = 16;  // TODO defs
    } else {
        nResult = 8;  // TODO defs
    }

    return nResult;
}

qint64 XELF::getDynamicArrayTag(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        nResult = read_int64(nOffset, bIsBigEndian);
    } else {
        nResult = read_int32(nOffset, bIsBigEndian);
    }

    return nResult;
}

qint64 XELF::getDynamicArrayValue(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        nResult = read_int64(nOffset + 8, bIsBigEndian);
    } else {
        nResult = read_int32(nOffset + 4, bIsBigEndian);
    }

    return nResult;
}

void XELF::setDynamicArrayTag(qint64 nOffset, qint64 nValue)
{
    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        write_int64(nOffset, nValue, bIsBigEndian);
    } else {
        write_int32(nOffset, (qint32)nValue, bIsBigEndian);
    }
}

void XELF::setDynamicArrayValue(qint64 nOffset, qint64 nValue)
{
    bool bIs64 = is64();
    bool bIsBigEndian = isBigEndian();

    if (bIs64) {
        write_int64(nOffset + 8, nValue, bIsBigEndian);
    } else {
        write_int32(nOffset + 4, (qint32)nValue, bIsBigEndian);
    }
}

XBinary::OFFSETSIZE XELF::getStringTable()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listStructs = getTagStructs();

    return getStringTable(&memoryMap, &listStructs);
}

XBinary::OFFSETSIZE XELF::getStringTable(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    OFFSETSIZE osResult = {};

    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);

    if (listStrTab.count() && listStrSize.count()) {
        qint64 nOffset = addressToOffset(pMemoryMap, listStrTab.at(0).nValue);
        qint64 nSize = listStrSize.at(0).nValue;

        if (isOffsetAndSizeValid(pMemoryMap, nOffset, nSize)) {
            osResult.nOffset = nOffset;
            osResult.nSize = nSize;
        }
    }

    return osResult;
}

QList<QString> XELF::getLibraries()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listTagStructs = getTagStructs();

    return getLibraries(&memoryMap, &listTagStructs);
}

QList<QString> XELF::getLibraries(_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pList)
{
    QList<QString> listResult;

    QList<TAG_STRUCT> listNeeded = _getTagStructs(pList, XELF_DEF::S_DT_NEEDED);

    OFFSETSIZE osStringTable = getStringTable(pMemoryMap, pList);

    if (osStringTable.nSize) {
        qint64 nOffset = osStringTable.nOffset;

        QByteArray baSection = read_array(nOffset, osStringTable.nSize);

        qint64 nSectionTableSize = baSection.size();

        qint32 nNumberOfNeededs = listNeeded.count();

        for (qint32 i = 0; i < nNumberOfNeededs; i++) {
            qint64 nValue = listNeeded.at(i).nValue;

            if ((nValue >= 0) & (nValue < nSectionTableSize)) {
                QString sLibrary = baSection.data() + nValue;

                listResult.append(sLibrary);
            }
        }
    }

    return listResult;
}

XBinary::OS_STRING XELF::getRunPath()
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    QList<TAG_STRUCT> listTagStructs = getTagStructs();

    return getRunPath(&memoryMap, &listTagStructs);
}

XBinary::OS_STRING XELF::getRunPath(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    OS_STRING result = {};

    QList<TAG_STRUCT> listRunPath = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RUNPATH);
    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);

    if (listStrTab.count() && listStrSize.count() && listRunPath.count()) {
        qint64 nOffset = addressToOffset(pMemoryMap, listStrTab.at(0).nValue);
        qint64 nSize = listStrSize.at(0).nValue;
        qint64 nRunPath = listRunPath.at(0).nValue;

        if ((nRunPath >= 0) & (nRunPath < nSize)) {
            result.nOffset = nOffset + nRunPath;
            result.sString = read_ansiString(result.nOffset);
            result.nSize = result.sString.length();
        }
    }

    return result;
}

QList<XBinary::MAPMODE> XELF::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_SEGMENTS);
    listResult.append(MAPMODE_SECTIONS);

    return listResult;
}

QMap<quint64, QString> XELF::getDynamicTags(const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_DynamicTags_Base, sizeof(_TABLE_XELF_DynamicTags_Base) / sizeof(XBinary::XIDSTRING), PREFIX_DT);

    XBinary::XIDSTRING *pArchTable = _TABLE_XELF_DynamicTags_SPARC;
    qint32 nArchCount = sizeof(_TABLE_XELF_DynamicTags_SPARC) / sizeof(XBinary::XIDSTRING);

    if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_DynamicTags_MIPS;
        nArchCount = sizeof(_TABLE_XELF_DynamicTags_MIPS) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, PREFIX_DT + "_" + pArchTable[i].sString);
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_DynamicTags_Tail) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_DynamicTags_Tail[i].nID, PREFIX_DT + "_" + _TABLE_XELF_DynamicTags_Tail[i].sString);
        }
    }

    return mapResult;
}

QMap<quint64, QString> XELF::getDynamicTagsS(const QString &sArch)
{
    QMap<quint64, QString> mapResult = XBinary::XIDSTRING_createMap(_TABLE_XELF_DynamicTags_Base, sizeof(_TABLE_XELF_DynamicTags_Base) / sizeof(XBinary::XIDSTRING));

    XBinary::XIDSTRING *pArchTable = _TABLE_XELF_DynamicTags_SPARC;
    qint32 nArchCount = sizeof(_TABLE_XELF_DynamicTags_SPARC) / sizeof(XBinary::XIDSTRING);

    if (sArch == "MIPS") {
        pArchTable = _TABLE_XELF_DynamicTags_MIPS;
        nArchCount = sizeof(_TABLE_XELF_DynamicTags_MIPS) / sizeof(XBinary::XIDSTRING);
    }

    for (qint32 i = 0; i < nArchCount; i++) {
        mapResult.insert(pArchTable[i].nID, pArchTable[i].sString);
    }

    {
        qint32 nCount = sizeof(_TABLE_XELF_DynamicTags_Tail) / sizeof(XBinary::XIDSTRING);

        for (qint32 i = 0; i < nCount; i++) {
            mapResult.insert(_TABLE_XELF_DynamicTags_Tail[i].nID, _TABLE_XELF_DynamicTags_Tail[i].sString);
        }
    }

    return mapResult;
}

XBinary::_MEMORY_MAP XELF::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    // TODO Check alignment!
    XBinary::_MEMORY_MAP result = {};

    qint32 nIndex = 0;

    result.mode = getMode();
    if (result.mode == MODE_64) {
        result.fileType = FT_ELF64;
    } else {
        result.fileType = FT_ELF32;
    }

    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();
    // XADDR _nModuleAddress = getModuleAddress();
    result.nModuleAddress = getModuleAddress();
    result.nBinarySize = getSize();

    qint64 nMaxUsedOffset = 0;
    XADDR nMinUsedAddress = 0;
    XADDR nMaxUsedAddress = 0;

    if ((mapMode == MAPMODE_UNKNOWN) || (mapMode == MAPMODE_SEGMENTS)) {
        QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(1000);
        QList<XELF_DEF::Elf_Phdr> listSegments = _getPrograms(&listProgramHeaders, XELF_DEF::S_PT_LOAD);

        //    bool bIs64=is64();
        qint32 nNumberOfSegments = listSegments.count();

        if (nNumberOfSegments) {
            nMinUsedAddress = listSegments.at(0).p_vaddr;
        }

        for (qint32 i = 0; i < nNumberOfSegments; i++) {
            QString sName = QString("%1(%2)").arg(QString("PT_LOAD"), QString::number(i));

            quint64 nVirtualAlign = listSegments.at(i).p_align;  // TODO Check!

            if (nVirtualAlign <= 1) {
                nVirtualAlign = 1;
            }
            //        quint64 nFileAlign=0x1; // TODO Check!!!
            quint64 nFileAlign = nVirtualAlign;
            XADDR nVirtualAddress = S_ALIGN_DOWN64(listSegments.at(i).p_vaddr, nVirtualAlign);
            qint64 nFileOffset = S_ALIGN_DOWN64(listSegments.at(i).p_offset, nFileAlign);

            qint64 nVirtualDelta = listSegments.at(i).p_vaddr - nVirtualAddress;
            qint64 nFileDelta = listSegments.at(i).p_offset - nFileOffset;

            qint64 nVirtualSize = S_ALIGN_UP64(nVirtualDelta + listSegments.at(i).p_memsz, nVirtualAlign);
            qint64 nFileSize = S_ALIGN_UP64(nFileDelta + listSegments.at(i).p_filesz, nFileAlign);

            if (nFileOffset + nFileSize > result.nBinarySize) {
                nFileSize = result.nBinarySize - nFileOffset;
            }

            if (nFileSize < 0) {
                nFileSize = 0;
            }

            // Padding
            if (nVirtualDelta) {
                if (nVirtualDelta > nFileDelta) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.filePart = FILEPART_SEGMENT;
                    record.nAddress = nVirtualAddress;
                    record.nSize = nVirtualDelta - nFileDelta;
                    record.nOffset = -1;
                    record.nIndex = nIndex++;
                    record.bIsVirtual = true;
                    record.sName = sName;

                    result.listRecords.append(record);
                }

                if (nVirtualDelta >= nFileDelta) {
                    XBinary::_MEMORY_RECORD record = {};

                    record.filePart = FILEPART_SEGMENT;
                    record.nAddress = nVirtualAddress + nVirtualDelta - nFileDelta;
                    record.nSize = nFileDelta;
                    record.nOffset = nFileOffset;
                    record.nIndex = nIndex++;
                    record.bIsVirtual = false;
                    record.sName = sName;

                    result.listRecords.append(record);
                }
            }

            // Main
            {
                XBinary::_MEMORY_RECORD record = {};

                record.filePart = FILEPART_SEGMENT;
                record.nAddress = listSegments.at(i).p_vaddr;
                record.nSize = listSegments.at(i).p_filesz;
                record.nOffset = listSegments.at(i).p_offset;
                record.nIndex = nIndex++;
                record.bIsVirtual = false;
                record.sName = sName;

                result.listRecords.append(record);
            }

            // padding
            if ((nFileOffset + nFileSize) - (listSegments.at(i).p_offset + listSegments.at(i).p_filesz) > 0) {
                XBinary::_MEMORY_RECORD record = {};

                record.filePart = FILEPART_SEGMENT;
                record.nSize = (nFileOffset + nFileSize) - (listSegments.at(i).p_offset + listSegments.at(i).p_filesz);
                record.nAddress = listSegments.at(i).p_vaddr + listSegments.at(i).p_filesz;

                record.nOffset = listSegments.at(i).p_offset + listSegments.at(i).p_filesz;
                record.nIndex = nIndex++;
                record.bIsVirtual = false;
                record.sName = sName;

                result.listRecords.append(record);
            }

            // padding
            if (nVirtualSize - nFileSize > 0) {
                XBinary::_MEMORY_RECORD record = {};

                record.filePart = FILEPART_SEGMENT;
                record.nSize = nVirtualSize - nFileSize;
                record.nAddress = (nVirtualAddress + nVirtualSize) - record.nSize;

                record.nOffset = -1;
                record.nIndex = nIndex++;
                record.bIsVirtual = true;
                record.sName = sName;

                result.listRecords.append(record);
            }

            nMaxUsedOffset = qMax(nMaxUsedOffset, nFileOffset + nFileSize);
            nMinUsedAddress = qMin(nVirtualAddress, nMinUsedAddress);
            nMaxUsedAddress = qMax(nVirtualAddress + nVirtualSize, nMaxUsedAddress);
        }
    } else if (mapMode == MAPMODE_SECTIONS) {
        bool bIs64 = (result.mode == MODE_64);

        qint32 nStringTableSection = getSectionStringTable(bIs64);
        QByteArray baStringTable = getSection(nStringTableSection);
        QList<XELF_DEF::Elf_Shdr> listSections = getElf_ShdrList(1000);
        QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSections, isImage(), &baStringTable);

        qint32 nNumberOfSections = listSectionRecords.count();

        if (nNumberOfSections) {
            nMinUsedAddress = listSectionRecords.at(0).nAddress;
        }

        for (qint32 i = 0; i < nNumberOfSections; i++) {
            QString sSectionName = listSectionRecords.at(i).sName;

            XBinary::_MEMORY_RECORD record = {};

            record.filePart = FILEPART_REGION;
            record.nSize = listSectionRecords.at(i).nSize;

            if (record.nSize != 0) {
                record.nOffset = listSectionRecords.at(i).nOffset;
            } else {
                record.nOffset = -1;
                record.bIsVirtual = true;
            }

            if (listSectionRecords.at(i).nAddress != 0) {
                record.nAddress = listSectionRecords.at(i).nAddress;
            } else {
                record.nAddress = -1;
            }

            record.nIndex = nIndex++;
            record.sName = sSectionName;

            result.listRecords.append(record);

            nMaxUsedOffset = qMax(nMaxUsedOffset, listSectionRecords.at(i).nOffset + listSectionRecords.at(i).nSize);
            nMinUsedAddress = qMin(listSectionRecords.at(i).nAddress, nMinUsedAddress);
            nMaxUsedAddress = qMax(listSectionRecords.at(i).nAddress + listSectionRecords.at(i).nSize, nMaxUsedAddress);
        }
    }

    result.nImageSize = nMaxUsedAddress - nMinUsedAddress;

    qint64 nFixAddressDelta = getModuleAddress() - nMinUsedAddress;

    if (result.fileType == FT_ELF64) {
        result.nEntryPointAddress = getHdr64_entry() + nFixAddressDelta;
    } else {
        result.nEntryPointAddress = getHdr32_entry() + nFixAddressDelta;
    }

    qint32 _nNumberOfRecords = result.listRecords.count();

    for (qint32 i = 0; i < _nNumberOfRecords; i++) {
        result.listRecords[i].nAddress += nFixAddressDelta;
    }

    qint64 nMaxSectionOffset = nMaxUsedOffset;

    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    qint32 nNumberOfSections = listSectionHeaders.count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        nMaxSectionOffset = qMax(nMaxSectionOffset, (qint64)(listSectionHeaders.at(i).sh_offset + listSectionHeaders.at(i).sh_size));
    }

    qint64 nNoLoadableSize = result.nBinarySize - nMaxUsedOffset;

    if (nNoLoadableSize > 0) {
        XBinary::_MEMORY_RECORD record = {};

        record.filePart = FILEPART_OVERLAY;
        record.sName = tr("Overlay");
        // TODO Section number!
        // TODO virtual sections!
        record.nAddress = -1;
        record.nSize = nNoLoadableSize;
        record.nOffset = nMaxUsedOffset;
        record.nIndex = nIndex++;

        result.listRecords.append(record);
    }

    return result;
}

qint64 XELF::getEntryPointOffset(_MEMORY_MAP *pMemoryMap)
{
    return addressToOffset(pMemoryMap, pMemoryMap->nEntryPointAddress);
}

QList<XELF::SECTION_RECORD> XELF::getSectionRecords(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, bool bIsImage, QByteArray *pbaSectionTable)
{
    QList<SECTION_RECORD> listRecords;

    qint32 nNumberOfSections = pListSectionHeaders->count();
    quint32 nSectionTableSize = pbaSectionTable->size();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        SECTION_RECORD record = {};

        record.nSize = pListSectionHeaders->at(i).sh_size;
        record.nFlags = pListSectionHeaders->at(i).sh_flags;
        record.nAddress = pListSectionHeaders->at(i).sh_addr;

        if (bIsImage) {
            record.nOffset = pListSectionHeaders->at(i).sh_addr;
        } else {
            record.nOffset = pListSectionHeaders->at(i).sh_offset;
        }

        if (pListSectionHeaders->at(i).sh_name < nSectionTableSize) {
            record.sName = pbaSectionTable->data() + pListSectionHeaders->at(i).sh_name;
        }

        listRecords.append(record);
    }

    return listRecords;
}

bool XELF::isSectionNamePresent(const QString &sSectionName)
{
    bool bIs64 = is64();
    quint16 nStringTableSection = getSectionStringTable(bIs64);
    QByteArray baStringTable = getSection(nStringTableSection);
    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

    return isSectionNamePresent(sSectionName, &listSectionRecords);
}

bool XELF::isSectionNamePresent(const QString &sSectionName, QList<XELF::SECTION_RECORD> *pListSectionRecords)
{
    bool bResult = false;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

qint32 XELF::getSectionNumber(const QString &sSectionName)
{
    bool bIs64 = is64();
    quint16 nStringTableSection = getSectionStringTable(bIs64);
    QByteArray baStringTable = getSection(nStringTableSection);
    QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

    QList<SECTION_RECORD> listSectionRecords = getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

    return getSectionNumber(sSectionName, &listSectionRecords);
}

qint32 XELF::getSectionNumber(const QString &sSectionName, QList<XELF::SECTION_RECORD> *pListSectionRecords)
{
    qint32 nResult = -1;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sSectionName) {
            nResult = i;
            break;
        }
    }

    return nResult;
}

XBinary::MODE XELF::getMode()
{
    MODE result = MODE_32;

    quint8 ident = getIdent_class();

    if (ident == XELF_DEF::S_ELFCLASS32) {
        result = MODE_32;
    } else if (ident == XELF_DEF::S_ELFCLASS64) {
        result = MODE_64;
    }

    return result;
}

QString XELF::getArch()
{
    return getMachinesS().value(getHdr32_machine(), tr("Unknown"));
}

qint32 XELF::getType()
{
    qint32 nResult = TYPE_UNKNOWN;

    quint16 nType = 0;

    if (is64()) {
        nType = getHdr64_type();
    } else {
        nType = getHdr32_type();
    }

    if (nType == XELF_DEF::S_ET_REL) {
        nResult = TYPE_REL;
    } else if (nType == XELF_DEF::S_ET_EXEC) {
        nResult = TYPE_EXEC;
    } else if (nType == XELF_DEF::S_ET_DYN) {
        nResult = TYPE_DYN;
    } else if (nType == XELF_DEF::S_ET_CORE) {
        nResult = TYPE_CORE;
    } else if (nType == XELF_DEF::S_ET_NUM) {
        nResult = TYPE_NUM;
    }

    return nResult;
}

XBinary::FT XELF::getFileType()
{
    FT result = FT_ELF32;

    MODE mode = getMode();

    if (mode == MODE_32) {
        result = FT_ELF32;
    } else if (mode == MODE_64) {
        result = FT_ELF64;
    }

    return result;
}

QString XELF::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_REL: sResult = QString("REL"); break;
        case TYPE_EXEC: sResult = QString("EXEC"); break;
        case TYPE_DYN: sResult = QString("DYN"); break;
        case TYPE_CORE: sResult = QString("CORE"); break;
        case TYPE_NUM: sResult = QString("NUM"); break;
    }

    return sResult;
}

XBinary::FILEFORMATINFO XELF::getFileFormatInfo(PDSTRUCT *pPdStruct)
{
    FILEFORMATINFO result = {};

    result.bIsValid = isValid(pPdStruct);

    if (result.bIsValid) {
        result.nSize = getSize();
        result.fileType = getFileType();
        result.sExt = getFileFormatExt();
        result.sVersion = getVersion();
        result.sInfo = getInfo();
        result.sArch = getArch();
        result.mode = getMode();
        result.sType = typeIdToString(getType());
        result.endian = getEndian();
        result.sMIME = getMIMEString();

        result.osName = OSNAME_UNIX;

        quint8 osabi = getIdent_osabi();

        if (osabi == XELF_DEF::S_ELFOSABI_HPUX) result.osName = OSNAME_HPUX;
        else if (osabi == XELF_DEF::S_ELFOSABI_NETBSD) result.osName = OSNAME_NETBSD;
        else if (osabi == XELF_DEF::S_ELFOSABI_LINUX) result.osName = OSNAME_LINUX;
        else if (osabi == XELF_DEF::S_ELFOSABI_SOLARIS) result.osName = OSNAME_SOLARIS;
        else if (osabi == XELF_DEF::S_ELFOSABI_AIX) result.osName = OSNAME_AIX;
        else if (osabi == XELF_DEF::S_ELFOSABI_IRIX) result.osName = OSNAME_IRIX;
        else if (osabi == XELF_DEF::S_ELFOSABI_FREEBSD) result.osName = OSNAME_FREEBSD;
        else if (osabi == XELF_DEF::S_ELFOSABI_TRU64) result.osName = OSNAME_TRU64;
        else if (osabi == XELF_DEF::S_ELFOSABI_MODESTO) result.osName = OSNAME_MODESTO;
        else if (osabi == XELF_DEF::S_ELFOSABI_OPENBSD) result.osName = OSNAME_OPENBSD;
        else if (osabi == XELF_DEF::S_ELFOSABI_OPENVMS) result.osName = OSNAME_OPENVMS;
        else if (osabi == XELF_DEF::S_ELFOSABI_NSK) result.osName = OSNAME_NSK;
        else if (osabi == XELF_DEF::S_ELFOSABI_AROS) result.osName = OSNAME_AROS;
        else if (osabi == XELF_DEF::S_ELFOSABI_FENIXOS) result.osName = OSNAME_FENIXOS;
        else if (osabi == XELF_DEF::S_ELFOSABI_OPENVOS) result.osName = OSNAME_OPENVOS;

        QList<XELF_DEF::Elf_Phdr> listProgramHeaders = getElf_PhdrList(100);
        QList<XELF_DEF::Elf_Shdr> listSectionHeaders = getElf_ShdrList(100);

        qint32 nStringTableSection = getSectionStringTable();
        QByteArray baStringTable = getSection(nStringTableSection);

        QList<SECTION_RECORD> listSectionRecords = XELF::getSectionRecords(&listSectionHeaders, isImage(), &baStringTable);

        QList<QString> listComments = getCommentStrings(&listSectionRecords);

        QList<NOTE> listNotes = getNotes(&listProgramHeaders);

        if (listNotes.count() == 0) {
            listNotes = getNotes(&listSectionHeaders);
        }

        QString sInterpteter = getProgramInterpreterName(&listProgramHeaders).sString;

        if (sInterpteter == "") {
            sInterpteter = getProgramInterpreterName(&listSectionRecords).sString;
        }

        XBinary::_MEMORY_MAP memoryMap = getMemoryMap();
        QList<TAG_STRUCT> listTagStructs = getTagStructs(&listProgramHeaders, &memoryMap);

        QList<QString> listLibraries = getLibraries(&memoryMap, &listTagStructs);

        if (result.osName == OSNAME_UNIX) {
            if (sInterpteter.contains("ld-elf.so")) {
                result.osName = OSNAME_FREEBSD;
            }
        }

        if (result.osName == OSNAME_UNIX) {
            if (sInterpteter.contains("linux")) {
                result.osName = OSNAME_LINUX;
            }
        }

        if (result.osName == OSNAME_UNIX) {
            if (sInterpteter.contains("ldqnx")) {
                result.osName = OSNAME_QNX;
            }
        }

        if (result.osName == OSNAME_UNIX) {
            if (sInterpteter.contains("uClibc")) {
                result.osName = OSNAME_MCLINUX;
            }
        }

        if ((result.osName == OSNAME_UNIX) || (result.osName == OSNAME_LINUX)) {
            qint32 nNumberOfComments = listComments.count();

            for (qint32 i = 0; i < nNumberOfComments; i++) {
                bool bFound = false;

                QString sComment = listComments.at(i);

                if (sComment.contains("Ubuntu") || sComment.contains("ubuntu")) {
                    result.osName = OSNAME_UBUNTULINUX;

                    if (sComment.contains("ubuntu1~")) {
                        result.sOsVersion = sComment.section("ubuntu1~", 1, -1).section(")", 0, 0);
                    }

                    bFound = true;
                } else if (sComment.contains("Debian") || sComment.contains("debian")) {
                    result.osName = OSNAME_DEBIANLINUX;

                    bFound = true;
                } else if (sComment.contains("StartOS")) {
                    result.osName = OSNAME_STARTOSLINUX;

                    bFound = true;
                } else if (sComment.contains("Gentoo")) {
                    result.osName = OSNAME_GENTOOLINUX;

                    bFound = true;
                } else if (sComment.contains("Alpine")) {
                    result.osName = OSNAME_ALPINELINUX;

                    bFound = true;
                } else if (sComment.contains("Wind River Linux")) {
                    result.osName = OSNAME_WINDRIVERLINUX;

                    bFound = true;
                } else if (sComment.contains("SuSE") || sComment.contains("SUSE Linux")) {
                    result.osName = OSNAME_SUSELINUX;

                    bFound = true;
                } else if (sComment.contains("Mandrakelinux") || sComment.contains("Linux-Mandrake") || sComment.contains("Mandrake Linux")) {
                    result.osName = OSNAME_MANDRAKELINUX;

                    bFound = true;
                } else if (sComment.contains("ASPLinux")) {
                    result.osName = OSNAME_ASPLINUX;

                    bFound = true;
                } else if (sComment.contains("Red Hat")) {
                    result.osName = OSNAME_REDHATLINUX;

                    bFound = true;
                } else if (sComment.contains("Hancom Linux")) {
                    result.osName = OSNAME_HANCOMLINUX;

                    bFound = true;
                } else if (sComment.contains("TurboLinux")) {
                    result.osName = OSNAME_TURBOLINUX;

                    bFound = true;
                } else if (sComment.contains("Vine Linux")) {
                    result.osName = OSNAME_VINELINUX;

                    bFound = true;
                }

                if (result.osName != OSNAME_LINUX) {
                    if (sComment.contains("SunOS")) {
                        result.osName = OSNAME_SUNOS;

                        if (sComment.contains("@(#)SunOS ")) {
                            result.sOsVersion = sComment.section("@(#)SunOS ", 1, -1);
                        }

                        bFound = true;
                    }
                }

                if (bFound) {
                    break;
                }
            }
        }

        if (result.osName == OSNAME_FREEBSD) {
            qint32 nNumberOfComments = listComments.count();

            for (qint32 i = 0; i < nNumberOfComments; i++) {
                bool bFound = false;

                QString sComment = listComments.at(i);

                if (sComment.contains("FreeBSD: release/")) {
                    result.sOsVersion = sComment.section("FreeBSD: release/", 1, -1).section("/", 0, 0);
                }

                if (bFound) {
                    break;
                }
            }
        }

        if (result.osName == OSNAME_UNIX) {
            if (isNotePresent(&listNotes, "Android")) {
                result.osName = OSNAME_ANDROID;
                NOTE note = getNote(&listNotes, "Android");

                if (note.nSize >= 4) {
                    quint32 nSDKVersion = read_uint32(note.nDataOffset);
                    result.sOsVersion = XBinary::getAndroidVersionFromApi(nSDKVersion);  // TODO
                }
            } else if ((XBinary::isStringInListPresent(&listLibraries, "liblog.so")) ||
                       ((sInterpteter == "system/bin/linker") || (sInterpteter == "system/bin/linker64"))) {
                result.osName = OSNAME_ANDROID;
            }
        }

        if (isNotePresent(&listNotes, 1, "GNU")) {
            NOTE note = getNote(&listNotes, 1, "GNU");

            quint32 nOS = read_uint32(note.nDataOffset, result.endian == ENDIAN_BIG);
            quint32 nMajor = read_uint32(note.nDataOffset + 4, result.endian == ENDIAN_BIG);
            quint32 nMinor = read_uint32(note.nDataOffset + 8, result.endian == ENDIAN_BIG);
            quint32 nSubMinor = read_uint32(note.nDataOffset + 12, result.endian == ENDIAN_BIG);

            if (result.osName == OSNAME_UNIX) {
                if (nOS == 0) result.osName = OSNAME_LINUX;
                //            else if (nOS==1)  result.osName=OSNAME_GNU;
                else if (nOS == 2) result.osName = OSNAME_SOLARIS;
                else if (nOS == 3) result.osName = OSNAME_FREEBSD;
                else if (nOS == 4) result.osName = OSNAME_NETBSD;
                else if (nOS == 5) result.osName = OSNAME_SYLLABLE;
            }

            QString sABI = QString("ABI: %1.%2.%3").arg(QString::number(nMajor), QString::number(nMinor), QString::number(nSubMinor));

            result.sOsVersion = appendText(result.sOsVersion, sABI, ", ");
        }

        if (result.osName == OSNAME_UNIX) {
            if (isSectionNamePresent(".note.android.ident", &listSectionRecords)) result.osName = OSNAME_ANDROID;
            else if (isSectionNamePresent(".note.minix.ident", &listSectionRecords)) result.osName = OSNAME_MINIX;
            else if (isSectionNamePresent(".note.netbsd.ident", &listSectionRecords)) result.osName = OSNAME_NETBSD;
            else if (isSectionNamePresent(".note.openbsd.ident", &listSectionRecords)) result.osName = OSNAME_OPENBSD;
        }

        if (result.osName == OSNAME_UNIX) {
            result.sOsVersion = QString("%1").arg(osabi);
        }

        result.sArch = getArch();
        result.mode = getMode();
        result.sType = typeIdToString(getType());
        result.endian = getEndian();

        if (result.nSize == 0) {
            result.bIsValid = false;
        }
    }

    return result;
}

QList<XELF_DEF::Elf_Phdr> XELF::_getPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders, quint32 nType)
{
    QList<XELF_DEF::Elf_Phdr> listResult;

    qint32 nNumberOfPrograms = pListProgramHeaders->count();

    for (qint32 i = 0; i < nNumberOfPrograms; i++) {
        if (pListProgramHeaders->at(i).p_type == nType) {
            listResult.append(pListProgramHeaders->at(i));
        }
    }

    return listResult;
}

QList<XELF_DEF::Elf_Shdr> XELF::_getSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders, quint32 nType)
{
    QList<XELF_DEF::Elf_Shdr> listResult;

    qint32 nNumberOfSections = pListSectionHeaders->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionHeaders->at(i).sh_type == nType) {
            listResult.append(pListSectionHeaders->at(i));
        }
    }

    return listResult;
}

QList<XELF::SECTION_RECORD> XELF::_getSectionRecords(QList<SECTION_RECORD> *pListSectionRecords, const QString &sName)
{
    QList<SECTION_RECORD> listResult;

    qint32 nNumberOfSections = pListSectionRecords->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        if (pListSectionRecords->at(i).sName == sName) {
            listResult.append(pListSectionRecords->at(i));
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromSections(QList<XELF_DEF::Elf_Shdr> *pListSectionHeaders)
{
    QList<XBinary::DATASET> listResult;

    quint32 nMainStringSection = getSectionStringTable();

    XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nMainStringSection);

    qint32 nNumberOfSections = pListSectionHeaders->count();

    for (qint32 i = 0; i < nNumberOfSections; i++) {
        QString sSectionName = getStringFromIndex(osStringTable.nOffset, osStringTable.nSize, pListSectionHeaders->at(i).sh_name);

        if ((pListSectionHeaders->at(i).sh_type == 1) && (sSectionName == ".interp"))  // Interpreter TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_INTERPRETER;
            dataset.sName = QString("%1[%2]").arg("Interpreter", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 2)  // Symbol table TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_SYMBOLTABLE;
            dataset.sName = QString("%1[%2]").arg("Symbol table", sSectionName);  // TODO mb translate

            qint32 nSectionIndex = getSectionIndexByName(".strtab");

            if (nSectionIndex != -1) {
                XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nSectionIndex);

                dataset.nStringTableOffset = osStringTable.nOffset;
                dataset.nStringTableSize = osStringTable.nSize;
            }

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 0xb)  // Symbol table dynamic TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_SYMBOLTABLE;
            dataset.sName = QString("%1[%2]").arg("Symbol table", sSectionName);  // TODO mb translate

            qint32 nSectionIndex = getSectionIndexByName(".dynstr");

            if (nSectionIndex != -1) {
                XBinary::OFFSETSIZE osStringTable = getSectionOffsetSize(nSectionIndex);

                dataset.nStringTableOffset = osStringTable.nOffset;
                dataset.nStringTableSize = osStringTable.nSize;
            }

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 3)  // String table TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_STRINGTABLE;
            dataset.sName = QString("%1[%2]").arg("String table", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 4)  // RELA TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_RELA;
            dataset.sName = QString("%1[%2]").arg("RELA", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 6)  // Dynamic TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_DYNAMICTAGS;
            dataset.sName = QString("%1[%2]").arg("Dynamic tags", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 7)  // Notes TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_NOTES;
            dataset.sName = QString("%1[%2]").arg("Notes", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        } else if (pListSectionHeaders->at(i).sh_type == 9)  // REL TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListSectionHeaders->at(i).sh_addr;
            dataset.nOffset = pListSectionHeaders->at(i).sh_offset;
            dataset.nSize = pListSectionHeaders->at(i).sh_size;
            dataset.nType = DS_REL;
            dataset.sName = QString("%1[%2]").arg("REL", sSectionName);  // TODO mb translate

            listResult.append(dataset);
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromPrograms(QList<XELF_DEF::Elf_Phdr> *pListProgramHeaders)
{
    QList<XBinary::DATASET> listResult;

    qint32 nNumberOfPrograms = pListProgramHeaders->count();

    for (qint32 i = 0; i < nNumberOfPrograms; i++) {
        if ((pListProgramHeaders->at(i).p_type == 3))  // Interpreter TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_INTERPRETER;
            dataset.sName = QString("%1").arg("Interpreter");  // TODO mb translate

            listResult.append(dataset);
        } else if ((pListProgramHeaders->at(i).p_type == 4))  // Notes TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_NOTES;
            dataset.sName = QString("%1").arg("Notes");  // TODO mb translate

            listResult.append(dataset);
        } else if ((pListProgramHeaders->at(i).p_type == 2))  // Tags TODO const
        {
            DATASET dataset = {};

            dataset.nAddress = pListProgramHeaders->at(i).p_vaddr;
            dataset.nOffset = pListProgramHeaders->at(i).p_offset;
            dataset.nSize = pListProgramHeaders->at(i).p_filesz;
            dataset.nType = DS_DYNAMICTAGS;
            dataset.sName = QString("%1").arg("Dynamic tags");  // TODO mb translate

            listResult.append(dataset);
        }
    }

    return listResult;
}

QList<XBinary::DATASET> XELF::getDatasetsFromTagStructs(XBinary::_MEMORY_MAP *pMemoryMap, QList<XELF::TAG_STRUCT> *pListTagStructs)
{
    QList<XBinary::DATASET> listResult;

    QList<TAG_STRUCT> listStrTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRTAB);
    QList<TAG_STRUCT> listStrSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_STRSZ);
    QList<TAG_STRUCT> listStrNeeded = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_NEEDED);
    QList<TAG_STRUCT> listRunPath = _getTagStructs(pListTagStructs, 0x1d);  // TODO const
    QList<TAG_STRUCT> listSymbols = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_SYMTAB);
    QList<TAG_STRUCT> listRelaTab = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RELA);
    QList<TAG_STRUCT> listRelaSize = _getTagStructs(pListTagStructs, XELF_DEF::S_DT_RELASZ);
    QList<TAG_STRUCT> listRelTab = _getTagStructs(pListTagStructs, 17);   // TODO const
    QList<TAG_STRUCT> listRelSize = _getTagStructs(pListTagStructs, 18);  // TODO const

    qint64 nStringTableOffset = 0;
    qint64 nStringTableSize = 0;

    if (listStrTab.count() && listStrSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listStrTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listStrSize.at(0).nValue;
        dataset.nType = DS_STRINGTABLE;
        dataset.sName = tr("String table");

        if (isOffsetAndSizeValid(pMemoryMap, dataset.nOffset, dataset.nSize)) {
            listResult.append(dataset);
        }

        nStringTableOffset = dataset.nOffset;
        nStringTableSize = dataset.nSize;
    }

    if (listRelaTab.count() && listRelaSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listRelaTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listRelaSize.at(0).nValue;
        dataset.nType = DS_RELA;
        dataset.sName = "RELA";
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listRelTab.count() && listRelSize.count()) {
        DATASET dataset = {};

        dataset.nAddress = listRelTab.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = listRelSize.at(0).nValue;
        dataset.nType = DS_REL;
        dataset.sName = "REL";
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listStrNeeded.count()) {
        DATASET dataset = {};

        dataset.nAddress = 0;
        dataset.nOffset = 0;
        dataset.nSize = 0;
        dataset.nType = DS_LIBRARIES;
        dataset.sName = tr("Libraries");
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    if (listRunPath.count()) {
        XADDR nAddress = listStrTab.at(0).nValue;
        qint64 nOffset = addressToOffset(pMemoryMap, nAddress);
        qint64 nRunPath = listRunPath.at(0).nValue;
        qint64 nSize = listStrSize.at(0).nValue;

        if (nRunPath < nSize) {
            DATASET dataset = {};

            dataset.nAddress = nAddress + nRunPath;
            dataset.nOffset = nOffset + nRunPath;
            QString sAnsiString = read_ansiString(dataset.nOffset);
            dataset.nSize = sAnsiString.length();
            dataset.nType = DS_RUNPATH;
            dataset.sName = tr("Run path");
            dataset.nStringTableOffset = nStringTableOffset;
            dataset.nStringTableSize = nStringTableSize;

            listResult.append(dataset);
        }
    }

    if (listSymbols.count()) {
        DATASET dataset = {};

        dataset.nAddress = listSymbols.at(0).nValue;
        dataset.nOffset = addressToOffset(pMemoryMap, dataset.nAddress);
        dataset.nSize = getSymTableSize(dataset.nOffset);
        dataset.nType = DS_SYMBOLTABLE;
        dataset.sName = tr("Symbol table");
        dataset.nStringTableOffset = nStringTableOffset;
        dataset.nStringTableSize = nStringTableSize;

        listResult.append(dataset);
    }

    return listResult;
}

QList<XELF_DEF::Elf32_Sym> XELF::getElf32_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Sym> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Sym record = _readElf32_Sym(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Sym);
        nSize -= sizeof(XELF_DEF::Elf32_Sym);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Sym> XELF::getElf64_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Sym> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Sym record = _readElf64_Sym(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Sym);
        nSize -= sizeof(XELF_DEF::Elf64_Sym);
    }

    return listResult;
}

QList<XELF_DEF::Elf_Sym> XELF::getElf_SymList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf_Sym> listResult;

    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    while (nSize > 0) {
        XELF_DEF::Elf_Sym record = {};

        if (bIs64) {
            XELF_DEF::Elf64_Sym _record = _readElf64_Sym(nOffset, bIsBigEndian);

            record.st_name = _record.st_name;
            record.st_info = _record.st_info;
            record.st_other = _record.st_other;
            record.st_shndx = _record.st_shndx;
            record.st_value = _record.st_value;
            record.st_size = _record.st_size;

            nOffset += sizeof(XELF_DEF::Elf64_Sym);
            nSize -= sizeof(XELF_DEF::Elf64_Sym);
        } else {
            XELF_DEF::Elf32_Sym _record = _readElf32_Sym(nOffset, bIsBigEndian);

            record.st_name = _record.st_name;
            record.st_info = _record.st_info;
            record.st_other = _record.st_other;
            record.st_shndx = _record.st_shndx;
            record.st_value = _record.st_value;
            record.st_size = _record.st_size;

            nOffset += sizeof(XELF_DEF::Elf32_Sym);
            nSize -= sizeof(XELF_DEF::Elf32_Sym);
        }

        listResult.append(record);
    }

    return listResult;
}

XELF_DEF::Elf32_Sym XELF::_readElf32_Sym(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Sym result = {};

    result.st_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_name), bIsBigEndian);
    result.st_value = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_value), bIsBigEndian);
    result.st_size = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_size), bIsBigEndian);
    result.st_info = read_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_info));
    result.st_other = read_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_other));
    result.st_shndx = read_uint16(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_shndx), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Sym XELF::_readElf64_Sym(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Sym result = {};

    result.st_name = read_uint32(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_name), bIsBigEndian);
    result.st_info = read_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_info));
    result.st_other = read_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_other));
    result.st_shndx = read_uint16(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_shndx), bIsBigEndian);
    result.st_value = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_value), bIsBigEndian);
    result.st_size = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_size), bIsBigEndian);

    return result;
}

void XELF::setElf32_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_name), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_value(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_value), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_size(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_size), nValue, bIsBigEndian);
}

void XELF::setElf32_Sym_st_info(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_info), nValue);
}

void XELF::setElf32_Sym_st_other(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_other), nValue);
}

void XELF::setElf32_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian)
{
    write_uint16(nOffset + offsetof(XELF_DEF::Elf32_Sym, st_shndx), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_name(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_name), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_info(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_info), nValue);
}

void XELF::setElf64_Sym_st_other(qint64 nOffset, quint8 nValue)
{
    write_uint8(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_other), nValue);
}

void XELF::setElf64_Sym_st_shndx(qint64 nOffset, quint16 nValue, bool bIsBigEndian)
{
    write_uint16(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_shndx), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_value(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_value), nValue, bIsBigEndian);
}

void XELF::setElf64_Sym_st_size(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Sym, st_size), nValue, bIsBigEndian);
}

qint64 XELF::getSymSize()
{
    quint64 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = sizeof(XELF_DEF::Elf64_Sym);
    } else {
        nResult = sizeof(XELF_DEF::Elf32_Sym);
    }

    return nResult;
}

qint64 XELF::getSymTableSize(qint64 nOffset)
{
    qint64 nResult = 0;

    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    if (bIs64) {
        nResult += sizeof(XELF_DEF::Elf64_Sym);
        nOffset += sizeof(XELF_DEF::Elf64_Sym);

        while (true) {
            XELF_DEF::Elf64_Sym record = _readElf64_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult += sizeof(XELF_DEF::Elf64_Sym);
            nOffset += sizeof(XELF_DEF::Elf64_Sym);
        }
    } else {
        nResult += sizeof(XELF_DEF::Elf32_Sym);
        nOffset += sizeof(XELF_DEF::Elf32_Sym);

        while (true) {
            XELF_DEF::Elf32_Sym record = _readElf32_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult += sizeof(XELF_DEF::Elf32_Sym);
            nOffset += sizeof(XELF_DEF::Elf32_Sym);
        }
    }

    return nResult;
}

qint32 XELF::getNumberOfSymbols(qint64 nOffset, PDSTRUCT *pPdStruct)
{
    bool bIsBigEndian = isBigEndian();
    bool bIs64 = is64();

    return getNumberOfSymbols(nOffset, bIsBigEndian, bIs64, pPdStruct);
}

qint32 XELF::getNumberOfSymbols(qint64 nOffset, bool bIsBigEndian, bool bIs64, PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    if (bIs64) {
        nResult += sizeof(XELF_DEF::Elf64_Sym);
        nOffset += sizeof(XELF_DEF::Elf64_Sym);

        while (XBinary::isPdStructNotCanceled(pPdStruct)) {
            XELF_DEF::Elf64_Sym record = _readElf64_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult++;
            nOffset += sizeof(XELF_DEF::Elf64_Sym);
        }
    } else {
        nResult += sizeof(XELF_DEF::Elf32_Sym);
        nOffset += sizeof(XELF_DEF::Elf32_Sym);

        while (XBinary::isPdStructNotCanceled(pPdStruct)) {
            XELF_DEF::Elf32_Sym record = _readElf32_Sym(nOffset, bIsBigEndian);

            if ((!record.st_info) || (record.st_other)) {
                break;
            }

            nResult++;
            nOffset += sizeof(XELF_DEF::Elf32_Sym);
        }
    }

    return nResult;
}

XELF_DEF::Elf32_Rel XELF::_readElf32_Rel(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Rel result = {};

    result.r_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_offset), bIsBigEndian);
    result.r_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_info), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Rel XELF::_readElf64_Rel(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Rel result = {};

    result.r_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_offset), bIsBigEndian);
    result.r_info = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_info), bIsBigEndian);

    return result;
}

XELF_DEF::Elf32_Rela XELF::_readElf32_Rela(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf32_Rela result = {};

    result.r_offset = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_offset), bIsBigEndian);
    result.r_info = read_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_info), bIsBigEndian);
    result.r_addend = read_int32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_addend), bIsBigEndian);

    return result;
}

XELF_DEF::Elf64_Rela XELF::_readElf64_Rela(qint64 nOffset, bool bIsBigEndian)
{
    XELF_DEF::Elf64_Rela result = {};

    result.r_offset = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_offset), bIsBigEndian);
    result.r_info = read_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_info), bIsBigEndian);
    result.r_addend = read_int64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_addend), bIsBigEndian);

    return result;
}

QList<XELF_DEF::Elf32_Rel> XELF::getElf32_RelList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Rel> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Rel record = _readElf32_Rel(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Rel);
        nSize -= sizeof(XELF_DEF::Elf32_Rel);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Rel> XELF::getElf64_RelList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Rel> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Rel record = _readElf64_Rel(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Rel);
        nSize -= sizeof(XELF_DEF::Elf64_Rel);
    }

    return listResult;
}

QList<XELF_DEF::Elf32_Rela> XELF::getElf32_RelaList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf32_Rela> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf32_Rela record = _readElf32_Rela(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf32_Rela);
        nSize -= sizeof(XELF_DEF::Elf32_Rela);
    }

    return listResult;
}

QList<XELF_DEF::Elf64_Rela> XELF::getElf64_RelaList(qint64 nOffset, qint64 nSize)
{
    QList<XELF_DEF::Elf64_Rela> listResult;

    bool bIsBigEndian = isBigEndian();

    while (nSize > 0) {
        XELF_DEF::Elf64_Rela record = _readElf64_Rela(nOffset, bIsBigEndian);

        listResult.append(record);

        nOffset += sizeof(XELF_DEF::Elf64_Rela);
        nSize -= sizeof(XELF_DEF::Elf64_Rela);
    }

    return listResult;
}

void XELF::setElf32_Rel_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf32_Rel_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rel, r_info), nValue, bIsBigEndian);
}

void XELF::setElf64_Rel_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf64_Rel_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rel, r_info), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_offset(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_info(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_info), nValue, bIsBigEndian);
}

void XELF::setElf32_Rela_r_addend(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    write_uint32(nOffset + offsetof(XELF_DEF::Elf32_Rela, r_addend), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_offset(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_offset), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_info(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_info), nValue, bIsBigEndian);
}

void XELF::setElf64_Rela_r_addend(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    write_uint64(nOffset + offsetof(XELF_DEF::Elf64_Rela, r_addend), nValue, bIsBigEndian);
}

quint32 XELF::getNumberOfSections()
{
    quint32 nResult = 0;

    bool bIs64 = is64();

    if (bIs64) {
        nResult = getHdr64_shnum();
    } else {
        nResult = getHdr32_shnum();
    }

    if (nResult == 0) {
        quint64 nOffset = 0;

        if (bIs64) {
            nOffset = getHdr64_shoff();
        } else {
            nOffset = getHdr32_shoff();
        }

        if (nOffset) {
            bool bIsBigEndian = isBigEndian();

            if (bIs64) {
                XELF_DEF::Elf64_Shdr record = _readElf64_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_size;
            } else {
                XELF_DEF::Elf32_Shdr record = _readElf32_Shdr(nOffset, bIsBigEndian);
                nResult = record.sh_size;
            }
        }
    }

    return nResult;
}

quint16 XELF::getNumberOfPrograms()
{
    quint16 nResult = 0;

    if (is64()) {
        nResult = getHdr64_phnum();
    } else {
        nResult = getHdr32_phnum();
    }

    return nResult;
}

bool XELF::isSectionsTablePresent()
{
    return (bool)getNumberOfSections();
}

bool XELF::isProgramsTablePresent()
{
    return (bool)getNumberOfPrograms();
}

QString XELF::getFileFormatExt()
{
    QString sResult;

    TYPE _type = (TYPE)getType();

    if (_type == TYPE_DYN) {
        sResult = "so";
    } else {
        sResult = "elf";
    }

    return sResult;
}

QString XELF::getFileFormatExtsString()
{
    return QString("ELF(elf, so)");
}

qint64 XELF::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    {
        _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_SEGMENTS, pPdStruct);

        nResult = _calculateRawSize(&memoryMap, pPdStruct);
    }

    if (nResult == 0) {
        _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_SECTIONS, pPdStruct);

        nResult = _calculateRawSize(&memoryMap, pPdStruct);
    }

    return nResult;
}

QString XELF::getMIMEString()
{
    return "application/x-elf";
}

bool XELF::fixDump(const QString &sResultFile, const FIXDUMP_OPTIONS &fixDumpOptions, PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    QFile file;
    file.setFileName(sResultFile);

    if (fixDumpOptions.bOptimizeSize) {
        if (file.open(QIODevice::ReadWrite)) {
            QByteArray baHeaders = getHeaders();

            file.write(baHeaders);

            // TODO

            file.close();
        }
    } else {
        QString sSrcFile = getDeviceFileName(getDevice());
        XBinary::copyFile(sSrcFile, sResultFile);
    }

    if (fixDumpOptions.bSetEntryPoint) {
        if (file.open(QIODevice::ReadWrite)) {
            XELF elf(&file);

            if (elf.isValid(pPdStruct)) {
                if ((fixDumpOptions.bFixSegments) && (!(fixDumpOptions.bOptimizeSize))) {
                    if (elf.is64()) {
                        qint32 nNumberOfSegments = getHdr64_phnum();
                        for (qint32 i = 0; i < nNumberOfSegments; i++) {
                            // TODO
                        }
                    } else {
                        qint32 nNumberOfSegments = getHdr32_phnum();
                        for (qint32 i = 0; i < nNumberOfSegments; i++) {
                            // TODO
                        }
                    }
                }

                if (fixDumpOptions.bSetEntryPoint) {
                    if (elf.is64()) {
                        elf.setHdr64_entry((quint64)fixDumpOptions.nEntryPoint);
                    } else {
                        elf.setHdr32_entry((quint32)fixDumpOptions.nEntryPoint);
                    }
                }
            }

            file.close();
        }
    }

    return bResult;
}

XELF::FIXDUMP_OPTIONS XELF::getFixDumpOptions(PDSTRUCT *pPdStruct)
{
    XELF::FIXDUMP_OPTIONS result = {};

    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    result.bOptimizeSize = true;
    result.bFixSegments = true;
    result.bSetEntryPoint = false;
    result.nEntryPoint = getEntryPointAddress(&memoryMap);

    return result;
}

QByteArray XELF::getHeaders()
{
    qint64 nSize = 0;

    if (is64()) {
        nSize = getHdr64_phoff() + getHdr64_phentsize() * getHdr64_phnum();
    } else {
        nSize = getHdr32_phoff() + getHdr32_phentsize() * getHdr32_phnum();
    }

    return read_array(0, nSize);
}

QMap<quint64, QString> XELF::getRelTypes_x86()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_RelTypes_x86, sizeof(_TABLE_XELF_RelTypes_x86) / sizeof(XBinary::XIDSTRING), PREFIX_R);
}

QMap<quint64, QString> XELF::getRelTypesS_x86()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_RelTypes_x86, sizeof(_TABLE_XELF_RelTypes_x86) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getRelTypes_x64()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_RelTypes_x64, sizeof(_TABLE_XELF_RelTypes_x64) / sizeof(XBinary::XIDSTRING), PREFIX_R);
}

QMap<quint64, QString> XELF::getRelTypesS_x64()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_RelTypes_x64, sizeof(_TABLE_XELF_RelTypes_x64) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getRelTypes_SPARC()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_RelTypes_SPARC, sizeof(_TABLE_XELF_RelTypes_SPARC) / sizeof(XBinary::XIDSTRING), PREFIX_R);
}

QMap<quint64, QString> XELF::getRelTypesS_SPARC()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_RelTypes_SPARC, sizeof(_TABLE_XELF_RelTypes_SPARC) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getStBinds()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_StBinds, sizeof(_TABLE_XELF_StBinds) / sizeof(XBinary::XIDSTRING), PREFIX_STB);
}

QMap<quint64, QString> XELF::getStBindsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_StBinds, sizeof(_TABLE_XELF_StBinds) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XELF::getStTypes()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XELF_StTypes, sizeof(_TABLE_XELF_StTypes) / sizeof(XBinary::XIDSTRING), PREFIX_STT);
}

QMap<quint64, QString> XELF::getStTypesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XELF_StTypes, sizeof(_TABLE_XELF_StTypes) / sizeof(XBinary::XIDSTRING));
}

QList<QString> XELF::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("7F'ELF'");

    return listResult;
}

XBinary *XELF::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XELF(pDevice);
}
