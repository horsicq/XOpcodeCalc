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
#include "xle.h"

XLE::XLE(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress) : XMSDOS(pDevice, bIsImage, nModuleAddress)
{
}

bool XLE::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    bool bResult = false;

    quint16 magic = get_magic();

    if (magic == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_MZ) {
        qint32 lfanew = get_lfanew();
        qint64 nSize = getSize();

        if ((lfanew > 0) && (lfanew < (nSize & 0xFFFFFFFF))) {
            quint32 signature = read_uint32(lfanew);

            if ((signature == XLE_DEF::S_IMAGE_VXD_SIGNATURE) || (signature == XLE_DEF::S_IMAGE_LX_SIGNATURE)) {
                bResult = true;
            }
        }
    }

    return bResult;
}

bool XLE::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XLE xle(pDevice, bIsImage, nModuleAddress);

    return xle.isValid();
}

XBinary::MODE XLE::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XLE xle(pDevice, bIsImage, nModuleAddress);

    return xle.getMode();
}

qint64 XLE::getImageVxdHeaderOffset()
{
    qint64 nResult = get_lfanew();

    if (!_isOffsetValid(nResult)) {
        nResult = -1;
    }

    return nResult;
}

qint64 XLE::getImageVxdHeaderSize()
{
    return sizeof(XLE_DEF::IMAGE_VXD_HEADER);
}

XLE_DEF::IMAGE_VXD_HEADER XLE::getImageVxdHeader()
{
    XLE_DEF::IMAGE_VXD_HEADER result = {};

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        result.e32_magic = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_magic));
        result.e32_border = read_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_border));
        result.e32_worder = read_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_worder));
        result.e32_level = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_level));
        result.e32_cpu = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cpu));
        result.e32_os = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_os));
        result.e32_ver = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ver));
        result.e32_mflags = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mflags));
        result.e32_mpages = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mpages));
        result.e32_startobj = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_startobj));
        result.e32_eip = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_eip));
        result.e32_stackobj = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_stackobj));
        result.e32_esp = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_esp));
        result.e32_pagesize = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesize));
        result.e32_lastpagesize = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_lastpagesize));
        result.e32_fixupsize = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsize));
        result.e32_fixupsum = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsum));
        result.e32_ldrsize = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsize));
        result.e32_ldrsum = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsum));
        result.e32_objtab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objtab));
        result.e32_objcnt = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objcnt));
        result.e32_objmap = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objmap));
        result.e32_itermap = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_itermap));
        result.e32_rsrctab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrctab));
        result.e32_rsrccnt = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrccnt));
        result.e32_restab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_restab));
        result.e32_enttab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_enttab));
        result.e32_dirtab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dirtab));
        result.e32_dircnt = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dircnt));
        result.e32_fpagetab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fpagetab));
        result.e32_frectab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_frectab));
        result.e32_impmod = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmod));
        result.e32_impmodcnt = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmodcnt));
        result.e32_impproc = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impproc));
        result.e32_pagesum = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesum));
        result.e32_datapage = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_datapage));
        result.e32_preload = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_preload));
        result.e32_nrestab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nrestab));
        result.e32_cbnrestab = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cbnrestab));
        result.e32_nressum = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nressum));
        result.e32_autodata = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_autodata));
        result.e32_debuginfo = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuginfo));
        result.e32_debuglen = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuglen));
        result.e32_instpreload = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instpreload));
        result.e32_instdemand = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instdemand));
        result.e32_heapsize = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_heapsize));
    }

    return result;
}

quint16 XLE::getImageVxdHeader_magic()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_magic));
    }

    return nResult;
}

quint8 XLE::getImageVxdHeader_border()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_border));
    }

    return nResult;
}

quint8 XLE::getImageVxdHeader_worder()
{
    quint8 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_worder));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_level()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_level));
    }

    return nResult;
}

quint16 XLE::getImageVxdHeader_cpu()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cpu));
    }

    return nResult;
}

