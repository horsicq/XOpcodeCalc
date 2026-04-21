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
#include "xne.h"

XBinary::XCONVERT _TABLE_XNE_STRUCTID[] = {
    {XNE::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XNE::STRUCTID_IMAGE_DOS_HEADER, "IMAGE_DOS_HEADER", QString("IMAGE_DOS_HEADER")},
    {XNE::STRUCTID_IMAGE_DOS_HEADEREX, "IMAGE_DOS_HEADER", QString("IMAGE_DOS_HEADER")},
    {XNE::STRUCTID_IMAGE_OS2_HEADER, "IMAGE_OS2_HEADER", QString("IMAGE_OS2_HEADER")},
    {XNE::STRUCTID_ENTRY_TABLE, "ENTRY_TABLE", QString("Entry table")},
    {XNE::STRUCTID_SEGMENT_TABLE, "SEGMENT_TABLE", QString("Segment table")},
    {XNE::STRUCTID_RESOURCE_TABLE, "RESOURCE_TABLE", QString("Resource table")},
    {XNE::STRUCTID_RESIDENT_NAME_TABLE, "RESIDENT_NAME_TABLE", QString("Resident name table")},
    {XNE::STRUCTID_MODULE_REFERENCE_TABLE, "MODULE_REFERENCE_TABLE", QString("Module reference table")},
    {XNE::STRUCTID_IMPORTED_NAMES_TABLE, "IMPORTED_NAMES_TABLE", QString("Imported names table")},
    {XNE::STRUCTID_NONRESIDENT_NAME_TABLE, "NONRESIDENT_NAME_TABLE", QString("Non-resident name table")},
};

XBinary::XIDSTRING _TABLE_XNE_ImageNEMagics[] = {
    {0x454E, "OS2_SIGNATURE"},
};

XBinary::XIDSTRING _TABLE_XNE_ImageNEFlags[] = {
    {0x0001, "single shared"},
    {0x0002, "multiple"},
    {0x0004, "Global initialization"},
    {0x0008, "Protected mode only"},
    {0x0010, "8086 instructions"},
    {0x0020, "80286 instructions"},
    {0x0040, "80386 instructions"},
    {0x0080, "80x87 instructions"},
    {0x0100, "Full screen"},
    {0x0200, "Compatible with Windows/P.M."},
    {0x0800, "OS/2 family application"},
    {0x1000, "reserved?"},
    {0x2000, "Errors in image/executable"},
    {0x4000, "non-conforming program"},
    {0x8000, "DLL or driver"},
};

XBinary::XIDSTRING _TABLE_XNE_ImageNEExetypes[] = {
    {0x0000, "Unknown"},
    {0x0001, "OS/2"},
    {0x0002, "Windows"},
    {0x0003, "European MS-DOS 4.x"},
    {0x0004, "Windows 386"},
    {0x0005, "BOSS (Borland Operating System Services)"},
};

XBinary::XIDSTRING _TABLE_XNE_ImageNEFlagsothers[] = {
    {0x0001, "Long filename support"},
    {0x0002, "2.x protected mode"},
    {0x0004, "2.x proportional fonts"},
    {0x0008, "Executable has gangload area"},
};

XBinary::XIDSTRING _TABLE_XNE_ImageSegmentTypes[] = {
    {0x0000, "CODE"},
    {0x0001, "DATA"},
};

const QString XNE::PREFIX_ImageNEMagics = "IMAGE_";

