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
#include "xbinary.h"
#include <cstring>
#include <QDebug>
#ifdef Q_OS_WIN
#include <windows.h>
#endif

bool compareMemoryMapRecord(const XBinary::_MEMORY_RECORD &a, const XBinary::_MEMORY_RECORD &b)
{
    if (a.nAddress != b.nAddress) {
        return a.nAddress < b.nAddress;
    } else {
        return a.nOffset < b.nOffset;
    }
}

bool compareFileParts(const XBinary::FPART &a, const XBinary::FPART &b)
{
    if ((a.nVirtualAddress != -1) && (b.nVirtualAddress != -1)) {
        return a.nVirtualAddress < b.nVirtualAddress;
    } else {
        return a.nFileOffset < b.nFileOffset;
    }
}

bool compareMS_RECORD(const XBinary::MS_RECORD &a, const XBinary::MS_RECORD &b)
{
    if (a.nRegionIndex != b.nRegionIndex) {
        return a.nRegionIndex < b.nRegionIndex;
    } else {
        return a.nRelOffset < b.nRelOffset;
    }
}

const quint32 _crc32_EDB88320_tab[] = {
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b,
    0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7, 0x136c9856, 0x646ba8c0,
    0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd,
    0xa50ab56b, 0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59, 0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190,
    0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433, 0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
    0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea,
    0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65, 0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525,
    0x206f85b3, 0xb966d409, 0xce61e49f, 0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6,
    0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27,
    0x7d079eb1, 0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b, 0xd80d2bda,
    0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703,
    0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d, 0x9b64c2b0, 0xec63f226, 0x756aa39c,
    0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777, 0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff,
    0xf862ae69, 0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
    0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9, 0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729,
    0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d};

const quint16 _crc16_tab[] = {
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 0xcc01, 0x0cc0, 0x0d80, 0xcd41,
    0x0f00, 0xcfc1, 0xce81, 0x0e40, 0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 0xd201, 0x12c0, 0x1380, 0xd341,
    0x1100, 0xd1c1, 0xd081, 0x1040, 0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 0x2800, 0xe8c1, 0xe981, 0x2940,
    0xeb01, 0x2bc0, 0x2a80, 0xea41, 0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40, 0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 0x6600, 0xa6c1, 0xa781, 0x6740,
    0xa501, 0x65c0, 0x6480, 0xa441, 0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 0xb401, 0x74c0, 0x7580, 0xb541,
    0x7700, 0xb7c1, 0xb681, 0x7640, 0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 0x5a00, 0x9ac1, 0x9b81, 0x5b40,
    0x9901, 0x59c0, 0x5880, 0x9841, 0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040};

// Alphanumeric lookup table for signature matching (0-9, A-Z, a-z)
const bool g_alphaNumTable[256] = {
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false, false, false, false, false,  // 0-9
    false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,                               // A-O
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false, false, false, false,                              // P-Z
    false, true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,                               // a-o
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,  false, false, false, false, false,                              // p-z
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};

XBinary::XCONVERT _TABLE_XBINARY_STRUCTID[] = {
    {XBinary::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XBinary::STRUCTID_NFDSCAN, "nfd", QString("Nauz File Detector")},
    {XBinary::STRUCTID_DIESCAN, "die", QString("Detect It Easy")},
    {XBinary::STRUCTID_YARASCAN, "yara", QString("Yara")},
    {XBinary::STRUCTID_VIRUSTOTALSCAN, "virustotal", QString("VirusTotal")},
    {XBinary::STRUCTID_VISUALIZATION, "visualization", QObject::tr("Visualization")},
    {XBinary::STRUCTID_HEX, "hex", QObject::tr("Hex")},
    {XBinary::STRUCTID_DISASM, "disasm", QObject::tr("Disasm")},
    {XBinary::STRUCTID_ARCHIVE, "archive", QObject::tr("Archive")},
    {XBinary::STRUCTID_HASH, "hash", QObject::tr("Hash")},
    {XBinary::STRUCTID_STRINGS, "strings", QObject::tr("Strings")},
    {XBinary::STRUCTID_SIGNATURES, "signatures", QObject::tr("Signatures")},
    {XBinary::STRUCTID_REGIONS, "regions", QObject::tr("Regions")},
    {XBinary::STRUCTID_MEMORYMAP, "memorymap", QObject::tr("Memory map")},
    {XBinary::STRUCTID_SYMBOLS, "symbols", QObject::tr("Symbols")},
    {XBinary::STRUCTID_ENTROPY, "entropy", QObject::tr("Entropy")},
    {XBinary::STRUCTID_EXTRACTOR, "extractor", QObject::tr("Extractor")},
    {XBinary::STRUCTID_SEARCH, "search", QObject::tr("Search")},
    {XBinary::STRUCTID_OVERLAY, "overlay", QObject::tr("Overlay")},
};

XBinary::XCONVERT _TABLE_XBINARY_HANDLE_METHOD[] = {
    {XBinary::HANDLE_METHOD_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XBinary::HANDLE_METHOD_STORE, "Store", QString("Store")},
    {XBinary::HANDLE_METHOD_PDF_IMAGEDATA, "PDF_IMAGEDATA", QString("PDF Image data")},
    {XBinary::HANDLE_METHOD_PDF_CCITTIMAGE, "PDF_CCITTIMAGE", QString("PDF CCITT Image")},
    {XBinary::HANDLE_METHOD_PDF_PALETTE, "PDF_PALETTE", QString("PDF Palette")},
    {XBinary::HANDLE_METHOD_ANDROID_XML, "Android_XML", QString("Android XML")},
    {XBinary::HANDLE_METHOD_FILE, "File", QObject::tr("File")},  // TODO Check
    {XBinary::HANDLE_METHOD_DEFLATE, "Deflate", QString("Deflate")},
    {XBinary::HANDLE_METHOD_DEFLATE64, "Deflate64", QString("Deflate64")},
    {XBinary::HANDLE_METHOD_BZIP2, "Bzip2", QString("Bzip2")},
    {XBinary::HANDLE_METHOD_LZMA, "LZMA", QString("LZMA")},
    {XBinary::HANDLE_METHOD_LZMA2, "LZMA2", QString("LZMA2")},
    {XBinary::HANDLE_METHOD_LZW_PDF, "LZW_PDF", QString("LZW PDF")},
    {XBinary::HANDLE_METHOD_ASCII85, "ASCII85", QString("ASCII85 PDF")},
    {XBinary::HANDLE_METHOD_PPMD7, "PPMD7", QString("PPMD7")},
    {XBinary::HANDLE_METHOD_PPMD8, "PPMD8", QString("PPMD8")},  // TODO
    {XBinary::HANDLE_METHOD_LZH5, "LZH5", QString("LZH5")},
    {XBinary::HANDLE_METHOD_LZH6, "LZH6", QString("LZH6")},
    {XBinary::HANDLE_METHOD_LZH7, "LZH7", QString("LZH7")},
    {XBinary::HANDLE_METHOD_RAR_15, "RAR_15", QString("RAR 1.5")},
    {XBinary::HANDLE_METHOD_RAR_20, "RAR_20", QString("RAR 2.0")},
    {XBinary::HANDLE_METHOD_RAR_29, "RAR_29", QString("RAR 2.9")},
    {XBinary::HANDLE_METHOD_RAR_50, "RAR_50", QString("RAR 5.0")},
    {XBinary::HANDLE_METHOD_RAR_70, "RAR_70", QString("RAR 7.0")},
    {XBinary::HANDLE_METHOD_LZSS_SZDD, "LZSS_SZDD", QString("LZSS SZDD")},
    {XBinary::HANDLE_METHOD_IT214_8, "IT214_8", QString("IT214 8-bit")},
    {XBinary::HANDLE_METHOD_IT214_16, "IT214_16", QString("IT214 16-bit")},
    {XBinary::HANDLE_METHOD_IT215_8, "IT215_8", QString("IT215 8-bit")},
    {XBinary::HANDLE_METHOD_IT215_16, "IT215_16", QString("IT215 16-bit")},
    {XBinary::HANDLE_METHOD_IMPLODED_4KDICT_2TREES, "Imploded_4K_2", QString("Imploded 4K/2")},
    {XBinary::HANDLE_METHOD_IMPLODED_4KDICT_3TREES, "Imploded_4K_3", QString("Imploded 4K/3")},
    {XBinary::HANDLE_METHOD_IMPLODED_8KDICT_2TREES, "Imploded_8K_2", QString("Imploded 8K/2")},
    {XBinary::HANDLE_METHOD_IMPLODED_8KDICT_3TREES, "Imploded_8K_3", QString("Imploded 8K/3")},
    {XBinary::HANDLE_METHOD_SHRINK, "Shrink", QString("Shrink")},
    {XBinary::HANDLE_METHOD_REDUCE_1, "Reduce_1", QString("Reduce 1")},
    {XBinary::HANDLE_METHOD_REDUCE_2, "Reduce_2", QString("Reduce 2")},
    {XBinary::HANDLE_METHOD_REDUCE_3, "Reduce_3", QString("Reduce 3")},
    {XBinary::HANDLE_METHOD_REDUCE_4, "Reduce_4", QString("Reduce 4")},
    {XBinary::HANDLE_METHOD_ZIP_AES, "ZIP_AES", QString("7Z AES")},
    {XBinary::HANDLE_METHOD_ZIP_AES128, "ZIP_AES128", QString("ZIP AES128")},
    {XBinary::HANDLE_METHOD_ZIP_AES192, "ZIP_AES128", QString("ZIP AES128")},
    {XBinary::HANDLE_METHOD_ZIP_AES256, "ZIP_AES256", QString("ZIP AES256")},
    {XBinary::HANDLE_METHOD_7Z_AES, "7Z_AES", QString("7Z AES")},
    {XBinary::HANDLE_METHOD_ZIPCRYPTO, "ZIP_Crypto", QString("ZIP Crypto")},
    {XBinary::HANDLE_METHOD_ZLIB, "ZLIB", QString("ZLIB")},
    {XBinary::HANDLE_METHOD_MSZIP_CAB, "MSZIP_CAB", QString("MSZIP CAB")},
    {XBinary::HANDLE_METHOD_STORE_CAB, "STORE_CAB", QString("STORE CAB")},
    {XBinary::HANDLE_METHOD_LZX_CAB, "LZX_CAB", QString("LZX CAB")},
    {XBinary::HANDLE_METHOD_ZSTD, "ZSTD", QString("Zstandard")},
    {XBinary::HANDLE_METHOD_LZIP, "LZIP", QString("LZIP")},
    {XBinary::HANDLE_METHOD_LZOP, "LZOP", QString("LZOP")},
    {XBinary::HANDLE_METHOD_COMPRESS, "COMPRESS", QString("Compress (LZW)")},
};

XBinary::XCONVERT _TABLE_XBinary_FILEPART[] = {
    {XBinary::FILEPART_UNKNOWN, "Unknown", QObject::tr("Unknown")}, {XBinary::FILEPART_HEADER, "Header", QObject::tr("Header")},
    {XBinary::FILEPART_OVERLAY, "Overlay", QObject::tr("Overlay")}, {XBinary::FILEPART_RESOURCE, "Resource", QObject::tr("Resource")},
    {XBinary::FILEPART_REGION, "Region", QObject::tr("Region")},    {XBinary::FILEPART_DEBUGDATA, "DebugData", QObject::tr("Debug data")},
    {XBinary::FILEPART_STREAM, "Stream", QObject::tr("Stream")},    {XBinary::FILEPART_SIGNATURE, "Signature", QObject::tr("Signature")},
    {XBinary::FILEPART_SECTION, "Section", QObject::tr("Section")}, {XBinary::FILEPART_DATA, "Data", QObject::tr("Data")},
    {XBinary::FILEPART_SEGMENT, "Segment", QObject::tr("Segment")}, {XBinary::FILEPART_FOOTER, "Footer", QObject::tr("Footer")},
    {XBinary::FILEPART_OBJECT, "Object", QObject::tr("Object")},    {XBinary::FILEPART_TABLE, "Table", QObject::tr("Table")},
    {XBinary::FILEPART_VALUE, "Value", QObject::tr("Value")},
};

XBinary::XCONVERT _TABLE_XBinary_FT[] = {
    {XBinary::FT_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XBinary::FT_DATA, "Data", QObject::tr("Data")},
    {XBinary::FT_REGION, "Region", QObject::tr("Region")},
    {XBinary::FT_PROCESS, "Process", QObject::tr("Process")},
    {XBinary::FT_BINARY, "Binary", QString("Binary")},
    {XBinary::FT_BINARY16, "Binary16", QString("Binary16")},
    {XBinary::FT_BINARY32, "Binary32", QString("Binary32")},
    {XBinary::FT_BINARY64, "Binary64", QString("Binary64")},
    {XBinary::FT_COM, "COM", QString("COM")},
    {XBinary::FT_MSDOS, "MSDOS", QString("MSDOS")},
    {XBinary::FT_DOS16M, "DOS16M", QString("DOS/16M")},
    {XBinary::FT_DOS4G, "DOS4G", QString("DOS/4G")},
    {XBinary::FT_NE, "NE", QString("NE")},
    {XBinary::FT_LE, "LE", QString("LE")},
    {XBinary::FT_LX, "LX", QString("LX")},
    {XBinary::FT_PE, "PE", QString("PE")},
    {XBinary::FT_PE32, "PE32", QString("PE32")},
    {XBinary::FT_PE64, "PE64", QString("PE64")},
    {XBinary::FT_ELF, "ELF", QString("ELF")},
    {XBinary::FT_ELF32, "ELF32", QString("ELF32")},
    {XBinary::FT_ELF64, "ELF64", QString("ELF64")},
    {XBinary::FT_MACHO, "Mach-O", QString("Mach-O")},
    {XBinary::FT_MACHO32, "Mach-O32", QString("Mach-O32")},
    {XBinary::FT_MACHO64, "Mach-O64", QString("Mach-O64")},
    {XBinary::FT_AMIGAHUNK, "Amiga Hunk", QString("Amiga Hunk")},
    {XBinary::FT_ATARIST, "Atari ST", QString("Atari ST")},
    // Extra
    {XBinary::FT_7Z, "7-Zip", QString("7-Zip")},
    {XBinary::FT_ANDROIDASRC, "AndroidASRC", QString("Android ASRC")},
    {XBinary::FT_ANDROIDXML, "AndroidXML", QString("Android XML")},
    {XBinary::FT_APK, "APK", QString("APK")},
    {XBinary::FT_APKS, "APKS", QString("APKS")},
    {XBinary::FT_AR, "ar", QString("ar")},
    {XBinary::FT_ARCHIVE, "Archive", QObject::tr("Archive")},
    {XBinary::FT_AUDIO, "Audio", QObject::tr("Audio")},
    {XBinary::FT_AVI, "AVI", QString("AVI")},
    {XBinary::FT_BMP, "BMP", QString("BMP")},
    {XBinary::FT_CAB, "CAB", QString("CAB")},
    {XBinary::FT_CUR, "CUR", QString("CUR")},
    {XBinary::FT_DEX, "DEX", QString("DEX")},
    {XBinary::FT_DOCUMENT, "Document", QObject::tr("Document")},
    {XBinary::FT_GIF, "GIF", QString("GIF")},
    {XBinary::FT_GZIP, "GZIP", QString("GZIP")},
    {XBinary::FT_ICC, "ICC", QString("ICC")},
    {XBinary::FT_ICO, "ICO", QString("ICO")},
    {XBinary::FT_IMAGE, "Image", QObject::tr("Image")},
    {XBinary::FT_ISO9660, "ISO 9660", QString("ISO 9660")},
    {XBinary::FT_UDF, "UDF", QString("UDF")},
    {XBinary::FT_IPA, "IPA", QString("IPA")},
    {XBinary::FT_JAR, "JAR", QString("JAR")},
    {XBinary::FT_JPEG, "JPEG", QString("JPEG")},
    {XBinary::FT_LHA, "LHA", QString("LHA")},
    {XBinary::FT_MACHOFAT, "Mach-O FAT", QString("Mach-O FAT")},
    {XBinary::FT_MP3, "MP3", QString("MP3")},
    {XBinary::FT_MP4, "MP4", QString("MP4")},
    {XBinary::FT_XM, "XM", QString("XM")},
    {XBinary::FT_NPM, "NPM", QString("NPM")},
    {XBinary::FT_PDF, "PDF", QString("PDF")},
    {XBinary::FT_DER, "DER", QString("DER")},
    {XBinary::FT_PLAINTEXT, "PlainText", QObject::tr("Plain Text")},
    {XBinary::FT_PNG, "PNG", QString("PNG")},
    {XBinary::FT_RAR, "RAR", QString("RAR")},
    {XBinary::FT_RIFF, "RIFF", QString("RIFF")},
    {XBinary::FT_SIGNATURE, "Signature", QObject::tr("Signature")},
    {XBinary::FT_TAR, "tar", QString("tar")},
    {XBinary::FT_TAR_GZ, "tar.gz (typed)", QString("tar.gz")},
    {XBinary::FT_TAR_BZIP2, "tar.bz2", QString("tar.bz2")},
    {XBinary::FT_TAR_LZIP, "tar.lz", QString("tar.lz")},
    {XBinary::FT_TAR_LZMA, "tar.lzma", QString("tar.lzma")},
    {XBinary::FT_TAR_LZOP, "tar.lzo", QString("tar.lzo")},
    {XBinary::FT_TAR_XZ, "tar.xz", QString("tar.xz")},
    {XBinary::FT_TAR_Z, "tar.Z", QString("tar.Z")},
    {XBinary::FT_TAR_ZSTD, "tar.zst", QString("tar.zst")},
    {XBinary::FT_TEXT, "Text", QObject::tr("Text")},
    {XBinary::FT_TIFF, "TIFF", QString("TIFF")},
    {XBinary::FT_PAL, "PAL", QString("PAL")},
    {XBinary::FT_UNICODE, "Unicode", QString("Unicode")},
    {XBinary::FT_UNICODE_BE, "UnicodeBE", QString("Unicode BE")},
    {XBinary::FT_UNICODE_LE, "UnicodeLE", QString("Unicode LE")},
    {XBinary::FT_XML, "XML", QString("XML")},
    {XBinary::FT_UTF8, "UTF8", QString("UTF8")},
    {XBinary::FT_VIDEO, "Video", QObject::tr("Video")},
    {XBinary::FT_WEBP, "WebP", QString("WebP")},
    {XBinary::FT_WAV, "WAV", QString("WAV")},
    {XBinary::FT_ZIP, "ZIP", QString("ZIP")},
    {XBinary::FT_ZLIB, "zlib", QString("zlib")},
    {XBinary::FT_DEB, "deb", QString("deb")},
    {XBinary::FT_BWDOS16M, "BW DOS16M", QString("BW DOS16M")},
    {XBinary::FT_JAVACLASS, "Java Class", QString("Java Class")},
    {XBinary::FT_PYC, "Python Bytecode", QString("Python Bytecode")},
    {XBinary::FT_TTF, "TTF", QString("TTF")},
    {XBinary::FT_DJVU, "DjVu", QString("DjVu")},
    {XBinary::FT_CFBF, "CFBF", QString("CFBF")},
    {XBinary::FT_SZDD, "SZDD", QString("SZDD")},
    {XBinary::FT_BZIP2, "BZip2", QString("BZip2")},
    {XBinary::FT_ZSTD, "Zstandard", QString("Zstandard")},
    {XBinary::FT_XZ, "XZ", QString("XZ")},
    {XBinary::FT_LZIP, "LZIP", QString("Lzip (LZMA)")},
    {XBinary::FT_LZO, "LZO", QString("LZO (lzop)")},
    {XBinary::FT_COMPRESS, "Compress", QString("Unix compress (.Z)")},
    {XBinary::FT_CPIO, "CPIO", QString("CPIO")},
    {XBinary::FT_MINIDUMP, "MiniDump", QString("Windows MiniDump")},
    {XBinary::FT_DMG, "DMG", QString("Apple Disk Image")},
    {XBinary::FT_ARC, "ARC", QString("ARC")},
    {XBinary::FT_FREEARC, "FreeARC", QString("FreeARC")},
    {XBinary::FT_ARJ, "ARJ", QString("ARJ")},
    {XBinary::FT_ACE, "ACE", QString("ACE")},
    {XBinary::FT_BROTLI, "Brotli", QString("Brotli")},
};

XBinary::XIDSTRING _TABLE_XBinary_VT[] = {
    {XBinary::VT_UNKNOWN, "Unknown"},
    {XBinary::VT_STRING, "String"},
    {XBinary::VT_HEX, "Hex"},
    {XBinary::VT_DATETIME, "DateTime"},
    {XBinary::VT_A, "A"},
    {XBinary::VT_A_I, "A"},
    {XBinary::VT_U, "U"},
    {XBinary::VT_U_I, "U"},
    {XBinary::VT_UTF8, "UTF8"},
    {XBinary::VT_UTF8_I, "UTF"},
    {XBinary::VT_SIGNATURE, "Signature"},
    {XBinary::VT_VALUE, "Value"},
    {XBinary::VT_BIT, "bit"},
    {XBinary::VT_BYTE, "byte"},
    {XBinary::VT_WORD, "word"},
    {XBinary::VT_DWORD, "dword"},
    {XBinary::VT_QWORD, "qword"},
    {XBinary::VT_128, "U128"},
    {XBinary::VT_256, "U256"},
    {XBinary::VT_FPEG, "FPEG"},
    {XBinary::VT_CHAR, "char"},
    {XBinary::VT_UCHAR, "uchar"},
    {XBinary::VT_SHORT, "short"},
    {XBinary::VT_USHORT, "ushort"},
    {XBinary::VT_INT, "int"},
    {XBinary::VT_UINT, "uint"},
    {XBinary::VT_INT64, "int64"},
    {XBinary::VT_UINT8, "uint8"},
    {XBinary::VT_UINT16, "uint16"},
    {XBinary::VT_UINT32, "uint32"},
    {XBinary::VT_UINT64, "uint64"},
    {XBinary::VT_DOUBLE, "double"},
    {XBinary::VT_FLOAT, "float"},
    {XBinary::VT_PACKEDNUMBER, "PackedNumber"},
    {XBinary::VT_ULEB128, "ULEB128"},
};

XBinary::XCONVERT _TABLE_XBinary_XFTYPE[] = {
    {XBinary::XFTYPE_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XBinary::XFTYPE_HEADER, "Header", QObject::tr("Header")},
    {XBinary::XFTYPE_TABLE, "Table", QObject::tr("Table")},
};

const double XBinary::D_ENTROPY_THRESHOLD = 6.5;

QString XBinary::XCONVERT_idToTransString(quint32 nID, XCONVERT *pRecords, qint32 nRecordsSize)
{
    QString sResult;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        if (pRecords[i].nID == nID) {
            sResult = pRecords[i].sTransString;
            break;
        }
    }

    if (sResult == "") {
        sResult = tr("Unknown");
    }

    return sResult;
}

QString XBinary::XCONVERT_idToSetString(quint32 nID, XCONVERT *pRecords, qint32 nRecordsSize)
{
    QString sResult;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        if (pRecords[i].nID == nID) {
            sResult = pRecords[i].sSetString;
            break;
        }
    }

    if (sResult == "") {
        sResult = "Unknown";
    }

    return sResult;
}

QString XBinary::XCONVERT_idToFtString(quint32 nID, XCONVERT *pRecords, qint32 nRecordsSize)
{
    return XCONVERT_idToSetString(nID, pRecords, nRecordsSize).toUpper().remove(" ").remove("-");
}

quint32 XBinary::XCONVERT_ftStringToId(const QString &sString, XCONVERT *pRecords, qint32 nRecordsSize)
{
    quint32 nResult = 0;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        if (pRecords[i].sSetString.toUpper().remove(" ").remove("-") == sString.toUpper()) {
            nResult = pRecords[i].nID;
            break;
        }
    }

#ifdef QT_DEBUG
    if (nResult == 0) {
        qDebug() << "XCONVERT_ftStringToId: Not found" << sString;
    }
#endif

    return nResult;
}

QString XBinary::XCONVERT_translate(const QString &sString, XCONVERT *pRecords, qint32 nRecordsSize)
{
    QString sResult;

    if (sString != "") {
        QString _sString = sString.toLower();

        for (qint32 i = 0; i < nRecordsSize; i++) {
            if (pRecords[i].sSetString.toLower() == _sString) {
                sResult = pRecords[i].sTransString;
                break;
            }
        }

        if (sResult == "") {
            sResult = sString;
        } else {
            if (sString.at(0).isUpper()) {
                sResult[0] = sResult.at(0).toUpper();
            } else {
                sResult[0] = sResult.at(0).toLower();
            }
        }
    }

    return sResult;
}

QString XBinary::XIDSTRING_idToString(quint64 nID, XIDSTRING *pRecords, qint32 nRecordsSize)
{
    QString sResult;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        if (pRecords[i].nID == nID) {
            sResult = pRecords[i].sString;
            break;
        }
    }

    if (sResult == "") {
        sResult = "Unknown";
    }

    return sResult;
}

quint64 XBinary::XIDSTRING_ftStringToId(const QString &sString, XIDSTRING *pRecords, qint32 nRecordsSize)
{
    quint64 nResult = 0;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        if (pRecords[i].sString.toUpper().remove(" ").remove("-").remove("/").remove("\\") == sString.toUpper()) {
            nResult = pRecords[i].nID;
            break;
        }
    }

#ifdef QT_DEBUG
    if (nResult == 0) {
        qDebug() << "XIDSTRING_ftStringToId: Not found" << sString;
    }
#endif

    return nResult;
}

QMap<quint64, QString> XBinary::XIDSTRING_createMap(XIDSTRING *pRecords, qint32 nRecordsSize)
{
    QMap<quint64, QString> mapResult;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        mapResult.insert(pRecords[i].nID, pRecords[i].sString);
    }

    return mapResult;
}

QMap<quint64, QString> XBinary::XIDSTRING_createMapPrefix(XIDSTRING *pRecords, qint32 nRecordsSize, const QString &sPrefix)
{
    QMap<quint64, QString> mapResult;

    for (qint32 i = 0; i < nRecordsSize; i++) {
        mapResult.insert(pRecords[i].nID, sPrefix + pRecords[i].sString);
    }

    return mapResult;
}

qint64 XBinary::getNumberOfArchiveRecords(PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    UNPACK_STATE state = {};
    QMap<UNPACK_PROP, QVariant> mapProperties;

    // Initialize packing (this writes signature/header)
    if (initUnpack(&state, mapProperties, pPdStruct)) {
        nResult = state.nNumberOfRecords;
    }

    return nResult;
}

QList<XBinary::ARCHIVERECORD> XBinary::getArchiveRecords(qint32 nLimit, PDSTRUCT *pPdStruct)
{
    QList<XBinary::ARCHIVERECORD> listResult;

    PDSTRUCT pdStructEmpty = createPdStruct();
    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    // Initialize unpack state
    UNPACK_STATE state = {};
    QMap<UNPACK_PROP, QVariant> mapProperties;

    if (initUnpack(&state, mapProperties, pPdStruct)) {
        qint32 nCount = 0;

        // Iterate through records using streaming API
        while (isPdStructNotCanceled(pPdStruct) && (state.nCurrentIndex < state.nNumberOfRecords)) {
            // Get current record info
            ARCHIVERECORD record = infoCurrent(&state, pPdStruct);

            listResult.append(record);
            nCount++;

            // Check limit
            if (nLimit != -1 && nCount >= nLimit) {
                break;
            }

            // Move to next record
            if (!moveToNext(&state, pPdStruct)) {
                break;
            }
        }

        // Clean up
        finishUnpack(&state, pPdStruct);
    }

    return listResult;
}

bool XBinary::packFolderToDevice(QIODevice *pDevice, const QMap<PACK_PROP, QVariant> &mapProperties, const QString &sFolderName, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = createPdStruct();
    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    // Validate input
    if (!isDirectoryExists(sFolderName)) {
        return false;
    }

    PACK_STATE state = {};

    QMap<PACK_PROP, QVariant> _mapProperties = mapProperties;

    if (!_mapProperties.contains(PACK_PROP_PATHMODE)) {
        _mapProperties.insert(PACK_PROP_PATHMODE, PATH_MODE_RELATIVE);
    }

    if (!_mapProperties.contains(PACK_PROP_BASEPATH)) {
        _mapProperties.insert(PACK_PROP_BASEPATH, sFolderName);
    }

    // Initialize packing (this writes signature/header)
    if (!initPack(&state, pDevice, _mapProperties, pPdStruct)) {
        return false;
    }

    // Add entire folder contents
    bResult = addFolder(&state, sFolderName, pPdStruct);

    // Finish packing (writes end marker and cleans up)
    // Always call finishPack() even if addFolder() failed, to ensure cleanup
    bool bFinish = finishPack(&state, pPdStruct);

    // Return true only if both operations succeeded
    return bResult && bFinish;
}

QList<QString> XBinary::getListOfArchiveRecordNames(const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct)
{
    QList<QString> listResult;

    PDSTRUCT pdStructEmpty = createPdStruct();
    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    // Initialize unpack state
    UNPACK_STATE state = {};

    if (initUnpack(&state, mapProperties, pPdStruct)) {
        // Iterate through records using streaming API
        while (isPdStructNotCanceled(pPdStruct) && (state.nCurrentIndex < state.nNumberOfRecords)) {
            // Get current record info
            ARCHIVERECORD record = infoCurrent(&state, pPdStruct);

            // Extract the record name from properties
            if (record.mapProperties.contains(FPART_PROP_ORIGINALNAME)) {
                QString sRecordName = record.mapProperties.value(FPART_PROP_ORIGINALNAME).toString();
                if (!sRecordName.isEmpty()) {
                    listResult.append(sRecordName);
                }
            }

            // Move to next record
            if (!moveToNext(&state, pPdStruct)) {
                break;
            }
        }

        // Clean up
        finishUnpack(&state, pPdStruct);
    }

    return listResult;
}

qint32 XBinary::_readDevice(char *pBuffer, qint32 nBufferSize, DATAPROCESS_STATE *pState)
{
    qint32 nRead = pState->pDeviceInput->read(pBuffer, nBufferSize);

    pState->nCountInput += nRead;

    if (nRead != nBufferSize) {
        pState->bReadError = true;
    }

    return nRead;
}

qint32 XBinary::_readDevice(DATAPROCESS_STATE *pState)
{
    qint32 nRead = pState->pDeviceInput->read(pState->pInputBuffer, pState->nInputBufferSize);

    pState->nCountInput += nRead;

    return nRead;
}

qint32 XBinary::_writeDevice(char *pBuffer, qint32 nBufferSize, DATAPROCESS_STATE *pState)
{
    qint64 nRealSize = 0;
    qint64 nSkip = 0;

    if (pState->nProcessedOffset == 0 && (pState->nProcessedLimit == -1)) {
        nRealSize = nBufferSize;
        nSkip = 0;
    } else if (pState->nProcessedOffset == 0 && (pState->nProcessedLimit != -1)) {
        // Start of output with a cap: limit total output to nProcessedLimit bytes
        nRealSize = nBufferSize;
        if ((pState->nCountOutput + nRealSize) > pState->nProcessedLimit) {
            nRealSize = pState->nProcessedLimit - pState->nCountOutput;
            if (nRealSize < 0) {
                nRealSize = 0;
            }
        }
    } else if (pState->nProcessedOffset > 0) {
        nSkip = pState->nProcessedOffset;  // TODO fix
        nRealSize = nBufferSize - nSkip;

        if (nRealSize < 0) {
            nRealSize = 0;
        }

        if (pState->nProcessedLimit != -1) {
            if ((pState->nProcessedOffset + nRealSize) > pState->nProcessedLimit) {
                nRealSize = pState->nProcessedLimit - pState->nProcessedOffset;
            }
        }
    } else {
        nRealSize = nBufferSize;
    }

    if ((nRealSize > 0) && (pState->pDeviceOutput)) {
        qint64 nWritten = pState->pDeviceOutput->write(pBuffer + nSkip, nRealSize);

        if (nWritten != nRealSize) {
            pState->bWriteError = true;
        }
    }

    pState->nCountOutput += nBufferSize;

    return nBufferSize;
}

quint32 XBinary::getFPART_crc32(const FPART &fpart)
{
    QByteArray buffer;
    QDataStream stream(&buffer, QIODevice::WriteOnly);

    // Serialize basic types
    stream << fpart.nFileOffset;
    stream << fpart.nFileSize;
    stream << fpart.nVirtualAddress;
    stream << fpart.nVirtualSize;
    stream << fpart.sName;
    stream << static_cast<qint32>(fpart.filePart);

    // Serialize mapProperties
    stream << fpart.mapProperties;

    quint32 crc = _getCRC32(buffer.constData(), buffer.size(), 0xFFFFFFFF, XBinary::_getCRC32Table_EDB88320());

    return crc;
}

XBinary::DATA_HEADER XBinary::_searchDataHeaderById(FT fileType, quint32 nID, const QList<DATA_HEADER> &listDataHeaders)
{
    XBinary::DATA_HEADER result = {};

    qint32 nNumberOfRecords = listDataHeaders.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((listDataHeaders.at(i).dsID.fileType == fileType) && (listDataHeaders.at(i).dsID.nID == nID)) {
            result = listDataHeaders.at(i);
            break;
        }
    }

    return result;
}

XBinary::DATA_HEADER XBinary::_searchDataHeaderByGuid(const QString &sGUID, const QList<DATA_HEADER> &listDataHeaders)
{
    XBinary::DATA_HEADER result = {};

    qint32 nNumberOfRecords = listDataHeaders.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (listDataHeaders.at(i).dsID.sGUID == sGUID) {
            result = listDataHeaders.at(i);
            break;
        }
    }

    return result;
}

XBinary::FPART XBinary::findParentFPart(const QList<FPART> &listHRegions, const FPART &hRegion)
{
    FPART result = {};

    qint32 nNumberOfRegions = listHRegions.size();

    for (qint32 i = 0; i < nNumberOfRegions; i++) {
        // if hRegion inside parent hRegion
        if ((listHRegions[i].nVirtualAddress <= hRegion.nVirtualAddress) &&
            (listHRegions[i].nVirtualAddress + listHRegions[i].nVirtualSize >= hRegion.nVirtualAddress + hRegion.nVirtualSize)) {
            result = listHRegions[i];
            break;
        }
    }

    return result;
}

XBinary::DATA_RECORD XBinary::getDataRecordDV(qint64 nRelOffset, qint64 nSize, const QString &sName, VT valType, quint32 nFlags, ENDIAN endian,
                                              QMap<quint64, QString> mapValues, VL_TYPE vlType)
{
    XBinary::DATA_RECORD result = getDataRecord(nRelOffset, nSize, sName, valType, nFlags, endian);

    DATAVALUESET dataValueSet;
    dataValueSet.mapValues = mapValues;
    dataValueSet.vlType = vlType;
    dataValueSet.nMask = 0xFFFFFFFFFFFFFFFF;

    result.listDataValueSets.append(dataValueSet);

    return result;
}

QString XBinary::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XBINARY_STRUCTID, sizeof(_TABLE_XBINARY_STRUCTID) / sizeof(XBinary::XCONVERT));
}

quint32 XBinary::ftStringToStructID(const QString &sFtString)
{
    return XCONVERT_ftStringToId(sFtString, _TABLE_XBINARY_STRUCTID, sizeof(_TABLE_XBINARY_STRUCTID) / sizeof(XBinary::XCONVERT));
}

QList<XBinary::XFHEADER> XBinary::_getXFHeaders(PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    XFSTRUCT xfStruct = {};
    xfStruct.bIsParent = true;
    xfStruct.fileType = getFileType();
    xfStruct.nStructID = 0;
    xfStruct.pMemoryMap = &memoryMap;

    return getXFHeaders(xfStruct, pPdStruct);
}

XBinary::DATA_HEADER XBinary::_dataHeaderHex(const DATA_HEADERS_OPTIONS &dataHeadersOptions, const QString &sName, const DSID &dsID_parent, quint32 nID, qint64 nOffset,
                                             qint64 nSize)
{
    XBinary::DATA_HEADER result = _initDataHeader(dataHeadersOptions, sName);
    result.dsID_parent = dsID_parent;
    result.dsID.nID = nID;
    result.nLocation = nOffset;
    result.nSize = nSize;
    result.locType = LT_OFFSET;
    result.dhMode = DHMODE_HEX;
    result.nCount = 1;  // Hex view is always one record

    return result;
}

XBinary::DATA_HEADER XBinary::_initDataHeader(const DATA_HEADERS_OPTIONS &dataHeadersOptions, const QString &sName)
{
    DATA_HEADER result = {};

    result.dsID_parent = dataHeadersOptions.dsID_parent;
    result.dsID.sGUID = generateUUID();
    result.dsID.fileType = dataHeadersOptions.fileType;
    result.dsID.nID = dataHeadersOptions.nID;
    result.locType = dataHeadersOptions.locType;
    result.nLocation = dataHeadersOptions.nLocation;
    result.sName = sName;
    result.dhMode = dataHeadersOptions.dhMode;
    result.nSize = dataHeadersOptions.nSize;
    result.nCount = dataHeadersOptions.nCount;

    if (result.nCount == 0) {
        result.nCount = 1;
    }

    if (result.dsID.fileType == FT_UNKNOWN) {
        result.dsID.fileType = dataHeadersOptions.pMemoryMap->fileType;
    }

    return result;
}

XBinary::DSID XBinary::_addDefaultHeaders(QList<DATA_HEADER> *pListHeaders, PDSTRUCT *pPdStruct)
{
    DATA_HEADER dhInfo = {};
    dhInfo.dsID_parent.fileType = FT_UNKNOWN;
    dhInfo.dsID_parent.nID = 0;
    dhInfo.dsID_parent.sGUID = generateUUID();
    dhInfo.dsID.fileType = getFileType();
    dhInfo.dsID.nID = 0;
    dhInfo.dsID.sGUID = generateUUID();
    dhInfo.sName = QObject::tr("Info");
    dhInfo.locType = LT_UNKNOWN;
    dhInfo.nLocation = 0;
    dhInfo.nSize = -1;

    pListHeaders->append(dhInfo);

    if (XBinary::isPdStructNotCanceled(pPdStruct)) {
        DATA_HEADER dhGeneric = {};
        dhGeneric.dsID_parent = dhInfo.dsID;
        dhGeneric.dsID.fileType = FT_BINARY;
        dhGeneric.locType = LT_UNKNOWN;
        dhGeneric.nLocation = 0;
        dhGeneric.nSize = -1;

        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_NFDSCAN;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_DIESCAN;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_YARASCAN;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_VIRUSTOTALSCAN;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_VISUALIZATION;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_HEX;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (isExecutable() && XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_DISASM;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (isArchive() && XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_ARCHIVE;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_HASH;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_STRINGS;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_SIGNATURES;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_REGIONS;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_MEMORYMAP;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_SYMBOLS;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_ENTROPY;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_EXTRACTOR;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (XBinary::isPdStructNotCanceled(pPdStruct)) {
            dhGeneric.dsID.nID = STRUCTID_SEARCH;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
        if (getFileParts(FILEPART_OVERLAY, 1, pPdStruct).count()) {
            dhGeneric.dsID.nID = STRUCTID_OVERLAY;
            dhGeneric.dsID.sGUID = generateUUID();
            dhGeneric.sName = XBinary::structIDToString(dhGeneric.dsID.nID);
            pListHeaders->append(dhGeneric);
        }
    }

    return dhInfo.dsID;
}

qint32 XBinary::getDataRecordValues(const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles,
                                    PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    if (pListTitles) {
        pListTitles->clear();
    }

    if (dataRecordsOptions.dataHeaderFirst.dhMode == DHMODE_HEADER) {
        DATA_RECORD_ROW dataRecordRow = {};
        dataRecordRow.locType = dataRecordsOptions.dataHeaderFirst.locType;
        dataRecordRow.nLocation = dataRecordsOptions.dataHeaderFirst.nLocation;

        DATA_HEADER dataHeader = {};

        if (_isFlagPresentInRecords(&(dataRecordsOptions.dataHeaderFirst.listRecords), DRF_VOLATILE)) {
            DATA_HEADERS_OPTIONS dataHeadersOptions = {};
            dataHeadersOptions.pMemoryMap = dataRecordsOptions.pMemoryMap;
            dataHeadersOptions.dsID_parent = dataRecordsOptions.dataHeaderFirst.dsID_parent;
            dataHeadersOptions.fileType = dataRecordsOptions.dataHeaderFirst.dsID.fileType;
            dataHeadersOptions.nID = dataRecordsOptions.dataHeaderFirst.dsID.nID;
            dataHeadersOptions.locType = dataRecordsOptions.dataHeaderFirst.locType;
            dataHeadersOptions.nLocation = dataRecordsOptions.dataHeaderFirst.nLocation;
            dataHeadersOptions.bChildren = false;
            dataHeadersOptions.dhMode = DHMODE_HEADER;

            QList<DATA_HEADER> listDataHeaders = getDataHeaders(dataHeadersOptions, pPdStruct);

            if (listDataHeaders.count()) {
                dataHeader = listDataHeaders.at(0);
            }
        } else {
            dataHeader = dataRecordsOptions.dataHeaderFirst;
        }

        qint64 nStartOffset = locationToOffset(dataRecordsOptions.pMemoryMap, dataHeader.locType, dataHeader.nLocation);
        qint32 nNumberOfRecords = dataHeader.listRecords.count();

        qint32 nMax = 0;

        for (qint32 j = 0; (j < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); j++) {
            DATA_RECORD dataRecord = dataHeader.listRecords.at(j);

            QVariant variant =
                read_value(dataRecord.valType, nStartOffset + dataRecord.nRelOffset, dataRecord.nSize, dataRecord.endian == XBinary::ENDIAN_BIG, pPdStruct);

            dataRecordRow.listValues.append(variant);

            if (pListTitles) {
                pListTitles->append(dataRecord.sName);
            }

            if (dataRecord.nRelOffset + dataRecord.nSize > nMax) {
                nMax = dataRecord.nRelOffset + dataRecord.nSize;
            }
        }

        dataRecordRow.nSize = nMax;

        pListDataRecords->append(dataRecordRow);

        nResult = nMax;
    } else if (dataRecordsOptions.dataHeaderFirst.dhMode == DHMODE_TABLE) {
        qint32 nCount = dataRecordsOptions.dataHeaderFirst.nCount;
        XADDR nLocation = dataRecordsOptions.dataHeaderFirst.nLocation;

        if (pListTitles) {
            pListTitles->clear();
            pListTitles->append(getTableTitles(dataRecordsOptions));
        }

        void *pUserData = nullptr;
        if (readTableInit(dataRecordsOptions, &pUserData, pPdStruct)) {
            for (qint32 i = 0; (i < nCount) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
                QList<DATA_RECORD_ROW> listDataRecordRows;

                qint32 nResultRead =
                    readTableRow(i, dataRecordsOptions.dataHeaderFirst.locType, nLocation, dataRecordsOptions, &listDataRecordRows, pUserData, pPdStruct);

                pListDataRecords->append(listDataRecordRows);

                nLocation += nResultRead;
            }

            readTableFinalize(dataRecordsOptions, pUserData, pPdStruct);
        }

        nResult = nLocation - dataRecordsOptions.dataHeaderFirst.nLocation;
    }

    return nResult;
}

QList<QString> XBinary::getDataRecordComments(const DATA_RECORDS_OPTIONS &dataRecordsOptions, const DATA_RECORD_ROW &dataRecordRow, PDSTRUCT *pPdStruct)
{
    QList<QString> listComments;

    qint32 nNumberOfRecords = dataRecordsOptions.dataHeaderFirst.listRecords.count();

    for (qint32 j = 0; (j < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); j++) {
        DATA_RECORD dataRecord = dataRecordsOptions.dataHeaderFirst.listRecords.at(j);

        QString sComment;

        if (dataRecord.nFlags & DRF_SIZE) {
            sComment = appendText(bytesCountToString(dataRecordRow.listValues.at(j).toULongLong(), 1024), sComment, ", ");
        }

        if (dataRecord.nFlags & DRF_COUNT) {
            sComment = appendText(QString::number(dataRecordRow.listValues.at(j).toULongLong()), sComment, ", ");
        }

        QString sFlags;
        if (dataRecord.listDataValueSets.count()) {
            qint32 nNumberOfDataValueSets = dataRecord.listDataValueSets.count();

            for (qint32 k = 0; (k < nNumberOfDataValueSets) && XBinary::isPdStructNotCanceled(pPdStruct); k++) {
                DATAVALUESET dataValueSet = dataRecord.listDataValueSets.at(k);

                sFlags = appendText(
                    sFlags, valueToFlagsString(dataRecordRow.listValues.at(j).toULongLong() & dataValueSet.nMask, dataValueSet.mapValues, dataValueSet.vlType), "|");
            }

            sComment = appendText(sComment, sFlags, ", ");
        }

        listComments.append(sComment);
    }

    return listComments;
}

QList<QString> XBinary::getTableTitles(const DATA_RECORDS_OPTIONS &dataRecordsOptions)
{
    QList<QString> listTitles;

    qint32 nNumberOfRecords = dataRecordsOptions.dataHeaderFirst.listRecords.count();

    for (qint32 j = 0; j < nNumberOfRecords; j++) {
        DATA_RECORD dataRecord = dataRecordsOptions.dataHeaderFirst.listRecords.at(j);

        listTitles.append(dataRecord.sName);
    }

    return listTitles;
}

bool XBinary::readTableInit(const DATA_RECORDS_OPTIONS &dataRecordsOptions, void **ppUserData, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(dataRecordsOptions)
    Q_UNUSED(ppUserData)
    Q_UNUSED(pPdStruct)

    // Default implementation does nothing
    return true;
}

qint32 XBinary::readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
                             void *pUserData, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nRow)
    Q_UNUSED(pUserData)

    DATA_RECORDS_OPTIONS _dataRecordsOptions = dataRecordsOptions;
    _dataRecordsOptions.dataHeaderFirst.locType = locType;
    _dataRecordsOptions.dataHeaderFirst.nLocation = nLocation;
    _dataRecordsOptions.dataHeaderFirst.dhMode = DHMODE_HEADER;

    return getDataRecordValues(_dataRecordsOptions, pListDataRecords, nullptr, pPdStruct);
}

void XBinary::readTableFinalize(const DATA_RECORDS_OPTIONS &dataRecordsOptions, void *pUserData, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(dataRecordsOptions)
    Q_UNUSED(pUserData)
    Q_UNUSED(pPdStruct)

    // Default implementation does nothing
}

bool XBinary::_isFlagPresentInRecords(const QList<DATA_RECORD> *pListRecords, quint32 nFlag)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pListRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListRecords->at(i).nFlags & nFlag) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

QString XBinary::getCompressMethodString()
{
    return "";
}

QString XBinary::handleMethodToString(HANDLE_METHOD handleMethod)
{
    return XBinary::XCONVERT_idToTransString(handleMethod, _TABLE_XBINARY_HANDLE_METHOD, sizeof(_TABLE_XBINARY_HANDLE_METHOD) / sizeof(XBinary::XCONVERT));
}

QString XBinary::handleMethodToFtString(HANDLE_METHOD handleMethod)
{
    return XBinary::XCONVERT_idToFtString(handleMethod, _TABLE_XBINARY_HANDLE_METHOD, sizeof(_TABLE_XBINARY_HANDLE_METHOD) / sizeof(XBinary::XCONVERT));
}

XBinary::HANDLE_METHOD XBinary::ftStringToHandleMethod(const QString &sString)
{
    return (HANDLE_METHOD)XBinary::XCONVERT_ftStringToId(sString, _TABLE_XBINARY_HANDLE_METHOD, sizeof(_TABLE_XBINARY_HANDLE_METHOD) / sizeof(XBinary::XCONVERT));
}

XBinary::DATA_RECORD XBinary::getDataRecord(qint64 nRelOffset, qint64 nSize, const QString &sName, VT valType, quint32 nFlags, ENDIAN endian)
{
    XBinary::DATA_RECORD dataRecord;

    dataRecord.nRelOffset = nRelOffset;
    dataRecord.nSize = nSize;
    dataRecord.sName = sName;
    dataRecord.valType = valType;
    dataRecord.nFlags = nFlags;
    dataRecord.endian = endian;

    return dataRecord;
}

QList<XBinary::DATA_HEADER> XBinary::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<XBinary::DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == 0) {
        _addDefaultHeaders(&listResult, pPdStruct);
    }

    return listResult;
}

QList<XBinary::XFHEADER> XBinary::getXFHeaders(const XFSTRUCT &xfStruct, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(xfStruct)
    Q_UNUSED(pPdStruct)

    QList<XBinary::XFHEADER> listResult;

    return listResult;
}

QList<XBinary::XFRECORD> XBinary::getXFRecords(FT fileType, quint32 nStructID, const XLOC &xLoc)
{
    Q_UNUSED(fileType)
    Q_UNUSED(nStructID)
    Q_UNUSED(xLoc)

    QList<XBinary::XFRECORD> listResult;

    return listResult;
}

QList<QVariant> XBinary::getXFRecordValues(const QList<XFRECORD> &listXFRecords, const XLOC &xLoc)
{
    QList<QVariant> listResult;

    qint64 nBaseOffset = xLoc.nLocation;
    qint32 nCount = listXFRecords.count();

    for (qint32 i = 0; i < nCount; i++) {
        XFRECORD xfRecord = listXFRecords.at(i);
        qint64 nOffset = nBaseOffset + xfRecord.nOffset;
        QVariant varValue;

        if (xfRecord.valueType == VT_UINT8) {
            varValue = read_uint8(nOffset);
        } else if (xfRecord.valueType == VT_INT8) {
            varValue = read_int8(nOffset);
        } else if (xfRecord.valueType == VT_UINT16) {
            varValue = read_uint16(nOffset);
        } else if (xfRecord.valueType == VT_INT16) {
            varValue = read_int16(nOffset);
        } else if (xfRecord.valueType == VT_UINT32) {
            varValue = read_uint32(nOffset);
        } else if (xfRecord.valueType == VT_INT32) {
            varValue = read_int32(nOffset);
        } else if (xfRecord.valueType == VT_UINT64) {
            varValue = read_uint64(nOffset);
        } else if (xfRecord.valueType == VT_INT64) {
            varValue = read_int64(nOffset);
        } else if (xfRecord.valueType == VT_FLOAT) {
            varValue = read_float(nOffset);
        } else if (xfRecord.valueType == VT_DOUBLE) {
            varValue = read_double(nOffset);
        } else if (xfRecord.valueType == VT_BYTE_ARRAY) {
            varValue = read_array(nOffset, xfRecord.nSize);
        } else if (xfRecord.valueType == VT_CHAR_ARRAY) {
            varValue = read_ansiString(nOffset, xfRecord.nSize);
        } else {
            varValue = read_array(nOffset, xfRecord.nSize);
        }

        listResult.append(varValue);
    }

    return listResult;
}

QString XBinary::xfHeaderToTag(const XFHEADER &xfHeader, const QString &sStructName, const QString &sParentTag)
{
    QString sOffset = valueToHexEx(xfHeader.xLoc.nLocation);
    QString sFileTypeFt = fileTypeIdToFtString(xfHeader.fileType);
    QString sStructFt = QString(sStructName).toUpper().remove(" ").remove("-");

    QString sTypeName;

    if (xfHeader.xfType == XFTYPE_HEADER) {
        sTypeName = "HEADER";
    } else if (xfHeader.xfType == XFTYPE_TABLE) {
        sTypeName = "TABLE";
    } else {
        sTypeName = "UNKNOWN";
    }

    QString sResult = sOffset + "::" + sFileTypeFt + "::" + sStructFt + "::" + sTypeName;

    if (xfHeader.xfType == XFTYPE_TABLE) {
        sResult += "::" + QString::number(xfHeader.listRowLocations.count());
    }

    if (xfHeader.bIsParentNeeded && (sParentTag != "")) {
        sResult = sParentTag + "#" + sResult;
    }

    return sResult;
}

QString XBinary::xfHeaderToString(const XFHEADER &xfHeader, const QString &sStructName, const QString &sParentString)
{
    QString sResult;

    if (xfHeader.fileType != FT_UNKNOWN) {
        sResult += QString("%1::").arg(fileTypeIdToFtString(xfHeader.fileType));
    }

    sResult += QString(sStructName).toUpper().remove(" ").remove("-");

    QList<QString> listParams;

    if (xfHeader.xfType != XFTYPE_UNKNOWN) {
        listParams.append(QString("type=%1").arg(xfTypeIdToFtString(xfHeader.xfType)));
    }

    if (xfHeader.xLoc.nLocation > 0) {
        listParams.append(QString("offset=0x%1").arg(valueToHexEx(xfHeader.xLoc.nLocation)));
    }

    if (xfHeader.nSize > 0) {
        listParams.append(QString("size=0x%1").arg(valueToHexEx(xfHeader.nSize)));
    }

    if (xfHeader.listRowLocations.count() > 0) {
        listParams.append(QString("rows=0x%1").arg(valueToHexEx(xfHeader.listRowLocations.count())));
    }

    if (xfHeader.bIsParentNeeded && (sParentString != "")) {
        sResult = sParentString + "#" + sResult;
    }

    if (listParams.count()) {
        sResult += "?" + listParams.join("&");
    }

    return sResult;
}

XBinary::XBinary(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    setData(pDevice, bIsImage, nModuleAddress);
}

XBinary::XBinary(const QString &sFileName)
{
    m_sFileName = sFileName;

    if (m_pFile) {
        m_pFile->close();

        delete m_pFile;
    }

    m_pFile = new QFile(sFileName);

    tryToOpen(m_pFile);

    setData(m_pFile);
}

XBinary::~XBinary()
{
    if (m_pFile) {
        m_pFile->close();

        delete m_pFile;
    }
}

void XBinary::setData(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    m_pReadWriteMutex = nullptr;
    m_nSize = 0;
    m_nFileFormatSize = 0;
    m_pFile = nullptr;
    m_pConstMemory = nullptr;

    setDevice(pDevice);
    setIsImage(bIsImage);
    XBinary::setBaseAddress(0);
    setModuleAddress(nModuleAddress);
    setEndian(ENDIAN_UNKNOWN);
    XBinary::setEntryPointOffset(0);
    setMode(MODE_UNKNOWN);
    setFileType(FT_BINARY);
    setArch("NOEXEC");
    setVersion("");
    setType(TYPE_UNKNOWN);
    setOsType(OSNAME_UNKNOWN);
    setOsVersion("");
    setMultiSearchCallbackState(false);
    setIsExecutable(false);
    setIsArchive(false);

    if (pDevice) {
        // qDebug("%s",XBinary::valueToHex((quint64)pDevice).toLatin1().data());
        setFileFormatSize(pDevice->size());
    }
}

void XBinary::setDevice(QIODevice *pDevice)
{
    m_pDevice = pDevice;

    if (m_pDevice) {
        QBuffer *pBuffer = dynamic_cast<QBuffer *>(pDevice);

        if (pBuffer) {
            m_pConstMemory = pBuffer->data().data();
        } else {
            m_pConstMemory = (const char *)(pDevice->property("Memory").toULongLong());
        }

        if (m_pReadWriteMutex) m_pReadWriteMutex->lock();

        // qDebug("%s",XBinary::valueToHex((quint64)m_pDevice).toLatin1().data());

        m_nSize = m_pDevice->size();

        if (m_pReadWriteMutex) m_pReadWriteMutex->unlock();
    }
}

void XBinary::setReadWriteMutex(QMutex *pReadWriteMutex)
{
    m_pReadWriteMutex = pReadWriteMutex;
}

void XBinary::setFileName(const QString &sFileName)
{
    m_sFileName = sFileName;
}

qint64 XBinary::safeReadData(QIODevice *pDevice, qint64 nPos, char *pData, qint64 nMaxLen, PDSTRUCT *pPdStruct)
{
    // qDebug("%X %X pos: %X maxlen: %X", this, pDevice, nPos, nMaxLen);
    qint64 nResult = 0;

    if (m_pReadWriteMutex) m_pReadWriteMutex->lock();

    if ((pDevice->size() > nPos) && (nPos >= 0)) {
        if (pDevice->seek(nPos)) {
            qint64 nBufferSize = getBufferSize(pPdStruct);

            while ((nMaxLen > 0) && isPdStructNotCanceled(pPdStruct)) {
                qint64 nCurrentSize = qMin(nMaxLen, nBufferSize);

                nCurrentSize = pDevice->read(pData, nCurrentSize);

                if (nCurrentSize <= 0) {
                    break;
                }

                nMaxLen -= nCurrentSize;
                pData += nCurrentSize;
                nResult += nCurrentSize;
            }
        } else {
#ifdef QT_DEBUG
            qDebug("Cannot seek");
#endif
        }

    } else {
#ifdef QT_DEBUG
        qDebug("Invalid pos: %llX Size: %llX", nPos, getSize());
#endif
    }

    if (m_pReadWriteMutex) m_pReadWriteMutex->unlock();

    return nResult;
}

qint64 XBinary::safeWriteData(QIODevice *pDevice, qint64 nPos, const char *pData, qint64 nLen, PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    if (m_pReadWriteMutex) m_pReadWriteMutex->lock();

    if (pDevice->size() > nPos) {
        if (pDevice->seek(nPos)) {
            qint64 nBufferSize = getBufferSize(pPdStruct);

            while ((nLen > 0) && isPdStructNotCanceled(pPdStruct)) {
                qint64 nCurrentSize = qMin(nLen, nBufferSize);

                nCurrentSize = pDevice->write(pData, nCurrentSize);

                if (nCurrentSize == 0) {
                    break;
                }

                nLen -= nCurrentSize;
                pData += nCurrentSize;
                nResult += nCurrentSize;
            }
        }
    }

    if (m_pReadWriteMutex) m_pReadWriteMutex->unlock();

    return nResult;
}

qint64 XBinary::_readDataSimple(QIODevice *pDevice, qint64 nPos, char *pData, qint64 nMaxLen)
{
    // qDebug("%X %X pos: %X maxlen: %X", this, pDevice, nPos, nMaxLen);
    qint64 nResult = 0;

    if (m_pReadWriteMutex) m_pReadWriteMutex->lock();

    if ((pDevice->size() > nPos) && (nPos >= 0)) {
        if (pDevice->seek(nPos)) {
            nResult = pDevice->read(pData, nMaxLen);
        } else {
#ifdef QT_DEBUG
            qDebug("Cannot seek");
#endif
        }
    } else {
#ifdef QT_DEBUG
        qDebug("Invalid pos: %llX Size: %llX", nPos, getSize());
#endif
    }

    if (m_pReadWriteMutex) m_pReadWriteMutex->unlock();

    return nResult;
}

QByteArray XBinary::_readDataSimple(QIODevice *pDevice, qint64 nPos, qint64 nSize)
{
    QByteArray baResult;

    XBinary::OFFSETSIZE osRegion = convertOffsetAndSize(nPos, nSize);

    if (osRegion.nOffset != -1) {
        baResult.resize((qint32)osRegion.nSize);

        qint64 nBytes = _readDataSimple(pDevice, osRegion.nOffset, baResult.data(), osRegion.nSize);

        if (osRegion.nSize != nBytes) {
            baResult.resize((qint32)nBytes);
        }
    }

    return baResult;
}

qint64 XBinary::_writeDataSimple(QIODevice *pDevice, qint64 nPos, const char *pData, qint64 nLen)
{
    qint64 nResult = 0;

    if (m_pReadWriteMutex) m_pReadWriteMutex->lock();

    if (pDevice->size() > nPos) {
        if (pDevice->seek(nPos)) {
            nResult = pDevice->write(pData, nLen);
        } else {
#ifdef QT_DEBUG
            qDebug("Cannot seek");
#endif
        }
    } else {
#ifdef QT_DEBUG
        qDebug("Invalid pos: %llX Size: %llX", nPos, getSize());
#endif
    }

    if (m_pReadWriteMutex) m_pReadWriteMutex->unlock();

    return nResult;
}

qint64 XBinary::getSize()
{
    return m_nSize;
}

qint64 XBinary::getSize(QIODevice *pDevice)
{
    XBinary binary(pDevice);

    return binary.getSize();
}

qint64 XBinary::getSize(const QString &sFileName)
{
    qint64 nResult = 0;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        nResult = getSize(&file);

        file.close();
    }

    return nResult;
}

void XBinary::setMode(XBinary::MODE mode)
{
    m_mode = mode;
}

XBinary::MODE XBinary::getMode()
{
    return m_mode;
}

void XBinary::setType(qint32 nType)
{
    m_nType = nType;
}

qint32 XBinary::getType()
{
    return m_nType;
}

QString XBinary::typeIdToString(qint32 nType)
{
    QString sResult = tr("Unknown");

    switch (nType) {
        case TYPE_UNKNOWN: sResult = tr("Unknown");
    }

    return sResult;
}

QString XBinary::getTypeAsString()
{
    return typeIdToString(getType());
}

void XBinary::setFileType(XBinary::FT fileType)
{
    m_fileType = fileType;
}

XBinary::FT XBinary::getFileType()
{
    return m_fileType;
}

QString XBinary::modeIdToString(XBinary::MODE mode)
{
    QString sResult = tr("Unknown");

    switch (mode) {
        case MODE_UNKNOWN: sResult = tr("Unknown"); break;
        case MODE_DATA: sResult = QString("Data"); break;
        case MODE_BIT: sResult = QString("BIT"); break;
        case MODE_8: sResult = tr("8-bit"); break;
        case MODE_16: sResult = tr("16-bit"); break;
        case MODE_16SEG: sResult = QString("16SEG"); break;
        case MODE_32: sResult = tr("32-bit"); break;
        case MODE_64: sResult = tr("64-bit"); break;
        case MODE_128: sResult = tr("128-bit"); break;
        case MODE_256: sResult = tr("256-bit"); break;
        case MODE_FREG: sResult = QString("freg"); break;
    }

    return sResult;
}

QString XBinary::endianToString(ENDIAN endian)
{
    QString sResult;

    if (endian == ENDIAN_UNKNOWN) {
        sResult = tr("Unknown");
    } else if (endian == ENDIAN_BIG) {
        sResult = "BE";
    } else if (endian == ENDIAN_LITTLE) {
        sResult = "LE";
    }

    return sResult;
}

void XBinary::setArch(const QString &sArch)
{
    m_sArch = sArch;
}

QString XBinary::getArch()
{
    return m_sArch;
}

QString XBinary::getFileFormatString(const FILEFORMATINFO *pFileFormatInfo)
{
    QString sResult = fileTypeIdToString(pFileFormatInfo->fileType);

    if (pFileFormatInfo->sVersion != "") {
        sResult += QString("(%1)").arg(pFileFormatInfo->sVersion);
    }

    QString sInfo = getFileFormatInfoString(pFileFormatInfo);

    if (sInfo != "") {
        sResult += QString("[%1]").arg(sInfo);
    }

    return sResult;
}

QString XBinary::getFileFormatInfoString(const FILEFORMATINFO *pFileFormatInfo)
{
    QString sResult;

    if (pFileFormatInfo->bIsEncrypted) {
        sResult = appendText(sResult, QObject::tr("Encrypted"), ", ");
    }

    if (pFileFormatInfo->sCompresionMethod != "") {
        sResult = appendText(sResult, pFileFormatInfo->sCompresionMethod, ", ");
    }

    if (pFileFormatInfo->sInfo != "") {
        sResult = appendText(sResult, pFileFormatInfo->sInfo, ", ");
    }

    // TODO

    return sResult;
}

XBinary::OSNAME XBinary::getOsName(const FILEFORMATINFO *pFileFormatInfo)
{
    return pFileFormatInfo->osName;
}

void XBinary::setFileFormatExt(const QString &sFileFormatExt)
{
    m_sFileFormatExt = sFileFormatExt;
}

QString XBinary::getFileFormatExt()
{
#ifdef QT_DEBUG
    // qDebug("TODO: XBinary::getFileFormatExt()");
#endif
    return m_sFileFormatExt;
}

void XBinary::setFileFormatExtsString(const QString &sFileFormatExts)
{
    m_sFileFormatExts = sFileFormatExts;
}

QString XBinary::getFileFormatExtsString()
{
    if (m_sFileFormatExts == "") {
#ifdef QT_DEBUG
        qDebug("TODO: XBinary::getFileFormatExtsString()");
#endif
    }

    return m_sFileFormatExts;
}

void XBinary::setFileFormatSize(qint64 nFileFormatSize)
{
    m_nFileFormatSize = nFileFormatSize;
}

bool XBinary::setFileDateTime(const QString &sFileName, const QDateTime &dateTime)
{
    // Try to set both modification and access times via Qt API (member function)
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QFile file;
    file.setFileName(sFileName);
    bool bMod = file.setFileTime(dateTime, QFileDevice::FileModificationTime);
    bool bAccess = file.setFileTime(dateTime, QFileDevice::FileAccessTime);
    return (bMod || bAccess);
#else
    Q_UNUSED(sFileName)
    Q_UNUSED(dateTime)
    return false;
#endif
}

QDateTime XBinary::winFileTimeToQDateTime(quint64 nWinFileTime)
{
    // Windows FILETIME: 100-nanosecond intervals since January 1, 1601 UTC.
    // Offset to Unix epoch (January 1, 1970): 116444736000000000 * 100ns.
    if (nWinFileTime == 0) {
        return QDateTime();
    }

    const quint64 nEpochDelta = Q_UINT64_C(116444736000000000);

    if (nWinFileTime < nEpochDelta) {
        return QDateTime();
    }

    qint64 nMsecsSinceEpoch = (qint64)((nWinFileTime - nEpochDelta) / Q_UINT64_C(10000));

    return QDateTime::fromMSecsSinceEpoch(nMsecsSinceEpoch, Qt::UTC);
}

bool XBinary::setFileProperties(const QMap<FPART_PROP, QVariant> &mapProperties, const QString &sFileName)
{
    bool bResult = false;

    // --- Timestamps ---
    // QFile::setFileTime requires the file to be open; use ReadWrite so the
    // OS handle has FILE_WRITE_ATTRIBUTES rights on Windows.
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    {
        QFile file(sFileName);
        if (file.open(QIODevice::ReadWrite)) {
            if (mapProperties.contains(FPART_PROP_MTIME)) {
                QDateTime dt = mapProperties.value(FPART_PROP_MTIME).toDateTime();
                if (dt.isValid()) {
                    if (file.setFileTime(dt, QFileDevice::FileModificationTime)) {
                        bResult = true;
                    }
                }
            }
            if (mapProperties.contains(FPART_PROP_ATIME)) {
                QDateTime dt = mapProperties.value(FPART_PROP_ATIME).toDateTime();
                if (dt.isValid()) {
                    file.setFileTime(dt, QFileDevice::FileAccessTime);
                }
            }
            if (mapProperties.contains(FPART_PROP_CTIME)) {
                QDateTime dt = mapProperties.value(FPART_PROP_CTIME).toDateTime();
                if (dt.isValid()) {
                    // FileBirthTime = creation time on Windows; inode change time on Unix
                    file.setFileTime(dt, QFileDevice::FileBirthTime);
                }
            }
            file.close();
        }
    }
#endif

    // --- File attributes ---
#ifdef Q_OS_WIN
    bool bHasAttrib = mapProperties.contains(FPART_PROP_ISREADONLY) || mapProperties.contains(FPART_PROP_ISHIDDEN) || mapProperties.contains(FPART_PROP_ISSYSTEM) ||
                      mapProperties.contains(FPART_PROP_ISARCHIVE);

    if (bHasAttrib) {
        DWORD nAttribs = GetFileAttributesW((LPCWSTR)sFileName.utf16());

        if (nAttribs != INVALID_FILE_ATTRIBUTES) {
            if (mapProperties.contains(FPART_PROP_ISREADONLY)) {
                if (mapProperties.value(FPART_PROP_ISREADONLY).toBool()) {
                    nAttribs |= FILE_ATTRIBUTE_READONLY;
                } else {
                    nAttribs &= ~(DWORD)FILE_ATTRIBUTE_READONLY;
                }
            }

            if (mapProperties.contains(FPART_PROP_ISHIDDEN)) {
                if (mapProperties.value(FPART_PROP_ISHIDDEN).toBool()) {
                    nAttribs |= FILE_ATTRIBUTE_HIDDEN;
                } else {
                    nAttribs &= ~(DWORD)FILE_ATTRIBUTE_HIDDEN;
                }
            }

            if (mapProperties.contains(FPART_PROP_ISSYSTEM)) {
                if (mapProperties.value(FPART_PROP_ISSYSTEM).toBool()) {
                    nAttribs |= FILE_ATTRIBUTE_SYSTEM;
                } else {
                    nAttribs &= ~(DWORD)FILE_ATTRIBUTE_SYSTEM;
                }
            }

            if (mapProperties.contains(FPART_PROP_ISARCHIVE)) {
                if (mapProperties.value(FPART_PROP_ISARCHIVE).toBool()) {
                    nAttribs |= FILE_ATTRIBUTE_ARCHIVE;
                } else {
                    nAttribs &= ~(DWORD)FILE_ATTRIBUTE_ARCHIVE;
                }
            }

            if (SetFileAttributesW((LPCWSTR)sFileName.utf16(), nAttribs)) {
                bResult = true;
            }
        }
    }
#else
    // Unix: map ISREADONLY to write-permission bits
    if (mapProperties.contains(FPART_PROP_ISREADONLY)) {
        QFile::Permissions perms = QFile::permissions(sFileName);

        if (mapProperties.value(FPART_PROP_ISREADONLY).toBool()) {
            perms &= ~(QFile::WriteOwner | QFile::WriteUser | QFile::WriteGroup | QFile::WriteOther);
        } else {
            perms |= QFile::WriteOwner | QFile::WriteUser;
        }

        if (QFile::setPermissions(sFileName, perms)) {
            bResult = true;
        }
    }
#endif

    return bResult;
}

qint64 XBinary::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return m_nFileFormatSize;
}

bool XBinary::isSigned()
{
    return false;
}

XBinary::OFFSETSIZE XBinary::getSignOffsetSize()
{
    OFFSETSIZE osResult = {};

    return osResult;
}

void XBinary::setOsType(OSNAME osName)
{
    m_osName = osName;
}

XBinary::OSNAME XBinary::getOsName()
{
    return m_osName;
}

void XBinary::setOsVersion(const QString &sOsVersion)
{
    m_sOsVersion = sOsVersion;
}

QString XBinary::getOsVersion()
{
    return m_sOsVersion;
}

XBinary::FILEFORMATINFO XBinary::getFileFormatInfo(PDSTRUCT *pPdStruct)
{
    // TODO userData
    FILEFORMATINFO result = {};

    result.bIsValid = isValid(pPdStruct);

    if (result.bIsValid) {
        result.nSize = getSize();
        result.fileType = getFileType();
        result.sExt = getFileFormatExt();
        result.sVersion = getVersion();
        result.sInfo = getInfo(pPdStruct);
        result.osName = getOsName();
        result.sOsVersion = getOsVersion();
        result.sArch = getArch();
        result.mode = getMode();
        result.sType = typeIdToString(getType());
        result.endian = getEndian();
        result.sMIME = getMIMEString();
        result.sCompresionMethod = getCompressMethodString();
        result.bIsEncrypted = isEncrypted();

        if (result.nSize == 0) {
            result.bIsValid = false;
        }
    }

    return result;
}

void XBinary::setEndian(ENDIAN endian)
{
    m_endian = endian;
}

void XBinary::setIsExecutable(bool bIsExecutable)
{
    m_bIsExecutable = bIsExecutable;
}

void XBinary::setIsArchive(bool bIsArchive)
{
    m_bIsArchive = bIsArchive;
}

XBinary::ENDIAN XBinary::getEndian()
{
    return m_endian;
}

bool XBinary::isPacked(double dEntropy)
{
    return (dEntropy >= D_ENTROPY_THRESHOLD);  // TODO Check
}

bool XBinary::isExecutable()
{
    return m_bIsExecutable;
}

bool XBinary::isArchive()
{
    return m_bIsArchive;
}

quint8 XBinary::random8()
{
    return (quint8)random16();
}

quint16 XBinary::random16()
{
    quint16 nResult = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    nResult = (quint16)(QRandomGenerator::global()->generate());
#elif (QT_VERSION_MAJOR >= 6)
    nResult = (quint16)(QRandomGenerator::global()->generate());
#else
    static quint32 nSeed = 0;

    if (!nSeed) {
        quint32 nRValue = QDateTime::currentMSecsSinceEpoch() & 0xFFFFFFFF;

        nSeed ^= nRValue;
        qsrand(nSeed);
    }
    nResult = (quint16)qrand();
#endif

    return nResult;
}

quint32 XBinary::random32()
{
    quint16 nValue1 = random16();
    quint16 nValue2 = random16();

    return (nValue1 << 16) + nValue2;
}

quint64 XBinary::random64()
{
    quint64 nVal1 = random32();
    quint64 nVal2 = random32();

    nVal1 = nVal1 << 32;

    return nVal1 + nVal2;
}

quint64 XBinary::random(quint64 nLimit)
{
    quint64 nResult = 0;

    if (nLimit) {
        nResult = (random64()) % nLimit;
    }

    return nResult;
}

QString XBinary::randomString(qint32 nSize)
{
    QString sResult;
    sResult.reserve(nSize);

    for (int i = 0; i < nSize; i++) {
        // sResult += QChar('A' + (char)random(26 * 2));
        sResult += QChar('A' + (char)random(26));
    }

    return sResult;
}

QString XBinary::fileTypeIdToString(XBinary::FT fileType)
{
    return XCONVERT_idToTransString(fileType, _TABLE_XBinary_FT, sizeof(_TABLE_XBinary_FT) / sizeof(XBinary::XCONVERT));
}

QString XBinary::xfTypeIdToString(XBinary::XFTYPE xfType)
{
    return XCONVERT_idToTransString(xfType, _TABLE_XBinary_XFTYPE, sizeof(_TABLE_XBinary_XFTYPE) / sizeof(XBinary::XCONVERT));
}

QString XBinary::xfTypeIdToFtString(XBinary::XFTYPE xfType)
{
    return XCONVERT_idToFtString(xfType, _TABLE_XBinary_XFTYPE, sizeof(_TABLE_XBinary_XFTYPE) / sizeof(XBinary::XCONVERT));
}

// QString XBinary::fileTypeIdToExts(FT fileType)
// {
//     QString sResult = tr("Unknown");

//     switch (fileType) {
//         case FT_ZIP: sResult = QString("ZIP(zip, jar, apk, ipa, docx)"); break;
//         case FT_RAR: sResult = QString("RAR"); break;
//         case FT_PDF: sResult = QString("PDF"); break;
//         case FT_7Z: sResult = QString("7-Zip"); break;
//         case FT_PNG: sResult = QString("PNG"); break;
//         case FT_JPEG: sResult = QString("JPEG"); break;
//         case FT_CAB: sResult = QString("CAB"); break;
//         case FT_ICO: sResult = QString("ICO"); break;
//         case FT_CUR: sResult = QString("CUR"); break;
//         case FT_TIFF: sResult = QString("TIFF"); break;
//         case FT_DEX: sResult = QString("DEX"); break;
//         case FT_MACHOFAT: sResult = QString("MACHOFAT"); break;
//         case FT_MACHO: sResult = QString("MACHO"); break;
//         case FT_BMP: sResult = QString("BMP"); break;
//         case FT_GIF: sResult = QString("GIF"); break;
//         case FT_MP3: sResult = QString("MP3"); break;
//         case FT_MP4: sResult = QString("MP4"); break;
//         case FT_XM: sResult = QString("XM"); break;
//         case FT_RIFF: sResult = QString("RIFF(avi, webp)"); break;
//         case FT_ZLIB: sResult = QString("zlib"); break;
//         case FT_GZIP: sResult = QString("GZIP(gz, tgz, tar.gz)"); break;
//         case FT_LE: sResult = QString("LE(le, lx)"); break;
//         case FT_NE: sResult = QString("NE"); break;
//         case FT_AMIGAHUNK: sResult = QString("AmigaHunk"); break;
//         case FT_JAVACLASS: sResult = QString("class"); break;
//         case FT_CFBF: sResult = QString("CFBF"); break;
//         case FT_TTF: sResult = QString("TTF"); break;
//         case FT_SIGNATURE: sResult = tr("Signatures"); break;
//         default: sResult = tr("Unknown");
//     }

//     return sResult;
// }

XBinary::FT XBinary::ftStringToFileTypeId(QString sFileType)
{
    return (XBinary::FT)XCONVERT_ftStringToId(sFileType, _TABLE_XBinary_FT, sizeof(_TABLE_XBinary_FT) / sizeof(XBinary::XCONVERT));
}

QString XBinary::fileTypeIdToFtString(FT fileType)
{
    return XCONVERT_idToFtString(fileType, _TABLE_XBinary_FT, sizeof(_TABLE_XBinary_FT) / sizeof(XBinary::XCONVERT));
}

QString XBinary::fileTypesToString(const QSet<XBinary::FT> &stResult)
{
    QString sResult;

    qint32 nNumberOfRecords = sizeof(_TABLE_XBinary_FT) / sizeof(XBinary::XCONVERT);

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        XBinary::FT fileType = (XBinary::FT)_TABLE_XBinary_FT[i].nID;

        if (stResult.contains(fileType)) {
            if (!sResult.isEmpty()) {
                sResult += QString("|");
            }

            sResult += fileTypeIdToFtString(fileType);
        }
    }

    return sResult;
}

QSet<XBinary::FT> XBinary::stringToFileTypes(const QString &sString)
{
    QSet<XBinary::FT> stResult;

    QString sCurrent;
    qint32 nSize = sString.size();

    for (qint32 i = 0; i < nSize; i++) {
        const QChar c = sString.at(i);

        if ((c == ',') || (c == ';') || (c == '|') || c.isSpace()) {
            QString sToken = sCurrent.trimmed();

            if (!sToken.isEmpty()) {
                XBinary::FT fileType = ftStringToFileTypeId(sToken);

                if (fileType == XBinary::FT_UNKNOWN) {
                    fileType = ftStringToFileTypeId(sToken.toUpper());
                }

                if (fileType == XBinary::FT_UNKNOWN) {
                    fileType = ftStringToFileTypeId(sToken.toLower());
                }

                if (fileType != XBinary::FT_UNKNOWN) {
                    stResult.insert(fileType);
                }
            }

            sCurrent.clear();
        } else {
            sCurrent += c;
        }
    }

    QString sToken = sCurrent.trimmed();

    if (!sToken.isEmpty()) {
        XBinary::FT fileType = ftStringToFileTypeId(sToken);

        if (fileType == XBinary::FT_UNKNOWN) {
            fileType = ftStringToFileTypeId(sToken.toUpper());
        }

        if (fileType == XBinary::FT_UNKNOWN) {
            fileType = ftStringToFileTypeId(sToken.toLower());
        }

        if (fileType != XBinary::FT_UNKNOWN) {
            stResult.insert(fileType);
        }
    }

    return stResult;
}

QString XBinary::convertFileName(const QString &sFileName)  // TODO Check
{
    QString sResult = sFileName;
#ifdef Q_OS_MAC  // Old Qt(4.X)
    //    if(sFileName.startsWith("/.file/id="))
    //    {
    //        CFStringRef relCFStringRef =
    //            CFStringCreateWithCString(
    //                kCFAllocatorDefault,
    //                sFileName.toUtf8().constData(),
    //                kCFStringEncodingUTF8
    //            );
    //    CFURLRef relCFURL =
    //        CFURLCreateWithFileSystemPath(
    //            kCFAllocatorDefault,
    //            relCFStringRef,
    //            kCFURLPOSIXPathStyle,
    //            false // isDirectory
    //        );
    //    CFErrorRef error=0;
    //    CFURLRef absCFURL =
    //    CFURLCreateFilePathURL(
    //            kCFAllocatorDefault,
    //            relCFURL,
    //            &error
    //        );
    //    if(!error)
    //    {
    //        static const CFIndex maxAbsPathCStrBufLen=4096;
    //        char absPathCStr[maxAbsPathCStrBufLen];
    //        if(CFURLGetFileSystemRepresentation(
    //                absCFURL,
    //                true, // resolveAgainstBase
    //                reinterpret_cast<UInt8 *>(&absPathCStr[0] ),
    //                maxAbsPathCStrBufLen
    //            ))
    //            {
    //            sFileName=QString(absPathCStr);
    //            }
    //        }
    //        CFRelease(absCFURL);
    //        CFRelease(relCFURL);
    //        CFRelease(relCFStringRef);
    //    }
#endif

    QFileInfo fiLink(sFileName);

    if (fiLink.isSymLink()) {
        sResult = fiLink.symLinkTarget();
    }

    return sResult;
}

// QString XBinary::convertPathName(const QString &sPathName)
// {
//     QString sResult = sPathName;

//     // TODO more
//     if (sPathName.contains("$app")) {
//         sResult.replace("$app", QCoreApplication::applicationDirPath());
//         sResult.replace("/", QDir::separator());
//     }

//     if (sPathName.contains("$data")) {
// #ifdef Q_OS_MAC
//         sResult.replace("$data", QCoreApplication::applicationDirPath() + "/../Resources");
// #else
//         sResult.replace("$data", QCoreApplication::applicationDirPath());
// #endif
//         sResult.replace("/", QDir::separator());
//     }

//     return sResult;
// }

XBinary::OS_STRING XBinary::getOsAnsiString(qint64 nOffset, qint64 nSize)
{
    OS_STRING result = {};

    result.nOffset = nOffset;
    result.nSize = nSize;
    result.sString = read_ansiString(nOffset, nSize);

    return result;
}

void XBinary::findFiles(const QString &sDirectoryName, XBinary::FFOPTIONS *pFFOption, qint32 nLevel)
{
    *(pFFOption->pnNumberOfFiles) = pFFOption->pListFileNames->count();

    if (!(*pFFOption->pbIsStop)) {
        QFileInfo fi(sDirectoryName);

        if (fi.isFile()) {
            pFFOption->pListFileNames->append(fi.absoluteFilePath());
        } else if (fi.isDir() && ((pFFOption->bSubdirectories) || (nLevel == 0))) {
            QDir dir(sDirectoryName);

            QFileInfoList eil = dir.entryInfoList();

            qint32 nNumberOfFiles = eil.count();

            for (qint32 i = 0; (i < nNumberOfFiles) && (!(*(pFFOption->pbIsStop))); i++) {
                QString sFN = eil.at(i).fileName();

                if ((sFN != ".") && (sFN != "..")) {
                    findFiles(eil.at(i).absoluteFilePath(), pFFOption, nLevel + 1);
                }
            }
        }
    }
}

void XBinary::findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames)
{
    QFileInfo fi(sDirectoryName);

    if (fi.isFile()) {
        pListFileNames->append(fi.absoluteFilePath());
    } else if (fi.isDir()) {
        QDir dir(sDirectoryName);

        QFileInfoList eil = dir.entryInfoList();

        qint32 nNumberOfFiles = eil.count();

        for (qint32 i = 0; i < nNumberOfFiles; i++) {
            QString sFN = eil.at(i).fileName();

            if ((sFN != ".") && (sFN != "..")) {
                findFiles(eil.at(i).absoluteFilePath(), pListFileNames);
            }
        }
    }
}

void XBinary::findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames, bool bSubDirectories, qint32 nLevel, PDSTRUCT *pPdStruct)
{
    qint32 _nFreeIndex = 0;

    if (nLevel == 0) {
        _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, 0);
    }

    if (!(pPdStruct->bIsStop)) {
        QFileInfo fi(sDirectoryName);

        if (fi.isFile()) {
            pListFileNames->append(fi.absoluteFilePath());
        } else if (fi.isDir() && ((bSubDirectories) || (nLevel == 0))) {
            QDir dir(sDirectoryName);

            QFileInfoList eil = dir.entryInfoList();

            qint32 nNumberOfFiles = eil.count();

            for (qint32 i = 0; (i < nNumberOfFiles) && (!(pPdStruct->bIsStop)); i++) {
                QString sFN = eil.at(i).fileName();

                if ((sFN != ".") && (sFN != "..")) {
                    findFiles(eil.at(i).absoluteFilePath(), pListFileNames, bSubDirectories, nLevel + 1, pPdStruct);
                }
            }
        }
    }

    if (nLevel == 0) {
        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
    }
}

qint32 XBinary::getNumberOfFiles(const QString &sDirectoryName, bool bSubDirectories, qint32 nLevel, PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    qint32 _nFreeIndex = 0;

    if (nLevel == 0) {
        _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, 0);
    }

    if (isPdStructNotCanceled(pPdStruct)) {
        QFileInfo fi(sDirectoryName);

        if (fi.isFile()) {
            _nFreeIndex++;
        } else if (fi.isDir() && ((bSubDirectories) || (nLevel == 0))) {
            QDir dir(sDirectoryName);

            QFileInfoList eil = dir.entryInfoList();

            qint32 nNumberOfFiles = eil.count();

            for (qint32 i = 0; (i < nNumberOfFiles) && isPdStructNotCanceled(pPdStruct); i++) {
                QString sFN = eil.at(i).fileName();

                if ((sFN != ".") && (sFN != "..")) {
                    nResult += getNumberOfFiles(eil.at(i).absoluteFilePath(), bSubDirectories, nLevel + 1, pPdStruct);
                }
            }
        }
    }

    if (nLevel == 0) {
        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
    }

    return nResult;
}

QString XBinary::regExp(const QString &sRegExp, const QString &sString, qint32 nIndex)
{
    QString sResult;
#if (QT_VERSION_MAJOR < 5)
    QRegExp rxString(sRegExp);
    rxString.indexIn(sString);

    QStringList list = rxString.capturedTexts();

    if (list.count() > nIndex) {
        sResult = list.at(nIndex);
    }
#else
    QRegularExpression rxString(sRegExp);
    QRegularExpressionMatch matchString = rxString.match(sString);

    if (matchString.hasMatch()) {
        sResult = matchString.captured(nIndex);
    }
#endif

    return sResult;
}

bool XBinary::isRegExpPresent(const QString &sRegExp, const QString &sString)
{
    return (regExp(sRegExp, sString, 0) != "");
}

qint32 XBinary::getRegExpCount(const QString &sRegExp, const QString &sString)
{
    quint32 nResult = 0;
#if (QT_VERSION_MAJOR < 5)
    QRegExp rxString(sRegExp);
    rxString.indexIn(sString);
    nResult = rxString.capturedTexts().count();
#else
    QRegularExpression rxString(sRegExp);
    QRegularExpressionMatchIterator i = rxString.globalMatch(sString);
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        if (match.hasMatch()) {
            nResult++;
        }
    }
#endif

    return nResult;
}

QString XBinary::getRegExpSection(const QString &sRegExp, const QString &sString, qint32 nStart, qint32 nEnd)
{
#if (QT_VERSION_MAJOR < 5)
    return sString.section(QRegExp(sRegExp), nStart, nEnd);
#else
    return sString.section(QRegularExpression(sRegExp), nStart, nEnd);
#endif
}

bool XBinary::isRegExpValid(const QString &sRegExp)
{
#if (QT_VERSION_MAJOR < 5)
    QRegExp rxString(sRegExp);
    return rxString.isValid();
#else
    QRegularExpression rxString(sRegExp);
    return rxString.isValid();
#endif
}

qint64 XBinary::read_array_process(qint64 nOffset, char *pBuffer, qint64 nMaxSize, PDSTRUCT *pPdStruct)
{
    qint64 nResult = 0;

    if (!m_pDevice) {
        return 0;
    }

    nResult = safeReadData(m_pDevice, nOffset, pBuffer, nMaxSize, pPdStruct);  // Check for read large files

    return nResult;
}

qint64 XBinary::read_array(qint64 nOffset, char *pBuffer, qint64 nMaxSize)
{
    qint64 nResult = 0;

    if (!m_pDevice) {
        return 0;
    }

    nResult = _readDataSimple(m_pDevice, nOffset, pBuffer, nMaxSize);  // Check for read large files

    return nResult;
}

QByteArray XBinary::read_array(qint64 nOffset, qint64 nSize)
{
    QByteArray baResult;

    XBinary::OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    if (osRegion.nOffset != -1) {
        baResult.resize((qint32)osRegion.nSize);

        qint64 nBytes = read_array(osRegion.nOffset, baResult.data(), osRegion.nSize);

        if (osRegion.nSize != nBytes) {
            baResult.resize((qint32)nBytes);
        }
    }

    return baResult;
}

QByteArray XBinary::read_array_process(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    XBinary::OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    if (osRegion.nOffset != -1) {
        baResult.resize((qint32)osRegion.nSize);

        qint64 nBytes = read_array_process(osRegion.nOffset, baResult.data(), osRegion.nSize, pPdStruct);

        if (osRegion.nSize != nBytes) {
            baResult.resize((qint32)nBytes);
        }
    }

    return baResult;
}

qint64 XBinary::write_array_process(qint64 nOffset, const char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint64 nResult = 0;

    qint64 _nTotalSize = getSize();

    if ((nSize <= (_nTotalSize - nOffset)) && (nOffset >= 0)) {
        nResult = safeWriteData(m_pDevice, nOffset, pBuffer, nSize, pPdStruct);
    }

    return nResult;
}

qint64 XBinary::write_array_process(qint64 nOffset, const QByteArray &baData, PDSTRUCT *pPdStruct)
{
    return write_array_process(nOffset, baData.data(), baData.size(), pPdStruct);
}

qint64 XBinary::write_array(qint64 nOffset, const char *pBuffer, qint64 nSize)
{
    qint64 nResult = 0;

    qint64 _nTotalSize = getSize();

    if ((nSize <= (_nTotalSize - nOffset)) && (nOffset >= 0)) {
        nResult = _writeDataSimple(m_pDevice, nOffset, pBuffer, nSize);
    }

    return nResult;
}

qint64 XBinary::write_array(qint64 nOffset, const QByteArray &baData)
{
    return write_array(nOffset, baData.data(), baData.size());
}

QByteArray XBinary::read_array_process(QIODevice *pDevice, qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    XBinary binary(pDevice);

    return binary.read_array_process(nOffset, nSize, pPdStruct);
}

qint64 XBinary::read_array_process(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct)
{
    XBinary binary(pDevice);

    return binary.read_array_process(nOffset, pBuffer, nSize, pPdStruct);
}

qint64 XBinary::write_array_process(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct)
{
    XBinary binary(pDevice);

    return binary.write_array_process(nOffset, pBuffer, nSize, pPdStruct);
}

qint64 XBinary::write_array_process(QIODevice *pDevice, qint64 nOffset, const QByteArray &baData, PDSTRUCT *pPdStruct)
{
    XBinary binary(pDevice);

    return binary.write_array_process(nOffset, baData, pPdStruct);
}

QByteArray XBinary::read_array(QIODevice *pDevice, qint64 nOffset, qint64 nSize)
{
    XBinary binary(pDevice);

    return binary.read_array(nOffset, nSize);
}

qint64 XBinary::write_array(QIODevice *pDevice, qint64 nOffset, const char *pBuffer, qint64 nSize)
{
    XBinary binary(pDevice);

    return binary.write_array(nOffset, pBuffer, nSize);
}

qint64 XBinary::write_array(QIODevice *pDevice, qint64 nOffset, const QByteArray &baData)
{
    XBinary binary(pDevice);

    return binary.write_array(nOffset, baData);
}

quint8 XBinary::read_uint8(qint64 nOffset)
{
    quint8 result = 0;

    read_array(nOffset, (char *)(&result), 1);

    return result;
}

qint8 XBinary::read_int8(qint64 nOffset)
{
    quint8 result = 0;

    read_array(nOffset, (char *)(&result), 1);

    return (qint8)result;
}

quint16 XBinary::read_uint16(qint64 nOffset, bool bIsBigEndian)
{
    quint16 result = 0;

    read_array(nOffset, (char *)(&result), 2);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint16 XBinary::read_int16(qint64 nOffset, bool bIsBigEndian)
{
    quint16 result = 0;

    read_array(nOffset, (char *)(&result), 2);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return (qint16)result;
}

quint32 XBinary::read_uint32(qint64 nOffset, bool bIsBigEndian)
{
    quint32 result = 0;

    read_array(nOffset, (char *)(&result), 4);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint32 XBinary::read_int32(qint64 nOffset, bool bIsBigEndian)
{
    quint32 result = 0;

    read_array(nOffset, (char *)(&result), 4);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return (qint32)result;
}

quint64 XBinary::read_uint64(qint64 nOffset, bool bIsBigEndian)
{
    quint64 result = 0;

    read_array(nOffset, (char *)(&result), 8);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint64 XBinary::read_int64(qint64 nOffset, bool bIsBigEndian)
{
    qint64 result = 0;

    read_array(nOffset, (char *)(&result), 8);

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return (qint64)result;
}

float XBinary::read_float16(qint64 nOffset, bool bIsBigEndian)
{
    // TODO Check
    quint16 float16_value = read_uint16(nOffset, bIsBigEndian);

    // MSB -> LSB
    // float16=1bit: sign, 5bit: exponent, 10bit: fraction
    // float32=1bit: sign, 8bit: exponent, 23bit: fraction
    // for normal exponent(1 to 0x1e): value=2**(exponent-15)*(1.fraction)
    // for denormalized exponent(0): value=2**-14*(0.fraction)
    quint32 sign = float16_value >> 15;
    quint32 exponent = (float16_value >> 10) & 0x1F;
    quint32 fraction = (float16_value & 0x3FF);
    quint32 float32_value = 0;
    if (exponent == 0) {
        if (fraction == 0) {
            // zero
            float32_value = (sign << 31);
        } else {
            // can be represented as ordinary value in float32
            // 2 ** -14 * 0.0101
            // => 2 ** -16 * 1.0100
            // qint32 int_exponent = -14;
            exponent = 127 - 14;
            while ((fraction & (1 << 10)) == 0) {
                // int_exponent--;
                exponent--;
                fraction <<= 1;
            }
            fraction &= 0x3FF;
            // int_exponent += 127;
            float32_value = (sign << 31) | (exponent << 23) | (fraction << 13);
        }
    } else if (exponent == 0x1F) {
        /* Inf or NaN */
        float32_value = (sign << 31) | (0xFF << 23) | (fraction << 13);
    } else {
        /* ordinary number */
        float32_value = (sign << 31) | ((exponent + (127 - 15)) << 23) | (fraction << 13);
    }

    return *((float *)&float32_value);
}

float XBinary::read_float(qint64 nOffset, bool bIsBigEndian)
{
    float result = 0;

    read_array(nOffset, (char *)(&result), 4);

    endian_float(&result, bIsBigEndian);

    return result;
}

double XBinary::read_double(qint64 nOffset, bool bIsBigEndian)
{
    double result = 0;

    read_array(nOffset, (char *)(&result), 8);

    endian_double(&result, bIsBigEndian);

    return result;
}

quint32 XBinary::read_uint24(qint64 nOffset, bool bIsBigEndian)
{
    quint32 result = 0;

    if (bIsBigEndian) {
        read_array(nOffset, (char *)(&result) + 1, 3);
        result = qFromBigEndian(result);
    } else {
        read_array(nOffset, (char *)(&result) + 0, 3);
        result = qFromLittleEndian(result);
    }

    return (result & (0xFFFFFF));
}

qint32 XBinary::read_int24(qint64 nOffset, bool bIsBigEndian)
{
    return (qint32)(read_uint24(nOffset, bIsBigEndian));
}

XBinary::PACKED_UINT XBinary::read_packedNumber(qint64 nOffset, qint64 nSize)
{
    nSize = qMin((qint64)9, nSize);

    QByteArray baData = read_array(nOffset, nSize);

    return XBinary::_read_packedNumber(baData.data(), nSize);
}

qint64 XBinary::write_ansiString(qint64 nOffset, const QString &sString, qint64 nMaxSize)
{
    qint64 _nMaxSize = sString.length() + 1;
    if ((nMaxSize == -1) || (nMaxSize > _nMaxSize)) {
        nMaxSize = _nMaxSize;
    }

    return write_array(nOffset, sString.toLatin1().data(), nMaxSize);
}

void XBinary::write_ansiStringFix(qint64 nOffset, qint64 nSize, const QString &sString)
{
    QString _sString = sString;
    char *pBuffer = new char[nSize + 1];  // mb TODO Check

    _zeroMemory(pBuffer, nSize + 1);

    if (_sString.size() > nSize) {
        _sString.resize(nSize);
    }

    _copyMemory(pBuffer, _sString.toLatin1().data(), _sString.size());

    XBinary::write_array(nOffset, pBuffer, nSize);

    delete[] pBuffer;
}

qint64 XBinary::write_unicodeString(qint64 nOffset, const QString &sString, qint64 nMaxSize, bool bIsBigEndian)
{
    qint64 nResult = 0;

    if ((nMaxSize > 0) && (nMaxSize < 0x10000)) {
        qint64 nSize = qMin((qint64)sString.size(), nMaxSize);

        quint16 *pUtf16 = (quint16 *)sString.utf16();

        for (qint32 i = 0; i < nSize; i++) {
            write_uint16(nOffset + 2 * i, *(pUtf16 + i), bIsBigEndian);
            nResult++;
        }
    }

    return nResult;
}

QString XBinary::read_ansiString(qint64 nOffset, qint64 nMaxSize)
{
    QString sResult;

    if (nMaxSize > 0x10000) {
        nMaxSize = 0x10000;
    }

    if (nMaxSize > 0) {
        QByteArray baBuffer = read_array(nOffset, nMaxSize);
        qint32 nActualSize = (qint32)baBuffer.size();
        qint32 nLen = 0;

        while ((nLen < nActualSize) && (baBuffer.at(nLen) != 0)) {
            nLen++;
        }

        sResult = QString::fromLatin1(baBuffer.constData(), nLen);
    }

    return sResult;
}

QString XBinary::read_unicodeString(qint64 nOffset, qint64 nMaxSize, bool bIsBigEndian)
{
    QString sResult;

    if ((nMaxSize > 0) && (nMaxSize < 0x10000)) {
        QByteArray baBuffer = read_array(nOffset, nMaxSize * 2);
        qint32 nActualWords = (qint32)(baBuffer.size() / 2);
        quint16 *pBuffer = new quint16[nActualWords + 1];
        const char *pData = baBuffer.constData();
        qint32 nLen = 0;

        for (qint32 i = 0; i < nActualWords; i++) {
            quint16 nWord;
            memcpy(&nWord, pData + i * 2, 2);
            if (bIsBigEndian) {
                nWord = (quint16)(((nWord & 0xFF) << 8) | ((nWord >> 8) & 0xFF));
            }
            if (nWord == 0) {
                break;
            }
            pBuffer[nLen++] = nWord;
        }
        pBuffer[nLen] = 0;

        sResult = QString::fromUtf16(pBuffer, nLen);

        delete[] pBuffer;
    }

    return sResult;
}

QString XBinary::read_ucsdString(qint64 nOffset)
{
    QString sResult;

    qint32 nSize = read_uint8(nOffset);

    if (nSize > 0x10000) {
        nSize = 0x10000;
    }

    if (nSize > 0) {
        nOffset++;
        quint8 *pBuffer = new quint8[nSize + 1];

        for (qint32 i = 0; i < nSize; i++) {
            pBuffer[i] = read_uint8(nOffset + i);

            if (pBuffer[i] == 0) {
                pBuffer[i] = 0x20;  // Space
            }

            if (i == nSize - 1) {
                pBuffer[nSize] = 0;
            }
        }

        sResult.append((char *)pBuffer);

        delete[] pBuffer;
    }

    return sResult;
}

QString XBinary::read_utf8String(qint64 nOffset, qint64 nMaxSize)
{
    QString sResult;

    if (nMaxSize) {
        qint32 nRealSize = 0;

        for (qint32 i = 0; i < nMaxSize; i++) {
            quint8 nByte = read_uint8(nOffset + nRealSize);

            if (nByte == 0) {
                break;
            }

            // TODO Check !!!
            if ((nByte >> 7) & 0x1) {
                nRealSize++;
            } else if ((nByte >> 5) & 0x1) {
                nRealSize += 2;
            } else if ((nByte >> 4) & 0x1) {
                nRealSize += 3;
            } else if ((nByte >> 3) & 0x1) {
                nRealSize += 4;
            }
        }

        if (nRealSize) {
            QByteArray baString = read_array(nOffset, nRealSize);
            sResult = QString::fromUtf8(baString.data());
        }
    }

    return sResult;
}

QString XBinary::_read_utf8String(qint64 nOffset, qint64 nMaxSize)
{
    QString sResult;

    PACKED_UINT ulebSize = read_uleb128(nOffset, nMaxSize);

    sResult = read_utf8String(nOffset + ulebSize.nByteSize,
                              ulebSize.nValue);  // TODO mutf8

    return sResult;
}

QString XBinary::_read_utf8String(char *pData, qint64 nMaxSize)
{
    QString sResult;

    PACKED_UINT ulebSize = _read_uleb128(pData, nMaxSize);

    qint32 nStringSize = qMin((qint64)ulebSize.nValue, nMaxSize - ulebSize.nByteSize);

    if (nStringSize > 0) {
        sResult = QString::fromUtf8(pData + ulebSize.nByteSize, nStringSize);
    }

    return sResult;
}

QString XBinary::_read_utf8String(qint64 nOffset, char *pData, qint32 nDataSize, qint32 nDataOffset)
{
    QString sResult;

    if ((nOffset >= nDataOffset) && (nOffset < (nDataOffset + nDataSize))) {
        char *pStringData = pData + (nOffset - nDataOffset);
        qint32 nStringSize = nDataSize - (nOffset - nDataOffset);
        sResult = XBinary::_read_utf8String(pStringData, nStringSize);
    }

    return sResult;
}

QString XBinary::read_codePageString(qint64 nOffset, qint64 nMaxByteSize, const QString &sCodePage)
{
    QString sResult;

#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
    QByteArray baData = read_array(nOffset, nMaxByteSize);

    QTextCodec *pCodec = QTextCodec::codecForName(sCodePage.toLatin1().data());

    if (pCodec) {
        sResult = pCodec->toUnicode(baData);
    }
#endif

    sResult = sResult.section(QChar(0xFFFD), 0, 0);
    sResult = sResult.section(QChar(0), 0, 0);

    return sResult;
}

bool XBinary::isUnicodeStringLatin(qint64 nOffset, qint64 nMaxSize, bool bIsBigEndian)
{
    bool bResult = true;

    if ((nMaxSize > 0) && (nMaxSize < 0x10000)) {
        for (qint32 i = 0; i < nMaxSize; i++) {
            quint16 nWord = read_uint16(nOffset + 2 * i, bIsBigEndian);

            if (nWord == 0) {
                break;
            } else if (nWord > 0x7F) {
                bResult = false;
                break;
            }
        }
    }

    return bResult;
}

void XBinary::write_uint8(qint64 nOffset, quint8 nValue)
{
    write_array(nOffset, (char *)(&nValue), 1);
}

void XBinary::write_int8(qint64 nOffset, qint8 nValue)
{
    quint8 _value = (quint8)nValue;
    write_array(nOffset, (char *)(&_value), 1);
}

void XBinary::write_uint16(qint64 nOffset, quint16 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    write_array(nOffset, (char *)(&nValue), 2);
}

void XBinary::write_int16(qint64 nOffset, qint16 nValue, bool bIsBigEndian)
{
    quint16 _value = (quint16)nValue;

    if (bIsBigEndian) {
        _value = qFromBigEndian(_value);
    } else {
        _value = qFromLittleEndian(_value);
    }

    write_array(nOffset, (char *)(&_value), 2);
}

void XBinary::write_uint32(qint64 nOffset, quint32 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    write_array(nOffset, (char *)(&nValue), 4);
}

void XBinary::write_int32(qint64 nOffset, qint32 nValue, bool bIsBigEndian)
{
    quint32 _value = (quint32)nValue;

    if (bIsBigEndian) {
        _value = qFromBigEndian(_value);
    } else {
        _value = qFromLittleEndian(_value);
    }

    write_array(nOffset, (char *)(&_value), 4);
}

void XBinary::write_uint64(qint64 nOffset, quint64 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    write_array(nOffset, (char *)(&nValue), 8);
}

void XBinary::write_int64(qint64 nOffset, qint64 nValue, bool bIsBigEndian)
{
    quint64 _value = (quint64)nValue;

    if (bIsBigEndian) {
        _value = qFromBigEndian(_value);
    } else {
        _value = qFromLittleEndian(_value);
    }

    write_array(nOffset, (char *)(&_value), 8);
}

void XBinary::write_float16(qint64 nOffset, float fValue, bool bIsBigEndian)
{
    // TODO Check
    quint32 fltInt32 = *(quint32 *)(&fValue);
    quint16 fltInt16 = 0;

    fltInt16 = (fltInt32 >> 31) << 5;
    quint16 tmp = (fltInt32 >> 23) & 0xff;
    tmp = (tmp - 0x70) & ((quint32)((qint32)(0x70 - tmp) >> 4) >> 27);
    fltInt16 = (fltInt16 | tmp) << 10;
    fltInt16 |= (fltInt32 >> 13) & 0x3ff;

    if (bIsBigEndian) {
        fltInt16 = qFromBigEndian(fltInt16);
    } else {
        fltInt16 = qFromLittleEndian(fltInt16);
    }

    write_array(nOffset, (char *)(&fltInt16), 2);
}

void XBinary::write_float(qint64 nOffset, float fValue, bool bIsBigEndian)
{
    endian_float(&fValue, bIsBigEndian);

    write_array(nOffset, (char *)(&fValue), 4);
}

void XBinary::write_double(qint64 nOffset, double dValue, bool bIsBigEndian)
{
    endian_double(&dValue, bIsBigEndian);

    write_array(nOffset, (char *)(&dValue), 8);
}

QString XBinary::read_UUID_bytes(qint64 nOffset)
{
    // TODO check!
    // TODO Check Endian
    QString sResult = QString("%1-%2-%3-%4-%5")
                          .arg(read_array(nOffset + 0, 4).toHex().data(), read_array(nOffset + 4, 2).toHex().data(), read_array(nOffset + 6, 2).toHex().data(),
                               read_array(nOffset + 8, 2).toHex().data(), read_array(nOffset + 10, 6).toHex().data());

    return sResult;
}

void XBinary::write_UUID_bytes(qint64 nOffset, const QString &sValue)
{
    QString _sValue = sValue;
    // TODO Check
    _sValue = _sValue.remove("-");

    QByteArray baUUID = QByteArray::fromHex(_sValue.toLatin1().data());

    write_array(nOffset, baUUID.data(), 16);
}

QString XBinary::read_UUID(qint64 nOffset, bool bIsBigEndian)
{
    QString sResult = QString("%1-%2-%3-%4-%5")
                          .arg(valueToHex(read_uint32(nOffset + 0, bIsBigEndian), bIsBigEndian), valueToHex(read_uint16(nOffset + 4, bIsBigEndian), bIsBigEndian),
                               valueToHex(read_uint16(nOffset + 6, bIsBigEndian), bIsBigEndian), valueToHex(read_uint16(nOffset + 8, bIsBigEndian), bIsBigEndian),
                               read_array(nOffset + 10, 6).toHex().data());

    return sResult;
}

quint8 XBinary::_read_uint8(char *pData)
{
    return *(quint8 *)pData;
}

qint8 XBinary::_read_int8(char *pData)
{
    return *(qint8 *)pData;
}

quint16 XBinary::_read_uint16(char *pData, bool bIsBigEndian)
{
    quint16 result = *(quint16 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint16 XBinary::_read_int16(char *pData, bool bIsBigEndian)
{
    qint16 result = *(qint16 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

quint32 XBinary::_read_uint24(char *pData, bool bIsBigEndian)
{
    quint32 result = 0;

    if (bIsBigEndian) {
        _copyMemory(((char *)(&result)) + 1, pData, 3);
        result = qFromBigEndian(result);
    } else {
        _copyMemory(((char *)(&result)) + 0, pData, 3);
        result = qFromLittleEndian(result);
    }

    return (result & (0xFFFFFF));
}

quint32 XBinary::_read_uint32(char *pData, bool bIsBigEndian)
{
    quint32 result = *(quint32 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint32 XBinary::_read_int32(char *pData, bool bIsBigEndian)
{
    qint32 result = *(qint32 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

quint64 XBinary::_read_uint64(char *pData, bool bIsBigEndian)
{
    quint64 result = *(quint64 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

qint64 XBinary::_read_int64(char *pData, bool bIsBigEndian)
{
    qint64 result = *(qint64 *)pData;

    if (bIsBigEndian) {
        result = qFromBigEndian(result);
    } else {
        result = qFromLittleEndian(result);
    }

    return result;
}

QString XBinary::_read_ansiString(char *pData, qint32 nMaxSize)
{
    QString sResult;

    QByteArray baData(pData, nMaxSize);
    sResult.append(baData.data());

    return sResult;
}

QByteArray XBinary::_read_byteArray(char *pData, qint32 nSize)
{
    return QByteArray(pData, nSize);
}

float XBinary::_read_float(char *pData, bool bIsBigEndian)
{
    float result = *(float *)pData;

    endian_float(&result, bIsBigEndian);

    return result;
}

double XBinary::_read_double(char *pData, bool bIsBigEndian)
{
    double result = *(double *)pData;

    endian_double(&result, bIsBigEndian);

    return result;
}

quint64 XBinary::_read_value(MODE mode, char *pData, bool bIsBigEndian)
{
    quint64 nResult = 0;

    if (mode == MODE::MODE_8) {
        nResult = _read_uint8(pData);
    } else if (mode == MODE::MODE_16) {
        nResult = _read_uint16(pData, bIsBigEndian);
    } else if (mode == MODE::MODE_32) {
        nResult = _read_uint32(pData, bIsBigEndian);
    } else if (mode == MODE::MODE_64) {
        nResult = _read_uint64(pData, bIsBigEndian);
    }

    return nResult;
}

quint8 XBinary::_read_uint8_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset)
{
    quint8 result = 0;

    if ((nOffset >= 0) && (nOffset < nBufferSize)) {
        result = _read_uint8(pBuffer + nOffset);
    }

    return result;
}

quint16 XBinary::_read_uint16_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian)
{
    quint16 result = 0;

    if ((nOffset >= 0) && (nOffset + 1 < nBufferSize)) {
        result = _read_uint16(pBuffer + nOffset, bIsBigEndian);
    }

    return result;
}

quint32 XBinary::_read_uint32_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian)
{
    quint32 result = 0;

    if ((nOffset >= 0) && (nOffset + 3 < nBufferSize)) {
        result = _read_uint32(pBuffer + nOffset, bIsBigEndian);
    }

    return result;
}

quint64 XBinary::_read_uint64_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian)
{
    quint64 result = 0;

    if ((nOffset >= 0) && (nOffset + 7 < nBufferSize)) {
        result = _read_uint64(pBuffer + nOffset, bIsBigEndian);
    }

    return result;
}

QString XBinary::_read_ansiString_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, qint32 nMaxSize)
{
    QString sResult;

    if (nOffset + nMaxSize > nBufferSize) {
        nMaxSize = nBufferSize - nOffset;
    }

    if ((nOffset >= 0) && (nOffset < nBufferSize)) {
        sResult = _read_ansiString(pBuffer + nOffset, nMaxSize);
    }

    return sResult;
}

bool XBinary::_read_bool_safe(char *pBuffer, qint32 nBufferSize, qint32 nBitPosition)
{
    bool bResult = false;

    if (nBitPosition < nBufferSize * 8) {
        qint32 _nIndex = nBitPosition / 8;
        qint32 _nPosition = nBitPosition % 8;
        bResult = (quint8)(pBuffer[_nIndex]) & (0x1 << _nPosition);
    }

    return bResult;
}

bool XBinary::_read_bool_safe_rev(char *pBuffer, qint32 nBufferSize, qint32 nBitPosition)
{
    bool bResult = false;

    if (nBitPosition < nBufferSize * 8) {
        qint32 _nIndex = nBitPosition / 8;
        qint32 _nPosition = nBitPosition % 8;
        bResult = (quint8)(pBuffer[_nIndex]) & (0x80 >> _nPosition);
    }

    return bResult;
}

quint32 XBinary::_getBitCount_safe(char *pBuffer, qint32 nBufferSize)
{
    quint32 nResult = 0;

    if (pBuffer && (nBufferSize > 0)) {
        for (qint32 i = 0; i < nBufferSize; i++) {
            unsigned char byte = static_cast<unsigned char>(pBuffer[i]);

            // Count set bits in the byte
            while (byte) {
                nResult += byte & 1;
                byte >>= 1;
            }
        }
    }

    return nResult;
}

void XBinary::_write_uint8(char *pData, quint8 nValue)
{
    *(quint8 *)pData = nValue;
}

void XBinary::_write_int8(char *pData, qint8 nValue)
{
    *(qint8 *)pData = nValue;
}

void XBinary::_write_uint16(char *pData, quint16 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(quint16 *)pData = nValue;
}

void XBinary::_write_int16(char *pData, qint16 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(qint16 *)pData = nValue;
}

void XBinary::_write_uint32(char *pData, quint32 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(quint32 *)pData = nValue;
}

void XBinary::_write_int32(char *pData, qint32 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(qint32 *)pData = nValue;
}

void XBinary::_write_uint64(char *pData, quint64 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(quint64 *)pData = nValue;
}

void XBinary::_write_int64(char *pData, qint64 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qToBigEndian(nValue);
    } else {
        nValue = qToLittleEndian(nValue);
    }

    *(qint64 *)pData = nValue;
}

void XBinary::_write_float(char *pData, float fValue, bool bIsBigEndian)
{
    endian_float(&fValue, bIsBigEndian);

    *(float *)pData = fValue;
}

void XBinary::_write_double(char *pData, double dValue, bool bIsBigEndian)
{
    endian_double(&dValue, bIsBigEndian);

    *(double *)pData = dValue;
}

void XBinary::_write_value(MODE mode, char *pData, quint64 nValue, bool bIsBigEndian)
{
    if (mode == MODE::MODE_8) {
        _write_uint8(pData, nValue);
    } else if (mode == MODE::MODE_16) {
        _write_uint16(pData, nValue, bIsBigEndian);
    } else if (mode == MODE::MODE_32) {
        _write_uint32(pData, nValue, bIsBigEndian);
    } else if (mode == MODE::MODE_64) {
        _write_uint64(pData, nValue, bIsBigEndian);
    }
}

quint8 XBinary::read_bcd_uint8(qint64 nOffset)
{
    quint8 nResult = 0;

    nResult = _bcd_decimal(read_uint8(nOffset));

    return nResult;
}

quint16 XBinary::read_bcd_uint16(qint64 nOffset, bool bIsBigEndian)
{
    quint16 nResult = 0;

    quint16 nValue = read_uint16(nOffset, bIsBigEndian);

    nResult = _bcd_decimal(0xFF & nValue) + _bcd_decimal(0xFF & (nValue >> 8)) * 10;

    return nResult;
}

quint16 XBinary::read_bcd_uint32(qint64 nOffset, bool bIsBigEndian)
{
    quint32 nResult = 0;

    quint32 nValue = read_uint32(nOffset, bIsBigEndian);

    nResult =
        _bcd_decimal(0xFF & nValue) + _bcd_decimal(0xFF & (nValue >> 8)) * 10 + _bcd_decimal(0xFF & (nValue >> 16)) * 100 + _bcd_decimal(0xFF & (nValue >> 24)) * 1000;

    return nResult;
}

quint16 XBinary::read_bcd_uint64(qint64 nOffset, bool bIsBigEndian)
{
    quint64 nResult = 0;

    quint64 nValue = read_uint64(nOffset, bIsBigEndian);

    nResult = _bcd_decimal(0xFF & nValue) + _bcd_decimal(0xFF & (nValue >> 8)) * 10 + _bcd_decimal(0xFF & (nValue >> 16)) * 100 +
              _bcd_decimal(0xFF & (nValue >> 24)) * 1000 + _bcd_decimal(0xFF & (nValue >> 32)) * 10000 + _bcd_decimal(0xFF & (nValue >> 40)) * 100000 +
              _bcd_decimal(0xFF & (nValue >> 48)) * 1000000 + _bcd_decimal(0xFF & (nValue >> 56)) * 10000000;

    return nResult;
}

quint8 XBinary::_bcd_decimal(quint8 nValue)
{
    quint8 nResult = 0;

    nResult = ((nValue & 0xF0) >> 4) * 10 + (nValue & 0x0F);

    return nResult;
}

static qint64 _x_findbyte(char *pBuffer, qint64 nTemp, const char *pArray, qint64 nOffset)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_byte(pBuffer, nTemp, (unsigned char)pArray[0], nOffset);
    if (nResult != -1) return nResult;
#else
    // Fallback to standard memchr
    const void *p = memchr(pBuffer, (unsigned char)pArray[0], (size_t)nTemp);
    if (p) {
        nResult = nOffset + ((const char *)p - pBuffer);
    }
#endif

    return nResult;
}

static qint64 _x_findpattern_bmh(char *pBuffer, qint64 nTemp, const char *pArray, qint64 nArraySize, qint64 nOffset, const qint32 *bmhShift, char nLastSearchChar)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_pattern_bmh(pBuffer, nTemp, pArray, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    // Fallback: scalar BMH search
    const char *hay = pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    qint64 i = 0;
    const qint64 limit = hayLen - m;

    while (i <= limit) {
        unsigned char c = (unsigned char)hay[i + m - 1];

        if (c == (unsigned char)nLastSearchChar) {
            if (memcmp(hay + i, pArray, (size_t)m) == 0) {
                nResult = nOffset + i;
                break;
            }
        }
        i += (qint64)bmhShift[c];
    }
#endif

    return nResult;
}

static qint64 _x_find_notnull(char *pBuffer, qint64 nTemp, qint64 nArraySize, qint64 nOffset)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_notnull(pBuffer, nTemp, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    // Find first window of length nArraySize with no zero bytes using memchr to skip over zero-containing regions.
    const char *hay = pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    const qint64 limit = hayLen - (m - 1);
    qint64 j = 0;
    while (j < limit) {
        const void *pz = memchr(hay + j, 0, (size_t)(hayLen - j));
        qint64 runLen = (pz ? ((const char *)pz - (hay + j)) : (hayLen - j));
        if (runLen >= m) {
            nResult = nOffset + j;
            break;
        }
        // Skip to just after the zero byte
        j += runLen + 1;
    }
#endif

    return nResult;
}

static qint64 _x_find_ansi(char *pBuffer, qint64 nTemp, qint64 nArraySize, qint64 nOffset, const bool *ansiTable)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_ansi(pBuffer, nTemp, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    // Fallback: scalar loop
    const unsigned char *hay = (const unsigned char *)pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    const qint64 limit = hayLen - (m - 1);
    qint64 j = 0;

    while (j < limit) {
        // Skip non-ANSI bytes to the start of an ANSI run
        while (j < hayLen && !ansiTable[hay[j]]) j++;
        if (j >= limit) break;
        qint64 start = j;
        // Extend ANSI run
        while (j < hayLen && ansiTable[hay[j]]) j++;
        qint64 runLen = j - start;
        if (runLen >= m) {
            nResult = nOffset + start;
            break;
        }
    }
#endif

    return nResult;
}

static qint64 _x_find_notansi(char *pBuffer, qint64 nTemp, qint64 nArraySize, qint64 nOffset, const bool *ansiTable)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_not_ansi(pBuffer, nTemp, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    // Fallback: scalar loop
    const unsigned char *hay = (const unsigned char *)pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    const qint64 limit = hayLen - (m - 1);
    qint64 j = 0;

    while (j < limit) {
        // Skip ANSI bytes to the start of a non-ANSI run
        while (j < hayLen && ansiTable[hay[j]]) j++;
        if (j >= limit) break;
        qint64 start = j;
        // Extend non-ANSI run
        while (j < hayLen && !ansiTable[hay[j]]) j++;
        qint64 runLen = j - start;
        if (runLen >= m) {
            nResult = nOffset + start;
            break;
        }
    }
#endif

    return nResult;
}

static qint64 _x_find_notansiandnull(char *pBuffer, qint64 nTemp, qint64 nArraySize, qint64 nOffset, const bool *ansiTable)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_not_ansi_and_null(pBuffer, nTemp, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    const unsigned char *hay = (const unsigned char *)pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    const qint64 limit = hayLen - (m - 1);
    qint64 j = 0;

    while (j < limit) {
        // Skip bytes that are ANSI or zero to the start of a desired run
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((!ansiTable[c]) && (c != 0)) break;
            j++;
        }
        if (j >= limit) break;
        qint64 start = j;
        // Extend run of non-ANSI and non-zero bytes
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((ansiTable[c]) || (c == 0)) break;
            j++;
        }
        qint64 runLen = j - start;
        if (runLen >= m) {
            nResult = nOffset + start;
            break;
        }
    }
#endif

    return nResult;
}

static qint64 _x_find_ansinumber(char *pBuffer, qint64 nTemp, qint64 nArraySize, qint64 nOffset)
{
    qint64 nResult = -1;

#ifdef USE_XSIMD
    nResult = xsimd_find_ansi_number(pBuffer, nTemp, nArraySize, nOffset);
    if (nResult != -1) return nResult;
#else
    const unsigned char *hay = (const unsigned char *)pBuffer;
    qint64 hayLen = nTemp;
    qint64 m = nArraySize;
    const qint64 limit = hayLen - (m - 1);
    qint64 j = 0;
    while (j < limit) {
        // Skip bytes that are not digits to the start of a desired run
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((c >= 0x30) && (c <= 0x39)) break;
            j++;
        }
        if (j >= limit) break;
        qint64 start = j;
        // Extend run of digit bytes
        while (j < hayLen) {
            unsigned char c = hay[j];
            if ((c < 0x30) || (c > 0x39)) break;
            j++;
        }
        qint64 runLen = j - start;
        if (runLen >= m) {
            nResult = nOffset + start;
            break;
        }
    }
#endif

    return nResult;
}

qint64 XBinary::_find_array(ST st, qint64 nOffset, qint64 nSize, const char *pArray, qint64 nArraySize, PDSTRUCT *pPdStruct)
{
    qint64 nResult = -1;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }
    // Validate input
    // For ST_COMPAREBYTES, pArray must be provided
    // For other ST modes (NOTNULL, ANSI, etc.), pArray can be nullptr
    if ((st == ST_COMPAREBYTES) && (!pArray)) {
        return -1;
    }

    if (nArraySize <= 0) {
        return -1;
    }

    // TODO CheckSize function
    qint64 _nSize = getSize();

    if (nSize == -1) {
        nSize = _nSize - nOffset;
    }

    if (nSize <= 0) {
        return -1;
    }

    if (nOffset + nSize > _nSize) {
        return -1;
    }

    if (nArraySize > nSize) {
        return -1;
    }

    if (nOffset < 0) {
        return -1;
    }

    qint64 nTemp = 0;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint64 nStartOffset = nOffset;

    char *pBuffer = nullptr;

    qint32 nBufferSize = getBufferSize(pPdStruct);

    if (!m_pConstMemory) {
        pBuffer = new char[nBufferSize];
    }

    // Precompute BMH shift table for byte-compare mode
    qint32 bmhShift[256];
    bool bUseBMH = (st == ST_COMPAREBYTES) && (nArraySize >= 2);
    if (bUseBMH) {
        qint32 m = (qint32)qMin<qint64>(nArraySize, (qint64)0x7fffffff);
        // Initialize all entries to m
        for (int i = 0; i < 256; ++i) {
            bmhShift[i] = m;
        }
        // Set shifts for all but last character
        for (int i = 0; i < m - 1; ++i) bmhShift[(quint8)pArray[i]] = m - 1 - i;
    }

    // Precompute ANSI classification table if needed
    bool needAnsiTable = (st == ST_ANSI) || (st == ST_NOTANSI) || (st == ST_NOTANSIANDNULL) || (st == ST_ANSINUMBER);
    bool ansiTable[256];
    if (needAnsiTable) {
        for (int i = 0; i < 256; ++i) {
            ansiTable[i] = isAnsiSymbol((quint8)i);
        }
    }

    const char nLastSearchChar = (st == ST_COMPAREBYTES) ? pArray[nArraySize - 1] : 0;

    while ((nSize > nArraySize - 1) && (!(pPdStruct->bIsStop))) {
        nTemp = (nSize < nBufferSize) ? nSize : nBufferSize;

        if (m_pConstMemory) {
            pBuffer = (char *)m_pConstMemory + nOffset;
        } else {
            qint64 nBytesRead = read_array_process(nOffset, pBuffer, nTemp, pPdStruct);

            if (nBytesRead != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                break;
            }
        }

        if (st == ST_COMPAREBYTES) {
            // Fast path: single-byte needle with SIMD optimization
            if (nArraySize == 1) {
                nResult = _x_findbyte(pBuffer, nTemp, pArray, nOffset);
                if (nResult != -1) break;
            } else if (bUseBMH) {
                // Boyer–Moore–Horspool pattern search
                nResult = _x_findpattern_bmh(pBuffer, nTemp, pArray, nArraySize, nOffset, bmhShift, nLastSearchChar);
                if (nResult != -1) break;
            } else {
                // Fallback naive scan
                const qint64 limit = nTemp - (nArraySize - 1);
                for (qint64 i = 0; i < limit; ++i) {
                    if (compareMemory(pBuffer + i, pArray, nArraySize)) {
                        nResult = nOffset + i;
                        break;
                    }
                }

                if (nResult != -1) break;
            }
        } else if (st == ST_NOTNULL) {
            nResult = _x_find_notnull(pBuffer, nTemp, nArraySize, nOffset);
            if (nResult != -1) break;
        } else if (st == ST_ANSI) {
            nResult = _x_find_ansi(pBuffer, nTemp, nArraySize, nOffset, ansiTable);
            if (nResult != -1) break;
        } else if (st == ST_NOTANSI) {
            nResult = _x_find_notansi(pBuffer, nTemp, nArraySize, nOffset, ansiTable);
            if (nResult != -1) break;
        } else if (st == ST_NOTANSIANDNULL) {
            nResult = _x_find_notansiandnull(pBuffer, nTemp, nArraySize, nOffset, ansiTable);
            if (nResult != -1) break;
        } else if (st == ST_ANSINUMBER) {
            nResult = _x_find_ansinumber(pBuffer, nTemp, nArraySize, nOffset);
            if (nResult != -1) break;
        }

        if (nResult != -1) {
            break;
        }

        nSize -= nTemp - (nArraySize - 1);
        nOffset += nTemp - (nArraySize - 1);

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset - nStartOffset);
    }

    if (!m_pConstMemory) {
        delete[] pBuffer;
    }

    setPdStructFinished(pPdStruct, _nFreeIndex);

    return nResult;
}

qint64 XBinary::find_array(qint64 nOffset, qint64 nSize, const char *pArray, qint64 nArraySize, PDSTRUCT *pPdStruct)
{
    return _find_array(ST_COMPAREBYTES, nOffset, nSize, pArray, nArraySize, pPdStruct);
}

qint64 XBinary::find_byteArray(qint64 nOffset, qint64 nSize, const QByteArray &baData, PDSTRUCT *pPdStruct)
{
    return _find_array(ST_COMPAREBYTES, nOffset, nSize, baData.data(), baData.size(), pPdStruct);
}

qint64 XBinary::find_uint8(qint64 nOffset, qint64 nSize, quint8 nValue, PDSTRUCT *pPdStruct)
{
    quint8 baValue[1];
    baValue[0] = nValue;
    return find_array(nOffset, nSize, (char *)baValue, 1, pPdStruct);
}

qint64 XBinary::find_int8(qint64 nOffset, qint64 nSize, qint8 nValue, PDSTRUCT *pPdStruct)
{
    quint8 baValue[1];
    baValue[0] = (quint8)nValue;
    return find_array(nOffset, nSize, (char *)baValue, 1, pPdStruct);
}

qint64 XBinary::find_uint16(qint64 nOffset, qint64 nSize, quint16 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    quint8 baValue[2];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(nValue >> 8);
        baValue[1] = (quint8)nValue;
    } else {
        baValue[0] = (quint8)nValue;
        baValue[1] = (quint8)(nValue >> 8);
    }

    return find_array(nOffset, nSize, (char *)baValue, 2, pPdStruct);
}

qint64 XBinary::find_int16(qint64 nOffset, qint64 nSize, qint16 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    quint16 _value = (quint16)nValue;
    quint8 baValue[2];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(_value >> 8);
        baValue[1] = (quint8)_value;
    } else {
        baValue[0] = (quint8)_value;
        baValue[1] = (quint8)(_value >> 8);
    }

    return find_array(nOffset, nSize, (char *)baValue, 2, pPdStruct);
}

qint64 XBinary::find_uint32(qint64 nOffset, qint64 nSize, quint32 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    // // DEBUG: Print for ECD signature search (0x06054B50)
    // if (nValue == 0x06054B50) {
    //     qDebug() << "find_uint32: Searching for ECD signature 0x06054B50 at offset" << nOffset << "nSize=" << nSize;
    // }

    quint8 baValue[4];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(nValue >> 24);
        baValue[1] = (quint8)(nValue >> 16);
        baValue[2] = (quint8)(nValue >> 8);
        baValue[3] = (quint8)nValue;
    } else {
        baValue[0] = (quint8)nValue;
        baValue[1] = (quint8)(nValue >> 8);
        baValue[2] = (quint8)(nValue >> 16);
        baValue[3] = (quint8)(nValue >> 24);
    }

    qint64 nResult = find_array(nOffset, nSize, (char *)baValue, 4, pPdStruct);

    // if (nValue == 0x06054B50) {
    //     qDebug() << "find_uint32: ECD search result:" << nResult;
    // }

    return nResult;
}

qint64 XBinary::find_int32(qint64 nOffset, qint64 nSize, qint32 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    quint32 _value = (quint32)nValue;
    quint8 baValue[4];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(_value >> 24);
        baValue[1] = (quint8)(_value >> 16);
        baValue[2] = (quint8)(_value >> 8);
        baValue[3] = (quint8)_value;
    } else {
        baValue[0] = (quint8)_value;
        baValue[1] = (quint8)(_value >> 8);
        baValue[2] = (quint8)(_value >> 16);
        baValue[3] = (quint8)(_value >> 24);
    }

    return find_array(nOffset, nSize, (char *)baValue, 4, pPdStruct);
}

qint64 XBinary::find_uint64(qint64 nOffset, qint64 nSize, quint64 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    quint8 baValue[8];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(nValue >> 56);
        baValue[1] = (quint8)(nValue >> 48);
        baValue[2] = (quint8)(nValue >> 40);
        baValue[3] = (quint8)(nValue >> 32);
        baValue[4] = (quint8)(nValue >> 24);
        baValue[5] = (quint8)(nValue >> 16);
        baValue[6] = (quint8)(nValue >> 8);
        baValue[7] = (quint8)nValue;
    } else {
        baValue[0] = (quint8)nValue;
        baValue[1] = (quint8)(nValue >> 8);
        baValue[2] = (quint8)(nValue >> 16);
        baValue[3] = (quint8)(nValue >> 24);
        baValue[4] = (quint8)(nValue >> 32);
        baValue[5] = (quint8)(nValue >> 40);
        baValue[6] = (quint8)(nValue >> 48);
        baValue[7] = (quint8)(nValue >> 56);
    }

    return find_array(nOffset, nSize, (char *)baValue, 8, pPdStruct);
}

qint64 XBinary::find_int64(qint64 nOffset, qint64 nSize, qint64 nValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    quint64 _value = (quint64)nValue;
    quint8 baValue[8];

    if (bIsBigEndian) {
        baValue[0] = (quint8)(_value >> 56);
        baValue[1] = (quint8)(_value >> 48);
        baValue[2] = (quint8)(_value >> 40);
        baValue[3] = (quint8)(_value >> 32);
        baValue[4] = (quint8)(_value >> 24);
        baValue[5] = (quint8)(_value >> 16);
        baValue[6] = (quint8)(_value >> 8);
        baValue[7] = (quint8)_value;
    } else {
        baValue[0] = (quint8)_value;
        baValue[1] = (quint8)(_value >> 8);
        baValue[2] = (quint8)(_value >> 16);
        baValue[3] = (quint8)(_value >> 24);
        baValue[4] = (quint8)(_value >> 32);
        baValue[5] = (quint8)(_value >> 40);
        baValue[6] = (quint8)(_value >> 48);
        baValue[7] = (quint8)(_value >> 56);
    }

    return find_array(nOffset, nSize, (char *)baValue, 8, pPdStruct);
}

qint64 XBinary::find_float(qint64 nOffset, qint64 nSize, float fValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    float _value = fValue;

    endian_float(&_value, bIsBigEndian);

    return find_array(nOffset, nSize, (char *)&_value, 4, pPdStruct);
}

qint64 XBinary::find_double(qint64 nOffset, qint64 nSize, double dValue, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    double _value = dValue;

    endian_double(&_value, bIsBigEndian);

    return find_array(nOffset, nSize, (char *)&_value, 8, pPdStruct);
}

void XBinary::endian_float(float *pValue, bool bIsBigEndian)
{
    bool bReverse = false;

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    bReverse = !bIsBigEndian;
#else
    bReverse = bIsBigEndian;
#endif

    if (bReverse) {
        qSwap(((quint8 *)pValue)[0], ((quint8 *)pValue)[3]);
        qSwap(((quint8 *)pValue)[1], ((quint8 *)pValue)[2]);
    }
}

void XBinary::endian_double(double *pValue, bool bIsBigEndian)
{
    bool bReverse = false;

#if Q_BYTE_ORDER == Q_BIG_ENDIAN
    bReverse = !bIsBigEndian;
#else
    bReverse = bIsBigEndian;
#endif

    if (bReverse) {
        qSwap(((quint8 *)pValue)[0], ((quint8 *)pValue)[7]);
        qSwap(((quint8 *)pValue)[1], ((quint8 *)pValue)[6]);
        qSwap(((quint8 *)pValue)[2], ((quint8 *)pValue)[5]);
        qSwap(((quint8 *)pValue)[3], ((quint8 *)pValue)[4]);
    }
}

qint64 XBinary::find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct)
{
    QByteArray baString = sString.toLatin1();

    return find_array(nOffset, nSize, baString.data(), baString.size(), pPdStruct);
}

qint64 XBinary::find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    // TODO convert
    QByteArray baString = getUnicodeString(sString, bIsBigEndian);

    return find_array(nOffset, nSize, (char *)baString.data(), baString.size() * 2, pPdStruct);
}

qint64 XBinary::find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct)
{
    QByteArray baData = sString.toUtf8();

    return find_array(nOffset, nSize, (char *)baData.data(), baData.size(), pPdStruct);
}

QByteArray XBinary::_signatureToSigBytes(const QString &sSignature, PDSTRUCT *pPdStruct)
{
    QByteArray baResult;

    QString _sSignature = convertSignature(sSignature);
    qint32 nSignatureSize = _sSignature.size();

    for (qint32 i = 0; (i < nSignatureSize) && isPdStructNotCanceled(pPdStruct);) {
        QChar cSymbol = _sSignature.at(i);
        QChar cSymbol2;

        if ((i + 1) < nSignatureSize) {
            cSymbol2 = _sSignature.at(i + 1);
        }

        if ((cSymbol == QChar('.')) && (cSymbol2 == QChar('.'))) {
            // Wildcard
            baResult.append((char)SIGBYTETYPE_WILDCARD);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('*')) && (cSymbol2 == QChar('*'))) {
            // Not null byte
            baResult.append((char)SIGBYTE_NOT_NULL);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('%')) && (cSymbol2 == QChar('%'))) {
            // ANSI character
            baResult.append((char)SIGBYTETYPE_ANSI);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('%')) && (cSymbol2 == QChar('&'))) {
            // ANSI alphanumeric
            baResult.append((char)SIGBYTETYPE_ANSI_ALPHANUMERIC);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('!')) && (cSymbol2 == QChar('%'))) {
            // Not ANSI
            baResult.append((char)SIGBYTETYPE_NOT_ANSI);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('_')) && (cSymbol2 == QChar('%'))) {
            // Not ANSI and not null
            baResult.append((char)SIGBYTETYPE_NOT_ANSI_AND_NOT_NULL);
            baResult.append((char)0x00);
            i += 2;
        } else if ((cSymbol == QChar('$')) || (cSymbol == QChar('#')) || (cSymbol == QChar('+')) || (cSymbol == QChar('['))) {
            // Skip relative offset, absolute address, find pattern, and address base markers
            i++;
        } else if (cSymbol == QChar(']')) {
            // Skip address base end marker
            i++;
        } else if ((i + 1) < nSignatureSize) {
            // Hex byte
            QString sHexByte = _sSignature.mid(i, 2);
            bool bOk = false;
            quint8 nValue = (quint8)sHexByte.toUInt(&bOk, 16);

            if (bOk) {
                baResult.append((char)SIGBYTETYPE_HEX);
                baResult.append((char)nValue);
                i += 2;
            } else {
                // Invalid hex, skip
                i++;
            }
        } else {
            // End of signature
            break;
        }
    }

    return baResult;
}

bool XBinary::_compareSigBytes(const QByteArray &baSigBytes, const QByteArray &baData, PDSTRUCT *pPdStruct)
{
    return _compareSigBytes(baSigBytes.constData(), baSigBytes.size(), baData.constData(), baData.size(), pPdStruct);
}

bool XBinary::_compareSigBytes(const char *pSigBytes, qint64 nSigBytesSize, const char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct)
{
    // SigBytes format: pairs of {type:quint8, value:quint8}
    // So nSigBytesSize must be even and represent nSigBytesSize/2 bytes to match
    if ((nSigBytesSize % 2) != 0) {
        return false;
    }

    qint64 nPatternLength = nSigBytesSize / 2;

    // Data must be at least as long as the pattern
    if (nDataSize < nPatternLength) {
        return false;
    }

#ifdef USE_XSIMD
    // Try SIMD path first for better performance
    qint32 nResult = xsimd_compare_sigbytes((const quint8 *)pSigBytes, (qint64)nSigBytesSize, (const quint8 *)pData, (qint64)nDataSize, (const quint8 *)g_alphaNumTable);

    if (nResult >= 0) {
        // SIMD was able to process it
        return (nResult == 1);
    }
    // If nResult < 0, fall back to scalar implementation below
#endif

    const quint8 *pSig = (const quint8 *)pSigBytes;
    const quint8 *pSigEnd = pSig + nSigBytesSize;
    const quint8 *pDat = (const quint8 *)pData;

    // Check cancellation less frequently for better performance
    qint64 nCheckInterval = 64;
    qint64 nNextCheck = nCheckInterval;

    while (pSig < pSigEnd) {
        quint8 nType = *pSig++;
        quint8 nValue = *pSig++;
        quint8 nDataByte = *pDat++;

        // Most common cases first (HEX and WILDCARD)
        if (nType == SIGBYTETYPE_HEX) {
            if (nDataByte != nValue) {
                return false;
            }
        } else if (nType == SIGBYTETYPE_WILDCARD) {
            // Matches any byte, continue
        } else if (nType == SIGBYTE_NOT_NULL) {
            if (nDataByte == 0x00) {
                return false;
            }
        } else if (nType == SIGBYTETYPE_ANSI) {
            // Printable ASCII: 0x20 (space) to 0x7E (~)
            // Optimized: single comparison using unsigned arithmetic
            if ((quint8)(nDataByte - 0x20) >= 0x5F) {
                return false;
            }
        } else if (nType == SIGBYTETYPE_NOT_ANSI) {
            // Not printable ASCII
            if ((quint8)(nDataByte - 0x20) < 0x5F) {
                return false;
            }
        } else if (nType == SIGBYTETYPE_ANSI_ALPHANUMERIC) {
            // Use lookup table for fast classification
            if (!g_alphaNumTable[nDataByte]) {
                return false;
            }
        } else if (nType == SIGBYTETYPE_NOT_ANSI_AND_NOT_NULL) {
            // Not printable ASCII and not null
            // Optimized: combine checks
            if (nDataByte == 0x00 || (quint8)(nDataByte - 0x20) < 0x5F) {
                return false;
            }
        } else {
            // Unknown type, fail
            return false;
        }

        // Check for cancellation periodically
        if ((pSig >= pSigEnd) || ((pSig - (const quint8 *)pSigBytes) >= nNextCheck)) {
            if (!isPdStructNotCanceled(pPdStruct)) {
                return false;
            }
            nNextCheck += nCheckInterval;
        }
    }

    return true;
}

qint64 XBinary::_findSigBytes(qint64 nOffset, qint64 nSize, const char *pSigBytes, qint64 nSigBytesSize, PDSTRUCT *pPdStruct)
{
    // Validate inputs
    if (!pSigBytes || nSigBytesSize <= 0) {
        return -1;
    }

    // SigBytes format uses 2 bytes per pattern byte (type + value)
    if ((nSigBytesSize % 2) != 0) {
        return -1;
    }

    qint64 nPatternLength = nSigBytesSize / 2;

    // Validate region
    qint64 nFileSize = getSize();

    if (nSize == -1) {
        nSize = nFileSize - nOffset;
    }

    if (nSize <= 0 || nOffset < 0 || nOffset + nSize > nFileSize) {
        return -1;
    }

    if (nPatternLength > nSize) {
        return -1;
    }

    // Search through the region
    qint64 nSearchEnd = nOffset + nSize - nPatternLength + 1;

    if (m_pConstMemory) {
        // Fast path: constant memory access
        const char *pFileData = (const char *)m_pConstMemory;

#ifdef USE_XSIMD
        // Try SIMD path first for better performance
        qint64 nResult = xsimd_find_sigbytes((const quint8 *)(pFileData + nOffset), nSize, (const quint8 *)pSigBytes, nSigBytesSize, (const quint8 *)g_alphaNumTable);

        if (nResult >= 0) {
            // SIMD found it, adjust offset
            return nOffset + nResult;
        }
        // If nResult < 0, fall back to scalar implementation below
#endif

        for (qint64 i = nOffset; (i < nSearchEnd) && isPdStructNotCanceled(pPdStruct); i++) {
            if (_compareSigBytes(pSigBytes, nSigBytesSize, pFileData + i, nPatternLength, pPdStruct)) {
                return i;
            }
        }
    } else {
        // Buffered read path
        qint32 nBufferSize = 0x10000;  // 64KB buffer
        QByteArray baBuffer;

        for (qint64 i = nOffset; (i < nSearchEnd) && isPdStructNotCanceled(pPdStruct);) {
            // Read buffer at current position
            qint64 nRemaining = nSearchEnd - i;
            qint64 nReadSize = qMin<qint64>(nBufferSize, nRemaining + nPatternLength - 1);

            baBuffer = read_array(i, nReadSize);

            if (baBuffer.isEmpty()) {
                break;
            }

            // Search within buffer
            qint64 nBufferSearchEnd = baBuffer.size() - nPatternLength + 1;

            for (qint64 j = 0; (j < nBufferSearchEnd) && isPdStructNotCanceled(pPdStruct); j++) {
                if (_compareSigBytes(pSigBytes, nSigBytesSize, baBuffer.constData() + j, nPatternLength, pPdStruct)) {
                    return i + j;
                }
            }

            // Move to next buffer position
            // Overlap by (nPatternLength - 1) to catch patterns spanning buffer boundaries
            qint64 nAdvance = baBuffer.size() - (nPatternLength - 1);

            if (nAdvance <= 0) {
                nAdvance = 1;
            }

            i += nAdvance;
        }
    }

    return -1;
}

qint64 XBinary::find_signature(qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize, PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = XBinary::getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return find_signature(&memoryMap, nOffset, nSize, sSignature, pnResultSize, pPdStruct);
}

qint64 XBinary::find_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    //    bool bDisableSignals=true;

    //    if(pPdStruct->bIsDisable) // If we call find_signature in another
    //    search function
    //    {
    //        bDisableSignals=false; // TODO Check !!!
    //    }

    // TODO CheckSize function
    qint64 _nSize = getSize();

    qint64 nResultSize = 0;

    if (pnResultSize == 0) {
        pnResultSize = &nResultSize;
    }

    if (nSize == -1) {
        nSize = _nSize - nOffset;
    }

    if (nOffset + nSize > _nSize) {
        // return -1;
        nSize = _nSize - nOffset;
    }

    if (nSize <= 0) {
        return -1;
    }

    QString _sSignature = convertSignature(sSignature);

    if (_sSignature.contains(QChar('$')) || _sSignature.contains(QChar('#')) || _sSignature.contains(QChar('+'))) {
        *pnResultSize = 1;
    } else {
        // Fix size
        *pnResultSize = _sSignature.size() / 2;
    }

    qint64 nResult = -1;

    if (_sSignature.contains(QChar('$')) || _sSignature.contains(QChar('#')) || _sSignature.contains(QChar('+'))) {
        bool bIsValid = true;

        QList<SIGNATURE_RECORD> listSignatureRecords = getSignatureRecords(_sSignature, &bIsValid, pPdStruct);

        if (listSignatureRecords.count()) {
            qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

            qint32 nSearchFirstIndex = 0;
            qint64 nDelta = 0;
            ST stIndex = ST_COMPAREBYTES;
            qint64 nSearchMaxSize = 0;

            if (!_sSignature.contains(QChar('+'))) {
                qint32 nNumberOfRecords = listSignatureRecords.count();
                qint64 nCurrentDelta = 0;
                bool bNewAlgo = false;

                for (qint32 i = 0; i < nNumberOfRecords; i++) {
                    if ((listSignatureRecords.at(i).st == ST_ADDRESS) || (listSignatureRecords.at(i).st == ST_RELOFFSET)) {
                        break;
                    } else if ((listSignatureRecords.at(i).nWindowSize > nSearchMaxSize) &&
                               ((listSignatureRecords.at(i).st == ST_COMPAREBYTES) || (listSignatureRecords.at(i).st == ST_FINDBYTES))) {
                        nSearchMaxSize = listSignatureRecords.at(i).nWindowSize;
                        stIndex = listSignatureRecords.at(i).st;
                        nDelta = nCurrentDelta;
                        nSearchFirstIndex = i;
                    } else if (bNewAlgo && (listSignatureRecords.at(i).nWindowSize > nSearchMaxSize) &&
                               ((listSignatureRecords.at(i).st == ST_NOTNULL) || (listSignatureRecords.at(i).st == ST_ANSI) ||
                                (listSignatureRecords.at(i).st == ST_NOTANSI) || (listSignatureRecords.at(i).st == ST_NOTANSIANDNULL) ||
                                (listSignatureRecords.at(i).st == ST_ANSINUMBER))) {
                        nSearchMaxSize = listSignatureRecords.at(i).nWindowSize;
                        stIndex = listSignatureRecords.at(i).st;
                        nDelta = nCurrentDelta;
                        nSearchFirstIndex = i;
                    }
                    nCurrentDelta += listSignatureRecords.at(i).nWindowSize;
                }
            }

            if (nSearchFirstIndex > 0) {
                qint64 nTmpOffset = nOffset + nDelta;
                qint64 nTmpSize = nSize - nDelta;

                XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nTmpSize);

                QByteArray baData = listSignatureRecords.at(nSearchFirstIndex).baData;

                char *pData = baData.data();
                qint32 nDataSize = baData.size();

                for (qint64 i = 0; (i < nTmpSize) && (!(pPdStruct->bIsStop));) {
                    qint64 nCurrentOffset = -1;

                    if ((stIndex == ST_COMPAREBYTES) || (stIndex == ST_FINDBYTES)) {
                        nCurrentOffset = _find_array(stIndex, nTmpOffset + i, nTmpSize - i, pData, nDataSize, pPdStruct);
                    } else {
                        nCurrentOffset = _find_array(stIndex, nTmpOffset + i, nTmpSize - i, 0, nSearchMaxSize, pPdStruct);
                    }

                    if (nCurrentOffset != -1) {
                        if (_compareSignature(pMemoryMap, &listSignatureRecords, nCurrentOffset - nDelta, pPdStruct)) {
                            nResult = nCurrentOffset - nDelta;

                            break;
                        }
                    } else {
                        break;
                    }

                    i = nCurrentOffset + 1 - nTmpOffset;

                    XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, i);
                }
            } else if ((listSignatureRecords.at(0).st == ST_COMPAREBYTES) || (listSignatureRecords.at(0).st == ST_FINDBYTES) ||
                       (listSignatureRecords.at(0).st == ST_NOTNULL) || (listSignatureRecords.at(0).st == ST_ANSI) || (listSignatureRecords.at(0).st == ST_NOTANSI) ||
                       (listSignatureRecords.at(0).st == ST_NOTANSIANDNULL) || (listSignatureRecords.at(0).st == ST_ANSINUMBER)) {
                ST _st = listSignatureRecords.at(0).st;

                if (listSignatureRecords.at(0).st == ST_FINDBYTES) {
                    _st = ST_COMPAREBYTES;
                }

                XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

                QByteArray baFirst = listSignatureRecords.at(0).baData;

                char *pData = baFirst.data();
                // For ST_COMPAREBYTES (including remapped ST_FINDBYTES), use the actual baFirst size.
                // For other ST_* modes, use the recorded window size.
                qint32 nDataSize = (_st == ST_COMPAREBYTES) ? baFirst.size() : listSignatureRecords.at(0).nWindowSize;

                for (qint64 i = 0; (i < nSize) && (!(pPdStruct->bIsStop));) {
                    qint64 nTempOffset = _find_array(_st, nOffset + i, nSize - i, pData, nDataSize, pPdStruct);

                    if (nTempOffset != -1) {
                        if (_compareSignature(pMemoryMap, &listSignatureRecords, nTempOffset, pPdStruct)) {
                            nResult = nTempOffset;

                            break;
                        }
                    } else {
                        break;
                    }

                    i = nTempOffset + 1 - nOffset;

                    XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, i);
                }
            } else {
                for (qint64 i = 0; (i < nSize) && (!(pPdStruct->bIsStop)); i++) {
                    if (_compareSignature(pMemoryMap, &listSignatureRecords, nOffset + i, pPdStruct)) {
                        nResult = nOffset + i;
                        break;
                    }

                    XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, i);
                }
            }

            XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
        }
    } else if (_sSignature.contains(QChar('.')) || _sSignature.contains(QChar('*')) || _sSignature.contains(QChar('%')) || _sSignature.contains(QChar('!')) ||
               _sSignature.contains(QChar('_')) || _sSignature.contains(QChar('&'))) {
        QByteArray baSigBytes = _signatureToSigBytes(_sSignature, pPdStruct);
        if (baSigBytes.size()) {
            // Optimization: Check for leading non-HEX bytes
            // Sigbytes format: each entry is 2 bytes [type, value]
            qint32 nLeadingNonHex = 0;
            qint32 nSigBytesSize = baSigBytes.size();
            const quint8 *pSigBytes = (const quint8 *)baSigBytes.constData();

            // Count leading non-HEX bytes (each entry is 2 bytes: [SIGBYTETYPE, value])
            for (qint32 i = 0; i < nSigBytesSize; i += 2) {
                quint8 nType = pSigBytes[i];
                if (nType != SIGBYTETYPE_HEX) {
                    nLeadingNonHex++;
                } else {
                    break;
                }
            }

            // If we have >=3 leading non-HEX bytes, extract first fixed pattern and search for it
            if (nLeadingNonHex >= 3) {
                // Find first fixed hex byte sequence (at least 3 bytes)
                qint32 nFixedStartIndex = nLeadingNonHex * 2;  // Byte offset in sigbytes
                qint32 nFixedLength = 0;

                for (qint32 i = nFixedStartIndex; i < nSigBytesSize; i += 2) {
                    quint8 nType = pSigBytes[i];
                    if (nType == SIGBYTETYPE_HEX) {
                        nFixedLength++;
                    } else {
                        break;
                    }
                }

                // If fixed pattern is >=3 bytes, use optimized search
                if (nFixedLength >= 3) {
                    // Extract actual hex values (second byte of each [type, value] pair)
                    QByteArray baFixedPattern;
                    baFixedPattern.reserve(nFixedLength);
                    for (qint32 i = 0; i < nFixedLength; i++) {
                        baFixedPattern.append((char)pSigBytes[nFixedStartIndex + (i * 2) + 1]);
                    }

                    qint64 nCurrentOffset = nOffset;
                    qint64 nRemainingSize = nSize;

                    while (isPdStructNotCanceled(pPdStruct)) {
                        // Search for fixed pattern
                        qint64 nFoundOffset = find_array(nCurrentOffset, nRemainingSize, baFixedPattern.constData(), baFixedPattern.size(), pPdStruct);

                        if (nFoundOffset == -1) {
                            break;
                        }

                        // Check if full signature matches at position - nLeadingNonHex
                        qint64 nCheckOffset = nFoundOffset - nLeadingNonHex;

                        if (nCheckOffset >= nOffset) {
                            // Verify the full signature at this position
                            if (compareSignature(pMemoryMap, _sSignature, nCheckOffset, pPdStruct)) {
                                nResult = nCheckOffset;
                                break;
                            }
                        }

                        // Continue searching after this match
                        nCurrentOffset = nFoundOffset + 1;
                        if (nCurrentOffset >= nOffset + nSize) {
                            break;
                        }
                        nRemainingSize = nSize - (nCurrentOffset - nOffset);
                    }
                } else {
                    // Fixed pattern too short, use normal search
                    nResult = _findSigBytes(nOffset, nSize, baSigBytes.constData(), baSigBytes.size(), pPdStruct);
                }
            } else {
                // No leading non-HEX bytes or too few, use normal search
                nResult = _findSigBytes(nOffset, nSize, baSigBytes.constData(), baSigBytes.size(), pPdStruct);
            }
        }

    } else {
        QByteArray baData = QByteArray::fromHex(QByteArray(_sSignature.toLatin1().data()));

        if (baData.size()) {
            nResult = find_array(nOffset, nSize, baData.data(), baData.size(), pPdStruct);
        }
    }

    return nResult;
}

static qint64 _x_find_ansi_string_i(char *pBuffer, qint64 nTemp, qint64 nStringSize, const quint8 *pUpperData, const quint8 *pLowerData, qint64 nOffset)
{
    qint64 nResult = -1;
    qint64 nSearchEnd = nTemp - (nStringSize - 1);

#ifdef USE_XSIMD
    nResult = xsimd_find_ansi_string_i(pBuffer, nTemp, (const char *)pUpperData, nStringSize, nOffset);
    if (nResult != -1) return nResult;
#else
    quint8 *pData = (quint8 *)pBuffer;
    for (qint64 i = 0; i < nSearchEnd; i++) {
        // Inline comparison for better performance
        bool bMatch = true;
        quint8 *pCurrent = pData + i;
        const quint8 *pUpper = pUpperData;
        const quint8 *pLower = pLowerData;
        qint64 nRemaining = nStringSize;

        while (nRemaining > 0) {
            if ((*pCurrent != *pUpper) && (*pCurrent != *pLower)) {
                bMatch = false;
                break;
            }
            pCurrent++;
            pUpper++;
            pLower++;
            nRemaining--;
        }

        if (bMatch) {
            nResult = nOffset + i;
            break;
        }
    }
#endif

    return nResult;
}

qint64 XBinary::find_ansiStringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint64 nResult = -1;
    // TODO CheckSize function
    qint64 nStringSize = sString.size();
    qint64 _nSize = getSize();

    if (nSize == -1) {
        nSize = _nSize - nOffset;
    }

    if ((nSize > 0) && (nOffset + nSize <= _nSize) && (nStringSize <= nSize)) {
        qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        qint64 nTemp = 0;

        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        QByteArray baUpper = sString.toUpper().toLatin1();
        QByteArray baLower = sString.toLower().toLatin1();

        const quint8 *pUpperData = (const quint8 *)baUpper.constData();
        const quint8 *pLowerData = (const quint8 *)baLower.constData();

        qint64 nStartOffset = nOffset;

        while ((nSize > nStringSize - 1) && isPdStructNotCanceled(pPdStruct)) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));

                break;
            }

            nResult = _x_find_ansi_string_i(pBuffer, nTemp, nStringSize, pUpperData, pLowerData, nOffset);

            if (nResult != -1) {
                break;
            }

            nSize -= nTemp - (nStringSize - 1);
            nOffset += nTemp - (nStringSize - 1);

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset - nStartOffset);
        }

        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

        delete[] pBuffer;
    }

    return nResult;
}

qint64 XBinary::find_unicodeStringI(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    qint64 nResult = -1;
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }
    // TODO CheckSize function
    qint64 nStringSize = sString.size();
    qint64 _nSize = getSize();

    if (nSize == -1) {
        nSize = _nSize - nOffset;
    }

    if (nSize <= 0) {
        return -1;
    }

    if (nOffset + nSize > _nSize) {
        return -1;
    }

    if (nStringSize > nSize) {
        return -1;
    }

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint64 nTemp = 0;

    qint32 nBufferSize = getBufferSize(pPdStruct);
    char *pBuffer = new char[nBufferSize];

    QByteArray baUpper = getUnicodeString(sString.toUpper(), bIsBigEndian);
    QByteArray baLower = getUnicodeString(sString.toLower(), bIsBigEndian);

    const quint16 *pUpperData = (const quint16 *)baUpper.constData();
    const quint16 *pLowerData = (const quint16 *)baLower.constData();

    qint64 nStartOffset = nOffset;
    qint64 nByteStringSize = 2 * nStringSize;

    while ((nSize > 2 * (nStringSize - 1)) && (!(pPdStruct->bIsStop))) {
        nTemp = qMin((qint64)nBufferSize, nSize);

        if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
            setPdStructErrorString(pPdStruct, tr("Read error"));
            break;
        }

        quint16 *pData = (quint16 *)pBuffer;
        qint64 nSearchEnd = nTemp - 2 * (nStringSize - 1);

        for (qint64 i = 0; i < nSearchEnd; i++) {
            // Inline comparison for better performance
            bool bMatch = true;
            quint16 *pCurrent = (quint16 *)(pBuffer + i);
            const quint16 *pUpper = pUpperData;
            const quint16 *pLower = pLowerData;
            qint64 nRemaining = nStringSize;

            while (nRemaining > 0) {
                if ((*pCurrent != *pUpper) && (*pCurrent != *pLower)) {
                    bMatch = false;
                    break;
                }
                pCurrent++;
                pUpper++;
                pLower++;
                nRemaining--;
            }

            if (bMatch) {
                nResult = nOffset + i;
                break;
            }
        }

        if (nResult != -1) {
            break;
        }

        nSize -= nTemp - 2 * (nStringSize - 1);
        nOffset += nTemp - 2 * (nStringSize - 1);

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset - nStartOffset);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    delete[] pBuffer;

    return nResult;
}

qint64 XBinary::find_utf8StringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct)
{
    // TODO !!!
    // Double
    return find_utf8String(nOffset, nSize, sString, pPdStruct);
}

quint8 XBinary::getBits_uint8(quint8 nValue, qint32 nBitOffset, qint32 nBitSize)
{
    quint8 nResult = nValue;

    if (nBitSize) {
        quint8 nMask = 0xFF;

        for (qint32 i = 0; i < nBitOffset; i++) {
            nResult = nResult >> 1;
        }

        for (qint32 i = 0; i < nBitSize; i++) {
            nMask = nMask << 1;
        }

        nResult = nResult & (~nMask);
    }

    return nResult;
}

quint16 XBinary::getBits_uint16(quint16 nValue, qint32 nBitOffset, qint32 nBitSize)
{
    quint16 nResult = nValue;

    if (nBitSize) {
        quint16 nMask = 0xFFFF;

        for (qint32 i = 0; i < nBitOffset; i++) {
            nResult = nResult >> 1;
        }

        for (qint32 i = 0; i < nBitSize; i++) {
            nMask = nMask << 1;
        }

        nResult = nResult & (~nMask);
    }

    return nResult;
}

quint32 XBinary::getBits_uint32(quint32 nValue, qint32 nBitOffset, qint32 nBitSize)
{
    quint32 nResult = nValue;

    if (nBitSize) {
        quint32 nMask = 0xFFFFFFFF;

        for (qint32 i = 0; i < nBitOffset; i++) {
            nResult = nResult >> 1;
        }

        for (qint32 i = 0; i < nBitSize; i++) {
            nMask = nMask << 1;
        }

        nResult = nResult & (~nMask);
    }

    return nResult;
}

quint64 XBinary::getBits_uint64(quint64 nValue, qint32 nBitOffset, qint32 nBitSize)
{
    quint64 nResult = nValue;

    if (nBitSize) {
        quint64 nMask = 0xFFFFFFFFFFFFFFFF;

        for (qint32 i = 0; i < nBitOffset; i++) {
            nResult = nResult >> 1;
        }

        for (qint32 i = 0; i < nBitSize; i++) {
            nMask = nMask << 1;
        }

        nResult = nResult & (~nMask);
    }

    return nResult;
}

bool XBinary::_addMultiSearchStringRecord(QVector<MS_RECORD> *pList, MS_RECORD *pRecord, QString sString, STRINGSEARCH_OPTIONS *pSsOptions)
{
    bool bResult = false;

    bool bAdd = true;

    if (pSsOptions->bLinks) {
        bAdd = false;

        if (sString.contains("http:") || sString.contains("www.") || sString.contains("mailto:")) {
            bAdd = true;
        }
    }

    if (pSsOptions->sMask != "") {
        bAdd = isRegExpPresent(pSsOptions->sMask, sString);
    }

    if (bAdd) {
        pList->append(*pRecord);

        bResult = true;
    }

    return bResult;
}

// Optimized version that accepts pre-compiled regex to avoid creating QRegularExpression repeatedly
bool XBinary::_addMultiSearchStringRecordOptimized(QVector<MS_RECORD> *pList, MS_RECORD *pRecord, const QString &sString, STRINGSEARCH_OPTIONS *pSsOptions,
                                                   QRegularExpression *pRegex)
{
    bool bAdd = true;

    if (pSsOptions->bLinks) {
        // Early rejection - most strings don't contain links
        // Use indexOf for faster rejection than contains()
        bAdd = (sString.indexOf("http:", 0, Qt::CaseSensitive) != -1 || sString.indexOf("www.", 0, Qt::CaseSensitive) != -1 ||
                sString.indexOf("mailto:", 0, Qt::CaseSensitive) != -1);
    }

    if (bAdd && pRegex) {
        // Use pre-compiled regex - MUCH faster than creating new QRegularExpression each time
        QRegularExpressionMatch match = pRegex->match(sString);
        bAdd = match.hasMatch();
    }

    if (bAdd) {
        pList->append(*pRecord);
        return true;
    }

    return false;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_allStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    QVector<XBinary::MS_RECORD> listResult;

    if (ssOptions.nMinLenght == 0) {
        ssOptions.nMinLenght = 1;
    }

    if (ssOptions.nMaxLenght == 0) {
        ssOptions.nMaxLenght = 128;  // TODO Check
    }

    qint64 _nSize = nSize;
    qint64 _nOffset = nOffset;
    qint64 _nRawOffset = 0;

    bool bReadError = false;

    qint32 nBufferSize = getBufferSize(pPdStruct);
    char *pBuffer = new char[nBufferSize];

    char *pAnsiBuffer = new char[ssOptions.nMaxLenght + 1];

    quint16 *pUnicodeBuffer[2] = {new quint16[ssOptions.nMaxLenght + 1], new quint16[ssOptions.nMaxLenght + 1]};
    qint64 nCurrentUnicodeSize[2] = {0, 0};
    qint64 nCurrentUnicodeOffset[2] = {0, 0};

    qint64 nCurrentAnsiSize = 0;
    qint64 nCurrentAnsiOffset = 0;

    bool bIsStart = true;
    char cPrevSymbol = 0;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint32 nCurrentRecords = 0;

    while ((_nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nCurrentSize = _nSize;

        nCurrentSize = qMin((qint64)nBufferSize, nCurrentSize);

        if (read_array_process(_nOffset, pBuffer, nCurrentSize, pPdStruct) != nCurrentSize) {
            bReadError = true;
            break;
        }

        for (qint64 i = 0; i < nCurrentSize; i++) {
            bool bIsEnd = ((i == (nCurrentSize - 1)) && (_nSize == nCurrentSize));
            qint32 nParity = (_nOffset + i) % 2;

            char cSymbol = *(pBuffer + i);

            bool bIsAnsiSymbol = false;
            bool bLongString = false;

            if (ssOptions.bAnsi) {
                bIsAnsiSymbol = isAnsiSymbol((quint8)cSymbol);
            }

            if (bIsAnsiSymbol) {
                if (nCurrentAnsiSize == 0) {
                    nCurrentAnsiOffset = _nOffset + i;
                }

                if (nCurrentAnsiSize < ssOptions.nMaxLenght) {
                    *(pAnsiBuffer + nCurrentAnsiSize) = cSymbol;
                } else {
                    bIsAnsiSymbol = false;
                    bLongString = true;
                }

                nCurrentAnsiSize++;
            }

            if ((!bIsAnsiSymbol) || (bIsEnd)) {
                if (nCurrentAnsiSize >= ssOptions.nMinLenght) {
                    if (nCurrentAnsiSize - 1 < ssOptions.nMaxLenght) {
                        pAnsiBuffer[nCurrentAnsiSize] = 0;
                    } else {
                        pAnsiBuffer[ssOptions.nMaxLenght] = 0;
                    }

                    if (ssOptions.bAnsi) {
                        QString sString;

                        sString = pAnsiBuffer;

                        bool bAdd = true;

                        if (ssOptions.bNullTerminated && cSymbol && (!bLongString)) {
                            bAdd = false;
                        }

                        if (bAdd) {
                            MS_RECORD record = {};
                            record.nValueType = VT_A;
                            record.nSize = nCurrentAnsiSize;
                            record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                            if (record.nRegionIndex != -1) {
                                record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                            } else {
                                record.nRelOffset = nCurrentAnsiOffset;
                            }

                            if (_addMultiSearchStringRecord(&listResult, &record, sString, &ssOptions)) {
                                nCurrentRecords++;
                            }

                            if (nCurrentRecords >= ssOptions.nLimit) {
                                break;
                            }
                        }
                    }
                }

                nCurrentAnsiSize = 0;
            }

            if (!bIsStart) {
                quint16 nCode = cPrevSymbol + (cSymbol << 8);  // TODO BE/LE

                bool bIsUnicodeSymbol = false;

                if (ssOptions.bUnicode) {
                    bIsUnicodeSymbol = isUnicodeSymbol(nCode, true);
                }

                if (nCurrentUnicodeSize[nParity] >= ssOptions.nMaxLenght) {
                    bIsUnicodeSymbol = false;
                    bLongString = true;
                }

                if (bIsUnicodeSymbol) {
                    if (nCurrentUnicodeSize[nParity] == 0) {
                        nCurrentUnicodeOffset[nParity] = _nOffset - 1 + i;
                    }

                    if (nCurrentUnicodeSize[nParity] < ssOptions.nMaxLenght) {
                        *(pUnicodeBuffer[nParity] + nCurrentUnicodeSize[nParity]) = nCode;
                    }

                    nCurrentUnicodeSize[nParity]++;
                }

                if ((!bIsUnicodeSymbol) || (bIsEnd)) {
                    if (nCurrentUnicodeSize[nParity] >= ssOptions.nMinLenght) {
                        if (nCurrentUnicodeSize[nParity] - 1 < ssOptions.nMaxLenght) {
                            pUnicodeBuffer[nParity][nCurrentUnicodeSize[nParity]] = 0;
                        } else {
                            pUnicodeBuffer[nParity][ssOptions.nMaxLenght] = 0;
                        }

                        if (ssOptions.bUnicode) {
                            QString sString = QString::fromUtf16(pUnicodeBuffer[nParity]);  // TODO Check Qt6

                            bool bAdd = true;

                            if (ssOptions.bNullTerminated && nCode && (!bLongString)) {
                                bAdd = false;
                            }

                            if (bAdd) {
                                MS_RECORD record = {};
                                record.nValueType = VT_U;
                                record.nSize = nCurrentUnicodeSize[nParity] * 2;
                                record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentUnicodeOffset[nParity]);

                                if (record.nRegionIndex != -1) {
                                    record.nRelOffset = nCurrentUnicodeOffset[nParity] - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                                } else {
                                    record.nRelOffset = nCurrentUnicodeOffset[nParity];
                                }

                                if (_addMultiSearchStringRecord(&listResult, &record, sString, &ssOptions)) {
                                    nCurrentRecords++;
                                }

                                if (nCurrentRecords >= ssOptions.nLimit) {
                                    break;
                                }
                            }
                        }
                    }

                    if (bIsEnd) {
                        qint32 nO = (nParity == 1) ? (0) : (1);

                        if (nCurrentUnicodeSize[nO] >= ssOptions.nMinLenght) {
                            if (nCurrentUnicodeSize[nO] - 1 < ssOptions.nMaxLenght) {
                                pUnicodeBuffer[nO][nCurrentUnicodeSize[nO]] = 0;
                            } else {
                                pUnicodeBuffer[nO][ssOptions.nMaxLenght] = 0;
                            }

                            if (ssOptions.bUnicode) {
                                QString sString = QString::fromUtf16(pUnicodeBuffer[nO]);  // TODO Check Qt6

                                bool bAdd = true;

                                if (bAdd) {
                                    MS_RECORD record = {};
                                    record.nValueType = VT_U;
                                    record.nSize = nCurrentUnicodeSize[nO] * 2;
                                    record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentUnicodeOffset[nO]);
                                    // record.sString = sString;

                                    if (record.nRegionIndex != -1) {
                                        record.nRelOffset = nCurrentUnicodeOffset[nO] - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                                    } else {
                                        record.nRelOffset = nCurrentUnicodeOffset[nO];
                                    }

                                    if (_addMultiSearchStringRecord(&listResult, &record, sString, &ssOptions)) {
                                        nCurrentRecords++;
                                    }

                                    if (nCurrentRecords >= ssOptions.nLimit) {
                                        break;
                                    }
                                }
                            }
                        }
                    }

                    nCurrentUnicodeSize[nParity] = 0;
                }
            }

            cPrevSymbol = cSymbol;

            if (bIsStart) {
                bIsStart = false;
            }
        }

        _nSize -= nCurrentSize;
        _nOffset += nCurrentSize;
        _nRawOffset += nCurrentSize;

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, _nOffset - nOffset);

        if (nCurrentRecords >= ssOptions.nLimit) {
            setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(nCurrentRecords)));

            break;
        }
    }

    if (bReadError) {
        setPdStructErrorString(pPdStruct, tr("Read error"));
    }

    delete[] pBuffer;
    delete[] pAnsiBuffer;
    delete[] pUnicodeBuffer[0];
    delete[] pUnicodeBuffer[1];

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_allStrings2(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct)
{
    // Optimized implementation: Use specialized functions and combine results
    QVector<XBinary::MS_RECORD> listResult;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    // Calculate total limit to distribute between ANSI and Unicode searches
    qint32 nTotalLimit = ssOptions.nLimit;

    // Search for ANSI strings if flag is set
    if (ssOptions.bAnsi) {
        STRINGSEARCH_OPTIONS ansiOptions = ssOptions;
        ansiOptions.bUnicode = false;  // ANSI only
        ansiOptions.bAnsi = true;

        QVector<XBinary::MS_RECORD> listAnsi = multiSearch_ansiStrings(pMemoryMap, nOffset, nSize, ansiOptions, pPdStruct);
        listResult.append(listAnsi);

        // Adjust remaining limit
        if (nTotalLimit > 0) {
            nTotalLimit -= listAnsi.size();
            if (nTotalLimit <= 0) {
                nTotalLimit = 0;
            }
        }
    }

    // Search for Unicode strings if flag is set and we haven't reached limit
    if (ssOptions.bUnicode && (nTotalLimit != 0)) {
        STRINGSEARCH_OPTIONS unicodeOptions = ssOptions;
        unicodeOptions.bAnsi = false;  // Unicode only
        unicodeOptions.bUnicode = true;
        unicodeOptions.nLimit = nTotalLimit;  // Apply remaining limit

        QVector<XBinary::MS_RECORD> listUnicode = multiSearch_unicodeStrings(pMemoryMap, nOffset, nSize, unicodeOptions, pPdStruct);
        listResult.append(listUnicode);
    }

    // Sort results by region index first, then by relative offset within region
    if (listResult.size() > 1) {
        std::sort(listResult.begin(), listResult.end(), compareMS_RECORD);
    }

    // Enforce final limit if needed
    if (ssOptions.nLimit > 0 && listResult.size() > ssOptions.nLimit) {
        listResult.resize(ssOptions.nLimit);
        setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(ssOptions.nLimit)));
    }

    return listResult;
}

static qint32 _x_get_simd_threshold()
{
    qint32 nSimdThreshold = 4;

#ifdef USE_XSIMD
    // Pre-calculate SIMD threshold once based on available features
    nSimdThreshold = xsimd_is_avx2_enabled() ? 8 : 4;
#endif

    return nSimdThreshold;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_ansiStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    QVector<MS_RECORD> listResult;

    if (ssOptions.nMinLenght == 0) {
        ssOptions.nMinLenght = 1;
    }

    if (ssOptions.nMaxLenght == 0) {
        ssOptions.nMaxLenght = 128;
    }

    qint64 _nOffset = nOffset;
    qint64 _nSize = nSize;

    bool bReadError = false;

    const qint32 BUFFER_SIZE = 0x10000;  // 64KB chunks for efficient processing

    char *pBuffer = new char[BUFFER_SIZE];
    char *pAnsiBuffer = new char[ssOptions.nMaxLenght + 1];

    qint64 nCurrentAnsiSize = 0;
    qint64 nCurrentAnsiOffset = 0;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint32 nCurrentRecords = 0;

    // OPTIMIZATION: Pre-compile regex once instead of creating it for every string!
    QRegularExpression *pRegex = nullptr;
    if (ssOptions.sMask != "") {
        pRegex = new QRegularExpression(ssOptions.sMask);
        // Optimize regex for repeated matching
        pRegex->optimize();
    }

    qint32 nSimdThreshold = _x_get_simd_threshold();

    while ((_nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nCurrentSize = qMin((qint64)BUFFER_SIZE, _nSize);

        if (read_array_process(_nOffset, pBuffer, nCurrentSize, pPdStruct) != nCurrentSize) {
            bReadError = true;
            break;
        }

#ifdef USE_XSIMD
        // SIMD-optimized: aggressively use xsimd for maximum performance
        // Process entire buffer with SIMD acceleration
        qint64 i = 0;

        while ((i < nCurrentSize) && (nCurrentRecords < ssOptions.nLimit) && isPdStructNotCanceled(pPdStruct)) {
            bool bIsEnd = ((i == (nCurrentSize - 1)) && (_nSize == nCurrentSize));
            char cSymbol = *(pBuffer + i);
            bool bIsAnsiSymbol = isAnsiSymbol((quint8)cSymbol);

            if (bIsAnsiSymbol) {
                if (nCurrentAnsiSize == 0) {
                    nCurrentAnsiOffset = _nOffset + i;

                    // Use SIMD to find the end of the ANSI string quickly
                    qint64 nRemaining = nCurrentSize - i;
                    qint64 nMaxScan = qMin(nRemaining, ssOptions.nMaxLenght);

                    if (nMaxScan >= nSimdThreshold) {
                        // Use xsimd to count consecutive ANSI characters
                        qint64 nAnsiRun = xsimd_count_ansi_prefix(pBuffer + i, nMaxScan);

                        if (nAnsiRun >= nSimdThreshold) {
                            // Found substantial ANSI run - bulk copy with memcpy
                            qint64 nCopySize = qMin(nAnsiRun, ssOptions.nMaxLenght);
                            memcpy(pAnsiBuffer, pBuffer + i, nCopySize);
                            nCurrentAnsiSize = nCopySize;
                            i += nCopySize;

                            // Check if string actually ended or just reached buffer boundary
                            bool bStringEnded = false;
                            if (i < nCurrentSize) {
                                // We're still within buffer - check if next char is non-ANSI
                                if (!isAnsiSymbol((quint8)pBuffer[i])) {
                                    bStringEnded = true;
                                }
                            } else {
                                // Hit buffer boundary - string may continue in next buffer
                                // Only end string if this is the absolute end of all data
                                if (_nSize == nCurrentSize) {
                                    bStringEnded = true;
                                }
                                // Otherwise string continues in next buffer - preserve state
                            }

                            if (bStringEnded) {
                                // String ended - process it
                                bool bLongString = (nCurrentAnsiSize >= ssOptions.nMaxLenght);

                                if (nCurrentAnsiSize >= ssOptions.nMinLenght) {
                                    pAnsiBuffer[nCurrentAnsiSize] = 0;
                                    bool bAdd = true;

                                    if (ssOptions.bNullTerminated && i < nCurrentSize && pBuffer[i] && (!bLongString)) {
                                        bAdd = false;
                                    }

                                    if (bAdd) {
                                        MS_RECORD record = {};
                                        record.nValueType = VT_A;
                                        record.nSize = nCurrentAnsiSize;
                                        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                                        if (record.nRegionIndex != -1) {
                                            record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                                        } else {
                                            record.nRelOffset = nCurrentAnsiOffset;
                                        }

                                        QString sString = pAnsiBuffer;
                                        if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                                            nCurrentRecords++;
                                        }
                                    }
                                }

                                nCurrentAnsiSize = 0;
                                continue;
                            }

                            // String continues beyond SIMD scan - continue byte-by-byte
                            continue;
                        }
                    }
                }

                // Single character handling (short strings or continuation)
                if (nCurrentAnsiSize < ssOptions.nMaxLenght) {
                    *(pAnsiBuffer + nCurrentAnsiSize) = cSymbol;
                    nCurrentAnsiSize++;
                    i++;
                } else {
                    // String too long
                    if (nCurrentAnsiSize >= ssOptions.nMinLenght) {
                        pAnsiBuffer[ssOptions.nMaxLenght] = 0;

                        MS_RECORD record = {};
                        record.nValueType = VT_A;
                        record.nSize = ssOptions.nMaxLenght;
                        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                        if (record.nRegionIndex != -1) {
                            record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                        } else {
                            record.nRelOffset = nCurrentAnsiOffset;
                        }

                        QString sString = pAnsiBuffer;
                        if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                            nCurrentRecords++;
                        }
                    }

                    nCurrentAnsiSize = 0;
                    i++;
                }
            } else {
                // Non-ANSI character - terminate current string if any
                if (nCurrentAnsiSize >= ssOptions.nMinLenght) {
                    pAnsiBuffer[nCurrentAnsiSize] = 0;
                    bool bAdd = true;

                    if (ssOptions.bNullTerminated && cSymbol) {
                        bAdd = false;
                    }

                    if (bAdd) {
                        MS_RECORD record = {};
                        record.nValueType = VT_A;
                        record.nSize = nCurrentAnsiSize;
                        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                        if (record.nRegionIndex != -1) {
                            record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                        } else {
                            record.nRelOffset = nCurrentAnsiOffset;
                        }

                        QString sString = pAnsiBuffer;
                        if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                            nCurrentRecords++;
                        }
                    }
                }

                nCurrentAnsiSize = 0;
                i++;
            }

            // Handle end of buffer - DON'T terminate partial strings at buffer boundary
            // Only save strings if we're at the actual end of data OR if string is complete
            if (bIsEnd && nCurrentAnsiSize >= ssOptions.nMinLenght) {
                // Only save if this is truly the end of all data (not just buffer boundary)
                if (_nSize == nCurrentSize) {
                    pAnsiBuffer[nCurrentAnsiSize] = 0;

                    MS_RECORD record = {};
                    record.nValueType = VT_A;
                    record.nSize = nCurrentAnsiSize;
                    record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                    if (record.nRegionIndex != -1) {
                        record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                    } else {
                        record.nRelOffset = nCurrentAnsiOffset;
                    }

                    QString sString = pAnsiBuffer;
                    if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                        nCurrentRecords++;
                    }

                    nCurrentAnsiSize = 0;
                }
                // Otherwise, partial string continues in next buffer - preserve state
            }
        }
#else
        // Fallback: non-SIMD implementation
        for (qint64 i = 0; (i < nCurrentSize) && (nCurrentRecords < ssOptions.nLimit) && isPdStructNotCanceled(pPdStruct); i++) {
            bool bIsEnd = ((i == (nCurrentSize - 1)) && (_nSize == nCurrentSize));
            char cSymbol = *(pBuffer + i);
            bool bIsAnsiSymbol = isAnsiSymbol((quint8)cSymbol);
            bool bLongString = false;

            if (bIsAnsiSymbol) {
                if (nCurrentAnsiSize == 0) {
                    nCurrentAnsiOffset = _nOffset + i;
                }

                if (nCurrentAnsiSize < ssOptions.nMaxLenght) {
                    *(pAnsiBuffer + nCurrentAnsiSize) = cSymbol;
                } else {
                    bIsAnsiSymbol = false;
                    bLongString = true;
                }

                nCurrentAnsiSize++;
            }

            if ((!bIsAnsiSymbol) || (bIsEnd)) {
                if (nCurrentAnsiSize >= ssOptions.nMinLenght) {
                    if (nCurrentAnsiSize - 1 < ssOptions.nMaxLenght) {
                        pAnsiBuffer[nCurrentAnsiSize] = 0;
                    } else {
                        pAnsiBuffer[ssOptions.nMaxLenght] = 0;
                    }

                    bool bAdd = true;

                    if (ssOptions.bNullTerminated && cSymbol && (!bLongString)) {
                        bAdd = false;
                    }

                    if (bAdd) {
                        MS_RECORD record = {};
                        record.nValueType = VT_A;
                        record.nSize = nCurrentAnsiSize;
                        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentAnsiOffset);

                        if (record.nRegionIndex != -1) {
                            record.nRelOffset = nCurrentAnsiOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                        } else {
                            record.nRelOffset = nCurrentAnsiOffset;
                        }

                        QString sString = pAnsiBuffer;
                        if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                            nCurrentRecords++;
                        }
                    }
                }

                nCurrentAnsiSize = 0;
            }
        }
#endif

        _nSize -= nCurrentSize;
        _nOffset += nCurrentSize;

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, _nOffset - nOffset);

        if (nCurrentRecords >= ssOptions.nLimit) {
            setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(nCurrentRecords)));
            break;
        }
    }

    if (bReadError) {
        setPdStructErrorString(pPdStruct, tr("Read error"));
    }

    delete[] pBuffer;
    delete[] pAnsiBuffer;

    // Clean up pre-compiled regex
    if (pRegex) {
        delete pRegex;
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_unicodeStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions,
                                                                PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    QVector<MS_RECORD> listResult;

    if (ssOptions.nMinLenght == 0) {
        ssOptions.nMinLenght = 1;
    }

    if (ssOptions.nMaxLenght == 0) {
        ssOptions.nMaxLenght = 128;
    }

    qint64 _nOffset = nOffset;
    qint64 _nSize = nSize;

    bool bReadError = false;

    const qint32 BUFFER_SIZE = 0x10000;  // 64KB chunks for efficient processing

    char *pBuffer = new char[BUFFER_SIZE];
    quint16 *pUnicodeBuffer[2];  // Two buffers for even/odd parity
    pUnicodeBuffer[0] = new quint16[ssOptions.nMaxLenght + 1];
    pUnicodeBuffer[1] = new quint16[ssOptions.nMaxLenght + 1];

    qint64 nCurrentUnicodeSize[2] = {0, 0};  // Track both parities
    qint64 nCurrentUnicodeOffset[2] = {0, 0};
    char cPrevSymbol = 0;  // For building 16-bit Unicode characters

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint32 nCurrentRecords = 0;

    // OPTIMIZATION: Pre-compile regex once instead of creating it for every string!
    QRegularExpression *pRegex = nullptr;
    if (ssOptions.sMask != "") {
        pRegex = new QRegularExpression(ssOptions.sMask);
        pRegex->optimize();
    }

    bool bIsStart = true;  // Track if we're at the start of processing

    while ((_nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nCurrentSize = qMin((qint64)BUFFER_SIZE, _nSize);

        if (read_array_process(_nOffset, pBuffer, nCurrentSize, pPdStruct) != nCurrentSize) {
            bReadError = true;
            break;
        }

        // Process buffer byte-by-byte to build Unicode (UTF-16) characters
        // Unicode strings are 16-bit (2 bytes per character)
        for (qint64 i = 0; (i < nCurrentSize) && (nCurrentRecords < ssOptions.nLimit) && isPdStructNotCanceled(pPdStruct); i++) {
            char cSymbol = *(pBuffer + i);
            bool bIsEnd = ((i == (nCurrentSize - 1)) && (_nSize == nCurrentSize));
            qint32 nParity = (_nOffset + i) % 2;  // Track even/odd byte position

            if (!bIsStart) {
                // Build 16-bit Unicode character (little-endian by default)
                quint16 nCode = (quint8)cPrevSymbol + ((quint8)cSymbol << 8);

                // Check if this is a valid Unicode character
                bool bIsUnicodeSymbol = isUnicodeSymbol(nCode, true);
                bool bLongString = false;

                if (bIsUnicodeSymbol) {
                    if (nCurrentUnicodeSize[nParity] == 0) {
                        // Start of new Unicode string (offset points to first byte of first char)
                        nCurrentUnicodeOffset[nParity] = _nOffset + i - 1;
                    }

                    if (nCurrentUnicodeSize[nParity] < ssOptions.nMaxLenght) {
                        *(pUnicodeBuffer[nParity] + nCurrentUnicodeSize[nParity]) = nCode;
                        nCurrentUnicodeSize[nParity]++;
                    } else {
                        // String too long - save it
                        bIsUnicodeSymbol = false;
                        bLongString = true;

                        if (nCurrentUnicodeSize[nParity] >= ssOptions.nMinLenght) {
                            pUnicodeBuffer[nParity][ssOptions.nMaxLenght] = 0;
                            QString sString = QString::fromUtf16(pUnicodeBuffer[nParity], ssOptions.nMaxLenght);

                            MS_RECORD record = {};
                            record.nValueType = VT_U;
                            record.nSize = ssOptions.nMaxLenght * 2;  // Byte size = char count * 2
                            record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentUnicodeOffset[nParity]);

                            if (record.nRegionIndex != -1) {
                                record.nRelOffset = nCurrentUnicodeOffset[nParity] - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                            } else {
                                record.nRelOffset = nCurrentUnicodeOffset[nParity];
                            }

                            if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                                nCurrentRecords++;
                            }
                        }

                        nCurrentUnicodeSize[nParity] = 0;
                    }
                }

                // Check if string ended
                if ((!bIsUnicodeSymbol) || (bIsEnd)) {
                    if (nCurrentUnicodeSize[nParity] >= ssOptions.nMinLenght) {
                        pUnicodeBuffer[nParity][nCurrentUnicodeSize[nParity]] = 0;
                        QString sString = QString::fromUtf16(pUnicodeBuffer[nParity], nCurrentUnicodeSize[nParity]);
                        bool bAdd = true;

                        if (ssOptions.bNullTerminated && !bLongString) {
                            // For Unicode, check if followed by 0x0000 (two zero bytes)
                            // The current position is at the byte that broke the string
                            // We need to check if this byte and the previous byte form 0x0000
                            if (!bIsEnd) {
                                // Check if the Unicode character that broke the string is 0x0000
                                quint16 nBreakCode = (quint8)cPrevSymbol + ((quint8)cSymbol << 8);
                                if (nBreakCode != 0) {
                                    bAdd = false;
                                }
                            } else {
                                // At end of buffer - assume not null-terminated unless we can verify
                                bAdd = false;
                            }
                        }

                        if (bAdd) {
                            MS_RECORD record = {};
                            record.nValueType = VT_U;
                            record.nSize = nCurrentUnicodeSize[nParity] * 2;  // Byte size = char count * 2
                            record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentUnicodeOffset[nParity]);

                            if (record.nRegionIndex != -1) {
                                record.nRelOffset = nCurrentUnicodeOffset[nParity] - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                            } else {
                                record.nRelOffset = nCurrentUnicodeOffset[nParity];
                            }

                            if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                                nCurrentRecords++;
                            }
                        }
                    }

                    // Handle the other parity at end of file
                    if (bIsEnd) {
                        qint32 nOtherParity = (nParity == 1) ? 0 : 1;

                        if (nCurrentUnicodeSize[nOtherParity] >= ssOptions.nMinLenght) {
                            pUnicodeBuffer[nOtherParity][nCurrentUnicodeSize[nOtherParity]] = 0;
                            QString sString = QString::fromUtf16(pUnicodeBuffer[nOtherParity], nCurrentUnicodeSize[nOtherParity]);

                            MS_RECORD record = {};
                            record.nValueType = VT_U;
                            record.nSize = nCurrentUnicodeSize[nOtherParity] * 2;
                            record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nCurrentUnicodeOffset[nOtherParity]);

                            if (record.nRegionIndex != -1) {
                                record.nRelOffset = nCurrentUnicodeOffset[nOtherParity] - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
                            } else {
                                record.nRelOffset = nCurrentUnicodeOffset[nOtherParity];
                            }

                            if (_addMultiSearchStringRecordOptimized(&listResult, &record, sString, &ssOptions, pRegex)) {
                                nCurrentRecords++;
                            }
                        }
                    }

                    nCurrentUnicodeSize[nParity] = 0;
                }
            }

            cPrevSymbol = cSymbol;
            bIsStart = false;
        }

        _nSize -= nCurrentSize;
        _nOffset += nCurrentSize;

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, _nOffset - nOffset);

        if (nCurrentRecords >= ssOptions.nLimit) {
            setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(nCurrentRecords)));
            break;
        }
    }

    if (bReadError) {
        setPdStructErrorString(pPdStruct, tr("Read error"));
    }

    delete[] pBuffer;
    delete[] pUnicodeBuffer[0];
    delete[] pUnicodeBuffer[1];

    // Clean up pre-compiled regex
    if (pRegex) {
        delete pRegex;
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_signature(qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, quint32 nInfo, PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return multiSearch_signature(&memoryMap, nOffset, nSize, nLimit, sSignature, nInfo, pPdStruct);
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, quint32 nInfo,
                                                           PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    if (nSize == -1) {
        nSize = getSize() - nOffset;
    }

    QVector<XBinary::MS_RECORD> listResult;

    qint64 _nSize = nSize;
    qint64 _nOffset = nOffset;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint32 nCurrentRecords = 0;

    while ((_nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nSignatureSize = 0;
        qint64 nSignatureOffset = find_signature(pMemoryMap, _nOffset, _nSize, sSignature, &nSignatureSize, pPdStruct);

        if (nSignatureOffset == -1) {
            break;
        }

        MS_RECORD record = {};
        record.nValueType = VT_SIGNATURE;
        record.nSize = nSignatureSize;
        record.nInfo = nInfo;
        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nSignatureOffset);

        if (record.nRegionIndex != -1) {
            record.nRelOffset = nSignatureOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
        } else {
            record.nRelOffset = nSignatureOffset;
        }

        listResult.append(record);

        nCurrentRecords++;

        if (nCurrentRecords >= nLimit) {
            setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(nCurrentRecords)));

            break;
        }

        _nOffset = nSignatureOffset + nSignatureSize;
        _nSize = nSize - (_nOffset - nOffset);

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, _nOffset - nOffset);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_value(qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType, bool bIsBigEndian,
                                                       PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return multiSearch_value(&memoryMap, nOffset, nSize, nLimit, varValue, valueType, bIsBigEndian, pPdStruct);
}

QVector<XBinary::MS_RECORD> XBinary::multiSearch_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType,
                                                       bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    if (nSize == -1) {
        nSize = getSize() - nOffset;
    }

    // QString sValuePrefix = valueTypeToString(valueType);
    // QString sValue = getValueString(varValue, valueType);
    qint64 nValSize = getValueSize(varValue, valueType);

    QVector<XBinary::MS_RECORD> listResult;

    qint64 _nSize = nSize;
    qint64 _nOffset = nOffset;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint32 nCurrentRecords = 0;

    while ((_nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nValOffset = find_value(pMemoryMap, _nOffset, _nSize, varValue, valueType, bIsBigEndian, &nValSize, pPdStruct);

        if (nValOffset == -1) {
            break;
        }

        // QString _sValue;

        MS_RECORD record = {};
        record.nValueType = valueType;
        record.nSize = nValSize;
        record.nRegionIndex = getMemoryIndexByOffset(pMemoryMap, nValOffset);

        if (record.nRegionIndex != -1) {
            record.nRelOffset = nValOffset - pMemoryMap->listRecords.at(record.nRegionIndex).nOffset;
        } else {
            record.nRelOffset = nValOffset;
        }

        // if (valueType == VT_ANSISTRING_I) {
        //     _sValue = read_ansiString(nValOffset, nValSize);
        // } else if (valueType == VT_UNICODESTRING_I) {
        //     _sValue = read_unicodeString(nValOffset, nValSize / 2, bIsBigEndian);
        // } else if (valueType == VT_UTF8STRING_I) {
        //     _sValue = read_unicodeString(nValOffset, nValSize, bIsBigEndian);
        // } else if (valueType == VT_SIGNATURE) {
        //     _sValue = getSignature(nValOffset, nValSize);
        // } else {
        //     _sValue = sValue;
        // }

        // record.sString = QString("%1: %2").arg(sValuePrefix, _sValue);

        listResult.append(record);

        nCurrentRecords++;

        if (nCurrentRecords >= nLimit) {
            setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Maximum"), QString::number(nCurrentRecords)));

            break;
        }

        _nOffset = nValOffset + nValSize;
        _nSize = nSize - (_nOffset - nOffset);

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, _nOffset - nOffset);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    return listResult;
}

qint64 XBinary::find_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, QVariant varValue, VT valueType, bool bIsBigEndian, qint64 *pnResultSize,
                           PDSTRUCT *pPdStruct)
{
    qint64 nResult = -1;

    // TODO more mb pascal strings
    if (valueType == XBinary::VT_A) {
        nResult = find_ansiString(nOffset, nSize, varValue.toString(), pPdStruct);
    } else if (valueType == XBinary::VT_A_I) {
        nResult = find_ansiStringI(nOffset, nSize, varValue.toString(), pPdStruct);
    } else if (valueType == XBinary::VT_U) {
        nResult = find_unicodeString(nOffset, nSize, varValue.toString(), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_U_I) {
        nResult = find_unicodeStringI(nOffset, nSize, varValue.toString(), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_UTF8) {
        nResult = find_utf8String(nOffset, nSize, varValue.toString(), pPdStruct);
    } else if (valueType == XBinary::VT_UTF8_I) {
        nResult = find_utf8StringI(nOffset, nSize, varValue.toString(), pPdStruct);
    } else if (valueType == XBinary::VT_SIGNATURE) {
        nResult = find_signature(pMemoryMap, nOffset, nSize, varValue.toString(), pnResultSize, pPdStruct);
    } else if (valueType == XBinary::VT_BYTE) {
        nResult = find_uint8(nOffset, nSize, (quint8)(varValue.toULongLong()), pPdStruct);
    } else if (valueType == XBinary::VT_WORD) {
        nResult = find_uint16(nOffset, nSize, (quint16)(varValue.toULongLong()), pPdStruct);
    } else if (valueType == XBinary::VT_DWORD) {
        nResult = find_uint32(nOffset, nSize, (quint32)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_QWORD) {
        nResult = find_uint64(nOffset, nSize, (quint64)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_CHAR) {
        nResult = find_int8(nOffset, nSize, (qint8)(varValue.toULongLong()), pPdStruct);
    } else if (valueType == XBinary::VT_UCHAR) {
        nResult = find_uint8(nOffset, nSize, (quint8)(varValue.toULongLong()), pPdStruct);
    } else if (valueType == XBinary::VT_SHORT) {
        nResult = find_int16(nOffset, nSize, (qint16)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if ((valueType == XBinary::VT_USHORT) || (valueType == XBinary::VT_UINT16)) {
        nResult = find_uint16(nOffset, nSize, (quint16)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_INT) {
        nResult = find_int32(nOffset, nSize, (qint32)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if ((valueType == XBinary::VT_UINT) || (valueType == XBinary::VT_UINT32)) {
        nResult = find_uint32(nOffset, nSize, (quint32)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_INT64) {
        nResult = find_int64(nOffset, nSize, (qint64)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_UINT64) {
        nResult = find_uint64(nOffset, nSize, (quint64)(varValue.toULongLong()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_FLOAT) {
        nResult = find_float(nOffset, nSize, (float)(varValue.toFloat()), bIsBigEndian, pPdStruct);
    } else if (valueType == XBinary::VT_DOUBLE) {
        nResult = find_double(nOffset, nSize, (double)(varValue.toDouble()), bIsBigEndian, pPdStruct);
    } else {
#ifdef QT_DEBUG
        qDebug() << "Unknown valueType" << valueTypeToString(valueType, 1);
#endif
    }

    return nResult;
}

QVariant XBinary::read_value(VT valueType, qint64 nOffset, qint64 nSize, bool bIsBigEndian, PDSTRUCT *pPdStruct)
{
    QVariant varResult;

    nSize = qMin(nSize, qint64(128));

    if ((valueType == XBinary::VT_UINT8) || (valueType == XBinary::VT_BYTE)) {
        varResult = read_uint8(nOffset);
    } else if (valueType == XBinary::VT_UINT8) {
        varResult = read_int8(nOffset);
    } else if ((valueType == XBinary::VT_UINT16) || (valueType == XBinary::VT_WORD) || (valueType == XBinary::VT_USHORT)) {
        varResult = read_uint16(nOffset, bIsBigEndian);
    } else if ((valueType == XBinary::VT_INT16) || (valueType == XBinary::VT_SHORT)) {
        varResult = read_int16(nOffset, bIsBigEndian);
    } else if ((valueType == XBinary::VT_UINT32) || (valueType == XBinary::VT_UINT) || (valueType == XBinary::VT_DWORD)) {
        varResult = read_uint32(nOffset, bIsBigEndian);
    } else if ((valueType == XBinary::VT_INT32) || (valueType == XBinary::VT_INT)) {
        varResult = read_int32(nOffset, bIsBigEndian);
    } else if ((valueType == XBinary::VT_UINT64) || (valueType == XBinary::VT_QWORD)) {
        varResult = read_uint64(nOffset, bIsBigEndian);
    } else if (valueType == XBinary::VT_INT64) {
        varResult = read_int64(nOffset, bIsBigEndian);
    } else if ((valueType == XBinary::VT_A) || (valueType == XBinary::VT_A_I)) {
        varResult = read_ansiString(nOffset, nSize);
    } else if ((valueType == XBinary::VT_UTF8) || (valueType == XBinary::VT_UTF8_I)) {
        varResult = read_utf8String(nOffset, nSize);
    } else if ((valueType == XBinary::VT_U) || (valueType == XBinary::VT_U_I)) {
        varResult = read_unicodeString(nOffset, nSize, bIsBigEndian);
    } else if (valueType == XBinary::VT_PACKEDNUMBER) {
        varResult = read_packedNumber(nOffset, nSize).nValue;
    } else if (valueType == XBinary::VT_ULEB128) {
        varResult = read_uleb128(nOffset, nSize).nValue;
    } else if (valueType == XBinary::VT_BYTE_ARRAY) {
        if (nSize <= 256) {
            varResult = read_array_process(nOffset, nSize, pPdStruct);
        }
    } else if (valueType == XBinary::VT_WORD_ARRAY) {
        if (nSize <= 256) {
            varResult = read_array_process(nOffset, nSize, pPdStruct);
        }
    } else if (valueType == XBinary::VT_DWORD_ARRAY) {
        if (nSize <= 256) {
            varResult = read_array_process(nOffset, nSize, pPdStruct);  // TODO
        }
    } else if (valueType == XBinary::VT_CHAR_ARRAY) {
        if (nSize <= 256) {
            varResult = read_ansiString(nOffset, nSize);
        }
    } else {
#ifdef QT_DEBUG
        qDebug() << "Unknown valueType" << valueTypeToString(valueType, nSize);
#endif
    }

    // #ifdef QT_DEBUG
    //         qDebug() << "varResult" << varResult.toString();
    // #endif

    return varResult;
}

QString XBinary::valueTypeToString(VT valueType, qint32 nSize)
{
    QString sResult;
    VT _valueType = valueType;
    bool bArray = false;

    if ((valueType == VT_CHAR_ARRAY) || (valueType == VT_BYTE_ARRAY) || (valueType == VT_WORD_ARRAY) || (valueType == VT_DWORD_ARRAY)) {
        bArray = true;

        if (valueType == VT_CHAR_ARRAY) {
            _valueType = VT_CHAR;
        } else if (valueType == VT_BYTE_ARRAY) {
            _valueType = VT_BYTE;
        } else if (valueType == VT_WORD_ARRAY) {
            _valueType = VT_WORD;
        } else if (valueType == VT_DWORD_ARRAY) {
            _valueType = VT_DWORD;
        }
    }

    sResult = XIDSTRING_idToString((quint32)_valueType, _TABLE_XBinary_VT, sizeof(_TABLE_XBinary_VT) / sizeof(XBinary::XIDSTRING));

    if (bArray) {
        qint32 nCount = nSize / getBaseValueSize(_valueType);

        sResult += QString("[%1]").arg(nCount);
    }

    return sResult;
}

QString XBinary::getValueString(QVariant varValue, VT valueType, bool bTypesAsHex)
{
    QString sResult;

    if (valueType == XBinary::VT_A) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_A_I) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_U) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_U_I) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_UTF8) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_UTF8_I) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_SIGNATURE) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_BYTE) {
        sResult = valueToHex((quint8)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_WORD) {
        sResult = valueToHex((quint16)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_DWORD) {
        sResult = valueToHex((quint32)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_QWORD) {
        sResult = valueToHex((quint64)(varValue.toULongLong()));
    } else if ((valueType == XBinary::VT_CHAR) || (valueType == XBinary::VT_INT8)) {
        sResult = QString("%1").arg((qint8)(varValue.toLongLong()));
    } else if ((valueType == XBinary::VT_UCHAR) || (valueType == XBinary::VT_UINT8)) {
        sResult = QString("%1").arg((quint8)(varValue.toULongLong()));
    } else if ((valueType == XBinary::VT_SHORT) || (valueType == XBinary::VT_INT16)) {
        sResult = QString("%1").arg((qint16)(varValue.toLongLong()));
    } else if ((valueType == XBinary::VT_USHORT) || (valueType == XBinary::VT_UINT16)) {
        if (bTypesAsHex) {
            sResult = valueToHex((quint16)(varValue.toULongLong()));
        } else {
            sResult = QString("%1").arg((quint16)(varValue.toULongLong()));
        }
    } else if ((valueType == XBinary::VT_INT) || (valueType == XBinary::VT_INT32)) {
        sResult = QString("%1").arg((qint32)(varValue.toLongLong()));
    } else if ((valueType == XBinary::VT_UINT) || (valueType == XBinary::VT_UINT32)) {
        if (bTypesAsHex) {
            sResult = valueToHex((quint32)(varValue.toULongLong()));
        } else {
            sResult = QString("%1").arg((quint32)(varValue.toULongLong()));
        }
    } else if (valueType == XBinary::VT_INT64) {
        sResult = QString("%1").arg((qint64)(varValue.toLongLong()));
    } else if (valueType == XBinary::VT_UINT64) {
        sResult = QString("%1").arg((quint64)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_PACKEDNUMBER) {
        sResult = QString("%1").arg((quint64)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_ULEB128) {
        sResult = QString("%1").arg((quint64)(varValue.toULongLong()));
    } else if (valueType == XBinary::VT_FLOAT) {
        sResult = QString("%1").arg(varValue.toFloat());
    } else if (valueType == XBinary::VT_DOUBLE) {
        sResult = QString("%1").arg(varValue.toDouble());
    } else if (valueType == XBinary::VT_CHAR_ARRAY) {
        sResult = varValue.toString();
    } else if (valueType == XBinary::VT_BYTE_ARRAY) {
        sResult = varValue.toByteArray().toHex();
    } else {
#ifdef QT_DEBUG
        qDebug() << "Unknown valueType" << valueTypeToString(valueType, 1);
#endif
    }

    return sResult;
}

qint32 XBinary::getValueSize(QVariant varValue, VT valueType)
{
    qint32 nResult = 1;

    if (valueType == XBinary::VT_A) {
        nResult = varValue.toString().size();
    } else if (valueType == XBinary::VT_A_I) {
        nResult = varValue.toString().size();
    } else if (valueType == XBinary::VT_U) {
        nResult = varValue.toString().size() * 2;
    } else if (valueType == XBinary::VT_U_I) {
        nResult = varValue.toString().size() * 2;
    } else if (valueType == XBinary::VT_UTF8) {
        nResult = varValue.toString().toUtf8().size();
    } else if (valueType == XBinary::VT_UTF8_I) {
        nResult = varValue.toString().toUtf8().size();
    } else if (valueType == XBinary::VT_SIGNATURE) {
        QString sSignature = convertSignature(varValue.toString());
        nResult = sSignature.size() / 2;
    } else {
        nResult = getBaseValueSize(valueType);
    }

    return nResult;
}

qint32 XBinary::getBaseValueSize(VT valueType)
{
    qint32 nResult = 1;

    if ((valueType == XBinary::VT_BYTE) || (valueType == XBinary::VT_INT8) || (valueType == XBinary::VT_UINT8)) {
        nResult = 1;
    } else if (valueType == XBinary::VT_WORD) {
        nResult = 2;
    } else if (valueType == XBinary::VT_DWORD) {
        nResult = 4;
    } else if (valueType == XBinary::VT_QWORD) {
        nResult = 8;
    } else if (valueType == XBinary::VT_CHAR) {
        nResult = 1;
    } else if (valueType == XBinary::VT_UCHAR) {
        nResult = 1;
    } else if (valueType == XBinary::VT_SHORT) {
        nResult = 2;
    } else if (valueType == XBinary::VT_USHORT) {
        nResult = 2;
    } else if ((valueType == XBinary::VT_INT16) || (valueType == XBinary::VT_UINT16)) {
        nResult = 2;
    } else if (valueType == XBinary::VT_INT) {
        nResult = 4;
    } else if ((valueType == XBinary::VT_UINT) || (valueType == XBinary::VT_UINT32)) {
        nResult = 4;
    } else if ((valueType == XBinary::VT_INT64) || (valueType == XBinary::VT_UINT64)) {
        nResult = 8;
    } else if (valueType == XBinary::VT_FLOAT) {
        nResult = 4;
    } else if (valueType == XBinary::VT_DOUBLE) {
        nResult = 8;
    }

    return nResult;
}

qint32 XBinary::getValueSymbolSize(VT valueType)
{
    qint32 nResult = 1;

    if ((valueType == XBinary::VT_BYTE) || (valueType == XBinary::VT_INT8) || (valueType == XBinary::VT_UINT8) || (valueType == XBinary::VT_CHAR) ||
        (valueType == XBinary::VT_UCHAR)) {
        nResult = 2;
    } else if ((valueType == XBinary::VT_WORD) || (valueType == XBinary::VT_INT16) || (valueType == XBinary::VT_UINT16) || (valueType == XBinary::VT_SHORT) ||
               (valueType == XBinary::VT_USHORT)) {
        nResult = 4;
    } else if ((valueType == XBinary::VT_DWORD) || (valueType == XBinary::VT_INT32) || (valueType == XBinary::VT_UINT32) || (valueType == XBinary::VT_INT) ||
               (valueType == XBinary::VT_UINT)) {
        nResult = 8;
    } else if ((valueType == XBinary::VT_QWORD) || (valueType == XBinary::VT_INT64) || (valueType == XBinary::VT_UINT64)) {
        nResult = 16;
    }

    return nResult;
}

bool XBinary::isIntegerType(VT valueType)
{
    bool bResult = false;

    if ((valueType == XBinary::VT_BYTE) || (valueType == XBinary::VT_WORD) || (valueType == XBinary::VT_DWORD) || (valueType == XBinary::VT_QWORD) ||
        (valueType == XBinary::VT_CHAR) || (valueType == XBinary::VT_SHORT) || (valueType == XBinary::VT_UCHAR) || (valueType == XBinary::VT_USHORT) ||
        (valueType == XBinary::VT_INT) || (valueType == XBinary::VT_INT8) || (valueType == XBinary::VT_INT16) || (valueType == XBinary::VT_INT32) ||
        (valueType == XBinary::VT_INT64) || (valueType == XBinary::VT_UINT) || (valueType == XBinary::VT_UINT8) || (valueType == XBinary::VT_UINT16) ||
        (valueType == XBinary::VT_UINT32) || (valueType == XBinary::VT_UINT64)) {
        bResult = true;
    }

    return bResult;
}

XBinary::VT XBinary::getValueType(quint64 nValue)
{
    XBinary::VT result = VT_QWORD;

    if (nValue <= 0xFFFFFFFF) {
        result = VT_DWORD;
    } else if (nValue <= 0xFFFF) {
        result = VT_WORD;
    } else if (nValue <= 0xFF) {
        result = VT_BYTE;
    }

    return result;
}

QByteArray XBinary::getUnicodeString(const QString &sString, bool bIsBigEndian)
{
    QByteArray baResult;

    qint32 nSize = sString.size() * 2;

    baResult.resize(nSize);

    baResult.fill(0);

    char *pData = baResult.data();

    _copyMemory(pData, (char *)sString.utf16(), nSize);

    for (qint32 i = 0; i < nSize; i++) {
        if ((i % 2) == 0) {
            quint16 nValue = *(quint16 *)(pData + i);

            if (bIsBigEndian) {
                nValue = qFromBigEndian(nValue);
            } else {
                nValue = qFromLittleEndian(nValue);
            }

            *(quint16 *)(pData + i) = nValue;
        }
    }

    return baResult;
}

QByteArray XBinary::getStringData(VT valueType, const QString &sString, bool bAddNull)
{
    QByteArray baResult;

    qint32 nSize = sString.size();

    char buffer[4] = {};

    if (valueType == VT_A) {
        baResult = sString.toLatin1();

        if (bAddNull) {
            baResult.append(buffer, 1);
        }
    } else if (valueType == VT_U) {
        baResult.resize(nSize * 2);

        baResult.fill(0);

        QByteArray baString = getUnicodeString(sString, false);  // mb TODO Endian

        _copyMemory(baResult.data(), baString.data(), baString.size());

        if (bAddNull) {
            baResult.append(buffer, 2);
        }
    } else if (valueType == VT_UTF8) {
        baResult = sString.toUtf8();

        if (bAddNull) {
            baResult.append(buffer, 1);
        }
    }

    return baResult;
}

bool XBinary::isSignaturePresent(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, PDSTRUCT *pPdStruct)
{
    qint64 nResultSize = 0;

    return (find_signature(pMemoryMap, nOffset, nSize, sSignature, &nResultSize, pPdStruct) != -1);
}

bool XBinary::isSignatureValid(const QString &sSignature, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (sSignature.size()) {
        QString _sSignature = convertSignature(sSignature);

        bResult = true;

        QList<SIGNATURE_RECORD> listSignatureRecords = getSignatureRecords(_sSignature, &bResult, pPdStruct);
    }

    return bResult;
}

bool XBinary::createFile(const QString &sFileName, qint64 nFileSize)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        bResult = true;

        if (nFileSize) {
            bResult = file.resize(nFileSize);
        }

        file.close();
    }

    return bResult;
}

bool XBinary::isFileExists(const QString &sFileName, bool bTryToOpen)
{
    bool bResult = false;

    QFileInfo fi(sFileName);

    bResult = ((fi.exists() && fi.isFile()));

    if (bResult && bTryToOpen) {
        QFile file(sFileName);

        bResult = tryToOpen(&file);

        file.close();
    }

    return bResult;
}

bool XBinary::removeFile(const QString &sFileName)
{
#ifdef Q_OS_WIN
    QFile::Permissions perm = QFile::permissions(sFileName);

    if (!(perm & QFile::WriteOther)) {
        QFile::setPermissions(sFileName, (QFile::Permissions)0x6666);
    }
#endif

    return QFile::remove(sFileName);
}

bool XBinary::copyFile(const QString &sSrcFileName, const QString &sDestFileName)
{
    // mb TODO remove first
    return QFile::copy(sSrcFileName, sDestFileName);
}

bool XBinary::moveFile(const QString &sSrcFileName, const QString &sDestFileName)
{
    bool bResult = false;

    if (copyFile(sSrcFileName, sDestFileName)) {
        bResult = removeFile(sSrcFileName);

        if (!bResult) {
            removeFile(sDestFileName);
        }
    }

    return bResult;
}

bool XBinary::moveFileToDirectory(const QString &sSrcFileName, const QString &sDestDirectory)
{
    QFileInfo fi(sSrcFileName);

    return moveFile(sSrcFileName, sDestDirectory + QDir::separator() + fi.fileName());
}

QString XBinary::convertFileNameSymbols(const QString &sFileName)
{
    QString sResult = sFileName;

    sResult = sResult.replace("/", "_");
    sResult = sResult.replace("\\", "_");
    sResult = sResult.replace("?", "_");
    sResult = sResult.replace("*", "_");
    sResult = sResult.replace("\"", "_");
    sResult = sResult.replace("<", "_");
    sResult = sResult.replace(">", "_");
    sResult = sResult.replace("|", "_");
    sResult = sResult.replace(":", "_");
    sResult = sResult.replace("\n", "_");
    sResult = sResult.replace("\r", "_");
    sResult = sResult.replace("{", "_");
    sResult = sResult.replace("}", "_");

    return sResult;
}

QString XBinary::fixFileName(const QString &sFileName)
{
    QString sResult;

    qint32 nLength = sFileName.length();

    for (qint32 i = 0; i < nLength; i++) {
        QChar ch = sFileName.at(i);
        ushort nCode = ch.unicode();

        // Replace control characters
        if (nCode < 0x20) {
            sResult.append(QLatin1Char('_'));
            continue;
        }

#ifdef Q_OS_WIN
        // Windows: replace characters not allowed in filenames
        // (except / and \ which are path separators — keep them)
        if ((ch == QLatin1Char('?')) || (ch == QLatin1Char('*')) || (ch == QLatin1Char('"')) || (ch == QLatin1Char('<')) || (ch == QLatin1Char('>')) ||
            (ch == QLatin1Char('|')) || (ch == QLatin1Char(':'))) {
            sResult.append(QLatin1Char('_'));
            continue;
        }

        // Replace characters that Windows filesystem cannot represent
        // (surrogate halves, certain Unicode chars that NTFS rejects)
        if (nCode >= 0xD800 && nCode <= 0xDFFF) {
            sResult.append(QLatin1Char('_'));
            continue;
        }

        // Replace non-ASCII characters that may fail on Windows (code page issues)
        // Keep common Latin-1 supplement (0x00A0-0x00FF) and general letters
        // Replace characters outside BMP or unusual Unicode that causes filesystem errors
        if (nCode > 0xFFFD) {
            sResult.append(QLatin1Char('_'));
            continue;
        }
#endif

        sResult.append(ch);
    }

    // Normalize path separators to /
    sResult = sResult.replace(QLatin1Char('\\'), QLatin1Char('/'));

    // Remove trailing dots and spaces from each path component (Windows restriction)
#ifdef Q_OS_WIN
    QStringList listParts = sResult.split(QLatin1Char('/'));
    QStringList listFixed;

    for (qint32 i = 0; i < listParts.count(); i++) {
        QString sPart = listParts.at(i);

        // Remove trailing dots and spaces
        while (sPart.endsWith(QLatin1Char('.')) || sPart.endsWith(QLatin1Char(' '))) {
            sPart.chop(1);
        }

        // If part became empty, use underscore
        if (sPart.isEmpty() && !listParts.at(i).isEmpty()) {
            sPart = QLatin1String("_");
        }

        listFixed.append(sPart);
    }

    sResult = listFixed.join(QLatin1Char('/'));
#endif

    // If the entire result is empty, return underscore
    if (sResult.isEmpty() && !sFileName.isEmpty()) {
        sResult = QLatin1String("_");
    }

    return sResult;
}

QString XBinary::getBaseFileName(const QString &sFileName)
{
    QFileInfo fi(sFileName);

    return fi.baseName();
}

bool XBinary::createDirectory(const QString &sDirectoryName)
{
    return QDir().mkpath(sDirectoryName);
}

bool XBinary::isDirectoryExists(const QString &sDirectoryName)
{
    QFileInfo fi(sDirectoryName);

    return (fi.exists() && fi.isDir());
}

bool XBinary::removeDirectory(const QString &sDirectoryName)
{
    bool bResult = false;

    QDir dir(sDirectoryName);

#if (QT_VERSION_MAJOR > 4)
    bResult = dir.removeRecursively();
#else
    bResult = dir.remove(sDirectoryName);
#endif

    return bResult;
}

bool XBinary::isDirectoryEmpty(const QString &sDirectoryName)
{
    return (QDir(sDirectoryName).entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries).count() == 0);
}

QDateTime XBinary::getDirectoryLatestModificationDate(const QString &sDirectoryName)
{
    QDateTime latestMod;
    QDir dir(sDirectoryName);

    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    qint32 nNumberOfEntries = entries.size();

    for (qint32 i = 0; i < nNumberOfEntries; i++) {
        const QFileInfo &entry = entries.at(i);
        if (entry.isDir()) {
            QDateTime subDirMod = getDirectoryLatestModificationDate(entry.absoluteFilePath());
            if (subDirMod.isValid() && subDirMod > latestMod) latestMod = subDirMod;
        } else if (entry.isFile()) {
            QDateTime fileMod = entry.lastModified();
            if (fileMod > latestMod) latestMod = fileMod;
        }
    }
    return latestMod;
}

bool XBinary::createEmptyFile(const QString &sFileName)
{
    return createFile(sFileName, 0);
}

QByteArray XBinary::readFile(const QString &sFileName, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    // TODO PDSTRUCT

    QByteArray baResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        baResult = file.readAll();

        file.close();
    }

    return baResult;
}

bool XBinary::readFile(const QString &sFileName, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);
        qint64 nOffset = 0;

        bResult = true;
        qint32 nBufferSize = getBufferSize(pPdStruct);

        while ((nSize > 0) && !(pPdStruct->bIsStop)) {
            qint64 nCurrentSize = qMin(nSize, (qint64)nBufferSize);

            if (nCurrentSize != file.read(pBuffer, nCurrentSize)) {
                bResult = false;
                break;
            }

            nSize -= nCurrentSize;
            pBuffer += nCurrentSize;
            nOffset += nCurrentSize;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset);
        }

        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

        file.close();
    }

    return bResult;
}

void XBinary::_copyMemory(char *pDest, const char *pSource, qint64 nSize)
{
    while (nSize > 0) {
        if (nSize >= 8) {
            *((quint64 *)pDest) = *((quint64 *)pSource);
            pDest += 8;
            pSource += 8;
            nSize -= 8;
        } else if (nSize >= 4) {
            *((quint32 *)pDest) = *((quint32 *)pSource);
            pDest += 4;
            pSource += 4;
            nSize -= 4;
        } else if (nSize >= 2) {
            *((quint16 *)pDest) = *((quint16 *)pSource);
            pDest += 2;
            pSource += 2;
            nSize -= 2;
        } else {
            *(pDest) = *(pSource);
            pDest++;
            pSource++;
            nSize--;
        }
    }
}

void XBinary::_zeroMemory(char *pDest, qint64 nSize)
{
    // TODO optimize
    while (nSize) {
        *pDest = 0;
        pDest++;
        nSize--;
    }
}

bool XBinary::_isMemoryZeroFilled(char *pSource, qint64 nSize)
{
    // Check in 8-byte chunks for better performance
    while (nSize >= 8) {
        if (*((quint64 *)pSource) != 0) {
            return false;
        }
        pSource += 8;
        nSize -= 8;
    }

    // Check remaining bytes one by one
    while (nSize) {
        if (*pSource) {
            return false;
        }
        pSource++;
        nSize--;
    }

    return true;
}

bool XBinary::_isMemoryNotNull(char *pSource, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_is_not_null(pSource, nSize) != 0;
#else
    // Fallback: 64-bit processing
    const char *ptr = pSource;
    const quint64 *__restrict pSource64 = reinterpret_cast<const quint64 *>(ptr);
    while (nSize >= 8) {
        quint64 v = *pSource64;
        if (((v - 0x0101010101010101ULL) & ~v & 0x8080808080808080ULL) != 0) {
            return false;
        }
        pSource64++;
        nSize -= 8;
    }

    // Process remaining bytes
    const char *pRemaining = reinterpret_cast<const char *>(pSource64);
    while (nSize > 0) {
        if (*pRemaining == 0) {
            return false;
        }
        pRemaining++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::_isMemoryAnsi(char *pSource, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_is_ansi(pSource, nSize) != 0;
#else
    // Fallback: 64-bit processing
    const quint64 *__restrict pSource64 = reinterpret_cast<const quint64 *>(pSource);
    while (nSize >= 8) {
        quint64 v = *pSource64;
        if ((((v - 0x2020202020202020ULL) | v) & 0x8080808080808080ULL) != 0) {
            const quint8 *pCheck = reinterpret_cast<const quint8 *>(pSource64);
            for (qint32 i = 0; i < 8; i++) {
                if (pCheck[i] < 0x20 || pCheck[i] >= 0x80) {
                    return false;
                }
            }
        }
        pSource64++;
        nSize -= 8;
    }

    // Process remaining bytes
    const quint8 *pRemaining = reinterpret_cast<const quint8 *>(pSource64);
    while (nSize > 0) {
        if (*pRemaining < 0x20 || *pRemaining >= 0x80) {
            return false;
        }
        pRemaining++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::_isMemoryNotAnsi(char *pSource, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_is_not_ansi(pSource, nSize) != 0;
#else
    // Fallback: 64-bit processing
    const quint64 *__restrict pSource64 = reinterpret_cast<const quint64 *>(pSource);
    while (nSize >= 8) {
        quint64 v = *pSource64;
        if ((((v - 0x2020202020202020ULL) | v) & 0x8080808080808080ULL) == 0) {
            const quint8 *pCheck = reinterpret_cast<const quint8 *>(pSource64);
            for (qint32 i = 0; i < 8; i++) {
                if (pCheck[i] >= 0x20 && pCheck[i] < 0x80) {
                    return false;
                }
            }
        }
        pSource64++;
        nSize -= 8;
    }

    // Process remaining bytes
    const quint8 *pRemaining = reinterpret_cast<const quint8 *>(pSource64);
    while (nSize > 0) {
        if (*pRemaining >= 0x20 && *pRemaining < 0x80) {
            return false;
        }
        pRemaining++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::_isMemoryNotAnsiAndNull(char *pSource, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_is_not_ansi_and_null(pSource, nSize) ? true : false;
#else
    // Fallback: 64-bit processing
    const quint64 *__restrict pSource64 = reinterpret_cast<const quint64 *>(pSource);
    while (nSize >= 8) {
        quint64 v = *pSource64;
        if (((v - 0x0101010101010101ULL) & ~v & 0x8080808080808080ULL) != 0) {
            return false;
        }
        if ((((v - 0x2020202020202020ULL) | v) & 0x8080808080808080ULL) == 0) {
            const quint8 *pCheck = reinterpret_cast<const quint8 *>(pSource64);
            for (qint32 i = 0; i < 8; i++) {
                if (pCheck[i] >= 0x20 && pCheck[i] < 0x80) {
                    return false;
                }
            }
        }
        pSource64++;
        nSize -= 8;
    }

    // Process remaining bytes
    const quint8 *pRemaining = reinterpret_cast<const quint8 *>(pSource64);
    while (nSize > 0) {
        if (*pRemaining == 0 || (*pRemaining >= 0x20 && *pRemaining < 0x80)) {
            return false;
        }
        pRemaining++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::_isMemoryAnsiNumber(char *pSource, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_is_ansi_number(pSource, nSize) ? true : false;
#else
    // Fallback: 64-bit processing
    const quint64 *__restrict pSource64 = reinterpret_cast<const quint64 *>(pSource);
    while (nSize >= 8) {
        quint64 v = *pSource64;
        quint64 v_minus_0x30 = v - 0x3030303030303030ULL;
        if (((v_minus_0x30 | (v_minus_0x30 >> 4)) & 0xF0F0F0F0F0F0F0F0ULL) != 0) {
            const quint8 *pCheck = reinterpret_cast<const quint8 *>(pSource64);
            for (qint32 i = 0; i < 8; i++) {
                if (pCheck[i] < 0x30 || pCheck[i] > 0x39) {
                    return false;
                }
            }
        }
        pSource64++;
        nSize -= 8;
    }

    // Process remaining bytes
    const quint8 *pRemaining = reinterpret_cast<const quint8 *>(pSource64);
    while (nSize > 0) {
        if (*pRemaining < 0x30 || *pRemaining > 0x39) {
            return false;
        }
        pRemaining++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::copyDeviceMemory(QIODevice *pSourceDevice, qint64 nSourceOffset, QIODevice *pDestDevice, qint64 nDestOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    // TODO optimize
    if ((!pSourceDevice->seek(nSourceOffset)) || (!pDestDevice->seek(nDestOffset))) {
        return false;
    }

    qint32 nBufferSize = getBufferSize(pPdStruct);

    char *pBuffer = new char[nBufferSize];

    while ((nSize > 0) && (isPdStructNotCanceled(pPdStruct))) {
        qint64 nCurrentBufferSize = qMin(nSize, (qint64)nBufferSize);

        if (nCurrentBufferSize != pSourceDevice->read(pBuffer, nCurrentBufferSize)) {
            break;
        }

        if (nCurrentBufferSize != pDestDevice->write(pBuffer, nCurrentBufferSize)) {
            break;
        }

        nSize -= nCurrentBufferSize;
    }

    delete[] pBuffer;

    return (bool)(nSize == 0);
}

bool XBinary::copyMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize, quint32 nBufferSize, bool bReverse)
{
    bool bResult = false;

    // TODO optimize
    if (nBufferSize == 0) {
        return false;
    }

    if (nBufferSize >= 0x1000) {
        nBufferSize = 0x1000;
    }

    if (nDestOffset == nSourceOffset) {
        return true;
    }

    qint64 nMaxSize = getSize();

    if ((nDestOffset + nSize > nMaxSize) || (nSourceOffset + nSize > nMaxSize)) {
        return false;
    }

    // TODO
    char *pBuffer = new char[nBufferSize];

    if (bReverse) {
        nSourceOffset += nSize;
        nDestOffset += nSize;
    }

    bResult = true;

    while (nSize > 0) {
        qint64 nTempSize = qMin(nSize, (qint64)nBufferSize);

        if (bReverse) {
            nSourceOffset -= nTempSize;
            nDestOffset -= nTempSize;
        }

        read_array(nSourceOffset, pBuffer, nTempSize);
        write_array(nDestOffset, pBuffer, nTempSize);

        if (!bReverse) {
            nSourceOffset += nTempSize;
            nDestOffset += nTempSize;
        }

        nSize -= nTempSize;
    }

    delete[] pBuffer;

    return bResult;
}

bool XBinary::zeroFill(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    // TODO optimize
    if (nSize == 0) {
        return false;
    }

    if ((nOffset < 0) || (nSize < 0)) {
        return false;
    }

    qint64 nMaxSize = getSize();

    // Prevent overflow and out-of-bounds
    if ((nOffset > nMaxSize) || (nSize > (nMaxSize - nOffset))) {
        return false;
    }

    qint32 nBufferSize = getBufferSize(pPdStruct);
    char *pZero = new char[nBufferSize];

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

    qint64 nProcessed = 0;
    bool bSuccess = true;

    while ((nSize > 0) && isPdStructNotCanceled(pPdStruct)) {
        qint64 nWrite = qMin(nSize, (qint64)nBufferSize);

        if (write_array(nOffset, pZero, nWrite) != nWrite) {
            bSuccess = false;
            break;
        }

        nOffset += nWrite;
        nSize -= nWrite;
        nProcessed += nWrite;

        XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nProcessed);
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    delete[] pZero;

    return bSuccess && (nSize == 0);
}

bool XBinary::compareMemory(char *pMemory1, const char *pMemory2, qint64 nSize)
{
#ifdef USE_XSIMD
    return xsimd_compare_memory(pMemory1, pMemory2, nSize) != 0;
#else
    const char *__restrict ptr1 = pMemory1;
    const char *__restrict ptr2 = pMemory2;

    // 64-bit comparison
    const quint64 *__restrict p1_64 = reinterpret_cast<const quint64 *>(ptr1);
    const quint64 *__restrict p2_64 = reinterpret_cast<const quint64 *>(ptr2);

    while (nSize >= 8) {
        if (*p1_64 != *p2_64) {
            return false;
        }
        p1_64++;
        p2_64++;
        nSize -= 8;
    }

    // Process remaining 0-7 bytes
    ptr1 = reinterpret_cast<const char *>(p1_64);
    ptr2 = reinterpret_cast<const char *>(p2_64);

    while (nSize > 0) {
        if (*ptr1 != *ptr2) {
            return false;
        }
        ptr1++;
        ptr2++;
        nSize--;
    }

    return true;
#endif
}

bool XBinary::compareMemoryByteI(quint8 *pMemory, const quint8 *pMemoryU, const quint8 *pMemoryL, qint64 nSize)
{
    bool bResult = true;

    while (nSize > 0) {
        if ((*(pMemory) != *(pMemoryU)) && (*(pMemory) != *(pMemoryL))) {
            bResult = false;
            break;
        }

        pMemory++;
        pMemoryU++;
        pMemoryL++;
        nSize--;
    }

    return bResult;
}

bool XBinary::compareMemoryWordI(quint16 *pMemory, const quint16 *pMemoryU, const quint16 *pMemoryL, qint64 nSize)
{
    bool bResult = true;

    while (nSize > 0) {
        if ((*(pMemory) != *(pMemoryU)) && (*(pMemory) != *(pMemoryL))) {
            bResult = false;
            break;
        }

        pMemory++;
        pMemoryU++;
        pMemoryL++;
        nSize--;
    }

    return bResult;
}

bool XBinary::isOffsetValid(qint64 nOffset, qint64 nTotalSize)
{
    return (nOffset >= 0) && (nOffset < nTotalSize);
}

bool XBinary::isOffsetValid(qint64 nOffset)
{
    return isOffsetValid(nOffset, getSize());
}

bool XBinary::isAddressValid(XADDR nAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return isAddressValid(&memoryMap, nAddress);
}

bool XBinary::isRelAddressValid(qint64 nRelAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return isRelAddressValid(&memoryMap, nRelAddress);
}

XADDR XBinary::offsetToAddress(qint64 nOffset)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return offsetToAddress(&memoryMap, nOffset);
}

qint64 XBinary::addressToOffset(quint64 nAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();
    return addressToOffset(&memoryMap, nAddress);
}

XADDR XBinary::offsetToRelAddress(qint64 nOffset)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return offsetToRelAddress(&memoryMap, nOffset);
}

qint64 XBinary::relAddressToOffset(qint64 nRelAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return relAddressToOffset(&memoryMap, nRelAddress);
}

bool XBinary::isEOD(_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    return (pMemoryMap->nBinarySize == nOffset);
}

bool XBinary::isOffsetValid(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    bool bResult = false;

    if (pMemoryMap->nBinarySize) {
        bResult = ((nOffset >= 0) && (nOffset < pMemoryMap->nBinarySize));
    } else {
        qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1)) {
                if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
                    bResult = true;
                    break;
                }
            }
        }
    }

    return bResult;
}

bool XBinary::isOffsetAndSizeValid(XBinary::_MEMORY_MAP *pMemoryMap, XBinary::OFFSETSIZE *pOsRegion)
{
    return isOffsetAndSizeValid(pMemoryMap, pOsRegion->nOffset, pOsRegion->nSize);
}

bool XBinary::isOffsetAndSizeValid(qint64 nOffset, qint64 nSize)
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return isOffsetAndSizeValid(&memoryMap, nOffset, nSize);
}

bool XBinary::isOffsetAndSizeValid(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize)
{
    bool bResult = false;

    if (nSize > 0) {
        bool bValidOffset = isOffsetValid(pMemoryMap, nOffset);
        bool bValidSize = isOffsetValid(pMemoryMap, nOffset + nSize - 1);

        bResult = bValidOffset && bValidSize;
    }

    return bResult;
}

bool XBinary::isOffsetAndSizeValid(QIODevice *pDevice, qint64 nOffset, qint64 nSize)
{
    XBinary binary(pDevice);

    _MEMORY_MAP memoryMap = binary.getMemoryMap();

    return isOffsetAndSizeValid(&memoryMap, nOffset, nSize);
}

bool XBinary::isAddressValid(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    bool bResult = false;

    if (pMemoryMap->nImageSize) {
        bResult = ((pMemoryMap->nModuleAddress <= nAddress) && (nAddress < (pMemoryMap->nModuleAddress + pMemoryMap->nImageSize)));
    } else {
        qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1)) {
                if ((pMemoryMap->listRecords.at(i).nAddress <= nAddress) && (nAddress < pMemoryMap->listRecords.at(i).nAddress + pMemoryMap->listRecords.at(i).nSize)) {
                    bResult = true;
                    break;
                }
            }
        }
    }

    return bResult;
}

bool XBinary::isRelAddressValid(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    return isAddressValid(pMemoryMap, pMemoryMap->nModuleAddress + nRelAddress);
}

bool XBinary::isAddressPhysical(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    qint64 nOffset = addressToOffset(pMemoryMap, nAddress);

    return (nOffset != -1);
}

bool XBinary::isRelAddressPhysical(_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    qint64 nOffset = relAddressToOffset(pMemoryMap, nAddress);

    return (nOffset != -1);
}

XADDR XBinary::offsetToAddress(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    XADDR nResult = -1;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    //    for (qint32 i = 0; i < nNumberOfRecords; i++) {
    //        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1) && (pMemoryMap->listRecords.at(i).nAddress != -1)) {
    //            if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
    //                nResult = (nOffset - pMemoryMap->listRecords.at(i).nOffset) + pMemoryMap->listRecords.at(i).nAddress;
    //                break;
    //            }
    //        }
    //    }

    // From the last to the fist

    for (qint32 i = nNumberOfRecords - 1; i >= 0; i--) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1) && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1)) {
            if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
                nResult = (nOffset - pMemoryMap->listRecords.at(i).nOffset) + pMemoryMap->listRecords.at(i).nAddress;
                break;
            }
        }
    }

    return nResult;
}

qint64 XBinary::addressToOffset(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    qint64 nResult = -1;

    //    if(pMemoryMap->mode==MODE_16) // Check COM Check 16SEG
    //    {
    //        if(nAddress>0xFFFF)
    //        {
    //            nAddress=((nAddress>>16)&0xFFFF)*16+(nAddress&0xFFFF);
    //        }
    //    }

    // if (pMemoryMap->fileType == FT_MSDOS) {
    //     qint64 _nResult = ((nAddress >> 16) & 0xFFFF) * 16 + (nAddress & 0xFFFF);

    //     if (_nResult >= 0x10000000) {
    //         _nResult -= 0x10000000;
    //     }

    //     if (_nResult == 0x100000) {
    //         _nResult = 0;
    //     }

    //     nResult = _nResult + pMemoryMap->nSegmentBase;

    //     if (nResult > pMemoryMap->nBinarySize) {
    //         nResult = -1;
    //     }
    // } else {
    //     qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    //     for (qint32 i = 0; i < nNumberOfRecords; i++) {
    //         if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1) && (pMemoryMap->listRecords.at(i).nOffset != -1)) {
    //             if ((pMemoryMap->listRecords.at(i).nAddress <= nAddress) && (nAddress < pMemoryMap->listRecords.at(i).nAddress + pMemoryMap->listRecords.at(i).nSize))
    //             {
    //                 nResult = (nAddress - pMemoryMap->listRecords.at(i).nAddress) + pMemoryMap->listRecords.at(i).nOffset;
    //                 break;
    //             }
    //         }
    //     }
    // }

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = nNumberOfRecords - 1; i >= 0; i--) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1) && (pMemoryMap->listRecords.at(i).nOffset != -1)) {
            if ((pMemoryMap->listRecords.at(i).nAddress <= nAddress) && (nAddress < pMemoryMap->listRecords.at(i).nAddress + pMemoryMap->listRecords.at(i).nSize)) {
                nResult = (nAddress - pMemoryMap->listRecords.at(i).nAddress) + pMemoryMap->listRecords.at(i).nOffset;
                break;
            }
        }
    }

    return nResult;
}

XADDR XBinary::offsetToRelAddress(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    XADDR nResult = offsetToAddress(pMemoryMap, nOffset);

    if (nResult != -1) {
        nResult -= pMemoryMap->nModuleAddress;
    }

    return nResult;
}

qint64 XBinary::relAddressToOffset(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    return addressToOffset(pMemoryMap, nRelAddress + pMemoryMap->nModuleAddress);
}

XADDR XBinary::relAddressToAddress(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    XADDR nResult = -1;

    if (isRelAddressValid(pMemoryMap, nRelAddress)) {
        nResult = nRelAddress + pMemoryMap->nModuleAddress;
    }

    return nResult;
}

qint64 XBinary::addressToRelAddress(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    qint64 nResult = -1;

    if (isAddressValid(pMemoryMap, nAddress)) {
        nResult = nAddress - pMemoryMap->nModuleAddress;
    }

    return nResult;
}

XADDR XBinary::segmentRelOffsetToAddress(_MEMORY_MAP *pMemoryMap, quint16 nSegment, XADDR nRelOffset)
{
    XADDR nResult = -1;

    if (nSegment < pMemoryMap->listRecords.count()) {
        nResult = pMemoryMap->listRecords.at(nSegment).nAddress + nRelOffset;
    }

    return nResult;
}

qint64 XBinary::locationToOffset(_MEMORY_MAP *pMemoryMap, LT locType, XADDR nLocation)
{
    qint64 nResult = -1;

    if (locType == LT_ADDRESS) {
        nResult = addressToOffset(pMemoryMap, nLocation);
    } else if (locType == LT_RELADDRESS) {
        nResult = relAddressToOffset(pMemoryMap, nLocation);
    } else if (locType == LT_OFFSET) {
        nResult = nLocation;
    }

    return nResult;
}

qint64 XBinary::locToOffset(_MEMORY_MAP *pMemoryMap, const XLOC &xLoc)
{
    return locationToOffset(pMemoryMap, xLoc.locType, xLoc.nLocation);
}

XBinary::XLOC XBinary::offsetToLoc(qint64 nOffset)
{
    XLOC result = {};
    result.locType = LT_OFFSET;
    result.nLocation = nOffset;

    return result;
}

XADDR XBinary::getSegmentAddress(quint16 nSegment, quint16 nAddress)
{
    XADDR nResult = nSegment * 16 + nAddress;

    if (nResult >= 0x100000) {
        nResult -= 0x100000;
    }

    return nResult;
}

XBinary::_MEMORY_RECORD XBinary::getMemoryRecordByIndex(_MEMORY_MAP *pMemoryMap, qint32 nIndex)
{
    _MEMORY_RECORD result = {};

    if ((nIndex >= 0) && (nIndex < pMemoryMap->listRecords.count())) {
        result = pMemoryMap->listRecords.at(nIndex);
    }

    return result;
}

qint32 XBinary::getMemoryIndexByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    qint32 nResult = -1;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1)) {
            if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
                nResult = i;
                break;
            }
        }
    }

    return nResult;
}

XBinary::_MEMORY_RECORD XBinary::getMemoryRecordByOffset(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    _MEMORY_RECORD result = {};

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1)) {
            if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
                result = pMemoryMap->listRecords.at(i);
                break;
            }
        }
    }

    return result;
}

XBinary::_MEMORY_RECORD XBinary::getMemoryRecordByAddress(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    _MEMORY_RECORD result = {};

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1)) {
            if ((pMemoryMap->listRecords.at(i).nAddress <= nAddress) && (nAddress < pMemoryMap->listRecords.at(i).nAddress + pMemoryMap->listRecords.at(i).nSize)) {
                result = pMemoryMap->listRecords.at(i);
                break;
            }
        }
    }

    return result;
}

XBinary::_MEMORY_RECORD XBinary::getMemoryRecordByRelAddress(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    _MEMORY_RECORD result = {};

    XADDR nAddress = relAddressToAddress(pMemoryMap, nRelAddress);

    if (nAddress != (XADDR)-1) {
        result = getMemoryRecordByAddress(pMemoryMap, nAddress);
    }

    return result;
}

qint32 XBinary::addressToFileTypeNumber(_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    _MEMORY_RECORD mm = getMemoryRecordByAddress(pMemoryMap, nAddress);

    return mm.nFilePartNumber;
    ;
}

qint32 XBinary::relAddressToFileTypeNumber(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    _MEMORY_RECORD mm = getMemoryRecordByRelAddress(pMemoryMap, nRelAddress);

    return mm.nFilePartNumber;
}

bool XBinary::isAddressInHeader(_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    bool bResult = false;

    _MEMORY_RECORD mm = getMemoryRecordByAddress(pMemoryMap, nAddress);

    if (mm.filePart == FILEPART_HEADER) {
        bResult = true;
    }

    return bResult;
}

bool XBinary::isRelAddressInHeader(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    bool bResult = false;

    _MEMORY_RECORD mm = getMemoryRecordByRelAddress(pMemoryMap, nRelAddress);

    if (mm.filePart == FILEPART_HEADER) {
        bResult = true;
    }

    return bResult;
}

QString XBinary::getLoadSectionNameByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    QString sResult;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nSize && (pMemoryMap->listRecords.at(i).nOffset != -1) && (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1)) {
            if ((pMemoryMap->listRecords.at(i).nOffset <= nOffset) && (nOffset < pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize)) {
                sResult = pMemoryMap->listRecords.at(i).sName;
                break;
            }
        }
    }

    return sResult;
}

bool XBinary::isSolidAddressRange(XBinary::_MEMORY_MAP *pMemoryMap, quint64 nAddress, qint64 nSize)
{
    bool bResult = false;

    qint32 nIndex1 = getMemoryRecordByAddress(pMemoryMap, nAddress).nIndex;
    qint32 nIndex2 = getMemoryRecordByAddress(pMemoryMap, nAddress + nSize - 1).nIndex;

    bResult = (nIndex1 == nIndex2);

    return bResult;
}

QString XBinary::getMemoryRecordInfoByOffset(qint64 nOffset)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return getMemoryRecordInfoByOffset(&memoryMap, nOffset);
}

QString XBinary::getMemoryRecordInfoByAddress(XADDR nAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return getMemoryRecordInfoByAddress(&memoryMap, nAddress);
}

QString XBinary::getMemoryRecordInfoByRelAddress(qint64 nRelAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return getMemoryRecordInfoByRelAddress(&memoryMap, nRelAddress);
}

QString XBinary::getMemoryRecordInfoByOffset(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nOffset)
{
    XBinary::_MEMORY_RECORD memoryRecord = getMemoryRecordByOffset(pMemoryMap, nOffset);

    return getMemoryRecordName(&memoryRecord);
}

QString XBinary::getMemoryRecordInfoByAddress(XBinary::_MEMORY_MAP *pMemoryMap, XADDR nAddress)
{
    XBinary::_MEMORY_RECORD memoryRecord = getMemoryRecordByAddress(pMemoryMap, nAddress);

    return getMemoryRecordName(&memoryRecord);
}

QString XBinary::getMemoryRecordInfoByRelAddress(XBinary::_MEMORY_MAP *pMemoryMap, qint64 nRelAddress)
{
    XBinary::_MEMORY_RECORD memoryRecord = getMemoryRecordByAddress(pMemoryMap, nRelAddress + pMemoryMap->nModuleAddress);

    return getMemoryRecordName(&memoryRecord);
}

QString XBinary::getMemoryRecordName(XBinary::_MEMORY_RECORD *pMemoryRecord)
{
    QString sRecord;

    sRecord = pMemoryRecord->sName;  // TODO

    return sRecord;
}

QString XBinary::mapModeToString(MAPMODE mapMode)
{
    QString sResult = tr("Unknown");

    switch (mapMode) {
        case MAPMODE_UNKNOWN: sResult = tr("Unknown"); break;
        case MAPMODE_REGIONS: sResult = tr("Regions"); break;
        case MAPMODE_SEGMENTS: sResult = tr("Segments"); break;
        case MAPMODE_SECTIONS: sResult = tr("Sections"); break;
        case MAPMODE_OBJECTS: sResult = tr("Objects"); break;
        case MAPMODE_MAPS: sResult = tr("Maps"); break;
        case MAPMODE_DATA: sResult = tr("Data"); break;
        case MAPMODE_STREAMS: sResult = tr("Streams"); break;
    }

    return sResult;
}

bool XBinary::_initMemoryMap(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    qint64 nTotalSize = getSize();

    pMemoryMap->nModuleAddress = getModuleAddress();
    pMemoryMap->bIsImage = isImage();
    pMemoryMap->nBinarySize = nTotalSize;
    pMemoryMap->nImageSize = getImageSize();
    pMemoryMap->fileType = getFileType();
    pMemoryMap->mode = getMode();
    pMemoryMap->sArch = getArch();
    pMemoryMap->endian = getEndian();
    pMemoryMap->sType = getTypeAsString();
    pMemoryMap->nEntryPointAddress = _getEntryPointAddress();

    return true;
}

void XBinary::_processMemoryMap(_MEMORY_MAP *pMemoryMap, QList<FPART> *pListFParts, PDSTRUCT *pPdStruct)
{
    std::sort(pListFParts->begin(), pListFParts->end(), compareFileParts);

    XADDR nMaxAddress = 0;
    XADDR nMinAddress = -1;
    qint32 nNumberOfParts = pListFParts->count();
    qint32 nIndex = 0;

    for (qint32 i = 0; i < nNumberOfParts && isPdStructNotCanceled(pPdStruct); i++) {
        FPART fpart = pListFParts->at(i);

        _MEMORY_RECORD record = {};
        record.nAddress = fpart.nVirtualAddress;
        record.nOffset = fpart.nFileOffset;
        record.nSize = fpart.nFileSize;
        record.nIndex = nIndex++;
        record.sName = fpart.sName;
        record.bIsVirtual = false;
        record.filePart = fpart.filePart;
        record.nFilePartNumber = i;

        pMemoryMap->listRecords.append(record);

        if (fpart.nVirtualSize > fpart.nFileSize) {
            // Add virtual size
            _MEMORY_RECORD virtualRecord = {};
            virtualRecord.nAddress = fpart.nVirtualAddress + fpart.nFileSize;
            virtualRecord.nOffset = -1;
            virtualRecord.nSize = fpart.nVirtualSize - fpart.nFileSize;
            virtualRecord.nIndex = nIndex++;
            virtualRecord.sName = fpart.sName + " (virtual)";
            virtualRecord.bIsVirtual = true;
            virtualRecord.filePart = fpart.filePart;
            virtualRecord.nFilePartNumber = i;

            pMemoryMap->listRecords.append(virtualRecord);
        }

        if (fpart.nVirtualAddress != -1) {
            if (nMinAddress == -1) {
                nMinAddress = fpart.nVirtualAddress;
            }

            nMinAddress = qMin(nMinAddress, fpart.nVirtualAddress);
            nMaxAddress = qMax(nMaxAddress, (XADDR)(fpart.nVirtualAddress + fpart.nVirtualSize));
        }
    }
    pMemoryMap->nModuleAddress = nMinAddress;
    pMemoryMap->nImageSize = nMaxAddress - nMinAddress;
}

XBinary::_MEMORY_MAP XBinary::_getSimpleMemoryMap(quint32 nFileParts, PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP result = {};

    QList<FPART> listParts = getFileParts(nFileParts, 1000, pPdStruct);

    _processMemoryMap(&result, &listParts, pPdStruct);

    return result;
}

XBinary::_MEMORY_MAP XBinary::getSimpleMemoryMap()
{
    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nBinarySize = nTotalSize;

    _MEMORY_RECORD record = {};
    record.nAddress = 0;
    record.nOffset = 0;
    record.nSize = nTotalSize;
    record.nIndex = 0;

    result.listRecords.append(record);

    return result;
}

XBinary::_MEMORY_MAP XBinary::_getMemoryMap(QList<FPART> *pListFParts, PDSTRUCT *pPdStruct)
{
    // TODO isImage
    _MEMORY_MAP result = {};

    if (_initMemoryMap(&result, pPdStruct)) {
        _processMemoryMap(&result, pListFParts, pPdStruct);
    }

    return result;
}

XBinary::_MEMORY_MAP XBinary::_getMemoryMap(quint32 nFileParts, PDSTRUCT *pPdStruct)
{
    QList<FPART> listParts = getFileParts(nFileParts, 1000, pPdStruct);

    return _getMemoryMap(&listParts, pPdStruct);
}

QList<XBinary::FPART> XBinary::getHData(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct);

    QList<XBinary::FPART> listResult;

    return listResult;
}

QList<XBinary::FPART> XBinary::getNativeRegions(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct);

    QList<XBinary::FPART> listResult;

    return listResult;
}

QList<XBinary::FPART> XBinary::getNativeSubRegions(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct);

    QList<XBinary::FPART> listResult;

    return listResult;
}

QList<XBinary::MAPMODE> XBinary::getMapModesList()
{
    QList<MAPMODE> listResult;

    listResult.append(MAPMODE_REGIONS);

    return listResult;
}

XBinary::_MEMORY_MAP XBinary::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    Q_UNUSED(pPdStruct)

    _MEMORY_MAP result = {};

    qint64 nTotalSize = getSize();

    result.nModuleAddress = getModuleAddress();
    result.nBinarySize = nTotalSize;
    result.nImageSize = getImageSize();
    result.fileType = getFileType();
    result.mode = getMode();
    result.sArch = getArch();
    result.endian = getEndian();
    result.sType = getTypeAsString();

    _MEMORY_RECORD record = {};
    record.nAddress = result.nModuleAddress;

    record.nOffset = 0;
    record.nSize = nTotalSize;
    record.nIndex = 0;
    record.sName = tr("Data");

    result.listRecords.append(record);

    return result;
}

qint32 XBinary::getNumberOfPhysicalRecords(XBinary::_MEMORY_MAP *pMemoryMap)
{
    qint32 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (!pMemoryMap->listRecords.at(i).bIsVirtual) {
            nResult++;
        }
    }

    return nResult;
}

qint32 XBinary::getNumberOfVirtualRecords(_MEMORY_MAP *pMemoryMap)
{
    qint32 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).bIsVirtual) {
            nResult++;
        }
    }

    return nResult;
}

qint32 XBinary::getNumberOfMemoryMapFileParts(_MEMORY_MAP *pMemoryMap, FILEPART filePart)
{
    qint32 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((pMemoryMap->listRecords.at(i).filePart == filePart) && (!pMemoryMap->listRecords.at(i).bIsVirtual)) {
            nResult++;
        }
    }

    return nResult;
}

qint64 XBinary::getRecordsTotalRowSize(_MEMORY_MAP *pMemoryMap)
{
    qint64 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (!pMemoryMap->listRecords.at(i).bIsVirtual) {
            nResult += pMemoryMap->listRecords.at(i).nSize;
        }
    }

    return nResult;
}

XADDR XBinary::getBaseAddress()
{
    return this->m_nBaseAddress;
}

void XBinary::setBaseAddress(XADDR nBaseAddress)
{
    this->m_nBaseAddress = nBaseAddress;
}

qint64 XBinary::getImageSize()
{
    return getSize();
}

bool XBinary::isImage()
{
    return m_bIsImage;
}

void XBinary::setIsImage(bool bValue)
{
    m_bIsImage = bValue;
}

void XBinary::setMultiSearchCallbackState(bool bState)
{
    m_bMultiSearchCallback = bState;
}

bool XBinary::compareSignature(const QString &sSignature, qint64 nOffset)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return compareSignature(&memoryMap, sSignature, nOffset);
}

bool XBinary::compareSignature(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QString sOrigin = sSignature;

    QString _sSignature = convertSignature(sSignature);

    bool bValid = true;

    QList<SIGNATURE_RECORD> listSignatureRecords = getSignatureRecords(_sSignature, &bValid, pPdStruct);

    if (listSignatureRecords.count()) {
        bResult = _compareSignature(pMemoryMap, &listSignatureRecords, nOffset, pPdStruct);
    } else {
        setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Invalid signature"), sOrigin));
    }

    return bResult;
}

bool XBinary::_compareByteArrayWithSignature(const QByteArray &baData, const QString &sSignature)
{
    bool bResult = false;

    QString sHex = baData.toHex().data();

    if (sHex.size() == sSignature.size()) {
        bResult = true;

        qint32 nNumberOfSymbols = sSignature.size();

        for (qint32 i = 0; i < nNumberOfSymbols; i++) {
            if (sSignature.at(i) != QChar('.')) {
                if (sSignature.at(i) != sHex.at(i)) {
                    bResult = false;
                    break;
                }
            }
        }
    }

    return bResult;
}

QString XBinary::_createSignature(const QString &sSignature1, const QString &sSignature2)
{
    QString sResult;

    qint32 nSize = sSignature1.size();

    if (sSignature1.size() == sSignature2.size()) {
        for (qint32 i = 0; i < nSize; i += 2) {
            if (sSignature1.mid(i, 2) == sSignature2.mid(i, 2)) {
                sResult.append(sSignature1.mid(i, 2));
            } else {
                sResult.append("..");
            }
        }
    }

    return sResult;
}

bool XBinary::compareSignatureOnAddress(const QString &sSignature, XADDR nAddress)
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return compareSignatureOnAddress(&memoryMap, sSignature, nAddress);
}

bool XBinary::compareSignatureOnAddress(XBinary::_MEMORY_MAP *pMemoryMap, const QString &sSignature, XADDR nAddress)
{
    bool bResult = false;

    qint64 nOffset = addressToOffset(pMemoryMap, nAddress);

    if (nOffset != -1) {
        bResult = compareSignature(pMemoryMap, sSignature, nOffset);
    }

    return bResult;
}

qint64 XBinary::_getEntryPointOffset()
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return getEntryPointOffset(&memoryMap);
}

qint64 XBinary::getEntryPointOffset(_MEMORY_MAP *pMemoryMap)
{
    return addressToOffset(pMemoryMap, pMemoryMap->nEntryPointAddress);
}

void XBinary::setEntryPointOffset(qint64 nEntryPointOffset)
{
    this->m_nEntryPointOffset = nEntryPointOffset;
}

XADDR XBinary::getEntryPointAddress()
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return getEntryPointAddress(&memoryMap);
}

XADDR XBinary::getEntryPointAddress(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return pMemoryMap->nEntryPointAddress;
}

XADDR XBinary::_getEntryPointAddress()
{
    return 0;
}

qint64 XBinary::getEntryPointRVA()
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return getEntryPointRVA(&memoryMap);
}

qint64 XBinary::getEntryPointRVA(_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->nEntryPointAddress) - pMemoryMap->nModuleAddress;
}

XADDR XBinary::getLowestAddress(XBinary::_MEMORY_MAP *pMemoryMap)
{
    XADDR nResult = -1;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nAddress != (XADDR)-1) {
            if (nResult == (XADDR)-1) {
                nResult = pMemoryMap->listRecords.at(i).nAddress;
            }

            nResult = qMin(pMemoryMap->listRecords.at(i).nAddress, nResult);
        }
    }

    return nResult;
}

qint64 XBinary::getTotalVirtualSize(XBinary::_MEMORY_MAP *pMemoryMap)
{
    qint64 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).filePart != FILEPART_OVERLAY)  // TODO Check ELF, MachO -1
        {
            nResult += pMemoryMap->listRecords.at(i).nSize;
        }
    }

    return nResult;
}

quint64 XBinary::positionToVirtualAddress(_MEMORY_MAP *pMemoryMap, qint64 nPosition)
{
    XADDR nResult = -1;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    qint64 _nSize = 0;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if ((_nSize <= nPosition) && (nPosition < _nSize + pMemoryMap->listRecords.at(i).nSize)) {
            nResult = pMemoryMap->listRecords.at(i).nAddress + (nPosition - _nSize);
        }

        _nSize += pMemoryMap->listRecords.at(i).nSize;
    }

    return nResult;
}

void XBinary::setModuleAddress(quint64 nValue)
{
    this->m_nModuleAddress = nValue;
}

XADDR XBinary::getModuleAddress()
{
    XADDR nResult = 0;

    if (m_nModuleAddress != (XADDR)-1) {
        nResult = m_nModuleAddress;
    } else {
        nResult = getBaseAddress();
    }

    return nResult;
}

bool XBinary::compareEntryPoint(const QString &sSignature, qint64 nOffset)
{
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    return compareEntryPoint(&memoryMap, sSignature, nOffset);
}

bool XBinary::compareEntryPoint(XBinary::_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset)
{
    qint64 nEPOffset = getEntryPointOffset(pMemoryMap) + nOffset;

    return compareSignature(pMemoryMap, sSignature, nEPOffset);
}

bool XBinary::moveMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize)
{
    bool bResult = false;

    if (nDestOffset == nSourceOffset) {
        return true;
    }

    qint64 nMaxSize = getSize();

    if ((nDestOffset + nSize > nMaxSize) || (nSourceOffset + nSize > nMaxSize)) {
        return false;
    }

    qint64 nDelta = nDestOffset - nSourceOffset;

    if (nDelta > 0) {
        bResult = copyMemory(nSourceOffset, nDestOffset, nSize, nDelta, true);
        zeroFill(nSourceOffset, nDelta);
    } else {
        bResult = copyMemory(nSourceOffset, nDestOffset, nSize, -nDelta, false);
        zeroFill(nDestOffset + nSize, -nDelta);
    }

    return bResult;
}

bool XBinary::moveMemory(QIODevice *pDevice, qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize)
{
    XBinary binary(pDevice);

    return binary.moveMemory(nSourceOffset, nDestOffset, nSize);
}

bool XBinary::dumpToFile(const QString &sFileName, const char *pData, qint64 nDataSize)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);
    file.resize(0);

    if (file.open(QIODevice::ReadWrite)) {
        file.write(pData, nDataSize);
        file.close();
        bResult = true;
    }

    return bResult;
}

bool XBinary::dumpToFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    if (nDataSize == -1) {
        qint64 _nSize = getSize();
        nDataSize = _nSize - nDataOffset;
    }

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);

        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];  // TODO const

        qint64 nSourceOffset = nDataOffset;
        qint64 nDestOffset = 0;

        qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nDataSize);

        bResult = true;

        while ((nDataSize > 0) && (!(pPdStruct->bIsStop))) {
            qint64 nTempSize = qMin(nDataSize, (qint64)0x1000);  // TODO const

            if (read_array_process(nSourceOffset, pBuffer, nTempSize, pPdStruct) != nTempSize) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                bResult = false;
                break;
            }

            if (!((file.seek(nDestOffset)) && (file.write(pBuffer, nTempSize) == nTempSize))) {
                setPdStructErrorString(pPdStruct, tr("Write error"));
                bResult = false;
                break;
            }

            nSourceOffset += nTempSize;
            nDestOffset += nTempSize;

            nDataSize -= nTempSize;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nDestOffset);
        }

        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

        delete[] pBuffer;

        file.close();
    } else {
        _errorMessage(QString("%1: %2").arg(QObject::tr("Cannot open file"), sFileName));
    }

    return bResult;
}

bool XBinary::dumpToFile(const QString &sFileName, QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);
        bResult = copyDeviceMemory(pDevice, 0, &file, 0, pDevice->size(), pPdStruct);
        file.close();
    }

    return bResult;
}

bool XBinary::patchFromFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    if (nDataSize == -1) {
        qint64 _nSize = getSize();
        nDataSize = _nSize - nDataOffset;
    }

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];  // TODO const

        qint64 nSourceOffset = 0;
        qint64 nDestOffset = nDataOffset;

        qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nDataSize);

        bResult = true;

        while ((nDataSize > 0) && (!(pPdStruct->bIsStop))) {
            qint64 nTempSize = qMin(nDataSize, (qint64)0x1000);  // TODO const

            if (safeReadData(&file, nSourceOffset, pBuffer, nTempSize, pPdStruct) != nTempSize) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                bResult = false;
                break;
            }

            if (safeWriteData(m_pDevice, nDestOffset, pBuffer, nTempSize, pPdStruct) != nTempSize) {
                setPdStructErrorString(pPdStruct, tr("Write error"));
                bResult = false;
                break;
            }

            nSourceOffset += nTempSize;
            nDestOffset += nTempSize;

            nDataSize -= nTempSize;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nDestOffset);
        }

        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

        delete[] pBuffer;

        file.close();
    } else {
        _errorMessage(QString("%1: %2").arg(QObject::tr("Cannot open file"), sFileName));
    }

    return bResult;
}

QSet<XBinary::FT> XBinary::getFileTypes(bool bExtra)
{
    QSet<XBinary::FT> stResult;

    stResult.insert(FT_BINARY);

    QByteArray baHeader;
    QByteArray baNewHeader;
    baHeader = read_array(0, qMin(getSize(), (qint64)0x200));  // TODO const
    char *pOffset = baHeader.data();
    qint64 nSize = getSize();
    bool bAllFound = false;

    if (nSize >= (qint64)sizeof(XMSDOS_DEF::IMAGE_DOS_HEADEREX)) {
        if ((_read_uint16(pOffset) == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_MZ) || (_read_uint16(pOffset) == XMSDOS_DEF::S_IMAGE_DOS_SIGNATURE_ZM)) {
            stResult.insert(FT_MSDOS);
            // TODO rewrite for NE, LE
            quint32 nLfanew = _read_uint32(pOffset + offsetof(XMSDOS_DEF::IMAGE_DOS_HEADEREX, e_lfanew));
            quint32 nHeaderSize = (quint32)baHeader.size() - sizeof(XPE_DEF::IMAGE_NT_HEADERS32);

            bool bIsNewHeaderValid = false;

            if ((nLfanew < nHeaderSize) && ((quint32)baHeader.size() > (nLfanew + sizeof(XPE_DEF::IMAGE_NT_HEADERS32))))  // TODO do not use
                                                                                                                          // IMAGE_NT_HEADERS32
            {
                pOffset += nLfanew;
                bIsNewHeaderValid = true;
            } else {
                qint64 nNtHeadersSize = 4 + sizeof(XPE_DEF::IMAGE_FILE_HEADER);

                baNewHeader = read_array(nLfanew, nNtHeadersSize);

                nHeaderSize = baNewHeader.size();

                if (nHeaderSize == nNtHeadersSize) {
                    pOffset = baNewHeader.data();
                    bIsNewHeaderValid = true;
                }
            }

            if (bIsNewHeaderValid) {
                bIsNewHeaderValid = false;

                if (_read_uint32(pOffset) == XPE_DEF::S_IMAGE_NT_SIGNATURE) {
                    stResult.insert(FT_PE);

                    quint16 nMachine = _read_uint32(pOffset + 4 + offsetof(XPE_DEF::IMAGE_FILE_HEADER, Machine));

                    // TODO more
                    if ((nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_AMD64) || (nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_IA64) ||
                        (nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_ARM64) || (nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_ALPHA64) ||
                        (nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_RISCV64) || (nMachine == XPE_DEF::S_IMAGE_FILE_MACHINE_LOONGARCH64)) {
                        stResult.insert(FT_PE64);
                    } else {
                        stResult.insert(FT_PE32);
                    }
                    bIsNewHeaderValid = true;
                } else if (_read_uint16(pOffset) == XNE_DEF::S_IMAGE_OS2_SIGNATURE) {
                    stResult.insert(FT_NE);
                    bIsNewHeaderValid = true;
                } else if (_read_uint32(pOffset) == XLE_DEF::S_IMAGE_VXD_SIGNATURE) {
                    stResult.insert(FT_LE);
                    bIsNewHeaderValid = true;
                } else if (_read_uint32(pOffset) == XLE_DEF::S_IMAGE_LX_SIGNATURE) {
                    stResult.insert(FT_LX);
                    bIsNewHeaderValid = true;
                }
            }

            if (!bIsNewHeaderValid) {
                quint16 nCP = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cp));
                quint16 nCblp = read_uint16(offsetof(XMSDOS_DEF::IMAGE_DOS_HEADER, e_cblp));

                if (nCP > 0) {
                    qint64 nSignatureOffset = (nCP - 1) * 512 + nCblp;
                    if (nSize - nSignatureOffset > 0) {
                        bool bBW = false;
                        bool b16M = false;
                        bool b4G = false;
                        while (true) {
                            quint16 nSignature = read_uint16(nSignatureOffset);

                            if (nSignature == 0x5742) {  // BW
                                bBW = true;
                                b16M = true;
                                nSignatureOffset = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::dos16m_exe_header, next_header_pos));
                            } else if (nSignature == 0x464D) {  // MF - find info
                                nSignatureOffset += read_uint32(nSignatureOffset + 2);
                            } else if (nSignature == 0x5A4D) {  // MZ
                                qint64 nSignatureOffsetOpt = read_uint32(nSignatureOffset + offsetof(XMSDOS_DEF::IMAGE_DOS_HEADEREX, e_lfanew));
                                quint16 nSignatureOpt = read_uint16(nSignatureOffsetOpt + nSignatureOffset);

                                if (nSignatureOpt == 0x454E) {  // NE
                                    b16M = true;
                                } else if (nSignatureOpt == 0x454C) {  // LE
                                    b4G = true;
                                } else if (nSignatureOpt == 0x584C) {  // LX
                                    b4G = true;
                                }
                                break;
                            } else {
                                break;
                            }
                        }

                        if (bBW && b4G) {
                            stResult.insert(FT_ARCHIVE);
                            stResult.insert(FT_DOS4G);
                        } else if (bBW && b16M) {
                            stResult.insert(FT_ARCHIVE);
                            stResult.insert(FT_DOS16M);
                        }
                    }
                }
            }

            bAllFound = true;
        }
    }

    if ((!bAllFound) && (nSize >= (qint64)sizeof(XELF_DEF::Elf32_Ehdr))) {
        if ((((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[0] == 0x7f) && (((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[1] == 'E') &&
            (((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[2] == 'L') && (((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[3] == 'F')) {
            stResult.insert(FT_ELF);

            if (((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[4] == 1) {
                stResult.insert(FT_ELF32);
            } else if (((XELF_DEF::Elf32_Ehdr *)pOffset)->e_ident[4] == 2) {
                stResult.insert(FT_ELF64);
            }
            // mb TODO another e_ident[4]
            bAllFound = true;
        }
    }

    if ((!bAllFound) && (nSize >= (qint64)sizeof(XMACH_DEF::mach_header))) {
        bool bMach = false;
        bool bBE = false;
        bool b64 = false;
        if (_read_uint32(pOffset) == XMACH_DEF::S_MH_MAGIC) {
            bMach = true;
            bBE = false;
            b64 = false;
        } else if (_read_uint32(pOffset) == XMACH_DEF::S_MH_CIGAM) {
            bMach = true;
            bBE = true;
            b64 = false;
        } else if (_read_uint32(pOffset) == XMACH_DEF::S_MH_MAGIC_64) {
            bMach = true;
            bBE = false;
            b64 = true;
        } else if (_read_uint32(pOffset) == XMACH_DEF::S_MH_CIGAM_64) {
            bMach = true;
            bBE = true;
            b64 = true;
        }

        if (bMach) {
            if (_read_uint32(pOffset + 0x0C, bBE) < 20) {
                stResult.insert(FT_MACHO);
                if (b64) {
                    stResult.insert(FT_MACHO64);
                } else {
                    stResult.insert(FT_MACHO32);
                }
                bAllFound = true;
            }
        }
    }

    if ((!bAllFound) && (nSize >= 8)) {
        quint32 nMagic = _read_uint32(pOffset);
        if ((nMagic == 0xf3030000) || (nMagic == 0xe7030000)) {
            stResult.insert(FT_AMIGAHUNK);

            bAllFound = true;
        }
    }

    if ((!bAllFound) && (nSize >= 28)) {
        quint16 nMagic = _read_uint16(pOffset, true);  // Big-endian
        if (nMagic == 0x601A) {
            stResult.insert(FT_ATARIST);

            bAllFound = true;
        }
    }

    if ((!bAllFound) && bExtra) {
        _MEMORY_MAP memoryMap = XBinary::getSimpleMemoryMap();
        UNICODE_TYPE unicodeType = getUnicodeType(&baHeader);

        bAllFound = true;

        if (compareSignature(&memoryMap, "'PK'0304", 0) || compareSignature(&memoryMap, "'PK'0506", 0))  // TODO baHeader
        {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_ZIP);
            // TODO Check APK, JAR
            // TODO basic ZIP
        } else if (compareSignature(&memoryMap, "1F8B08")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_GZIP);
        } else if (compareSignature(&memoryMap, "7801") || compareSignature(&memoryMap, "785E") || compareSignature(&memoryMap, "789C") ||
                   compareSignature(&memoryMap, "78DA")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_ZLIB);
        } else if (compareSignature(&memoryMap, "....'-lh'..2d") || compareSignature(&memoryMap, "....'-lz'..2d") || compareSignature(&memoryMap, "....'-pm'..2d")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_LHA);
        } else if (compareSignature(&memoryMap, "'!<arch>'0a")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_AR);
            // TODO DEB
        } else if ((memoryMap.nBinarySize >= 0x200) && compareSignature(&memoryMap, "00'ustar'", 0x100)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_TAR);
        } else if (compareSignature(&memoryMap, "'RE~^'") || compareSignature(&memoryMap, "'Rar!'1A07")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_RAR);
        } else if (compareSignature(&memoryMap, "'MSCF'00000000", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_CAB);
        } else if (compareSignature(&memoryMap, "'7z'BCAF271C", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_7Z);
        } else if (compareSignature(&memoryMap, "'LZIP'", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_LZIP);
        } else if (compareSignature(&memoryMap, "894C5A4F000D0A1A0A", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_LZO);
        } else if (compareSignature(&memoryMap, "1F9D", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_COMPRESS);
        } else if (compareSignature(&memoryMap, "303730373031", 0) || compareSignature(&memoryMap, "303730373032", 0) ||
                   compareSignature(&memoryMap, "303730373037", 0) ||
                   ((nSize >= 26) && (((read_uint16(0) == 0x71C7) && (read_uint16(20) > 0) && (read_uint16(20) < 0x1000)) ||
                                      ((read_uint16(0, true) == 0x71C7) && (read_uint16(20, true) > 0) && (read_uint16(20, true) < 0x1000))))) {
            // CPIO formats: 070701, 070702, 070707, binary little-endian, binary big-endian
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_CPIO);
        } else if (compareSignature(&memoryMap, "'MDMP'", 0)) {
            // Windows MiniDump format
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_MINIDUMP);
        } else if (compareSignature(&memoryMap, "'koly'", nSize - 512)) {
            // Apple Disk Image format (koly block at end)
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_DMG);
        } else if (compareSignature(&memoryMap, "60EA", 0) && nSize >= 34) {
            // ARJ format: 0x60 0xEA marker, basic_header_size >= 30, first_hdr_size >= 30
            // Note: ARJ 2.50+ uses first_hdr_size=34; older versions use 30
            quint16 _nArjHdrSize = read_uint16(2, false);
            quint8 _nArjFirstHdr = read_uint8(4);

            if ((_nArjHdrSize >= 30) && (_nArjHdrSize <= 2600) && (_nArjFirstHdr >= 30)) {
                stResult.insert(FT_ARCHIVE);
                stResult.insert(FT_ARJ);
            }
        } else if (nSize >= 14) {
            // ACE format: head_type=0 at offset 4, magic "**ACE**" at offset 7
            quint8 _nAceHeadType = read_uint8(4);
            quint16 _nAceHeadSize = read_uint16(2, false);

            if ((_nAceHeadType == 0) && (_nAceHeadSize >= 10)) {
                if (compareSignature(&memoryMap, "'**ACE**'", 7)) {
                    stResult.insert(FT_ARCHIVE);
                    stResult.insert(FT_ACE);
                }
            }
        } else if (nSize >= 29) {
            // ARC format: 0x1A + method(1-9) + filename(13 bytes null-terminated, first char printable ASCII 0x21-0x7E)
            quint8 _nArcMarker = read_uint8(0);
            quint8 _nArcMethod = read_uint8(1);
            quint8 _nArcFirstChar = read_uint8(2);

            if ((_nArcMarker == 0x1A) && (_nArcMethod >= 1) && (_nArcMethod <= 9) && (_nArcFirstChar >= 0x21) && (_nArcFirstChar <= 0x7E)) {
                stResult.insert(FT_ARCHIVE);
                stResult.insert(FT_ARC);
            }
        }

        // FreeARC format: "ArC\x01" signature at offset 0, second "ArC\x01" at offset 8
        if (compareSignature(&memoryMap, "'ArC'01", 0) && (nSize >= 12)) {
            quint32 _nFreeArcBlock = read_uint32(8, false);

            if (_nFreeArcBlock == 0x01437241) {
                stResult.insert(FT_ARCHIVE);
                stResult.insert(FT_FREEARC);
            }
        }

        if (compareSignature(&memoryMap, "89'PNG\r\n'1A0A", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_PNG);
        } else if (compareSignature(&memoryMap, "FFD8FFE0....'JFIF'00", 0) || compareSignature(&memoryMap, "FFD8FFE1....'Exif'00", 0) ||
                   compareSignature(&memoryMap, "FFD8FFDB", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_JPEG);
        } else if (compareSignature(&memoryMap, "'GIF87a'", 0) || compareSignature(&memoryMap, "'GIF89a'", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_GIF);
        } else if (compareSignature(&memoryMap, "'BM'..................000000", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_BMP);
        } else if (compareSignature(&memoryMap, "'MM'002A", 0) || compareSignature(&memoryMap, "'II'2A00", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_TIFF);
        } else if (compareSignature(&memoryMap, "00000100", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_ICO);
        } else if (compareSignature(&memoryMap, "00000200", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_CUR);
        } else if (compareSignature(&memoryMap, "........................'mntr'", 0)) {
            stResult.insert(FT_IMAGE);
            stResult.insert(FT_ICC);
        }

        if (compareSignature(&memoryMap, "4344303031", 0x8001, 0)) {  // "CD001" at offset 0x8001
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_ISO9660);
        } else if (compareSignature(&memoryMap, "0002", 256 * 2048, 0)) {  // UDF Anchor at sector 256
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_UDF);
        } else if (compareSignature(&memoryMap, "'ID3'..00", 0)) {
            stResult.insert(FT_AUDIO);
            stResult.insert(FT_MP3);
        } else if (compareSignature(&memoryMap, "000000..'ftyp'", 0)) {
            stResult.insert(FT_VIDEO);
            stResult.insert(FT_MP4);
        } else if (compareSignature(&memoryMap, "'Extended Module'", 0)) {
            stResult.insert(FT_AUDIO);
            stResult.insert(FT_XM);
        } else if (compareSignature(&memoryMap, "'dex\n'......00")) {
            stResult.insert(FT_DEX);
        } else if (compareSignature(&memoryMap, "00000800........0100") || compareSignature(&memoryMap, "03000800........0100")) {
            stResult.insert(FT_ANDROIDXML);
        } else if (compareSignature(&memoryMap, "02000C00........0100")) {
            stResult.insert(FT_ANDROIDASRC);
        } else if (compareSignature(&memoryMap, "'%PDF'", 0)) {
            stResult.insert(FT_DOCUMENT);
            stResult.insert(FT_PDF);
        } else if ((compareSignature(&memoryMap, "30", 0)) && (nSize >= 4)) {
            // Minimal DER/ASN.1 check: first byte is a tag, second is definite length short form (<0x80)
            // or long form (>=0x80) followed by that many length bytes; ensure it fits into the file.
            // quint8 nTag = _read_uint8(pOffset);
            PACKED_UINT packedLen = _read_acn1_integer(pOffset + 1, nSize - 1);

            if ((packedLen.bIsValid) && (packedLen.nByteSize > 0) && (1 + packedLen.nByteSize + packedLen.nValue <= (quint64)nSize)) {
                bool bDer = false;

                if (_read_uint8(pOffset + 1 + packedLen.nByteSize) == 0x06) {
                    // OID
                    bDer = true;
                }

                if (bDer) {
                    stResult.insert(FT_DOCUMENT);
                    stResult.insert(FT_DER);
                } else {
                    bAllFound = false;
                }
            }
        } else if (compareSignature(&memoryMap, "'RIFF'", 0) || compareSignature(&memoryMap, "'RIFX'", 0)) {
            // TODO AIFF
            stResult.insert(FT_RIFF);
            if (compareSignature(&memoryMap, "'RIFF'........'AVI '", 0)) {
                stResult.insert(FT_VIDEO);
                stResult.insert(FT_AVI);
            } else if (compareSignature(&memoryMap, "'RIFF'........'WEBPVP8'", 0)) {
                stResult.insert(FT_IMAGE);
                stResult.insert(FT_WEBP);
            } else if (compareSignature(&memoryMap, "'RIFF'........'WAVE'", 0)) {
                stResult.insert(FT_AUDIO);
                stResult.insert(FT_WAV);
            } else if (compareSignature(&memoryMap, "'RIFF'........'PAL '", 0)) {
                stResult.insert(FT_IMAGE);
                stResult.insert(FT_PAL);
            } /*else if (compareSignature(&memoryMap, "'RIFF'........'ACON'", 0)) {
                stResult.insert(FT_IMAGE);
                stResult.insert(FT_ANI);
            }*/
        } else if (compareSignature(&memoryMap, "'BW'....00..00000000", 0)) {
            stResult.insert(FT_BWDOS16M);
        } else if (compareSignature(&memoryMap, "D0CF11E0A1B11AE1", 0)) {
            stResult.insert(FT_CFBF);
        } else if (compareSignature(&memoryMap, "'OTTO'00", 0) || compareSignature(&memoryMap, "0001000000", 0)) {
            stResult.insert(FT_TTF);
        } else if (compareSignature(&memoryMap, "'AT&TFORM'", 0) || compareSignature(&memoryMap, "'SDJVFORM'", 0)) {
            stResult.insert(FT_DJVU);
        } else if (compareSignature(&memoryMap, "'SZDD'88F027'3A'", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_SZDD);
        } else if (compareSignature(&memoryMap, "'BZh'..314159265359", 0) || compareSignature(&memoryMap, "'BZh'..17724538509000000000")) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_BZIP2);
        } else if (compareSignature(&memoryMap, "28B52FFD", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_ZSTD);
        } else if (compareSignature(&memoryMap, "FD'7zXZ'00", 0)) {
            stResult.insert(FT_ARCHIVE);
            stResult.insert(FT_XZ);
        } else {
            bAllFound = false;
        }

        if (!bAllFound) {
            if (nSize >= (qint64)sizeof(XMACH_DEF::fat_header) + (qint64)sizeof(XMACH_DEF::fat_arch)) {
                if (read_uint32(0, true) == XMACH_DEF::S_FAT_MAGIC) {
                    if (read_uint32(4, true) < 10) {
                        stResult.insert(FT_ARCHIVE);
                        stResult.insert(FT_MACHOFAT);
                        bAllFound = true;
                    }
                } else if (read_uint32(0, false) == XMACH_DEF::S_FAT_MAGIC) {
                    if (read_uint32(4, false) < 10) {
                        stResult.insert(FT_ARCHIVE);
                        stResult.insert(FT_MACHOFAT);
                        bAllFound = true;
                    }
                }
            }
        }

        if (!bAllFound) {
            if (nSize >= 24) {
                if (read_uint32(0, true) == 0xCAFEBABE) {
                    if (read_uint32(4, true) > 10) {
                        stResult.insert(FT_JAVACLASS);
                        bAllFound = true;
                    }
                }
            }
        }

        if (!bAllFound) {
            if (nSize >= 12) {
                if (read_uint16(2) == 0x0A0D) {
                    // XPYC validation check
                    quint16 nMagic = read_uint16(0);
                    quint16 nCRLF = read_uint16(2);

                    // Python bytecode magic numbers (partial list, see XPYC for complete list)
                    // Format: first 2 bytes = magic, next 2 bytes = 0x0A0D (CRLF)
                    bool bIsPYC = false;
                    if (nCRLF == 0x0A0D) {
                        // Check for common Python magic numbers
                        if ((nMagic >= 0x0A0D) && (nMagic <= 0xFFFF)) {
                            // Basic validation: magic number present and CRLF marker
                            bIsPYC = true;
                        }
                    }

                    if (bIsPYC) {
                        stResult.insert(FT_PYC);
                        bAllFound = true;
                    }
                }
            }
        }

        if (isPlainTextType(&baHeader)) {
            stResult.insert(FT_TEXT);
            stResult.insert(FT_PLAINTEXT);
        } else if (isUTF8TextType(&baHeader)) {
            stResult.insert(FT_TEXT);
            stResult.insert(FT_UTF8);
        } else if (unicodeType != UNICODE_TYPE_NONE) {
            stResult.insert(FT_TEXT);
            stResult.insert(FT_UNICODE);

            if (unicodeType == UNICODE_TYPE_LE) {
                stResult.insert(FT_UNICODE_LE);
            } else {
                stResult.insert(FT_UNICODE_BE);
            }
        }
        // TODO more
        // TODO MIME

        // Fix
        if (stResult.contains(FT_GIF) && stResult.contains(FT_TEXT)) {
            stResult.remove(FT_GIF);
        }
    }

    if ((stResult.count() <= 1) || (stResult.contains(FT_PLAINTEXT))) {
        if ((nSize >= 0) && (nSize <= (0x10000 - 0x100))) {
            if (getDeviceFileSuffix(getDevice()).toUpper() == "COM") {
                stResult.insert(FT_COM);
            }
        }
    }

    return stResult;
}

QSet<XBinary::FT> XBinary::getFileTypes(QIODevice *pDevice, bool bExtra)
{
    XBinary _binary(pDevice);

    return _binary.getFileTypes(bExtra);
}

QSet<XBinary::FT> XBinary::getFileTypes(const QString &sFileName, bool bExtra)
{
    QSet<XBinary::FT> result;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        XBinary _binary(&file);

        result = _binary.getFileTypes(bExtra);

        file.close();
    }

    return result;
}

QSet<XBinary::FT> XBinary::getFileTypes(QByteArray *pbaData, bool bExtra)
{
    QSet<XBinary::FT> result;

    QBuffer buffer;

    buffer.setBuffer(pbaData);

    if (buffer.open(QIODevice::ReadOnly)) {
        XBinary _binary(&buffer);

        result = _binary.getFileTypes(bExtra);

        buffer.close();
    }

    return result;
}

XBinary::FT XBinary::_getPrefFileType(const QSet<FT> *pStFileTypes)
{
    if (!pStFileTypes) {
        return FT_UNKNOWN;
    }

    static const XBinary::FT g_arrPrefFileTypeOrder[] = {
        // Executables
        FT_PE64,
        FT_PE32,
        FT_MACHOFAT,
        FT_MACHO64,
        FT_MACHO32,
        FT_ELF64,
        FT_ELF32,
        FT_LE,
        FT_LX,
        FT_NE,
        FT_AMIGAHUNK,
        FT_ATARIST,
        FT_BWDOS16M,
        FT_DOS16M,
        FT_DOS4G,
        FT_MSDOS,

        // Android/Java ecosystems and archives
        FT_APKS,
        FT_APK,
        FT_IPA,
        FT_JAR,
        FT_ZIP,
        FT_NPM,
        FT_TAR_GZ,
        FT_TAR_BZIP2,
        FT_TAR_LZIP,
        FT_TAR_LZMA,
        FT_TAR_LZOP,
        FT_TAR_XZ,
        FT_TAR_Z,
        FT_TAR_ZSTD,
        FT_TAR,
        FT_GZIP,
        FT_ZLIB,
        FT_7Z,
        FT_RAR,
        FT_LHA,
        FT_ARJ,
        FT_ACE,
        FT_BROTLI,
        FT_ARC,
        FT_FREEARC,
        FT_DEB,
        FT_AR,
        FT_CAB,
        FT_CPIO,
        FT_ISO9660,
        FT_MINIDUMP,
        FT_DMG,

        // Android resources and bytecode
        FT_ANDROIDXML,
        FT_ANDROIDASRC,
        FT_DEX,
        FT_JAVACLASS,
        FT_PYC,

        // Documents and container formats
        FT_PDF,
        FT_DER,
        FT_CFBF,

        // Compressed/pack formats
        FT_SZDD,
        FT_BZIP2,
        FT_BROTLI,
        FT_ZSTD,
        FT_XZ,
        FT_LZIP,
        FT_LZO,
        FT_COMPRESS,

        // Fonts and images/media
        FT_TTF,
        FT_PNG,
        FT_JPEG,
        FT_WEBP,
        FT_BMP,
        FT_GIF,
        FT_TIFF,
        FT_ICO,
        FT_CUR,
        FT_ICC,
        FT_PAL,
        FT_MP4,
        FT_AVI,
        FT_MP3,
        FT_WAV,
        FT_XM,
        FT_RIFF,

        // Special
        FT_SIGNATURE,
        FT_DJVU,

        // Text/encoding
        FT_UNICODE,
        FT_UTF8,
        FT_TEXT,

        // Generic
        FT_COM,
        FT_DATA,
        FT_BINARY,
    };

    const qint32 nNumberOfTypes = sizeof(g_arrPrefFileTypeOrder) / sizeof(XBinary::FT);

    for (qint32 i = 0; i < nNumberOfTypes; i++) {
        if (pStFileTypes->contains(g_arrPrefFileTypeOrder[i])) {
            return g_arrPrefFileTypeOrder[i];
        }
    }

    return FT_UNKNOWN;
}

QList<XBinary::FT> XBinary::_getFileTypeListFromSet(const QSet<FT> &stFileTypes, TL_OPTION tlOption)
{
    QList<XBinary::FT> listResult;

    // TODO optimize !
    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_EXECUTABLE) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_REGION)) listResult.append(FT_REGION);
        if (stFileTypes.contains(FT_DATA)) listResult.append(FT_DATA);
    }

    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_BINARY)) listResult.append(FT_BINARY);
    }

    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_EXECUTABLE) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_COM)) listResult.append(FT_COM);
    }

    if (tlOption == TL_OPTION_ALL) {
        if (stFileTypes.contains(FT_ARCHIVE)) listResult.append(FT_ARCHIVE);
    }

    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_BINARY16)) listResult.append(FT_BINARY16);
        if (stFileTypes.contains(FT_BINARY32)) listResult.append(FT_BINARY32);
        if (stFileTypes.contains(FT_BINARY64)) listResult.append(FT_BINARY64);
        if (stFileTypes.contains(FT_ZIP)) listResult.append(FT_ZIP);
        if (stFileTypes.contains(FT_GZIP)) listResult.append(FT_GZIP);
        if (stFileTypes.contains(FT_ZLIB)) listResult.append(FT_ZLIB);
        if (stFileTypes.contains(FT_LHA)) listResult.append(FT_LHA);
        if (stFileTypes.contains(FT_ARJ)) listResult.append(FT_ARJ);
        if (stFileTypes.contains(FT_ACE)) listResult.append(FT_ACE);
        if (stFileTypes.contains(FT_ARC)) listResult.append(FT_ARC);
        if (stFileTypes.contains(FT_FREEARC)) listResult.append(FT_FREEARC);
        if (stFileTypes.contains(FT_RAR)) listResult.append(FT_RAR);
        if (stFileTypes.contains(FT_CAB)) listResult.append(FT_CAB);
        if (stFileTypes.contains(FT_JAR)) listResult.append(FT_JAR);
        if (stFileTypes.contains(FT_APK)) listResult.append(FT_APK);
        if (stFileTypes.contains(FT_IPA)) listResult.append(FT_IPA);
        if (stFileTypes.contains(FT_7Z)) listResult.append(FT_7Z);
        if (stFileTypes.contains(FT_DEX)) listResult.append(FT_DEX);
        if (stFileTypes.contains(FT_PDF)) listResult.append(FT_PDF);
        if (stFileTypes.contains(FT_DER)) listResult.append(FT_DER);
        if (stFileTypes.contains(FT_PNG)) listResult.append(FT_PNG);
        if (stFileTypes.contains(FT_ICO)) listResult.append(FT_ICO);
        if (stFileTypes.contains(FT_ICC)) listResult.append(FT_ICC);
        if (stFileTypes.contains(FT_JPEG)) listResult.append(FT_JPEG);
        if (stFileTypes.contains(FT_BMP)) listResult.append(FT_BMP);
        if (stFileTypes.contains(FT_GIF)) listResult.append(FT_GIF);
        if (stFileTypes.contains(FT_TIFF)) listResult.append(FT_TIFF);
        if (stFileTypes.contains(FT_PAL)) listResult.append(FT_PAL);
        if (stFileTypes.contains(FT_MP3)) listResult.append(FT_MP3);
        if (stFileTypes.contains(FT_MP4)) listResult.append(FT_MP4);
        if (stFileTypes.contains(FT_XM)) listResult.append(FT_XM);
        if (stFileTypes.contains(FT_AVI)) listResult.append(FT_AVI);
        if (stFileTypes.contains(FT_WEBP)) listResult.append(FT_WEBP);
        if (stFileTypes.contains(FT_WAV)) listResult.append(FT_WAV);
        if (stFileTypes.contains(FT_RIFF)) listResult.append(FT_RIFF);
        if (stFileTypes.contains(FT_SIGNATURE)) listResult.append(FT_SIGNATURE);
        if (stFileTypes.contains(FT_TAR)) listResult.append(FT_TAR);
        if (stFileTypes.contains(FT_TAR_GZ)) listResult.append(FT_TAR_GZ);
        if (stFileTypes.contains(FT_TAR_BZIP2)) listResult.append(FT_TAR_BZIP2);
        if (stFileTypes.contains(FT_TAR_LZIP)) listResult.append(FT_TAR_LZIP);
        if (stFileTypes.contains(FT_TAR_LZMA)) listResult.append(FT_TAR_LZMA);
        if (stFileTypes.contains(FT_TAR_LZOP)) listResult.append(FT_TAR_LZOP);
        if (stFileTypes.contains(FT_TAR_XZ)) listResult.append(FT_TAR_XZ);
        if (stFileTypes.contains(FT_TAR_Z)) listResult.append(FT_TAR_Z);
        if (stFileTypes.contains(FT_TAR_ZSTD)) listResult.append(FT_TAR_ZSTD);
        if (stFileTypes.contains(FT_NPM)) listResult.append(FT_NPM);
        if (stFileTypes.contains(FT_MACHOFAT)) listResult.append(FT_MACHOFAT);
        if (stFileTypes.contains(FT_AR)) listResult.append(FT_AR);
        if (stFileTypes.contains(FT_DEB)) listResult.append(FT_DEB);
        if (stFileTypes.contains(FT_JAVACLASS)) listResult.append(FT_JAVACLASS);
        if (stFileTypes.contains(FT_PYC)) listResult.append(FT_PYC);
        if (stFileTypes.contains(FT_TTF)) listResult.append(FT_TTF);
        if (stFileTypes.contains(FT_DJVU)) listResult.append(FT_DJVU);
        if (stFileTypes.contains(FT_TEXT)) listResult.append(FT_TEXT);
        if (stFileTypes.contains(FT_UTF8)) listResult.append(FT_UTF8);
        if (stFileTypes.contains(FT_UNICODE)) listResult.append(FT_UNICODE);
        if (stFileTypes.contains(FT_CFBF)) listResult.append(FT_CFBF);
        if (stFileTypes.contains(FT_SZDD)) listResult.append(FT_SZDD);
        if (stFileTypes.contains(FT_BZIP2)) listResult.append(FT_BZIP2);
        if (stFileTypes.contains(FT_BROTLI)) listResult.append(FT_BROTLI);
        if (stFileTypes.contains(FT_ZSTD)) listResult.append(FT_ZSTD);
        if (stFileTypes.contains(FT_XZ)) listResult.append(FT_XZ);
        if (stFileTypes.contains(FT_LZIP)) listResult.append(FT_LZIP);
        if (stFileTypes.contains(FT_LZO)) listResult.append(FT_LZO);
        if (stFileTypes.contains(FT_COMPRESS)) listResult.append(FT_COMPRESS);
        if (stFileTypes.contains(FT_CPIO)) listResult.append(FT_CPIO);
        if (stFileTypes.contains(FT_ISO9660)) listResult.append(FT_ISO9660);
        if (stFileTypes.contains(FT_UDF)) listResult.append(FT_UDF);
        if (stFileTypes.contains(FT_MINIDUMP)) listResult.append(FT_MINIDUMP);
        if (stFileTypes.contains(FT_DMG)) listResult.append(FT_DMG);
        if (stFileTypes.contains(FT_STK)) listResult.append(FT_STK);
    }

    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_EXECUTABLE) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_MSDOS)) listResult.append(FT_MSDOS);
        if (stFileTypes.contains(FT_NE)) listResult.append(FT_NE);
        if (stFileTypes.contains(FT_LE)) listResult.append(FT_LE);
        if (stFileTypes.contains(FT_LX)) listResult.append(FT_LX);
        if (stFileTypes.contains(FT_PE32)) listResult.append(FT_PE32);
        if (stFileTypes.contains(FT_PE64)) listResult.append(FT_PE64);
        if (stFileTypes.contains(FT_ELF32)) listResult.append(FT_ELF32);
        if (stFileTypes.contains(FT_ELF64)) listResult.append(FT_ELF64);
        if (stFileTypes.contains(FT_MACHO32)) listResult.append(FT_MACHO32);
        if (stFileTypes.contains(FT_MACHO64)) listResult.append(FT_MACHO64);
        if (stFileTypes.contains(FT_BWDOS16M)) listResult.append(FT_BWDOS16M);
        if (stFileTypes.contains(FT_AMIGAHUNK)) listResult.append(FT_AMIGAHUNK);
        if (stFileTypes.contains(FT_ATARIST)) listResult.append(FT_ATARIST);
    }

    if ((tlOption == TL_OPTION_DEFAULT) || (tlOption == TL_OPTION_ALL)) {
        if (stFileTypes.contains(FT_DOS16M)) listResult.append(FT_DOS16M);
        if (stFileTypes.contains(FT_DOS4G)) listResult.append(FT_DOS4G);
    }

    if ((listResult.count() == 0) && (tlOption == TL_OPTION_EXECUTABLE)) {
        listResult.append(FT_BINARY);
    }

    if (tlOption == TL_OPTION_SYMBOLS) {
        if (stFileTypes.contains(FT_PE32)) listResult.append(FT_PE32);
        if (stFileTypes.contains(FT_PE64)) listResult.append(FT_PE64);
        if (stFileTypes.contains(FT_MACHO32)) listResult.append(FT_MACHO32);
        if (stFileTypes.contains(FT_MACHO64)) listResult.append(FT_MACHO64);
    }

    return listResult;
}

QString XBinary::valueToHex(quint8 nValue)
{
    return QString("%1").arg(nValue, 2, 16, QChar('0'));
}

QString XBinary::valueToHex(qint8 nValue)
{
    return valueToHex((quint8)nValue);
}

QString XBinary::valueToHex(quint16 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return QString("%1").arg(nValue, 4, 16, QChar('0'));
}

QString XBinary::valueToHex(qint16 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return valueToHex((quint16)nValue);
}

QString XBinary::valueToHex(quint32 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return QString("%1").arg(nValue, 8, 16, QChar('0'));
}

QString XBinary::valueToHex(qint32 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return valueToHex((quint32)nValue);
}

QString XBinary::valueToHex(quint64 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return QString("%1").arg(nValue, 16, 16, QChar('0'));
}

QString XBinary::valueToHex(qint64 nValue, bool bIsBigEndian)
{
    if (bIsBigEndian) {
        nValue = qFromBigEndian(nValue);
    } else {
        nValue = qFromLittleEndian(nValue);
    }

    return valueToHex((quint64)nValue);
}

QString XBinary::valueToHex(float fValue, bool bIsBigEndian)
{
    float _value = fValue;

    endian_float(&_value, bIsBigEndian);

    quint32 _nValue = 0;

    _copyMemory((char *)&_nValue, (char *)&_value, 4);

    return QString("%1").arg(_nValue, 8, 16, QChar('0'));
}

QString XBinary::valueToHex(double dValue, bool bIsBigEndian)
{
    double _value = dValue;

    endian_double(&_value, bIsBigEndian);

    quint64 _nValue = 0;

    _copyMemory((char *)&_nValue, (char *)&_value, 8);

    return QString("%1").arg(_nValue, 16, 16, QChar('0'));
}

QString XBinary::valueToHex(XBinary::MODE mode, quint64 nValue, bool bIsBigEndian)
{
    QString sResult;

    if (mode == MODE_UNKNOWN) {
        mode = getWidthModeFromSize(nValue);
    }

    if (mode == MODE_BIT) {
        if (nValue) {
            sResult = "1";
        } else {
            sResult = "0";
        }
    } else if (mode == MODE_8) {
        sResult = valueToHex((quint8)nValue);
    } else if (mode == MODE_16) {
        sResult = valueToHex((quint16)nValue, bIsBigEndian);
    } else if (mode == MODE_32) {
        sResult = valueToHex((quint32)nValue, bIsBigEndian);
    } else if (mode == MODE_64) {
        sResult = valueToHex((quint64)nValue, bIsBigEndian);
    }

    return sResult;
}

QString XBinary::valueToHexEx(quint64 nValue, bool bIsBigEndian)
{
    XBinary::MODE mode = getWidthModeFromSize(nValue);

    return valueToHex(mode, nValue, bIsBigEndian);
}

QString XBinary::valueToHexOS(quint64 nValue, bool bIsBigEndian)
{
    MODE mode = MODE_32;

    if (sizeof(void *) == 8) {
        mode = MODE_64;
    }

    return valueToHex(mode, nValue, bIsBigEndian);
}

QString XBinary::valueToHexColon(MODE mode, quint64 nValue, bool bIsBigEndian)
{
    QString sResult;

    if (mode == MODE_64) {
        quint32 nHigh = (quint32)(nValue >> 32);
        quint32 nLow = (quint32)(nValue);
        sResult = QString("%1:%2").arg(valueToHex(nHigh), valueToHex(nLow));
    } else if (mode == MODE_32) {
        quint16 nHigh = (quint16)(nValue >> 16);
        quint16 nLow = (quint16)(nValue);
        sResult = QString("%1:%2").arg(valueToHex(nHigh), valueToHex(nLow));
    } else {
        sResult = valueToHex(mode, nValue, bIsBigEndian);
    }

    return sResult;
}

QString XBinary::xVariantToHex(XVARIANT value)
{
    QString sResult;

    if (value.varType == VT_BIT) {
        if (value.var.toBool()) {
            sResult = "1";
        } else {
            sResult = "0";
        }
    } else if (value.varType == VT_BYTE) {
        sResult = valueToHex((quint8)value.var.toULongLong());
    } else if (value.varType == VT_WORD) {
        sResult = valueToHex((quint16)value.var.toULongLong(), value.bIsBigEndian);
    } else if (value.varType == VT_DWORD) {
        sResult = valueToHex((quint32)value.var.toULongLong(), value.bIsBigEndian);
    } else if (value.varType == VT_QWORD) {
        sResult = valueToHex((quint64)value.var.toULongLong(), value.bIsBigEndian);
    } else if (value.varType == VT_128) {
        // QString sLow = valueToHex(value.var.v_uint128[0], value.bIsBigEndian);
        // QString sHigh = valueToHex(value.var.v_uint128[1], value.bIsBigEndian);

        // if (value.bIsBigEndian) {
        //     sResult = sLow + sHigh;
        // } else {
        //     sResult = sHigh + sLow;
        // }
    } else if (value.varType == VT_256) {
        // QString s0 = valueToHex(value.var.v_uint256[0], value.bIsBigEndian);
        // QString s1 = valueToHex(value.var.v_uint256[1], value.bIsBigEndian);
        // QString s2 = valueToHex(value.var.v_uint256[2], value.bIsBigEndian);
        // QString s3 = valueToHex(value.var.v_uint256[3], value.bIsBigEndian);

        // if (value.bIsBigEndian) {
        //     sResult = s0 + s1 + s2 + s3;
        // } else {
        //     sResult = s3 + s2 + s1 + s0;
        // }
    } else if (value.varType == VT_FPEG) {
        // for (qint32 i = 0; i < 10; i++) {
        //     sResult += valueToHex(value.var.v_freg[i]);
        // }
    }

    return sResult;
}

QString XBinary::thisToString(qint64 nDelta, qint32 nBase)
{
    QString sResult;

    if (nDelta == 0) {
        sResult = "$ ==>";
    } else if (nDelta > 0) {
        sResult = QString("$+%1").arg(nDelta, 0, nBase);
    } else if (nDelta < 0) {
        sResult = QString("$-%1").arg(-nDelta, 0, nBase);
    }

    return sResult;
}

bool XBinary::checkString_byte(const QString &sValue)
{
    bool bResult = false;

    // TODO Check

    quint16 nValue = sValue.toUShort(&bResult, 16);

    if (bResult) {
        bResult = (nValue <= 256);
    }

    return bResult;
}

bool XBinary::checkString_word(const QString &sValue)
{
    bool bResult = false;

    sValue.toUShort(&bResult, 16);

    return bResult;
}

bool XBinary::checkString_dword(const QString &sValue)
{
    bool bResult = false;

    sValue.toUInt(&bResult, 16);

    return bResult;
}

bool XBinary::checkString_qword(const QString &sValue)
{
    bool bResult = false;

    sValue.toULongLong(&bResult, 16);

    return bResult;
}

bool XBinary::checkString_uint8(const QString &sValue)
{
    bool bResult = false;

    // TODO Check

    quint16 nValue = sValue.toUShort(&bResult);

    if (bResult) {
        bResult = (nValue <= 256);
    }

    return bResult;
}

bool XBinary::checkString_int8(const QString &sValue)
{
    bool bResult = false;

    qint16 nValue = sValue.toShort(&bResult);

    if (bResult) {
        bResult = ((-127 <= nValue) && (nValue <= 128));
    }

    return bResult;
}

bool XBinary::checkString_uint16(const QString &sValue)
{
    bool bResult = false;

    sValue.toUShort(&bResult);

    return bResult;
}

bool XBinary::checkString_int16(const QString &sValue)
{
    bool bResult = false;

    sValue.toShort(&bResult);

    return bResult;
}

bool XBinary::checkString_uint32(const QString &sValue)
{
    bool bResult = false;

    sValue.toUInt(&bResult);

    return bResult;
}

bool XBinary::checkString_int32(const QString &sValue)
{
    bool bResult = false;

    sValue.toInt(&bResult);

    return bResult;
}

bool XBinary::checkString_uint64(const QString &sValue)
{
    bool bResult = false;

    sValue.toULongLong(&bResult);

    return bResult;
}

bool XBinary::checkString_int64(const QString &sValue)
{
    bool bResult = false;

    sValue.toLongLong(&bResult);

    return bResult;
}

bool XBinary::checkString_float(const QString &sValue)
{
    bool bResult = false;

    sValue.toFloat(&bResult);

    return bResult;
}

bool XBinary::checkString_double(const QString &sValue)
{
    bool bResult = false;

    sValue.toDouble(&bResult);

    return bResult;
}

QString XBinary::boolToString(bool bValue)
{
    QString sResult;

    if (bValue) {
        sResult = tr("true");
    } else {
        sResult = tr("false");
    }

    return sResult;
}

QString XBinary::getSpaces(qint32 nNumberOfSpaces)
{
    QString sResult;

    sResult = sResult.rightJustified(nNumberOfSpaces, QChar(' '));

    return sResult;
}

QString XBinary::getUnpackedFileName(QIODevice *pDevice, bool bShort)
{
    QString sResult = "unpacked";

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        QString sFileName = pFile->fileName();

        if (sFileName != "") {
            sResult = getUnpackedFileName(sFileName);
        }

        if (bShort) {
            QFileInfo fi(sResult);

            sResult = fi.completeBaseName() + "." + fi.suffix();
        }
    }

    return sResult;
}

QString XBinary::getUnpackedFileName(const QString &sFileName)
{
    QFileInfo fileInfo(sFileName);
    QString sResult = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".unp." + fileInfo.suffix();
    //            sResult=fi.absolutePath()+QDir::separator()+fi.baseName()+".unp."+fi.completeSuffix();

    return sResult;
}

QString XBinary::getDumpFileName(QIODevice *pDevice)
{
    QString sResult = "dump";

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        QString sFileName = pFile->fileName();

        if (sFileName != "") {
            sResult = getDumpFileName(sFileName);
        }
    }

    return sResult;
}

QString XBinary::getDumpFileName(const QString &sFileName)
{
    QFileInfo fileInfo(sFileName);
    QString sResult = fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".dump." + fileInfo.suffix();

    return sResult;
}

QString XBinary::getBackupFileName(QIODevice *pDevice)
{
    QString sResult = QString("Backup_%1.BAK").arg(getCurrentBackupDate());

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        QString sFileName = pFile->fileName();

        if (sFileName != "") {
            sResult = getBackupFileName(sFileName);
        }
    }

    return sResult;
}

QString XBinary::getBackupFileName(const QString &sFileName)
{
    QFileInfo fi(sFileName);
    QString sResult;

    sResult += fi.absolutePath() + QDir::separator() + fi.completeBaseName();

    QString sSuffix = fi.suffix();

    if (sSuffix != "") {
        sResult += "." + sSuffix;
    }

    sResult += QString("_%1.BAK").arg(getCurrentBackupDate());

    return sResult;
}

QString XBinary::getResultFileName(QIODevice *pDevice, const QString &sAppendix)
{
    QString sResult = sAppendix;

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        QString sFileName = pFile->fileName();

        if (sFileName != "") {
            sResult = getResultFileName(sFileName, sAppendix);
        }
    }

    return sResult;
}

QString XBinary::getResultFileName(const QString &sFileName, const QString &sAppendix)
{
    QString sResult;
    // mb TODO if file exists write other .1 .2 ...
    QFileInfo fileInfo(sFileName);

    QString sSuffix = fileInfo.suffix();

    sResult += fileInfo.absolutePath() + QDir::separator() + fileInfo.completeBaseName() + ".";

    if (sSuffix != "") {
        sResult += sSuffix + ".";
    }

    sResult += sAppendix;

    return sResult;
}

QString XBinary::getDeviceFileName(QIODevice *pDevice)
{
    QString sResult = pDevice->property("FileName").toString();

    if (sResult == "") {
        QFile *pFile = dynamic_cast<QFile *>(pDevice);

        if (pFile) {
            sResult = pFile->fileName();
        }
    }

    return sResult;
}

QString XBinary::getDeviceFilePath(QIODevice *pDevice)
{
    QString sResult;

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        QString sFileName = pFile->fileName();

        if (sFileName != "") {
            QFileInfo fi(sFileName);

            sResult = fi.absoluteFilePath();
        }
    }

    return sResult;
}

QString XBinary::getDeviceDirectory(QIODevice *pDevice)
{
    QString sResult;

    QString sFileName = getDeviceFileName(pDevice);

    if (sFileName != "") {
        QFileInfo fi(sFileName);

        sResult = fi.absolutePath();
    }

    return sResult;
}

QString XBinary::getDeviceFileBaseName(QIODevice *pDevice)
{
    QString sResult;

    QString sFileName = getDeviceFileName(pDevice);

    if (sFileName != "") {
        QFileInfo fi(sFileName);

        sResult = fi.baseName();
    }

    return sResult;
}

QString XBinary::getDeviceFileCompleteSuffix(QIODevice *pDevice)
{
    QString sResult;

    QString sFileName = getDeviceFileName(pDevice);

    if (sFileName != "") {
        QFileInfo fi(sFileName);

        sResult = fi.completeSuffix();
    }

    return sResult;
}

QString XBinary::getDeviceFileSuffix(QIODevice *pDevice)
{
    QString sResult;

    QString sFileName = getDeviceFileName(pDevice);

    if (sFileName != "") {
        QFileInfo fi(sFileName);

        sResult = fi.suffix();
    }

    return sResult;
}

QString XBinary::getFileDirectory(const QString &sFileName)
{
    QFileInfo fi(sFileName);

    return fi.absolutePath();
}

QIODevice *XBinary::getBackupDevice(QIODevice *pDevice)
{
    QIODevice *pResult = nullptr;

    pResult = (QIODevice *)pDevice->property("BACKUPDEVICE").toULongLong();

    if (!pResult) {
        pResult = pDevice;
    }

    return pResult;
}

bool XBinary::isBackupPresent(QIODevice *pDevice)
{
    return XBinary::isFileExists(XBinary::getBackupFileName(pDevice));
}

bool XBinary::saveBackup(QIODevice *pDevice)
{
    bool bResult = false;

    QString sBackupFileName = XBinary::getBackupFileName(pDevice);

    if (sBackupFileName != "") {
        if (!QFile::exists(sBackupFileName)) {
            QString sFileName = XBinary::getDeviceFileName(pDevice);

            if (sFileName != "") {
                bResult = QFile::copy(sFileName, sBackupFileName);
            } else {
                bResult = XBinary::writeToFile(sBackupFileName, pDevice);
            }
        } else {
            bResult = true;
        }
    }

    return bResult;
}

QString XBinary::getCurrentBackupDate()
{
    QString sResult;

    sResult = QDate::currentDate().toString("yyyy-MM-dd");

    return sResult;
}

QList<qint64> XBinary::getFixupList(QIODevice *pDevice1, QIODevice *pDevice2, qint64 nDelta, PDSTRUCT *pPdStruct)
{
    QList<qint64> listResult;

    qint64 nSize1 = pDevice1->size();
    qint64 nSize2 = pDevice2->size();

    if (nSize1 == nSize2) {
        qint64 nSize = nSize1;
        qint64 nTemp = 0;

        qint32 nBufferSize = getBufferSize(pPdStruct);

        char *pBuffer1 = new char[nBufferSize];
        char *pBuffer2 = new char[nBufferSize];
        qint64 nOffset = 0;

        while (nSize > 3) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            pDevice1->seek(nOffset);

            if (pDevice1->read(pBuffer1, nTemp) <= 0) {
                break;
            }

            pDevice2->seek(nOffset);

            if (pDevice2->read(pBuffer2, nTemp) <= 0) {
                break;
            }

            for (quint32 i = 0; i < nTemp - 3; i++) {
                qint32 nValue1 = *(qint32 *)(pBuffer1 + i);
                qint32 nValue2 = *(qint32 *)(pBuffer2 + i);

                if (nValue1 + nDelta == nValue2) {
                    listResult.append(nOffset + i);
                }
            }

            nSize -= nTemp - 3;
            nOffset += nTemp - 3;
        }

        delete[] pBuffer1;
        delete[] pBuffer2;
    }

    return listResult;
}

QString XBinary::getHash(XBinary::HASH hash, const QString &sFileName, PDSTRUCT *pPdStruct)
{
    QString sResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        sResult = XBinary::getHash(hash, &file, pPdStruct);

        file.close();
    }

    return sResult;
}

QString XBinary::getHash(XBinary::HASH hash, QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    QString sResult;

    XBinary binary(pDevice);

    sResult = binary.getHash(hash, 0, -1, pPdStruct);

    pDevice->reset();

    return sResult;
}

QString XBinary::getHash(XBinary::HASH hash, qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    QString sResult;

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    if (osRegion.nOffset != -1) {
        QList<OFFSETSIZE> listOS;
        listOS.append(osRegion);

        sResult = getHash(hash, &listOS, pPdStruct);
    }

    return sResult;
}

QString XBinary::getHash(HASH hash, QList<OFFSETSIZE> *pListOS, PDSTRUCT *pPdStruct)
{
    QString sResult;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint64 nTemp = 0;
    qint32 nBufferSize = getBufferSize(pPdStruct);
    char *pBuffer = new char[nBufferSize];

    QCryptographicHash::Algorithm algorithm = QCryptographicHash::Md4;

    switch (hash)  // TODO Check new versions of Qt
    {
        case HASH_MD4: algorithm = QCryptographicHash::Md4; break;
        case HASH_MD5: algorithm = QCryptographicHash::Md5; break;
        case HASH_SHA1: algorithm = QCryptographicHash::Sha1; break;
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA
#if (QT_VERSION_MAJOR > 4)
        case HASH_SHA224: algorithm = QCryptographicHash::Sha224; break;  // Keccak_224 ?
        case HASH_SHA256: algorithm = QCryptographicHash::Sha256; break;
        case HASH_SHA384: algorithm = QCryptographicHash::Sha384; break;
        case HASH_SHA512: algorithm = QCryptographicHash::Sha512; break;
#endif
#endif
    }

    QCryptographicHash crypto(algorithm);

    qint32 nNumberOfRecords = pListOS->count();

    qint64 nCurrentSize = 0;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    bool bReadError = false;

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        qint64 nOffset = pListOS->at(i).nOffset;
        qint64 nSize = pListOS->at(i).nSize;

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));

                bReadError = true;

                break;
            }

            crypto.addData(pBuffer, nTemp);

            nSize -= nTemp;
            nOffset += nTemp;
            nCurrentSize += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nCurrentSize);
        }
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    delete[] pBuffer;

    if (!bReadError) {
        sResult = crypto.result().toHex();
    }

    if (pPdStruct->bIsStop) {
        sResult = "";
    }

    return sResult;
}

QSet<XBinary::HASH> XBinary::getHashMethods()
{
    QSet<XBinary::HASH> stResult;

    stResult.insert(HASH_MD4);
    stResult.insert(HASH_MD5);
    stResult.insert(HASH_SHA1);
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
#if (QT_VERSION_MAJOR > 4)
    stResult.insert(HASH_SHA224);
    stResult.insert(HASH_SHA256);
    stResult.insert(HASH_SHA384);
    stResult.insert(HASH_SHA512);
#endif
#endif

    return stResult;
}

QList<XBinary::HASH> XBinary::getHashMethodsAsList()
{
    QList<XBinary::HASH> listResult;

    listResult.append(HASH_MD4);
    listResult.append(HASH_MD5);
    listResult.append(HASH_SHA1);
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
#if (QT_VERSION_MAJOR > 4)
    listResult.append(HASH_SHA224);
    listResult.append(HASH_SHA256);
    listResult.append(HASH_SHA384);
    listResult.append(HASH_SHA512);
#endif
#endif

    return listResult;
}

QString XBinary::hashIdToString(XBinary::HASH hash)
{
    QString sResult = tr("Unknown");

    switch (hash) {
        case HASH_MD4: sResult = QString("MD4"); break;
        case HASH_MD5: sResult = QString("MD5"); break;
        case HASH_SHA1: sResult = QString("SHA1"); break;
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
#if (QT_VERSION_MAJOR > 4)
        case HASH_SHA224: sResult = QString("SHA224"); break;
        case HASH_SHA256: sResult = QString("SHA256"); break;
        case HASH_SHA384: sResult = QString("SHA384"); break;
        case HASH_SHA512: sResult = QString("SHA512"); break;
#endif
#endif
    }

    return sResult;
}

bool XBinary::isFileHashValid(XBinary::HASH hash, const QString &sFileName, const QString &sHash)
{
    bool bResult = false;

    if (isFileExists(sFileName)) {
        bResult = (getHash(hash, sFileName).toUpper() == sHash.toUpper());
    }

    return bResult;
}

quint32 XBinary::getAdler32(const QString &sFileName)
{
    quint32 nResult = 0;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        nResult = XBinary::getAdler32(&file);

        file.close();
    }

    return nResult;
}

quint32 XBinary::getAdler32(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    quint32 nResult = 0;

    pDevice->reset();

    XBinary binary(pDevice);

    nResult = binary.getAdler32(0, -1, pPdStruct);

    pDevice->reset();

    return nResult;
}

quint32 XBinary::getAdler32(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    // TODO Check crash
    // TODO optimize!!!
    // TODO Progress bar
    quint32 nResult = 0;

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    const quint32 MOD_ADLER = 65521;

    if (nOffset != -1) {
        qint64 nTemp = 0;
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        quint32 a = 1;
        quint32 b = 0;

        qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        while ((nSize > 0) && isPdStructNotCanceled(pPdStruct)) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array_process(nOffset, pBuffer, nTemp, pPdStruct) != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                delete[] pBuffer;
                return 0;
            }

            for (qint64 i = 0; (i < nTemp) && isPdStructNotCanceled(pPdStruct); i++) {
                a = (a + (quint8)(pBuffer[i])) % MOD_ADLER;
                b = (b + a) % MOD_ADLER;
            }

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset);
        }

        XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

        delete[] pBuffer;

        nResult = (b << 16) | a;
    }

    return nResult;
}

void XBinary::_createCRC32Table(quint32 *pCRCTable, quint32 nPoly)
{
    for (qint32 i = 0; i < 256; i++) {
        quint32 crc = i;

        for (qint32 j = 0; j < 8; j++) {
            crc = (crc & 1) ? ((crc >> 1) ^ nPoly) : (crc >> 1);
        }

        *(pCRCTable + i) = crc;
    }
}

quint32 *XBinary::_getCRC32Table_EDB88320()
{
    return (quint32 *)_crc32_EDB88320_tab;
}

quint16 *XBinary::_getCRC16Table()
{
    return (quint16 *)_crc16_tab;
}

quint32 XBinary::_getCRC32(const QString &sFileName, PDSTRUCT *pPdStruct)
{
    quint32 nResult = 0;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        nResult = XBinary::_getCRC32(&file, pPdStruct);

        file.close();
    }

    return nResult;
}

quint32 XBinary::_getCRC32(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    quint32 nResult = 0;

    XBinary binary(pDevice);

    nResult = binary._getCRC32(0, -1, 0xFFFFFFFF, _getCRC32Table_EDB88320(), pPdStruct);

    pDevice->reset();

    //    quint32 nResult2 = binary._getCRC32_2(0, -1);

    return nResult;
}

quint32 XBinary::_getCRC32(QIODevice *pDevice, quint32 nInit, quint32 *pCRCTable, PDSTRUCT *pPdStruct)
{
    quint32 nResult = nInit;

    XBinary binary(pDevice);

    OFFSETSIZE osRegion = binary.convertOffsetAndSize(0, -1);
    qint64 nOffset = osRegion.nOffset;
    qint64 nSize = osRegion.nSize;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    if ((nOffset != -1) && (XBinary::isPdStructNotCanceled(pPdStruct))) {
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        qint64 nTemp = 0;
        qint32 nBufferSize = binary.getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while (nSize > 0) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (binary.read_array(nOffset, pBuffer, nTemp) != nTemp) {
                XBinary::setPdStructInfoString(pPdStruct, QObject::tr("Read error"));
                nResult = 0;
                break;
            }

            nResult = _getCRC32(pBuffer, (qint32)nTemp, nResult, pCRCTable);

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset);
        }

        delete[] pBuffer;
    }

    pDevice->reset();

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if (XBinary::isPdStructStopped(pPdStruct)) {
        nResult = 0;
    }

    return (quint32)nResult;
}

quint32 XBinary::_getCRC32(const char *pData, qint32 nDataSize, quint32 nInit, quint32 *pCRCTable)
{
    quint32 nResult = nInit;

    while (nDataSize > 0) {
        quint8 nIndex = (nResult ^ (((quint8)(*pData)) & 0xFF));
        nResult = (*(pCRCTable + nIndex)) ^ (nResult >> 8);

        nDataSize--;
        pData++;
    }

    return nResult;
}

quint16 XBinary::_getCRC16(const char *pData, qint32 nDataSize, quint16 nInit, quint16 *pCRCTable)
{
    quint16 nResult = nInit;

    while (nDataSize > 0) {
        quint8 nIndex = (nResult ^ (((quint8)(*pData)) & 0xFF));
        nResult = (*(pCRCTable + nIndex)) ^ (nResult >> 8);

        nDataSize--;
        pData++;
    }

    return nResult;
}

quint32 XBinary::_getCRC32(const QByteArray &baData, quint32 nInit, quint32 *pCRCTable)
{
    return _getCRC32(baData.data(), baData.size(), nInit, pCRCTable);
}

quint16 XBinary::_getCRC16(const QByteArray &baData, quint16 nInit, quint16 *pCRCTable)
{
    return _getCRC16(baData.data(), baData.size(), nInit, pCRCTable);
}

bool XBinary::checkCRC(QIODevice *pDevice, CRC_TYPE crcType, QVariant value, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (!pDevice) {
        return bResult;
    }

    quint32 nExpectedCRC = value.toUInt();
    quint32 nCalculatedCRC = 0;
    bool bCRCComputed = true;

    if (crcType == CRC_TYPE_FFFFFFFF_EDB88320_00000000) {
        nCalculatedCRC = _getCRC32(pDevice, 0xFFFFFFFF, _getCRC32Table_EDB88320(), pPdStruct);
    } else if (crcType == CRC_TYPE_FFFFFFFF_EDB88320_FFFFFFFFF) {
        nCalculatedCRC = _getCRC32(pDevice, 0xFFFFFFFF, _getCRC32Table_EDB88320(), pPdStruct);
        nCalculatedCRC ^= 0xFFFFFFFF;
    } else if (crcType == CRC_TYPE_EDB88320) {
        nCalculatedCRC = _getCRC32(pDevice, pPdStruct);
    } else if (crcType == CRC_TYPE_ADLER32) {
        nCalculatedCRC = getAdler32(pDevice, pPdStruct);
    } else {
        bCRCComputed = false;
    }

    if (bCRCComputed) {
        bResult = (nCalculatedCRC == nExpectedCRC);
    }

    return bResult;
}

quint32 XBinary::_getCRC32(qint64 nOffset, qint64 nSize, quint32 nInit, quint32 *pCRCTable, PDSTRUCT *pPdStruct)
{
    // TODO optimize!!!
    quint32 nResult = nInit;

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    if ((nOffset != -1) && (isPdStructNotCanceled(pPdStruct))) {
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        qint64 nTemp = 0;
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while (nSize > 0) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                setPdStructInfoString(pPdStruct, tr("Read error"));

                nResult = 0;

                break;
            }

            nResult = _getCRC32(pBuffer, nTemp, nResult, pCRCTable);

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset);
        }

        delete[] pBuffer;
    }

    nResult ^= 0xFFFFFFFF;

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if (isPdStructStopped(pPdStruct)) {
        nResult = 0;
    }

    return nResult;
}

quint16 XBinary::_getCRC16(qint64 nOffset, qint64 nSize, quint16 nInit, PDSTRUCT *pPdStruct)
{
    quint16 nResult = nInit;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    if ((nOffset != -1) && (!(pPdStruct->bIsStop))) {
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        qint64 nTemp = 0;
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while (nSize > 0) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                nResult = 0;

                break;
            }

            nResult = _getCRC16(pBuffer, nTemp, nResult, XBinary::_getCRC16Table());

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset);
        }

        delete[] pBuffer;
    }

    nResult ^= 0xFFFF;

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if (pPdStruct->bIsStop) {
        nResult = 0;
    }

    return nResult;
}

quint32 XBinary::_getCRC32ByFileContent(const QString &sFileName)
{
    return _getCRC32(readFile(sFileName), 0xFFFFFFFF, _getCRC32Table_EDB88320());
}

quint32 XBinary::_getCRC32ByDirectory(const QString &sDirectoryName, bool bRecursive, quint32 nInit)
{
    quint32 nResult = nInit;

    QDirIterator it(sDirectoryName);

    while (it.hasNext()) {
        QString sRecord = it.next();

        QFileInfo fi(sRecord);

        if (fi.isDir() && bRecursive) {
            nResult = _getCRC32ByDirectory(fi.absoluteFilePath(), bRecursive, nResult);
        }

        QByteArray baFileName = fi.baseName().toUtf8();
        nResult = _getCRC32(baFileName, nResult, _getCRC32Table_EDB88320());
    }

    return nResult;
}

double XBinary::getEntropy(const QString &sFileName, PDSTRUCT *pPdStruct)
{
    double dResult = 0;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        dResult = XBinary::getEntropy(&file, pPdStruct);

        file.close();
    }

    return dResult;
}

double XBinary::getEntropy(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    double dResult = 0;

    XBinary binary(pDevice);

    dResult = binary.getBinaryStatus(BSTATUS_ENTROPY, 0, -1, pPdStruct);

    pDevice->reset();

    return dResult;
}

double XBinary::getBinaryStatus(BSTATUS bstatus, qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    double dResult = 0;

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    if (nSize == 0) {
        dResult = 0;
    }

    bool bReadError = false;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    if ((nOffset != -1) && (!(pPdStruct->bIsStop))) {
        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        // Use integer histogram for entropy for better performance
        quint64 counts[256] = {0};
        qint64 nSymbolCount = 0;  // reused for ZEROS/TEXT
        quint64 nSum = 0;         // for GRADIENT

        qint64 nTemp = 0;
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                setPdStructErrorString(pPdStruct, tr("Read error"));
                bReadError = true;

                break;
            }

            if (bstatus == BSTATUS_ENTROPY) {
                const unsigned char *ptr = reinterpret_cast<const unsigned char *>(pBuffer);
                qint64 i = 0;

                // Simple loop - let compiler optimize (unrolling can hurt cache performance)
                for (; i < nTemp; i++) {
                    counts[ptr[i]]++;
                }
            } else if (bstatus == BSTATUS_ZEROS) {
                const unsigned char *ptr = reinterpret_cast<const unsigned char *>(pBuffer);
                for (qint64 i = 0; i < nTemp; i++) {
                    nSymbolCount += (ptr[i] == 0);
                }
            } else if (bstatus == BSTATUS_GRADIENT) {
                const unsigned char *ptr = reinterpret_cast<const unsigned char *>(pBuffer);
                for (qint64 i = 0; i < nTemp; i++) {
                    nSum += ptr[i];
                }
            } else if (bstatus == BSTATUS_TEXT) {
                const unsigned char *ptr = reinterpret_cast<const unsigned char *>(pBuffer);

                // Simple loop - compiler optimizes better than manual unrolling for this
                for (qint64 i = 0; i < nTemp; i++) {
                    unsigned char c = ptr[i];
                    // ASCII printable range [32..126] plus 8(BS),10(LF),13(CR)
                    if ((c >= 32 && c <= 126) || c == 8 || c == 10 || c == 13) {
                        nSymbolCount++;
                    }
                }
            }

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset - osRegion.nOffset);
        }

        delete[] pBuffer;

        if ((!(pPdStruct->bIsStop)) && (!bReadError)) {
            if (bstatus == BSTATUS_ENTROPY) {
                // H = -sum(p * log2 p)
                const double invLog2 = 1.4426950408889634073599246810023;  // 1/ln(2)
                const double N = (double)osRegion.nSize;
                for (qint32 j = 0; j < 256; j++) {
                    if (counts[j]) {
                        double p = (double)counts[j] / N;
                        dResult += -p * (log(p) * invLog2);
                    }
                }
            } else if (bstatus == BSTATUS_ZEROS) {
                dResult = (double)nSymbolCount / (double)(osRegion.nSize);
            } else if (bstatus == BSTATUS_GRADIENT) {
                dResult = (double)nSum / ((double)(osRegion.nSize) * (double)0xFF);
            } else if (bstatus == BSTATUS_TEXT) {
                dResult = (double)nSymbolCount / (double)(osRegion.nSize);
            }
        }
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if (pPdStruct->bIsStop) {
        dResult = 0;
    }

    return dResult;
}

bool XBinary::isZeroFilled(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    if (nSize <= 0) {
        return false;
    }

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);
    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    if (nSize == 0) {
        return false;
    }

    bool bResult = true;
    qint32 nBufferSize = getBufferSize(pPdStruct);
    char *pBuffer = new char[nBufferSize];

    while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
        qint64 nTemp = qMin((qint64)nBufferSize, nSize);

        if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
            bResult = false;
            break;
        }

        if (!_isMemoryZeroFilled(pBuffer, nTemp)) {
            bResult = false;
            break;
        }

        nSize -= nTemp;
        nOffset += nTemp;
    }

    delete[] pBuffer;

    if (pPdStruct->bIsStop) {
        bResult = false;
    }

    return bResult;
}

XBinary::BYTE_COUNTS XBinary::getByteCounts(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    BYTE_COUNTS result = {};

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    bool bReadError = false;

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);

    if ((nOffset != -1) && (!(pPdStruct->bIsStop))) {
        result.nSize = nSize;

        XBinary::setPdStructInit(pPdStruct, _nFreeIndex, nSize);

        qint64 nTemp = 0;
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
            nTemp = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                _errorMessage(tr("Read error"));

                bReadError = true;

                break;
            }

            for (qint64 i = 0; i < nTemp; i++) {
                result.nCount[(unsigned char)pBuffer[i]] += 1;
            }

            nSize -= nTemp;
            nOffset += nTemp;

            XBinary::setPdStructCurrent(pPdStruct, _nFreeIndex, nOffset - osRegion.nOffset);
        }

        delete[] pBuffer;
    }

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);

    if ((pPdStruct->bIsStop) || (bReadError)) {
        result = BYTE_COUNTS();
    }

    return result;
}

void XBinary::_xor(quint8 nXorValue, qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    // TODO Optimize
    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    if (nOffset != -1) {
        if (nOffset != -1) {
            qint64 nTemp = 0;
            qint32 nBufferSize = getBufferSize(pPdStruct);
            char *pBuffer = new char[nBufferSize];

            while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
                nTemp = qMin((qint64)nBufferSize, nSize);

                if (read_array(nOffset, pBuffer, nTemp) != nTemp) {
                    _errorMessage(tr("Read error"));
                    break;
                }

                for (qint32 i = 0; i < nTemp; i++) {
                    *(pBuffer + i) ^= nXorValue;
                }

                if (!write_array(nOffset, pBuffer, nTemp)) {
                    _errorMessage(tr("Write error"));
                    break;
                }

                nSize -= nTemp;
                nOffset += nTemp;
            }

            delete[] pBuffer;
        }
    }
}

// quint32 XBinary::_ror32(quint32 nValue, quint32 nShift)
//{
//     // TODO Check
//     nShift&=(31);
//     return (nValue>>nShift)|(nValue<<((-nShift)&31));
// }

// quint32 XBinary::_rol32(quint32 nValue, quint32 nShift)
//{
//     // TODO Check
//     nShift&=(31);
//     return (nValue<<nShift)|(nValue>>((-nShift)&31));
// }

quint32 XBinary::getStringCustomCRC32(const QString &sString)
{
    quint32 nResult = 0;  // not ~0 !!! if ~0 (0xFFFFFFFF) it will be a CRC32C

    qint32 nSize = sString.size();
    QByteArray baString = sString.toUtf8();

    for (qint32 i = 0; i < nSize; i++) {
        unsigned char _char = (unsigned char)baString.data()[i];
        //        unsigned char _char1=(unsigned char)sString.at(i).toLatin1();

        //        if(_char!=_char1)
        //        {
        //            qFatal("Error"); // TODO remove
        //        }

        nResult ^= _char;

        for (qint32 k = 0; k < 8; k++) {
            nResult = (nResult & 1) ? ((nResult >> 1) ^ 0x82f63b78) : (nResult >> 1);
        }
    }

    nResult = ~nResult;

    return nResult;
}

QIODevice *XBinary::getDevice()
{
    return m_pDevice;
}

bool XBinary::isValid(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return true;
}

bool XBinary::isValid(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress, PDSTRUCT *pPdStruct)
{
    XBinary xbinary(pDevice, bIsImage, nModuleAddress);

    return xbinary.isValid();
}

XBinary::MODE XBinary::getMode(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    XBinary xbinary(pDevice, bIsImage, nModuleAddress);

    return xbinary.getMode();
}

bool XBinary::isBigEndian()
{
    return (getEndian() == ENDIAN_BIG);
}

bool XBinary::isLittleEndian()
{
    return (getEndian() == ENDIAN_LITTLE);
}

bool XBinary::is16()
{
    MODE mode = getMode();

    return ((mode == MODE_16) || (mode == MODE_16SEG));
}

bool XBinary::is32()
{
    MODE mode = getMode();

    return (mode == MODE_32);
}

bool XBinary::is64()
{
    MODE mode = getMode();

    return (mode == MODE_64);
}

bool XBinary::isBigEndian(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->endian == ENDIAN_BIG);
}

bool XBinary::isLittleEndian(_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->endian == ENDIAN_LITTLE);
}

bool XBinary::is16(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->mode == MODE_16) || (pMemoryMap->mode == MODE_16SEG);
}

bool XBinary::is32(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->mode == MODE_32);
}

bool XBinary::is64(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return (pMemoryMap->mode == MODE_64);
}

void XBinary::setVersion(const QString &sVersion)
{
    m_sVersion = sVersion;
}

void XBinary::setOptions(const QString &sOptions)
{
    m_sOptions = sOptions;
}

QString XBinary::getVersion()
{
    return m_sVersion;
}

QString XBinary::getInfo(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    return m_sOptions;
}

bool XBinary::isEncrypted()
{
    return false;
}

bool XBinary::isCommentPresent()
{
    return false;
}

QString XBinary::getComment()
{
    return QString();
}

QString XBinary::getSignature(QIODevice *pDevice, qint64 nOffset, qint64 nSize)
{
    XBinary binary(pDevice);

    return binary.getSignature(nOffset, nSize);
}

QString XBinary::getSignature(qint64 nOffset, qint64 nSize)
{
    QString sResult;

    if (nOffset != -1) {
        OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, -1);

        nSize = qMin(osRegion.nSize, nSize);

        sResult = read_array(nOffset, nSize).toHex().toUpper();
    }

    return sResult;
}

XBinary::OFFSETSIZE XBinary::convertOffsetAndSize(qint64 nOffset, qint64 nSize)
{
    OFFSETSIZE osResult = {};

    osResult.nOffset = -1;
    osResult.nSize = 0;

    qint64 nTotalSize = getSize();

    if ((nSize == -1) && (nTotalSize > nOffset)) {
        nSize = nTotalSize - nOffset;
    }

    if ((nOffset + nSize > nTotalSize) && (nOffset < nTotalSize)) {
        nSize = nTotalSize - nOffset;
    }

    if ((nSize > 0) && (nOffset >= 0) && (nOffset < nTotalSize) && (nOffset + nSize - 1 < nTotalSize)) {
        osResult.nOffset = nOffset;
        osResult.nSize = nSize;
    }

    return osResult;
}

XBinary::OFFSETSIZE XBinary::convertOffsetAndSize(QIODevice *pDevice, qint64 nOffset, qint64 nSize)
{
    XBinary binary(pDevice);

    return binary.convertOffsetAndSize(nOffset, nSize);
}

bool XBinary::compareSignatureStrings(const QString &sBaseSignature, const QString &sOptSignature)
{
    bool bResult = false;
    // TODO optimize
    // TODO check
    QString _sBaseSignature = convertSignature(sBaseSignature);
    QString _sOptSignature = convertSignature(sOptSignature);

    qint32 nSize = qMin(_sBaseSignature.size(), _sOptSignature.size());

    if ((nSize) && (_sBaseSignature.size() >= _sOptSignature.size())) {
        bResult = true;

        for (qint32 i = 0; i < nSize; i++) {
            QChar _qchar1 = _sBaseSignature.at(i);
            QChar _qchar2 = _sOptSignature.at(i);

            if ((_qchar1 != QChar('.')) && (_qchar2 != QChar('.'))) {
                if (_qchar1 != _qchar2) {
                    bResult = false;

                    break;
                }
            }
        }
    }

    return bResult;
}

void XBinary::_errorMessage(const QString &sErrorMessage, PDSTRUCT *pPdStruct)
{
#ifdef QT_DEBUG
    QFile *pFile = dynamic_cast<QFile *>(m_pDevice);

    if (pFile) {
        qDebug("Filename: %s", pFile->fileName().toUtf8().data());
    }

    qDebug("Error: %s", sErrorMessage.toLatin1().data());
#endif
    setPdStructInfoString(pPdStruct, sErrorMessage);
    emit errorMessage(sErrorMessage);
}

void XBinary::_infoMessage(const QString &sInfoMessage, PDSTRUCT *pPdStruct)
{
    setPdStructInfoString(pPdStruct, sInfoMessage);
#ifdef QT_DEBUG
    qDebug("Info: %s", sInfoMessage.toLatin1().data());
#endif
    emit infoMessage(sInfoMessage);
}

qint64 XBinary::_calculateRawSize(PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return _calculateRawSize(&memoryMap, pPdStruct);
}

qint64 XBinary::_calculateRawSize(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint64 nResult = 0;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    qint64 _nOverlayOffset = -1;

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        if ((pMemoryMap->listRecords.at(i).nOffset != -1) && (pMemoryMap->listRecords.at(i).filePart != FILEPART_OVERLAY)) {
            nResult = qMax(nResult, (qint64)(pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize));
        }

        if (pMemoryMap->listRecords.at(i).filePart == FILEPART_OVERLAY) {
            _nOverlayOffset = pMemoryMap->listRecords.at(i).nOffset;
        }
    }

    if (_nOverlayOffset != -1) {
        nResult = qMin(nResult, _nOverlayOffset);
    }

    return nResult;
}

QString XBinary::convertSignature(const QString &sSignature)
{
    QString _sSignature;
    // 'AnsiString'
    // TODO more defs

    qint32 nSignatureSize = sSignature.size();
    bool bHasQuote = false;

    // First pass: detect what conversions are needed and pre-allocate
    for (qint32 i = 0; i < nSignatureSize; i++) {
        QChar c = sSignature.at(i);
        if (c == QChar(39)) {
            bHasQuote = true;
            break;
        }
    }

    // Reserve appropriate capacity
    if (bHasQuote) {
        _sSignature.reserve(nSignatureSize * 2);  // Worst case: all chars become hex
    } else {
        _sSignature.reserve(nSignatureSize);
    }

    if (bHasQuote) {
        bool bAnsiString = false;

        for (qint32 i = 0; i < nSignatureSize; i++) {
            QChar c = sSignature.at(i);

            if (c == QChar(39)) {
                bAnsiString = !bAnsiString;
            } else if (bAnsiString) {
                // Inline hex conversion for better performance
                quint8 nValue = (quint8)(c.toLatin1());
                quint8 nHigh = (nValue >> 4) & 0x0F;
                quint8 nLow = nValue & 0x0F;
                _sSignature.append(QChar(nHigh + (nHigh < 10 ? '0' : 'a' - 10)));
                _sSignature.append(QChar(nLow + (nLow < 10 ? '0' : 'a' - 10)));
            } else {
                if (c != QChar(' ')) {  // Skip spaces while we're at it
                    if (c == QChar('?')) {
                        _sSignature.append(QChar('.'));
                    } else {
                        _sSignature.append(c.toLower());
                    }
                }
            }
        }
    } else {
        // Simpler path when no quotes present
        for (qint32 i = 0; i < nSignatureSize; i++) {
            QChar c = sSignature.at(i);

            if (c != QChar(' ')) {  // Skip spaces
                if (c == QChar('?')) {
                    _sSignature.append(QChar('.'));
                } else {
                    _sSignature.append(c.toLower());
                }
            }
        }
    }

    return _sSignature;
}

bool XBinary::isDebugBuild()
{
    return false;
}

bool XBinary::isReleaseBuild()
{
    return true;
}

QList<QString> XBinary::getFileFormatMessages(const QList<FMT_MSG> *pListFmtMsg)
{
    QList<QString> listResult;

    if (pListFmtMsg) {
        qint32 nNumberOfRecords = pListFmtMsg->count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            QString sRecord;

            if (pListFmtMsg->at(i).type == FMT_MSG_TYPE_INFO) sRecord += QString("[%1]").arg(tr("Info"));
            else if (pListFmtMsg->at(i).type == FMT_MSG_TYPE_WARNING) sRecord += QString("[%1]").arg(tr("Warning"));
            else if (pListFmtMsg->at(i).type == FMT_MSG_TYPE_ERROR) sRecord += QString("[%1]").arg(tr("Error"));

            sRecord += QString("(%1) ").arg(pListFmtMsg->at(i).code, 4, 16, QChar('0'));

            sRecord += pListFmtMsg->at(i).sString;
            listResult.append(sRecord);
        }
    }

    return listResult;
}

bool XBinary::isFmtMsgCodePresent(const QList<FMT_MSG> *pListFmtMsgs, FMT_MSG_CODE code, FMT_MSG_TYPE type, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (pListFmtMsgs) {
        qint32 nNumberOfRecords = pListFmtMsgs->count();

        for (qint32 i = 0; (i < nNumberOfRecords) && isPdStructNotCanceled(pPdStruct); i++) {
            if ((pListFmtMsgs->at(i).code == code) && (pListFmtMsgs->at(i).type == type)) {
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}

bool XBinary::_addCheckFormatTest(QList<FMT_MSG> *pListFmtMsgs, bool *pbContinue, FMT_MSG_CODE code, FMT_MSG_TYPE type, const QString &sString, QVariant value,
                                  QString sInfo, bool bFailCase)
{
    bool bResult = !bFailCase;

    if (*pbContinue) {
        if (bFailCase) {
            FMT_MSG record = {};
            record.type = type;
            record.code = code;
            record.sString += QString("%1: %2: %3").arg(sString, tr("Corrupted data"), sInfo);
            record.value = value;

            pListFmtMsgs->append(record);

            if (type == FMT_MSG_TYPE_ERROR) {
                *pbContinue = false;
            }
        }
    }

    return bResult;
}

void XBinary::dumpMemoryMap()
{
#ifdef QT_DEBUG
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN);

    qDebug("%s", memoryMap.bIsImage ? "Image" : "File");
    qDebug("Binary Size: %s", valueToHex(memoryMap.nBinarySize).toLatin1().data());
    qDebug("Image Size: %s", valueToHex(memoryMap.nImageSize).toLatin1().data());
    qDebug("Module Address: %s", valueToHex(memoryMap.nModuleAddress).toLatin1().data());
    qDebug("EntryPoint Address: %s", valueToHex(memoryMap.nEntryPointAddress).toLatin1().data());
    qDebug("File Type: %s", fileTypeIdToString(memoryMap.fileType).toLatin1().data());
    qDebug("Mode: %s", modeIdToString(memoryMap.mode).toLatin1().data());
    qDebug("Endian: %s", endianToString(memoryMap.endian).toLatin1().data());
    qDebug("Arch: %s", memoryMap.sArch.toLatin1().data());
    qDebug("Type: %s", memoryMap.sType.toLatin1().data());

    qint32 nNumberOfRecords = memoryMap.listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        qDebug("--------------------------------------------------");
        qDebug("Record: %d", i);
        qDebug("Index: %d", memoryMap.listRecords.at(i).nIndex);
        qDebug("Offset: %lld", memoryMap.listRecords.at(i).nOffset);
        qDebug("Address: %lld", memoryMap.listRecords.at(i).nAddress);
        // qDebug("Segment: %s", addressSegmentToString(memoryMap.listRecords.at(i).segment).toLatin1().data());
        qDebug("Size: %lld", memoryMap.listRecords.at(i).nSize);
        // qDebug("Type: %s", mmtToString(memoryMap.listRecords.at(i).type).toLatin1().data());
        qDebug("LoadSectionNumber: %d", memoryMap.listRecords.at(i).nFilePartNumber);
        qDebug("Name: %s", memoryMap.listRecords.at(i).sName.toLatin1().data());
        qDebug("IsVirtual: %s", memoryMap.listRecords.at(i).bIsVirtual ? "true" : "false");
        qDebug("IsInvisible: %s", memoryMap.listRecords.at(i).bIsInvisible ? "true" : "false");
        // qDebug("ID: %lld", memoryMap.listRecords.at(i).nID);
        qDebug("--------------------------------------------------");
    }

#endif
}

void XBinary::dumpHeaders()
{
#ifdef QT_DEBUG
    XBinary::_MEMORY_MAP memoryMap = getMemoryMap();

    XBinary::DATA_HEADERS_OPTIONS dataHeaderOptions = {};
    dataHeaderOptions.locType = XBinary::LT_OFFSET;
    dataHeaderOptions.nLocation = 0;
    dataHeaderOptions.pMemoryMap = &memoryMap;
    dataHeaderOptions.nID = 0;

    QList<XBinary::DATA_HEADER> listHeaders = getDataHeaders(dataHeaderOptions, nullptr);

    qint32 nNumberOfHeaders = listHeaders.count();

    for (qint32 i = 0; i < nNumberOfHeaders; i++) {
        XBinary::DATA_HEADER dataHeader = listHeaders.at(i);

        if (dataHeader.dsID.fileType == getFileType()) {
            qDebug("%s: %X", structIDToString(dataHeader.dsID.nID).toLatin1().data(), locationToOffset(&memoryMap, dataHeader.locType, dataHeader.nLocation));

            QList<XBinary::DATA_RECORD_ROW> listDataRecordsRow;

            XBinary::DATA_RECORDS_OPTIONS dataRecordsOptions = {};
            dataRecordsOptions.pMemoryMap = &memoryMap;
            dataRecordsOptions.dataHeaderFirst = dataHeader;

            getDataRecordValues(dataRecordsOptions, &listDataRecordsRow, nullptr, nullptr);
            QList<QString> listComments;

            if (listDataRecordsRow.count() > 0) {
                XBinary::DATA_RECORD_ROW dataRecordRow = listDataRecordsRow.at(0);
                listComments = getDataRecordComments(dataRecordsOptions, dataRecordRow, nullptr);
            }

            qint32 nNumberOfRecords = dataHeader.listRecords.count();

            for (qint32 j = 0; j < nNumberOfRecords; j++) {
                XBinary::DATA_RECORD dataRecord = dataHeader.listRecords.at(j);

                qDebug("%X: %X %s %s %s %s", dataRecord.nRelOffset, dataRecord.nSize, XBinary::valueTypeToString(dataRecord.valType, dataRecord.nSize).toLatin1().data(),
                       dataRecord.sName.toLatin1().data(), XBinary::getValueString(listDataRecordsRow.at(0).listValues.at(j), dataRecord.valType, true).toLatin1().data(),
                       listComments.at(j).toLatin1().data());
            }
        } else if (dataHeader.dsID.fileType == FT_BINARY) {
            qDebug("%s: %X", XBinary::structIDToString(dataHeader.dsID.nID).toLatin1().data(), 0);
        }
    }
#endif
}

XBinary::FPART XBinary::getFPART(FILEPART filePart, const QString &sOriginalName, qint64 nFileOffset, qint64 nFileSize, XADDR nVirtualAddress, qint64 nVirtualSize)
{
    XBinary::FPART fpart = {};

    fpart.filePart = filePart;
    fpart.sName = sOriginalName;
    fpart.nFileOffset = nFileOffset;
    fpart.nFileSize = nFileSize;
    fpart.nVirtualAddress = nVirtualAddress;
    fpart.nVirtualSize = nVirtualSize;

    return fpart;
}

QList<XBinary::FPART> XBinary::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    QList<XBinary::FPART> listResult;

    if (nFileParts & FILEPART_REGION) {
        XBinary::FPART fpart = {};
        fpart.nFileOffset = 0;
        fpart.nFileSize = getSize();
        fpart.sName = tr("Data");
        fpart.filePart = FILEPART_REGION;

        listResult.append(fpart);
    }

    return listResult;
}

QString XBinary::getMIMEString()
{
    return "application/octet-stream";
}

QList<XBinary::FMT_MSG> XBinary::checkFileFormat(bool bDeep, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(bDeep)
    Q_UNUSED(pPdStruct)

    QList<XBinary::FMT_MSG> listResult;

    return listResult;
}

bool XBinary::isFileFormatValid(bool bDeep, PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    QList<FMT_MSG> list = checkFileFormat(bDeep, pPdStruct);

    qint32 nNumberOfRecords = list.count();

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        if (list.at(i).type == FMT_MSG_TYPE_ERROR) {
            bResult = false;
            break;
        }
    }

    return bResult;
}

QList<XBinary::STRINGTABLE_RECORD> XBinary::getStringTable_ANSI(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct)
{
    QList<XBinary::STRINGTABLE_RECORD> listResult;

    QByteArray baStringTable = read_array_process(nOffset, nSize, pPdStruct);

    char *_pOffset = baStringTable.data();
    qint32 _nSize = baStringTable.size();

    // TODO UTF8
    for (qint32 i = 0; i < _nSize; i++) {
        XBinary::STRINGTABLE_RECORD record = {};

        record.nOffsetFromStart = i;
        record.sString = _pOffset;  // TODO
        record.nSizeInBytes = record.sString.size();

        listResult.append(record);

        _pOffset += (record.nSizeInBytes + 1);
        i += record.nSizeInBytes;
    }

    return listResult;
}

QString XBinary::qcharToHex(QChar cSymbol)
{
    // TODO mb
    return QString("%1").arg((quint8)(cSymbol.toLatin1()), 2, 16, QChar('0'));
}

QString XBinary::stringToHex(const QString &sString)
{
    return QString::fromLatin1(sString.toLatin1().toHex());
}

QString XBinary::hexToString(const QString &sHex)
{
    QString sResult;

    sResult = QByteArray::fromHex(sHex.toLatin1().data());

    return sResult;
}

QString XBinary::floatToString(float fValue, qint32 nPrec)
{
    return QString("%1").arg(fValue, 0, 'f', nPrec);
}

QString XBinary::doubleToString(double dValue, qint32 nPrec)
{
    return QString("%1").arg(dValue, 0, 'f', nPrec);
}

quint8 XBinary::hexToUint8(const QString &sHex)
{
    QString _sHex = sHex;
    quint8 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(quint8)) {
        _sHex = _sHex.mid(0, 2 * sizeof(quint8));
        bool bStatus = false;
        nResult = (quint8)(_sHex.toInt(&bStatus, 16));
    }

    return nResult;
}

qint8 XBinary::hexToInt8(const QString &sHex)
{
    QString _sHex = sHex;
    quint8 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(qint8)) {
        _sHex = _sHex.mid(0, 2 * sizeof(qint8));
        bool bStatus = false;
        nResult = (qint8)(_sHex.toInt(&bStatus, 16));
    }

    return nResult;
}

quint16 XBinary::hexToUint16(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    quint16 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(quint16)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(quint16)));
        }

        bool bStatus = false;
        nResult = _sHex.toUShort(&bStatus, 16);
    }

    return nResult;
}

qint16 XBinary::hexToInt16(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    qint16 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(qint16)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(qint16)));
        }

        bool bStatus = false;
        nResult = _sHex.toShort(&bStatus, 16);
    }

    return nResult;
}

quint32 XBinary::hexToUint32(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    quint32 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(quint32)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(quint32)));
        }

        bool bStatus = false;
        nResult = _sHex.toUInt(&bStatus, 16);
    }

    return nResult;
}

qint32 XBinary::hexToInt32(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    qint32 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(qint32)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(qint32)));
        }

        bool bStatus = false;
        nResult = _sHex.toInt(&bStatus, 16);
    }

    return nResult;
}

quint64 XBinary::hexToUint64(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    quint64 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(quint64)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(quint64)));
        }

        bool bStatus = false;
        nResult = _sHex.toULongLong(&bStatus, 16);
    }

    return nResult;
}

qint64 XBinary::hexToInt64(const QString &sHex, bool bIsBigEndian)
{
    QString _sHex = sHex;
    qint64 nResult = 0;

    if ((quint32)_sHex.length() >= sizeof(qint64)) {
        if (!bIsBigEndian) {
            _sHex = invertHexByteString(_sHex.mid(0, 2 * sizeof(qint64)));
        }

        bool bStatus = false;
        nResult = _sHex.toLongLong(&bStatus, 16);
    }

    return nResult;
}

QString XBinary::invertHexByteString(const QString &sHex)
{
    QString sResult;

    for (qint32 i = sHex.length() - 2; i >= 0; i -= 2) {
        sResult += sHex.mid(i, 2);
    }

    return sResult;
}

void XBinary::_swapBytes(char *pSource, qint32 nSize)
{
    for (qint32 i = 0; i < (nSize / 2); i++) {
        char cTemp = pSource[i];
        pSource[i] = pSource[(nSize - 1) - i];
        pSource[(nSize - 1) - i] = cTemp;
    }
}

quint16 XBinary::swapBytes(quint16 nValue)
{
    _swapBytes((char *)&nValue, 2);

    return nValue;
}

quint32 XBinary::swapBytes(quint32 nValue)
{
    _swapBytes((char *)&nValue, 4);

    return nValue;
}

quint64 XBinary::swapBytes(quint64 nValue)
{
    _swapBytes((char *)&nValue, 8);

    return nValue;
}

bool XBinary::isPlainTextType()
{
    // Read a larger sample for better text detection accuracy
    QByteArray baData = read_array(0, qMin(getSize(), (qint64)0x8000));  // Increased from 0x2000 to 0x8000

    return isPlainTextType(&baData);
}

bool XBinary::isPlainTextType(QByteArray *pbaData)
{
    if (!pbaData || pbaData->isEmpty()) {
        return false;
    }

    qint32 nDataSize = pbaData->size();
    const unsigned char *pDataOffset = (const unsigned char *)(pbaData->constData());

    // Check for BOM markers - if present, not ANSI
    if (nDataSize >= 3 && pDataOffset[0] == 0xEF && pDataOffset[1] == 0xBB && pDataOffset[2] == 0xBF) {
        return false;  // UTF-8 BOM
    }

    // Check for UTF-16 BOM
    if (nDataSize >= 2) {
        if ((pDataOffset[0] == 0xFF && pDataOffset[1] == 0xFE) || (pDataOffset[0] == 0xFE && pDataOffset[1] == 0xFF)) {
            return false;  // UTF-16 BOM
        }
    }

    qint32 nNullCount = 0;
    qint32 nControlCount = 0;
    qint32 nPrintableCount = 0;
    qint32 nExtendedCount = 0;

    // Analyze character distribution for ANSI compatibility
    for (qint32 i = 0; i < nDataSize; i++) {
        unsigned char byte = pDataOffset[i];

        if (byte == 0) {
            nNullCount++;
            // Any null bytes indicate binary data
            if (nNullCount > 0) {
                return false;
            }
        } else if (byte < 0x09) {
            // Control characters (excluding null)
            nControlCount++;
        } else if (byte == 0x09 || byte == 0x0A || byte == 0x0D) {
            // Tab, LF, CR - common whitespace
            nPrintableCount++;
        } else if (byte >= 0x20 && byte <= 0x7E) {
            // Standard ASCII printable characters
            nPrintableCount++;
        } else if (byte >= 0x80 && byte <= 0xFF) {
            // Extended ASCII (ANSI code page characters)
            nExtendedCount++;
        } else {
            // Other control characters (0x0B, 0x0C, 0x0E-0x1F)
            nControlCount++;
        }
    }

    // Calculate ratios for ANSI text detection
    double printableRatio = (double)(nPrintableCount + nExtendedCount) / nDataSize;
    double controlRatio = (double)nControlCount / nDataSize;
    double extendedRatio = (double)nExtendedCount / nDataSize;

    // ANSI text should have:
    // - High printable ratio (including extended ASCII)
    // - Low control character ratio
    // - Extended ASCII allowed but not dominant
    return (printableRatio >= 0.85 && controlRatio <= 0.05 && extendedRatio <= 0.50);
}

bool XBinary::isUTF8TextType()
{
    QByteArray baData = read_array(0, qMin(getSize(), (qint64)0x2000));  // Larger sample for better detection

    return isUTF8TextType(&baData);
}

bool XBinary::isUTF8TextType(QByteArray *pbaData)
{
    if (!pbaData || pbaData->isEmpty()) {
        return false;
    }

    const unsigned char *pDataOffset = (const unsigned char *)(pbaData->constData());
    qint32 nDataSize = pbaData->size();

    // Check for UTF-8 BOM
    bool bHasBOM = false;
    qint32 nStartOffset = 0;

    if (nDataSize >= 3 && pDataOffset[0] == 0xEF && pDataOffset[1] == 0xBB && pDataOffset[2] == 0xBF) {
        bHasBOM = true;
        nStartOffset = 3;
    }

    // Validate UTF-8 encoding from start offset
    qint32 nValidChars = 0;
    qint32 nMultiByteChars = 0;
    qint32 nPrintableChars = 0;
    qint32 nNullBytes = 0;

    for (qint32 i = nStartOffset; i < nDataSize;) {
        unsigned char byte = pDataOffset[i];

        if (byte == 0) {
            nNullBytes++;
            // UTF-8 text shouldn't contain null bytes
            if (nNullBytes > 0) {
                return false;
            }
            i++;
        } else if (byte < 0x80) {
            // ASCII character (0x00-0x7F)
            if (byte >= 0x20 || byte == 0x09 || byte == 0x0A || byte == 0x0D) {
                nPrintableChars++;
            }
            nValidChars++;
            i++;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2-byte UTF-8 sequence (110xxxxx 10xxxxxx)
            if (i + 1 >= nDataSize || (pDataOffset[i + 1] & 0xC0) != 0x80) {
                return false;  // Invalid UTF-8 sequence
            }
            // Check for overlong encoding
            if (byte < 0xC2) {
                return false;  // Overlong 2-byte sequence
            }
            nMultiByteChars++;
            nValidChars++;
            i += 2;
        } else if ((byte & 0xF0) == 0xE0) {
            // 3-byte UTF-8 sequence (1110xxxx 10xxxxxx 10xxxxxx)
            if (i + 2 >= nDataSize || (pDataOffset[i + 1] & 0xC0) != 0x80 || (pDataOffset[i + 2] & 0xC0) != 0x80) {
                return false;  // Invalid UTF-8 sequence
            }
            // Check for overlong encoding
            if (byte == 0xE0 && pDataOffset[i + 1] < 0xA0) {
                return false;  // Overlong 3-byte sequence
            }
            nMultiByteChars++;
            nValidChars++;
            i += 3;
        } else if ((byte & 0xF8) == 0xF0) {
            // 4-byte UTF-8 sequence (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            if (i + 3 >= nDataSize || (pDataOffset[i + 1] & 0xC0) != 0x80 || (pDataOffset[i + 2] & 0xC0) != 0x80 || (pDataOffset[i + 3] & 0xC0) != 0x80) {
                return false;  // Invalid UTF-8 sequence
            }
            // Check for overlong encoding and valid Unicode range
            if (byte == 0xF0 && pDataOffset[i + 1] < 0x90) {
                return false;  // Overlong 4-byte sequence
            }
            if (byte > 0xF4 || (byte == 0xF4 && pDataOffset[i + 1] > 0x8F)) {
                return false;  // Beyond valid Unicode range
            }
            nMultiByteChars++;
            nValidChars++;
            i += 4;
        } else {
            // Invalid UTF-8 start byte
            return false;
        }
    }

    // For UTF-8 detection, we need either:
    // 1. UTF-8 BOM present, or
    // 2. Valid UTF-8 sequences with some multi-byte characters and high printable ratio
    if (bHasBOM) {
        return nValidChars > 0;  // BOM present and valid UTF-8 content
    } else if (nValidChars > 0) {
        double printableRatio = (double)nPrintableChars / nValidChars;
        double multiByteRatio = (double)nMultiByteChars / nValidChars;

        // Require some multi-byte characters and high printable ratio for UTF-8 without BOM
        return (multiByteRatio > 0.05 && printableRatio >= 0.70);
    }

    return false;
}

bool XBinary::isPlainTextType(QIODevice *pDevice)
{
    XBinary binary(pDevice);

    return binary.isPlainTextType();
}

XBinary::UNICODE_TYPE XBinary::getUnicodeType()
{
    QByteArray baData = read_array(0, qMin(getSize(), (qint64)0x1000));  // Larger sample for better detection

    return getUnicodeType(&baData);
}

XBinary::UNICODE_TYPE XBinary::getUnicodeType(QByteArray *pbaData)
{
    if (!pbaData || pbaData->isEmpty()) {
        return XBinary::UNICODE_TYPE_NONE;
    }

    const unsigned char *pDataOffset = (const unsigned char *)(pbaData->constData());
    qint32 nDataSize = pbaData->size();

    // Check for BOM first (most reliable method)
    if (nDataSize >= 2) {
        quint16 nSymbol = qFromLittleEndian(*((quint16 *)(pDataOffset)));

        if (nSymbol == 0xFFFE) {
            return UNICODE_TYPE_BE;  // UTF-16 Big Endian BOM
        } else if (nSymbol == 0xFEFF) {
            return UNICODE_TYPE_LE;  // UTF-16 Little Endian BOM
        }
    }

    // If no BOM, try to detect by content analysis
    if (nDataSize >= 4) {
        qint32 nNullCount = 0;
        qint32 nEvenNulls = 0;  // Null bytes at even positions
        qint32 nOddNulls = 0;   // Null bytes at odd positions
        qint32 nPrintableCount = 0;
        qint32 nValidChars = 0;

        // Analyze first part of data for patterns
        qint32 nSampleSize = qMin(nDataSize, 512);
        for (qint32 i = 0; i < nSampleSize; i++) {
            unsigned char byte = pDataOffset[i];

            if (byte == 0) {
                nNullCount++;
                if (i % 2 == 0) {
                    nEvenNulls++;
                } else {
                    nOddNulls++;
                }
            } else if (byte >= 0x20 && byte <= 0x7E) {
                nPrintableCount++;
            }

            nValidChars++;
        }

        // UTF-16 typically has null bytes in alternating positions
        if (nNullCount > 0 && nValidChars > 4) {
            double nullRatio = (double)nNullCount / nValidChars;
            double printableRatio = (double)nPrintableCount / nValidChars;

            // For UTF-16, expect significant null presence and reasonable printable content
            if (nullRatio >= 0.30 && printableRatio >= 0.30) {
                // Determine endianness by null byte pattern
                if (nEvenNulls > nOddNulls * 2) {
                    // More nulls at even positions suggests UTF-16 LE (ASCII chars have null high byte)
                    return UNICODE_TYPE_LE;
                } else if (nOddNulls > nEvenNulls * 2) {
                    // More nulls at odd positions suggests UTF-16 BE
                    return UNICODE_TYPE_BE;
                }

                // If pattern is unclear, try byte pair analysis
                qint32 nLELikelyPairs = 0;
                qint32 nBELikelyPairs = 0;

                for (qint32 i = 0; i < nSampleSize - 1; i += 2) {
                    quint16 wChar = qFromLittleEndian(*((quint16 *)(pDataOffset + i)));

                    // Check if it's a likely ASCII character in LE format
                    if ((wChar & 0xFF00) == 0 && (wChar & 0xFF) >= 0x20 && (wChar & 0xFF) <= 0x7E) {
                        nLELikelyPairs++;
                    }

                    // Check if it's a likely ASCII character in BE format
                    wChar = qFromBigEndian(*((quint16 *)(pDataOffset + i)));
                    if ((wChar & 0xFF00) == 0 && (wChar & 0xFF) >= 0x20 && (wChar & 0xFF) <= 0x7E) {
                        nBELikelyPairs++;
                    }
                }

                if (nLELikelyPairs > nBELikelyPairs) {
                    return UNICODE_TYPE_LE;
                } else if (nBELikelyPairs > nLELikelyPairs) {
                    return UNICODE_TYPE_BE;
                }
            }
        }
    }

    return XBinary::UNICODE_TYPE_NONE;
}

bool XBinary::tryToOpen(QIODevice *pDevice)
{
    bool bResult = false;
    bool bCheck = false;

    QFile *pFile = dynamic_cast<QFile *>(pDevice);

    if (pFile) {
        bCheck = (pFile->fileName() != "");
    } else {
        bCheck = true;
    }

    if (bCheck) {
        bResult = pDevice->open(QIODevice::ReadWrite);

        if (!bResult) {
            bResult = pDevice->open(QIODevice::ReadOnly);
        }
    }

    return bResult;
}

bool XBinary::checkOffsetSize(XBinary::OFFSETSIZE osRegion)
{
    qint64 nTotalSize = getSize();

    bool bOffsetValid = (osRegion.nOffset >= 0) && (osRegion.nOffset < nTotalSize);
    bool bSizeValid = (osRegion.nSize > 0) && ((osRegion.nOffset + osRegion.nSize - 1) < nTotalSize);

    return (bOffsetValid) && (bSizeValid);
}

bool XBinary::checkOffsetSize(qint64 nOffset, qint64 nSize)
{
    XBinary::OFFSETSIZE os = {};
    os.nOffset = nOffset;
    os.nSize = nSize;

    return checkOffsetSize(os);
}

QString XBinary::get_uint8_full_version(quint8 nValue)
{
    return QString("%1").arg(QString::number((nValue) & 0xFF));
}

QString XBinary::get_uint16_full_version(quint16 nValue)
{
    return QString("%1.%2").arg(QString::number((nValue >> 8) & 0xFF), QString::number((nValue) & 0xFF));
}

QString XBinary::get_uint32_full_version(quint32 nValue)
{
    return QString("%1.%2.%3").arg(QString::number((nValue >> 16) & 0xFFFF), QString::number((nValue >> 8) & 0xFF), QString::number((nValue) & 0xFF));
}

QString XBinary::get_uint64_full_version(quint64 nValue)
{
    QString sResult;

    quint32 nValue1 = (nValue >> 32) & 0xFFFFFFFF;
    quint32 nValue2 = nValue & 0xFFFFFFFF;

    sResult = QString("%1.%2").arg(get_uint32_full_version(nValue1), get_uint32_full_version(nValue2));

    return sResult;
}

QString XBinary::get_uint16_version(quint16 nValue)
{
    return QString("%1").arg(QString::number((nValue) & 0xFFFF));
}

QString XBinary::get_uint32_version(quint32 nValue)
{
    return QString("%1.%2").arg(QString::number((nValue >> 16) & 0xFFFF), QString::number((nValue) & 0xFFFF));
}

bool XBinary::isResizeEnable(QIODevice *pDevice)
{
    // mb TODO casr
    bool bResult = false;

    QString sClassName = pDevice->metaObject()->className();

    if (sClassName == "QFile") {
        bResult = true;
    } else if (sClassName == "QBuffer") {
        bResult = true;
    } else if (sClassName == "QTemporaryFile") {
        bResult = true;
    }

    return bResult;
}

bool XBinary::resize(QIODevice *pDevice, qint64 nSize)
{
    bool bResult = false;

    QString sClassName = pDevice->metaObject()->className();  // TODO

    if (sClassName == "QFile") {
        bResult = ((QFile *)pDevice)->resize(nSize);
    } else if (sClassName == "QBuffer") {
        ((QBuffer *)pDevice)->buffer().resize((qint32)nSize);
        bResult = true;
    } else if (sClassName == "QTemporaryFile") {
        bResult = ((QTemporaryFile *)pDevice)->resize(nSize);
    }

    return bResult;
}

XBinary::PACKED_UINT XBinary::read_uleb128(qint64 nOffset, qint64 nSize)
{
    PACKED_UINT result = {};

    quint32 nShift = 0;

    for (qint32 i = 0; i < nSize; i++) {
        quint8 nByte = read_uint8(nOffset + i);
        result.nValue |= ((nByte & 0x7F) << nShift);
        result.nByteSize++;
        nShift += 7;

        if ((nByte & 0x80) == 0) {
            result.bIsValid = true;
            break;
        }
    }

    return result;
}

XBinary::PACKED_UINT XBinary::_read_uleb128(char *pData, qint64 nSize)
{
    PACKED_UINT result = {};

    quint32 nShift = 0;

    for (qint32 i = 0; i < nSize; i++) {
        quint8 nByte = (quint8)(*(pData + i));
        result.nValue |= ((nByte & 0x7F) << nShift);
        result.nByteSize++;
        nShift += 7;

        if ((nByte & 0x80) == 0) {
            result.bIsValid = true;
            break;
        }
    }

    return result;
}

XBinary::PACKED_UINT XBinary::read_acn1_integer(qint64 nOffset, qint64 nSize)
{
    PACKED_UINT result = {};

    if (nSize > 0) {
        quint8 nByte = read_uint8(nOffset);

        if ((nByte & 0x80) == 0) {
            result.bIsValid = true;
            result.nByteSize = 1;
            result.nValue = nByte;
        } else {
            quint8 _nSize = (nByte & 0x7F);

            // Indefinite form (_nSize == 0) is not allowed here
            // Ensure we have at least 1 + _nSize bytes available and cap to 4 bytes
            if ((_nSize != 0) && (_nSize <= 4) && (1 + (qint64)_nSize <= nSize)) {
                result.bIsValid = true;
                result.nByteSize = 1 + _nSize;

                for (qint32 i = 0; i < _nSize; i++) {
                    result.nValue <<= 8;
                    result.nValue |= read_uint8(nOffset + 1 + i);
                }
            }
        }
    }

    return result;
}

XBinary::PACKED_UINT XBinary::_read_acn1_integer(char *pData, qint64 nSize)
{
    PACKED_UINT result = {};

    if ((pData != nullptr) && (nSize > 0)) {
        quint8 nByte = (quint8)(*pData);

        if ((nByte & 0x80) == 0) {
            result.bIsValid = true;
            result.nByteSize = 1;
            result.nValue = nByte;
        } else {
            quint8 _nSize = (nByte & 0x7F);

            if ((_nSize != 0) && (_nSize <= 4) && (1 + (qint64)_nSize <= nSize)) {
                result.bIsValid = true;
                result.nByteSize = 1 + _nSize;

                quint64 nVal = 0;
                for (qint32 i = 0; i < _nSize; i++) {
                    nVal <<= 8;
                    nVal |= (quint8)(*(pData + 1 + i));
                }
                result.nValue = nVal;
            }
        }
    }

    return result;
}

QString XBinary::read_ASN_OIDString(qint64 nOffset, qint64 nSize)
{
    QString sResult;

    if (nSize > 0) {
        quint8 nStart = read_uint8(nOffset);

        sResult += QString("%1.%2").arg(QString::number(nStart / 40), QString::number(nStart % 40));

        nOffset++;
        nSize--;

        quint64 nValue = 0;

        while (nSize > 0) {
            quint8 nByte = read_uint8(nOffset);

            nValue <<= 7;
            nValue += (nByte & 0x7F);

            if (!(nByte & 0x80)) {
                sResult += QString(".%1").arg(nValue);
                nValue = 0;
            }

            nOffset++;
            nSize--;
        }
    }

    return sResult;
}

qint64 XBinary::read_ASN_Integer(qint64 nOffset, qint64 nSize)
{
    qint64 nResult = 0;

    PACKED_UINT packedInt = read_acn1_integer(nOffset, nSize);

    if (packedInt.bIsValid) {
        nResult = packedInt.nValue;
    }

    return nResult;
}

bool XBinary::read_ASN_Bool(qint64 nOffset, qint64 nSize)
{
    if (nSize <= 0) return false;
    quint8 v = read_uint8(nOffset);
    return v != 0x00;  // per DER, any non-zero is TRUE
}

QDateTime XBinary::read_ASN_DateTime(qint64 nOffset, qint64 nSize)
{
    // Try UTCTime (YYMMDDHHMMSSZ) then GeneralizedTime (YYYYMMDDHHMMSSZ)
    QByteArray bytes = read_array(nOffset, qMax<qint64>(0, nSize));
    QString s = QString::fromLatin1(bytes);
    QDateTime dt;
    // Ensure ends with 'Z' for UTC; handle missing seconds too
    static const QStringList fmts = {"yyMMddHHmmss'Z'", "yyMMddHHmm'Z'", "yyyyMMddHHmmss'Z'", "yyyyMMddHHmm'Z'"};
    for (const QString &f : fmts) {
        dt = QDateTime::fromString(s, f);
        if (dt.isValid()) {
            dt.setTimeSpec(Qt::UTC);
            return dt;
        }
    }
    return QDateTime();
}

QString XBinary::read_ASN_AnsiString(qint64 nOffset, qint64 nSize)
{
    if (nSize <= 0) return QString();
    QByteArray bytes = read_array(nOffset, nSize);
    // Trim potential trailing NULs
    int trim = bytes.size();
    while (trim > 0 && bytes.at(trim - 1) == '\0') trim--;
    return QString::fromLatin1(bytes.constData(), trim);
}

XBinary::PACKED_UINT XBinary::_read_packedNumber(char *pData, qint64 nSize)
{
    PACKED_UINT result = {};

    if (nSize <= 0) {
        return result;
    }

    quint8 nFirstByte = (quint8)(*(pData));

    result.nByteSize = 0;

    // if ((nFirstByte & 0x80) == 0) {  // 0xxxxxxx
    //     result.nValue = nFirstByte & 0x7F;
    //     result.nByteSize = 1;
    // } else if ((nFirstByte & 0xC0) == 0x80) {  // 10xxxxxx
    //     result.nValue = nFirstByte & 0x3F;
    //     result.nByteSize = 2;
    // } else if ((nFirstByte & 0xE0) == 0xC0) {  // 110xxxxx
    //     result.nValue = nFirstByte & 0x1F;
    //     result.nByteSize = 3;
    // } else if ((nFirstByte & 0xF0) == 0xE0) {  // 1110xxxx
    //     result.nValue = nFirstByte & 0x0F;
    //     result.nByteSize = 4;
    // } else if ((nFirstByte & 0xF8) == 0xF0) {  // 11110xxx
    //     result.nValue = nFirstByte & 0x07;
    //     result.nByteSize = 5;
    // } else if ((nFirstByte & 0xFC) == 0xF8) {  // 111110xx
    //     result.nValue = nFirstByte & 0x03;
    //     result.nByteSize = 6;
    // } else if ((nFirstByte & 0xFE) == 0xFC) {  // 1111110x
    //     result.nValue = nFirstByte & 0x01;
    //     result.nByteSize = 7;
    // } else if (nFirstByte == 0xFE) {  // 11111110
    //     result.nValue = 0;
    //     result.nByteSize = 8;
    // } else if (nFirstByte == 0xFF) {  // 11111111
    //     result.nValue = 0;
    //     result.nByteSize = 9;
    // }

    // if (result.nByteSize <= nSize) {
    //     result.bIsValid = true;

    //     for (qint32 i = 1; i < result.nByteSize; ++i) {
    //         quint8 _nByte = (quint8)(*(pData + i));
    //         result.nValue |= static_cast<quint32>(_nByte) << (8 * (i - 1));
    //     }
    // }

    result.nByteSize = 1;

    if (nFirstByte < 0x80) {
        // Single byte
        result.nValue = nFirstByte;
        result.bIsValid = true;
    } else {
        // Multi-byte encoding
        quint8 nMask = 0x80;
        result.nByteSize = 1;

        for (qint32 i = 0; i < 8; i++) {
            if (nFirstByte & nMask) {
                if (result.nByteSize < nSize) {
                    result.nValue |= ((quint64)(quint8)pData[result.nByteSize] << (8 * i));
                    result.nByteSize++;
                } else {
                    break;
                }
            } else {
                result.nValue |= ((quint64)(nFirstByte & (nMask - 1)) << (8 * i));
                result.bIsValid = true;
                break;
            }
            nMask >>= 1;
        }
    }

    return result;
}

QList<QString> XBinary::getListFromFile(const QString &sFileName)
{
    QList<QString> listResult;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString sLine = in.readLine().trimmed();
            if (sLine != "") {
                listResult.append(sLine);
            }
        }

        file.close();
    }

    return listResult;
}

bool XBinary::_handleOverlay(_MEMORY_MAP *pMemoryMap)
{
    bool bResult = false;

    qint64 nTotalSize = pMemoryMap->nBinarySize;

    // get maximal offset
    qint64 nMaxOffset = 0;
    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize > nMaxOffset) {
            nMaxOffset = pMemoryMap->listRecords.at(i).nOffset + pMemoryMap->listRecords.at(i).nSize;
        }
    }

    if (nTotalSize - nMaxOffset > 0) {
        // overlay present
        qint64 nOverlayOffset = nMaxOffset;
        qint64 nOverlaySize = nTotalSize - nOverlayOffset;

        _MEMORY_RECORD record = {};
        record.nOffset = nOverlayOffset;
        record.nSize = nOverlaySize;
        record.sName = tr("Overlay");
        record.filePart = FILEPART_OVERLAY;
        record.nAddress = -1;  // TODO
        // record.nID = 0;        // TODO
        record.bIsVirtual = false;
        record.bIsInvisible = false;
        record.nIndex = pMemoryMap->listRecords.count();

        pMemoryMap->listRecords.append(record);

        bResult = true;
    }

    return bResult;
}

qint64 XBinary::getOverlaySize(PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return getOverlaySize(&memoryMap, pPdStruct);
}

qint64 XBinary::getOverlaySize(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    qint64 nSize = pMemoryMap->nBinarySize;
    qint64 nOverlayOffset = getOverlayOffset(pMemoryMap, pPdStruct);
    qint64 nDelta = 0;

    if (nOverlayOffset > 0) {
        nDelta = nSize - nOverlayOffset;
    }

    return qMax(nDelta, (qint64)0);
}

qint64 XBinary::getOverlayOffset(PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return getOverlayOffset(&memoryMap, pPdStruct);
}

qint64 XBinary::getOverlayOffset(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    qint64 nResult = -1;
    qint64 nRawSize = _calculateRawSize(pMemoryMap, pPdStruct);

    if (nRawSize) {
        nResult = nRawSize;
    }

    return nResult;
}

bool XBinary::isOverlayPresent(PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return isOverlayPresent(&memoryMap, pPdStruct);
}

bool XBinary::isOverlayPresent(XBinary::_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct)
{
    return (getOverlaySize(pMemoryMap, pPdStruct) != 0);
}

bool XBinary::compareOverlay(const QString &sSignature, qint64 nOffset, PDSTRUCT *pPdStruct)
{
    _MEMORY_MAP memoryMap = getMemoryMap(MAPMODE_UNKNOWN, pPdStruct);

    return compareOverlay(&memoryMap, sSignature, nOffset, pPdStruct);
}

bool XBinary::compareOverlay(XBinary::_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (isOverlayPresent(pMemoryMap, pPdStruct)) {
        qint64 nOverlayOffset = getOverlayOffset(pMemoryMap, pPdStruct) + nOffset;

        bResult = compareSignature(pMemoryMap, sSignature, nOverlayOffset, pPdStruct);
    }

    return bResult;
}

bool XBinary::addOverlay(char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    qint64 nRawSize = getOverlayOffset(pPdStruct);

    if (resize(getDevice(), nRawSize + nDataSize)) {
        if (nDataSize) {
            write_array(nRawSize, pData, nDataSize);

            bResult = true;
        }
    }

    return bResult;
}

bool XBinary::addOverlay(const QString &sFileName, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadOnly)) {
        qint64 nRawSize = getOverlayOffset(pPdStruct);
        qint64 nDataSize = file.size();

        if (resize(getDevice(), nRawSize + nDataSize)) {
            if (nDataSize) {
                bResult = copyDeviceMemory(&file, 0, getDevice(), nRawSize, nDataSize);
            }
        }

        file.close();
    }

    return bResult;
}

bool XBinary::removeOverlay()
{
    return addOverlay(0, 0);
}

bool XBinary::isSignatureInFilePartPresent(qint32 nFilePartNumber, const QString &sSignature)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return isSignatureInFilePartPresent(&memoryMap, nFilePartNumber, sSignature);
}

bool XBinary::isSignatureInFilePartPresent(XBinary::_MEMORY_MAP *pMemoryMap, qint32 nFilePartNumber, const QString &sSignature, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pMemoryMap->listRecords.count();

    for (qint32 i = 0; (i < nNumberOfRecords) && XBinary::isPdStructNotCanceled(pPdStruct); i++) {
        if (pMemoryMap->listRecords.at(i).nFilePartNumber == nFilePartNumber) {
            if (pMemoryMap->listRecords.at(i).nOffset != -1) {
                bResult = isSignaturePresent(pMemoryMap, pMemoryMap->listRecords.at(i).nOffset, pMemoryMap->listRecords.at(i).nSize, sSignature, pPdStruct);

                break;
            }
        }
    }

    return bResult;
}

QString XBinary::getStringCollision(QList<QString> *pListStrings, const QString &sString1, const QString &sString2)
{
    // TODO Check&optimize
    QString sResult;

    qint32 nNumberOfStrings = pListStrings->count();

    QString sRoot1;
    QString sRoot2;

    for (qint32 i = 0; i < nNumberOfStrings; i++) {
        QString sCurrentString = pListStrings->at(i);

        if (sCurrentString.contains(sString1)) {
            sRoot1 = sCurrentString.section(sString1, 0, 0);
        }

        if ((sRoot1 != "") && sCurrentString.contains(sString2)) {
            sRoot2 = sCurrentString.section(sString2, 0, 0);

            break;
        }
    }

    if ((sRoot1 != "") && (sRoot1 == sRoot2)) {
        sResult = sRoot1;
    }

    return sResult;
}

bool XBinary::writeToFile(const QString &sFileName, const QByteArray &baData)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);
        file.write(baData.data(), baData.size());
        file.close();
        bResult = true;
    }

    return bResult;
}

bool XBinary::writeToFile(const QString &sFileName, QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (createFile(sFileName, pDevice->size())) {
        QFile file;
        file.setFileName(sFileName);

        if (file.open(QIODevice::ReadWrite)) {
            bResult = copyDeviceMemory(pDevice, 0, &file, 0, pDevice->size(), pPdStruct);
            file.close();
        }
    }

    return bResult;
}

bool XBinary::appendToFile(const QString &sFileName, const QString &sString)
{
    QString _sString = sString;
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite | QIODevice::Append)) {
        _sString += "\r\n";  // TODO Linux
        file.write(_sString.toUtf8());
        file.close();
        bResult = true;
    }

    return bResult;
}

bool XBinary::clearFile(const QString &sFileName)
{
    bool bResult = false;

    QFile file;
    file.setFileName(sFileName);

    if (file.open(QIODevice::ReadWrite)) {
        file.resize(0);
        file.close();
        bResult = true;
    }

    return bResult;
}

qint32 XBinary::getStringNumberFromList(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint32 nResult = -1;

    qint32 nNumberOfRecords = pListStrings->count();

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        if (pListStrings->at(i) == sString) {
            nResult = i;

            break;
        }
    }

    return nResult;
}

qint32 XBinary::getStringNumberFromListExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint32 nResult = -1;

    qint32 nNumberOfRecords = pListStrings->count();

    for (qint32 i = 0; (i < nNumberOfRecords) && (!(pPdStruct->bIsStop)); i++) {
        if (isRegExpPresent(sString, pListStrings->at(i))) {
            nResult = i;

            break;
        }
    }

    return nResult;
}

bool XBinary::isStringInListPresent(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct)
{
    return (getStringNumberFromList(pListStrings, sString, pPdStruct) != -1);
}

bool XBinary::isStringInListPresentExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct)
{
    return (getStringNumberFromListExp(pListStrings, sString, pPdStruct) != -1);
}

QString XBinary::getStringByIndex(QList<QString> *pListStrings, qint32 nIndex, qint32 nNumberOfStrings)
{
    QString sResult;

    if (nNumberOfStrings == -1) {
        nNumberOfStrings = pListStrings->count();
    }

    if ((nIndex >= 0) && (nIndex < nNumberOfStrings)) {
        sResult = pListStrings->at(nIndex);
    }

    return sResult;
}

bool XBinary::isStringUnicode(const QString &sString, qint32 nMaxCheckSize)
{
    // TODO Optimize
    // TODO Check
    bool bResult = false;

    if (nMaxCheckSize == -1) {
        nMaxCheckSize = sString.size();
    } else {
        nMaxCheckSize = qMin(sString.size(), nMaxCheckSize);
    }

    for (qint32 i = 0; i < nMaxCheckSize; i++) {
        unsigned char cChar = sString.at(i).toLatin1();
        if ((cChar > 127) || (cChar < 27)) {
            bResult = true;
            break;
        }
    }

    return bResult;
}

quint32 XBinary::elfHash(const quint8 *pData)
{
    quint32 nResult = 0;

    while (*pData) {
        nResult = (nResult << 4) + (*pData);

        quint32 nHigh = nResult & 0xF0000000;

        if (nHigh) {
            nResult ^= (nResult >> 24);
        }

        nResult &= (~nHigh);

        pData++;
    }

    return nResult;
}

QString XBinary::getVersionString(const QString &sString)
{
    QString sResult;

    qint32 nSize = sString.size();

    for (qint32 i = 0; i < nSize; i++) {
        QChar c = sString.at(i);

        if (((QChar('9') >= c) && (c >= QChar('0'))) || (c == QChar('.'))) {
            sResult.append(c);
        } else {
            break;
        }
    }

    return sResult;
}

qint64 XBinary::getVersionIntValue(const QString &sString)
{
    qint64 nResult = 0;

    qint32 nNumberOfDots = sString.count(QChar('.'));

    for (qint32 i = 0; i < (nNumberOfDots + 1); i++) {
        nResult *= 10;

        nResult += sString.section(QChar('.'), i, i).toLongLong();
    }

    return nResult;
}

bool XBinary::checkStringNumber(const QString &sString, quint32 nMin, quint32 nMax)
{
    bool bResult = false;

    quint32 nValue = sString.toUInt();

    bResult = ((nValue >= nMin) && (nValue <= nMax));

    return bResult;
}

QDateTime XBinary::valueToTime(quint64 nValue, DT_TYPE type)
{
    QDateTime result;

    if (type == DT_TYPE_POSIX) {
        result.setMSecsSinceEpoch(nValue * 1000);
    } else if (type == DT_TYPE_UNIXTIME) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
        result = QDateTime::fromSecsSinceEpoch((quint32)nValue, Qt::UTC);
#else
        result = QDateTime::fromMSecsSinceEpoch((quint32)nValue * 1000, Qt::UTC);
#endif
    } else if (type == DT_TYPE_DOSTIME) {
        quint16 nDosTime = (quint16)nValue;
        // Use a valid dummy date (1980-01-01 = 0x0021) since we only need the time component
        result = dosDateTimeToQDateTime(0x0021, nDosTime);
    } else if (type == DT_TYPE_DOSDATE) {
        quint16 nDosDate = (quint16)nValue;
        result = dosDateTimeToQDateTime(nDosDate, 0);
    } else if (type == DT_TYPE_FILETIME) {
        // FILETIME: 100-nanosecond intervals since 1601-01-01
        qint64 nTicksSinceEpoch = (qint64)nValue - 116444736000000000LL;
        if (nTicksSinceEpoch >= 0) {
            qint64 nMsec = nTicksSinceEpoch / 10000;
            result = QDateTime::fromMSecsSinceEpoch(nMsec, Qt::UTC);
        }
    }

    return result;
}

QString XBinary::valueToTimeString(quint64 nValue, XBinary::DT_TYPE type)
{
    QString sResult;

    if (type == DT_TYPE_DOSTIME) {
        QDateTime dateTime = valueToTime(nValue, type);
        if (dateTime.isValid()) {
            sResult = dateTime.time().toString("HH:mm:ss");
        }
    } else if (type == DT_TYPE_DOSDATE) {
        QDateTime dateTime = valueToTime(nValue, type);
        if (dateTime.isValid()) {
            sResult = dateTime.date().toString("yyyy-MM-dd");
        }
    } else {
        QDateTime dateTime = valueToTime(nValue, type);
        if (dateTime.isValid()) {
            sResult = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        }
    }

    return sResult;
}

QString XBinary::msecToDate(quint64 nValue)
{
    QString sResult;

    quint64 _nTmp = nValue / 1000;

    quint64 nSec = _nTmp % 60;
    _nTmp /= 60;
    quint64 nMin = _nTmp % 60;
    _nTmp /= 60;
    quint64 nHour = _nTmp % 24;
    _nTmp /= 24;
    quint64 nDay = _nTmp;

    if (nDay > 0) {
        sResult += QString("%1:").arg(nDay, 2, 10, QChar('0'));
    }

    sResult += QString("%1:").arg(nHour, 2, 10, QChar('0'));
    sResult += QString("%1:").arg(nMin, 2, 10, QChar('0'));
    sResult += QString("%1").arg(nSec, 2, 10, QChar('0'));

    return sResult;
}

QDateTime XBinary::dosDateTimeToQDateTime(quint16 nDosDate, quint16 nDosTime)
{
    // MS-DOS date format: bits 0-4 day (1-31), 5-8 month (1-12), 9-15 years from 1980
    // MS-DOS time format: bits 0-4 seconds/2 (0-29 => 0-58), 5-10 minutes (0-59), 11-15 hours (0-23)
    qint32 nDay = (nDosDate & 0x1F);
    qint32 nMonth = (nDosDate >> 5) & 0x0F;
    qint32 nYear = ((nDosDate >> 9) & 0x7F) + 1980;

    qint32 nSecond = (nDosTime & 0x1F) * 2;
    qint32 nMinute = (nDosTime >> 5) & 0x3F;
    qint32 nHour = (nDosTime >> 11) & 0x1F;

    QDate date(nYear, nMonth, nDay);
    QTime time(nHour, nMinute, nSecond);

    QDateTime result;
    if (date.isValid() && time.isValid()) {
        result = QDateTime(date, time);
    }

    return result;
}

QPair<quint16, quint16> XBinary::qDateTimeToDosDateTime(const QDateTime &dateTime)
{
    QPair<quint16, quint16> result = {0, 0};

    if (dateTime.isValid()) {
        QDate date = dateTime.date();
        QTime time = dateTime.time();

        quint16 nDosDate = 0;
        quint16 nDosTime = 0;

        if (date.isValid()) {
            qint32 nYear = date.year();
            if (nYear >= 1980 && nYear <= 2107) {
                nDosDate = (date.day() & 0x1F) | ((date.month() & 0x0F) << 5) | (((nYear - 1980) & 0x7F) << 9);
            }
        }

        if (time.isValid()) {
            nDosTime = ((time.second() / 2) & 0x1F) | ((time.minute() & 0x3F) << 5) | ((time.hour() & 0x1F) << 11);
        }

        result = {nDosDate, nDosTime};
    }

    return result;
}

QString XBinary::valueToFlagsString(quint64 nValue, const QMap<quint64, QString> &mapFlags, VL_TYPE vlType)
{
    QString sResult;

    if (vlType == VL_TYPE_LIST) {
        sResult = mapFlags.value(nValue);
    } else if (vlType == VL_TYPE_FLAGS) {
        QMapIterator<quint64, QString> iter(mapFlags);

        while (iter.hasNext()) {
            iter.next();

            quint64 nFlag = iter.key();

            if (nValue & nFlag) {
                if (sResult != "") {
                    sResult += "|";
                }

                sResult += iter.value();
            }
        }
    }

    return sResult;
}

bool XBinary::isX86asm(const QString &sArch)
{
    // TODO remove, use getDisasmMode
    bool bResult = false;

    QString _sArch = sArch.toUpper();

    // TODO Check
    if ((_sArch == "8086") || (_sArch == "80286") || (_sArch == "80386") || (_sArch == "80486") || (_sArch == "80586") || (_sArch == "386") || (_sArch == "I386") ||
        (_sArch == "AMD64") || (_sArch == "X86_64")) {
        bResult = true;
    }

    return bResult;
}

QString XBinary::disasmIdToString(XBinary::DM disasmMode)
{
    QString sResult = tr("Unknown");

    switch (disasmMode) {
        case DM_DATA: sResult = tr("Data"); break;
        case DM_X86_16: sResult = QString("x86 16-bit mode"); break;
        case DM_X86_32: sResult = QString("x86 32-bit mode"); break;
        case DM_X86_64: sResult = QString("x86 64-bit mode"); break;
        case DM_ARM_LE: sResult = QString("ARM"); break;
        case DM_ARM_BE: sResult = QString("ARM, big endian"); break;
        case DM_AARCH64_LE: sResult = QString("AArch64"); break;
        case DM_AARCH64_BE: sResult = QString("AArch64, big endian"); break;
        case DM_CORTEXM: sResult = QString("CORTEXM"); break;
        case DM_THUMB_LE: sResult = QString("THUMB"); break;
        case DM_THUMB_BE: sResult = QString("THUMB BE"); break;
        case DM_MIPS_LE: sResult = QString("MIPS"); break;
        case DM_MIPS_BE: sResult = QString("MIPS BE"); break;
        case DM_MIPS64_LE: sResult = QString("MIPS64"); break;
        case DM_MIPS64_BE: sResult = QString("MIPS64 BE"); break;
        case DM_PPC_LE: sResult = QString("PPC"); break;
        case DM_PPC_BE: sResult = QString("PPC BE"); break;
        case DM_PPC64_LE: sResult = QString("PPC64"); break;
        case DM_PPC64_BE: sResult = QString("PPC64 BE"); break;
        case DM_SPARC: sResult = QString("Sparc"); break;
        case DM_SPARCV9: sResult = QString("Sparc V9"); break;
        case DM_S390X: sResult = QString("S390X"); break;
        case DM_XCORE: sResult = QString("XCORE"); break;
        case DM_M68K: sResult = QString("M68K"); break;
        case DM_M68K00: sResult = QString("M68K00"); break;
        case DM_M68K10: sResult = QString("M68K10"); break;
        case DM_M68K20: sResult = QString("M68K20"); break;
        case DM_M68K30: sResult = QString("M68K30"); break;
        case DM_M68K40: sResult = QString("M68K40"); break;
        case DM_M68K60: sResult = QString("M68K60"); break;
        case DM_TMS320C64X: sResult = QString("TMS320C64X"); break;
        case DM_M6800: sResult = QString("M6800"); break;
        case DM_M6801: sResult = QString("M6801"); break;
        case DM_M6805: sResult = QString("M6805"); break;
        case DM_M6808: sResult = QString("M6808"); break;
        case DM_M6809: sResult = QString("M6809"); break;
        case DM_M6811: sResult = QString("M6811"); break;
        case DM_CPU12: sResult = QString("CPU12"); break;
        case DM_HD6301: sResult = QString("HD6301"); break;
        case DM_HD6309: sResult = QString("HD6309"); break;
        case DM_HCS08: sResult = QString("HCS08"); break;
        case DM_EVM: sResult = QString("EVM"); break;
        case DM_RISKV32: sResult = QString("RISKV32"); break;
        case DM_RISKV64: sResult = QString("RISKV64"); break;
        case DM_RISKVC: sResult = QString("RISKVC"); break;
        case DM_MOS65XX: sResult = QString("MOS65XX"); break;
        case DM_WASM: sResult = QString("WASM"); break;
        case DM_BPF_LE: sResult = QString("BPF LE"); break;
        case DM_BPF_BE: sResult = QString("BPF BE"); break;
        case DM_CUSTOM_MACH_REBASE: sResult = QString("MACH REBASE"); break;
        case DM_CUSTOM_7ZIP_PROPERTIES: sResult = QString("7ZIP PROPERTIES"); break;
        default: sResult = tr("Unknown");
    }

    return sResult;
}

QString XBinary::disasmIdToArch(DM disasmMode)
{
    QString sResult = QString("data");

    switch (disasmMode) {
        case DM_DATA: sResult = QString("data"); break;
        case DM_X86_16:
        case DM_X86_32:
        case DM_X86_64: sResult = QString("x86"); break;
        case DM_ARM_LE:
        case DM_ARM_BE:
        case DM_CORTEXM:
        case DM_THUMB_LE:
        case DM_THUMB_BE: sResult = QString("ARM"); break;
        case DM_AARCH64_LE:
        case DM_AARCH64_BE: sResult = QString("AArch64"); break;
        case DM_MIPS_LE:
        case DM_MIPS_BE:
        case DM_MIPS64_LE:
        case DM_MIPS64_BE: sResult = QString("MIPS"); break;
        case DM_PPC_LE:
        case DM_PPC_BE:
        case DM_PPC64_LE:
        case DM_PPC64_BE: sResult = QString("PowerPC"); break;
        case DM_SPARC:
        case DM_SPARCV9: sResult = QString("Sparc"); break;
        case DM_S390X: sResult = QString("S390X"); break;
        case DM_XCORE: sResult = QString("XCORE"); break;
        case DM_M68K:
        case DM_M68K00:
        case DM_M68K10:
        case DM_M68K20:
        case DM_M68K30:
        case DM_M68K40:
        case DM_M68K60: sResult = QString("M68K"); break;
        case DM_TMS320C64X: sResult = QString("TMS320C64X"); break;
        case DM_M6800:
        case DM_M6801:
        case DM_M6805:
        case DM_M6808:
        case DM_M6809:
        case DM_M6811:
        case DM_CPU12:
        case DM_HD6301:
        case DM_HD6309:
        case DM_HCS08: sResult = QString("Motorola"); break;
        case DM_EVM: sResult = QString("EVM"); break;
        case DM_RISKV32:
        case DM_RISKV64:
        case DM_RISKVC: sResult = QString("RISC-V"); break;
        case DM_MOS65XX: sResult = QString("MOS"); break;
        case DM_WASM: sResult = QString("WebAssembly"); break;
        case DM_BPF_LE:
        case DM_BPF_BE: sResult = QString("BPF"); break;
        default: sResult = tr("Unknown");
    }

    return sResult;
}

QString XBinary::syntaxIdToString(SYNTAX syntax)
{
    QString sResult = tr("Unknown");

    switch (syntax) {
        case SYNTAX_DEFAULT: sResult = tr("Default"); break;
        case SYNTAX_ARM: sResult = tr("ARM"); break;
        case SYNTAX_ATT: sResult = QString("ATT"); break;
        case SYNTAX_AMD: sResult = QString("AMD"); break;
        case SYNTAX_INTEL: sResult = QString("INTEL"); break;
        case SYNTAX_MASM: sResult = QString("MASM"); break;
        case SYNTAX_MOTOROLA: sResult = QString("MOTOROLA"); break;
        case SYNTAX_MIPS: sResult = QString("MIPS"); break;
    }

    return sResult;
}

XBinary::SYNTAX XBinary::stringToSyntaxId(const QString &sString)
{
    SYNTAX result = SYNTAX_DEFAULT;

    if (sString == "ATT") result = SYNTAX_ATT;
    else if (sString == "ARM") result = SYNTAX_ARM;
    else if (sString == "AMD") result = SYNTAX_AMD;
    else if (sString == "INTEL") result = SYNTAX_INTEL;
    else if (sString == "MASM") result = SYNTAX_MASM;
    else if (sString == "MOTOROLA") result = SYNTAX_MOTOROLA;
    else if (sString == "MIPS") result = SYNTAX_MIPS;

    return result;
}

QString XBinary::osNameIdToString(OSNAME osName)
{
    QString sResult = tr("Unknown");

    switch (osName) {
        case OSNAME_MULTIPLATFORM: sResult = tr("Multiplatform"); break;
        case OSNAME_AIX: sResult = QString("AIX"); break;
        case OSNAME_ALPINELINUX: sResult = QString("Alpine Linux"); break;
        case OSNAME_AMIGA: sResult = QString("Amiga"); break;
        case OSNAME_ANDROID: sResult = QString("Android"); break;
        case OSNAME_AROS: sResult = QString("Amiga Research OS"); break;
        case OSNAME_ASPLINUX: sResult = QString("ASPLinux"); break;
        case OSNAME_ATARIST: sResult = QString("Atari ST"); break;
        case OSNAME_BORLANDOSSERVICES: sResult = QString("Borland OS Services"); break;
        case OSNAME_BRIDGEOS: sResult = QString("bridgeOS"); break;
        case OSNAME_DEBIANLINUX: sResult = QString("Debian Linux"); break;
        case OSNAME_FENIXOS: sResult = QString("FenixOS"); break;
        case OSNAME_FREEBSD: sResult = QString("FreeBSD"); break;
        case OSNAME_GENTOOLINUX: sResult = QString("Gentoo Linux"); break;
        case OSNAME_HANCOMLINUX: sResult = QString("Hancom Linux"); break;
        case OSNAME_HPUX: sResult = QString("Hewlett-Packard HP-UX"); break;
        case OSNAME_IOS: sResult = QString("iOS"); break;
        case OSNAME_IPADOS: sResult = QString("iPadOS"); break;
        case OSNAME_IPHONEOS: sResult = QString("iPhone OS"); break;
        case OSNAME_IRIX: sResult = QString("IRIX"); break;
        case OSNAME_LINUX: sResult = QString("Linux"); break;
        case OSNAME_MACOS: sResult = QString("macOS"); break;
        case OSNAME_MAC_OS: sResult = QString("Mac OS"); break;
        case OSNAME_MAC_OS_X: sResult = QString("Mac OS X"); break;
        case OSNAME_MACCATALYST: sResult = QString("Mac Catalyst"); break;
        case OSNAME_MANDRAKELINUX: sResult = QString("Mandrake Linux"); break;
        case OSNAME_MACDRIVERKIT: sResult = QString("Mac DriverKit"); break;
        case OSNAME_MACFIRMWARE: sResult = QString("Mac Firmware"); break;
        case OSNAME_MCLINUX: sResult = QString("mClinux"); break;
        case OSNAME_MINIX: sResult = QString("Minix"); break;
        case OSNAME_MODESTO: sResult = QString("Novell Modesto"); break;
        case OSNAME_MSDOS: sResult = QString("MS-DOS"); break;
        case OSNAME_FREEDOS: sResult = QString("FreeDOS"); break;
        case OSNAME_NETBSD: sResult = QString("NetBSD"); break;
        case OSNAME_NSK: sResult = QString("Hewlett-Packard Non-Stop Kernel"); break;
        case OSNAME_OPENBSD: sResult = QString("OpenBSD"); break;
        case OSNAME_OPENVMS: sResult = QString("Open VMS"); break;
        case OSNAME_OPENVOS: sResult = QString("Open VOS"); break;
        case OSNAME_OS2: sResult = QString("OS/2"); break;
        case OSNAME_OS_X: sResult = QString("OS X"); break;
        case OSNAME_POSIX: sResult = QString("Posix"); break;
        case OSNAME_QNX: sResult = QString("QNX"); break;
        case OSNAME_REDHATLINUX: sResult = QString("Red Hat Linux"); break;
        case OSNAME_SEPOS: sResult = QString("sepOS"); break;
        case OSNAME_SOLARIS: sResult = QString("Sun Solaris"); break;
        case OSNAME_STARTOSLINUX: sResult = QString("StartOS Linux"); break;
        case OSNAME_SUNOS: sResult = QString("SunOS"); break;
        case OSNAME_SUSELINUX: sResult = QString("SUSE Linux"); break;
        case OSNAME_SYLLABLE: sResult = QString("Syllable"); break;
        case OSNAME_TRU64: sResult = QString("Compaq TRU64 UNIX"); break;
        case OSNAME_TURBOLINUX: sResult = QString("Turbolinux"); break;
        case OSNAME_TVOS: sResult = QString("tvOS"); break;
        case OSNAME_UBUNTULINUX: sResult = QString("Ubuntu Linux"); break;
        case OSNAME_UEFI: sResult = QString("UEFI"); break;
        case OSNAME_UNIX: sResult = QString("Unix"); break;
        case OSNAME_VINELINUX: sResult = QString("Vine Linux"); break;
        case OSNAME_WATCHOS: sResult = QString("watchOS"); break;
        case OSNAME_REACTOS: sResult = QString("ReactOS"); break;
        case OSNAME_WINDOWS: sResult = QString("Windows"); break;
        case OSNAME_WINDOWSCE: sResult = QString("Windows CE"); break;
        case OSNAME_WINDOWSPHONE: sResult = QString("Windows Phone"); break;
        case OSNAME_WINDRIVERLINUX: sResult = QString("Wind River Linux"); break;
        case OSNAME_XBOX: sResult = QString("XBOX"); break;
        case OSNAME_PLAYSTATION: sResult = QString("PlayStation"); break;
        case OSNAME_JVM: sResult = QString("JVM"); break;
        case OSNAME_CHROMEOS: sResult = QString("ChromeOS"); break;
        case OSNAME_NINTENDOSWITCH: sResult = QString("Nintendo Switch"); break;
        case OSNAME_WEBOS: sResult = QString("webOS"); break;
        case OSNAME_HARMONYOS: sResult = QString("Harmony OS"); break;
        case OSNAME_ARCHLINUX: sResult = QString("Arch Linux"); break;

        default: sResult = tr("Unknown");
    }

    return sResult;
}

XBinary::DM XBinary::getDisasmMode()
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return getDisasmMode(&memoryMap);
}

XBinary::DM XBinary::getDisasmMode(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return getDisasmMode(pMemoryMap->sArch, (pMemoryMap->endian == ENDIAN_BIG), pMemoryMap->mode);
}

XBinary::DM XBinary::getDisasmMode(const QString &sArch, bool bIsBigEndian, MODE mode)
{
    XBinary::DM dmResult = DM_UNKNOWN;

    QString _sArch = sArch.toUpper();

    if (_sArch == "PPC") {
        if (bIsBigEndian) {
            dmResult = DM_PPC_BE;
        } else {
            dmResult = DM_PPC_LE;
        }
    } else if (_sArch == "PPC64") {
        if (bIsBigEndian) {
            dmResult = DM_PPC64_BE;
        } else {
            dmResult = DM_PPC64_LE;
        }
    } else if ((_sArch == "MIPS") || (_sArch == "R3000") || (_sArch == "R4000") || (_sArch == "R10000") || (_sArch == "WCEMIPSV2")) {
        if (mode == MODE_64) {
            if (bIsBigEndian) {
                dmResult = DM_MIPS64_BE;
            } else {
                dmResult = DM_MIPS64_LE;
            }
        } else {
            if (bIsBigEndian) {
                dmResult = DM_MIPS_BE;
            } else {
                dmResult = DM_MIPS_LE;
            }
        }
    } else if ((_sArch == "ARM") || (_sArch == "ARMNT") || (_sArch == "THUMB") || (_sArch == "ARM_V6") || (_sArch == "ARM_V7") || (_sArch == "ARM_V7S") ||
               (_sArch == "ARM64_32")) {
        if (bIsBigEndian) {
            dmResult = DM_ARM_BE;
        } else {
            dmResult = DM_ARM_LE;
        }
    } else if ((_sArch == "AARCH64") || (_sArch == "ARM64") || (_sArch == "ARM64E") || (_sArch == "ARM_A500")) {
        if (bIsBigEndian) {
            dmResult = DM_AARCH64_BE;
        } else {
            dmResult = DM_AARCH64_LE;
        }
    } else if ((_sArch == "8086") || (_sArch == "286")) {
        dmResult = DM_X86_16;
    } else if ((_sArch == "386") || (_sArch == "80386") || (_sArch == "80486") || (_sArch == "80586") || (_sArch == "I386") || (_sArch == "486") || (_sArch == "X86")) {
        if ((mode == MODE_16) || (mode == MODE_16SEG)) {
            dmResult = DM_X86_16;
        } else {
            dmResult = DM_X86_32;
        }
    } else if ((_sArch == "AMD64") || (_sArch == "X86_64") || (_sArch == "X64")) {
        dmResult = DM_X86_64;
    } else if ((_sArch == "68K") || (_sArch == "MC680x0")) {
        dmResult = DM_M68K;
    } else if ((_sArch == "MC68030") || (_sArch == "MC68030_ONLY")) {
        dmResult = DM_M68K30;
    } else if (_sArch == "MC68040") {
        dmResult = DM_M68K40;
    } else if (_sArch == "POWERPC") {
        if (bIsBigEndian) {
            dmResult = DM_PPC_BE;
        } else {
            dmResult = DM_PPC_LE;
        }
    } else if (_sArch == "POWERPC_BE") {
        dmResult = DM_PPC_BE;
    } else if (_sArch == "SPARC") {
        dmResult = DM_SPARC;
    } else if (_sArch == "SPARCV9") {
        dmResult = DM_SPARCV9;
    } else if (_sArch == "S390") {
        dmResult = DM_S390X;
    } else if ((_sArch == "RISC_V") || (_sArch == "RISCV32") || (_sArch == "RISCV64")) {
        if (mode == MODE_64) {
            dmResult = DM_RISKV64;
        } else {
            dmResult = DM_RISKV32;
        }
    }
    // else if (_sArch == "MACH_REBASE") {
    //     dmResult = DM_CUSTOM_MACH_REBASE;
    // }
    // TODO SH
    // TODO more

    return dmResult;
}

XBinary::DM XBinary::getDisasmMode(FILEFORMATINFO *pFileFormatInfo)
{
    return getDisasmMode(pFileFormatInfo->sArch, (pFileFormatInfo->endian == ENDIAN_BIG), pFileFormatInfo->mode);
}

XBinary::DMFAMILY XBinary::getDisasmFamily(XBinary::DM disasmMode)
{
    DMFAMILY result = DMFAMILY_UNKNOWN;

    if ((disasmMode == DM_X86_16) || (disasmMode == DM_X86_32) || (disasmMode == DM_X86_64)) {
        result = DMFAMILY_X86;
    } else if ((disasmMode == DM_ARM_BE) || (disasmMode == DM_ARM_LE)) {
        result = DMFAMILY_ARM;
    } else if ((disasmMode == DM_AARCH64_BE) || (disasmMode == DM_AARCH64_LE)) {
        result = DMFAMILY_ARM64;
    } else if ((disasmMode == DM_MIPS64_BE) || (disasmMode == DM_MIPS64_LE) || (disasmMode == DM_MIPS_BE) || (disasmMode == DM_MIPS_LE)) {
        result = DMFAMILY_MIPS;
    } else if ((disasmMode == DM_PPC64_BE) || (disasmMode == DM_PPC64_LE) || (disasmMode == DM_PPC_BE) || (disasmMode == DM_PPC_LE)) {
        result = DMFAMILY_PPC;
    } else if (disasmMode == DM_SPARC) {
        result = DMFAMILY_SPARC;
    } else if (disasmMode == DM_S390X) {
        result = DMFAMILY_SYSZ;
    } else if (disasmMode == DM_XCORE) {
        result = DMFAMILY_XCORE;
    } else if ((disasmMode == DM_M68K) || (disasmMode == DM_M68K00) || (disasmMode == DM_M68K10) || (disasmMode == DM_M68K20) || (disasmMode == DM_M68K30) ||
               (disasmMode == DM_M68K40) || (disasmMode == DM_M68K60)) {
        result = DMFAMILY_M68K;
    } else if ((disasmMode == DM_M6800) || (disasmMode == DM_M6801) || (disasmMode == DM_M6805) || (disasmMode == DM_M6808) || (disasmMode == DM_M6809) ||
               (disasmMode == DM_M6811) || (disasmMode == DM_CPU12) || (disasmMode == DM_HD6301) || (disasmMode == DM_HD6309) || (disasmMode == DM_HCS08)) {
        result = DMFAMILY_M68OK;
    } else if ((disasmMode == DM_RISKV32) || (disasmMode == DM_RISKV64) || (disasmMode == DM_RISKVC)) {
        result = DMFAMILY_RISCV;
    } else if (disasmMode == DM_EVM) {
        result = DMFAMILY_EVM;
    } else if (disasmMode == DM_MOS65XX) {
        result = DMFAMILY_MOS65XX;
    } else if (disasmMode == DM_WASM) {
        result = DMFAMILY_WASM;
    } else if ((disasmMode == DM_BPF_LE) || (disasmMode == DM_BPF_BE)) {
        result = DMFAMILY_BPF;
    } else if (disasmMode == DM_CUSTOM_MACH_REBASE) {
        result = DMFAMILY_CUSTOM_MACH_REBASE;
    } else if ((disasmMode == DM_CUSTOM_MACH_BIND) || (disasmMode == DM_CUSTOM_MACH_WEAK)) {
        result = DMFAMILY_CUSTOM_MACH_BIND;
    } else if (disasmMode == DM_CUSTOM_MACH_EXPORT) {
        result = DMFAMILY_CUSTOM_MACH_EXPORT;
    } else if (disasmMode == DM_CUSTOM_7ZIP_PROPERTIES) {
        result = DMFAMILY_CUSTOM_7ZIP;
    }

    return result;
}

XBinary::DMFAMILY XBinary::getDisasmFamily(XBinary::_MEMORY_MAP *pMemoryMap)
{
    return getDisasmFamily(getDisasmMode(pMemoryMap));
}

QList<XBinary::SYNTAX> XBinary::getDisasmSyntax(DM disasmMode)
{
    QList<SYNTAX> listResult;

    listResult.append(SYNTAX_DEFAULT);

    if (getDisasmFamily(disasmMode) == DMFAMILY_X86) {
        listResult.append(SYNTAX_ATT);
        listResult.append(SYNTAX_AMD);
        listResult.append(SYNTAX_INTEL);
        listResult.append(SYNTAX_MASM);
    }

    if (getDisasmFamily(disasmMode) == DMFAMILY_M68K) {
        listResult.append(SYNTAX_MOTOROLA);
    }

    return listResult;
}

XBinary::MODE XBinary::getModeFromDisasmMode(DM disasmMode)
{
    MODE result = MODE_32;

    if (disasmMode == DM_X86_16) {
        result = MODE_16;
    } else if (disasmMode == DM_X86_32) {
        result = MODE_32;
    } else if (disasmMode == DM_X86_64) {
        result = MODE_64;
    } else if ((disasmMode == DM_ARM_BE) || (disasmMode == DM_ARM_LE)) {
        result = MODE_32;
    } else if ((disasmMode == DM_AARCH64_BE) || (disasmMode == DM_AARCH64_LE)) {
        result = MODE_64;
    } else if ((disasmMode == DM_MIPS_BE) || (disasmMode == DM_MIPS_LE)) {
        result = MODE_32;
    } else if ((disasmMode == DM_MIPS64_BE) || (disasmMode == DM_MIPS64_LE)) {
        result = MODE_64;
    }

    return result;
}

bool XBinary::checkFileType(XBinary::FT fileTypeMain, XBinary::FT fileTypeOptional)
{
    bool bResult = false;

    if ((fileTypeMain == FT_PE) && ((fileTypeOptional == FT_PE) || (fileTypeOptional == FT_PE32) || (fileTypeOptional == FT_PE64))) {
        bResult = true;
    } else if ((fileTypeMain == FT_ELF) && ((fileTypeOptional == FT_ELF) || (fileTypeOptional == FT_ELF32) || (fileTypeOptional == FT_ELF64))) {
        bResult = true;
    } else if ((fileTypeMain == FT_MACHO) && ((fileTypeOptional == FT_MACHO) || (fileTypeOptional == FT_MACHO32) || (fileTypeOptional == FT_MACHO64))) {
        bResult = true;
    } else if ((fileTypeMain == FT_ICO) && ((fileTypeOptional == FT_ICO) || (fileTypeOptional == FT_CUR))) {
        bResult = true;
    } else if ((fileTypeMain == FT_ICC) && (fileTypeOptional == FT_ICC)) {
        bResult = true;
    } else if ((fileTypeMain == FT_TEXT) && ((fileTypeOptional == FT_TEXT) || (fileTypeOptional == FT_UTF8) || (fileTypeOptional == FT_UNICODE))) {
        bResult = true;
    } else if ((fileTypeMain == FT_TAR_GZ) &&
               ((fileTypeOptional == FT_TAR_GZ) || (fileTypeOptional == FT_TAR_BZIP2) || (fileTypeOptional == FT_TAR_LZIP) || (fileTypeOptional == FT_TAR_LZMA) ||
                (fileTypeOptional == FT_TAR_LZOP) || (fileTypeOptional == FT_TAR_XZ) || (fileTypeOptional == FT_TAR_Z) || (fileTypeOptional == FT_TAR_ZSTD))) {
        bResult = true;
    } else if (fileTypeMain == fileTypeOptional) {
        bResult = true;
    }

    return bResult;
}

void XBinary::filterFileTypes(QSet<XBinary::FT> *pStFileTypes)
{
    // TODO Check!
    // TODO optimize! Add new Types!
    if (pStFileTypes->contains(XBinary::FT_MSDOS) || pStFileTypes->contains(XBinary::FT_DOS16M) || pStFileTypes->contains(XBinary::FT_DOS4G) ||
        pStFileTypes->contains(XBinary::FT_NE) || pStFileTypes->contains(XBinary::FT_LE) || pStFileTypes->contains(XBinary::FT_LX) ||
        pStFileTypes->contains(XBinary::FT_PE) || pStFileTypes->contains(XBinary::FT_PE32) || pStFileTypes->contains(XBinary::FT_PE64) ||
        pStFileTypes->contains(XBinary::FT_ELF) || pStFileTypes->contains(XBinary::FT_ELF32) || pStFileTypes->contains(XBinary::FT_ELF64) ||
        pStFileTypes->contains(XBinary::FT_MACHO) || pStFileTypes->contains(XBinary::FT_MACHO32) || pStFileTypes->contains(XBinary::FT_MACHO64) ||
        pStFileTypes->contains(XBinary::FT_DEX) || pStFileTypes->contains(XBinary::FT_ZIP) || pStFileTypes->contains(XBinary::FT_GZIP) ||
        pStFileTypes->contains(XBinary::FT_ZLIB) || pStFileTypes->contains(XBinary::FT_LHA) || pStFileTypes->contains(XBinary::FT_ARJ) ||
        pStFileTypes->contains(XBinary::FT_AMIGAHUNK) || pStFileTypes->contains(XBinary::FT_ATARIST)) {
        XBinary::removeFileTypes(pStFileTypes);
    }
}

void XBinary::removeFileTypes(QSet<XBinary::FT> *pStFileTypes)
{
    pStFileTypes->remove(XBinary::FT_BINARY);
}

void XBinary::filterFileTypes(QSet<XBinary::FT> *pStFileTypes, XBinary::FT fileType)
{
    if (fileType != XBinary::FT_UNKNOWN) {
        QSet<XBinary::FT> stFileTypesNew;

        if (fileType == XBinary::FT_PE) {
            if (pStFileTypes->contains(XBinary::FT_PE)) stFileTypesNew.insert(XBinary::FT_PE);
            if (pStFileTypes->contains(XBinary::FT_PE32)) stFileTypesNew.insert(XBinary::FT_PE32);
            if (pStFileTypes->contains(XBinary::FT_PE64)) stFileTypesNew.insert(XBinary::FT_PE64);
        } else if (fileType == XBinary::FT_ELF) {
            if (pStFileTypes->contains(XBinary::FT_ELF)) stFileTypesNew.insert(XBinary::FT_ELF);
            if (pStFileTypes->contains(XBinary::FT_ELF32)) stFileTypesNew.insert(XBinary::FT_ELF32);
            if (pStFileTypes->contains(XBinary::FT_ELF64)) stFileTypesNew.insert(XBinary::FT_ELF64);
        } else if (fileType == XBinary::FT_MACHO) {
            if (pStFileTypes->contains(XBinary::FT_MACHO)) stFileTypesNew.insert(XBinary::FT_MACHO);
            if (pStFileTypes->contains(XBinary::FT_MACHO32)) stFileTypesNew.insert(XBinary::FT_MACHO32);
            if (pStFileTypes->contains(XBinary::FT_MACHO64)) stFileTypesNew.insert(XBinary::FT_MACHO64);
        } else {
            if (pStFileTypes->contains(fileType)) stFileTypesNew.insert(fileType);
        }

        *pStFileTypes = stFileTypesNew;
    }
}

bool XBinary::isFileTypePresent(QSet<XBinary::FT> *pStFileTypes, QSet<XBinary::FT> *pStAvailableFileTypes)
{
    bool bResult = false;

    QSet<XBinary::FT>::iterator i = pStFileTypes->begin();

    while (i != pStFileTypes->end()) {
        if (pStAvailableFileTypes->contains(*i)) {
            bResult = true;

            break;
        }

        i++;
    }

    return bResult;
}

XBinary::PERCENTAGE XBinary::percentageInit(qint64 nMaxValue, bool bTimer)
{
    PERCENTAGE result = {};
    result.bTimer = bTimer;

    result.nMaxValue = nMaxValue;

    if (!(result.bTimer)) {
        result.nMaxPercentage = 1;

        if (result.nMaxValue > 0x100000000) {
            result.nMaxPercentage = 100;
        } else if (result.nMaxValue > 0x100000) {
            result.nMaxPercentage = 10;
        } else if (result.nMaxValue > 0x1000) {
            result.nMaxPercentage = 5;
        }
    } else {
        result.timer.start();
        result.nMaxPercentage = 100;
    }

    return result;
}

bool XBinary::percentageSetCurrentValue(XBinary::PERCENTAGE *pPercentage, qint64 nCurrentValue)
{
    bool bResult = false;

    pPercentage->nCurrentValue = nCurrentValue;

    if (!(pPercentage->bTimer)) {
        if (pPercentage->nCurrentValue > ((pPercentage->nCurrentPercentage + 1) * (pPercentage->nMaxValue / pPercentage->nMaxPercentage))) {
            pPercentage->nCurrentPercentage++;
            bResult = true;
        }
    } else {
        if (pPercentage->timer.elapsed() >= 1000)  // TODO Check speed
        {
            pPercentage->timer.restart();

            qint32 _nCurrent = (pPercentage->nCurrentValue * 100) / (pPercentage->nMaxValue);

            pPercentage->nCurrentPercentage = _nCurrent;
            bResult = true;
        }
    }

    return bResult;
}

qint64 XBinary::getTotalOSSize(QList<OFFSETSIZE> *pListOffsetSize)
{
    qint64 nResult = 0;

    qint32 nNumberOfRecords = pListOffsetSize->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        nResult += pListOffsetSize->at(i).nSize;
    }

    return nResult;
}

XBinary::MODE XBinary::getWidthModeFromSize_32_64(quint64 nSize)
{
    MODE result = MODE_32;

    if (((quint64)nSize) >= 0xFFFFFFFF) {
        result = MODE_64;
    }

    return result;
}

quint32 XBinary::getByteSizeFromWidthMode(MODE mode)
{
    quint32 nResult = 0;

    switch (mode) {
        case MODE_8: nResult = 1; break;
        case MODE_16: nResult = 2; break;
        case MODE_32: nResult = 4; break;
        case MODE_64: nResult = 8; break;
        default: nResult = 0;
    }

    return nResult;
}

XBinary::MODE XBinary::getWidthModeFromSize(quint64 nSize)
{
    MODE result = MODE_32;

    if (((quint64)nSize) >= 0xFFFFFFFF) {
        result = MODE_64;
    } else if (((quint64)nSize) >= 0xFFFF) {
        result = MODE_32;
    } else if (((quint64)nSize) >= 0xFF) {
        result = MODE_16;
    } else {
        result = MODE_8;
    }

    return result;
}

XBinary::MODE XBinary::getWidthModeFromMemoryMap(XBinary::_MEMORY_MAP *pMemoryMap)
{
    MODE result = MODE_32;

    qint64 nMax = qMax(pMemoryMap->nModuleAddress + pMemoryMap->nImageSize, (XADDR)(pMemoryMap->nBinarySize));

    result = getWidthModeFromSize(nMax);

    return result;
}

XBinary::MODE XBinary::getWidthModeFromByteSize(quint32 nByteSize)
{
    MODE result = MODE_32;

    if (nByteSize == 2) {
        result = MODE_8;
    } else if (nByteSize == 4) {
        result = MODE_16;
    } else if (nByteSize == 8) {
        result = MODE_32;
    } else if (nByteSize == 16) {
        result = MODE_64;
    }

    return result;
}

bool XBinary::isAnsiSymbol(quint8 cCode, bool bExtra)
{
    bool bResult = false;

    if (!bExtra) {
        if ((cCode >= 20) && (cCode < 0x80)) {
            bResult = true;
        }
    } else {
        if (cCode >= 20) {
            bResult = true;
        }
    }

    return bResult;
}

bool XBinary::isUTF8Symbol(quint8 cCode, qint32 *pnWidth)
{
    bool bResult = false;

    if (cCode >= 20) {
        if ((cCode >> 7) == 0) {
            *pnWidth = 1;
            bResult = true;
        } else if ((cCode >> 5) == 0x6) {
            *pnWidth = 2;
            bResult = true;
        } else if ((cCode >> 4) == 0xE) {
            *pnWidth = 3;
            bResult = true;
        } else if ((cCode >> 3) == 0x1E) {
            *pnWidth = 4;
            bResult = true;
        } else if ((cCode >> 6) == 0x2)  // Rest bytes
        {
            *pnWidth = 0;
            bResult = true;
        }
    }

    return bResult;
}

bool XBinary::isUnicodeSymbol(quint16 nCode, bool bExtra)
{
    bool bResult = false;

    if (!bExtra) {
        if ((nCode >= 20) && (nCode < 0x80)) {
            bResult = true;
        }
    } else {
        if ((nCode >= 20) && (nCode <= 0xFF)) {
            bResult = true;
        } else if ((nCode >= 0x0400) && (nCode <= 0x04FF))  // Cyrillic
        {
            bResult = true;
        }
    }

    return bResult;
}

QString XBinary::getStringFromIndex(qint64 nOffset, qint64 nSize, qint32 nIndex)
{
    QString sResult;

    if (nIndex < nSize) {
        sResult = read_ansiString(nOffset + nIndex);
    }

    return sResult;
}

QList<QString> XBinary::getAllFilesFromDirectory(const QString &sDirectory, const QString &sExtension)
{
    QDir directory(sDirectory);
    QStringList fileList = directory.entryList(QStringList() << sExtension, QDir::Files);
    QList<QString> listFileNames;

    qint32 nNumberOfFiles = fileList.size();

    for (int i = 0; i < nNumberOfFiles; ++i) {
        listFileNames.append(directory.absoluteFilePath(fileList.at(i)));
    }

    return listFileNames;
}

QList<XBinary::OPCODE> XBinary::getOpcodes(qint64 nOffset, XADDR nStartAddress, qint64 nSize, quint32 nType, PDSTRUCT *pPdStruct)
{
    PDSTRUCT pdStructEmpty = XBinary::createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    QList<OPCODE> listResult;

    OFFSETSIZE osRegion = convertOffsetAndSize(nOffset, nSize);

    nOffset = osRegion.nOffset;
    nSize = osRegion.nSize;

    if (nOffset != -1) {
        qint32 nBufferSize = getBufferSize(pPdStruct);
        char *pBuffer = new char[nBufferSize];

        while ((nSize > 0) && (!(pPdStruct->bIsStop))) {
            qint64 nTempSize = qMin((qint64)nBufferSize, nSize);

            if (read_array(nOffset, pBuffer, nTempSize) != nTempSize) {
                setPdStructErrorString(pPdStruct, tr("Read error"));

                break;
            }

            qint64 nOpcodesSize = 0;
            OPCODE_STATUS opcodeStatus = OPCODE_STATUS_SUCCESS;

            for (qint32 i = 0; i < nTempSize;) {
                qint64 _nSize = readOpcodes(nType, pBuffer + i, nStartAddress + i, nTempSize - i, &listResult, &opcodeStatus);

                i += _nSize;
                nOpcodesSize += _nSize;

                if ((_nSize == 0) || (opcodeStatus == OPCODE_STATUS_END)) {
                    break;
                }
            }

            if ((nOpcodesSize == 0) || (opcodeStatus == OPCODE_STATUS_END)) {
                //                _errorMessage(tr("Read error"));
                break;
            }

            nSize -= nOpcodesSize;
            nOffset += nOpcodesSize;
            nStartAddress += nOpcodesSize;
        }

        delete[] pBuffer;
    }

    return listResult;
}

XADDR XBinary::readOpcodes(quint32 nType, char *pData, XADDR nStartAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus)
{
    Q_UNUSED(nType)
    Q_UNUSED(pData)
    Q_UNUSED(nStartAddress)
    Q_UNUSED(nSize)
    Q_UNUSED(pListOpcodes)

    *pOpcodeStatus = OPCODE_STATUS_END;

    return false;
}

bool XBinary::_read_opcode_uleb128(OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix)
{
    bool bResult = false;

    if (*pnSize > 0) {
        PACKED_UINT uleb128 = _read_uleb128(*ppData, *pnSize);

        if (uleb128.bIsValid) {
            pOpcode->nAddress = *pnAddress;
            pOpcode->nSize = uleb128.nByteSize;
            pOpcode->sName = QString("%1(%2)").arg(sPrefix, QString::number((qint64)uleb128.nValue));

            *pnSize -= pOpcode->nSize;
            *ppData += pOpcode->nSize;
            *pnResult += pOpcode->nSize;
            *pnAddress += pOpcode->nSize;

            bResult = true;
        }
    }

    return bResult;
}

bool XBinary::_read_opcode_ansiString(XBinary::OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix)
{
    bool bResult = false;

    if (*pnSize > 0) {
        QString sString = *ppData;

        if (sString.size() < (*pnSize))  // We need cstring with \0
        {
            pOpcode->nAddress = *pnAddress;
            pOpcode->nSize = sString.size() + 1;
            pOpcode->sName = QString("%1(\"%2\")").arg(sPrefix, sString);

            *pnSize -= pOpcode->nSize;
            *ppData += pOpcode->nSize;
            *pnResult += pOpcode->nSize;
            *pnAddress += pOpcode->nSize;

            bResult = true;
        }
    }

    return bResult;
}

QList<quint32> XBinary::get_uint32_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian)
{
    QList<quint32> listResult;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        quint32 nRecord = read_uint32(nOffset + i * sizeof(quint32), bIsBigEndian);

        listResult.append(nRecord);
    }

    return listResult;
}

QList<quint64> XBinary::get_uint64_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian)
{
    QList<quint64> listResult;

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        quint64 nRecord = read_uint64(nOffset + i * sizeof(quint64), bIsBigEndian);

        listResult.append(nRecord);
    }

    return listResult;
}

bool XBinary::_isOffsetsCrossed(qint64 nOffset1, qint64 nSize1, qint64 nOffset2, qint64 nSize2)
{
    bool bResult = false;

    if (((nOffset2 >= nOffset1) && ((nOffset1 + nSize1) > nOffset2)) || ((nOffset1 >= nOffset2) && ((nOffset2 + nSize2) > nOffset1))) {
        bResult = true;
    }

    return bResult;
}

bool XBinary::_isAddressCrossed(XADDR nAddress1, qint64 nSize1, XADDR nAddress2, qint64 nSize2)
{
    bool bResult = false;

    // #ifdef QT_DEBUG
    //     if (nAddress1 == 0x771F8164) {
    //         bResult = false;
    //     }
    // #endif

    if (((nAddress2 >= nAddress1) && ((nAddress1 + nSize1) > nAddress2)) || ((nAddress1 >= nAddress2) && ((nAddress2 + nSize2) > nAddress1))) {
        bResult = true;
    }

    return bResult;
}

bool XBinary::_isReplaced(qint64 nOffset, qint64 nSize, QList<XBinary::MEMORY_REPLACE> *pListMemoryReplace)
{
    bool bResult = false;

    if (nSize) {
        qint32 nNumberOfRecords = pListMemoryReplace->count();

        for (qint32 i = 0; i < nNumberOfRecords; i++) {
            if (_isOffsetsCrossed(nOffset, nSize, pListMemoryReplace->at(i).nOffset, pListMemoryReplace->at(i).nSize)) {
                bResult = true;
                break;
            }
        }
    }

    return bResult;
}

bool XBinary::_replaceMemory(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<XBinary::MEMORY_REPLACE> *pListMemoryReplace)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pListMemoryReplace->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (_isOffsetsCrossed(nDataOffset, nDataSize, pListMemoryReplace->at(i).nOffset, pListMemoryReplace->at(i).nSize)) {
            MEMORY_REPLACE memoryReplace = pListMemoryReplace->at(i);

            for (qint32 j = 0; j < memoryReplace.nSize; j++) {
                bResult = true;

                pData[(memoryReplace.nOffset + j) - nDataOffset] = memoryReplace.baOriginal.data()[j];

                if ((memoryReplace.nOffset + j) > (nDataOffset + nDataSize)) {
                    break;
                }
            }
        }
    }

    return bResult;
}

bool XBinary::_updateReplaces(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<MEMORY_REPLACE> *pListMemoryReplace)
{
    bool bResult = false;

    qint32 nNumberOfRecords = pListMemoryReplace->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (_isOffsetsCrossed(nDataOffset, nDataSize, pListMemoryReplace->at(i).nOffset, pListMemoryReplace->at(i).nSize)) {
            MEMORY_REPLACE memoryReplace = pListMemoryReplace->at(i);

            for (qint32 j = 0; j < memoryReplace.nSize; j++) {
                bResult = true;

                memoryReplace.baOriginal.data()[j] = pData[(memoryReplace.nOffset + j) - nDataOffset];

                if ((memoryReplace.nOffset + j) > (nDataOffset + nDataSize)) {
                    break;
                }
            }
        }
    }

    return bResult;
}

QList<XBinary::SYMBOL_RECORD> XBinary::getSymbolRecords(XBinary::_MEMORY_MAP *pMemoryMap, SYMBOL_TYPE symbolType)
{
    Q_UNUSED(pMemoryMap)
    Q_UNUSED(symbolType)

    QList<XBinary::SYMBOL_RECORD> listResult;

    // TODO

    return listResult;
}

XBinary::SYMBOL_RECORD XBinary::findSymbolByAddress(QList<SYMBOL_RECORD> *pListSymbolRecords, XADDR nAddress)
{
    SYMBOL_RECORD result = {};

    qint32 nNumberOfRecords = pListSymbolRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListSymbolRecords->at(i).nAddress == nAddress) {
            result = pListSymbolRecords->at(i);

            break;
        }
    }

    return result;
}

XBinary::SYMBOL_RECORD XBinary::findSymbolByName(QList<SYMBOL_RECORD> *pListSymbolRecords, const QString &sName)
{
    SYMBOL_RECORD result = {};

    qint32 nNumberOfRecords = pListSymbolRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListSymbolRecords->at(i).sName == sName) {
            result = pListSymbolRecords->at(i);

            break;
        }
    }

    return result;
}

XBinary::SYMBOL_RECORD XBinary::findSymbolByOrdinal(QList<SYMBOL_RECORD> *pListSymbolRecords, qint32 nOrdinal)
{
    SYMBOL_RECORD result = {};

    qint32 nNumberOfRecords = pListSymbolRecords->count();

    for (qint32 i = 0; i < nNumberOfRecords; i++) {
        if (pListSymbolRecords->at(i).nOrdinal == nOrdinal) {
            result = pListSymbolRecords->at(i);

            break;
        }
    }

    return result;
}

QString XBinary::generateUUID()
{
    return QUuid::createUuid().toString().remove("{").remove("}");
}

QString XBinary::appendText(const QString &sResult, const QString &sString, const QString &sSeparate)
{
    QString _sResult = sResult;

    if (sString != "") {
        if (_sResult != "") _sResult += sSeparate;
        _sResult += sString;
    }

    return _sResult;
}

QString XBinary::appendComma(const QString &sResult, const QString &sString)
{
    return appendText(sResult, sString, ", ");
}

QString XBinary::bytesCountToString(quint64 nValue, quint64 nBase)
{
    QString sResult;

    if (nBase != 1024) {
        nBase = 1000;
    }

    QString sValue;
    QString sUnit;

    if (nValue < nBase) {
        sValue = QString::number(nValue);
        sUnit = tr("Bytes");
    } else if (nValue < (nBase * nBase)) {
        sValue = QString::number((double)nValue / nBase, 'f', 2);

        if (nBase == 1024) sUnit = tr("KiB");
        else tr("kB");
    } else if (nValue < (nBase * nBase * nBase)) {
        sValue = QString::number((double)nValue / (nBase * nBase), 'f', 2);

        if (nBase == 1024) sUnit = tr("MiB");
        else tr("MB");
    } else if (nValue < (nBase * nBase * nBase * nBase)) {
        sValue = QString::number((double)nValue / (nBase * nBase * nBase), 'f', 2);

        if (nBase == 1024) sUnit = tr("GiB");
        else tr("GB");
    } else {
        sValue = QString::number((double)nValue / (nBase * nBase * nBase * nBase), 'f', 2);

        if (nBase == 1024) sUnit = tr("TiB");
        else tr("TB");
    }

    sResult = QString("%1 %2").arg(sValue, sUnit);

    return sResult;
}

QString XBinary::numberToString(quint64 nValue)
{
    QString sResult;

    sResult = QString::number(nValue);

    return sResult;
}

QString XBinary::fullVersionByteToString(quint8 nValue)
{
    return QString("\"%1\"").arg(get_uint8_full_version(nValue));
}

QString XBinary::fullVersionWordToString(quint16 nValue)
{
    return QString("\"%1\"").arg(get_uint16_full_version(nValue));
}

QString XBinary::fullVersionDwordToString(quint32 nValue)
{
    return QString("\"%1\"").arg(get_uint32_full_version(nValue));
}

QString XBinary::fullVersionQwordToString(quint64 nValue)
{
    return QString("\"%1\"").arg(get_uint64_full_version(nValue));
}

QString XBinary::versionWordToString(quint16 nValue)
{
    return QString("\"%1\"").arg(get_uint16_version(nValue));
}

QString XBinary::versionDwordToString(quint32 nValue)
{
    return QString("\"%1\"").arg(get_uint32_version(nValue));
}

QString XBinary::versionDivModByteToString(quint8 nValue)
{
    return QString("\"%1.%2\"").arg(nValue / 10).arg(nValue % 10);
}

QString XBinary::versionDivModWordToString(quint16 nValue)
{
    quint8 nHigh = (nValue >> 8) & 0xFF;
    quint8 nLow = nValue & 0xFF;
    return QString("\"%1.%2.%3.%4\"").arg(nHigh / 10).arg(nHigh % 10).arg(nLow / 10).arg(nLow % 10);
}

QString XBinary::formatXML(const QString &sXML)
{
    QString sResult;

    QXmlStreamReader reader(sXML);
    QXmlStreamWriter writer(&sResult);
    writer.setAutoFormatting(true);

    while (!reader.atEnd()) {
        reader.readNext();
        if (!reader.isWhitespace()) {
            writer.writeCurrentToken(reader);
        }
    }

    return sResult;
}

quint32 XBinary::make_dword(XBinary::XDWORD xdword)
{
    quint32 nResult = 0;

    nResult += (xdword.nValue1) << 16;
    nResult += xdword.nValue2;

    return nResult;
}

quint32 XBinary::make_dword(quint16 nValue1, quint16 nValue2)
{
    XDWORD xdword = {};
    xdword.nValue1 = nValue1;
    xdword.nValue2 = nValue2;

    return make_dword(xdword);
}

XBinary::XDWORD XBinary::make_xdword(quint32 nValue)
{
    XDWORD result = {};

    result.nValue1 = (quint16)(nValue >> 16);
    result.nValue2 = (quint16)(nValue & 0xFFFF);

    return result;
}

QString XBinary::recordFilePartIdToString(FILEPART id)
{
    return XCONVERT_idToTransString(id, _TABLE_XBinary_FILEPART, sizeof(_TABLE_XBinary_FILEPART) / sizeof(XBinary::XCONVERT));
}

QString XBinary::recordFilePartIdToFtString(FILEPART id)
{
    return XCONVERT_idToFtString(id, _TABLE_XBinary_FILEPART, sizeof(_TABLE_XBinary_FILEPART) / sizeof(XBinary::XCONVERT));
}

bool XBinary::checkVersionString(const QString &sVersion)
{
    bool bResult = false;

    if (sVersion.trimmed() != "") {
        bResult = true;

        qint32 nStringSize = sVersion.size();

        // TODO
        for (qint32 i = 0; i < nStringSize; i++) {
            QChar _char = sVersion.at(i);

            if ((_char >= QChar('0')) && (_char <= QChar('9'))) {
            } else if (_char == QChar('.')) {
            } else {
                bResult = false;
                break;
            }
        }
    }

    return bResult;
}

QString XBinary::cleanString(const QString &sString)
{
    QString sResult;

    qint32 nNumberOfChars = sString.size();

    for (qint32 i = 0; i < nNumberOfChars; i++) {
        QChar _char = sString.at(i);

        bool bAdd = false;

        if (_char.isLetterOrNumber() || _char.isPunct()) {
            bAdd = true;
        }

        if (bAdd) {
            sResult.append(_char);
        }
    }

    return sResult;
}

XBinary::XVARIANT XBinary::getXVariant(bool bValue)
{
    XVARIANT result = {};

    result.varType = VT_BIT;
    result.var = bValue;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint8 nValue)
{
    XVARIANT result = {};

    result.varType = VT_BYTE;
    result.var = nValue;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint16 nValue, bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_WORD;
    result.var = nValue;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint32 nValue, bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_DWORD;
    result.var = nValue;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint64 nValue, bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_QWORD;
    result.var = nValue;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint64 nValue[2], bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_128;
    // result.var.v_uint128[0] = nValue[0];
    // result.var.v_uint128[1] = nValue[1];

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint64 nLow, quint64 nHigh, bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_128;
    // result.var.v_uint128[0] = nLow;
    // result.var.v_uint128[1] = nHigh;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint64 nLow1, quint64 nLow2, quint64 nHigh1, quint64 nHigh2, bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_256;
    // result.var.v_uint256[0] = nLow1;
    // result.var.v_uint256[1] = nLow2;
    // result.var.v_uint256[2] = nHigh1;
    // result.var.v_uint256[3] = nHigh2;

    return result;
}

XBinary::XVARIANT XBinary::getXVariant(quint8 nValue[10], bool bIsBigEndian)
{
    XVARIANT result = {};

    result.bIsBigEndian = bIsBigEndian;
    result.varType = VT_FPEG;

    // for (qint32 i = 0; i < 10; i++) {
    //     result.var.v_freg[i] = nValue[i];
    // }

    return result;
}

quint64 XBinary::xVariantToQword(const XVARIANT &xvariant)
{
    quint64 nResult = 0;

    if ((xvariant.varType == VT_BYTE) || (xvariant.varType == VT_WORD) || (xvariant.varType == VT_DWORD) || (xvariant.varType == VT_QWORD)) {
        nResult = xvariant.var.toULongLong();
    }

    return nResult;
}

quint32 XBinary::getDwordFromQword(quint64 nValue, qint32 nIndex)
{
    quint32 nResult = 0;

    if (nIndex < 2) {
        nValue = nValue >> (nIndex * 32);

        nResult = nValue & 0xFFFFFFFF;
    }

    return nResult;
}

quint16 XBinary::getWordFromQword(quint64 nValue, qint32 nIndex)
{
    quint16 nResult = 0;

    if (nIndex < 4) {
        nValue = nValue >> (nIndex * 16);

        nResult = nValue & 0xFFFF;
    }

    return nResult;
}

quint8 XBinary::getByteFromQword(quint64 nValue, qint32 nIndex)
{
    quint8 nResult = 0;

    if (nIndex < 8) {
        nValue = nValue >> (nIndex * 8);

        nResult = nValue & 0xFF;
    }

    return nResult;
}

quint16 XBinary::getWordFromDword(quint32 nValue, qint32 nIndex)
{
    quint16 nResult = 0;

    if (nIndex < 2) {
        nValue = nValue >> (nIndex * 16);

        nResult = nValue & 0xFFFF;
    }

    return nResult;
}

quint8 XBinary::getByteFromDword(quint32 nValue, qint32 nIndex)
{
    quint8 nResult = 0;

    if (nIndex < 4) {
        nValue = nValue >> (nIndex * 8);

        nResult = nValue & 0xFF;
    }

    return nResult;
}

quint8 XBinary::getByteFromWord(quint16 nValue, qint32 nIndex)
{
    quint8 nResult = 0;

    if (nIndex < 2) {
        nValue = nValue >> (nIndex * 8);

        nResult = nValue & 0xFF;
    }

    return nResult;
}

bool XBinary::getBitFromByte(quint8 nValue, qint32 nIndex)
{
    bool bResult = false;

    if (nIndex < 8) {
        bResult = (nValue >> nIndex) & 0x1;
    }

    return bResult;
}

bool XBinary::getBitFromWord(quint16 nValue, qint32 nIndex)
{
    bool bResult = false;

    if (nIndex < 16) {
        bResult = (nValue >> nIndex) & 0x1;
    }

    return bResult;
}

bool XBinary::getBitFromDword(quint32 nValue, qint32 nIndex)
{
    bool bResult = false;

    if (nIndex < 32) {
        bResult = (nValue >> nIndex) & 0x1;
    }

    return bResult;
}

bool XBinary::getBitFromQword(quint64 nValue, qint32 nIndex)
{
    bool bResult = false;

    if (nIndex < 64) {
        bResult = (nValue >> nIndex) & 0x1;
    }

    return bResult;
}

quint64 XBinary::setDwordToQword(quint64 nInit, quint32 nValue, qint32 nIndex)
{
    quint64 nResult = nInit;

    if (nIndex < 2) {
        quint64 nFF = 0xFFFFFFFF;
        quint64 _nValue = nValue;

        nFF = nFF << (nIndex * 32);
        _nValue = _nValue << (nIndex * 32);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint64 XBinary::setWordToQword(quint64 nInit, quint16 nValue, qint32 nIndex)
{
    quint64 nResult = nInit;

    if (nIndex < 4) {
        quint64 nFF = 0xFFFF;
        quint64 _nValue = nValue;

        nFF = nFF << (nIndex * 16);
        _nValue = _nValue << (nIndex * 16);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint64 XBinary::setByteToQword(quint64 nInit, quint8 nValue, qint32 nIndex)
{
    quint64 nResult = nInit;

    if (nIndex < 8) {
        quint64 nFF = 0xFF;
        quint64 _nValue = nValue;

        nFF = nFF << (nIndex * 8);
        _nValue = _nValue << (nIndex * 8);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint32 XBinary::setWordToDword(quint32 nInit, quint16 nValue, qint32 nIndex)
{
    quint32 nResult = nInit;

    if (nIndex < 2) {
        quint32 nFF = 0xFFFF;
        quint32 _nValue = nValue;

        nFF = nFF << (nIndex * 16);
        _nValue = _nValue << (nIndex * 16);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint32 XBinary::setByteToDword(quint32 nInit, quint8 nValue, qint32 nIndex)
{
    quint32 nResult = nInit;

    if (nIndex < 4) {
        quint32 nFF = 0xFF;
        quint32 _nValue = nValue;

        nFF = nFF << (nIndex * 8);
        _nValue = _nValue << (nIndex * 8);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint16 XBinary::setByteToWord(quint16 nInit, quint8 nValue, qint32 nIndex)
{
    quint16 nResult = nInit;

    if (nIndex < 2) {
        quint16 nFF = 0xFF;
        quint16 _nValue = nValue;

        nFF = nFF << (nIndex * 8);
        _nValue = _nValue << (nIndex * 8);

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint8 XBinary::setBitToByte(quint8 nInit, bool bValue, qint32 nIndex)
{
    quint8 nResult = nInit;

    if (nIndex < 8) {
        quint8 nFF = 0x01;
        quint8 _nValue = (quint8)bValue;

        nFF = nFF << nIndex;
        _nValue = _nValue << nIndex;

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint16 XBinary::setBitToWord(quint16 nInit, bool bValue, qint32 nIndex)
{
    quint16 nResult = nInit;

    if (nIndex < 16) {
        quint16 nFF = 0x01;
        quint16 _nValue = (quint16)bValue;

        nFF = nFF << nIndex;
        _nValue = _nValue << nIndex;

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint32 XBinary::setBitToDword(quint32 nInit, bool bValue, qint32 nIndex)
{
    quint32 nResult = nInit;

    if (nIndex < 32) {
        quint32 nFF = 0x01;
        quint32 _nValue = (quint32)bValue;

        nFF = nFF << nIndex;
        _nValue = _nValue << nIndex;

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

quint64 XBinary::setBitToQword(quint64 nInit, bool bValue, qint32 nIndex)
{
    quint64 nResult = nInit;

    if (nIndex < 64) {
        quint64 nFF = 0x01;
        quint64 _nValue = (quint64)bValue;

        nFF = nFF << nIndex;
        _nValue = _nValue << nIndex;

        nResult = nResult & (~nFF);
        nResult = nResult | _nValue;
    }

    return nResult;
}

bool XBinary::isXVariantEqual(XVARIANT value1, XVARIANT value2)
{
    bool bResult = false;

    // if ((value1.mode == value2.mode) && (value1.bIsBigEndian == value2.bIsBigEndian)) {
    //     if (value1.mode == MODE_BIT) bResult = (value1.var.v_bool == value2.var.v_bool);
    //     else if (value1.mode == MODE_8) bResult = (value1.var.v_uint8 == value2.var.v_uint8);
    //     else if (value1.mode == MODE_16) bResult = (value1.var.v_uint16 == value2.var.v_uint16);
    //     else if (value1.mode == MODE_32) bResult = (value1.var.v_uint32 == value2.var.v_uint32);
    //     else if (value1.mode == MODE_64) bResult = (value1.var.v_uint64 == value2.var.v_uint64);
    //     else if (value1.mode == MODE_128) {
    //         bResult = (value1.var.v_uint128[0] == value2.var.v_uint128[0]) && (value1.var.v_uint128[1] == value2.var.v_uint128[1]);
    //     } else if (value1.mode == MODE_256) {
    //         bResult = true;

    //         for (qint32 i = 0; i < 4; i++) {
    //             if (value1.var.v_uint256[i] != value2.var.v_uint256[i]) {
    //                 bResult = false;
    //                 break;
    //             }
    //         }
    //     } else if (value1.mode == MODE_FREG) {
    //         bResult = true;

    //         for (qint32 i = 0; i < 10; i++) {
    //             if (value1.var.v_freg[i] != value2.var.v_freg[i]) {
    //                 bResult = false;
    //                 break;
    //             }
    //         }
    //     }
    // }

    return bResult;
}

void XBinary::clearXVariant(XVARIANT *pVar)
{
    pVar->var = 0;
}

XBinary::MODE XBinary::getModeOS()
{
    XBinary::MODE modeResult = XBinary::MODE_UNKNOWN;

    if (sizeof(void *) == 8) {
        modeResult = XBinary::MODE_64;
    } else {
        modeResult = XBinary::MODE_32;
    }

    return modeResult;
}

XBinary::PDSTRUCT XBinary::createPdStruct()
{
    PDSTRUCT result = {};

    result.bIsStop = false;
    result.nFinished = false;

    for (qint32 i = 0; i < N_NUMBER_PDRECORDS; i++) {
        result._pdRecord[i].bIsValid = false;
        result._pdRecord[i].nCurrent = 0;
        result._pdRecord[i].nTotal = 0;
        result._pdRecord[i].sStatus = "";
    }

    return result;
}

void XBinary::setPdStructInit(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nTotal)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].bIsValid = true;
            pPdStruct->_pdRecord[nIndex].nCurrent = 0;
            pPdStruct->_pdRecord[nIndex].nTotal = nTotal;
            pPdStruct->_pdRecord[nIndex].sStatus.clear();
        }
    }
}

void XBinary::setPdStructTotal(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].nTotal = nValue;
        }
    }
}

void XBinary::setPdStructCurrent(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].nCurrent = nValue;
            invokePdStructCallback(pPdStruct);
        }
    }
}

void XBinary::setPdStructCurrentIncrement(PDSTRUCT *pPdStruct, qint32 nIndex)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].nCurrent++;
            invokePdStructCallback(pPdStruct);
        }
    }
}

void XBinary::setPdStructStatus(PDSTRUCT *pPdStruct, qint32 nIndex, const QString &sStatus)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].sStatus = sStatus;
        }
    }
}

void XBinary::setPdStructFinished(PDSTRUCT *pPdStruct, qint32 nIndex)
{
    if (pPdStruct) {
        if ((nIndex >= 0) && (nIndex < N_NUMBER_PDRECORDS)) {
            pPdStruct->_pdRecord[nIndex].bIsValid = false;
            pPdStruct->_pdRecord[nIndex].nCurrent = 0;
            pPdStruct->_pdRecord[nIndex].nTotal = 0;
            pPdStruct->_pdRecord[nIndex].sStatus.clear();

            pPdStruct->nFinished++;
        }
    }
}

void XBinary::setPdStructInfoString(PDSTRUCT *pPdStruct, const QString &sInfoString)
{
    if (pPdStruct) {
        pPdStruct->sInfoString = sInfoString;
    }
}

void XBinary::setPdStructErrorString(PDSTRUCT *pPdStruct, const QString &sErrorString)
{
    if (pPdStruct) {
        pPdStruct->sErrorString = sErrorString;
    }
}

void XBinary::clearPdStructInfoString(PDSTRUCT *pPdStruct)
{
    if (pPdStruct) {
        pPdStruct->sInfoString.clear();
    }
}

void XBinary::clearPdStructErrorString(PDSTRUCT *pPdStruct)
{
    if (pPdStruct) {
        pPdStruct->sErrorString.clear();
    }
}

QString XBinary::getPdStructInfoString(PDSTRUCT *pPdStruct)
{
    QString sResult;

    if (pPdStruct) {
        sResult = pPdStruct->sInfoString;
    }

    return sResult;
}

QString XBinary::getPdStructErrorString(PDSTRUCT *pPdStruct)
{
    QString sResult;

    if (pPdStruct) {
        sResult = pPdStruct->sErrorString;
    }

    return sResult;
}

qint32 XBinary::getFreeIndex(PDSTRUCT *pPdStruct)
{
    qint32 nResult = -1;

    if (pPdStruct) {
        for (qint32 i = 0; i < N_NUMBER_PDRECORDS; i++) {
            if (!pPdStruct->_pdRecord[i].bIsValid) {
                nResult = i;

                break;
            }
        }
    }

    return nResult;
}

bool XBinary::isPdStructFinished(PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    for (qint32 i = 0; i < N_NUMBER_PDRECORDS; i++) {
        if (pPdStruct->_pdRecord[i].bIsValid) {
            bResult = false;

            break;
        }
    }

    if (pPdStruct->nFinished == 0) {
        bResult = false;
    }

    return bResult;
}

bool XBinary::isPdStructNotCanceled(PDSTRUCT *pPdStruct)
{
    bool bResult = true;

    if (pPdStruct) {
        if (pPdStruct->bIsStop) {
            bResult = false;
        }
    }

    return bResult;
}

bool XBinary::isPdStructSuccess(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (isPdStructFinished(pPdStruct) && (!(pPdStruct->bIsStop))) {
        if (pPdStruct->nFinished > 0) {
            bResult = true;
        }
    }

    if (pPdStruct->bForceStop) {
        bResult = false;
    }

    return bResult;
}

bool XBinary::isPdStructStopped(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (pPdStruct) {
        if (pPdStruct->bIsStop) {
            bResult = true;
        }
    }

    return bResult;
}

void XBinary::setPdStructStopped(PDSTRUCT *pPdStruct)
{
    if (pPdStruct) {
        pPdStruct->bIsStop = true;
    }
}

qint32 XBinary::getPdStructPercentage(PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    for (qint32 i = 0; i < N_NUMBER_PDRECORDS; i++) {
        if (pPdStruct->_pdRecord[i].nTotal) {
            nResult = (pPdStruct->_pdRecord[i].nCurrent * 100) / pPdStruct->_pdRecord[i].nTotal;

            break;
        }
    }

    return nResult;
}

void XBinary::invokePdStructCallback(PDSTRUCT *pPdStruct, qint32 nMinIntervalMs)
{
    if (pPdStruct && pPdStruct->pCallback) {
        qint64 nCurrentTime = QDateTime::currentMSecsSinceEpoch();

        if ((nCurrentTime - pPdStruct->nLastCallbackTime) >= nMinIntervalMs) {
            pPdStruct->pCallback(pPdStruct->pCallbackUserData, pPdStruct);
            pPdStruct->nLastCallbackTime = nCurrentTime;
        }
    }
}

XBinary::REGION_FILL XBinary::getRegionFill(qint64 nOffset, qint64 nSize, qint32 nAlignment)
{
    REGION_FILL result = {};
    result.nByte = read_uint8(nOffset);

    qint64 nMaxOffset = qMin(m_pDevice->size(), nOffset + nSize);
    char *pData = new char[nAlignment];

    bool bError = false;

    for (qint64 nCurrentOffset = nOffset; nCurrentOffset < nMaxOffset;) {
        qint64 nDataSize = qMin((qint64)nAlignment, nMaxOffset - nCurrentOffset);

        if (nDataSize < nAlignment) {
            break;
        }

        read_array(nCurrentOffset, pData, nDataSize);

        for (qint32 i = 0; i < nDataSize; i++) {
            if ((quint8)(pData[i]) != result.nByte) {
                bError = true;
                break;
            }
        }

        if (bError) {
            break;
        }

        result.nSize += nDataSize;

        nCurrentOffset += nDataSize;
    }

    delete[] pData;

    return result;
}

QString XBinary::getDataString(char *pData, qint32 nDataSize, const QString &sBaseType, bool bIsBigEndian)
{
    // TODO optimize
    QString sResult;

    if ((sBaseType == "db") || (sBaseType == "dw") || (sBaseType == "dd") || (sBaseType == "dq")) {
        for (qint32 i = 0; i < nDataSize;) {
            if (i > 0) {
                sResult.append(", ");
            }

            QString sString;
            qint32 _nSize = 1;

            if (sBaseType == "db") {
                sString = valueToHex(*(quint8 *)(pData));
                _nSize = 1;
            } else if (sBaseType == "dw") {
                sString = valueToHex(*(quint16 *)(pData), bIsBigEndian);
                _nSize = 2;
            } else if (sBaseType == "dd") {
                sString = valueToHex(*(quint32 *)(pData), bIsBigEndian);
                _nSize = 4;
            } else if (sBaseType == "dq") {
                sString = valueToHex(*(quint64 *)(pData), bIsBigEndian);
                _nSize = 8;
            }

            sResult.append("0x" + sString);
            pData += _nSize;
            i += _nSize;
        }
    }

    return sResult;
}

quint32 XBinary::hlTypeToFParts(HLTYPE hlType)
{
    Q_UNUSED(hlType)

    return 0;
}

QList<XBinary::FPART> XBinary::getHighlights(HLTYPE hlType, PDSTRUCT *pPdStruct)
{
    QList<XBinary::FPART> listResult;

    if (hlType == HLTYPE_TOTAL) {
        FPART region = {};
        region.filePart = FILEPART_DATA;
        region.sName = tr("Total");
        region.nVirtualAddress = getBaseAddress();
        region.nVirtualSize = getImageSize();
        region.nFileOffset = 0;
        region.nFileSize = m_pDevice->size();
        listResult.append(region);
    } else {
        listResult = getFileParts(hlTypeToFParts(hlType), -1, pPdStruct);
    }

    return listResult;
}

qint64 XBinary::align_up(qint64 nValue, qint64 nAlignment)
{
    qint64 nResult = nValue;

    if (nAlignment) {
        qint64 r = nValue % nAlignment;

        if (r) {
            nResult += (nAlignment - r);
        }
    }

    return nResult;
}

qint64 XBinary::align_down(qint64 nValue, qint64 nAlignment)
{
    qint64 nResult = nValue;

    if (nAlignment) {
        qint64 r = nValue % nAlignment;

        if (r) {
            nResult -= r;
        }
    }

    return nResult;
}

char *XBinary::strCopy(char *dest, const char *src, size_t destSize)
{
    if (!dest || !src || destSize == 0) return dest;

    size_t i = 0;
    while (i + 1 < destSize && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
    return dest;
}

QString XBinary::getAndroidVersionFromApi(quint32 nAPI)
{
    QString sResult = tr("Unknown");

    // https://developer.android.com/tools/releases/platforms
    if (nAPI == 1) sResult = QString("1.0");
    else if (nAPI == 2) sResult = QString("1.1");
    else if (nAPI == 3) sResult = QString("1.5");
    else if (nAPI == 4) sResult = QString("1.6");
    else if (nAPI == 5) sResult = QString("2.0");
    else if (nAPI == 6) sResult = QString("2.0.1");
    else if (nAPI == 7) sResult = QString("2.1");
    else if (nAPI == 8) sResult = QString("2.2.X");
    else if (nAPI == 9) sResult = QString("2.3-2.3.2");
    else if (nAPI == 10) sResult = QString("2.3.3-2.3.7");
    else if (nAPI == 11) sResult = QString("3.0");
    else if (nAPI == 12) sResult = QString("3.1");
    else if (nAPI == 13) sResult = QString("3.2.X");
    else if (nAPI == 14) sResult = QString("4.0.1-4.0.2");
    else if (nAPI == 15) sResult = QString("4.0.3-4.0.4");
    else if (nAPI == 16) sResult = QString("4.1.X");
    else if (nAPI == 17) sResult = QString("4.2.X");
    else if (nAPI == 18) sResult = QString("4.3.X");
    else if (nAPI == 19) sResult = QString("4.4-4.4.4");
    else if (nAPI == 20) sResult = QString("4.4W");
    else if (nAPI == 21) sResult = QString("5.0");
    else if (nAPI == 22) sResult = QString("5.1");
    else if (nAPI == 23) sResult = QString("6.0");
    else if (nAPI == 24) sResult = QString("7.0");
    else if (nAPI == 25) sResult = QString("7.1");
    else if (nAPI == 26) sResult = QString("8.0");
    else if (nAPI == 27) sResult = QString("8.1");
    else if (nAPI == 28) sResult = QString("9.0");
    else if (nAPI == 29) sResult = QString("10.0");
    else if (nAPI == 30) sResult = QString("11.0");
    else if (nAPI == 31) sResult = QString("12.0");
    else if (nAPI == 32) sResult = QString("12.1");
    else if (nAPI == 33) sResult = QString("13.0");
    else if (nAPI == 34) sResult = QString("14.0");
    else if (nAPI == 35) sResult = QString("15.0");
    else if (nAPI == 36) sResult = QString("16.0");

    return sResult;
}

QString XBinary::getiOSVersionFromDarwin(quint32 nDarwin)
{
    QString sResult = tr("Unknown");

    if (nDarwin == 9) sResult = QString("1.X - 2.X");
    else if (nDarwin == 10) sResult = QString("3.X - 4.X");
    else if (nDarwin == 11) sResult = QString("5.X - 6.X");
    else if (nDarwin == 13) sResult = QString("7.X");
    else if (nDarwin == 14) sResult = QString("8.X");
    else if (nDarwin == 15) sResult = QString("9.X");
    else if (nDarwin == 16) sResult = QString("10.X");
    else if (nDarwin == 17) sResult = QString("11.X");
    else if (nDarwin == 18) sResult = QString("12.X");
    else if (nDarwin == 19) sResult = QString("13.X");
    else if (nDarwin == 20) sResult = QString("14.X");
    else if (nDarwin == 21) sResult = QString("15.X");
    else if (nDarwin == 22) sResult = QString("16.X");
    else if (nDarwin == 23) sResult = QString("17.X");
    else if (nDarwin == 24) sResult = QString("18.X");

    return sResult;
}

QString XBinary::_fromWCharArray(const wchar_t *pWString, qint32 size)
{
    QString sResult;

#if (QT_VERSION_MAJOR < 5)  // TODO Check
    sResult = QString::fromWCharArray(pWString, size);
    // set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Zc:wchar_t-")
#else
    sResult = QString::fromWCharArray(pWString, size);
#endif

    return sResult;
}

qint32 XBinary::_toWCharArray(const QString &sString, wchar_t *pWString)
{
    qint32 nResult = 0;

#if (QT_VERSION_MAJOR < 5)                     // TODO Check
    nResult = sString.toWCharArray(pWString);  // TODO
    // set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t-")
    // set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Zc:wchar_t-")
#else
    nResult = sString.toWCharArray(pWString);
#endif

    return nResult;
}

QString XBinary::dataToString(const QByteArray &baData, DSMODE dsmode)
{
    QString sResult;

    qint32 nSize = baData.size();
    sResult.reserve(nSize);

    for (qint32 i = 0; i < nSize; i++) {
        QChar _char = QChar(baData.at(i));
        if (dsmode == DSMODE_NONE) {
            sResult += _char;
        } else if (dsmode == DSMODE_NOPRINT_TO_DOT) {
            if (_char.isPrint()) {
                sResult += _char;
            } else {
                sResult += QChar('.');
            }
        }
    }

    return sResult;
}

QList<XBinary::SIGNATURE_RECORD> XBinary::getSignatureRecords(const QString &sSignature, bool *pbValid, PDSTRUCT *pPdStruct)
{
    QList<SIGNATURE_RECORD> listResult;

    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = 0; i < nSignatureSize;) {
        QChar cSymbol = sSignature.at(i);
        QChar cSymbol2;

        if ((cSymbol == QChar('%')) || (cSymbol == QChar('!')) || (cSymbol == QChar('_'))) {
            if ((i + 1) < (nSignatureSize)) {
                cSymbol2 = sSignature.at(i + 1);
            }
        }

        if (cSymbol == QChar('.')) {
            i += _getSignatureSkip(&listResult, sSignature, i);
        } else if (cSymbol == QChar('*')) {
            i += _getSignatureNotNull(&listResult, sSignature, i);
        } else if ((cSymbol == QChar('%')) && (cSymbol2 == QChar('%'))) {
            i += _getSignatureANSI(&listResult, sSignature, i);
        } else if ((cSymbol == QChar('%')) && (cSymbol2 == QChar('&'))) {
            i += _getSignatureANSIAndNumber(&listResult, sSignature, i);
        } else if ((cSymbol == QChar('!')) && (cSymbol2 == QChar('%'))) {
            i += _getSignatureNotANSI(&listResult, sSignature, i);
        } else if ((cSymbol == QChar('_')) && (cSymbol2 == QChar('%'))) {
            i += _getSignatureNotANSIAndNull(&listResult, sSignature, i);
        } else if (cSymbol == QChar('+')) {
            i += _getSignatureDelta(&listResult, sSignature, i, pbValid, pPdStruct);
        } else if (cSymbol == QChar('$')) {
            i += _getSignatureRelOffset(&listResult, sSignature, i);
        } else if (cSymbol == QChar('#')) {  // TODO Check []
            i += _getSignatureAddress(&listResult, sSignature, i);
        } else {
            qint32 nBytes = _getSignatureBytes(&listResult, sSignature, i, pbValid, pPdStruct);

            if (nBytes) {
                i += nBytes;
            } else {
                break;
            }
        }
    }

    return listResult;
}

bool XBinary::_compareSignature(_MEMORY_MAP *pMemoryMap, QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, qint64 nOffset, PDSTRUCT *pPdStruct)
{
    const qint64 fileSize = getSize();

    const int nNumberOfSignatures = pListSignatureRecords->count();
    for (int i = 0; i < nNumberOfSignatures; ++i) {
        const SIGNATURE_RECORD &rec = pListSignatureRecords->at(i);

        switch (rec.st) {
            case ST_COMPAREBYTES: {
                qint32 need = rec.baData.size();
                if (need <= 0 || (nOffset < 0) || (nOffset + need > fileSize)) return false;

                if (m_pConstMemory) {
                    const char *src = ((const char *)m_pConstMemory) + nOffset;
                    if (memcmp(src, rec.baData.constData(), (size_t)need) != 0) return false;
                } else {
                    QByteArray ba = read_array(nOffset, need);
                    if (ba.size() != need) return false;
                    if (!compareMemory((char *)ba.constData(), rec.baData.constData(), need)) return false;
                }
                nOffset += need;
            } break;

            case ST_NOTNULL:
            case ST_ANSI:
            case ST_NOTANSI:
            case ST_NOTANSIANDNULL:
            case ST_ANSINUMBER: {
                const int need = rec.nWindowSize;
                if (need <= 0 || (nOffset < 0) || (nOffset + need > fileSize)) return false;

                if (m_pConstMemory) {
                    char *ptr = ((char *)m_pConstMemory) + nOffset;
                    bool ok = true;
                    if (rec.st == ST_NOTNULL) ok = _isMemoryNotNull(ptr, need);
                    else if (rec.st == ST_ANSI) ok = _isMemoryAnsi(ptr, need);
                    else if (rec.st == ST_NOTANSI) ok = _isMemoryNotAnsi(ptr, need);
                    else if (rec.st == ST_NOTANSIANDNULL) ok = _isMemoryNotAnsiAndNull(ptr, need);
                    else if (rec.st == ST_ANSINUMBER) ok = _isMemoryAnsiNumber(ptr, need);
                    if (!ok) return false;
                } else {
                    QByteArray ba = read_array(nOffset, need);
                    if (ba.size() != need) return false;
                    bool ok = true;
                    if (rec.st == ST_NOTNULL) ok = _isMemoryNotNull(ba.data(), ba.size());
                    else if (rec.st == ST_ANSI) ok = _isMemoryAnsi(ba.data(), ba.size());
                    else if (rec.st == ST_NOTANSI) ok = _isMemoryNotAnsi(ba.data(), ba.size());
                    else if (rec.st == ST_NOTANSIANDNULL) ok = _isMemoryNotAnsiAndNull(ba.data(), ba.size());
                    else if (rec.st == ST_ANSINUMBER) ok = _isMemoryAnsiNumber(ba.data(), ba.size());
                    if (!ok) return false;
                }
                nOffset += need;
            } break;

            case ST_FINDBYTES: {
                const qint64 limit = rec.nFindDelta + rec.baData.size();
                qint64 where = find_byteArray(nOffset, limit, rec.baData, pPdStruct);
                if (where == -1) return false;
                nOffset = where + rec.baData.size();
            } break;

            case ST_SKIP: {
                const qint64 add = rec.nWindowSize;
                if (add < 0 || (nOffset + add > fileSize)) return false;
                nOffset += add;
            } break;

            case ST_RELOFFSET: {
                qint64 nValue = 0;

                if (pMemoryMap->fileType == FT_AMIGAHUNK) {
                    switch (rec.nSizeOfAddr) {
                        case 1: nValue = 1 + read_int8(nOffset); break;
                        case 2: nValue = read_uint16(nOffset, isBigEndian(pMemoryMap)); break;
                        case 4: nValue = read_int32(nOffset, isBigEndian(pMemoryMap)); break;
                        case 8: nValue = read_int64(nOffset, isBigEndian(pMemoryMap)); break;
                        default: return false;
                    }
                } else {
                    switch (rec.nSizeOfAddr) {
                        case 1: nValue = 1 + read_int8(nOffset); break;
                        case 2: nValue = 2 + read_uint16(nOffset, isBigEndian(pMemoryMap)); break;
                        case 4: nValue = 4 + read_int32(nOffset, isBigEndian(pMemoryMap)); break;
                        case 8: nValue = 8 + read_int64(nOffset, isBigEndian(pMemoryMap)); break;
                        default: return false;
                    }
                }

                if ((pMemoryMap->fileType == FT_COM) || (pMemoryMap->fileType == FT_MSDOS)) {
                    const qint64 _nOffset = nOffset & 0xFFFF0000;
                    const qint64 _nDelta = nOffset & 0xFFFF;
                    nOffset = _nOffset + ((_nDelta + nValue) & 0xFFFF);
                } else {
                    XADDR _nAddress = offsetToAddress(pMemoryMap, nOffset);
                    _nAddress += nValue;
                    nOffset = addressToOffset(pMemoryMap, _nAddress);
                }
            } break;

            case ST_ADDRESS: {
                XADDR _nAddress = 0;
                switch (rec.nSizeOfAddr) {
                    case 1: _nAddress = read_uint8(nOffset); break;
                    case 2: _nAddress = read_uint16(nOffset, isBigEndian(pMemoryMap)); break;
                    case 4: _nAddress = read_uint32(nOffset, isBigEndian(pMemoryMap)); break;
                    case 8: _nAddress = read_uint64(nOffset, isBigEndian(pMemoryMap)); break;
                    default: return false;
                }

                if (pMemoryMap->fileType == FT_MSDOS) {
                    if (rec.nSizeOfAddr == 2) {
                        _nAddress += pMemoryMap->nCodeBase;
                        nOffset = addressToOffset(pMemoryMap, _nAddress);
                    } else if (rec.nSizeOfAddr == 4) {
                        quint16 nLow = (quint16)_nAddress;
                        quint16 nHigh = (quint16)(_nAddress >> 16);
                        nOffset = pMemoryMap->nStartLoadOffset + getSegmentAddress(nHigh, nLow);
                    }
                } else {
                    nOffset = addressToOffset(pMemoryMap, _nAddress);
                }
            } break;
        }

        if ((!isOffsetValid(pMemoryMap, nOffset)) && (!isEOD(pMemoryMap, nOffset))) {
            return false;
        }
    }

    return true;
}

qint32 XBinary::_getSignatureSkip(QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (sSignature.at(i) == QChar('.')) {
            nResult++;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_SKIP;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureNotNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (sSignature.at(i) == QChar('*')) {
            nResult++;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_NOTNULL;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i += 2) {
        if (sSignature.mid(i, 2) == "%%") {
            nResult += 2;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_ANSI;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureNotANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i += 2) {
        if (sSignature.mid(i, 2) == "!%") {
            nResult += 2;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_NOTANSI;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureNotANSIAndNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i += 2) {
        if (sSignature.mid(i, 2) == "_%") {
            nResult += 2;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_NOTANSIANDNULL;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureANSIAndNumber(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i += 2) {
        if (sSignature.mid(i, 2) == "%&") {
            nResult += 2;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_ANSINUMBER;
        record.nSizeOfAddr = 0;
        record.nWindowSize = nResult / 2;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureDelta(QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid,
                                   PDSTRUCT *pPdStruct)
{
    // TODO Check!!!
    qint32 nResult = 0;
    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (sSignature.at(i) == QChar('+')) {
            nResult++;
        } else {
            break;
        }
    }

    if (nResult) {
        QList<XBinary::SIGNATURE_RECORD> _listSignatureRecords;

        qint32 nTemp = _getSignatureBytes(&_listSignatureRecords, sSignature, nStartIndex + nResult, pbValid, pPdStruct);

        if (_listSignatureRecords.count()) {
            SIGNATURE_RECORD record = {};

            record.st = XBinary::ST_FINDBYTES;
            record.nSizeOfAddr = 0;
            record.nWindowSize = 0;
            record.baData = _listSignatureRecords.at(0).baData;
            record.nFindDelta = 32 * nResult;

            pListSignatureRecords->append(record);

            nResult += nTemp;
        }
    }

    return nResult;
}

qint32 XBinary::_getSignatureRelOffset(QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;

    qint32 nSignatureSize = sSignature.size();

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (sSignature.at(i) == QChar('$')) {
            nResult++;
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_RELOFFSET;
        record.nSizeOfAddr = nResult / 2;
        record.nWindowSize = 0;

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureAddress(QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex)
{
    qint32 nResult = 0;

    qint32 nSignatureSize = sSignature.size();
    QString sBaseAddress;
    bool bIsBaseAddress = false;
    qint32 nSizeOfAddress = 0;

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (sSignature.at(i) == QChar('#')) {
            nResult++;
            nSizeOfAddress++;
        } else if (sSignature.at(i) == QChar('[')) {
            nResult++;
            bIsBaseAddress = true;
        } else if (sSignature.at(i) == QChar(']')) {
            nResult++;
            bIsBaseAddress = false;
        } else if (bIsBaseAddress) {
            nResult++;
            sBaseAddress.append(sSignature.at(i));
        } else {
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_ADDRESS;
        record.nSizeOfAddr = nSizeOfAddress / 2;
        record.nBaseAddress = sBaseAddress.toInt(0, 16);

        pListSignatureRecords->append(record);
    }

    return nResult;
}

qint32 XBinary::_getSignatureBytes(QList<XBinary::SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid,
                                   PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0;

    qint32 nSignatureSize = sSignature.size();
    QString sBytes;

    for (qint32 i = nStartIndex; i < nSignatureSize; i++) {
        if (((sSignature.at(i) >= QChar('a')) && (sSignature.at(i) <= QChar('f'))) || ((sSignature.at(i) >= QChar('0')) && (sSignature.at(i) <= QChar('9')))) {
            nResult++;
            sBytes.append(sSignature.at(i));
        } else if ((sSignature.at(i) == '.') || (sSignature.at(i) == '$') || (sSignature.at(i) == '#') || (sSignature.at(i) == '*') || (sSignature.at(i) == '!') ||
                   (sSignature.at(i) == '_') || (sSignature.at(i) == '%') || (sSignature.at(i) == '+')) {
            break;
        } else {
            *pbValid = false;
            break;
        }
    }

    if (nResult) {
        SIGNATURE_RECORD record = {};

        record.st = XBinary::ST_COMPAREBYTES;
        record.nSizeOfAddr = 0;
        record.baData = QByteArray::fromHex(sBytes.toUtf8());  // TODO Check
        record.nWindowSize = record.baData.size();

        pListSignatureRecords->append(record);
    }

    if (!(*pbValid)) {
        setPdStructErrorString(pPdStruct, QString("%1: %2").arg(tr("Invalid signature"), sSignature));
    }

    return nResult;
}

qint64 XBinary::getPhysSize(char *pBuffer, qint64 nSize)
{
    if ((nSize <= 0) || (pBuffer == nullptr)) {
        return 0;
    }

    const qint64 wordSize = (qint64)sizeof(quint64);

    // Scan in word-sized chunks from the end for speed. Use memcpy to avoid
    // alignment UB on platforms that care.
    while (nSize >= wordSize) {
        const char *pWord = pBuffer + nSize - wordSize;
        quint64 val = 0;
        memcpy(&val, pWord, sizeof(val));

        if (val != 0) {
            // Found a non-zero word, search inside it from the end
            for (qint64 i = wordSize - 1; i >= 0; --i) {
                if (pWord[i] != 0) {
                    return (nSize - wordSize) + i + 1;
                }
            }
            // Shouldn't reach here since val != 0, but keep safe behavior
        }

        nSize -= wordSize;
    }

    // Tail: scan remaining bytes one by one
    while (nSize > 0) {
        if (pBuffer[nSize - 1] != 0) {
            break;
        }
        --nSize;
    }

    return nSize;
}

bool XBinary::isEmptyData(char *pBuffer, qint64 nSize)  // TODO dwords
{
    bool bResult = true;

    for (qint64 i = 0; i < nSize; i++) {
        char *pOffset = (pBuffer + i);

        if (*pOffset) {
            bResult = false;
            break;
        }
    }

    return bResult;
}

bool XBinary::_isOffsetValid(qint64 nOffset)
{
    qint64 nFileSize = getSize();

    return (nOffset < nFileSize);
}

bool XBinary::isAddressPhysical(XADDR nAddress)
{
    _MEMORY_MAP memoryMap = getMemoryMap();

    return isAddressPhysical(&memoryMap, nAddress);
}

bool XBinary::initUnpack(UNPACK_STATE *pState, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::unpack(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)
    Q_UNUSED(pPdStruct)

    return false;
}

XBinary::ARCHIVERECORD XBinary::infoCurrent(UNPACK_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pPdStruct)

    ARCHIVERECORD result = {};

    return result;
}

bool XBinary::unpackCurrent(UNPACK_STATE *pState, QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    if ((!pState) || (!pDevice)) {
        return false;
    }

    if ((pState->nCurrentIndex < 0) || (pState->nCurrentIndex >= pState->nNumberOfRecords)) {
        return false;
    }

    return unpack(pDevice, pPdStruct);
}

bool XBinary::moveToNext(UNPACK_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::finishUnpack(UNPACK_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pPdStruct)

    return false;
}

QList<XBinary::FPART_PROP> XBinary::getAvailableFPARTProperties()
{
    QList<XBinary::FPART_PROP> listResult;

    listResult.append(FPART_PROP_ORIGINALNAME);
    listResult.append(FPART_PROP_COMPRESSEDSIZE);
    listResult.append(FPART_PROP_UNCOMPRESSEDSIZE);
    listResult.append(FPART_PROP_STREAMOFFSET);
    listResult.append(FPART_PROP_STREAMSIZE);
    listResult.append(FPART_PROP_HANDLEMETHOD);

    return listResult;
}

bool XBinary::initPack(PACK_STATE *pState, QIODevice *pDevice, const QMap<PACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pDevice)
    Q_UNUSED(mapProperties)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::addDevice(PACK_STATE *pState, QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pDevice)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::addFile(PACK_STATE *pState, const QString &sFileName, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(sFileName)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::addFolder(PACK_STATE *pState, const QString &sDirectoryPath, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(sDirectoryPath)
    Q_UNUSED(pPdStruct)

    return false;
}

bool XBinary::finishPack(PACK_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pState)
    Q_UNUSED(pPdStruct)

    return false;
}

QVariant XBinary::calculateHash(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pDevice)
    Q_UNUSED(pPdStruct)

    return 0;
}

QVariant XBinary::calculateHash(const QString &sFileName, PDSTRUCT *pPdStruct)
{
    return XBinary::_getCRC32(sFileName, pPdStruct);
}

bool XBinary::unpackSingleStream(QIODevice *pOutDevice, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    UNPACK_STATE state = {};

    if (initUnpack(&state, mapProperties, pPdStruct)) {
        bResult = unpackCurrent(&state, pOutDevice, pPdStruct);

        finishUnpack(&state, pPdStruct);
    }

    return bResult;
}

bool XBinary::unpackToFolder(const QString &sFolderName, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    PDSTRUCT pdStructEmpty = createPdStruct();

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    if (!sFolderName.isEmpty()) {
        QDir dir;

        if (!dir.exists(sFolderName)) {
            dir.mkpath(sFolderName);
        }

        bool bFixFileNames = mapProperties.value(UNPACK_PROP_FIXFILENAMES).toBool();
        bool bNoCRC = mapProperties.value(UNPACK_PROP_NOCRC).toBool();

        // Track used filenames for duplicate detection when FIXFILENAMES is enabled
        QMap<QString, qint32> mapUsedNames;

        UNPACK_STATE state = {};

        if (initUnpack(&state, mapProperties, pPdStruct)) {
            bResult = true;

            do {
                if (isPdStructNotCanceled(pPdStruct)) {
                    ARCHIVERECORD record = infoCurrent(&state, pPdStruct);

                    QString sFileName = record.mapProperties.value(FPART_PROP_ORIGINALNAME).toString();
                    qint64 nFileSize = record.mapProperties.value(FPART_PROP_UNCOMPRESSEDSIZE).toLongLong();

                    // Check if this is a directory entry (ends with '/' and has zero size)
                    bool bIsDirectory = sFileName.endsWith(QLatin1Char('/')) || record.mapProperties.value(FPART_PROP_ISFOLDER).toBool();

                    // FIXFILENAMES: sanitize filename for current OS
                    if (bFixFileNames && !sFileName.isEmpty()) {
                        sFileName = fixFileName(sFileName);

                        // If symlink (has link target) -> treat as folder
                        QString sLinkName = record.mapProperties.value(FPART_PROP_LINKNAME).toString();
                        if (!sLinkName.isEmpty()) {
                            bIsDirectory = true;
                        }

                        // Handle duplicate filenames by appending _2, _3, etc.
                        QString sKey = sFileName.toLower();

                        if (mapUsedNames.contains(sKey)) {
                            qint32 nSuffix = mapUsedNames.value(sKey) + 1;
                            mapUsedNames.insert(sKey, nSuffix);

                            // Insert suffix before extension (or at end if no extension)
                            qint32 nLastSlash = sFileName.lastIndexOf(QLatin1Char('/'));
                            QString sDir;
                            QString sBase;

                            if (nLastSlash >= 0) {
                                sDir = sFileName.left(nLastSlash + 1);
                                sBase = sFileName.mid(nLastSlash + 1);
                            } else {
                                sBase = sFileName;
                            }

                            qint32 nDotPos = sBase.lastIndexOf(QLatin1Char('.'));

                            if (nDotPos > 0) {
                                sFileName = sDir + sBase.left(nDotPos) + QString("_%1").arg(nSuffix) + sBase.mid(nDotPos);
                            } else {
                                sFileName = sDir + sBase + QString("_%1").arg(nSuffix);
                            }
                        } else {
                            mapUsedNames.insert(sKey, 1);
                        }
                    }

                    if (!sFileName.isEmpty()) {
                        QString sFilePath = sFolderName + QDir::separator() + sFileName;

                        // Create directory structure if needed
                        if (bIsDirectory) {
                            // This is a directory entry - just create the directory
                            if (!dir.exists(sFilePath)) {
                                dir.mkpath(sFilePath);
                            }
                        } else {
                            // This is a file entry
                            QFileInfo fileInfo(sFilePath);
                            QString sDirPath = fileInfo.absolutePath();

                            if (!dir.exists(sDirPath)) {
                                dir.mkpath(sDirPath);
                            }
                        }

                        // Unpack file (skip if this is a directory entry)
                        if (bIsDirectory) {
                            bResult = XBinary::isDirectoryExists(sFilePath);
                        } else if (XBinary::isFileExists(sFilePath)) {
                            bResult = true;
                        } else if (nFileSize == 0) {
                            bResult = XBinary::createEmptyFile(sFilePath);
                        } else if (!bIsDirectory) {
                            QFile file(sFilePath);

                            if (file.open(QIODevice::WriteOnly)) {
                                if (!unpackCurrent(&state, &file, pPdStruct)) {
#ifdef QT_DEBUG
                                    qDebug() << "Cannot unpack" << sFilePath;
#endif
                                    bResult = false;
                                }

                                file.close();
                            } else {
#ifdef QT_DEBUG
                                qDebug() << "Cannot open for writing:" << sFilePath;
#endif
                                bResult = false;
                            }

                            if (bResult && !bNoCRC) {
                                XBinary::CRC_TYPE crcType =
                                    (XBinary::CRC_TYPE)record.mapProperties.value(XBinary::FPART_PROP_CRC_TYPE, XBinary::CRC_TYPE_UNKNOWN).toUInt();

                                if (crcType != XBinary::CRC_TYPE_UNKNOWN) {
                                    if (file.open(QIODevice::ReadOnly)) {
                                        QVariant varCRC = record.mapProperties.value(XBinary::FPART_PROP_RESULTCRC, 0);
                                        bool bCRCOk = checkCRC(&file, crcType, varCRC, pPdStruct);
                                        file.close();
#ifdef QT_DEBUG
                                        if (!bCRCOk) {
                                            qDebug() << "CRC is false for" << sFilePath << ": stored=" << QString::number(varCRC.toUInt(), 16);
                                        }
#endif
                                    }
                                }
                            }
                        }

                        // Set file datetime if provided by the archive record
                        if (bResult) {
                            QVariant vDateTime = record.mapProperties.value(XBinary::FPART_PROP_DATETIME);
                            if (vDateTime.isValid() && !vDateTime.isNull()) {
                                QDateTime dt;
                                if (vDateTime.canConvert<QDateTime>()) {
                                    dt = vDateTime.toDateTime();
                                } else if (vDateTime.canConvert<quint64>()) {
                                    quint64 t = vDateTime.toULongLong();
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
                                    dt = QDateTime::fromSecsSinceEpoch((qint64)t);
#else
                                    dt = QDateTime::fromMSecsSinceEpoch((qint64)t * 1000);
#endif
                                } else if (vDateTime.canConvert<qint64>()) {
                                    qint64 t = vDateTime.toLongLong();
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
                                    dt = QDateTime::fromSecsSinceEpoch(t);
#else
                                    dt = QDateTime::fromMSecsSinceEpoch(t * 1000);
#endif
                                }

                                if (dt.isValid()) {
                                    XBinary::setFileDateTime(sFilePath, dt);
                                }
                            }
                        }
                    }
                } else {
                    bResult = false;
                }

                if (!bResult) {
                    break;
                }
            } while (moveToNext(&state, pPdStruct));

            finishUnpack(&state, pPdStruct);
        }
    }

    return bResult;
}

bool XBinary::initFFSearch(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    // Initialize search state
    pState->nCurrentOffset = pState->nStartOffset;
    pState->pContext = nullptr;

    return true;
}

XBinary::FFSEARCH_INFO XBinary::searchFFNext(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct)
{
    FFSEARCH_INFO result = {};

    if (!pState) {
        return result;
    }

    QList<QString> listSignatures = getSearchSignatures();

    if (listSignatures.isEmpty()) {
        return result;
    }

    QIODevice *pDevice = pState->pDevice;

    if (!pDevice) {
        pDevice = getDevice();
    }

    if (!pDevice) {
        return result;
    }

    qint64 nTotalSize = pDevice->size();
    qint64 nStartOffset = pState->nCurrentOffset;
    qint64 nSearchSize = -1;

    if (pState->nSize > 0) {
        qint64 nSearchEnd = qMin(nTotalSize, pState->nStartOffset + pState->nSize);
        nSearchSize = nSearchEnd - nStartOffset;
    }

    while (XBinary::isPdStructNotCanceled(pPdStruct)) {
        qint64 nFoundOffset = -1;

        qint32 nCount = listSignatures.count();

        for (qint32 i = 0; i < nCount; i++) {
            QString sSignature = listSignatures.at(i);

            if (!sSignature.isEmpty()) {
                qint64 nCandidate = find_signature(nullptr, nStartOffset, nSearchSize, sSignature, nullptr, pPdStruct);

                if ((nCandidate != -1) && ((nFoundOffset == -1) || (nCandidate < nFoundOffset))) {
                    nFoundOffset = nCandidate;
                }
            }
        }

        if (nFoundOffset == -1) {
            break;
        }

        qint64 nEmbeddedSize = 0;
        FT fileType = FT_UNKNOWN;

        {
            SubDevice subdevice(pDevice, nFoundOffset, -1);

            if (subdevice.open(QIODevice::ReadOnly)) {
                XBinary *pInstance = createInstance(&subdevice);

                if (pInstance) {
                    if (pInstance->isValid(pPdStruct)) {
                        nEmbeddedSize = pInstance->getFileFormatSize(pPdStruct);
                        fileType = pInstance->getFileType();
                    }

                    delete pInstance;
                    pInstance = nullptr;
                }

                subdevice.close();
            }
        }

        if (nEmbeddedSize > 0) {
            if ((pState->nSize > 0) && ((nFoundOffset + nEmbeddedSize) > (pState->nStartOffset + pState->nSize))) {
                nStartOffset = nFoundOffset + 1;

                if (pState->nSize > 0) {
                    qint64 nSearchEnd = qMin(nTotalSize, pState->nStartOffset + pState->nSize);
                    nSearchSize = nSearchEnd - nStartOffset;
                }

                continue;
            }

            SubDevice subdevice(pDevice, nFoundOffset, nEmbeddedSize);

            if (subdevice.open(QIODevice::ReadOnly)) {
                XBinary *pInstance = createInstance(&subdevice);

                if (pInstance) {
                    XBinary::FILEFORMATINFO formatInfo = pInstance->getFileFormatInfo(pPdStruct);

                    delete pInstance;
                    pInstance = nullptr;

                    if (formatInfo.bIsValid) {
                        result.bIsValid = true;
                        result.fileTYPE = (fileType != FT_UNKNOWN) ? fileType : formatInfo.fileType;
                        result.nOffset = nFoundOffset;
                        result.nSize = nEmbeddedSize;
                        result.sExt = formatInfo.sExt;
                        result.sString = XBinary::getFileFormatString(&formatInfo);

                        pState->nCurrentOffset = nFoundOffset + nEmbeddedSize;

                        subdevice.close();
                        return result;
                    }
                }

                subdevice.close();
            }
        }

        nStartOffset = nFoundOffset + 1;

        if (pState->nSize > 0) {
            qint64 nSearchEnd = qMin(nTotalSize, pState->nStartOffset + pState->nSize);
            nSearchSize = nSearchEnd - nStartOffset;
        }
    }

    return result;
}

QList<QString> XBinary::getSearchSignatures()
{
    QList<QString> listResult;

    FT fileType = getFileType();

    if (XBinary::checkFileType(FT_PE, fileType)) {
        listResult.append("'MZ'");
    } else if (XBinary::checkFileType(FT_ELF, fileType)) {
        listResult.append("7F'ELF'");
    } else if (XBinary::checkFileType(FT_MACHOFAT, fileType)) {
        listResult.append("CAFEBABE");
        listResult.append("BEBAFECA");
    } else if (XBinary::checkFileType(FT_MACHO, fileType)) {
        listResult.append("FEEDFACE");
        listResult.append("CEFAEDFE");
        listResult.append("FEEDFACF");
        listResult.append("CFFAEDFE");
    } else if (XBinary::checkFileType(FT_PDF, fileType)) {
        listResult.append("'%PDF'");
    } else if (XBinary::checkFileType(FT_PNG, fileType)) {
        listResult.append("89'PNG\r\n'1A0A");
    } else if (XBinary::checkFileType(FT_JPEG, fileType)) {
        listResult.append("FFD8FF");
    } else if (XBinary::checkFileType(FT_TIFF, fileType)) {
        listResult.append("'MM'002A");
        listResult.append("'II'2A00");
    } else if (XBinary::checkFileType(FT_BMP, fileType)) {
        listResult.append("'BM'");
    } else if (XBinary::checkFileType(FT_GIF, fileType)) {
        listResult.append("'GIF8'");
    } else if (XBinary::checkFileType(FT_ICO, fileType)) {
        listResult.append("00000100");
    } else if (XBinary::checkFileType(FT_DEX, fileType)) {
        listResult.append("'dex\n'");
    } else if (XBinary::checkFileType(FT_ZIP, fileType)) {
        listResult.append("'PK'0304");
    } else if (XBinary::checkFileType(FT_RAR, fileType)) {
        listResult.append("'Rar!'1A07");
    } else if (XBinary::checkFileType(FT_GZIP, fileType)) {
        listResult.append("1F8B08");
    } else if (XBinary::checkFileType(FT_ZLIB, fileType)) {
        listResult.append("785E");
        listResult.append("789C");
        listResult.append("78DA");
    } else if (XBinary::checkFileType(FT_7Z, fileType)) {
        listResult.append("'7z'BCAF271C");
    } else if (XBinary::checkFileType(FT_CAB, fileType)) {
        listResult.append("'MSCF'");
    } else if (XBinary::checkFileType(FT_MP3, fileType)) {
        listResult.append("'ID3'");
    } else if (XBinary::checkFileType(FT_MP4, fileType)) {
        listResult.append("'ftyp'");
    } else if (XBinary::checkFileType(FT_RIFF, fileType)) {
        listResult.append("'RIFF'");
    } else if (XBinary::checkFileType(FT_LE, fileType)) {
        listResult.append("'MZ'");
    } else if (XBinary::checkFileType(FT_NE, fileType)) {
        listResult.append("'MZ'");
    } else if (XBinary::checkFileType(FT_AMIGAHUNK, fileType)) {
        listResult.append("000003F3");
        listResult.append("000003E7");
    } else if (XBinary::checkFileType(FT_JAVACLASS, fileType)) {
        listResult.append("CAFEBABE");
    } else if (XBinary::checkFileType(FT_SZDD, fileType)) {
        listResult.append("'SZDD'88F027'3A'");
    } else if (XBinary::checkFileType(FT_BZIP2, fileType)) {
        listResult.append("314159265359");
        listResult.append("17724538509000000000");
    } else if (XBinary::checkFileType(FT_LHA, fileType)) {
        listResult.append("'-lh'..2d");
        listResult.append("'-lz'..2d");
        listResult.append("'-pm'..2d");
    } else if (XBinary::checkFileType(FT_DJVU, fileType)) {
        listResult.append("'AT&TFORM'");
        listResult.append("'SDJVFORM'");
    }

    return listResult;
}

XBinary *XBinary::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    FT fileType = getFileType();

    if (fileType != FT_BINARY) {
        return nullptr;
    }

    return new XBinary(pDevice, bIsImage, nModuleAddress);
}

bool XBinary::finishFFSearch(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    if (!pState) {
        return false;
    }

    // Clean up search state
    pState->pDevice = nullptr;
    pState->nCurrentOffset = pState->nStartOffset;

    if (pState->pContext) {
        delete[] (char *)pState->pContext;
        pState->pContext = nullptr;
    }

    return true;
}

qint32 XBinary::getBufferSize(PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0x4000;

    if (pPdStruct) {
        if (pPdStruct->nBufferSize) {
            nResult = pPdStruct->nBufferSize;
        }
    }

#ifdef QT_DEBUG
    if (!pPdStruct) {
        qDebug("Empty PDSTRUCT!!!");
    }
#endif

    return nResult;
}

qint32 XBinary::getFileBufferSize(PDSTRUCT *pPdStruct)
{
    qint32 nResult = 0x1000000;

    if (pPdStruct) {
        if (pPdStruct->nFileBufferSize) {
            nResult = pPdStruct->nFileBufferSize;
        }
    }

#ifdef QT_DEBUG
    if (!pPdStruct) {
        qDebug("Empty PDSTRUCT!!!");
    }
#endif

    return nResult;
}

QIODevice *XBinary::createFileBuffer(qint64 nSize, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    QIODevice *pResult = nullptr;

    qint32 nFileBufferSize = 0;

    if (pPdStruct) {
        nFileBufferSize = pPdStruct->nFileBufferSize;
    }

    if (nFileBufferSize == 0) {
        nFileBufferSize = 0x1000000;  // 16 MB
    }

    if (nSize < nFileBufferSize) {
        QBuffer *pBuffer = new QBuffer();

        if (pBuffer->open(QIODevice::ReadWrite)) {
            QByteArray ba;
            ba.resize(nSize);
            pBuffer->write(ba);
            pBuffer->seek(0);  // FIX: Reset position to beginning after pre-allocating
            pResult = pBuffer;
            pResult->setProperty("Memory", (quint64)ba.data());
        } else {
            delete pBuffer;
        }
    } else {
        QTemporaryFile *pTempFile = new QTemporaryFile();
        if (pTempFile->open()) {
            pTempFile->resize(nSize);
            pResult = pTempFile;
        } else {
            delete pTempFile;
        }
    }

    return pResult;
}

void XBinary::freeFileBuffer(QIODevice **ppBuffer)
{
    if (ppBuffer && *ppBuffer) {
        delete *ppBuffer;
        *ppBuffer = nullptr;
    }
}

QString XBinary::getArchiveRecordComment(const ARCHIVERECORD &record)
{
    QString sResult;

    if (record.mapProperties.value(FPART_PROP_ISCOMMENTPRESENT).toBool()) {
        qint64 nCommentOffset = record.mapProperties.value(FPART_PROP_FILECOMMENTOFFSET).toLongLong();
        qint64 nCommentLength = record.mapProperties.value(FPART_PROP_FILECOMMENTLENGTH).toLongLong();

        Q_UNUSED(nCommentOffset)
        Q_UNUSED(nCommentLength)
        // Comment data must be read from the archive device at nCommentOffset with nCommentLength
        // This static method returns the comment text if stored directly in a property,
        // otherwise the caller must read from the device.
    }

    return sResult;
}

QString XBinary::getHandleMethods(const QMap<FPART_PROP, QVariant> &mapProperties)
{
    QString sResult;

    HANDLE_METHOD handleMethod = (HANDLE_METHOD)(mapProperties.value(XBinary::FPART_PROP_HANDLEMETHOD).toULongLong());
    HANDLE_METHOD handleMethod2 = (HANDLE_METHOD)(mapProperties.value(XBinary::FPART_PROP_HANDLEMETHOD2).toULongLong());

    // QString sHandleMethod3 = handleMethodToString(mapProperties.value(XBinary::FPART_PROP_HANDLEMETHOD3).toULongLong());

    if (handleMethod2 != HANDLE_METHOD_UNKNOWN) {
        sResult = appendText(sResult, handleMethodToString(handleMethod2), "/");
    }

    if (handleMethod != HANDLE_METHOD_UNKNOWN) {
        sResult = appendText(sResult, handleMethodToString(handleMethod), "/");
    }

    return sResult;
}