quint16 XLE::getImageVxdHeader_os()
{
    quint16 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_os));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_ver()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ver));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_mflags()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mflags));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_mpages()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mpages));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_startobj()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_startobj));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_eip()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_eip));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_stackobj()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_stackobj));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_esp()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_esp));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_pagesize()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesize));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_lastpagesize()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_lastpagesize));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_fixupsize()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsize));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_fixupsum()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsum));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_ldrsize()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsize));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_ldrsum()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsum));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_objtab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objtab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_objcnt()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objcnt));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_objmap()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objmap));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_itermap()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_itermap));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_rsrctab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrctab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_rsrccnt()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrccnt));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_restab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_restab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_enttab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_enttab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_dirtab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dirtab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_dircnt()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dircnt));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_fpagetab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fpagetab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_frectab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_frectab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_impmod()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmod));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_impmodcnt()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmodcnt));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_impproc()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impproc));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_pagesum()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesum));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_datapage()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_datapage));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_preload()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_preload));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_nrestab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nrestab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_cbnrestab()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cbnrestab));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_nressum()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nressum));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_autodata()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_autodata));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_debuginfo()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuginfo));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_debuglen()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuglen));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_instpreload()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instpreload));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_instdemand()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instdemand));
    }

    return nResult;
}

quint32 XLE::getImageVxdHeader_heapsize()
{
    quint32 nResult = 0;

    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        nResult = read_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_heapsize));
    }

    return nResult;
}

void XLE::setImageVxdHeader_magic(quint16 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_magic), nValue);
    }
}

void XLE::setImageVxdHeader_border(quint8 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_border), nValue);
    }
}

void XLE::setImageVxdHeader_worder(quint8 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint8(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_worder), nValue);
    }
}

void XLE::setImageVxdHeader_level(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_level), nValue);
    }
}

void XLE::setImageVxdHeader_cpu(quint16 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cpu), nValue);
    }
}

void XLE::setImageVxdHeader_os(quint16 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint16(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_os), nValue);
    }
}

void XLE::setImageVxdHeader_ver(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ver), nValue);
    }
}

void XLE::setImageVxdHeader_mflags(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mflags), nValue);
    }
}

void XLE::setImageVxdHeader_mpages(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mpages), nValue);
    }
}

void XLE::setImageVxdHeader_startobj(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_startobj), nValue);
    }
}

void XLE::setImageVxdHeader_eip(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_eip), nValue);
    }
}

void XLE::setImageVxdHeader_stackobj(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_stackobj), nValue);
    }
}

void XLE::setImageVxdHeader_esp(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_esp), nValue);
    }
}

void XLE::setImageVxdHeader_pagesize(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesize), nValue);
    }
}

void XLE::setImageVxdHeader_lastpagesize(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_lastpagesize), nValue);
    }
}

void XLE::setImageVxdHeader_fixupsize(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsize), nValue);
    }
}

void XLE::setImageVxdHeader_fixupsum(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fixupsum), nValue);
    }
}

void XLE::setImageVxdHeader_ldrsize(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsize), nValue);
    }
}

void XLE::setImageVxdHeader_ldrsum(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_ldrsum), nValue);
    }
}

void XLE::setImageVxdHeader_objtab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objtab), nValue);
    }
}

void XLE::setImageVxdHeader_objcnt(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objcnt), nValue);
    }
}

void XLE::setImageVxdHeader_objmap(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objmap), nValue);
    }
}

void XLE::setImageVxdHeader_itermap(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_itermap), nValue);
    }
}

void XLE::setImageVxdHeader_rsrctab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrctab), nValue);
    }
}

void XLE::setImageVxdHeader_rsrccnt(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_rsrccnt), nValue);
    }
}

void XLE::setImageVxdHeader_restab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_restab), nValue);
    }
}

void XLE::setImageVxdHeader_enttab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_enttab), nValue);
    }
}