XNE::XNE(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XMSDOS(pDevice, bIsImage, nModuleAddress)
{
}

bool XNE::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    quint16 magic = get_magic();

    if (magic == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_MZ) {
        qint32 lfanew = get_lfanew();
        qint64 nSize = getSize();

        if ((lfanew > 0) && (lfanew < (nSize & 0xFFFFFFFF))) {
            quint32 signature = read_uint16(lfanew);

            if (signature == XNE_DEF::S_IMAGE_OS2_SIGNATURE) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XNE::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XNE xne(pDevice, bIsImage, nModuleAddress);

    return xne.isValid();
}

XBinary::MODE XNE::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XNE xne(pDevice, bIsImage, nModuleAddress);

    return xne.getMode();
}

qint64 XNE::getImageOS2HeaderOffset()
{
    qint64 nResult = get_lfanew();

    if (!_isOffsetValid(nResult)) {
        nResult = -1;
    }

    return nResult;
}

qint64 XNE::getImageOS2HeaderSize()
{
    return sizeof(XNE_DEF::IMAGE_OS2_HEADER);
}

XNE_DEF::IMAGE_OS2_HEADER XNE::getImageOS2Header()
{
    XNE_DEF::IMAGE_OS2_HEADER result = {};

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        result.ne_magic = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic));
        result.ne_ver = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver));
        result.ne_rev = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev));
        result.ne_enttab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab));
        result.ne_cbenttab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab));
        result.ne_crc = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc));
        result.ne_flags = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags));
        result.ne_autodata = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata));
        result.ne_heap = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap));
        result.ne_stack = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack));
        result.ne_csip = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip));
        result.ne_sssp = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp));
        result.ne_cseg = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg));
        result.ne_cmod = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod));
        result.ne_cbnrestab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab));
        result.ne_segtab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab));
        result.ne_rsrctab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab));
        result.ne_restab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab));
        result.ne_modtab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab));
        result.ne_imptab = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab));
        result.ne_nrestab = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab));
        result.ne_cmovent = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent));
        result.ne_align = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align));
        result.ne_cres = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres));
        result.ne_exetyp = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp));
        result.ne_flagsothers = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers));
        result.ne_pretthunks = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks));
        result.ne_psegrefbytes = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes));
        result.ne_swaparea = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea));
        result.ne_expver = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver));
    }

    return result;
}

quint16 XNE::getImageOS2Header_magic()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_ver()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_rev()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_enttab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cbenttab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_crc()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_flags()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_autodata()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_heap()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_stack()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_csip()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_sssp()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cseg()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cmod()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cbnrestab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_segtab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_rsrctab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_restab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_modtab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_imptab()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab));
    }

    return nResult;
}

quint32 XNE::getImageOS2Header_nrestab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cmovent()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_align()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_cres()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_exetyp()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp));
    }

    return nResult;
}

quint8 XNE::getImageOS2Header_flagsothers()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_pretthunks()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_psegrefbytes()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_swaparea()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea));
    }

    return nResult;
}

quint16 XNE::getImageOS2Header_expver()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver));
    }

    return nResult;
}

void XNE::setImageOS2Header_magic(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic), nValue);
    }
}

void XNE::setImageOS2Header_ver(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver), nValue);
    }
}

void XNE::setImageOS2Header_rev(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev), nValue);
    }
}

void XNE::setImageOS2Header_enttab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab), nValue);
    }
}

void XNE::setImageOS2Header_cbenttab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab), nValue);
    }
}

void XNE::setImageOS2Header_crc(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc), nValue);
    }
}

void XNE::setImageOS2Header_flags(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags), nValue);
    }
}

void XNE::setImageOS2Header_autodata(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata), nValue);
    }
}

void XNE::setImageOS2Header_heap(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap), nValue);
    }
}

void XNE::setImageOS2Header_stack(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack), nValue);
    }
}

void XNE::setImageOS2Header_csip(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip), nValue);
    }
}

void XNE::setImageOS2Header_sssp(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp), nValue);
    }
}

void XNE::setImageOS2Header_cseg(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg), nValue);
    }
}

void XNE::setImageOS2Header_cmod(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod), nValue);
    }
}

void XNE::setImageOS2Header_cbnrestab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab), nValue);
    }
}

void XNE::setImageOS2Header_segtab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab), nValue);
    }
}

void XNE::setImageOS2Header_rsrctab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab), nValue);
    }
}

void XNE::setImageOS2Header_restab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab), nValue);
    }
}

void XNE::setImageOS2Header_modtab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab), nValue);
    }
}

void XNE::setImageOS2Header_imptab(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab), nValue);
    }
}

void XNE::setImageOS2Header_nrestab(quint32 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab), nValue);
    }
}

void XNE::setImageOS2Header_cmovent(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent), nValue);
    }
}

void XNE::setImageOS2Header_align(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align), nValue);
    }
}

void XNE::setImageOS2Header_cres(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres), nValue);
    }
}

