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
#ifndef XLE_DEF_H
#define XLE_DEF_H

#include <QtGlobal>

namespace XLE_DEF {
const quint32 S_IMAGE_VXD_SIGNATURE = 0x454C;  // LE
const quint32 S_IMAGE_LX_SIGNATURE = 0x584C;   // LX

struct IMAGE_VXD_HEADER  // Windows VXD header
{
    quint16 e32_magic;         // Magic number
    quint8 e32_border;         // The byte ordering for the VXD
    quint8 e32_worder;         // The word ordering for the VXD
    quint32 e32_level;         // The EXE format level for now = 0
    quint16 e32_cpu;           // The CPU type
    quint16 e32_os;            // The OS type
    quint32 e32_ver;           // Module version
    quint32 e32_mflags;        // Module flags
    quint32 e32_mpages;        // Module # pages
    quint32 e32_startobj;      // Object # for instruction pointer
    quint32 e32_eip;           // Extended instruction pointer
    quint32 e32_stackobj;      // Object # for stack pointer
    quint32 e32_esp;           // Extended stack pointer
    quint32 e32_pagesize;      // VXD page size
    quint32 e32_lastpagesize;  // Last page size in VXD
    quint32 e32_fixupsize;     // Fixup section size
    quint32 e32_fixupsum;      // Fixup section checksum
    quint32 e32_ldrsize;       // Loader section size
    quint32 e32_ldrsum;        // Loader section checksum
    quint32 e32_objtab;        // Object table offset
    quint32 e32_objcnt;        // Number of objects in module
    quint32 e32_objmap;        // Object page map offset
    quint32 e32_itermap;       // Object iterated data map offset
    quint32 e32_rsrctab;       // Offset of Resource Table
    quint32 e32_rsrccnt;       // Number of resource entries
    quint32 e32_restab;        // Offset of resident name table
    quint32 e32_enttab;        // Offset of Entry Table
    quint32 e32_dirtab;        // Offset of Module Directive Table
    quint32 e32_dircnt;        // Number of module directives
    quint32 e32_fpagetab;      // Offset of Fixup Page Table
    quint32 e32_frectab;       // Offset of Fixup Record Table
    quint32 e32_impmod;        // Offset of Import Module Name Table
    quint32 e32_impmodcnt;     // Number of entries in Import Module Name Table
    quint32 e32_impproc;       // Offset of Import Procedure Name Table
    quint32 e32_pagesum;       // Offset of Per-Page Checksum Table
    quint32 e32_datapage;      // Offset of Enumerated Data Pages
    quint32 e32_preload;       // Number of preload pages
    quint32 e32_nrestab;       // Offset of Non-resident Names Table
    quint32 e32_cbnrestab;     // Size of Non-resident Name Table
    quint32 e32_nressum;       // Non-resident Name Table Checksum
    quint32 e32_autodata;      // Object # for automatic data object
    quint32 e32_debuginfo;     // Offset of the debugging information
    quint32 e32_debuglen;      // The length of the debugging info. in bytes
    quint32 e32_instpreload;   // Number of instance pages in preload section of
                               // VXD file
    quint32 e32_instdemand;    // Number of instance pages in demand load section
                               // of VXD file
    quint32 e32_heapsize;      // Size of heap - for 16-bit apps
    //    BYTE   e32_res3[12];              // Reserved words
    //    DWORD  e32_winresoff;
    //    DWORD  e32_winreslen;
    //    WORD   e32_devid;                 // Device ID for VxD
    //    WORD   e32_ddkver;                // DDK version for VxD
};

struct o32_obj {
    quint32 o32_size;
    quint32 o32_base;
    quint32 o32_flags;
    quint32 o32_pagemap;  // index in map
    quint32 o32_mapsize;  // number of entry
    quint32 o32_reserved;
};

// struct o32_map
//{
//     quint32 o32_pagedataoffset;
//     quint16 o32_pagesize;
//     quint16 o32_pageflags;
// };

struct o16_map {
    quint8 o16_pagenum[3];
    quint8 o16_pageflags;
};

struct o32_map {
    quint32 o32_pagedataoffset;
    quint16 o32_pagesize;
    quint16 o32_pageflags;
};
}  // namespace XLE_DEF
#endif  // XLE_DEF_H
