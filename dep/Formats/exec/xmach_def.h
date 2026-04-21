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
#ifndef QMACH_DEF_H
#define QMACH_DEF_H

#include <QtGlobal>

namespace XMACH_DEF {
/* Constant for the magic field of the mach_header (32-bit architectures) */
const quint32 S_MH_MAGIC = 0xFEEDFACE; /* the mach magic number */
const quint32 S_MH_CIGAM = 0xCEFAEDFE; /* NXSwapInt(MH_MAGIC) */
/* Constant for the magic field of the mach_header_64 (64-bit architectures) */
const quint32 S_MH_MAGIC_64 = 0xFEEDFACF; /* the 64-bit mach magic number */
const quint32 S_MH_CIGAM_64 = 0xCFFAEDFE; /* NXSwapInt(MH_MAGIC_64) */

const quint32 S_FAT_MAGIC = 0xCAFEBABE;
const quint32 S_FAT_CIGAM = 0xBEBAFECA;

const quint32 S_CPU_TYPE_VAX = 0x00000001;
const quint32 S_CPU_TYPE_ROMP = 0x00000002;
const quint32 S_CPU_TYPE_NS32032 = 0x00000004;
const quint32 S_CPU_TYPE_NS32332 = 0x00000005;
const quint32 S_CPU_TYPE_MC680x0 = 0x00000006;
const quint32 S_CPU_TYPE_I386 = 0x00000007;
const quint32 S_CPU_TYPE_X86_64 = 0x01000007;
const quint32 S_CPU_TYPE_MIPS = 0x00000008;
const quint32 S_CPU_TYPE_NS32532 = 0x00000009;
const quint32 S_CPU_TYPE_HPPA = 0x0000000B;
const quint32 S_CPU_TYPE_ARM = 0x0000000C;
const quint32 S_CPU_TYPE_ARM64 = 0x0100000C;
const quint32 S_CPU_TYPE_ARM64_32 = 0x0200000C;
const quint32 S_CPU_TYPE_MC88000 = 0x0000000D;
const quint32 S_CPU_TYPE_SPARC = 0x0000000E;
const quint32 S_CPU_TYPE_I860 = 0x0000000F;
const quint32 S_CPU_TYPE_RS6000 = 0x00000011;
const quint32 S_CPU_TYPE_MC98000 = 0x00000012;
const quint32 S_CPU_TYPE_POWERPC = 0x00000012;
const quint32 S_CPU_TYPE_POWERPC64 = 0x01000012;
const quint32 S_CPU_TYPE_VEO = 0x000000FF;

const quint32 S_CPU_SUBTYPE_ARM_ALL = 0;
const quint32 S_CPU_SUBTYPE_ARM_A500_ARCH = 1;
const quint32 S_CPU_SUBTYPE_ARM_A500 = 2;
const quint32 S_CPU_SUBTYPE_ARM_A440 = 3;
const quint32 S_CPU_SUBTYPE_ARM_M4 = 4;
const quint32 S_CPU_SUBTYPE_ARM_V4T = 5;
const quint32 S_CPU_SUBTYPE_ARM_V6 = 6;
const quint32 S_CPU_SUBTYPE_ARM_V5TEJ = 7;
const quint32 S_CPU_SUBTYPE_ARM_XSCALE = 8;
const quint32 S_CPU_SUBTYPE_ARM_V7 = 9;
const quint32 S_CPU_SUBTYPE_ARM_V7F = 10;
const quint32 S_CPU_SUBTYPE_ARM_V7S = 11;
const quint32 S_CPU_SUBTYPE_ARM_V7K = 12;

const quint32 S_INDIRECT_SYMBOL_LOCAL = 0x80000000;
const quint32 S_INDIRECT_SYMBOL_ABS = 0x40000000;

struct fat_header {
    quint32 magic;
    quint32 nfat_arch;
};

struct fat_arch {
    quint32 cputype;
    quint32 cpusubtype;
    quint32 offset;
    quint32 size;
    quint32 align;
};

/*
 * The 32-bit mach header appears at the very beginning of the object file for
 * 32-bit architectures.
 */
struct mach_header {
    quint32 magic;      /* mach magic number identifier */
    quint32 cputype;    /* cpu specifier */
    quint32 cpusubtype; /* machine specifier */
    quint32 filetype;   /* type of file */
    quint32 ncmds;      /* number of load commands */
    quint32 sizeofcmds; /* the size of all the load commands */
    quint32 flags;      /* flags */
};

/*
 * The 64-bit mach header appears at the very beginning of object files for
 * 64-bit architectures.
 */
struct mach_header_64 {
    quint32 magic;      /* mach magic number identifier */
    quint32 cputype;    /* cpu specifier */
    quint32 cpusubtype; /* machine specifier */
    quint32 filetype;   /* type of file */
    quint32 ncmds;      /* number of load commands */
    quint32 sizeofcmds; /* the size of all the load commands */
    quint32 flags;      /* flags */
    quint32 reserved;
    /* reserved */
};

struct load_command {
    quint32 cmd;
    quint32 cmdsize;
};

struct segment_command {
    quint32 cmd;
    quint32 cmdsize;
    char segname[16];
    quint32 vmaddr;
    quint32 vmsize;
    quint32 fileoff;
    quint32 filesize;
    quint32 maxprot;
    quint32 initprot;
    quint32 nsects;
    quint32 flags;
};

struct segment_command_64 {
    quint32 cmd;
    quint32 cmdsize;
    char segname[16];
    quint64 vmaddr;
    quint64 vmsize;
    quint64 fileoff;
    quint64 filesize;
    quint32 maxprot;
    quint32 initprot;
    quint32 nsects;
    quint32 flags;
};

struct section /* for 32-bit architectures */
{
    char sectname[16]; /* name of this section */
    char segname[16];  /* segment this section goes in */
    quint32 addr;      /* memory address of this section */
    quint32 size;      /* size in bytes of this section */
    quint32 offset;    /* file offset of this section */
    quint32 align;     /* section alignment (power of 2) */
    quint32 reloff;    /* file offset of relocation entries */
    quint32 nreloc;    /* number of relocation entries */
    quint32 flags;     /* flags (section type and attributes)*/
    quint32 reserved1; /* reserved (for offset or index) */
    quint32 reserved2; /* reserved (for count or sizeof) */
};

struct section_64 /* for 64-bit architectures */
{
    char sectname[16]; /* name of this section */
    char segname[16];  /* segment this section goes in */
    quint64 addr;      /* memory address of this section */
    quint64 size;      /* size in bytes of this section */
    quint32 offset;    /* file offset of this section */
    quint32 align;     /* section alignment (power of 2) */
    quint32 reloff;    /* file offset of relocation entries */
    quint32 nreloc;    /* number of relocation entries */
    quint32 flags;     /* flags (section type and attributes)*/
    quint32 reserved1; /* reserved (for offset or index) */
    quint32 reserved2; /* reserved (for count or sizeof) */
    quint32 reserved3; /* reserved */
};

const quint32 S_LC_REQ_DYLD = 0x80000000;
const quint32 S_LC_SEGMENT = 0x1;                                /* segment of this file to be mapped */
const quint32 S_LC_SYMTAB = 0x2;                                 /* link-edit stab symbol table info */
const quint32 S_LC_SYMSEG = 0x3;                                 /* link-edit gdb symbol table info (obsolete) */
const quint32 S_LC_THREAD = 0x4;                                 /* thread */
const quint32 S_LC_UNIXTHREAD = 0x5;                             /* unix thread (includes a stack) */
const quint32 S_LC_LOADFVMLIB = 0x6;                             /* load a specified fixed VM shared library */
const quint32 S_LC_IDFVMLIB = 0x7;                               /* fixed VM shared library identification */
const quint32 S_LC_IDENT = 0x8;                                  /* object identification info (obsolete) */
const quint32 S_LC_FVMFILE = 0x9;                                /* fixed VM file inclusion (internal use) */
const quint32 S_LC_PREPAGE = 0xA;                                /* prepage command (internal use) */
const quint32 S_LC_DYSYMTAB = 0xB;                               /* dynamic link-edit symbol table info */
const quint32 S_LC_LOAD_DYLIB = 0xC;                             /* load a dynamicly linked shared library */
const quint32 S_LC_ID_DYLIB = 0xD;                               /* dynamicly linked shared lib identification */
const quint32 S_LC_LOAD_DYLINKER = 0xE;                          /* load a dynamic linker */
const quint32 S_LC_ID_DYLINKER = 0xF;                            /* dynamic linker identification */
const quint32 S_LC_PREBOUND_DYLIB = 0x10;                        /* modules prebound for a dynamicly */
const quint32 S_LC_ROUTINES = 0x11;                              /* image routines */
const quint32 S_LC_SUB_FRAMEWORK = 0x12;                         /* sub framework */
const quint32 S_LC_SUB_UMBRELLA = 0x13;                          /* sub umbrella */
const quint32 S_LC_SUB_CLIENT = 0x14;                            /* sub client */
const quint32 S_LC_SUB_LIBRARY = 0x15;                           /* sub library */
const quint32 S_LC_TWOLEVEL_HINTS = 0x16;                        /* two-level namespace lookup hints */
const quint32 S_LC_PREBIND_CKSUM = 0x17;                         /* prebind checksum */
const quint32 S_LC_LOAD_WEAK_DYLIB = (0x18 | S_LC_REQ_DYLD);     /* load a dynamically linked shared library that is allowed
                                                                    to be missing (all symbols are weak imported)*/
const quint32 S_LC_SEGMENT_64 = 0x19;                            /* 64-bit segment of this file to bemapped */
const quint32 S_LC_ROUTINES_64 = 0x1A;                           /* 64-bit image routines */
const quint32 S_LC_UUID = 0x1B;                                  /* the uuid */
const quint32 S_LC_RPATH = (0x1C | S_LC_REQ_DYLD);               /* runpath additions */
const quint32 S_LC_CODE_SIGNATURE = 0x1D;                        /* local of code signature */
const quint32 S_LC_SEGMENT_SPLIT_INFO = 0x1e;                    /* local of info to split segments */
const quint32 S_LC_REEXPORT_DYLIB = (0x1f | S_LC_REQ_DYLD);      /* load and re-export dylib */
const quint32 S_LC_LAZY_LOAD_DYLIB = 0x20;                       /* delay load of dylib until first use */
const quint32 S_LC_ENCRYPTION_INFO = 0x21;                       /* encrypted segment information */
const quint32 S_LC_DYLD_INFO = 0x22;                             /* compressed dyld information */
const quint32 S_LC_DYLD_INFO_ONLY = (0x22 | S_LC_REQ_DYLD);      /* compressed dyld information */
const quint32 S_LC_LOAD_UPWARD_DYLIB = (0x23 | S_LC_REQ_DYLD);   /* load upward dylib */
const quint32 S_LC_VERSION_MIN_MACOSX = 0x24;                    /* build for MacOSX min OS version */
const quint32 S_LC_VERSION_MIN_IPHONEOS = 0x25;                  /* build for iPhoneOS min OS version */
const quint32 S_LC_FUNCTION_STARTS = 0x26;                       /* compressed table of function start addresses */
const quint32 S_LC_DYLD_ENVIRONMENT = 0x27;                      /* string for dyld to treat */
const quint32 S_LC_MAIN = (0x28 | S_LC_REQ_DYLD);                /* main */
const quint32 S_LC_DATA_IN_CODE = 0x29;                          /* table of non-instructions in __text */
const quint32 S_LC_SOURCE_VERSION = 0x2A;                        /* source version used to build binary */
const quint32 S_LC_DYLIB_CODE_SIGN_DRS = 0x2B;                   /* Code signing DRs copied from linked dylibs */
const quint32 S_LC_ENCRYPTION_INFO_64 = 0x2C;                    /* 64-bit encrypted segment information */
const quint32 S_LC_LINKER_OPTION = 0x2D;                         /* linker options in MH_OBJECT files */
const quint32 S_LC_LINKER_OPTIMIZATION_HINT = 0x2E;              /* optimization hints in MH_OBJECT files */
const quint32 S_LC_VERSION_MIN_TVOS = 0x2F;                      /* build for AppleTV min OS version */
const quint32 S_LC_VERSION_MIN_WATCHOS = 0x30;                   /* build for Watch min OS version */
const quint32 S_LC_NOTE = 0x31;                                  /* arbitrary data included within a Mach-O file */
const quint32 S_LC_BUILD_VERSION = 0x32;                         /* build for platform min OS version */
const quint32 S_LC_DYLD_EXPORTS_TRIE = (0x33 | S_LC_REQ_DYLD);   /* used with linkedit_data_command, payload is trie */
const quint32 S_LC_DYLD_CHAINED_FIXUPS = (0x34 | S_LC_REQ_DYLD); /* used with linkedit_data_command */
const quint32 S_LC_FILESET_ENTRY = (0x35 | S_LC_REQ_DYLD);       /* used with fileset_entry_command */
const quint32 S_LC_ATOM_INFO = 0x36;                             /* used with linkedit_data_command */

const quint32 S_MH_OBJECT = 0x1;      /* relocatable object file */
const quint32 S_MH_EXECUTE = 0x2;     /* demand paged executable file */
const quint32 S_MH_FVMLIB = 0x3;      /* fixed VM shared library file */
const quint32 S_MH_CORE = 0x4;        /* core file */
const quint32 S_MH_PRELOAD = 0x5;     /* preloaded executable file */
const quint32 S_MH_DYLIB = 0x6;       /* dynamicly bound shared library file*/
const quint32 S_MH_DYLINKER = 0x7;    /* dynamic link editor */
const quint32 S_MH_BUNDLE = 0x8;      /* dynamicly bound bundle file */
const quint32 S_MH_DYLIB_STUB = 0x9;  /* shared library stub for static */
const quint32 S_MH_DSYM = 0xa;        /* companion file with only debug */
const quint32 S_MH_KEXT_BUNDLE = 0xb; /* x86_64 kexts */
const quint32 S_MH_FILESET = 0xc;     /* file set bundle */
const quint32 S_MH_GPU_EXECUTE = 0xd; /* GPU compute kernal */
const quint32 S_MH_GPU_DYLIB = 0xe;   /* GPU compute kernal */

const quint32 S_VM_PROT_NONE = 0x00;    /* no permission */
const quint32 S_VM_PROT_READ = 0x01;    /* read permission */
const quint32 S_VM_PROT_WRITE = 0x02;   /* write permission */
const quint32 S_VM_PROT_EXECUTE = 0x04; /* execute permission */

const quint32 S_SECTION_TYPE = 0x000000ff;
const quint32 S_SECTION_ATTRIBUTES_SYS = 0x00ffff00; /* system setable attributes */
const quint32 S_SECTION_ATTRIBUTES_USR = 0xff000000; /* User setable attributes */

struct x86_thread_state32_t {
    quint32 eax;
    quint32 ebx;
    quint32 ecx;
    quint32 edx;
    quint32 edi;
    quint32 esi;
    quint32 ebp;
    quint32 esp;
    quint32 ss;
    quint32 eflags;
    quint32 eip;
    quint32 cs;
    quint32 ds;
    quint32 es;
    quint32 fs;
    quint32 gs;
};

struct x86_thread_state64_t {
    quint64 rax;
    quint64 rbx;
    quint64 rcx;
    quint64 rdx;
    quint64 rdi;
    quint64 rsi;
    quint64 rbp;
    quint64 rsp;
    quint64 r8;
    quint64 r9;
    quint64 r10;
    quint64 r11;
    quint64 r12;
    quint64 r13;
    quint64 r14;
    quint64 r15;
    quint64 rip;
    quint64 rflags;
    quint64 cs;
    quint64 fs;
    quint64 gs;
};

struct fp_control_t {
    quint16 invalid : 1;
    quint16 denorm : 1;
    quint16 zdiv : 1;
    quint16 ovrfl : 1;
    quint16 undfl : 1;
    quint16 precis : 1;
    quint16 : 2;
    quint16 pc : 2;
    quint16 rc : 2;
    quint16 : 1;
    quint16 : 3;
};

struct fp_status_t {
    quint16 invalid : 1;
    quint16 denorm : 1;
    quint16 zdiv : 1;
    quint16 ovrfl : 1;
    quint16 undfl : 1;
    quint16 precis : 1;
    quint16 stkflt : 1;
    quint16 errsumm : 1;
    quint16 c0 : 1;
    quint16 c1 : 1;
    quint16 c2 : 1;
    quint16 tos : 3;
    quint16 c3 : 1;
    quint16 busy : 1;
};

struct mmst_reg_t {
    qint8 mmst_reg[10];
    qint8 mmst_rsrv[6];
};

struct xmm_reg_t {
    qint8 xmm_reg[16];
};

struct x86_float_state64_t {
    qint32 fpu_reserved[2];
    fp_control_t fpu_fcw;
    fp_status_t fpu_fsw;
    quint8 fpu_ftw;
    quint8 fpu_rsrv1;
    quint16 fpu_fop;
    quint32 fpu_ip;
    quint16 fpu_cs;
    quint16 fpu_rsrv2;
    quint32 fpu_dp;
    quint16 fpu_ds;
    quint16 fpu_rsrv3;
    quint32 fpu_mxcsr;
    quint32 fpu_mxcsrmask;
    mmst_reg_t fpu_stmm0;
    mmst_reg_t fpu_stmm1;
    mmst_reg_t fpu_stmm2;
    mmst_reg_t fpu_stmm3;
    mmst_reg_t fpu_stmm4;
    mmst_reg_t fpu_stmm5;
    mmst_reg_t fpu_stmm6;
    mmst_reg_t fpu_stmm7;
    xmm_reg_t fpu_xmm0;
    xmm_reg_t fpu_xmm1;
    xmm_reg_t fpu_xmm2;
    xmm_reg_t fpu_xmm3;
    xmm_reg_t fpu_xmm4;
    xmm_reg_t fpu_xmm5;
    xmm_reg_t fpu_xmm6;
    xmm_reg_t fpu_xmm7;
    xmm_reg_t fpu_xmm8;
    xmm_reg_t fpu_xmm9;
    xmm_reg_t fpu_xmm10;
    xmm_reg_t fpu_xmm11;
    xmm_reg_t fpu_xmm12;
    xmm_reg_t fpu_xmm13;
    xmm_reg_t fpu_xmm14;
    xmm_reg_t fpu_xmm15;
    qint8 fpu_rsrv4[6 * 16];
    quint32 fpu_reserved1;
};

struct x86_exception_state64_t {
    quint16 trapno;
    quint16 cpu;
    quint32 err;
    quint64 faultvaddr;
};

struct x86_state_hdr_t {
    quint32 flavor;
    quint32 count;
};

struct x86_thread_state_t {
    x86_state_hdr_t tsh;
    union {
        x86_thread_state64_t ts64;
        x86_thread_state32_t ts32;
    } uts;
};

struct x86_float_state_t {
    x86_state_hdr_t fsh;
    union {
        x86_float_state64_t fs64;
    } ufs;
};

struct x86_exception_state_t {
    x86_state_hdr_t esh;
    union {
        x86_exception_state64_t es64;
    } ues;
};

enum X86ThreadFlavors {
    S_x86_THREAD_STATE32 = 1,
    S_x86_FLOAT_STATE32 = 2,
    S_x86_EXCEPTION_STATE32 = 3,
    S_x86_THREAD_STATE64 = 4,
    S_x86_FLOAT_STATE64 = 5,
    S_x86_EXCEPTION_STATE64 = 6,
    S_x86_THREAD_STATE = 7,
    S_x86_FLOAT_STATE = 8,
    S_x86_EXCEPTION_STATE = 9,
    S_x86_DEBUG_STATE32 = 10,
    S_x86_DEBUG_STATE64 = 11,
    S_x86_DEBUG_STATE = 12
};

struct arm_thread_state32_t {
    quint32 r[13];
    quint32 sp;
    quint32 lr;
    quint32 pc;
    quint32 cpsr;
};

struct arm_thread_state64_t {
    quint64 x[29];
    quint64 fp;
    quint64 lr;
    quint64 sp;
    quint64 pc;
    quint32 cpsr;
    quint32 pad;
};

struct arm_state_hdr_t {
    quint32 flavor;
    quint32 count;
};

struct arm_thread_state_t {
    arm_state_hdr_t tsh;
    union {
        arm_thread_state32_t ts32;
    } uts;
};

enum S_ARMThreadFlavors {
    S_ARM_THREAD_STATE = 1,
    S_ARM_VFP_STATE = 2,
    S_ARM_EXCEPTION_STATE = 3,
    S_ARM_DEBUG_STATE = 4,
    S_ARN_THREAD_STATE_NONE = 5,
    S_ARM_THREAD_STATE64 = 6,
    S_ARM_EXCEPTION_STATE64 = 7
};

struct ppc_thread_state32_t {
    quint32 srr0;
    quint32 srr1;
    quint32 r[32];
    quint32 ct;
    quint32 xer;
    quint32 lr;
    quint32 ctr;
    quint32 mq;
    quint32 vrsave;
};

struct ppc_thread_state64_t {
    quint64 srr0;  // Machine state register (PC)
    quint64 srr1;
    quint64 r0;  // Stack pointer
    quint64 r1;
    quint64 r2;
    quint64 r3;
    quint64 r4;
    quint64 r5;
    quint64 r6;
    quint64 r7;
    quint64 r8;
    quint64 r9;
    quint64 r10;
    quint64 r11;
    quint64 r12;
    quint64 r13;
    quint64 r14;
    quint64 r15;
    quint64 r16;
    quint64 r17;
    quint64 r18;
    quint64 r19;
    quint64 r20;
    quint64 r21;
    quint64 r22;
    quint64 r23;
    quint64 r24;
    quint64 r25;
    quint64 r26;
    quint64 r27;
    quint64 r28;
    quint64 r29;
    quint64 r30;
    quint64 r31;
    quint32 cr;      // Condition register
    quint64 xer;     // User's integer exception register
    quint64 lr;      // Link register
    quint64 ctr;     // Count register
    quint32 vrsave;  // Vector Save Register
};

struct ppc_state_hdr_t {
    quint32 flavor;
    quint32 count;
};

struct ppc_thread_state_t {
    ppc_state_hdr_t tsh;
    union {
        ppc_thread_state32_t ts32;
    } uts;
};

enum PPCThreadFlavors {
    PPC_THREAD_STATE = 1,
    PPC_FLOAT_STATE = 2,
    PPC_EXCEPTION_STATE = 3,
    PPC_VECTOR_STATE = 4,
    PPC_THREAD_STATE64 = 5,
    PPC_EXCEPTION_STATE64 = 6,
    PPC_THREAD_STATE_NONE = 7
};

struct m68k_thread_state32_t {
    quint32 dreg[8]; /* data registers */
    quint32 areg[8]; /* address registers (incl stack pointer) */
    quint16 pad0;    /* not used */
    quint16 sr;      /* user's status register */
    quint32 pc;      /* user's program counter */
};

struct m68k_state_hdr_t {
    quint32 flavor;
    quint32 count;
};

struct m68k_thread_state_t {
    m68k_state_hdr_t tsh;
    union {
        m68k_thread_state32_t ts32;
    } uts;
};

const quint32 S_M68K_THREAD_STATE_REGS = 1;     /* normal registers */
const quint32 S_M68K_THREAD_STATE_68882 = 2;    /* 68882 registers */
const quint32 S_M68K_THREAD_STATE_USER_REG = 3; /* additional user register */

struct state_hdr_t {
    quint32 flavor;
    quint32 count;
};

struct entry_point_command {
    quint32 cmd;       /* LC_MAIN only used in MH_EXECUTE filetypes */
    quint32 cmdsize;   /* 24 */
    quint64 entryoff;  /* file (__TEXT) offset of main() */
    quint64 stacksize; /* if not zero, initial stack size */
};

struct unix_thread_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 flavor;
    quint32 count;
};