void XNE::setImageOS2Header_exetyp(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp), nValue);
    }
}

void XNE::setImageOS2Header_flagsothers(quint8 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers), nValue);
    }
}

void XNE::setImageOS2Header_pretthunks(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks), nValue);
    }
}

void XNE::setImageOS2Header_psegrefbytes(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes), nValue);
    }
}

void XNE::setImageOS2Header_swaparea(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea), nValue);
    }
}

void XNE::setImageOS2Header_expver(quint16 nValue)
{
    qint64 nOffset = getImageOS2HeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver), nValue);
    }
}

qint64 XNE::getEntryTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_enttab();
}

qint64 XNE::getEntryTableSize()
{
    return getImageOS2Header_cbenttab();
}

qint64 XNE::getSegmentTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_segtab();
}

qint64 XNE::getResourceTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_rsrctab();
}

qint64 XNE::getResidentNameTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_restab();
}

qint64 XNE::getModuleReferenceTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_modtab();
}

qint64 XNE::getImportedNamesTableOffset()
{
    return getImageOS2HeaderOffset() + getImageOS2Header_imptab();
}

qint64 XNE::getNotResindentNameTableOffset()
{
    return getImageOS2Header_nrestab();
}

QList<XNE_DEF::NE_SEGMENT> XNE::getSegmentList()
{
    QList<XNE_DEF::NE_SEGMENT> listResult;

    qint64 nOffset = getSegmentTableOffset();
    qint32 nNumberOfSegments = getImageOS2Header_cseg();

    for (qint32 i = 0; i < nNumberOfSegments; i++) {
        XNE_DEF::NE_SEGMENT segment = _read_NE_SEGMENT(nOffset);

        listResult.append(segment);

        nOffset += sizeof(XNE_DEF::NE_SEGMENT);
    }

    return listResult;
}

XNE_DEF::NE_SEGMENT XNE::_read_NE_SEGMENT(qint64 nOffset)
{
    XNE_DEF::NE_SEGMENT result = {};

    result.dwFileOffset = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFileOffset));
    result.dwFileSize = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFileSize));
    result.dwFlags = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwFlags));
    result.dwMinAllocSize = read_uint16(nOffset + offsetof(XNE_DEF::NE_SEGMENT, dwMinAllocSize));

    return result;
}

XBinary::_MEMORY_MAP XNE::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    XBinary::_MEMORY_MAP result = {};

    // Default to segmented view
    if (mapMode == MAPMODE_UNKNOWN) {
        mapMode = MAPMODE_SEGMENTS;
    }

    if (mapMode == MAPMODE_SEGMENTS) {
        result = _getMemoryMap(FILEPART_HEADER | FILEPART_SEGMENT | FILEPART_OVERLAY, pPdStruct);
    }

    return result;
}

QMap<quint64, QString> XNE::getImageNEMagics()
{
    return XBinary::XIDSTRING_createMapPrefix(_TABLE_XNE_ImageNEMagics, sizeof(_TABLE_XNE_ImageNEMagics) / sizeof(XBinary::XIDSTRING), PREFIX_ImageNEMagics);
}

QMap<quint64, QString> XNE::getImageNEMagicsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XNE_ImageNEMagics, sizeof(_TABLE_XNE_ImageNEMagics) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XNE::getImageNEFlagsS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XNE_ImageNEFlags, sizeof(_TABLE_XNE_ImageNEFlags) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XNE::getImageNEExetypesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XNE_ImageNEExetypes, sizeof(_TABLE_XNE_ImageNEExetypes) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XNE::getImageNEFlagsothersS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XNE_ImageNEFlagsothers, sizeof(_TABLE_XNE_ImageNEFlagsothers) / sizeof(XBinary::XIDSTRING));
}

QMap<quint64, QString> XNE::getImageSegmentTypesS()
{
    return XBinary::XIDSTRING_createMap(_TABLE_XNE_ImageSegmentTypes, sizeof(_TABLE_XNE_ImageSegmentTypes) / sizeof(XBinary::XIDSTRING));
}

qint64 XNE::getModuleAddress()
{
    return 0x10000;
}

XBinary::MODE XNE::getMode()
{
    return MODE_16SEG;
}