void XLE::setImageVxdHeader_dirtab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dirtab), nValue);
    }
}

void XLE::setImageVxdHeader_dircnt(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_dircnt), nValue);
    }
}

void XLE::setImageVxdHeader_fpagetab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_fpagetab), nValue);
    }
}

void XLE::setImageVxdHeader_frectab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_frectab), nValue);
    }
}

void XLE::setImageVxdHeader_impmod(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmod), nValue);
    }
}

void XLE::setImageVxdHeader_impmodcnt(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impmodcnt), nValue);
    }
}

void XLE::setImageVxdHeader_impproc(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_impproc), nValue);
    }
}

void XLE::setImageVxdHeader_pagesum(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesum), nValue);
    }
}

void XLE::setImageVxdHeader_datapage(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_datapage), nValue);
    }
}

void XLE::setImageVxdHeader_preload(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_preload), nValue);
    }
}

void XLE::setImageVxdHeader_nrestab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nrestab), nValue);
    }
}

void XLE::setImageVxdHeader_cbnrestab(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cbnrestab), nValue);
    }
}

void XLE::setImageVxdHeader_nressum(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_nressum), nValue);
    }
}

void XLE::setImageVxdHeader_autodata(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_autodata), nValue);
    }
}

void XLE::setImageVxdHeader_debuginfo(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuginfo), nValue);
    }
}

void XLE::setImageVxdHeader_debuglen(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_debuglen), nValue);
    }
}

void XLE::setImageVxdHeader_instpreload(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instpreload), nValue);
    }
}

void XLE::setImageVxdHeader_instdemand(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_instdemand), nValue);
    }
}

void XLE::setImageVxdHeader_heapsize(quint32 nValue)
{
    qint64 nOffset = getImageVxdHeaderOffset();

    if (nOffset != -1) {
        write_uint32(nOffset + offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_heapsize), nValue);
    }
}

XLE_DEF::o32_obj XLE::_read_o32_obj(qint64 nOffset)
{
    XLE_DEF::o32_obj result = {};

    result.o32_size = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_size));
    result.o32_base = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_base));
    result.o32_flags = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_flags));
    result.o32_pagemap = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_pagemap));
    result.o32_mapsize = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_mapsize));
    result.o32_reserved = read_uint32(nOffset + offsetof(XLE_DEF::o32_obj, o32_reserved));

    return result;
}

XLE_DEF::o16_map XLE::_read_o16_map(qint64 nOffset)
{
    XLE_DEF::o16_map result = {};

    result.o16_pagenum[0] = read_uint8(nOffset + offsetof(XLE_DEF::o16_map, o16_pagenum) + 0);
    result.o16_pagenum[1] = read_uint8(nOffset + offsetof(XLE_DEF::o16_map, o16_pagenum) + 1);
    result.o16_pagenum[2] = read_uint8(nOffset + offsetof(XLE_DEF::o16_map, o16_pagenum) + 2);
    result.o16_pageflags = read_uint8(nOffset + offsetof(XLE_DEF::o16_map, o16_pageflags));

    return result;
}

XLE_DEF::o32_map XLE::_read_o32_map(qint64 nOffset)
{
    XLE_DEF::o32_map result = {};

    result.o32_pagedataoffset = read_uint32(nOffset + offsetof(XLE_DEF::o32_map, o32_pagedataoffset));
    result.o32_pagesize = read_uint16(nOffset + offsetof(XLE_DEF::o32_map, o32_pagesize));
    result.o32_pageflags = read_uint16(nOffset + offsetof(XLE_DEF::o32_map, o32_pageflags));

    return result;
}