struct dylib {
    quint32 name;  // rel offset
    quint32 timestamp;
    quint32 current_version;
    quint32 compatibility_version;
};

struct dylib_command {
    quint32 cmd;
    quint32 cmdsize;
    dylib _dylib;
};

struct dyld_info_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 rebase_off;
    quint32 rebase_size;
    quint32 bind_off;
    quint32 bind_size;
    quint32 weak_bind_off;
    quint32 weak_bind_size;
    quint32 lazy_bind_off;
    quint32 lazy_bind_size;
    quint32 export_off;
    quint32 export_size;
};
/*
 * Fixed virtual memory shared libraries are identified by two things.  The
 * target pathname (the name of the library as found for execution), and the
 * minor version number.  The address of where the headers are loaded is in
 * header_addr.
 */
struct fvmlib {
    quint32 name;          /* library's target pathname */
    quint32 minor_version; /* library's minor version number */
    quint32 header_addr;   /* library's header address */
};

/*
 * A fixed virtual shared library (filetype == MH_FVMLIB in the mach header)
 * contains a fvmlib_command (cmd == LC_IDFVMLIB) to identify the library.
 * An object that uses a fixed virtual shared library also contains a
 * fvmlib_command (cmd == LC_LOADFVMLIB) for each library it uses.
 */