QString XNE::getArch()
{
    QString sResult = QString("286");

    quint16 nOS = getImageOS2Header_exetyp();

    if (nOS == 4) {
        sResult = "386";
    } else if (nOS == 5) {
        sResult = "386";
    } else if (nOS == 0x81) {
        sResult = "286";
    } else if (nOS == 0x82) {
        sResult = "286";
    }

    return sResult;
}

XBinary::ENDIAN XNE::getEndian()
{
    return ENDIAN_LITTLE;
}

XBinary::FT XNE::getFileType()
{
    return FT_NE;
}

qint32 XNE::getType()
{
    TYPE result = TYPE_EXE;

    quint16 nFlags = getImageOS2Header_flags();

    // If library/driver bit set in NE flags
    if (nFlags & 0x8000) {
        // Heuristic: scan non-resident name table for the word 'driver'
        // If found, treat as DRIVER; if 'font' present, treat as FONT; otherwise classify as DLL
        bool bIsDriver = false;
        bool bIsFont = false;
        qint64 nNRTableOffset = getNotResindentNameTableOffset();
        quint16 nNRTableSize = getImageOS2Header_cbnrestab();

        if ((nNRTableOffset > 0) && (nNRTableSize > 0) && checkOffsetSize({nNRTableOffset, nNRTableSize})) {
            qint64 nPos = nNRTableOffset;
            qint64 nEnd = nNRTableOffset + nNRTableSize;
            while (nPos < nEnd) {
                quint8 nLen = read_uint8(nPos);
                nPos += 1;
                if (nLen == 0) {
                    break;
                }
                QString sName = read_ansiString(nPos, qMin<qint64>(nLen, 255));
                if (sName.contains("driver", Qt::CaseInsensitive)) {
                    bIsDriver = true;
                    break;
                }
                if (sName.contains("font", Qt::CaseInsensitive)) {
                    bIsFont = true;
                    // don't break yet; prefer 'driver' if both appear, but mark font
                }
                // Skip name bytes and trailing ordinal (2 bytes)
                nPos += nLen + 2;
            }
        }

        if (bIsDriver) {
            result = TYPE_DRIVER;
        } else if (bIsFont) {
            result = TYPE_FONT;
        } else {
            result = TYPE_DLL;
        }
    }

    return result;
}

qint64 XNE::getImageSize()
{
    // Sum of segment virtual sizes in NE: N * 0x10000
    qint32 nNumberOfSegments = qMax<qint32>(1, getImageOS2Header_cseg());
    return (qint64)nNumberOfSegments * 0x10000;
}

QString XNE::getOsVersion()
{
    QString sResult = "";

    quint16 nOS = getImageOS2Header_exetyp();

    if (nOS == 3) {
        sResult = "4.x";
    } else if (nOS == 0x81) {
        sResult = "PharLap Dos Extender";
    } else if (nOS == 0x82) {
        sResult = "PharLap Dos Extender";
    }

    return sResult;
}

XBinary::OSNAME XNE::getOsName()
{
    OSNAME result = OSNAME_UNKNOWN;

    quint16 nOS = getImageOS2Header_exetyp();

    if (nOS == 1) {
        result = OSNAME_OS2;
    } else if (nOS == 2) {
        result = OSNAME_WINDOWS;
    } else if (nOS == 3) {
        result = OSNAME_MSDOS;
    } else if (nOS == 4) {
        result = OSNAME_WINDOWS;
    } else if (nOS == 5) {
        result = OSNAME_BORLANDOSSERVICES;
    } else if (nOS == 0x81) {
        result = OSNAME_OS2;
    } else if (nOS == 0x82) {
        result = OSNAME_WINDOWS;
    }

    return result;
}

QString XNE::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_EXE: sResult = QString("EXE"); break;
        case TYPE_DLL: sResult = QString("DLL"); break;
        case TYPE_DRIVER: sResult = QString("Driver"); break;
        case TYPE_FONT: sResult = QString("Font"); break;
    }

    return sResult;
}

QString XNE::getFileFormatExtsString()
{
    return QString("ne");
}

QList<XBinary::MAPMODE> XNE::getMapModesList()
{
    QList<MAPMODE> listResult;
    listResult.append(MAPMODE_SEGMENTS);
    return listResult;
}

