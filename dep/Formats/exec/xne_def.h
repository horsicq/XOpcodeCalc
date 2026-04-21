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
#ifndef XNE_DEF_H
#define XNE_DEF_H

#include <QtGlobal>

namespace XNE_DEF {
const quint16 S_IMAGE_OS2_SIGNATURE = 0x454E;  // NE

const quint16 FLAG_8086 = 0x0010;
const quint16 FLAG_80286 = 0x0020;
const quint16 FLAG_80386 = 0x0040;

struct IMAGE_OS2_HEADER  // OS/2 .EXE header
{
    quint16 ne_magic;         // Magic number
    quint8 ne_ver;            // Version number
    quint8 ne_rev;            // Revision number
    quint16 ne_enttab;        // Offset of Entry Table
    quint16 ne_cbenttab;      // Number of bytes in Entry Table
    quint32 ne_crc;           // Checksum of whole file
    quint16 ne_flags;         // Flag word
    quint16 ne_autodata;      // Automatic data segment number
    quint16 ne_heap;          // Initial heap allocation
    quint16 ne_stack;         // Initial stack allocation
    quint32 ne_csip;          // Initial CS:IP setting
    quint32 ne_sssp;          // Initial SS:SP setting
    quint16 ne_cseg;          // Count of file segments
    quint16 ne_cmod;          // Entries in Module Reference Table
    quint16 ne_cbnrestab;     // Size of non-resident name table
    quint16 ne_segtab;        // Offset of Segment Table
    quint16 ne_rsrctab;       // Offset of Resource Table
    quint16 ne_restab;        // Offset of resident name table
    quint16 ne_modtab;        // Offset of Module Reference Table
    quint16 ne_imptab;        // Offset of Imported Names Table
    quint32 ne_nrestab;       // Offset of Non-resident Names Table
    quint16 ne_cmovent;       // Count of movable entries
    quint16 ne_align;         // Segment alignment shift count
    quint16 ne_cres;          // Count of resource segments
    quint8 ne_exetyp;         // Target Operating system
    quint8 ne_flagsothers;    // Other .EXE flags
    quint16 ne_pretthunks;    // offset to return thunks
    quint16 ne_psegrefbytes;  // offset to segment ref. bytes
    quint16 ne_swaparea;      // Minimum code swap area size
    quint16 ne_expver;        // Expected Windows version number
};

struct NE_SEGMENT {
    quint16 dwFileOffset;
    quint16 dwFileSize;  // 0 -> 64K
    quint16 dwFlags;
    quint16 dwMinAllocSize;  // 0 -> 64K
};
}  // namespace XNE_DEF

#endif  // XNE_DEF_H