struct fvmlib_command {
    quint32 cmd;          /* LC_IDFVMLIB or LC_LOADFVMLIB */
    quint32 cmdsize;      /* includes pathname string */
    struct fvmlib fvmlib; /* the library identification */
};

struct uuid_command {
    quint32 cmd;
    quint32 cmdsize;
    quint8 uuid[16];
};

struct symtab_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 symoff;
    quint32 nsyms;
    quint32 stroff;
    quint32 strsize;
};

struct dysymtab_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 ilocalsym;
    quint32 nlocalsym;
    quint32 iextdefsym;
    quint32 nextdefsym;
    quint32 iundefsym;
    quint32 nundefsym;
    quint32 tocoff;
    quint32 ntoc;
    quint32 modtaboff;
    quint32 nmodtab;
    quint32 extrefsymoff;
    quint32 nextrefsyms;
    quint32 indirectsymoff;
    quint32 nindirectsyms;
    quint32 extreloff;
    quint32 nextrel;
    quint32 locreloff;
    quint32 nlocrel;
};

struct version_min_command {
    quint32 cmd;      // LC_VERSION_MIN_MACOSX or
                      // LC_VERSION_MIN_IPHONEOS
    quint32 cmdsize;  // sizeof(struct version_min_command)
    quint32 version;  // X.Y.Z is encoded in nibbles xxxx.yy.zz
    quint32 sdk;      // X.Y.Z is encoded in nibbles xxxx.yy.zz
};

