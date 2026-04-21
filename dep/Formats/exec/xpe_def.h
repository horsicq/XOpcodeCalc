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
#ifndef XPE_DEF_H
#define XPE_DEF_H

#include "xbinary_def.h"

namespace XPE_DEF {
const quint32 S_IMAGE_NT_SIGNATURE = 0x00004550;  // PE00
const quint32 S_IMAGE_NUMBEROF_DIRECTORY_ENTRIES = 16;

const quint32 S_IMAGE_DIRECTORY_ENTRY_EXPORT = 0;     // Export Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_IMPORT = 1;     // Import Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_RESOURCE = 2;   // Resource Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_EXCEPTION = 3;  // Exception Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_SECURITY = 4;   // Security Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_BASERELOC = 5;  // Base Relocation Table
const quint32 S_IMAGE_DIRECTORY_ENTRY_DEBUG = 6;      // Debug Directory
//      XPE_DEF::IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
const quint32 S_IMAGE_DIRECTORY_ENTRY_ARCHITECTURE = 7;     // Architecture Specific Data
const quint32 S_IMAGE_DIRECTORY_ENTRY_GLOBALPTR = 8;        // RVA of GP
const quint32 S_IMAGE_DIRECTORY_ENTRY_TLS = 9;              // TLS Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG = 10;     // Load Configuration Directory
const quint32 S_IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT = 11;    // Bound Import Directory in headers
const quint32 S_IMAGE_DIRECTORY_ENTRY_IAT = 12;             // Import Address Table
const quint32 S_IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT = 13;    // Delay Load Import Descriptors
const quint32 S_IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR = 14;  // COM Runtime descriptor

const quint32 S_IMAGE_SIZEOF_FILE_HEADER = 20;

const quint32 S_IMAGE_FILE_RELOCS_STRIPPED = 0x0001;          // Relocation info stripped from file.
const quint32 S_IMAGE_FILE_EXECUTABLE_IMAGE = 0x0002;         // File is executable  (i.e. no unresolved externel references).
const quint32 S_IMAGE_FILE_LINE_NUMS_STRIPPED = 0x0004;       // Line nunbers stripped from file.
const quint32 S_IMAGE_FILE_LOCAL_SYMS_STRIPPED = 0x0008;      // Local symbols stripped from file.
const quint32 S_IMAGE_FILE_AGGRESIVE_WS_TRIM = 0x0010;        // Agressively trim working set
const quint32 S_IMAGE_FILE_LARGE_ADDRESS_AWARE = 0x0020;      // App can handle >2gb addresses
const quint32 S_IMAGE_FILE_BYTES_REVERSED_LO = 0x0080;        // Bytes of machine quint16 are reversed.
const quint32 S_IMAGE_FILE_32BIT_MACHINE = 0x0100;            // 32 bit quint16 machine.
const quint32 S_IMAGE_FILE_DEBUG_STRIPPED = 0x0200;           // Debugging info stripped from file in .DBG file
const quint32 S_IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP = 0x0400;  // If Image is on removable media, copy and run from the swap file.
const quint32 S_IMAGE_FILE_NET_RUN_FROM_SWAP = 0x0800;        // If Image is on Net, copy and run from the swap file.
const quint32 S_IMAGE_FILE_SYSTEM = 0x1000;                   // System File.
const quint32 S_IMAGE_FILE_DLL = 0x2000;                      // File is a DLL.
const quint32 S_IMAGE_FILE_UP_SYSTEM_ONLY = 0x4000;           // File should only be run on a UP machine
const quint32 S_IMAGE_FILE_BYTES_REVERSED_HI = 0x8000;        // Bytes of machine quint16 are reversed.

const quint32 S_IMAGE_FILE_MACHINE_UNKNOWN = 0;
const quint32 S_IMAGE_FILE_MACHINE_I386 = 0x014C;  // Intel 386.
const quint32 S_IMAGE_FILE_MACHINE_R3000_BE = 0x0160;
const quint32 S_IMAGE_FILE_MACHINE_R3000 = 0x0162;      // MIPS little-endian, 0x160 big-endian
const quint32 S_IMAGE_FILE_MACHINE_R4000 = 0x0166;      // MIPS little-endian
const quint32 S_IMAGE_FILE_MACHINE_R10000 = 0x0168;     // MIPS little-endian
const quint32 S_IMAGE_FILE_MACHINE_WCEMIPSV2 = 0x0169;  // MIPS little-endian WCE v2
const quint32 S_IMAGE_FILE_MACHINE_ALPHA = 0x0184;      // Alpha_AXP
const quint32 S_IMAGE_FILE_MACHINE_SH3 = 0x01A2;        // SH3 little-endian
const quint32 S_IMAGE_FILE_MACHINE_SH3DSP = 0x01A3;
const quint32 S_IMAGE_FILE_MACHINE_SH3E = 0x01A4;  // SH3E little-endian
const quint32 S_IMAGE_FILE_MACHINE_SH4 = 0x01A6;   // SH4 little-endian
const quint32 S_IMAGE_FILE_MACHINE_SH5 = 0x01A8;   // SH5
const quint32 S_IMAGE_FILE_MACHINE_ARM = 0x01C0;   // ARM Little-Endian
const quint32 S_IMAGE_FILE_MACHINE_THUMB = 0x01C2;
const quint32 S_IMAGE_FILE_MACHINE_AM33 = 0x01D3;
const quint32 S_IMAGE_FILE_MACHINE_POWERPC = 0x01F0;  // IBM PowerPC Little-Endian
const quint32 S_IMAGE_FILE_MACHINE_POWERPCFP = 0x01F1;
const quint32 S_IMAGE_FILE_MACHINE_POWERPCBE = 0x01F2;
const quint32 S_IMAGE_FILE_MACHINE_IA64 = 0x0200;       // Intel 64
const quint32 S_IMAGE_FILE_MACHINE_MIPS16 = 0x0266;     // MIPS
const quint32 S_IMAGE_FILE_MACHINE_ALPHA64 = 0x0284;    // ALPHA64
const quint32 S_IMAGE_FILE_MACHINE_MIPSFPU = 0x0366;    // MIPS
const quint32 S_IMAGE_FILE_MACHINE_MIPSFPU16 = 0x0466;  // MIPS
const quint32 S_IMAGE_FILE_MACHINE_AXP64 = S_IMAGE_FILE_MACHINE_ALPHA64;
const quint32 S_IMAGE_FILE_MACHINE_TRICORE = 0x0520;  // Infineon
const quint32 S_IMAGE_FILE_MACHINE_CEF = 0x0CEF;
const quint32 S_IMAGE_FILE_MACHINE_EBC = 0x0EBC;    // EFI Byte Code
const quint32 S_IMAGE_FILE_MACHINE_AMD64 = 0x8664;  // AMD64 (K8)
const quint32 S_IMAGE_FILE_MACHINE_M32R = 0x9041;   // M32R little-endian
const quint32 S_IMAGE_FILE_MACHINE_CEE = 0xC0EE;
const quint32 S_IMAGE_FILE_MACHINE_ARM64 = 0xAA64;
const quint32 S_IMAGE_FILE_MACHINE_RISCV32 = 0x5032;      // RISC-V 32bit
const quint32 S_IMAGE_FILE_MACHINE_RISCV64 = 0x5064;      // RISC-V 64bit
const quint32 S_IMAGE_FILE_MACHINE_RISCV128 = 0x5128;     // RISC-V 128bit
const quint32 S_IMAGE_FILE_MACHINE_CHPE_X86 = 0x3A64;     // Intel x86
const quint32 S_IMAGE_FILE_MACHINE_LOONGARCH32 = 0x6232;  // Loongarch 32bit
const quint32 S_IMAGE_FILE_MACHINE_LOONGARCH64 = 0x6264;  // Loongarch 64bit

// # For .NET Core
const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_APPLE = 0x4644;
const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_FREEBSD = 0xadc4;
const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_LINUX = 0x7b79;
const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_NETBSD = 0x1993;
// const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_OPENBSD = 0x4c1e;
// const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_SOLARIS = 0x2f6a;
// const quint32 S_IMAGE_FILE_MACHINE_NATIVE_OS_OVERRIDE_WINDOWS = 0x4c;

const quint32 S_IMAGE_SUBSYSTEM_UNKNOWN = 0;
const quint32 S_IMAGE_SUBSYSTEM_NATIVE = 1;
const quint32 S_IMAGE_SUBSYSTEM_WINDOWS_GUI = 2;
const quint32 S_IMAGE_SUBSYSTEM_WINDOWS_CUI = 3;
const quint32 S_IMAGE_SUBSYSTEM_OS2_CUI = 5;
const quint32 S_IMAGE_SUBSYSTEM_POSIX_CUI = 7;
const quint32 S_IMAGE_SUBSYSTEM_NATIVE_WINDOWS = 8;
const quint32 S_IMAGE_SUBSYSTEM_WINDOWS_CE_GUI = 9;
const quint32 S_IMAGE_SUBSYSTEM_EFI_APPLICATION = 10;
const quint32 S_IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER = 11;
const quint32 S_IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER = 12;
const quint32 S_IMAGE_SUBSYSTEM_EFI_ROM = 13;
const quint32 S_IMAGE_SUBSYSTEM_XBOX = 14;
const quint32 S_IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION = 16;
const quint32 S_IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG = 17;

const qint32 S_IMAGE_SIZEOF_SHORT_NAME = 8;
const qint32 S_MAX_SECTIONCOUNT = 0x60;

const quint32 S_IMAGE_SCN_MEM_EXECUTE = 0x20000000;
const quint32 S_IMAGE_SCN_MEM_READ = 0x40000000;
const quint32 S_IMAGE_SCN_MEM_WRITE = 0x80000000;
const quint32 S_IMAGE_SCN_ALIGN_MASK = 0x00F00000;

const quint32 S_IMAGE_NT_OPTIONAL_HDR32_MAGIC = 0x10b;  // 32 bit"
const quint32 S_IMAGE_NT_OPTIONAL_HDR64_MAGIC = 0x20b;  // 64 bit"
const quint32 S_IMAGE_ROM_OPTIONAL_HDR_MAGIC = 0x107;   // ROM image"

/* Predefined resource types */
const quint32 S_RT_NEWRESOURCE = 0x2000;
const quint32 S_RT_ERROR = 0x7FFF;
const quint32 S_RT_CURSOR = 1;
const quint32 S_RT_BITMAP = 2;
const quint32 S_RT_ICON = 3;
const quint32 S_RT_MENU = 4;
const quint32 S_RT_DIALOG = 5;
const quint32 S_RT_STRING = 6;
const quint32 S_RT_FONTDIR = 7;
const quint32 S_RT_FONT = 8;
const quint32 S_RT_ACCELERATORS = 9;
const quint32 S_RT_RCDATA = 10;
const quint32 S_RT_MESSAGETABLE = 11;
const quint32 S_RT_GROUP_CURSOR = 12;
const quint32 S_RT_GROUP_ICON = 14;
const quint32 S_RT_NAMETABLE = 15;
const quint32 S_RT_VERSION = 16;
const quint32 S_RT_DLGINCLUDE = 17;
const quint32 S_RT_NEWBITMAP = (S_RT_BITMAP | S_RT_NEWRESOURCE);
const quint32 S_RT_NEWMENU = (S_RT_MENU | S_RT_NEWRESOURCE);
const quint32 S_RT_NEWDIALOG = (S_RT_DIALOG | S_RT_NEWRESOURCE);
const quint32 S_RT_HTML = 23;
const quint32 S_RT_MANIFEST = 24;

const quint32 S_IMAGE_DEBUG_TYPE_UNKNOWN = 0;
const quint32 S_IMAGE_DEBUG_TYPE_COFF = 1;
const quint32 S_IMAGE_DEBUG_TYPE_CODEVIEW = 2;
const quint32 S_IMAGE_DEBUG_TYPE_FPO = 3;
const quint32 S_IMAGE_DEBUG_TYPE_MISC = 4;
const quint32 S_IMAGE_DEBUG_TYPE_EXCEPTION = 5;
const quint32 S_IMAGE_DEBUG_TYPE_FIXUP = 6;
const quint32 S_IMAGE_DEBUG_TYPE_OMAP_TO_SRC = 7;
const quint32 S_IMAGE_DEBUG_TYPE_OMAP_FROM_SRC = 8;
const quint32 S_IMAGE_DEBUG_TYPE_BORLAND = 9;
const quint32 S_IMAGE_DEBUG_TYPE_RESERVED10 = 10;
const quint32 S_IMAGE_DEBUG_TYPE_CLSID = 11;
const quint32 S_IMAGE_DEBUG_TYPE_VC_FEATURE = 12;
const quint32 S_IMAGE_DEBUG_TYPE_POGO = 13;
const quint32 S_IMAGE_DEBUG_TYPE_ILTCG = 14;
const quint32 S_IMAGE_DEBUG_TYPE_MPX = 15;
const quint32 S_IMAGE_DEBUG_TYPE_NO_TIMESTAMP = 16;
const quint32 S_IMAGE_DEBUG_TYPE_EX_DLLCHARACTERISTICS = 20;

const quint32 S_ASN1_BOOLEAN = 0x01;
const quint32 S_ASN1_INTEGER = 0x02;
const quint32 S_ASN1_BIT_STRING = 0x03;
const quint32 S_ASN1_OCTET_STRING = 0x04;
const quint32 S_ASN1_NULL = 0x05;
const quint32 S_ASN1_OBJECT_ID = 0x06;
const quint32 S_ASN1_OBJECT_DESCRIPTOR = 0x07;
const quint32 S_ASN1_REAL = 0x09;
const quint32 S_ASN1_ENUMERATED = 0x0A;
const quint32 S_ASN1_EMBEDDED_PDV = 0x0B;
const quint32 S_ASN1_UTF8_STRING = 0x0C;
const quint32 S_ASN1_SEQUENCE = 0x10;
const quint32 S_ASN1_SET = 0x11;
const quint32 S_ASN1_NUMERIC_STRING = 0x12;
const quint32 S_ASN1_PRINTABLE_STRING = 0x13;
const quint32 S_ASN1_T61_STRING = 0x14;
const quint32 S_ASN1_VIDEOTEX_STRING = 0x15;
const quint32 S_ASN1_IA5_STRING = 0x16;
const quint32 S_ASN1_UTC_TIME = 0x17;
const quint32 S_ASN1_GENERALIZED_TIME = 0x18;
const quint32 S_ASN1_ASN1_GRAPHIC_STRING = 0x19;
const quint32 S_ASN1_VISIBLE_STRING = 0x1A;
const quint32 S_ASN1_GENERAL_STRING = 0x1B;
const quint32 S_ASN1_UNIVERSAL_STRING = 0x1C;
const quint32 S_ASN1_BMP_STRING = 0x1E;
const quint32 S_ASN1_PRIMITIVE = 0x00;
const quint32 S_ASN1_CONSTRUCTED = 0x20;
const quint32 S_ASN1_CONTEXT_SPECIFIC = 0x80;

struct IMAGE_FILE_HEADER {
    quint16 Machine;
    quint16 NumberOfSections;
    quint32 TimeDateStamp;
    quint32 PointerToSymbolTable;
    quint32 NumberOfSymbols;
    quint16 SizeOfOptionalHeader;
    quint16 Characteristics;
};

struct IMAGE_DATA_DIRECTORY {
    quint32 VirtualAddress;
    quint32 Size;
};

struct IMAGE_OPTIONAL_HEADER32 {
    // Standard fields.
    quint16 Magic;
    quint8 MajorLinkerVersion;
    quint8 MinorLinkerVersion;
    quint32 SizeOfCode;
    quint32 SizeOfInitializedData;
    quint32 SizeOfUninitializedData;
    quint32 AddressOfEntryPoint;
    quint32 BaseOfCode;
    quint32 BaseOfData;
    // NT additional fields.
    quint32 ImageBase;
    quint32 SectionAlignment;
    quint32 FileAlignment;
    quint16 MajorOperatingSystemVersion;
    quint16 MinorOperatingSystemVersion;
    quint16 MajorImageVersion;
    quint16 MinorImageVersion;
    quint16 MajorSubsystemVersion;
    quint16 MinorSubsystemVersion;
    quint32 Win32VersionValue;
    quint32 SizeOfImage;
    quint32 SizeOfHeaders;
    quint32 CheckSum;
    quint16 Subsystem;
    quint16 DllCharacteristics;
    quint32 SizeOfStackReserve;
    quint32 SizeOfStackCommit;
    quint32 SizeOfHeapReserve;
    quint32 SizeOfHeapCommit;
    quint32 LoaderFlags;
    quint32 NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[S_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

struct IMAGE_OPTIONAL_HEADER32S {
    // Standard fields.
    quint16 Magic;
    quint8 MajorLinkerVersion;
    quint8 MinorLinkerVersion;
    quint32 SizeOfCode;
    quint32 SizeOfInitializedData;
    quint32 SizeOfUninitializedData;
    quint32 AddressOfEntryPoint;
    quint32 BaseOfCode;
    quint32 BaseOfData;
    // NT additional fields.
    quint32 ImageBase;
    quint32 SectionAlignment;
    quint32 FileAlignment;
    quint16 MajorOperatingSystemVersion;
    quint16 MinorOperatingSystemVersion;
    quint16 MajorImageVersion;
    quint16 MinorImageVersion;
    quint16 MajorSubsystemVersion;
    quint16 MinorSubsystemVersion;
    quint32 Win32VersionValue;
    quint32 SizeOfImage;
    quint32 SizeOfHeaders;
    quint32 CheckSum;
    quint16 Subsystem;
    quint16 DllCharacteristics;
    quint32 SizeOfStackReserve;
    quint32 SizeOfStackCommit;
    quint32 SizeOfHeapReserve;
    quint32 SizeOfHeapCommit;
    quint32 LoaderFlags;
    quint32 NumberOfRvaAndSizes;
};

struct IMAGE_OPTIONAL_HEADER64 {
    // Standard fields.
    quint16 Magic;
    quint8 MajorLinkerVersion;
    quint8 MinorLinkerVersion;
    quint32 SizeOfCode;
    quint32 SizeOfInitializedData;
    quint32 SizeOfUninitializedData;
    quint32 AddressOfEntryPoint;
    quint32 BaseOfCode;
    // NT additional fields.
    qint64 ImageBase;
    quint32 SectionAlignment;
    quint32 FileAlignment;
    quint16 MajorOperatingSystemVersion;
    quint16 MinorOperatingSystemVersion;
    quint16 MajorImageVersion;
    quint16 MinorImageVersion;
    quint16 MajorSubsystemVersion;
    quint16 MinorSubsystemVersion;
    quint32 Win32VersionValue;
    quint32 SizeOfImage;
    quint32 SizeOfHeaders;
    quint32 CheckSum;
    quint16 Subsystem;
    quint16 DllCharacteristics;
    qint64 SizeOfStackReserve;
    qint64 SizeOfStackCommit;
    qint64 SizeOfHeapReserve;
    qint64 SizeOfHeapCommit;
    quint32 LoaderFlags;
    quint32 NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[S_IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
};

struct IMAGE_OPTIONAL_HEADER64S {
    // Standard fields.
    quint16 Magic;
    quint8 MajorLinkerVersion;
    quint8 MinorLinkerVersion;
    quint32 SizeOfCode;
    quint32 SizeOfInitializedData;
    quint32 SizeOfUninitializedData;
    quint32 AddressOfEntryPoint;
    quint32 BaseOfCode;
    // NT additional fields.
    qint64 ImageBase;
    quint32 SectionAlignment;
    quint32 FileAlignment;
    quint16 MajorOperatingSystemVersion;
    quint16 MinorOperatingSystemVersion;
    quint16 MajorImageVersion;
    quint16 MinorImageVersion;
    quint16 MajorSubsystemVersion;
    quint16 MinorSubsystemVersion;
    quint32 Win32VersionValue;
    quint32 SizeOfImage;
    quint32 SizeOfHeaders;
    quint32 CheckSum;
    quint16 Subsystem;
    quint16 DllCharacteristics;
    qint64 SizeOfStackReserve;
    qint64 SizeOfStackCommit;
    qint64 SizeOfHeapReserve;
    qint64 SizeOfHeapCommit;
    quint32 LoaderFlags;
    quint32 NumberOfRvaAndSizes;
};

struct IMAGE_NT_HEADERS64 {
    quint32 Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
};

struct IMAGE_NT_HEADERS32 {
    quint32 Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
};

struct IMAGE_SECTION_HEADER {
    quint8 Name[S_IMAGE_SIZEOF_SHORT_NAME];
    union {
        quint32 PhysicalAddress;
        quint32 VirtualSize;
    } Misc;
    quint32 VirtualAddress;
    quint32 SizeOfRawData;
    quint32 PointerToRawData;
    quint32 PointerToRelocations;
    quint32 PointerToLinenumbers;
    quint16 NumberOfRelocations;
    quint16 NumberOfLinenumbers;
    quint32 Characteristics;
};

struct IMAGE_RESOURCE_DIRECTORY {
    quint32 Characteristics;
    quint32 TimeDateStamp;
    quint16 MajorVersion;
    quint16 MinorVersion;
    quint16 NumberOfNamedEntries;
    quint16 NumberOfIdEntries;
};

struct IMAGE_RESOURCE_DIRECTORY_ENTRY {
    union {
        struct {
            quint32 NameOffset : 31;
            quint32 NameIsString : 1;
        };
        quint32 Name;
        quint16 Id;
    };
    union {
        quint32 OffsetToData;
        struct {
            quint32 OffsetToDirectory : 31;
            quint32 DataIsDirectory : 1;
        };
    };
};

struct IMAGE_RESOURCE_DATA_ENTRY {
    quint32 OffsetToData;
    quint32 Size;
    quint32 CodePage;
    quint32 Reserved;
};

struct IMAGE_IMPORT_DESCRIPTOR {
    union {
        quint32 Characteristics;
        quint32 OriginalFirstThunk;
    };
    quint32 TimeDateStamp;
    quint32 ForwarderChain;
    quint32 Name;
    quint32 FirstThunk;
};

struct IMAGE_BASE_RELOCATION {
    quint32 VirtualAddress;
    quint32 SizeOfBlock;
    // quint16    TypeOffset[1];
};

struct IMAGE_EXPORT_DIRECTORY {
    quint32 Characteristics;
    quint32 TimeDateStamp;
    quint16 MajorVersion;
    quint16 MinorVersion;
    quint32 Name;
    quint32 Base;
    quint32 NumberOfFunctions;
    quint32 NumberOfNames;
    quint32 AddressOfFunctions;     // RVA from base of image
    quint32 AddressOfNames;         // RVA from base of image
    quint32 AddressOfNameOrdinals;  // RVA from base of image
};

struct SYSTEM_INFO32 {
    union {
        quint32 dwOemId;
        struct {
            quint16 wProcessorArchitecture;
            quint16 wReserved;
        };
    };
    quint32 dwPageSize;
    quint32 lpMinimumApplicationAddress;
    quint32 lpMaximumApplicationAddress;
    quint32 dwActiveProcessorMask;
    quint32 dwNumberOfProcessors;
    quint32 dwProcessorType;
    quint32 dwAllocationGranularity;
    quint16 wProcessorLevel;
    quint16 wProcessorRevision;
};

struct SYSTEM_INFO64 {
    union {
        quint32 dwOemId;
        struct {
            quint16 wProcessorArchitecture;
            quint16 wReserved;
        };
    };
    quint32 dwPageSize;
    quint64 lpMinimumApplicationAddress;
    quint64 lpMaximumApplicationAddress;
    quint32 dwActiveProcessorMask;
    quint32 dwNumberOfProcessors;
    quint32 dwProcessorType;
    quint32 dwAllocationGranularity;
    quint16 wProcessorLevel;
    quint16 wProcessorRevision;
};

struct IMAGE_COR20_HEADER {
    // Header versioning
    quint32 cb;
    quint16 MajorRuntimeVersion;
    quint16 MinorRuntimeVersion;
    // Symbol table and startup information
    IMAGE_DATA_DIRECTORY MetaData;
    quint32 Flags;
    // DDBLD - Added next section to replace following lin
    // DDBLD - Still verifying, since not in NT SDK
    // unsigned int                   EntryPointToken;
    // If COMIMAGE_FLAGS_NATIVE_ENTRYPOINT is not set,
    // EntryPointToken represents a managed entrypoint.
    // If COMIMAGE_FLAGS_NATIVE_ENTRYPOINT is set,
    // EntryPointRVA represents an RVA to a native entrypoint.
    union {
        quint32 EntryPointToken;
        quint32 EntryPointRVA;
    };
    // DDBLD - End of Added Area
    // Binding information
    IMAGE_DATA_DIRECTORY Resources;
    IMAGE_DATA_DIRECTORY StrongNameSignature;
    // Regular fixup and binding information
    IMAGE_DATA_DIRECTORY CodeManagerTable;
    IMAGE_DATA_DIRECTORY VTableFixups;
    IMAGE_DATA_DIRECTORY ExportAddressTableJumps;
    // Precompiled image info (internal use only - set to zero)
    IMAGE_DATA_DIRECTORY ManagedNativeHeader;
};

enum ReplacesCorHdrNumericDefines {
    // COM+ Header entry point flags.
    COMIMAGE_FLAGS_ILONLY = 0x00000001,
    COMIMAGE_FLAGS_32BITREQUIRED = 0x00000002,
    COMIMAGE_FLAGS_IL_LIBRARY = 0x00000004,
    COMIMAGE_FLAGS_STRONGNAMESIGNED = 0x00000008,
    COMIMAGE_FLAGS_NATIVE_ENTRYPOINT = 0x00000010,
    COMIMAGE_FLAGS_TRACKDEBUGDATA = 0x00010000,
    // Version flags for image.
    COR_VERSION_MAJOR_V2 = 2,
    COR_VERSION_MAJOR = COR_VERSION_MAJOR_V2,
    COR_VERSION_MINOR = 5,
    COR_DELETED_NAME_LENGTH = 8,
    COR_VTABLEGAP_NAME_LENGTH = 8,
    // Maximum size of a NativeType descriptor.
    NATIVE_TYPE_MAX_CB = 1,
    COR_ILMETHOD_SECT_SMALL_MAX_DATASIZE = 0xFF,
    // #defines for the MIH FLAGS
    IMAGE_COR_MIH_METHODRVA = 0x01,
    IMAGE_COR_MIH_EHRVA = 0x02,
    IMAGE_COR_MIH_BASICBLOCK = 0x08,
    // V-table constants
    COR_VTABLE_32BIT = 0x01,                            // V-table slots are 32-bits in size.
    COR_VTABLE_64BIT = 0x02,                            // V-table slots are 64-bits in size.
    COR_VTABLE_FROM_UNMANAGED = 0x04,                   // If set, transition from unmanaged.
    COR_VTABLE_FROM_UNMANAGED_RETAIN_APPDOMAIN = 0x08,  // If set, transition from unmanaged with keeping the current
                                                        // appdomain.
    COR_VTABLE_CALL_MOST_DERIVED = 0x10,                // Call most derived method described
                                                        // by EATJ constants
    IMAGE_COR_EATJ_THUNK_SIZE = 32,                     // Size of a jump thunk reserved range.
                                                        // Max name lengths
                                                        //@todo: Change to unlimited name lengths.
    MAX_CLASS_NAME = 1024,
    MAX_PACKAGE_NAME = 1024
};

enum MetadataTable {
    metadata_Module = 0x00,
    metadata_TypeRef = 0x01,
    metadata_TypeDef = 0x02,
    metadata_Field = 0x04,
    metadata_MethodPtr = 0x05,
    metadata_MethodDef = 0x06,
    metadata_ParamPtr = 0x07,
    metadata_Param = 0x08,
    metadata_InterfaceImpl = 0x09,
    metadata_MemberRef = 0x0A,
    metadata_Constant = 0x0B,
    metadata_CustomAttribute = 0x0C,
    metadata_FieldMarshal = 0x0D,
    metadata_DeclSecurity = 0x0E,
    metadata_ClassLayout = 0x0F,
    metadata_FieldLayout = 0x10,
    metadata_StandAloneSig = 0x11,
    metadata_EventMap = 0x12,
    metadata_EventPtr = 0x13,
    metadata_Event = 0x14,
    metadata_PropertyMap = 0x15,
    metadata_PropertyPtr = 0x16,
    metadata_Property = 0x17,
    metadata_MethodSemantics = 0x18,
    metadata_MethodImpl = 0x19,
    metadata_ModuleRef = 0x1A,
    metadata_TypeSpec = 0x1B,
    metadata_ImplMap = 0x1C,
    metadata_FieldRVA = 0x1D,
    metadata_ENCLog = 0x1E,
    metadata_ENCMap = 0x1F,
    metadata_Assembly = 0x20,
    metadata_AssemblyProcessor = 0x21,
    metadata_AssemblyOS = 0x22,
    metadata_AssemblyRef = 0x23,
    metadata_AssemblyRefProcessor = 0x24,
    metadata_AssemblyRefOS = 0x25,
    metadata_File = 0x26,
    metadata_ExportedType = 0x27,
    metadata_ManifestResource = 0x28,
    metadata_NestedClass = 0x29,
    metadata_GenericParam = 0x2A,
    metadata_MethodSpec = 0x2B,
    metadata_GenericParamConstraint = 0x2C,
    metadata_Document = 0x2D,
    metadata_MethodDebugInformation = 0x2E,
    metadata_LocalScope = 0x2F,
    metadata_LocalVariable = 0x30,
    metadata_LocalConstant = 0x31,
    metadata_ImportScope = 0x32,
    metadata_StateMachineMethod = 0x33,
    metadata_CustomDebugInformation = 0x34,
    metadata_Reserved_38 = 0x38,
    metadata_Reserved_39 = 0x39,
    metadata_Reserved_3A = 0x3A,
    metadata_Reserved_3B = 0x3B,
    metadata_Reserved_3C = 0x3C,
    metadata_Reserved_3D = 0x3D,
    metadata_Reserved_3E = 0x3E,
    metadata_Reserved_3F = 0x3F
};

struct S_METADATA_MODULE {
    quint32 nGeneration;
    quint32 nName;
    quint32 nMvid;
    quint32 nEncId;
    quint32 nEncBaseId;
};

struct S_METADATA_TYPEREF {
    quint32 nResolutionScope;
    quint32 nTypeName;
    quint32 nTypeNamespace;
};

struct S_METADATA_TYPEDEF {
    quint32 nFlags;
    quint32 nTypeName;
    quint32 nTypeNamespace;
    quint32 nExtends;
    quint32 nFieldList;
    quint32 nMethodList;
};

struct S_METADATA_TYPESPEC {
    quint32 nSignature;
};

struct S_METADATA_FIELD {
    quint32 nFlags;
    quint32 nName;
    quint32 nSignature;
};

struct S_METADATA_METHODDEF {
    quint32 nRVA;
    quint32 nImplFlags;
    quint32 nFlags;
    quint32 nName;
    quint32 nSignature;
    quint32 nParamList;
};

struct S_METADATA_METHODPTR {
    quint32 nMethod;
};

struct S_METADATA_PARAM {
    quint32 nFlags;
    quint32 nSequence;
    quint32 nName;
};

struct S_METADATA_MEMBERREF {
    quint32 nClass;
    quint32 nName;
    quint32 nSignature;
};

struct S_METADATA_CONSTANT {
    quint32 nType;
    quint32 nParent;
    quint32 nValue;
};

struct S_METADATA_CUSTOMATTRIBUTE {
    quint32 nParent;
    quint32 nType;
    quint32 nValue;
};

struct S_METADATA_FIELDMARSHAL {
    quint32 nParent;
    quint32 nNativeType;
};

struct S_METADATA_DECLSECURITY {
    quint16 nAction;
    quint32 nParent;
    quint32 nPermissionSet;
};

struct S_METADATA_METHODIMPL {
    quint32 nClass;
    quint32 nMethodBody;
    quint32 nMethodDeclaration;
};

struct S_METADATA_MODULEREF {
    quint32 nName;
};

struct S_METADATA_ASSEMBLY {
    quint32 nHashAlgId;
    quint16 nMajorVersion;
    quint16 nMinorVersion;
    quint16 nBuildNumber;
    quint16 nRevisionNumber;
    quint32 nFlags;
    quint32 nPublicKeyOrToken;
    quint32 nName;
    quint32 nCulture;
};

const quint32 S_METADATA_METHODDEFORREF_METHODDEF = 0;
const quint32 S_METADATA_METHODDEFORREF_MEMBERREF = 1;

struct S_METADATA_METHODDEFORREF {
    quint32 nTag;
    quint32 nIndex;
    union {
        S_METADATA_METHODDEF methoddef;
        S_METADATA_MEMBERREF memberref;
    } record;
};

struct S_VS_VERSION_INFO {
    quint16 wLength;      /* Length of the version resource */
    quint16 wValueLength; /* Length of the value field for this block */
    quint16 wType;
};

struct tagVS_FIXEDFILEINFO {
    quint32 dwSignature;         // e.g.  0xfeef04bd
    quint32 dwStrucVersion;      // e.g.  0x00000042 = "0.42"
    quint32 dwFileVersionMS;     // e.g.  0x00030075 = "3.75"
    quint32 dwFileVersionLS;     // e.g.  0x00000031 = "0.31"
    quint32 dwProductVersionMS;  // e.g.  0x00030010 = "3.10"
    quint32 dwProductVersionLS;  // e.g.  0x00000031 = "0.31"
    quint32 dwFileFlagsMask;     // = 0x3F for version "0.42"
    quint32 dwFileFlags;         // e.g.  VFF_DEBUG | VFF_PRERELEASE
    quint32 dwFileOS;            // e.g.  VOS_DOS_WINDOWS16
    quint32 dwFileType;          // e.g.  VFT_DRIVER
    quint32 dwFileSubtype;       // e.g.  VFT2_DRV_KEYBOARD
    quint32 dwFileDateMS;        // e.g.  0
    quint32 dwFileDateLS;        // e.g.  0
};
// typedef VOID (NTAPI *PIMAGE_TLS_CALLBACK)(PVOID DllHandle,quint32
// Reason,PVOID Reserved);

struct S_IMAGE_TLS_DIRECTORY64 {
    quint64 StartAddressOfRawData;
    quint64 EndAddressOfRawData;
    quint64 AddressOfIndex;
    quint64 AddressOfCallBacks;
    quint32 SizeOfZeroFill;
    quint32 Characteristics;
};

struct S_IMAGE_TLS_DIRECTORY32 {
    quint32 StartAddressOfRawData;
    quint32 EndAddressOfRawData;
    quint32 AddressOfIndex;
    quint32 AddressOfCallBacks;
    quint32 SizeOfZeroFill;
    quint32 Characteristics;
};

struct S_IMAGE_ALPHA64_RUNTIME_FUNCTION_ENTRY {
    quint64 BeginAddress;
    quint64 EndAddress;
    quint64 ExceptionHandler;
    quint64 HandlerData;
    quint64 PrologEndAddress;
};

struct S_IMAGE_ALPHA_RUNTIME_FUNCTION_ENTRY {
    quint32 BeginAddress;
    quint32 EndAddress;
    quint32 ExceptionHandler;
    quint32 HandlerData;
    quint32 PrologEndAddress;
};

struct S_IMAGE_DEBUG_DIRECTORY {
    quint32 Characteristics;
    quint32 TimeDateStamp;
    quint16 MajorVersion;
    quint16 MinorVersion;
    quint32 Type;
    quint32 SizeOfData;
    quint32 AddressOfRawData;
    quint32 PointerToRawData;
};

struct S_IMAGE_RUNTIME_FUNCTION_ENTRY {
    quint32 BeginAddress;
    quint32 EndAddress;
    union {
        quint32 UnwindInfoAddress;
        quint32 UnwindData;
    };
};

struct S_IMAGE_LOAD_CONFIG_CODE_INTEGRITY {
    quint16 Flags;    // Flags to indicate if CI information is available, etc.
    quint16 Catalog;  // 0xFFFF means not available
    quint32 CatalogOffset;
    quint32 Reserved;  // Additional bitmask to be defined later
};

struct S_IMAGE_LOAD_CONFIG_DIRECTORY32 {
    quint32 Size;
    quint32 TimeDateStamp;
    quint16 MajorVersion;
    quint16 MinorVersion;
    quint32 GlobalFlagsClear;
    quint32 GlobalFlagsSet;
    quint32 CriticalSectionDefaultTimeout;
    quint32 DeCommitFreeBlockThreshold;
    quint32 DeCommitTotalFreeThreshold;
    quint32 LockPrefixTable;  // VA
    quint32 MaximumAllocationSize;
    quint32 VirtualMemoryThreshold;
    quint32 ProcessHeapFlags;
    quint32 ProcessAffinityMask;
    quint16 CSDVersion;
    quint16 DependentLoadFlags;
    quint32 EditList;        // VA
    quint32 SecurityCookie;  // VA
    quint32 SEHandlerTable;  // VA
    quint32 SEHandlerCount;
    // Standard
    quint32 GuardCFCheckFunctionPointer;     // VA
    quint32 GuardCFDispatchFunctionPointer;  // VA
    quint32 GuardCFFunctionTable;            // VA
    quint32 GuardCFFunctionCount;
    quint32 GuardFlags;
    S_IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    quint32 GuardAddressTakenIatEntryTable;  // VA
    quint32 GuardAddressTakenIatEntryCount;
    quint32 GuardLongJumpTargetTable;  // VA
    quint32 GuardLongJumpTargetCount;
    quint32 DynamicValueRelocTable;  // VA
    quint32 CHPEMetadataPointer;
    quint32 GuardRFFailureRoutine;                 // VA
    quint32 GuardRFFailureRoutineFunctionPointer;  // VA
    quint32 DynamicValueRelocTableOffset;
    quint16 DynamicValueRelocTableSection;
    quint16 Reserved2;
    quint32 GuardRFVerifyStackPointerFunctionPointer;  // VA
    quint32 HotPatchTableOffset;
    quint32 Reserved3;
    quint32 EnclaveConfigurationPointer;  // VA
    quint32 VolatileMetadataPointer;      // VA
    quint32 GuardEHContinuationTable;
    quint32 GuardEHContinuationCount;
    quint32 GuardXFGCheckFunctionPointer;
    quint32 GuardXFGDispatchFunctionPointer;
    quint32 GuardXFGTableDispatchFunctionPointer;
    quint32 CastGuardOsDeterminedFailureMode;
    quint32 GuardMemcpyFunctionPointer;
};

struct S_IMAGE_LOAD_CONFIG_DIRECTORY64 {
    quint32 Size;
    quint32 TimeDateStamp;
    quint16 MajorVersion;
    quint16 MinorVersion;
    quint32 GlobalFlagsClear;
    quint32 GlobalFlagsSet;
    quint32 CriticalSectionDefaultTimeout;
    quint64 DeCommitFreeBlockThreshold;
    quint64 DeCommitTotalFreeThreshold;
    quint64 LockPrefixTable;  // VA
    quint64 MaximumAllocationSize;
    quint64 VirtualMemoryThreshold;
    quint64 ProcessAffinityMask;
    quint32 ProcessHeapFlags;
    quint16 CSDVersion;
    quint16 DependentLoadFlags;
    quint64 EditList;        // VA
    quint64 SecurityCookie;  // VA
    quint64 SEHandlerTable;  // VA
    quint64 SEHandlerCount;
    // Standard
    quint64 GuardCFCheckFunctionPointer;     // VA
    quint64 GuardCFDispatchFunctionPointer;  // VA
    quint64 GuardCFFunctionTable;            // VA
    quint64 GuardCFFunctionCount;
    quint32 GuardFlags;
    S_IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    quint64 GuardAddressTakenIatEntryTable;  // VA
    quint64 GuardAddressTakenIatEntryCount;
    quint64 GuardLongJumpTargetTable;  // VA
    quint64 GuardLongJumpTargetCount;
    quint64 DynamicValueRelocTable;                // VA
    quint64 CHPEMetadataPointer;                   // VA
    quint64 GuardRFFailureRoutine;                 // VA
    quint64 GuardRFFailureRoutineFunctionPointer;  // VA
    quint32 DynamicValueRelocTableOffset;
    quint16 DynamicValueRelocTableSection;
    quint16 Reserved2;
    quint64 GuardRFVerifyStackPointerFunctionPointer;  // VA
    quint32 HotPatchTableOffset;
    quint32 Reserved3;
    quint64 EnclaveConfigurationPointer;  // VA
    quint64 VolatileMetadataPointer;      // VA
    quint64 GuardEHContinuationTable;
    quint64 GuardEHContinuationCount;
    quint64 GuardXFGCheckFunctionPointer;
    quint64 GuardXFGDispatchFunctionPointer;
    quint64 GuardXFGTableDispatchFunctionPointer;
    quint64 CastGuardOsDeterminedFailureMode;
    quint64 GuardMemcpyFunctionPointer;
};

struct S_IMAGE_DELAYLOAD_DESCRIPTOR {
    union {
        quint32 AllAttributes;
        struct {
            quint32 RvaBased : 1;  // Delay load version 2
            quint32 ReservedAttributes : 31;
        } DUMMYSTRUCTNAME;
    };
    quint32 DllNameRVA;                  // RVA to the name of the target library
                                         // (NULL-terminate ASCII string)
    quint32 ModuleHandleRVA;             // RVA to the HMODULE caching location (PHMODULE)
    quint32 ImportAddressTableRVA;       // RVA to the start of the IAT
                                         // (PIMAGE_THUNK_DATA)
    quint32 ImportNameTableRVA;          // RVA to the start of the name table
                                         // (PIMAGE_THUNK_DATA::AddressOfData)
    quint32 BoundImportAddressTableRVA;  // RVA to an optional bound IAT
    quint32 UnloadInformationTableRVA;   // RVA to an optional unload info table
    quint32 TimeDateStamp;               // 0 if not bound,
                                         // Otherwise, date/time of the target DLL
};

struct IMAGE_BOUND_IMPORT_DESCRIPTOR {
    quint32 TimeDateStamp;
    quint16 OffsetModuleName;
    quint16 NumberOfModuleForwarderRefs;
    // Array of zero or more IMAGE_BOUND_FORWARDER_REF follows
};

struct WIN_CERT_RECORD {
    quint32 dwLength;
    quint16 wRevision;
    quint16 wCertificateType;
};

struct NB10I  // NB10 debug info
{
    quint32 dwSig;     // NB10
    quint32 dwOffset;  // offset, always 0
    quint32 sig;
    quint32 age;
    char szPdb[260];  // TODO const
};

struct RSDSI  // RSDS debug info
{
    quint32 dwSig;  // RSDS
    XBINARY_DEF::XGUID guidSig;
    quint32 age;
    char szPdb[260 * 3];  // TODO const
};

}  // namespace XPE_DEF
#endif  // XPE_DEF_H