XADDR XNE::_getEntryPointAddress()
{
    // CS:IP encoded in ne_csip
    XADDR nModule = getModuleAddress();
    quint32 csip = getImageOS2Header_csip();
    quint16 ip = (quint16)(csip & 0xFFFF);
    quint16 cs = (quint16)((csip >> 16) & 0xFFFF);
    return nModule + ((XADDR)cs) * 0x10000 + ip;
}

QString XNE::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XNE_STRUCTID, sizeof(_TABLE_XNE_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::DATA_HEADER> XNE::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        // Root: add defaults and DOS header, then NE header at e_lfanew with children
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;
        _dataHeadersOptions.fileType = dataHeadersOptions.pMemoryMap->fileType;

        // Append DOS header (extended) via base implementation
        {
            DATA_HEADERS_OPTIONS dosOpts = _dataHeadersOptions;
            dosOpts.nID = XMSDOS::STRUCTID_IMAGE_DOS_HEADEREX;
            dosOpts.locType = LT_OFFSET;
            dosOpts.nLocation = 0;
            QList<DATA_HEADER> listDos = XMSDOS::getDataHeaders(dosOpts, pPdStruct);
            listResult.append(listDos);
        }

        // Append NE header
        {
            DATA_HEADERS_OPTIONS neOpts = _dataHeadersOptions;
            neOpts.nID = STRUCTID_IMAGE_OS2_HEADER;
            neOpts.locType = LT_OFFSET;
            neOpts.nLocation = getImageOS2HeaderOffset();
            listResult.append(getDataHeaders(neOpts, pPdStruct));
        }
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            if (dataHeadersOptions.nID == STRUCTID_IMAGE_OS2_HEADER) {
                // NE header
                DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XNE::structIDToString(dataHeadersOptions.nID));
                dataHeader.nSize = sizeof(XNE_DEF::IMAGE_OS2_HEADER);

                // Fields
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_magic), 2, "ne_magic", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_ver), 1, "ne_ver", VT_UINT8, DRF_VERSION, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rev), 1, "ne_rev", VT_UINT8, DRF_VERSION, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab), 2, "ne_enttab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab), 2, "ne_cbenttab", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_crc), 4, "ne_crc", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flags), 2, "ne_flags", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_autodata), 2, "ne_autodata", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_heap), 2, "ne_heap", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_stack), 2, "ne_stack", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_csip), 4, "ne_csip", VT_DWORD, DRF_ADDRESS, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_sssp), 4, "ne_sssp", VT_DWORD, DRF_ADDRESS, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg), 2, "ne_cseg", VT_WORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmod), 2, "ne_cmod", VT_WORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab), 2, "ne_cbnrestab", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab), 2, "ne_segtab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab), 2, "ne_rsrctab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab), 2, "ne_restab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab), 2, "ne_modtab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab), 2, "ne_imptab", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab), 4, "ne_nrestab", VT_DWORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cmovent), 2, "ne_cmovent", VT_WORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_align), 2, "ne_align", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cres), 2, "ne_cres", VT_WORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_exetyp), 1, "ne_exetyp", VT_UINT8, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_flagsothers), 1, "ne_flagsothers", VT_UINT8, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_pretthunks), 2, "ne_pretthunks", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_psegrefbytes), 2, "ne_psegrefbytes", VT_WORD, DRF_OFFSET,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_swaparea), 2, "ne_swaparea", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_expver), 2, "ne_expver", VT_WORD, DRF_VERSION, dataHeadersOptions.pMemoryMap->endian));

                listResult.append(dataHeader);

                if (dataHeadersOptions.bChildren) {
                    // Compute key offsets/sizes
                    const qint64 nBase = getImageOS2HeaderOffset();
                    const quint16 offEntry = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_enttab));
                    const quint16 cbEntry = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbenttab));
                    const quint16 offSeg = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_segtab));
                    const quint16 offRes = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_rsrctab));
                    const quint16 offResNames = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_restab));
                    const quint16 offModRef = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_modtab));
                    const quint16 offImpNames = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_imptab));
                    const quint32 offNonRes = read_uint32(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_nrestab));
                    const quint16 cSeg = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cseg));

                    // Entry table (hex)
                    if (cbEntry && _isOffsetValid(nBase + offEntry)) {
                        listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_ENTRY_TABLE), dataHeader.dsID, STRUCTID_ENTRY_TABLE,
                                                         nBase + offEntry, cbEntry));
                    }

                    // Segment table (table mode)
                    if (cSeg && _isOffsetValid(nBase + offSeg)) {
                        DATA_HEADERS_OPTIONS segOpts = dataHeadersOptions;
                        segOpts.dhMode = XBinary::DHMODE_TABLE;
                        segOpts.nID = STRUCTID_SEGMENT_TABLE;
                        segOpts.locType = LT_OFFSET;
                        segOpts.nLocation = nBase + offSeg;
                        segOpts.nCount = cSeg;
                        segOpts.nSize = (qint64)cSeg * (qint64)sizeof(XNE_DEF::NE_SEGMENT);

                        DATA_HEADER segHeader = _initDataHeader(segOpts, XNE::structIDToString(STRUCTID_SEGMENT_TABLE));
                        segHeader.dsID = segOpts.dsID_parent;  // keep chaining
                        segHeader.dsID_parent = dataHeader.dsID;
                        segHeader.nSize = segOpts.nSize;
                        segHeader.nCount = cSeg;
                        segHeader.dhMode = XBinary::DHMODE_TABLE;
                        segHeader.locType = LT_OFFSET;
                        segHeader.nLocation = segOpts.nLocation;
                        segHeader.listRecords.append(
                            getDataRecord(offsetof(XNE_DEF::NE_SEGMENT, dwFileOffset), 2, "FileOffset", VT_WORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
                        segHeader.listRecords.append(
                            getDataRecord(offsetof(XNE_DEF::NE_SEGMENT, dwFileSize), 2, "FileSize", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
                        segHeader.listRecords.append(
                            getDataRecord(offsetof(XNE_DEF::NE_SEGMENT, dwFlags), 2, "Flags", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                        segHeader.listRecords.append(
                            getDataRecord(offsetof(XNE_DEF::NE_SEGMENT, dwMinAllocSize), 2, "MinAllocSize", VT_WORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));

                        listResult.append(segHeader);
                    }

                    // Resource table: from rsrctab to restab
                    if (_isOffsetValid(nBase + offRes) && (offResNames > offRes)) {
                        qint64 nResSize = (qint64)offResNames - (qint64)offRes;
                        if (nResSize > 0) {
                            listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_RESOURCE_TABLE), dataHeader.dsID, STRUCTID_RESOURCE_TABLE,
                                                             nBase + offRes, nResSize));
                        }
                    }

                    // Resident name table: from restab to modtab
                    if (_isOffsetValid(nBase + offResNames) && (offModRef > offResNames)) {
                        qint64 nSize = (qint64)offModRef - (qint64)offResNames;
                        if (nSize > 0) {
                            listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_RESIDENT_NAME_TABLE), dataHeader.dsID,
                                                             STRUCTID_RESIDENT_NAME_TABLE, nBase + offResNames, nSize));
                        }
                    }

                    // Module reference table: from modtab to imptab
                    if (_isOffsetValid(nBase + offModRef) && (offImpNames > offModRef)) {
                        qint64 nSize = (qint64)offImpNames - (qint64)offModRef;
                        if (nSize > 0) {
                            listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_MODULE_REFERENCE_TABLE), dataHeader.dsID,
                                                             STRUCTID_MODULE_REFERENCE_TABLE, nBase + offModRef, nSize));
                        }
                    }

                    // Imported names table: from imptab to (nrestab low 16?) In NE, nrestab is file offset for non-resident names.
                    if (_isOffsetValid(nBase + offImpNames)) {
                        qint64 nEnd = offNonRes ? (qint64)offNonRes : (qint64)getSize();
                        if (nEnd > (nBase + offImpNames)) {
                            qint64 nSize = nEnd - (nBase + offImpNames);
                            if (nSize > 0) {
                                listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_IMPORTED_NAMES_TABLE), dataHeader.dsID,
                                                                 STRUCTID_IMPORTED_NAMES_TABLE, nBase + offImpNames, nSize));
                            }
                        }
                    }

                    // Non-resident names table: absolute file offset and size cbnrestab
                    const quint16 cbNonRes = read_uint16(nBase + offsetof(XNE_DEF::IMAGE_OS2_HEADER, ne_cbnrestab));
                    if (offNonRes && cbNonRes && _isOffsetValid(offNonRes)) {
                        listResult.append(_dataHeaderHex(dataHeadersOptions, XNE::structIDToString(STRUCTID_NONRESIDENT_NAME_TABLE), dataHeader.dsID,
                                                         STRUCTID_NONRESIDENT_NAME_TABLE, offNonRes, cbNonRes));
                    }
                }
            } else if ((dataHeadersOptions.nID == STRUCTID_IMAGE_DOS_HEADER) || (dataHeadersOptions.nID == STRUCTID_IMAGE_DOS_HEADEREX)) {
                // Delegate to base for DOS headers if requested explicitly
                listResult.append(XMSDOS::getDataHeaders(dataHeadersOptions, pPdStruct));
            }
        }
    }

    return listResult;
}