struct dylinker_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 name;
};

struct rpath_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 path;
};

struct source_version_command {
    quint32 cmd;
    quint32 cmdsize;
    quint64 version;
};

struct encryption_info_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 cryptoff;
    quint32 cryptsize;
    quint32 cryptid;
};

struct encryption_info_command_64 {
    quint32 cmd;
    quint32 cmdsize;
    quint32 cryptoff;
    quint32 cryptsize;
    quint32 cryptid;
    quint32 pad;
};

struct dylib_module {
    quint32 module_name;
    quint32 iextdefsym;
    quint32 nextdefsym;
    quint32 irefsym;
    quint32 nrefsym;
    quint32 ilocalsym;
    quint32 nlocalsym;
    quint32 iextrel;
    quint32 nextrel;
    quint32 iinit_iterm;
    quint32 ninit_nterm;
    quint32 objc_module_info_addr;
    quint32 objc_module_info_size;
};

struct dylib_module_64 {
    quint32 module_name;
    quint32 iextdefsym;
    quint32 nextdefsym;
    quint32 irefsym;
    quint32 nrefsym;
    quint32 ilocalsym;
    quint32 nlocalsym;
    quint32 iextrel;
    quint32 nextrel;
    quint32 iinit_iterm;
    quint32 ninit_nterm;
    quint32 objc_module_info_size;
    quint64 objc_module_info_addr;
};

