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
#ifndef XMSDOS_DEF_H
#define XMSDOS_DEF_H

#include <QtGlobal>

namespace XMSDOS_DEF {
const quint16 S_IMAGE_DOS_SIGNATURE = 0x5A4D;     // MZ
const quint16 S_IMAGE_DOS_SIGNATURE_MZ = 0x5A4D;  // MZ
const quint16 S_IMAGE_DOS_SIGNATURE_ZM = 0x4D5A;  // ZM

struct EXE_file {
    quint16 exe_signature;    // must contain 4D5A  (yay zibo!)
    quint16 exe_len_mod_512;  // low 9 bits of length
    quint16 exe_pages;        // number of 512b pages in file
    quint16 exe_rle_count;    // count of reloc entries
    quint16 exe_par_dir;      // number of paragraphs before image
    quint16 exe_min_BSS;      // minimum number of para of BSS
    quint16 exe_max_BSS;      // max number of para of BSS
    quint16 exe_SS;           // stack of image
    quint16 exe_SP;           // SP of image
    quint16 exe_chksum;       // checksum  of file (ignored)
    quint16 exe_IP;           // IP of entry
    quint16 exe_CS;           // CS of entry
    quint16 exe_rle_table;    // byte offset of reloc table
    quint16 exe_iov;          // overlay number (0 for root)
    quint32 exe_sym_tab;      // offset of symbol table in file
};

struct IMAGE_DOS_HEADER {
    quint16 e_magic;    /* 00: MZ Header signature */
    quint16 e_cblp;     /* 02: Bytes on last page of file */
    quint16 e_cp;       /* 04: Pages in file */
    quint16 e_crlc;     /* 06: Relocations */
    quint16 e_cparhdr;  /* 08: Size of header in paragraphs */
    quint16 e_minalloc; /* 0a: Minimum extra paragraphs needed */
    quint16 e_maxalloc; /* 0c: Maximum extra paragraphs needed */
    quint16 e_ss;       /* 0e: Initial (relative) SS value */
    quint16 e_sp;       /* 10: Initial SP value */
    quint16 e_csum;     /* 12: Checksum */
    quint16 e_ip;       /* 14: Initial IP value */
    quint16 e_cs;       /* 16: Initial (relative) CS value */
    quint16 e_lfarlc;   /* 18: File address of relocation table */
    quint16 e_ovno;     /* 1a: Overlay number */
};

struct IMAGE_DOS_HEADEREX {
    quint16 e_magic;    /* 00: MZ Header signature */
    quint16 e_cblp;     /* 02: Bytes on last page of file */
    quint16 e_cp;       /* 04: Pages in file */
    quint16 e_crlc;     /* 06: Relocations */
    quint16 e_cparhdr;  /* 08: Size of header in paragraphs */
    quint16 e_minalloc; /* 0a: Minimum extra paragraphs needed */
    quint16 e_maxalloc; /* 0c: Maximum extra paragraphs needed */
    quint16 e_ss;       /* 0e: Initial (relative) SS value */
    quint16 e_sp;       /* 10: Initial SP value */
    quint16 e_csum;     /* 12: Checksum */
    quint16 e_ip;       /* 14: Initial IP value */
    quint16 e_cs;       /* 16: Initial (relative) CS value */
    quint16 e_lfarlc;   /* 18: File address of relocation table */
    quint16 e_ovno;     /* 1a: Overlay number */
    // Extra
    quint16 e_res[4];   /* 1c: Reserved words */
    quint16 e_oemid;    /* 24: OEM identifier (for e_oeminfo) */
    quint16 e_oeminfo;  /* 26: OEM information; e_oemid specific */
    quint16 e_res2[10]; /* 28: Reserved words */
    qint32 e_lfanew;    /* 3c: Offset to extended header */
};

struct dos16m_exe_header {
    quint16 signature;       /* BW signature to mark valid file  */
    quint16 last_page_bytes; /* length of image mod 512          */
    quint16 pages_in_file;   /* number of 512 byte pages         */
    quint16 reserved1;
    quint16 reserved2;
    quint16 min_alloc;        /* required memory, in KB           */
    quint16 max_alloc;        /* max KB (private allocation)      */
    quint16 stack_seg;        /* segment of stack                 */
    quint16 stack_ptr;        /* initial SP value                 */
    quint16 first_reloc_sel;  /* huge reloc list selector         */
    quint16 init_ip;          /* initial IP value                 */
    quint16 code_seg;         /* segment of code                  */
    quint16 runtime_gdt_size; /* runtime GDT size in bytes        */
    quint16 MAKEPM_version;   /* ver * 100, GLU = (ver+10)*100    */
    /* end of DOS style EXE header */
    quint32 next_header_pos; /* file pos of next spliced .EXP    */
    quint32 cv_info_offset;  /* offset to start of debug info    */
    quint16 last_sel_used;   /* last selector value used         */
    quint16 pmem_alloc;      /* private xm amount KB if nonzero  */
    quint16 alloc_incr;      /* auto ExtReserve amount, in KB    */
    quint8 reserved4[6];
    /* the following used to be referenced as gdtimage[0..1] */
    quint16 options;         /* runtime options                  */
    quint16 trans_stack_sel; /* sel of transparent stack         */
    quint16 exp_flags;       /* see ef_ constants below          */
    quint16 program_size;    /* size of program in paras         */
    quint16 gdtimage_size;   /* size of gdt in file (bytes)      */
    quint16 first_selector;  /* gdt[first_sel] = gdtimage[0], 0 => 0x80 */
    quint8 default_mem_strategy;
    quint8 reserved5;
    quint16 transfer_buffer_size; /* default in bytes, 0 => 8KB   */
    /* the following used to be referenced as gdtimage[2..15] */
    quint8 reserved6[48];
    char EXP_path[64]; /* original .EXP file name  */
    /* gdtimage[16..gdtimage_size] follows immediately, then program image follows */
};
}  // namespace XMSDOS_DEF
#endif  // XMSDOS_DEF_H