QList<XLE_DEF::o32_obj> XLE::getObjects()
{
    QList<XLE_DEF::o32_obj> listResult;

    qint64 nObjOffset = getImageVxdHeaderOffset() + getImageVxdHeader_objtab();
    quint32 nNumberOfObjects = getImageVxdHeader_objcnt();

    nNumberOfObjects = qMin(nNumberOfObjects, (quint32)100);

    for (quint32 i = 0; i < nNumberOfObjects; i++) {
        XLE_DEF::o32_obj record = _read_o32_obj(nObjOffset);

        listResult.append(record);

        nObjOffset += sizeof(XLE_DEF::o32_obj);
    }

    return listResult;
}

QList<XLE_DEF::o16_map> XLE::getMapsLE()
{
    QList<XLE_DEF::o16_map> listResult;

    qint64 nMapOffset = getImageVxdHeaderOffset() + getImageVxdHeader_objmap();
    quint32 nNumberOfMaps = getImageVxdHeader_mpages();

    for (quint32 i = 0; i < nNumberOfMaps; i++) {
        XLE_DEF::o16_map record = _read_o16_map(nMapOffset);

        listResult.append(record);

        nMapOffset += sizeof(XLE_DEF::o16_map);
    }

    return listResult;
}

QList<XLE_DEF::o32_map> XLE::getMapsLX()
{
    QList<XLE_DEF::o32_map> listResult;

    qint64 nMapOffset = getImageVxdHeaderOffset() + getImageVxdHeader_objmap();
    quint32 nNumberOfMaps = getImageVxdHeader_mpages();

    for (quint32 i = 0; i < nNumberOfMaps; i++) {
        XLE_DEF::o32_map record = _read_o32_map(nMapOffset);

        listResult.append(record);

        nMapOffset += sizeof(XLE_DEF::o32_map);
    }

    return listResult;
}

XBinary::_MEMORY_MAP XLE::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)

    // Build memory map from file parts (header/objects-as-segments/overlay)
    return _getMemoryMap(FILEPART_HEADER | FILEPART_SEGMENT | FILEPART_OVERLAY, pPdStruct);
}

qint64 XLE::getImageSize()
{
    // Compute image size as highest end VA among objects
    QList<XLE_DEF::o32_obj> objs = getObjects();
    XADDR maxEnd = 0;
    for (int i = 0; i < objs.size(); ++i) {
        // Approximate object size as sum of its pages
        XADDR base = objs.at(i).o32_base;
        quint32 pages = objs.at(i).o32_mapsize;
        quint32 pageSize = getImageVxdHeader_pagesize();
        XADDR end = base + (XADDR)pages * (XADDR)pageSize;
        if (end > maxEnd) maxEnd = end;
    }
    return (qint64)maxEnd;
}

XADDR XLE::_getEntryPointAddress()
{
    // Entry point is base of start object + EIP
    quint32 startObj = getImageVxdHeader_startobj();
    quint32 eip = getImageVxdHeader_eip();
    if (startObj == 0) return (XADDR)-1;
    QList<XLE_DEF::o32_obj> objs = getObjects();
    if (startObj > (quint32)objs.size()) return (XADDR)-1;
    return (XADDR)objs.at(startObj - 1).o32_base + (XADDR)eip;
}

XBinary::MODE XLE::getMode()
{
    // TODO CHECK MODE_16SEG
    MODE result = MODE_16SEG;

    qint32 lfanew = get_lfanew();
    quint16 signature = read_uint16(lfanew);

    if (signature == XLE_DEF::S_IMAGE_VXD_SIGNATURE) {
        result = MODE_16SEG;
        // result = MODE_32;
    } else if (signature == XLE_DEF::S_IMAGE_LX_SIGNATURE) {
        result = MODE_32;
    }

    return result;
}

QString XLE::getArch()
{
    return getImageLECpusS().value(getImageVxdHeader_cpu(), tr("Unknown"));
}

XBinary::ENDIAN XLE::getEndian()
{
    return ENDIAN_LITTLE;
}

XBinary::FT XLE::getFileType()
{
    FT result = FT_LE;

    if (isLE()) {
        result = FT_LE;
    } else if (isLX()) {
        result = FT_LX;
    }

    return result;
}