struct routines_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 init_address;
    quint32 init_module;
    quint32 reserved1;
    quint32 reserved2;
    quint32 reserved3;
    quint32 reserved4;
    quint32 reserved5;
    quint32 reserved6;
};

struct routines_command_64 {
    quint32 cmd;
    quint32 cmdsize;
    quint64 init_address;
    quint64 init_module;
    quint64 reserved1;
    quint64 reserved2;
    quint64 reserved3;
    quint64 reserved4;
    quint64 reserved5;
    quint64 reserved6;
};

struct linkedit_data_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 dataoff;
    quint32 datasize;
};

struct data_in_code_entry {
    quint32 offset; /* from mach_header to start of data range*/
    quint16 length; /* number of bytes in data range */
    quint16 kind;   /* a DICE_KIND_* value  */
};

struct nlist {
    quint32 n_strx;
    quint8 n_type;
    quint8 n_sect;
    quint16 n_desc;
    quint32 n_value;
};

struct nlist_64 {
    quint32 n_strx;
    quint8 n_type;
    quint8 n_sect;
    quint16 n_desc;
    quint64 n_value;
};

struct sub_framework_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 umbrella;
};

struct sub_client_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 client;
};

struct sub_umbrella_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 sub_umbrella;
};

struct sub_library_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 sub_library;
};

struct prebound_dylib_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 name;
    quint32 nmodules;
    quint32 linked_modules;
};

struct thread_command {
    quint32 cmd;
    quint32 cmdsize;
};

struct twolevel_hints_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 offset;
    quint32 nhints;
};

struct prebind_cksum_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 cksum;
};

struct symseg_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 offset;
    quint32 size;
};

struct ident_command {
    quint32 cmd;
    quint32 cmdsize;
};

struct fvmfile_command {
    quint32 cmd;
    quint32 cmdsize;
    quint32 name;
    quint32 header_addr;
};

struct build_version_command {
    quint32 cmd;     /* LC_BUILD_VERSION */
    quint32 cmdsize; /* sizeof(struct build_version_command) plus */
    /* ntools * sizeof(struct build_tool_version) */
    quint32 platform; /* platform */
    quint32 minos;    /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    quint32 sdk;      /* X.Y.Z is encoded in nibbles xxxx.yy.zz */
    quint32 ntools;   /* number of tool entries following this */
};

struct build_tool_version {
    quint32 tool;    /* enum for the tool */
    quint32 version; /* version number of the tool */
};

struct fileset_entry_command {
    quint32 cmd;      /* LC_FILESET_ENTRY */
    quint32 cmdsize;  /* includes entry_id string */
    quint64 vmaddr;   /* memory address of the entry */
    quint64 fileoff;  /* file offset of the entry */
    quint32 entry_id; /* contained entry id */
    quint32 reserved; /* reserved */
};

struct twolevel_hint {
    quint32 isub_image : 8, itoc : 24;
};