QList<XBinary::FPART> XNE::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)

    QList<XBinary::FPART> listResult;
    QList<XBinary::FPART> _listCalc;

    bool bCalcAddress = false;
    if (nFileParts & FILEPART_SEGMENT) {
        bCalcAddress = true;
    }

    XADDR nModuleAddress = getModuleAddress();
    qint64 nTotalSize = getSize();
    qint64 nMaxOffset = 0;

    // Header
    if (bCalcAddress || (nFileParts & FILEPART_HEADER) || (nFileParts & FILEPART_OVERLAY)) {
        FPART record = {};
        record.filePart = FILEPART_HEADER;
        record.nVirtualAddress = nModuleAddress;  // NE header maps at module base
        record.nVirtualSize = 0x200;              // minimal header page; not used for calc
        record.nFileOffset = 0;
        record.nFileSize = qMin<qint64>(getImageOS2HeaderOffset() + sizeof(XNE_DEF::IMAGE_OS2_HEADER), nTotalSize);
        record.sName = tr("Header");

        if (bCalcAddress) {
            _listCalc.append(record);
        }
        if (nFileParts & FILEPART_HEADER) {
            listResult.append(record);
        }
        nMaxOffset = qMax(nMaxOffset, record.nFileOffset + record.nFileSize);
    }

    // Segments (16-bit segmented model)
    if (bCalcAddress || (nFileParts & FILEPART_SEGMENT) || (nFileParts & FILEPART_OVERLAY)) {
        QList<XNE_DEF::NE_SEGMENT> listSegments = getSegmentList();
        quint16 nShift = getImageOS2Header_align();

        for (qint32 i = 0; i < listSegments.count(); i++) {
            const XNE_DEF::NE_SEGMENT &seg = listSegments.at(i);

            qint64 nFileSize = seg.dwFileSize ? seg.dwFileSize : 0x10000;
            qint64 nFileOffset = (qint64)seg.dwFileOffset << nShift;

            if (nFileOffset > nTotalSize) {
                continue;
            }
            if (nFileOffset + nFileSize > nTotalSize) {
                nFileSize = nTotalSize - nFileOffset;
            }

            FPART record = {};
            record.filePart = FILEPART_SEGMENT;
            record.nFileOffset = nFileOffset;
            record.nFileSize = nFileSize;
            record.nVirtualAddress = nModuleAddress + (XADDR)((i + 1) * 0x10000);
            record.nVirtualSize = 0x10000;
            record.sName = QString("%1 %2").arg(tr("Segment"), QString::number(i + 1));

            if (bCalcAddress) {
                _listCalc.append(record);
            }
            if (nFileParts & FILEPART_SEGMENT) {
                listResult.append(record);
            }

            nMaxOffset = qMax(nMaxOffset, record.nFileOffset + record.nFileSize);
        }
    }

    // Overlay
    if (nFileParts & FILEPART_OVERLAY) {
        if (nMaxOffset < nTotalSize) {
            FPART record = {};
            record.filePart = FILEPART_OVERLAY;
            record.nFileOffset = nMaxOffset;
            record.nFileSize = nTotalSize - nMaxOffset;
            record.nVirtualAddress = -1;
            record.sName = tr("Overlay");
            listResult.append(record);
        }
    }

    Q_UNUSED(pPdStruct)
    return listResult;
}

QList<QString> XNE::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'MZ'");

    return listResult;
}

XBinary *XNE::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XNE(pDevice);
}