qint32 XLE::getType()
{
    return TYPE_EXE;  // TODO
}

QString XLE::getOsVersion()
{
    QString sResult = "";

    quint16 nOS = getImageVxdHeader_os();

    if (nOS == 3) {
        sResult = "4.X";
    } else if (nOS == 4) {
        sResult = "386";
    }

    return sResult;
}

XBinary::OSNAME XLE::getOsName()
{
    OSNAME result = OSNAME_UNKNOWN;

    quint16 nOS = getImageVxdHeader_os();

    if (nOS == 1) {
        result = OSNAME_OS2;
    } else if (nOS == 2) {
        result = OSNAME_WINDOWS;
    } else if (nOS == 3) {
        result = OSNAME_MSDOS;
    } else if (nOS == 4) {
        result = OSNAME_WINDOWS;
    }

    return result;
}

QString XLE::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown"); break;
        case TYPE_EXE: sResult = QString("EXE"); break;
    }

    return sResult;
}

QList<XBinary::MAPMODE> XLE::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_OBJECTS);
    listResult.append(MAPMODE_MAPS);

    return listResult;
}

qint64 XLE::getModuleAddress()
{
    return 0;  // TODO Check
}

QMap<quint64, QString> XLE::getImageLEMagics()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x454C, "IMAGE_VXD_SIGNATURE");
    mapResult.insert(0x584C, "IMAGE_LX_SIGNATURE");

    return mapResult;
}

QMap<quint64, QString> XLE::getImageLEMagicsS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x454C, "VXD_SIGNATURE");
    mapResult.insert(0x584C, "LX_SIGNATURE");

    return mapResult;
}

QMap<quint64, QString> XLE::getImageLECpusS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x01, "80286");
    mapResult.insert(0x02, "80386");
    mapResult.insert(0x03, "80486");
    mapResult.insert(0x04, "80586");
    mapResult.insert(0x20, "i860");
    mapResult.insert(0x21, "N11");
    mapResult.insert(0x40, "R2000");
    mapResult.insert(0x41, "R6000");
    mapResult.insert(0x42, "R4000");

    return mapResult;
}

QMap<quint64, QString> XLE::getImageLEOssS()
{
    QMap<quint64, QString> mapResult;

    mapResult.insert(0x00, "Unknown (any new-format OS)");
    mapResult.insert(0x01, "OS/2 (default)");
    mapResult.insert(0x02, "Windows");
    mapResult.insert(0x03, "DOS 4.x");
    mapResult.insert(0x04, "Windows 386");

    return mapResult;
}

QMap<quint64, QString> XLE::getImageLEMflagsS()
{
    QMap<quint64, QString> mapResult;

    // TODO

    return mapResult;
}

QString XLE::getFileFormatExtsString()
{
    return "LE(vxd lx)";
}

// --- Data headers/inspection and file parts ---------------------------------

QString XLE::structIDToString(quint32 nID)
{
    // Reuse DOS struct IDs for base if requested, else provide simple mapping for LE header
    switch (nID) {
        case XMSDOS::STRUCTID_IMAGE_DOS_HEADER: return QString("IMAGE_DOS_HEADER");
        case XMSDOS::STRUCTID_IMAGE_DOS_HEADEREX: return QString("IMAGE_DOS_HEADEREX");
        default: break;
    }
    // For LE/LX, expose a generic header id string
    return QString("IMAGE_VXD_HEADER");
}