/* Known values for the platform field above. */
const quint32 S_PLATFORM_MACOS = 1;
const quint32 S_PLATFORM_IOS = 2;
const quint32 S_PLATFORM_TVOS = 3;
const quint32 S_PLATFORM_WATCHOS = 4;
const quint32 S_PLATFORM_BRIDGEOS = 5;
const quint32 S_PLATFORM_MACCATALYST = 6;
const quint32 S_PLATFORM_IOSSIMULATOR = 7;
const quint32 S_PLATFORM_TVOSSIMULATOR = 8;
const quint32 S_PLATFORM_WATCHOSSIMULATOR = 9;
const quint32 S_PLATFORM_DRIVERKIT = 10;
const quint32 S_PLATFORM_VISIONOS = 11;
const quint32 S_PLATFORM_VISIONOSSIMULATOR = 12;
const quint32 S_PLATFORM_FIRMWARE = 13;
const quint32 S_PLATFORM_SEPOS = 14;

/* Known values for the tool field above. */
const quint32 S_TOOL_CLANG = 1;
const quint32 S_TOOL_SWIFT = 2;
const quint32 S_TOOL_LD = 3;

struct dylib_table_of_contents {
    quint32 symbol_index; /* the defined external symbol (index into the symbol
                             table) */
    quint32 module_index; /* index into the module table this symbol is defined
                             in */
};

struct relocation_info {
    quint32 r_address; /* offset in the section to what is being relocated */
    union {
        quint32 value;
        struct {
            quint32 r_symbolnum : 24; /* symbol index if r_extern == 1 or
                                         section ordinal if r_extern == 0 */
            quint32 r_pcrel : 1;      /* was relocated pc relative already */
            quint32 r_length : 2;     /* 0=byte, 1=word, 2=long, 3=quad */
            quint32 r_extern : 1;     /* does not include value of sym referenced */
            quint32 r_type : 4;       /* if not 0, machine specific relocation type */
        } _value;
    } s;
};

/*
 * The entries in the reference symbol table are used when loading the module
 * (both by the static and dynamic link editors) and if the module is unloaded
 * or replaced.  Therefore all external symbols (defined and undefined) are
 * listed in the module's reference table.  The flags describe the type of
 * reference that is being made.  The constants for the flags are defined in
 * <mach-o/nlist.h> as they are also used for symbol table entries.
 */
struct dylib_reference {
    union {
        quint32 value;
        struct {
            quint32 isym : 24; /* index into the symbol table */
            quint32 flags : 8; /* flags to indicate the type of reference */
        } _value;
    } s;
};

// https://llvm.org/doxygen/BinaryFormat_2MachO_8h_source.html
// http://formats.kaitai.io/mach_o/
// https://github.com/phausler/Shinobi/blob/master/include/llvm/Support/MachO.h
// https://gist.github.com/yamaya/2924292
// https://github.com/aidansteele/osx-abi-macho-file-format-reference
// https://code.woboq.org/llvm/lld/lib/ReaderWriter/MachO/MachONormalizedFile.h.html
// https://stuff.mit.edu/afs/sipb/project/gnu/share/gcc-lib/m68k-next-mach31/2.5.4/include/mach/m68k/thread_status.h
// https://github.com/qyang-nj/llios/blob/main/dynamic_linking/chained_fixups.md
enum reloc_type_x86_64 {
    X86_64_RELOC_UNSIGNED,    // for absolute addresses
    X86_64_RELOC_SIGNED,      // for signed 32-bit displacement
    X86_64_RELOC_BRANCH,      // a CALL/JMP instruction with 32-bit displacement
    X86_64_RELOC_GOT_LOAD,    // a MOVQ load of a GOT entry
    X86_64_RELOC_GOT,         // other GOT references
    X86_64_RELOC_SUBTRACTOR,  // must be followed by a X86_64_RELOC_UNSIGNED
    X86_64_RELOC_SIGNED_1,    // for signed 32-bit displacement with a -1 addend
    X86_64_RELOC_SIGNED_2,    // for signed 32-bit displacement with a -2 addend
    X86_64_RELOC_SIGNED_4,    // for signed 32-bit displacement with a -4 addend
};

enum reloc_type_arm {
    ARM_RELOC_VANILLA,        /* generic relocation as discribed above */
    ARM_RELOC_PAIR,           /* the second relocation entry of a pair */
    ARM_RELOC_SECTDIFF,       /* a PAIR follows with subtract symbol value */
    ARM_RELOC_LOCAL_SECTDIFF, /* like ARM_RELOC_SECTDIFF, but the symbol
                                 referenced was local.  */
    ARM_RELOC_PB_LA_PTR,      /* prebound lazy pointer */
    ARM_RELOC_BR24,           /* 24 bit branch displacement (to a word address) */
    ARM_THUMB_RELOC_BR22,     /* 22 bit branch displacement (to a half-word address)
                               */
};

enum reloc_type_ppc {
    PPC_RELOC_VANILLA,       /* generic relocation as discribed above */
    PPC_RELOC_PAIR,          /* the second relocation entry of a pair */
    PPC_RELOC_BR14,          /* 14 bit branch displacement (to a word address) */
    PPC_RELOC_BR24,          /* 24 bit branch displacement (to a word address) */
    PPC_RELOC_HI16,          /* a PAIR follows with the low half */
    PPC_RELOC_LO16,          /* a PAIR follows with the high half */
    PPC_RELOC_HA16,          /* Same as the RELOC_HI16 except the low 16 bits and the
                              * high 16 bits are added together with the low 16 bits
                              * sign extened first.  This means if bit 15 of the low
                              * 16 bits is set the high 16 bits stored in the
                              * instruction will be adjusted.
                              */
    PPC_RELOC_LO14,          /* Same as the LO16 except that the low 2 bits are not
                              * stored in the instruction and are always zero.  This
                              * is used in double word load/store instructions.
                              */
    PPC_RELOC_SECTDIFF,      /* a PAIR follows with subtract symbol value */
    PPC_RELOC_PB_LA_PTR,     /* prebound lazy pointer */
    PPC_RELOC_HI16_SECTDIFF, /* section difference forms of above.  a PAIR */
    PPC_RELOC_LO16_SECTDIFF, /* follows these with subtract symbol value */
    PPC_RELOC_HA16_SECTDIFF,
    PPC_RELOC_JBSR,
    PPC_RELOC_LO14_SECTDIFF,
    PPC_RELOC_LOCAL_SECTDIFF /* like PPC_RELOC_SECTDIFF, but the symbol
                             referenced was local.  */
};