QList<XBinary::DATA_HEADER> XLE::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == XBinary::STRUCTID_UNKNOWN) {
        // Root: defaults + DOS header + LE/LX header
        DATA_HEADERS_OPTIONS opts = dataHeadersOptions;
        opts.bChildren = true;
        opts.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        opts.dhMode = XBinary::DHMODE_HEADER;
        opts.fileType = dataHeadersOptions.pMemoryMap->fileType;

        // DOS header (extended)
        {
            DATA_HEADERS_OPTIONS dos = opts;
            dos.nID = XMSDOS::STRUCTID_IMAGE_DOS_HEADEREX;
            dos.locType = LT_OFFSET;
            dos.nLocation = 0;
            listResult.append(XMSDOS::getDataHeaders(dos, pPdStruct));
        }

        // LE/LX header at e_lfanew
        qint64 nHdrOff = getImageVxdHeaderOffset();
        if (nHdrOff != -1) {
            DATA_HEADERS_OPTIONS le = opts;
            le.nID = 1;  // local id placeholder
            le.locType = LT_OFFSET;
            le.nLocation = nHdrOff;

            DATA_HEADER dh = _initDataHeader(le, QString("%1").arg(isLE() ? "IMAGE_VXD_HEADER(LE)" : "IMAGE_VXD_HEADER(LX)"));
            dh.nSize = getImageVxdHeaderSize();
            // Minimal key fields
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_magic), 2, "e32_magic", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_cpu), 2, "e32_cpu", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_os), 2, "e32_os", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objtab), 4, "e32_objtab", VT_DWORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objcnt), 4, "e32_objcnt", VT_DWORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_objmap), 4, "e32_objmap", VT_DWORD, DRF_OFFSET, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_mpages), 4, "e32_mpages", VT_DWORD, DRF_COUNT, dataHeadersOptions.pMemoryMap->endian));
            dh.listRecords.append(
                getDataRecord(offsetof(XLE_DEF::IMAGE_VXD_HEADER, e32_pagesize), 4, "e32_pagesize", VT_DWORD, DRF_SIZE, dataHeadersOptions.pMemoryMap->endian));
            listResult.append(dh);
        }
    } else {
        // For any DOS header request, delegate to base
        if ((dataHeadersOptions.nID == XMSDOS::STRUCTID_IMAGE_DOS_HEADER) || (dataHeadersOptions.nID == XMSDOS::STRUCTID_IMAGE_DOS_HEADEREX)) {
            listResult.append(XMSDOS::getDataHeaders(dataHeadersOptions, pPdStruct));
        }
    }

    return listResult;
}