/*
 * The following are used to encode rebasing information
 */
const quint32 S_REBASE_TYPE_POINTER = 1;
const quint32 S_REBASE_TYPE_TEXT_ABSOLUTE32 = 2;
const quint32 S_REBASE_TYPE_TEXT_PCREL32 = 3;

const quint32 S_REBASE_OPCODE_MASK = 0xF0;
const quint32 S_REBASE_IMMEDIATE_MASK = 0x0F;
const quint32 S_REBASE_OPCODE_DONE = 0x00;
const quint32 S_REBASE_OPCODE_SET_TYPE_IMM = 0x10;
const quint32 S_REBASE_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB = 0x20;
const quint32 S_REBASE_OPCODE_ADD_ADDR_ULEB = 0x30;
const quint32 S_REBASE_OPCODE_ADD_ADDR_IMM_SCALED = 0x40;
const quint32 S_REBASE_OPCODE_DO_REBASE_IMM_TIMES = 0x50;
const quint32 S_REBASE_OPCODE_DO_REBASE_ULEB_TIMES = 0x60;
const quint32 S_REBASE_OPCODE_DO_REBASE_ADD_ADDR_ULEB = 0x70;
const quint32 S_REBASE_OPCODE_DO_REBASE_ULEB_TIMES_SKIPPING_ULEB = 0x80;
/*
 * The following are used to encode binding information
 */
const quint32 S_BIND_TYPE_POINTER = 1;
const quint32 S_BIND_TYPE_TEXT_ABSOLUTE32 = 2;
const quint32 S_BIND_TYPE_TEXT_PCREL32 = 3;

const quint32 S_BIND_SPECIAL_DYLIB_SELF = 0;
const quint32 S_BIND_SPECIAL_DYLIB_MAIN_EXECUTABLE = -1;
const quint32 S_BIND_SPECIAL_DYLIB_FLAT_LOOKUP = -2;
const quint32 S_BIND_SPECIAL_DYLIB_WEAK_LOOKUP = -3;

const quint32 S_BIND_SYMBOL_FLAGS_WEAK_IMPORT = 0x1;
const quint32 S_BIND_SYMBOL_FLAGS_NON_WEAK_DEFINITION = 0x8;

const quint32 S_BIND_OPCODE_MASK = 0xF0;
const quint32 S_BIND_IMMEDIATE_MASK = 0x0F;
const quint32 S_BIND_OPCODE_DONE = 0x00;
const quint32 S_BIND_OPCODE_SET_DYLIB_ORDINAL_IMM = 0x10;
const quint32 S_BIND_OPCODE_SET_DYLIB_ORDINAL_ULEB = 0x20;
const quint32 S_BIND_OPCODE_SET_DYLIB_SPECIAL_IMM = 0x30;
const quint32 S_BIND_OPCODE_SET_SYMBOL_TRAILING_FLAGS_IMM = 0x40;
const quint32 S_BIND_OPCODE_SET_TYPE_IMM = 0x50;
const quint32 S_BIND_OPCODE_SET_ADDEND_SLEB = 0x60;
const quint32 S_BIND_OPCODE_SET_SEGMENT_AND_OFFSET_ULEB = 0x70;
const quint32 S_BIND_OPCODE_ADD_ADDR_ULEB = 0x80;
const quint32 S_BIND_OPCODE_DO_BIND = 0x90;
const quint32 S_BIND_OPCODE_DO_BIND_ADD_ADDR_ULEB = 0xA0;
const quint32 S_BIND_OPCODE_DO_BIND_ADD_ADDR_IMM_SCALED = 0xB0;
const quint32 S_BIND_OPCODE_DO_BIND_ULEB_TIMES_SKIPPING_ULEB = 0xC0;
const quint32 S_BIND_OPCODE_THREADED = 0xD0;
const quint32 S_BIND_SUBOPCODE_THREADED_SET_BIND_ORDINAL_TABLE_SIZE_ULEB = 0x00;
const quint32 S_BIND_SUBOPCODE_THREADED_APPLY = 0x01;
/*
 * The following are used on the flags byte of a terminal node
 * in the export information.
 */
const quint32 S_EXPORT_SYMBOL_FLAGS_KIND_MASK = 0x03;
const quint32 S_EXPORT_SYMBOL_FLAGS_KIND_REGULAR = 0x00;
const quint32 S_EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL = 0x01;
const quint32 S_EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE = 0x02;
const quint32 S_EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION = 0x04;
const quint32 S_EXPORT_SYMBOL_FLAGS_REEXPORT = 0x08;
const quint32 S_EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER = 0x10;

const quint32 S_CSMAGIC_REQUIREMENT = 0xfade0c00;            /* single Requirement blob */
const quint32 S_CSMAGIC_REQUIREMENTS = 0xfade0c01;           /* Requirements vector (internal requirements) */
const quint32 S_CSMAGIC_CODEDIRECTORY = 0xfade0c02;          /* CodeDirectory blob */
const quint32 S_CSMAGIC_EMBEDDED_SIGNATURE = 0xfade0cc0;     /* embedded form of signature data */
const quint32 S_CSMAGIC_EMBEDDED_SIGNATURE_OLD = 0xfade0b02; /* XXX */
const quint32 S_CSMAGIC_EMBEDDED_ENTITLEMENTS = 0xfade7171;  /* embedded entitlements */
const quint32 S_CSMAGIC_DETACHED_SIGNATURE = 0xfade0cc1;     /* multi-arch collection of embedded signatures */
const quint32 S_CSMAGIC_BLOBWRAPPER = 0xfade0b01;            /* CMS Signature, among other things */