QList<XBinary::FPART> XLE::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    Q_UNUSED(pPdStruct)

    QList<FPART> listResult;
    qint64 nTotal = getSize();

    // Header
    if (nFileParts & FILEPART_HEADER) {
        FPART rec = {};
        rec.filePart = FILEPART_HEADER;
        rec.nFileOffset = 0;
        rec.nFileSize = qMin<qint64>(getImageVxdHeaderOffset() + getImageVxdHeaderSize(), nTotal);
        rec.nVirtualAddress = 0;
        rec.sName = tr("Header");
        listResult.append(rec);
    }

    // Segments/objects (treat as segments for file parts)
    if (nFileParts & FILEPART_SEGMENT) {
        QList<XLE_DEF::o32_obj> objs = getObjects();
        quint32 nPageSize = getImageVxdHeader_pagesize();
        quint32 nPages = getImageVxdHeader_mpages();
        quint32 nLastPageSize = getImageVxdHeader_lastpagesize();
        qint64 nDataPageOff = getImageVxdHeader_datapage();
        qint64 nMapOff = getImageVxdHeaderOffset() + getImageVxdHeader_objmap();
        qint64 nLoaderSize = 0;
        if (nPages > 0) nLoaderSize = nDataPageOff + (qint64)(nPages - 1) * (qint64)nPageSize + (qint64)nLastPageSize;
        bool bIsLE = isLE();
        for (qint32 i = 0; i < objs.size(); i++) {
            // Determine object span over pages
            qint64 nObjMin = -1;
            qint64 nObjMax = 0;
            qint32 nMapSize = qMin((qint32)objs.at(i).o32_mapsize, (qint32)nPages);
            for (qint32 j = 0; j < nMapSize; j++) {
                qint64 nPageDataOffset = 0;
                qint64 nThisPageSize = nPageSize;
                qint64 nMapEntryOff = 0;
                if (bIsLE) {
                    nMapEntryOff = nMapOff + (qint64)(objs.at(i).o32_pagemap - 1 + j) * (qint64)sizeof(XLE_DEF::o16_map);
                    if ((nMapEntryOff < 0) || (nMapEntryOff + (qint64)sizeof(XLE_DEF::o16_map) > nTotal)) {
                        continue;
                    }
                    XLE_DEF::o16_map m = _read_o16_map(nMapEntryOff);
                    quint32 nOfs = m.o16_pagenum[2] + (m.o16_pagenum[1] << 8) + ((quint32)m.o16_pagenum[0] << 16);
                    if (nOfs) {
                        nPageDataOffset = ((qint64)(nOfs - 1) * (qint64)nPageSize) + nDataPageOff;
                    }
                    // For LE, all pages except last have standard page size; clamp to loader end
                    if ((nLoaderSize > 0) && (nPageDataOffset < nLoaderSize)) {
                        nThisPageSize = qMin((qint64)nPageSize, nLoaderSize - nPageDataOffset);
                    }
                } else {
                    nMapEntryOff = nMapOff + (qint64)(objs.at(i).o32_pagemap - 1 + j) * (qint64)sizeof(XLE_DEF::o32_map);
                    if ((nMapEntryOff < 0) || (nMapEntryOff + (qint64)sizeof(XLE_DEF::o32_map) > nTotal)) {
                        continue;
                    }
                    XLE_DEF::o32_map m = _read_o32_map(nMapEntryOff);
                    nPageDataOffset = nDataPageOff + m.o32_pagedataoffset;
                    nThisPageSize = m.o32_pagesize ? m.o32_pagesize : nPageSize;
                }
                // Validate page data offset is within file
                if ((nPageDataOffset < 0) || (nPageDataOffset >= nTotal)) {
                    continue;
                }
                nThisPageSize = qMin(nThisPageSize, nTotal - nPageDataOffset);
                if (nThisPageSize <= 0) {
                    continue;
                }
                if (nObjMin == -1) nObjMin = nPageDataOffset;
                nObjMin = qMin(nObjMin, nPageDataOffset);
                nObjMax = qMax(nObjMax, nPageDataOffset + nThisPageSize);
            }

            if (nObjMin >= 0 && nObjMax > nObjMin) {
                FPART rec = {};
                rec.filePart = FILEPART_SEGMENT;
                rec.nFileOffset = nObjMin;
                rec.nFileSize = qMin<qint64>(nTotal - nObjMin, nObjMax - nObjMin);
                rec.nVirtualAddress = objs.at(i).o32_base;
                rec.nVirtualSize = rec.nFileSize;
                rec.sName = QString("%1(%2)").arg(tr("Object")).arg(i + 1);
                listResult.append(rec);
            }
        }
        Q_UNUSED(nPages)
    }

    // // Overlay
    // if (nFileParts & FILEPART_OVERLAY) {
    //     qint64 nMax = 0;
    //     for (int i = 0; i < listResult.size(); ++i) {
    //         const auto &r = listResult.at(i);
    //         nMax = qMax(nMax, r.nFileOffset + r.nFileSize);
    //     }
    //     if (nMax < nTotal) {
    //         FPART rec = {};
    //         rec.filePart = FILEPART_OVERLAY;
    //         rec.nFileOffset = nMax;
    //         rec.nFileSize = nTotal - nMax;
    //         rec.nVirtualAddress = -1;
    //         rec.sName = tr("Overlay");
    //         listResult.append(rec);
    //     }
    // }

    return listResult;
}

QList<QString> XLE::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'MZ'");

    return listResult;
}

XBinary *XLE::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XLE(pDevice);
}