struct __CodeDirectory {
    quint32 magic;         /* magic number (CSMAGIC_CODEDIRECTORY) */
    quint32 length;        /* total length of CodeDirectory blob */
    quint32 version;       /* compatibility version */
    quint32 flags;         /* setup and mode flags */
    quint32 hashOffset;    /* offset of hash slot element at index zero */
    quint32 identOffset;   /* offset of identifier string */
    quint32 nSpecialSlots; /* number of special hash slots */
    quint32 nCodeSlots;    /* number of ordinary (code) hash slots */
    quint32 codeLimit;     /* limit to main image signature range */
    quint32 hashSize;      /* size of each hash in bytes */
    quint32 hashType;      /* type of hash (cdHashType* constants) */
    quint32 platform;      /* platform identifier; zero if not platform binary */
    quint32 pageSize;      /* log2(page size in bytes); 0 => infinite */
    quint32 spare2;        /* unused (must be zero) */
    /* Version 0x20100 */
    quint32 scatterOffset; /* offset of optional scatter vector */
    /* Version 0x20200 */
    quint32 teamOffset; /* offset of optional team identifier */
    /* followed by dynamic content as located by offset fields above */
};

/*
 * Structure of an embedded-signature SuperBlob
 */

struct CS_BlobIndex {
    quint32 type;   /* type of entry */
    quint32 offset; /* offset of entry */
};

struct __SC_SuperBlob {
    quint32 magic;  /* magic number */
    quint32 length; /* total length of SuperBlob */
    quint32 count;  /* number of index entries following */
    //	CS_BlobIndex index[];           /* (count) entries */
    /* followed by Blobs in no particular order as indicated by offsets in index
     */
};

const quint32 S_CSSLOT_CODEDIRECTORY = 0;
const quint32 S_CSSLOT_INFOSLOT = 1;
const quint32 S_CSSLOT_REQUIREMENTS = 2;
const quint32 S_CSSLOT_RESOURCEDIR = 3;
const quint32 S_CSSLOT_APPLICATION = 4;
const quint32 S_CSSLOT_ENTITLEMENTS = 5;
// https://github.com/apple/darwin-xnu/blob/2ff845c2e033bd0ff64b5b6aa6063a1f8f65aa32/osfmk/kern/cs_blobs.h#L92
struct CS_CodeDirectory {
    quint32 magic;         /* magic number (CSMAGIC_CODEDIRECTORY) */
    quint32 length;        /* total length of CodeDirectory blob */
    quint32 version;       /* compatibility version */
    quint32 flags;         /* setup and mode flags */
    quint32 hashOffset;    /* offset of hash slot element at index zero */
    quint32 identOffset;   /* offset of identifier string */
    quint32 nSpecialSlots; /* number of special hash slots */
    quint32 nCodeSlots;    /* number of ordinary (code) hash slots */
    quint32 codeLimit;     /* limit to main image signature range */
    quint8 hashSize;       /* size of each hash in bytes */
    quint8 hashType;       /* type of hash (cdHashType* constants) */
    quint8 platform;       /* platform identifier; zero if not platform binary */
    quint8 pageSize;       /* log2(page size in bytes); 0 => infinite */
    quint32 spare2;        /* unused (must be zero) */
    /* followed by dynamic content as located by offset fields above */
    /* Version 0x20100 */
    quint32 scatterOffset; /* offset of optional scatter vector */
    /* Version 0x20200 */
    quint32 teamOffset; /* offset of optional team identifier */
    /* Version 0x20300 */
    quint32 spare3;      /* unused (must be zero) */
    quint64 codeLimit64; /* limit to main image signature range, 64 bits */
    /* Version 0x20400 */
    quint64 execSegBase;  /* offset of executable segment */
    quint64 execSegLimit; /* limit of executable segment */
    quint64 execSegFlags; /* executable segment flags */
    /* Version 0x20500 */
    quint32 runtime;
    quint32 preEncryptOffset;
    /* Version 0x20600 */
    quint8 linkageHashType;
    quint8 linkageTruncated;
    quint16 spare4;
    quint32 linkageOffset;
    quint32 linkageSize;
};

const quint32 S_DYLD_CHAINED_IMPORT = 1;
const quint32 S_DYLD_CHAINED_IMPORT_ADDEND = 2;
const quint32 S_DYLD_CHAINED_IMPORT_ADDEND64 = 3;

// header of the LC_DYLD_CHAINED_FIXUPS payload
struct dyld_chained_fixups_header {
    quint32 fixups_version;  // 0
    quint32 starts_offset;   // offset of dyld_chained_starts_in_image in chain_data
    quint32 imports_offset;  // offset of imports table in chain_data
    quint32 symbols_offset;  // offset of symbol strings in chain_data
    quint32 imports_count;   // number of imported symbol names
    quint32 imports_format;  // DYLD_CHAINED_IMPORT*
    quint32 symbols_format;  // 0 => uncompressed, 1 => zlib compressed
};

struct dyld_chained_starts_in_image {
    quint32 seg_count;
    quint32 seg_info_offset[1];  // each entry is offset into this struct for that segment
    // followed by pool of dyld_chain_starts_in_segment data
};

// This struct is embedded in dyld_chain_starts_in_image
// and passed down to the kernel for page-in linking
struct dyld_chained_starts_in_segment {
    quint32 size;               // size of this (amount kernel needs to copy)
    quint16 page_size;          // 0x1000 or 0x4000
    quint16 pointer_format;     // DYLD_CHAINED_PTR_*
    quint64 segment_offset;     // offset in memory to start of segment
    quint32 max_valid_pointer;  // for 32-bit OS, any value beyond this is not a pointer
    quint16 page_count;         // how many pages are in array
    quint16 page_start[1];      // each entry is offset in each page of first element in chain
                                // or DYLD_CHAINED_PTR_START_NONE if no fixups on page
                                // uint16_t    chain_starts[1];    // some 32-bit formats may require multiple starts per page.
                                // for those, if high bit is set in page_starts[], then it
                                // is index into chain_starts[] which is a list of starts
                                // the last of which has the high bit set
};

// DYLD_CHAINED_IMPORT
struct dyld_chained_import {
    quint32 lib_ordinal : 8, weak_import : 1, name_offset : 23;
};

// DYLD_CHAINED_IMPORT_ADDEND
struct dyld_chained_import_addend {
    quint32 lib_ordinal : 8, weak_import : 1, name_offset : 23;
    qint32 addend;
};

// DYLD_CHAINED_IMPORT_ADDEND64
struct dyld_chained_import_addend64 {
    quint64 lib_ordinal : 16, weak_import : 1, reserved : 15, name_offset : 32;
    quint64 addend;
};

}  // namespace XMACH_DEF

#endif  // QMACH_DEF_H
