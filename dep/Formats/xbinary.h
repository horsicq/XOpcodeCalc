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
#ifndef XBINARY_H
#define XBINARY_H

#include <QBuffer>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDataStream>
#include <QDateTime>
#include <QDirIterator>
#include <QDir>
#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QMap>
#include <QMutex>
#include <QPair>
#include <QSet>
#include <QTemporaryFile>
#include <QTextStream>
#include <QUuid>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QtEndian>
#if (QT_VERSION_MAJOR < 6) || defined(QT_CORE5COMPAT_LIB)
#include <QTextCodec>  // Qt5 Compat
#endif
#if (QT_VERSION_MAJOR < 5)  // TODO Check
#include <QRegExp>
#else
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#endif
#ifdef QT_DEBUG
#include <QDebug>
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#elif (QT_VERSION_MAJOR >= 6)  // TODO Check
#include <QRandomGenerator>
#endif

#ifdef QT_GUI_LIB
#include <QColor>
#endif

#include <math.h>

#include "subdevice.h"
#include "xbinary_def.h"
#include "xelf_def.h"
#include "xle_def.h"
#include "xmach_def.h"
#include "xmsdos_def.h"
#include "xne_def.h"
#include "xpe_def.h"

#ifdef USE_XSIMD
extern "C" {
#include "xsimd/src/xsimd.h"
}
#endif

// TODO mb Functions
#define S_ALIGN_DOWN32(value, align) (((quint32)value) & ~((quint32)align - 1))
#define S_ALIGN_UP32(value, align) ((((quint32)value) & ((quint32)align - 1)) ? (S_ALIGN_DOWN32((quint32)value, (quint32)align) + (quint32)align) : ((quint32)value))

#define S_ALIGN_DOWN64(value, align) (((quint64)value) & ~((quint64)align - 1))
#define S_ALIGN_UP64(value, align) ((((quint64)value) & ((quint64)align - 1)) ? (S_ALIGN_DOWN64((quint64)value, (quint64)align) + (quint64)align) : ((quint64)value))

#define S_ALIGN_DOWN(value, align) ((value) & ~(align - 1))
#define S_ALIGN_UP(value, align) (((value) & (align - 1)) ? S_ALIGN_DOWN(value, align) + align : value)

#define S_LOWORD(value) ((quint16)((quint32)(value) & 0xFFFF))
#define S_HIWORD(value) ((quint16)((quint32)(value) >> 16))
#define S_FULL_VERSION(value1, value2, value3) ((quint32)((((quint16)value1) << 16) | (((quint8)value2) << 8) | ((quint8)value3)))

typedef quint64 XADDR;
#define XADDR_MAX ((XADDR)0xFFFFFFFFFFFFFFFF)
// #define XADDR_ERROR (XADDR)-1

#ifdef Q_OS_MAC
#include <CoreFoundation/CoreFoundation.h>  // Check
#endif

class XBinary : public QObject {
    Q_OBJECT

    static const double D_ENTROPY_THRESHOLD;  // 6.5 TODO set get

public:
    struct XCONVERT {
        quint32 nID;
        QString sSetString;
        QString sTransString;
    };

    static QString XCONVERT_idToTransString(quint32 nID, XBinary::XCONVERT *pRecords, qint32 nRecordsSize);
    static QString XCONVERT_idToSetString(quint32 nID, XBinary::XCONVERT *pRecords, qint32 nRecordsSize);
    static QString XCONVERT_idToFtString(quint32 nID, XBinary::XCONVERT *pRecords, qint32 nRecordsSize);
    static quint32 XCONVERT_ftStringToId(const QString &sString, XBinary::XCONVERT *pRecords, qint32 nRecordsSize);
    static QString XCONVERT_translate(const QString &sString, XBinary::XCONVERT *pRecords, qint32 nRecordsSize);

    struct XIDSTRING {
        quint64 nID;
        QString sString;
    };

    static QString XIDSTRING_idToString(quint64 nID, XBinary::XIDSTRING *pRecords, qint32 nRecordsSize);
    static quint64 XIDSTRING_ftStringToId(const QString &sString, XBinary::XIDSTRING *pRecords, qint32 nRecordsSize);

    static QMap<quint64, QString> XIDSTRING_createMap(XBinary::XIDSTRING *pRecords, qint32 nRecordsSize);
    static QMap<quint64, QString> XIDSTRING_createMapPrefix(XBinary::XIDSTRING *pRecords, qint32 nRecordsSize, const QString &sPrefix);

    enum LT {
        LT_UNKNOWN = 0,
        LT_OFFSET,
        LT_ADDRESS,
        LT_RELADDRESS,
    };

    struct XLOC {
        LT locType;
        XADDR nLocation;
    };

    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_NFDSCAN,
        STRUCTID_DIESCAN,
        STRUCTID_YARASCAN,
        STRUCTID_VIRUSTOTALSCAN,
        STRUCTID_VISUALIZATION,
        STRUCTID_HEX,
        STRUCTID_DISASM,
        STRUCTID_ARCHIVE,
        STRUCTID_HASH,
        STRUCTID_STRINGS,
        STRUCTID_SIGNATURES,
        STRUCTID_REGIONS,
        STRUCTID_MEMORYMAP,
        STRUCTID_SYMBOLS,
        STRUCTID_ENTROPY,
        STRUCTID_EXTRACTOR,
        STRUCTID_SEARCH,
        STRUCTID_OVERLAY
    };

    struct DATASET {
        qint64 nOffset;
        XADDR nAddress;
        qint64 nSize;
        QString sName;
        quint32 nType;
        // Optional
        qint64 nStringTableOffset;
        qint64 nStringTableSize;
    };

    enum HANDLE_METHOD {
        HANDLE_METHOD_UNKNOWN = 0,
        HANDLE_METHOD_STORE,
        HANDLE_METHOD_FILE,  // TODO Check
        HANDLE_METHOD_DEFLATE,
        HANDLE_METHOD_DEFLATE64,
        HANDLE_METHOD_BZIP2,
        HANDLE_METHOD_LZMA,
        HANDLE_METHOD_LZMA2,
        HANDLE_METHOD_XZ,
        HANDLE_METHOD_LZW_PDF,
        HANDLE_METHOD_ASCII85,
        HANDLE_METHOD_PPMD7,
        HANDLE_METHOD_PPMD8,
        HANDLE_METHOD_LZH5,
        HANDLE_METHOD_LZH6,
        HANDLE_METHOD_LZH7,
        HANDLE_METHOD_RAR_15,
        HANDLE_METHOD_RAR_20,
        HANDLE_METHOD_RAR_29,
        HANDLE_METHOD_RAR_50,
        HANDLE_METHOD_RAR_70,
        HANDLE_METHOD_LZSS_SZDD,
        HANDLE_METHOD_IT214_8,
        HANDLE_METHOD_IT214_16,
        HANDLE_METHOD_IT215_8,
        HANDLE_METHOD_IT215_16,
        HANDLE_METHOD_IMPLODED_4KDICT_2TREES,
        HANDLE_METHOD_IMPLODED_4KDICT_3TREES,
        HANDLE_METHOD_IMPLODED_8KDICT_2TREES,
        HANDLE_METHOD_IMPLODED_8KDICT_3TREES,
        HANDLE_METHOD_SHRINK,
        HANDLE_METHOD_REDUCE_1,
        HANDLE_METHOD_REDUCE_2,
        HANDLE_METHOD_REDUCE_3,
        HANDLE_METHOD_REDUCE_4,
        HANDLE_METHOD_ZLIB,
        HANDLE_METHOD_ZIP_AES,
        HANDLE_METHOD_ZIP_AES128,
        HANDLE_METHOD_ZIP_AES192,
        HANDLE_METHOD_ZIP_AES256,
        HANDLE_METHOD_7Z_AES,
        HANDLE_METHOD_RAR5_AES,
        HANDLE_METHOD_ZIPCRYPTO,
        HANDLE_METHOD_STORE_CAB,
        HANDLE_METHOD_MSZIP_CAB,
        HANDLE_METHOD_LZX_CAB,
        HANDLE_METHOD_BCJ,
        HANDLE_METHOD_BCJ2,
        HANDLE_METHOD_ARM64_BCJ,  // ARM64 branch/call/jump filter
        HANDLE_METHOD_PDF_IMAGEDATA,
        HANDLE_METHOD_PDF_CCITTIMAGE,
        HANDLE_METHOD_PDF_PALETTE,
        HANDLE_METHOD_ANDROID_XML,
        HANDLE_METHOD_ZSTD,         // Zstandard compression (tar.zst, .zst)
        HANDLE_METHOD_LZIP,         // LZIP compression - LZMA variant (tar.lz, .lz)
        HANDLE_METHOD_LZOP,         // LZO compression (tar.lzo, .lzo)
        HANDLE_METHOD_COMPRESS,     // Compress (.Z) - LZW algorithm (tar.Z, .Z)
        HANDLE_METHOD_ARJ,          // ARJ compression methods 1-3 (Huffman + LZSS)
        HANDLE_METHOD_ARJ_FASTEST,  // ARJ compression method 4 (simple LZSS)
        HANDLE_METHOD_BROTLI,       // Brotli compression (.br)
        HANDLE_METHOD_ACE,          // ACE method 1: LZ77 + Huffman (32KB window)
        HANDLE_METHOD_ACE_DELTA     // ACE method 2: LZ77 + Huffman + inverse byte-delta filter
        // TODO check more methods
    };

    enum CRYPTO_METHOD {  // Obsolete
        CRYPTO_METHOD_UNKNOWN = 0,
        CRYPTO_METHOD_NONE,
        CRYPTO_METHOD_ZIPCRYPTO,
        CRYPTO_METHOD_AES128,
        CRYPTO_METHOD_AES192,
        CRYPTO_METHOD_AES256
    };

    enum FPART_PROP {
        FPART_PROP_UNKNOWN = 0,
        FPART_PROP_HANDLEMETHOD,
        FPART_PROP_HANDLEMETHOD2,
        FPART_PROP_HANDLEMETHOD3,
        FPART_PROP_COMPRESSEDSIZE,
        FPART_PROP_COMPRESSEDSIZE2,
        FPART_PROP_COMPRESSEDSIZE3,
        FPART_PROP_UNCOMPRESSEDSIZE,
        FPART_PROP_UNCOMPRESSEDSIZE2,
        FPART_PROP_UNCOMPRESSEDSIZE3,
        FPART_PROP_STREAMOFFSET,
        FPART_PROP_STREAMSIZE,
        FPART_PROP_STREAMUNPACKEDSIZE,
        FPART_PROP_STREAMOFFSET2,      // BCJ2: call LZMA archive offset
        FPART_PROP_STREAMSIZE2,        // BCJ2: call LZMA compressed size
        FPART_PROP_STREAMOFFSET3,      // BCJ2: jmp LZMA archive offset
        FPART_PROP_STREAMSIZE3,        // BCJ2: jmp LZMA compressed size
        FPART_PROP_STREAMOFFSET4,      // BCJ2: range coder archive offset
        FPART_PROP_STREAMSIZE4,        // BCJ2: range coder size
        FPART_PROP_HANDLEMETHOD4,      // BCJ2: main LZMA compress method
        FPART_PROP_UNCOMPRESSEDSIZE4,  // BCJ2: main LZMA uncompressed size
        FPART_PROP_SUBSTREAMOFFSET,
        FPART_PROP_CRC_TYPE,
        FPART_PROP_RESULTCRC,
        FPART_PROP_UNCOMPRESSEDCRC,
        FPART_PROP_UNCOMPRESSEDCRC2,
        FPART_PROP_UNCOMPRESSEDCRC3,
        FPART_PROP_DATETIME,
        FPART_PROP_ENCRYPTED,
        FPART_PROP_FILETYPE,
        FPART_PROP_TYPE,
        FPART_PROP_WIDTH,
        FPART_PROP_HEIGHT,
        FPART_PROP_BITSPERCOMPONENT,
        FPART_PROP_INFO,
        FPART_PROP_EXT,
        FPART_PROP_ORIGINALNAME,
        FPART_PROP_WINDOWSIZE,
        FPART_PROP_FILEMODE,
        FPART_PROP_ISFOLDER,
        FPART_PROP_ISSOLID,
        FPART_PROP_UID,
        FPART_PROP_GID,
        FPART_PROP_LINKNAME,
        FPART_PROP_COMPRESSPROPERTIES,  // Compression algorithm properties (e.g., LZMA dictionary size)
        FPART_PROP_COMPRESSPROPERTIES2,
        FPART_PROP_COMPRESSPROPERTIES3,
        FPART_PROP_AESKEY,  // AES encryption key derivation parameters (salt, IV, etc.)
        FPART_PROP_USERNAME,
        FPART_PROP_GROUPNAME,
        FPART_PROP_DEVVERSION,
        FPART_PROP_PREFIX,
        FPART_PROP_HEADER_OFFSET,
        FPART_PROP_HEADER_SIZE,
        FPART_PROP_OPTHEADER_OFFSET,
        FPART_PROP_OPTHEADER_SIZE,
        FPART_PROP_UUID,
        FPART_PROP_EXTRAFIELDOFFSET,
        FPART_PROP_EXTRAFIELDLENGTH,
        FPART_PROP_FILECOMMENTOFFSET,
        FPART_PROP_FILECOMMENTLENGTH,
        FPART_PROP_FLAGS,
        FPART_PROP_RESOURCEID,
        FPART_PROP_SOLIDFOLDERINDEX,  // Solid folder/block index within an archive (e.g., 7z folder index)
        FPART_PROP_FILEMD5,           // MD5 hash of the entire archive file (hex string, set during initUnpack)
        FPART_PROP_MTIME,             // Modification time (QDateTime)
        FPART_PROP_CTIME,             // Creation time / birth time (QDateTime; on Unix: inode change time)
        FPART_PROP_ATIME,             // Last access time (QDateTime)
        FPART_PROP_ISREADONLY,        // Windows FILE_ATTRIBUTE_READONLY / Unix write-permission absent (bool)
        FPART_PROP_ISHIDDEN,          // Windows FILE_ATTRIBUTE_HIDDEN (bool)
        FPART_PROP_ISSYSTEM,          // Windows FILE_ATTRIBUTE_SYSTEM (bool)
        FPART_PROP_ISARCHIVE,         // Windows FILE_ATTRIBUTE_ARCHIVE (bool)
        FPART_PROP_ISCOMMENTPRESENT,  // Archive record has a comment (bool)
        FPART_PROP_COLORSPACE,        // PDF image color space (e.g., "/DeviceRGB", "/DeviceGray", "/Indexed")
        FPART_PROP_BASECOLORSPACE,    // PDF indexed image base color space (e.g., "/DeviceRGB")
        FPART_PROP_PALETTE,           // PDF indexed image palette data (QByteArray)
        FPART_PROP_SUBTYPE,           // PDF stream subtype (e.g., "/Image", "/Form")
        FPART_PROP_FILTERNAME,        // PDF filter name (e.g., "/FlateDecode", "/DCTDecode")
        FPART_PROP_CCITTK,            // CCITT /K parameter (-1=Group4, 0=Group3 1D, >0=Group3 2D)
        // BCJ2 per-stream AES decryption properties (for AES-encrypted 7z BCJ2 archives)
        FPART_PROP_BCJ2_AES_PROPS_0,   // AES properties for BCJ2 main stream
        FPART_PROP_BCJ2_AES_UNPACK_0,  // AES output size for BCJ2 main stream (= LZMA2 compressed size)
        FPART_PROP_BCJ2_AES_PROPS_1,   // AES properties for BCJ2 call stream
        FPART_PROP_BCJ2_AES_UNPACK_1,  // AES output size for BCJ2 call stream
        FPART_PROP_BCJ2_AES_PROPS_2,   // AES properties for BCJ2 jmp stream
        FPART_PROP_BCJ2_AES_UNPACK_2,  // AES output size for BCJ2 jmp stream
        FPART_PROP_BCJ2_AES_PROPS_3,   // AES properties for BCJ2 range stream
        FPART_PROP_BCJ2_AES_UNPACK_3,  // AES output size for BCJ2 range stream
        FPART_PROP_PASSWORD_MODIFIER,  // ARJ garble password modifier byte
        // FPART_PROP_NEEDCONVERT
        // FPART_PROP_COMPRESSION_OPTION_0,
        // FPART_PROP_COMPRESSION_OPTION_1,
    };

    enum UNPACK_PROP {
        UNPACK_PROP_UNKNOWN = 0,
        UNPACK_PROP_PASSWORD,
        UNPACK_PROP_FIXFILENAMES,  // Sanitize filenames for current OS; handle duplicates with _2, _3; treat symlinks as folders
        UNPACK_PROP_NOCRC,         // Do not check CRC after unpacking
    };

    struct DATAPROCESS_STATE {
        QIODevice *pDeviceInput;
        QIODevice *pDeviceOutput;
        qint64 nInputOffset;
        qint64 nInputLimit;
        qint64 nProcessedOffset;
        qint64 nProcessedLimit;
        QMap<FPART_PROP, QVariant> mapProperties;
        QMap<UNPACK_PROP, QVariant> mapUnpackProperties;
        bool bReadError;
        bool bWriteError;
        qint64 nCountInput;
        qint64 nCountOutput;
        char *pInputBuffer;        // Opt
        qint32 nInputBufferSize;   // Opt
        char *pOutputBuffer;       // Opt
        qint32 nOutputBufferSize;  // Opt
    };

    struct ARCHIVERECORD {
        qint64 nStreamOffset;
        qint64 nStreamSize;
        QMap<FPART_PROP, QVariant> mapProperties;
    };

    struct UNPACK_STATE {
        qint64 nCurrentOffset;
        qint64 nTotalSize;
        qint32 nCurrentIndex;
        qint32 nNumberOfRecords;
        QMap<UNPACK_PROP, QVariant> mapUnpackProperties;
        QMap<FPART_PROP, QVariant> mapArchiveProperties;  // Archive-level properties (e.g., FPART_PROP_FILEMD5)
        void *pContext;                                   // Format-specific context
    };

    enum PATH_MODE {
        PATH_MODE_DEFAULT = 0,
        PATH_MODE_RELATIVE = 1,  // Store relative paths from base path
        PATH_MODE_BASENAME = 2,  // Store only filename (no directories)
        PATH_MODE_ABSOLUTE = 3   // Store absolute paths
    };

    enum PACK_PROP {
        PACK_PROP_UNKNOWN = 0,
        PACK_PROP_COMPRESSMETHOD,    // Compression method
        PACK_PROP_COMPRESSIONLEVEL,  // Compression level (0-9, -1 for default)
        PACK_PROP_ENCRYPTIONMETHOD,  // Encryption method
        PACK_PROP_PASSWORD,          // Password for encryption
        PACK_PROP_PATHMODE,          // Path storage mode
        PACK_PROP_BASEPATH,          // Base path for relative path calculation
        PACK_PROP_OUTPUT_FORMAT,     // Output format (for multi-format packers)
    };

    struct PACK_STATE {
        QIODevice *pDevice;  // Output device for the archive
        QMap<PACK_PROP, QVariant> mapProperties;
        qint64 nCurrentOffset;    // Current write position in archive
        qint32 nNumberOfRecords;  // Number of records added so far
        void *pContext;           // Format-specific context (cast to format's context struct)
    };

    static qint32 _readDevice(char *pBuffer, qint32 nBufferSize, DATAPROCESS_STATE *pState);
    static qint32 _readDevice(DATAPROCESS_STATE *pState);
    static qint32 _writeDevice(char *pBuffer, qint32 nBufferSize, DATAPROCESS_STATE *pState);

    struct BYTE_COUNTS {
        qint64 nSize;
        qint64 nCount[256];  // TODO const
    };

    struct OS_STRING {
        qint64 nOffset;
        qint64 nSize;
        QString sString;
    };

    struct OFFSETSIZE {
        qint64 nOffset;
        qint64 nSize;
    };

    struct ADDRESSSIZE {
        XADDR nAddress;
        qint64 nSize;
    };

    struct RELADDRESSSIZE {
        qint64 nAddress;
        qint64 nSize;
    };

    enum FILEPART {
        FILEPART_UNKNOWN = 0,
        FILEPART_REGION = 1 << 0,
        FILEPART_SECTION = 1 << 1,
        FILEPART_SEGMENT = 1 << 2,
        FILEPART_HEADER = 1 << 4,
        FILEPART_OVERLAY = 1 << 5,
        FILEPART_RESOURCE = 1 << 6,
        FILEPART_DEBUGDATA = 1 << 7,
        FILEPART_STREAM = 1 << 8,
        FILEPART_SIGNATURE = 1 << 9,
        FILEPART_FOOTER = 1 << 10,
        FILEPART_DATA = 1 << 12,
        FILEPART_OBJECT = 1 << 13,
        FILEPART_TABLE = 1 << 14,
        FILEPART_VALUE = 1 << 15,
        FILEPART_ALL = 0xFFFFFFFF,
    };

    struct FPART {
        qint64 nFileOffset;
        qint64 nFileSize;
        XADDR nVirtualAddress;
        qint64 nVirtualSize;
        QString sName;
        FILEPART filePart;
        QMap<FPART_PROP, QVariant> mapProperties;
    };

    static quint32 getFPART_crc32(const FPART &fpart);

    struct _MEMORY_RECORD {
        qint64 nOffset;
        XADDR nAddress;
        // ADDRESS_SEGMENT segment;
        qint64 nSize;
        // MMT type; // TODO Use File_Part
        FILEPART filePart;  // File_Part
        qint32 nFilePartNumber;
        QString sName;
        qint32 nIndex;
        bool bIsVirtual;
        bool bIsInvisible;  // TODO
        // quint64 nID;
    };

    enum FORMATTYPE {
        FORMATTYPE_UNKNOWN = 0,
        FORMATTYPE_TEXT,
        FORMATTYPE_PLAINTEXT,
        FORMATTYPE_XML,
        FORMATTYPE_JSON,
        FORMATTYPE_CSV,
        FORMATTYPE_TSV
    };

    enum FT {
        FT_UNKNOWN = 0,
        FT_DATA,
        FT_REGION,  // For Memory regions
        FT_PROCESS,
        FT_BINARY,
        FT_BINARY16,
        FT_BINARY32,
        FT_BINARY64,
        FT_COM,
        FT_MSDOS,
        FT_DOS16M,
        FT_DOS4G,
        FT_NE,
        FT_LE,
        FT_LX,
        FT_PE,
        FT_PE32,
        FT_PE64,
        FT_ELF,
        FT_ELF32,
        FT_ELF64,
        FT_MACHO,
        FT_MACHO32,
        FT_MACHO64,
        FT_AMIGAHUNK,
        FT_ATARIST,
        // Extra
        FT_7Z,
        FT_ANDROIDASRC,
        FT_ANDROIDXML,
        FT_APK,
        FT_APKS,
        FT_AR,
        FT_TAR,
        FT_TAR_GZ,
        FT_TAR_BZIP2,
        FT_TAR_LZIP,
        FT_TAR_LZMA,
        FT_TAR_LZOP,
        FT_TAR_XZ,
        FT_TAR_Z,
        FT_TAR_ZSTD,
        FT_ARCHIVE,
        FT_CAB,
        FT_DEX,
        FT_DOCUMENT,
        FT_GIF,
        FT_BMP,
        FT_IMAGE,
        FT_VIDEO,
        FT_AUDIO,
        FT_IPA,
        FT_JAR,
        FT_JPEG,
        FT_MACHOFAT,
        FT_PDF,
        FT_DER,
        FT_PLAINTEXT,
        FT_PNG,
        FT_RAR,
        FT_TEXT,
        FT_TIFF,
        FT_UNICODE,
        FT_UNICODE_BE,
        FT_UNICODE_LE,
        FT_UPX,
        FT_XML,
        FT_UTF8,
        FT_ZIP,
        FT_GZIP,
        FT_ZLIB,
        FT_LHA,
        FT_ICO,
        FT_ICC,
        FT_INNOSETUP,
        FT_CUR,
        FT_MP3,
        FT_MP4,
        FT_XM,
        FT_RIFF,
        FT_AVI,
        FT_WEBP,
        FT_WAV,
        FT_SIGNATURE,
        FT_NPM,
        FT_NSIS,
        FT_DEB,
        FT_BWDOS16M,
        FT_JAVACLASS,
        FT_PYC,
        FT_CFBF,
        FT_TTF,
        FT_DJVU,
        FT_SZDD,
        FT_BZIP2,
        FT_ZSTD,
        FT_LZIP,
        FT_LZO,
        FT_COMPRESS,
        FT_XZ,
        FT_CPIO,
        FT_SQUASHFS,
        FT_ISO9660,
        FT_UDF,
        FT_MINIDUMP,
        FT_DMG,
        FT_STK,
        FT_PAL,
        FT_ARC,
        FT_FREEARC,
        FT_ARJ,
        FT_ACE,
        FT_BROTLI,

        // TODO more
    };

    enum MODE {
        MODE_UNKNOWN = 0,
        MODE_DATA,  // Raw data
        MODE_BIT,   // 1/0
        MODE_8,
        MODE_16,
        MODE_16SEG,
        MODE_32,
        MODE_64,
        MODE_128,
        MODE_256,
        MODE_FREG
        // TODO more
    };

    enum DMFAMILY {
        DMFAMILY_UNKNOWN,
        DMFAMILY_X86,
        DMFAMILY_ARM,
        DMFAMILY_ARM64,
        DMFAMILY_MIPS,
        DMFAMILY_PPC,
        DMFAMILY_SPARC,
        DMFAMILY_SYSZ,
        DMFAMILY_XCORE,
        DMFAMILY_M68K,
        DMFAMILY_M68OK,
        DMFAMILY_RISCV,
        DMFAMILY_EVM,
        DMFAMILY_MOS65XX,
        DMFAMILY_WASM,
        DMFAMILY_BPF,
        DMFAMILY_CUSTOM,
        DMFAMILY_CUSTOM_MACH_REBASE,
        DMFAMILY_CUSTOM_MACH_BIND,
        DMFAMILY_CUSTOM_MACH_EXPORT,
        DMFAMILY_CUSTOM_7ZIP,
    };

    enum DM {
        DM_UNKNOWN = 0,
        DM_DATA,
        DM_X86_16,
        DM_X86_32,
        DM_X86_64,
        DM_ARM_LE,
        DM_ARM_BE,
        DM_AARCH64_LE,
        DM_AARCH64_BE,
        DM_CORTEXM,
        DM_THUMB_LE,
        DM_THUMB_BE,
        DM_MIPS_LE,
        DM_MIPS_BE,
        DM_MIPS64_LE,
        DM_MIPS64_BE,
        DM_PPC_LE,
        DM_PPC_BE,
        DM_PPC64_LE,
        DM_PPC64_BE,
        DM_SPARC,
        DM_SPARCV9,
        DM_S390X,
        DM_XCORE,
        DM_M68K,
        DM_M68K00,
        DM_M68K10,
        DM_M68K20,
        DM_M68K30,
        DM_M68K40,
        DM_M68K60,
        DM_TMS320C64X,
        DM_M6800,
        DM_M6801,
        DM_M6805,
        DM_M6808,
        DM_M6809,
        DM_M6811,
        DM_CPU12,
        DM_HD6301,
        DM_HD6309,
        DM_HCS08,
        DM_EVM,
        DM_RISKV32,
        DM_RISKV64,
        DM_RISKVC,
        DM_MOS65XX,
        DM_WASM,
        DM_BPF_LE,
        DM_BPF_BE,
        DM_CUSTOM,
        DM_CUSTOM_MACH_REBASE,
        DM_CUSTOM_MACH_BIND,
        DM_CUSTOM_MACH_WEAK,
        DM_CUSTOM_MACH_EXPORT,
        DM_CUSTOM_7ZIP_PROPERTIES,
        DM_ALL
    };

    enum SYNTAX {
        SYNTAX_DEFAULT = 0,
        SYNTAX_INTEL,
        SYNTAX_AMD,
        SYNTAX_ARM,
        SYNTAX_ATT,
        SYNTAX_MASM,
        SYNTAX_MOTOROLA,
        SYNTAX_MIPS
    };

    enum TYPE {
        TYPE_UNKNOWN = 0,
        // TODO more
    };

    enum OSNAME {
        OSNAME_UNKNOWN = 0,
        OSNAME_MULTIPLATFORM,
        OSNAME_AIX,
        OSNAME_ALPINELINUX,
        OSNAME_AMIGA,
        OSNAME_ANDROID,
        OSNAME_AROS,
        OSNAME_ASPLINUX,
        OSNAME_ATARIST,
        OSNAME_BORLANDOSSERVICES,
        OSNAME_BRIDGEOS,
        OSNAME_DEBIANLINUX,
        OSNAME_FENIXOS,
        OSNAME_FREEBSD,
        OSNAME_GENTOOLINUX,
        OSNAME_HANCOMLINUX,
        OSNAME_HPUX,
        OSNAME_IOS,
        OSNAME_IPADOS,
        OSNAME_IPHONEOS,
        OSNAME_IRIX,
        OSNAME_LINUX,
        OSNAME_MACOS,
        OSNAME_MAC_OS,
        OSNAME_MAC_OS_X,
        OSNAME_MANDRAKELINUX,
        OSNAME_MCLINUX,
        OSNAME_MINIX,
        OSNAME_MODESTO,
        OSNAME_MSDOS,
        OSNAME_FREEDOS,
        OSNAME_NETBSD,
        OSNAME_NSK,
        OSNAME_OPENBSD,
        OSNAME_OPENVMS,
        OSNAME_OPENVOS,
        OSNAME_OS2,
        OSNAME_OS_X,
        OSNAME_POSIX,
        OSNAME_QNX,
        OSNAME_REDHATLINUX,
        OSNAME_SOLARIS,
        OSNAME_STARTOSLINUX,
        OSNAME_SUNOS,
        OSNAME_SUSELINUX,
        OSNAME_SYLLABLE,
        OSNAME_TRU64,
        OSNAME_TURBOLINUX,
        OSNAME_TVOS,
        OSNAME_UBUNTULINUX,
        OSNAME_UEFI,
        OSNAME_UNIX,
        OSNAME_VINELINUX,
        OSNAME_WATCHOS,
        OSNAME_REACTOS,
        OSNAME_WINDOWS,
        OSNAME_WINDOWSCE,
        OSNAME_WINDOWSPHONE,
        OSNAME_WINDRIVERLINUX,
        OSNAME_XBOX,
        OSNAME_PLAYSTATION,
        OSNAME_JVM,
        OSNAME_MACCATALYST,
        OSNAME_MACDRIVERKIT,
        OSNAME_MACFIRMWARE,
        OSNAME_SEPOS,
        OSNAME_CHROMEOS,
        OSNAME_NINTENDOSWITCH,
        OSNAME_WEBOS,
        OSNAME_HARMONYOS,
        OSNAME_ARCHLINUX
        // TODO more
    };

    enum ENDIAN {
        ENDIAN_UNKNOWN = 0,
        ENDIAN_LITTLE,
        ENDIAN_BIG
    };

    struct FILEFORMATINFO {
        bool bIsValid;
        qint64 nSize;
        FT fileType;
        QString sExt;
        QString sVersion;
        QString sInfo;
        QString sType;
        ENDIAN endian;
        QString sMIME;
        MODE mode;
        QString sArch;
        OSNAME osName;
        QString sOsVersion;
        QString sOsBuild;
        bool bIsVM;
        bool bIsEncrypted;
        QString sCompresionMethod;
    };

    struct _MEMORY_MAP {
        XADDR nModuleAddress;
        bool bIsImage;  // TODO fill
        qint64 nImageSize;
        qint64 nBinarySize;
        XADDR nEntryPointAddress;
        qint64 nCodeBase;         // For MSDOS
        qint64 nStartLoadOffset;  // For MSDOS
        FT fileType;
        MODE mode;
        ENDIAN endian;
        QString sArch;
        QString sType;
        QList<_MEMORY_RECORD> listRecords;
    };

    enum SYMBOL_TYPE {
        SYMBOL_TYPE_UNKNOWN,
        SYMBOL_TYPE_EXPORT = 0x00000001,
        SYMBOL_TYPE_IMPORT = 0x00000002,
        SYMBOL_TYPE_LABEL = 0x00000004,  // DATA
        SYMBOL_TYPE_ANSISTRING = 0x00000008,
        SYMBOL_TYPE_UNICODESTRING = 0x00000010,
        SYMBOL_TYPE_ALL = 0xFFFFFFFF
    };

    struct SYMBOL_RECORD {
        XADDR nAddress;
        qint64 nSize;
        XADDR nModuleAddress;
        SYMBOL_TYPE symbolType;
        qint32 nOrdinal;  // For Windows OS;
        QString sName;
        QString sFunction;
    };

    enum HASH {
        HASH_MD4 = 0,
        HASH_MD5,
        HASH_SHA1,
#ifndef QT_CRYPTOGRAPHICHASH_ONLY_SHA1
#if (QT_VERSION_MAJOR > 4)
        HASH_SHA224,
        HASH_SHA256,
        HASH_SHA384,
        HASH_SHA512,
#endif
        // TODO Check more
        // TODO Check Qt versions!
//        HASH_KECCAK_224,
//        HASH_KECCAK_256,
//        HASH_KECCAK_384,
//        HASH_KECCAK_512
#endif
    };

    enum VT {
        VT_UNKNOWN = 0,
        VT_STRING,
        VT_HEX,
        VT_DATETIME,
        VT_A,
        VT_A_I,
        VT_U,
        VT_U_I,
        VT_UTF8,
        VT_UTF8_I,
        VT_SIGNATURE,
        VT_VALUE,
        VT_BIT,
        VT_BYTE,
        VT_WORD,
        VT_DWORD,
        VT_QWORD,
        VT_128,
        VT_256,
        VT_FPEG,
        VT_CHAR,
        VT_UCHAR,
        VT_SHORT,
        VT_USHORT,
        VT_INT,
        VT_UINT,
        VT_INT8,
        VT_INT16,
        VT_INT32,
        VT_INT64,
        VT_UINT8,
        VT_UINT16,
        VT_UINT32,
        VT_UINT64,
        VT_DOUBLE,
        VT_FLOAT,
        VT_ANSI,
        VT_CHAR_ARRAY,
        VT_BYTE_ARRAY,
        VT_WORD_ARRAY,
        VT_DWORD_ARRAY,
        VT_PACKEDNUMBER,
        VT_ULEB128,
        // TODO pascal strings(A/U)
    };

    enum XFRECORD_FLAG {
        XFRECORD_FLAG_NONE = 0,
        XFRECORD_FLAG_SIZE = 0x00000001,
        XFRECORD_FLAG_OFFSET = 0x00000002,
        XFRECORD_FLAG_RELATIVE_OFFSET = 0x00000004,
        XFRECORD_FLAG_ADDRESS = 0x00000008,
        XFRECORD_FLAG_RELATIVE_ADDRESS = 0x00000010,
        XFRECORD_FLAG_VERSION_MAJOR = 0x00000020,
        XFRECORD_FLAG_VERSION_MINOR = 0x00000040,
        XFRECORD_FLAG_VERSION_PATCH = 0x00000080,
        XFRECORD_FLAG_VERSION_BUILD = 0x00000100,
        XFRECORD_FLAG_VERSION = 0x00000200,
        XFRECORD_FLAG_VERSION_DIVMOD = 0x00000400,
        XFRECORD_FLAG_DATETIME = 0x00001000,
        XFRECORD_FLAG_DATE = 0x00002000,
        XFRECORD_FLAG_TIME = 0x00004000,
        XFRECORD_FLAG_DOSDATE = 0x00008000,
        XFRECORD_FLAG_DOSTIME = 0x00010000,
        XFRECORD_FLAG_UNIXTIME = 0x00020000,
        XFRECORD_FLAG_FILETIME = 0x00040000,
        XFRECORD_FLAG_COUNT = 0x00080000,
    };

    struct XFRECORD {
        QString sName;
        qint32 nOffset;  // From start
        qint32 nSize;
        quint64 nFlags;
        VT valueType;
    };

    enum XFTYPE {
        XFTYPE_UNKNOWN = 0,
        XFTYPE_HEADER,
        XFTYPE_TABLE
    };

    enum XFDATASTTYPE {
        XFDATASTYPE_UNKNOWN = 0,
        XFDATASTYPE_LIST,
        XFDATASTYPE_FLAGS
    };

    struct XFDATAST {
        qint32 nFieldIndex;  // Index in XFRECORD
        quint64 nMask;
        XFDATASTTYPE xfDataStType;
        XBinary::XIDSTRING *pRecords;
        qint32 nRecordsSize;
    };

    struct XFHEADER {
        QString sTag;
        QString sParentTag;  // empty if no parent
        FT fileType;
        STRUCTID structID;
        XLOC xLoc;
        qint64 nSize; // Optional
        XFTYPE xfType;
        bool bIsParentNeeded;           // if we need parent struct to calculate the header right way
        QList<XFRECORD> listFields;     // For XFTYPE_HEADER, for fixed XFTYPE_TABLE
        QList<XFDATAST> listDataSt;     // For XFTYPE_HEADER, for fixed XFTYPE_TABLE
        QList<XADDR> listRowLocations;  // For XFTYPE_TABLE
        QList<QString> listRowNames;    // For XFTYPE_TABLE, optional names for rows
    };

    struct XFSTRUCT {
        QString sParent;
        _MEMORY_MAP *pMemoryMap;
        FT fileType;
        quint32 nStructID;
        XLOC xLoc;
        qint64 nSize; // Optional, if 0 => calculate by parent struct
        XFTYPE xfType;
        bool bIsParent;
        qint32 nCount;
    };

    struct MS_RECORD {
        XADDR nRelOffset;
        qint16 nRegionIndex;
        quint16 nValueType;
        quint16 nSize;
        quint16 nInfo;
    };

    struct OPCODE {
        XADDR nAddress;
        qint64 nSize;
        QString sName;
    };

    struct MEMORY_REPLACE  // For debuggers&breakpoints
    {
        XADDR nAddress;
        qint64 nOffset;
        qint64 nSize;
        QByteArray baOriginal;
    };

    //    struct XUINT128 {
    //        quint64 low;
    //        quint64 high;
    //    };

    struct XVARIANT {
        VT varType;
        bool bIsBigEndian;
        QVariant var;
    };

    enum HLTYPE {
        HLTYPE_UNKNOWN = 0,
        HLTYPE_TOTAL,
        HLTYPE_NATIVEREGIONS,
        HLTYPE_NATIVESUBREGIONS,
        HLTYPE_DATA
    };

    static FPART findParentFPart(const QList<FPART> &listFParts, const FPART &fPart);

    struct PDRECORD {
        qint64 nCurrent;
        qint64 nTotal;
        QString sStatus;
        //        bool bSuccess;
        //        bool bFinished;
        bool bIsValid;
    };

    const static qint32 N_NUMBER_PDRECORDS = 5;

    struct PDSTRUCT;
    typedef void (*PDSTRUCT_CALLBACK)(void *pUserData, PDSTRUCT *pPdStruct);

    struct PDSTRUCT {
        PDRECORD _pdRecord[N_NUMBER_PDRECORDS];
        bool bIsStop;
        quint64 nFinished;
        //        bool bIsDisable;
        //        QString sStatus;
        //        bool bErrors;
        //        bool bSuccess; // TODO important
        QString sInfoString;
        QString sErrorString;
        bool bForceStop;         // TODO !!!
        qint32 nBufferSize;      // 0 =
        qint32 nFileBufferSize;  // 0 => 0x10000
        PDSTRUCT_CALLBACK pCallback;
        void *pCallbackUserData;
        qint64 nLastCallbackTime;
    };

    enum DHT {
        DHT_UNKNOWN = 0,
        DHT_HEADER,
        DHT_TABLE,
        DHT_DATA
    };

    struct DSID {
        FT fileType;
        quint32 nID;
        QString sGUID;
    };

    enum DRF {
        DRF_UNKNOWN = 0,
        DRF_SIZE = 0x00000001,
        DRF_COUNT = 0x00000002,
        DRF_ADDRESS = 0x00000004,
        DRF_OFFSET = 0x00000008,
        DRF_VERSION = 0x00000010,
        DRF_VOLATILE = 0x00000020,
    };

    enum VL_TYPE {
        VL_TYPE_UNKNOWN = 0,
        VL_TYPE_LIST,
        VL_TYPE_FLAGS
    };

    struct DATAVALUESET {
        quint64 nMask;
        VL_TYPE vlType;
        QMap<quint64, QString> mapValues;
    };

    struct DATA_RECORD {
        qint32 nRelOffset;
        qint32 nSize;
        QString sName;
        VT valType;
        ENDIAN endian;
        quint32 nFlags;
        QList<DATAVALUESET> listDataValueSets;
    };

    enum DHMODE {
        DHMODE_UNKNOWN = 0,
        DHMODE_HEADER,
        DHMODE_TABLE,
        DHMODE_HEX
    };

    struct DATA_HEADER {
        DSID dsID;
        DSID dsID_parent;
        QString sName;
        LT locType;
        XADDR nLocation;
        qint64 nSize;
        qint32 nCount;
        DHMODE dhMode;
        QList<DATA_RECORD> listRecords;
    };

    static DATA_HEADER _searchDataHeaderByGuid(const QString &sGUID, const QList<DATA_HEADER> &listDataHeaders);
    static DATA_HEADER _searchDataHeaderById(FT fileType, quint32 nID, const QList<DATA_HEADER> &listDataHeaders);

    DATA_RECORD getDataRecord(qint64 nRelOffset, qint64 nSize, const QString &sName, VT valType, quint32 nFlags, ENDIAN endian);
    DATA_RECORD getDataRecordDV(qint64 nRelOffset, qint64 nSize, const QString &sName, VT valType, quint32 nFlags, ENDIAN endian, QMap<quint64, QString> mapValues,
                                VL_TYPE vlType);

    virtual QString structIDToString(quint32 nID);
    virtual quint32 ftStringToStructID(const QString &sFtString);

    struct DATA_HEADERS_OPTIONS {
        _MEMORY_MAP *pMemoryMap;
        DSID dsID_parent;
        FT fileType;
        quint32 nID;
        LT locType;
        XADDR nLocation;
        bool bChildren;
        DHMODE dhMode;
        qint64 nSize;
        qint32 nCount;
    };

    DSID _addDefaultHeaders(QList<DATA_HEADER> *pListHeaders, PDSTRUCT *pPdStruct);

    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct);

    QList<XFHEADER> _getXFHeaders(PDSTRUCT *pPdStruct = nullptr);
    virtual QList<XFHEADER> getXFHeaders(const XFSTRUCT &xfStruct, PDSTRUCT *pPdStruct);
    virtual QList<XFRECORD> getXFRecords(FT fileType, quint32 nStructID, const XLOC &xLoc);
    QList<QVariant> getXFRecordValues(const QList<XFRECORD> &listXFRecords, const XLOC &xLoc);

    static QString xfHeaderToTag(const XFHEADER &xfHeader, const QString &sStructName, const QString &sParentTag);
    static QString xfHeaderToString(const XFHEADER &xfHeader, const QString &sStructName, const QString &sParentString);

    DATA_HEADER _initDataHeader(const DATA_HEADERS_OPTIONS &dataHeadersOptions, const QString &sName);
    DATA_HEADER _dataHeaderHex(const DATA_HEADERS_OPTIONS &dataHeadersOptions, const QString &sName, const DSID &dsID_parent, quint32 nID, qint64 nOffset, qint64 nSize);

    struct DATA_RECORDS_OPTIONS {
        _MEMORY_MAP *pMemoryMap;
        DATA_HEADER dataHeaderFirst;
    };

    struct DATA_RECORD_ROW {
        LT locType;
        XADDR nLocation;
        qint64 nSize;
        QList<QVariant> listValues;
    };

    qint32 getDataRecordValues(const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords, QList<QString> *pListTitles,
                               PDSTRUCT *pPdStruct);

    static QList<QString> getDataRecordComments(const DATA_RECORDS_OPTIONS &dataRecordsOptions, const DATA_RECORD_ROW &dataRecordRow, PDSTRUCT *pPdStruct);

    virtual QList<QString> getTableTitles(const DATA_RECORDS_OPTIONS &dataRecordsOptions);

    struct TABLE_LIST {
        QList<OFFSETSIZE> listOffsetsSizes;
    };

    virtual bool readTableInit(const DATA_RECORDS_OPTIONS &dataRecordsOptions, void **ppUserData, PDSTRUCT *pPdStruct);
    virtual qint32 readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<DATA_RECORD_ROW> *pListDataRecords,
                                void *pUserData, PDSTRUCT *pPdStruct);
    virtual void readTableFinalize(const DATA_RECORDS_OPTIONS &dataRecordsOptions, void *pUserData, PDSTRUCT *pPdStruct);

    bool _isFlagPresentInRecords(const QList<DATA_RECORD> *pListRecords, quint32 nFlag);

    virtual QString getCompressMethodString();

    static QString handleMethodToString(HANDLE_METHOD handleMethod);
    static QString handleMethodToFtString(HANDLE_METHOD handleMethod);
    static HANDLE_METHOD ftStringToHandleMethod(const QString &sString);

    enum ST {
        ST_COMPAREBYTES = 0,
        ST_NOTNULL,
        ST_ANSI,
        ST_NOTANSI,
        ST_NOTANSIANDNULL,
        ST_ANSINUMBER,
        ST_FINDBYTES,
        ST_SKIP,
        ST_RELOFFSET,
        ST_ADDRESS
    };

    struct SIGNATURE_RECORD {
        XADDR nBaseAddress;
        ST st;
        QByteArray baData;
        quint32 nSizeOfAddr;
        qint64 nFindDelta;
        qint32 nWindowSize;
    };

    explicit XBinary(QIODevice *pDevice = nullptr, bool bIsImage = false,
                     XADDR nModuleAddress = -1);  // mb TODO parent for signals/slot
    XBinary(const QString &sFileName);
    ~XBinary();

    void setData(QIODevice *pDevice = nullptr, bool bIsImage = false, XADDR nModuleAddress = -1);
    void setDevice(QIODevice *pDevice);
    void setReadWriteMutex(QMutex *pReadWriteMutex);

    void setFileName(const QString &sFileName);

    qint64 safeReadData(QIODevice *pDevice, qint64 nPos, char *pData, qint64 nMaxLen, PDSTRUCT *pPdStruct);
    qint64 safeWriteData(QIODevice *pDevice, qint64 nPos, const char *pData, qint64 nLen, PDSTRUCT *pPdStruct);

    qint64 _readDataSimple(QIODevice *pDevice, qint64 nPos, char *pData, qint64 nMaxLen);
    QByteArray _readDataSimple(QIODevice *pDevice, qint64 nPos, qint64 nSize);
    qint64 _writeDataSimple(QIODevice *pDevice, qint64 nPos, const char *pData, qint64 nLen);

    qint64 getSize();
    static qint64 getSize(QIODevice *pDevice);
    static qint64 getSize(const QString &sFileName);

    void setMode(MODE mode);

    void setType(qint32 nType);

    QString getTypeAsString();

    void setFileType(FT fileType);

    static QString modeIdToString(MODE mode);
    static QString endianToString(ENDIAN endian);

    void setArch(const QString &sArch);

    static QString getFileFormatString(const FILEFORMATINFO *pFileFormatInfo);
    static QString getFileFormatInfoString(const FILEFORMATINFO *pFileFormatInfo);
    static OSNAME getOsName(const FILEFORMATINFO *pFileFormatInfo);

    void setFileFormatExt(const QString &sFileFormatExt);

    void setFileFormatSize(qint64 nFileFormatSize);

    // Filesystem helpers
    static bool setFileDateTime(const QString &sFileName, const QDateTime &dateTime);
    static QDateTime winFileTimeToQDateTime(quint64 nWinFileTime);
    static bool setFileProperties(const QMap<FPART_PROP, QVariant> &mapProperties, const QString &sFileName);

    virtual OFFSETSIZE getSignOffsetSize();  // TODO rename

    void setOsType(OSNAME osName);

    void setOsVersion(const QString &sOsVersion);
    virtual QString getOsVersion();
    virtual FILEFORMATINFO getFileFormatInfo(PDSTRUCT *pPdStruct);

    void setEndian(ENDIAN endian);
    void setIsExecutable(bool bIsExecutable);
    void setIsArchive(bool bIsArchive);

    virtual FT getFileType();
    virtual MODE getMode();
    virtual QString getMIMEString();
    virtual qint32 getType();
    virtual QString typeIdToString(qint32 nType);
    virtual ENDIAN getEndian();
    virtual QString getArch();
    virtual QString getFileFormatExt();
    void setFileFormatExtsString(const QString &sFileFormatExts);
    virtual QString getFileFormatExtsString();
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct);
    virtual bool isSigned();
    virtual OSNAME getOsName();

    bool isPacked(double dEntropy);
    virtual bool isExecutable();
    virtual bool isArchive();

    static quint8 random8();
    static quint16 random16();
    static quint32 random32();
    static quint64 random64();
    static quint64 random(quint64 nLimit);
    static QString randomString(qint32 nSize);

    static QString fileTypeIdToString(FT fileType);
    static QString xfTypeIdToString(XFTYPE xfType);
    static QString xfTypeIdToFtString(XFTYPE xfType);
    // static QString fileTypeIdToExts(FT fileType);  // TODO move to classes
    static FT ftStringToFileTypeId(QString sFileType);
    static QString fileTypeIdToFtString(FT fileType);
    static QString fileTypesToString(const QSet<FT> &stResult);
    static QSet<FT> stringToFileTypes(const QString &sString);

    static QString convertFileName(const QString &sFileName);
    // static QString convertPathName(const QString &sPathName);

    OS_STRING getOsAnsiString(qint64 nOffset, qint64 nSize);

    struct FFOPTIONS {
        QList<QString> *pListFileNames;
        bool bSubdirectories;
        bool *pbIsStop;
        qint32 *pnNumberOfFiles;
        // TODO filter
    };

    static void findFiles(const QString &sDirectoryName, FFOPTIONS *pFFOption, qint32 nLevel = 0);
    static void findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames);
    static void findFiles(const QString &sDirectoryName, QList<QString> *pListFileNames, bool bSubDirectories, qint32 nLevel, PDSTRUCT *pPdStruct = nullptr);

    static qint32 getNumberOfFiles(const QString &sDirectoryName, bool bSubDirectories, qint32 nLevel, PDSTRUCT *pPdStruct = nullptr);

    static QString regExp(const QString &sRegExp, const QString &sString, qint32 nIndex);
    static bool isRegExpPresent(const QString &sRegExp, const QString &sString);
    static qint32 getRegExpCount(const QString &sRegExp, const QString &sString);  // TODO Check!
    static QString getRegExpSection(const QString &sRegExp, const QString &sString, qint32 nStart, qint32 nEnd);
    static bool isRegExpValid(const QString &sRegExp);
    qint64 read_array_process(qint64 nOffset, char *pBuffer, qint64 nMaxSize, PDSTRUCT *pPdStruct = nullptr);
    qint64 read_array(qint64 nOffset, char *pBuffer, qint64 nMaxSize);
    QByteArray read_array(qint64 nOffset, qint64 nSize);
    QByteArray read_array_process(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    qint64 write_array_process(qint64 nOffset, const char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    qint64 write_array_process(qint64 nOffset, const QByteArray &baData, PDSTRUCT *pPdStruct = nullptr);
    qint64 write_array(qint64 nOffset, const char *pBuffer, qint64 nSize);
    qint64 write_array(qint64 nOffset, const QByteArray &baData);

    static QByteArray read_array_process(QIODevice *pDevice, qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    static qint64 read_array_process(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    static qint64 write_array_process(QIODevice *pDevice, qint64 nOffset, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    static qint64 write_array_process(QIODevice *pDevice, qint64 nOffset, const QByteArray &baData, PDSTRUCT *pPdStruct = nullptr);
    static QByteArray read_array(QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    static qint64 write_array(QIODevice *pDevice, qint64 nOffset, const char *pBuffer, qint64 nSize);
    static qint64 write_array(QIODevice *pDevice, qint64 nOffset, const QByteArray &baData);

    quint8 read_uint8(qint64 nOffset);
    qint8 read_int8(qint64 nOffset);
    quint16 read_uint16(qint64 nOffset, bool bIsBigEndian = false);
    qint16 read_int16(qint64 nOffset, bool bIsBigEndian = false);
    quint32 read_uint32(qint64 nOffset, bool bIsBigEndian = false);
    qint32 read_int32(qint64 nOffset, bool bIsBigEndian = false);
    quint64 read_uint64(qint64 nOffset, bool bIsBigEndian = false);
    qint64 read_int64(qint64 nOffset, bool bIsBigEndian = false);
    float read_float16(qint64 nOffset, bool bIsBigEndian = false);  // TODO Check
    float read_float(qint64 nOffset, bool bIsBigEndian = false);    // TODO Check
    double read_double(qint64 nOffset, bool bIsBigEndian = false);  // TODO Check

    quint32 read_uint24(qint64 nOffset,
                        bool bIsBigEndian = false);  // Uses UPX in header
    qint32 read_int24(qint64 nOffset, bool bIsBigEndian = false);

    qint64 write_ansiString(qint64 nOffset, const QString &sString, qint64 nMaxSize = -1);
    void write_ansiStringFix(qint64 nOffset, qint64 nSize, const QString &sString);

    qint64 write_unicodeString(qint64 nOffset, const QString &sString, qint64 nMaxSize = -1, bool bIsBigEndian = false);

    QString read_ansiString(qint64 nOffset, qint64 nMaxSize = 256);
    QString read_unicodeString(qint64 nOffset, qint64 nMaxSize = 256, bool bIsBigEndian = false);
    QString read_ucsdString(qint64 nOffset);
    QString read_utf8String(qint64 nOffset, qint64 nMaxSize = 256);
    QString _read_utf8String(qint64 nOffset, qint64 nMaxSize = 256);
    static QString _read_utf8String(char *pData, qint64 nMaxSize);
    QString _read_utf8String(qint64 nOffset, char *pData, qint32 nDataSize, qint32 nDataOffset);

    QString read_codePageString(qint64 nOffset, qint64 nMaxByteSize = 256, const QString &sCodePage = "System");

    bool isUnicodeStringLatin(qint64 nOffset, qint64 nMaxSize = 256, bool bIsBigEndian = false);

    void write_uint8(qint64 nOffset, quint8 nValue);
    void write_int8(qint64 nOffset, qint8 nValue);
    void write_uint16(qint64 nOffset, quint16 nValue, bool bIsBigEndian = false);
    void write_int16(qint64 nOffset, qint16 nValue, bool bIsBigEndian = false);
    void write_uint32(qint64 nOffset, quint32 nValue, bool bIsBigEndian = false);
    void write_int32(qint64 nOffset, qint32 nValue, bool bIsBigEndian = false);
    void write_uint64(qint64 nOffset, quint64 nValue, bool bIsBigEndian = false);
    void write_int64(qint64 nOffset, qint64 nValue, bool bIsBigEndian = false);
    void write_float16(qint64 nOffset, float fValue,
                       bool bIsBigEndian = false);  // TODO Check
    void write_float(qint64 nOffset, float fValue,
                     bool bIsBigEndian = false);  // TODO Check
    void write_double(qint64 nOffset, double dValue,
                      bool bIsBigEndian = false);  // TODO Check

    QString read_UUID_bytes(qint64 nOffset);                       // uuid [16]
    void write_UUID_bytes(qint64 nOffset, const QString &sValue);  // uuid [16]

    QString read_UUID(qint64 nOffset, bool bIsBigEndian = false);

    static quint8 _read_uint8(char *pData);
    static qint8 _read_int8(char *pData);
    static quint16 _read_uint16(char *pData, bool bIsBigEndian = false);
    static qint16 _read_int16(char *pData, bool bIsBigEndian = false);
    static quint32 _read_uint24(char *pData, bool bIsBigEndian = false);
    static quint32 _read_uint32(char *pData, bool bIsBigEndian = false);
    static qint32 _read_int32(char *pData, bool bIsBigEndian = false);
    static quint64 _read_uint64(char *pData, bool bIsBigEndian = false);
    static qint64 _read_int64(char *pData, bool bIsBigEndian = false);
    static QString _read_ansiString(char *pData, qint32 nMaxSize = 50);
    static QByteArray _read_byteArray(char *pData, qint32 nSize);
    static float _read_float(char *pData,
                             bool bIsBigEndian = false);  // TODO Check
    static double _read_double(char *pData,
                               bool bIsBigEndian = false);  // TODO Check

    static quint64 _read_value(MODE mode, char *pData, bool bIsBigEndian = false);
    // TODO read uin64, freg

    static quint8 _read_uint8_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset);
    static quint16 _read_uint16_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian = false);
    static quint32 _read_uint32_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian = false);
    static quint64 _read_uint64_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, bool bIsBigEndian = false);
    static QString _read_ansiString_safe(char *pBuffer, qint32 nBufferSize, qint32 nOffset, qint32 nMaxSize = 512);

    static bool _read_bool_safe(char *pBuffer, qint32 nBufferSize, qint32 nBitPosition);
    static bool _read_bool_safe_rev(char *pBuffer, qint32 nBufferSize, qint32 nBitPosition);
    static quint32 _getBitCount_safe(char *pBuffer, qint32 nBufferSize);

    static void _write_uint8(char *pData, quint8 nValue);
    static void _write_int8(char *pData, qint8 nValue);
    static void _write_uint16(char *pData, quint16 nValue, bool bIsBigEndian = false);
    static void _write_int16(char *pData, qint16 nValue, bool bIsBigEndian = false);
    static void _write_uint32(char *pData, quint32 nValue, bool bIsBigEndian = false);
    static void _write_int32(char *pData, qint32 nValue, bool bIsBigEndian = false);
    static void _write_uint64(char *pData, quint64 nValue, bool bIsBigEndian = false);
    static void _write_int64(char *pData, qint64 nValue, bool bIsBigEndian = false);
    static void _write_float(char *pData, float fValue,
                             bool bIsBigEndian = false);  // TODO Check
    static void _write_double(char *pData, double dValue,
                              bool bIsBigEndian = false);  // TODO Check

    static void _write_value(MODE mode, char *pData, quint64 nValue, bool bIsBigEndian = false);
    // TODO write uin64, freg

    quint8 read_bcd_uint8(qint64 nOffset);
    quint16 read_bcd_uint16(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint32(qint64 nOffset, bool bIsBigEndian = false);
    quint16 read_bcd_uint64(qint64 nOffset, bool bIsBigEndian = false);

    quint8 _bcd_decimal(quint8 nValue);

    qint64 _find_array(ST st, qint64 nOffset, qint64 nSize, const char *pArray, qint64 nArraySize, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_array(qint64 nOffset, qint64 nSize, const char *pArray, qint64 nArraySize, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_byteArray(qint64 nOffset, qint64 nSize, const QByteArray &baData, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint8(qint64 nOffset, qint64 nSize, quint8 nValue, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int8(qint64 nOffset, qint64 nSize, qint8 nValue, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint16(qint64 nOffset, qint64 nSize, quint16 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int16(qint64 nOffset, qint64 nSize, qint16 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint32(qint64 nOffset, qint64 nSize, quint32 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int32(qint64 nOffset, qint64 nSize, qint32 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_uint64(qint64 nOffset, qint64 nSize, quint64 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_int64(qint64 nOffset, qint64 nSize, qint64 nValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_float(qint64 nOffset, qint64 nSize, float fValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_double(qint64 nOffset, qint64 nSize, double dValue, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);

    static void endian_float(float *pValue, bool bIsBigEndian);
    static void endian_double(double *pValue, bool bIsBigEndian);

    qint64 find_ansiString(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_unicodeString(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_utf8String(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);

    // Signature Format:
    // - Hex bytes: AA-FF or aa-ff (case insensitive, spaces ignored)
    // - Wildcards: .. or ?? (matches any byte, converted to .. internally)
    // - ASCII strings: 'string' (converted to hex automatically)
    // - Relative offset: $ ($$=1 byte, $$$$=2 bytes, $$$$$$$$=4 bytes, $$$$$$$$$$$$$$$$=8 bytes)
    // - Absolute address: # (####=4 bytes, ########=8 bytes, ##=2 bytes, ##=1 byte)
    // - Find pattern: + (e.g., ++4D5A searches for 4D5A within 64-byte window, each + adds 32 bytes)
    // - ANSI character: %% (matches printable ASCII 0x20-0x7E, each pair=1 byte)
    // - Not ANSI: !% (matches non-printable ASCII, each pair=1 byte)
    // - Not ANSI and not null: _% (matches non-ASCII and non-zero, each pair=1 byte)
    // - ANSI alphanumeric: %& (matches 0-9, A-Z, a-z, each pair=1 byte)
    // - Not null: ** (matches any non-zero byte, each pair=1 byte)
    // Examples: "4D5A..0003", "'PE'0000", "E9$$$$$$$$", "68########", "23%%%%%%%%%%2F"

    enum SIGBYTETYPE {
        SIGBYTETYPE_HEX,
        SIGBYTETYPE_WILDCARD,
        SIGBYTETYPE_ANSI,
        SIGBYTETYPE_NOT_ANSI,
        SIGBYTETYPE_NOT_ANSI_AND_NOT_NULL,
        SIGBYTETYPE_ANSI_ALPHANUMERIC,
        SIGBYTE_NOT_NULL
    };

    // SigByte format: {type:SIGBYTETYPE quint8, value:QByteArray (for hex) quint8}

    QByteArray _signatureToSigBytes(const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);
    bool _compareSigBytes(const QByteArray &baSigBytes, const QByteArray &baData, PDSTRUCT *pPdStruct = nullptr);
    bool _compareSigBytes(const char *pSigBytes, qint64 nSigBytesSize, const char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);
    qint64 _findSigBytes(qint64 nOffset, qint64 nSize, const char *pSigBytes, qint64 nSigBytesSize, PDSTRUCT *pPdStruct = nullptr);

    qint64 find_signature(qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize = 0, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, qint64 *pnResultSize = nullptr,
                          PDSTRUCT *pPdStruct = nullptr);
    qint64 find_ansiStringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_unicodeStringI(qint64 nOffset, qint64 nSize, const QString &sString, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    qint64 find_utf8StringI(qint64 nOffset, qint64 nSize, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    // TODO find_codePageString
    // TODO find_codePageStringI

    static quint8 getBits_uint8(quint8 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint16 getBits_uint16(quint16 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint32 getBits_uint32(quint32 nValue, qint32 nBitOffset, qint32 nBitSize);
    static quint64 getBits_uint64(quint64 nValue, qint32 nBitOffset, qint32 nBitSize);

    struct STRINGSEARCH_OPTIONS {
        // TODO more
        qint32 nLimit;
        qint64 nMinLenght;
        qint64 nMaxLenght;
        bool bAnsi;
        // bool bUTF8;
        bool bUnicode;
        bool bNullTerminated;
        QString sMask;
        // QString sANSICodec;
        bool bLinks;
    };

    struct SIGNATUREDB_RECORD {
        qint32 nNumber;
        QString sName;
        ENDIAN endian;
        QString sSignature;
        QString sPatch;
    };

    enum SF {
        SF_BEGIN = 0,
        SF_CURRENTOFFSET
    };

    enum SEARCHMODE {
        SEARCHMODE_STRING = 0,
        SEARCHMODE_SIGNATURE,
        SEARCHMODE_VALUE
    };

    struct SEARCHDATA {
        qint64 nResultOffset;
        qint64 nResultSize;
        qint64 nCurrentOffset;
        SF startFrom;
        QVariant varValue;
        VT valueType;
        ENDIAN endian;
        bool bIsInit;
    };

    bool _addMultiSearchStringRecord(QVector<MS_RECORD> *pList, MS_RECORD *pRecord, QString sString, STRINGSEARCH_OPTIONS *pSsOptions);
    bool _addMultiSearchStringRecordOptimized(QVector<MS_RECORD> *pList, MS_RECORD *pRecord, const QString &sString, STRINGSEARCH_OPTIONS *pSsOptions,
                                              QRegularExpression *pRegex);

    QVector<MS_RECORD> multiSearch_allStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_allStrings2(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_ansiStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_unicodeStrings(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, STRINGSEARCH_OPTIONS ssOptions, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_signature(qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, quint32 nInfo, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_signature(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, const QString &sSignature, quint32 nInfo,
                                             PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_value(qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType, bool bIsBigEndian, PDSTRUCT *pPdStruct = nullptr);
    QVector<MS_RECORD> multiSearch_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, qint32 nLimit, QVariant varValue, VT valueType, bool bIsBigEndian,
                                         PDSTRUCT *pPdStruct = nullptr);

    qint64 find_value(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, QVariant varValue, VT valueType, bool bIsBigEndian, qint64 *pnResultSize,
                      PDSTRUCT *pPdStruct = nullptr);

    QVariant read_value(VT valueType, qint64 nOffset, qint64 nSize, bool bIsBigEndian = false, PDSTRUCT *pPdStruct = nullptr);

    static QString valueTypeToString(VT valueType, qint32 nSize);
    static QString getValueString(QVariant varValue, VT valueType, bool bTypesAsHex = false);
    static qint32 getValueSize(QVariant varValue, VT valueType);
    static VT getValueType(quint64 nValue);
    static qint32 getBaseValueSize(VT valueType);

    static qint32 getValueSymbolSize(VT valueType);
    static bool isIntegerType(VT valueType);

    static QByteArray getUnicodeString(const QString &sString, bool bIsBigEndian);
    static QByteArray getStringData(VT valueType, const QString &sString, bool bAddNull);

    bool isSignaturePresent(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize, const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);
    static bool isSignatureValid(const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);

    static bool createFile(const QString &sFileName, qint64 nFileSize = 0);
    static bool isFileExists(const QString &sFileName, bool bTryToOpen = false);
    static bool removeFile(const QString &sFileName);
    static bool copyFile(const QString &sSrcFileName, const QString &sDestFileName);
    static bool moveFile(const QString &sSrcFileName, const QString &sDestFileName);
    static bool moveFileToDirectory(const QString &sSrcFileName, const QString &sDestDirectory);
    static QString convertFileNameSymbols(const QString &sFileName);
    static QString fixFileName(const QString &sFileName);
    static QString getBaseFileName(const QString &sFileName);
    static bool createDirectory(const QString &sDirectoryName);
    static bool isDirectoryExists(const QString &sDirectoryName);
    static bool removeDirectory(const QString &sDirectoryName);
    static bool isDirectoryEmpty(const QString &sDirectoryName);
    static QDateTime getDirectoryLatestModificationDate(const QString &sDirectoryName);

    static bool createEmptyFile(const QString &sFileName);

    static QByteArray readFile(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static bool readFile(const QString &sFileName, char *pBuffer, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);

    static void _copyMemory(char *pDest, const char *pSource, qint64 nSize);
    static void _zeroMemory(char *pDest, qint64 nSize);
    static bool _isMemoryZeroFilled(char *pSource, qint64 nSize);
    static bool _isMemoryNotNull(char *pSource, qint64 nSize);
    static bool _isMemoryAnsi(char *pSource, qint64 nSize);
    static bool _isMemoryNotAnsi(char *pSource, qint64 nSize);
    static bool _isMemoryNotAnsiAndNull(char *pSource, qint64 nSize);
    static bool _isMemoryAnsiNumber(char *pSource, qint64 nSize);
    static bool copyDeviceMemory(QIODevice *pSourceDevice, qint64 nSourceOffset, QIODevice *pDestDevice, qint64 nDestOffset, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    bool copyMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize, quint32 nBufferSize = 1, bool bReverse = false);
    bool zeroFill(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);
    static bool compareMemory(char *pMemory1, const char *pMemory2, qint64 nSize);
    // For strings compare
    static bool compareMemoryByteI(quint8 *pMemory, const quint8 *pMemoryU, const quint8 *pMemoryL,
                                   qint64 nSize);  // Ansi
    static bool compareMemoryWordI(quint16 *pMemory, const quint16 *pMemoryU, const quint16 *pMemoryL,
                                   qint64 nSize);  // Unicode

    bool isOffsetValid(qint64 nOffset, qint64 nTotalSize);
    bool isOffsetValid(qint64 nOffset);
    bool isAddressValid(XADDR nAddress);
    bool isRelAddressValid(qint64 nRelAddress);
    bool isAddressPhysical(XADDR nAddress);

    XADDR offsetToAddress(qint64 nOffset);
    qint64 addressToOffset(XADDR nAddress);
    XADDR offsetToRelAddress(qint64 nOffset);
    qint64 relAddressToOffset(qint64 nRelAddress);

    static bool isEOD(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static bool isOffsetValid(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static bool isOffsetAndSizeValid(_MEMORY_MAP *pMemoryMap, OFFSETSIZE *pOsRegion);
    bool isOffsetAndSizeValid(qint64 nOffset, qint64 nSize);
    static bool isOffsetAndSizeValid(_MEMORY_MAP *pMemoryMap, qint64 nOffset, qint64 nSize);
    static bool isOffsetAndSizeValid(QIODevice *pDevice, qint64 nOffset, qint64 nSize);

    static bool isAddressValid(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressValid(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);

    static bool isAddressPhysical(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressPhysical(_MEMORY_MAP *pMemoryMap, XADDR nAddress);

    static XADDR offsetToAddress(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static qint64 addressToOffset(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static XADDR offsetToRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static qint64 relAddressToOffset(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static XADDR relAddressToAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static qint64 addressToRelAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static XADDR segmentRelOffsetToAddress(_MEMORY_MAP *pMemoryMap, quint16 nSegment, XADDR nRelOffset);
    static qint64 locationToOffset(_MEMORY_MAP *pMemoryMap, LT locType, XADDR nLocation);
    static qint64 locToOffset(_MEMORY_MAP *pMemoryMap, const XLOC &xLoc);
    static XLOC offsetToLoc(qint64 nOffset);

    static XADDR getSegmentAddress(quint16 nSegment, quint16 nAddress);

    static _MEMORY_RECORD getMemoryRecordByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static _MEMORY_RECORD getMemoryRecordByAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static _MEMORY_RECORD getMemoryRecordByRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static _MEMORY_RECORD getMemoryRecordByIndex(_MEMORY_MAP *pMemoryMap, qint32 nIndex);

    static qint32 getMemoryIndexByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);

    static qint32 addressToFileTypeNumber(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static qint32 relAddressToFileTypeNumber(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static bool isAddressInHeader(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static bool isRelAddressInHeader(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);

    static QString getLoadSectionNameByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);

    static bool isSolidAddressRange(_MEMORY_MAP *pMemoryMap, XADDR nAddress, qint64 nSize);

    QString getMemoryRecordInfoByOffset(qint64 nOffset);
    QString getMemoryRecordInfoByAddress(XADDR nAddress);
    QString getMemoryRecordInfoByRelAddress(qint64 nRelAddress);
    static QString getMemoryRecordInfoByOffset(_MEMORY_MAP *pMemoryMap, qint64 nOffset);
    static QString getMemoryRecordInfoByAddress(_MEMORY_MAP *pMemoryMap, XADDR nAddress);
    static QString getMemoryRecordInfoByRelAddress(_MEMORY_MAP *pMemoryMap, qint64 nRelAddress);
    static QString getMemoryRecordName(_MEMORY_RECORD *pMemoryRecord);

    enum MAPMODE {
        MAPMODE_UNKNOWN = 0,
        MAPMODE_REGIONS,
        MAPMODE_SEGMENTS,
        MAPMODE_SECTIONS,
        MAPMODE_OBJECTS,
        MAPMODE_STREAMS,
        MAPMODE_MAPS,
        MAPMODE_DATA
    };

    static QString mapModeToString(MAPMODE mapMode);

    virtual QList<MAPMODE> getMapModesList();

    virtual bool _initMemoryMap(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct);
    virtual void _processMemoryMap(_MEMORY_MAP *pMemoryMap, QList<FPART> *pListFParts, PDSTRUCT *pPdStruct);

    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr);
    _MEMORY_MAP _getMemoryMap(quint32 nFileParts, PDSTRUCT *pPdStruct = nullptr);
    _MEMORY_MAP _getMemoryMap(QList<FPART> *pListFParts, PDSTRUCT *pPdStruct = nullptr);

    _MEMORY_MAP _getSimpleMemoryMap(quint32 nFileParts, PDSTRUCT *pPdStruct = nullptr);
    _MEMORY_MAP getSimpleMemoryMap();

    virtual QList<FPART> getNativeRegions(PDSTRUCT *pPdStruct = nullptr);
    virtual QList<FPART> getNativeSubRegions(PDSTRUCT *pPdStruct = nullptr);
    virtual QList<FPART> getHData(PDSTRUCT *pPdStruct = nullptr);

    static qint32 getNumberOfPhysicalRecords(_MEMORY_MAP *pMemoryMap);
    static qint32 getNumberOfVirtualRecords(_MEMORY_MAP *pMemoryMap);
    static qint32 getNumberOfMemoryMapFileParts(_MEMORY_MAP *pMemoryMap, FILEPART filePart);
    static qint64 getRecordsTotalRowSize(_MEMORY_MAP *pMemoryMap);

    virtual XADDR getBaseAddress();
    virtual void setBaseAddress(XADDR nBaseAddress);
    virtual qint64 getImageSize();

    qint64 _getEntryPointOffset();
    virtual qint64 getEntryPointOffset(_MEMORY_MAP *pMemoryMap);

    virtual void setEntryPointOffset(qint64 nEntryPointOffset);
    XADDR getEntryPointAddress();
    XADDR getEntryPointAddress(_MEMORY_MAP *pMemoryMap);
    virtual XADDR _getEntryPointAddress();

    qint64 getEntryPointRVA();
    qint64 getEntryPointRVA(_MEMORY_MAP *pMemoryMap);

    static XADDR getLowestAddress(_MEMORY_MAP *pMemoryMap);
    static qint64 getTotalVirtualSize(_MEMORY_MAP *pMemoryMap);
    static XADDR positionToVirtualAddress(_MEMORY_MAP *pMemoryMap, qint64 nPosition);

    void setModuleAddress(XADDR nValue);
    XADDR getModuleAddress();

    bool isImage();
    void setIsImage(bool bValue);

    void setMultiSearchCallbackState(bool bState);

    static qint64 getPhysSize(char *pBuffer, qint64 nSize);  // TODO Check!
    static bool isEmptyData(char *pBuffer, qint64 nSize);
    bool compareSignature(const QString &sSignature, qint64 nOffset = 0);
    bool compareSignature(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset = 0, PDSTRUCT *pPdStruct = nullptr);
    static bool _compareByteArrayWithSignature(const QByteArray &baData, const QString &sSignature);
    static QString _createSignature(const QString &sSignature1, const QString &sSignature2);

    bool compareSignatureOnAddress(const QString &sSignature, XADDR nAddress);
    bool compareSignatureOnAddress(_MEMORY_MAP *pMemoryMap, const QString &sSignature, XADDR nAddress);

    bool compareEntryPoint(const QString &sSignature, qint64 nOffset = 0);
    bool compareEntryPoint(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset = 0);

    bool moveMemory(qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize);
    static bool moveMemory(QIODevice *pDevice, qint64 nSourceOffset, qint64 nDestOffset, qint64 nSize);

    static bool dumpToFile(const QString &sFileName, const char *pData, qint64 nDataSize);
    bool dumpToFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);
    static bool dumpToFile(const QString &sFileName, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);

    bool patchFromFile(const QString &sFileName, qint64 nDataOffset, qint64 nDataSize, PDSTRUCT *pPdStruct = nullptr);

    QSet<FT> getFileTypes(bool bExtra = false);
    static QSet<FT> getFileTypes(QIODevice *pDevice,
                                 bool bExtra = false);  // mb TODO isImage
    static QSet<FT> getFileTypes(QIODevice *pDevice, qint64 nOffset, qint64 nSize, bool bExtra = false);
    static QSet<FT> getFileTypes(const QString &sFileName, bool bExtra = false);
    static QSet<FT> getFileTypes(QByteArray *pbaData, bool bExtra = false);

    static FT _getPrefFileType(const QSet<XBinary::FT> *pStFileTypes);

    enum TL_OPTION {
        TL_OPTION_DEFAULT = 0,
        TL_OPTION_ALL,
        TL_OPTION_EXECUTABLE,
        TL_OPTION_SYMBOLS
    };

    static QList<FT> _getFileTypeListFromSet(const QSet<FT> &stFileTypes, TL_OPTION tlOption);

    static QString valueToHex(quint8 nValue);
    static QString valueToHex(qint8 nValue);
    static QString valueToHex(quint16 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint16 nValue, bool bIsBigEndian = false);
    static QString valueToHex(quint32 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint32 nValue, bool bIsBigEndian = false);
    static QString valueToHex(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHex(qint64 nValue, bool bIsBigEndian = false);
    static QString valueToHex(float fValue, bool bIsBigEndian = false);
    static QString valueToHex(double dValue, bool bIsBigEndian = false);
    static QString valueToHex(MODE mode, quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexEx(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexOS(quint64 nValue, bool bIsBigEndian = false);
    static QString valueToHexColon(MODE mode, quint64 nValue, bool bIsBigEndian = false);
    static QString xVariantToHex(XVARIANT value);

    static QString thisToString(qint64 nDelta, qint32 nBase = 16);

    static bool checkString_byte(const QString &sValue);
    static bool checkString_word(const QString &sValue);
    static bool checkString_dword(const QString &sValue);
    static bool checkString_qword(const QString &sValue);
    static bool checkString_uint8(const QString &sValue);
    static bool checkString_int8(const QString &sValue);
    static bool checkString_uint16(const QString &sValue);
    static bool checkString_int16(const QString &sValue);
    static bool checkString_uint32(const QString &sValue);
    static bool checkString_int32(const QString &sValue);
    static bool checkString_uint64(const QString &sValue);
    static bool checkString_int64(const QString &sValue);
    static bool checkString_float(const QString &sValue);
    static bool checkString_double(const QString &sValue);

    static QString boolToString(bool bValue);
    static QString getSpaces(qint32 nNumberOfSpaces);

    static QString getUnpackedFileName(QIODevice *pDevice, bool bShort = false);
    static QString getUnpackedFileName(const QString &sFileName);
    static QString getDumpFileName(QIODevice *pDevice);
    static QString getDumpFileName(const QString &sFileName);
    static QString getBackupFileName(QIODevice *pDevice);
    static QString getBackupFileName(const QString &sFileName);
    static QString getResultFileName(QIODevice *pDevice, const QString &sAppendix);
    static QString getResultFileName(const QString &sFileName, const QString &sAppendix);
    static QString getDeviceFileName(QIODevice *pDevice);
    static QString getDeviceFilePath(QIODevice *pDevice);
    static QString getDeviceDirectory(QIODevice *pDevice);
    static QString getDeviceFileBaseName(QIODevice *pDevice);
    static QString getDeviceFileCompleteSuffix(QIODevice *pDevice);
    static QString getDeviceFileSuffix(QIODevice *pDevice);
    static QString getFileDirectory(const QString &sFileName);

    static QIODevice *getBackupDevice(QIODevice *pDevice);
    static bool isBackupPresent(QIODevice *pDevice);
    static bool saveBackup(QIODevice *pDevice);

    static QString getCurrentBackupDate();

    static QList<qint64> getFixupList(QIODevice *pDevice1, QIODevice *pDevice2, qint64 nDelta, PDSTRUCT *pPdStruct = nullptr);

    static QString getHash(HASH hash, const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static QString getHash(HASH hash, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    QString getHash(HASH hash, qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);
    QString getHash(HASH hash, QList<OFFSETSIZE> *pListOS, PDSTRUCT *pPdStruct = nullptr);

    static QSet<HASH> getHashMethods();
    static QList<HASH> getHashMethodsAsList();
    static QString hashIdToString(HASH hash);

    static bool isFileHashValid(HASH hash, const QString &sFileName, const QString &sHash);

    static quint32 getAdler32(const QString &sFileName);  // TODO ProcessData
    static quint32 getAdler32(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    quint32 getAdler32(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    // https://reveng.sourceforge.io/crc-catalogue/ TODO
    static void _createCRC32Table(quint32 *pCRCTable, quint32 nPoly = 0xEDB88320);
    static quint32 *_getCRC32Table_EDB88320();
    static quint16 *_getCRC16Table();

    static quint32 _getCRC32(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static quint32 _getCRC32(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    static quint32 _getCRC32(QIODevice *pDevice, quint32 nInit, quint32 *pCRCTable, PDSTRUCT *pPdStruct = nullptr);
    static quint32 _getCRC32(const char *pData, qint32 nDataSize, quint32 nInit, quint32 *pCRCTable);
    static quint16 _getCRC16(const char *pData, qint32 nDataSize, quint16 nInit, quint16 *pCRCTable);
    static quint32 _getCRC32(const QByteArray &baData, quint32 nInit, quint32 *pCRCTable);
    static quint16 _getCRC16(const QByteArray &baData, quint16 nInit, quint16 *pCRCTable);
    quint32 _getCRC32(qint64 nOffset = 0, qint64 nSize = -1, quint32 nInit = 0xFFFFFFFF, quint32 *pCRCTable = _getCRC32Table_EDB88320(), PDSTRUCT *pPdStruct = nullptr);
    quint16 _getCRC16(qint64 nOffset = 0, qint64 nSize = -1, quint16 nInit = 0, PDSTRUCT *pPdStruct = nullptr);

    static quint32 _getCRC32ByFileContent(const QString &sFileName);
    static quint32 _getCRC32ByDirectory(const QString &sDirectoryName, bool bRecursive, quint32 nInit = 0xFFFFFFFF);  // TODO PDSTRUCT

    static double getEntropy(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    static double getEntropy(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);

    enum BSTATUS {
        BSTATUS_ENTROPY = 0,
        BSTATUS_ZEROS,
        BSTATUS_GRADIENT,
        BSTATUS_TEXT
    };

    double getBinaryStatus(BSTATUS bstatus, qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);
    bool isZeroFilled(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct = nullptr);

    BYTE_COUNTS getByteCounts(qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    void _xor(quint8 nXorValue, qint64 nOffset = 0, qint64 nSize = -1, PDSTRUCT *pPdStruct = nullptr);

    //    static quint32 _ror32(quint32 nValue,quint32 nShift);
    //    static quint32 _rol32(quint32 nValue,quint32 nShift);
    static quint32 getStringCustomCRC32(const QString &sString);

    QIODevice *getDevice();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr);
    static bool isValid(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1, PDSTRUCT *pPdStruct = nullptr);
    static MODE getMode(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

    bool isBigEndian();
    bool isLittleEndian();
    bool is16();
    bool is32();
    bool is64();
    static bool isBigEndian(_MEMORY_MAP *pMemoryMap);
    static bool isLittleEndian(_MEMORY_MAP *pMemoryMap);
    static bool is16(_MEMORY_MAP *pMemoryMap);
    static bool is32(_MEMORY_MAP *pMemoryMap);
    static bool is64(_MEMORY_MAP *pMemoryMap);

    void setVersion(const QString &sVersion);
    void setOptions(const QString &sOptions);
    virtual QString getVersion();
    virtual QString getInfo(PDSTRUCT *pPdStruct = nullptr);

    virtual bool isEncrypted();

    virtual bool isCommentPresent();
    virtual QString getComment();

    static QString getSignature(QIODevice *pDevice, qint64 nOffset, qint64 nSize);
    QString getSignature(qint64 nOffset, qint64 nSize);

    OFFSETSIZE convertOffsetAndSize(qint64 nOffset,
                                    qint64 nSize);  // TODO rename
    static OFFSETSIZE convertOffsetAndSize(QIODevice *pDevice, qint64 nOffset,
                                           qint64 nSize);  // TODO rename

    static bool compareSignatureStrings(const QString &sBaseSignature, const QString &sOptSignature);  // TODO pdstruct
    static QString stringToHex(const QString &sString);
    static QString hexToString(const QString &sHex);
    static QString floatToString(float fValue, qint32 nPrec = 2);
    static QString doubleToString(double dValue, qint32 nPrec = 2);

    static quint8 hexToUint8(const QString &sHex);
    static qint8 hexToInt8(const QString &sHex);
    static quint16 hexToUint16(const QString &sHex, bool bIsBigEndian = false);
    static qint16 hexToInt16(const QString &sHex, bool bIsBigEndian = false);
    static quint32 hexToUint32(const QString &sHex, bool bIsBigEndian = false);
    static qint32 hexToInt32(const QString &sHex, bool bIsBigEndian = false);
    static quint64 hexToUint64(const QString &sHex, bool bIsBigEndian = false);
    static qint64 hexToInt64(const QString &sHex, bool bIsBigEndian = false);

    static QString invertHexByteString(const QString &sHex);

    static void _swapBytes(char *pSource, qint32 nSize);
    static quint16 swapBytes(quint16 nValue);
    static quint32 swapBytes(quint32 nValue);
    static quint64 swapBytes(quint64 nValue);

    bool isPlainTextType();
    bool isPlainTextType(QByteArray *pbaData);
    bool isUTF8TextType();
    bool isUTF8TextType(QByteArray *pbaData);

    static bool isPlainTextType(QIODevice *pDevice);

    enum UNICODE_TYPE {
        UNICODE_TYPE_NONE = 0,
        UNICODE_TYPE_LE,
        UNICODE_TYPE_BE
    };

    UNICODE_TYPE getUnicodeType();
    UNICODE_TYPE getUnicodeType(QByteArray *pbaData);

    static bool tryToOpen(QIODevice *pDevice);

    bool checkOffsetSize(OFFSETSIZE osRegion);
    bool checkOffsetSize(qint64 nOffset, qint64 nSize);

    static QString get_uint8_full_version(quint8 nValue);
    static QString get_uint16_full_version(quint16 nValue);
    static QString get_uint32_full_version(quint32 nValue);
    static QString get_uint64_full_version(quint64 nValue);
    static QString get_uint16_version(quint16 nValue);
    static QString get_uint32_version(quint32 nValue);
    static bool isResizeEnable(QIODevice *pDevice);
    static bool resize(QIODevice *pDevice, qint64 nSize);

    struct PACKED_UINT {
        bool bIsValid;
        quint64 nValue;
        qint32 nByteSize;
    };

    PACKED_UINT read_uleb128(qint64 nOffset, qint64 nSize);
    static PACKED_UINT _read_uleb128(char *pData, qint64 nSize);

    PACKED_UINT read_acn1_integer(qint64 nOffset, qint64 nSize);
    static PACKED_UINT _read_acn1_integer(char *pData, qint64 nSize);

    // ASN.1 helpers (moved from XPE)
    QString read_ASN_OIDString(qint64 nOffset, qint64 nSize);
    qint64 read_ASN_Integer(qint64 nOffset, qint64 nSize);
    bool read_ASN_Bool(qint64 nOffset, qint64 nSize);
    QDateTime read_ASN_DateTime(qint64 nOffset, qint64 nSize);
    QString read_ASN_AnsiString(qint64 nOffset, qint64 nSize);

    static PACKED_UINT _read_packedNumber(char *pData, qint64 nSize);
    PACKED_UINT read_packedNumber(qint64 nOffset, qint64 nSize);

    static QList<QString> getListFromFile(const QString &sFileName);

    bool _handleOverlay(_MEMORY_MAP *pMemoryMap);

    qint64 getOverlaySize(PDSTRUCT *pPdStruct = nullptr);
    static qint64 getOverlaySize(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct);
    qint64 getOverlayOffset(PDSTRUCT *pPdStruct = nullptr);
    static qint64 getOverlayOffset(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct);
    bool isOverlayPresent(PDSTRUCT *pPdStruct = nullptr);
    static bool isOverlayPresent(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct);

    bool compareOverlay(const QString &sSignature, qint64 nOffset, PDSTRUCT *pPdStruct);
    bool compareOverlay(_MEMORY_MAP *pMemoryMap, const QString &sSignature, qint64 nOffset, PDSTRUCT *pPdStruct);

    bool addOverlay(char *pData, qint64 nDataSize, PDSTRUCT *pPdStruct);
    bool addOverlay(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    bool removeOverlay();

    bool isSignatureInFilePartPresent(qint32 nFilePartNumber, const QString &sSignature);
    bool isSignatureInFilePartPresent(_MEMORY_MAP *pMemoryMap, qint32 nFilePartNumber, const QString &sSignature, PDSTRUCT *pPdStruct = nullptr);

    static QString getStringCollision(QList<QString> *pListStrings, const QString &sString1, const QString &sString2);

    static bool writeToFile(const QString &sFileName, const QByteArray &baData);
    static bool writeToFile(const QString &sFileName, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    static bool appendToFile(const QString &sFileName, const QString &sString);  // TODO rename
    static bool clearFile(const QString &sFileName);
    static qint32 getStringNumberFromList(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static qint32 getStringNumberFromListExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static bool isStringInListPresent(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static bool isStringInListPresentExp(QList<QString> *pListStrings, const QString &sString, PDSTRUCT *pPdStruct = nullptr);
    static QString getStringByIndex(QList<QString> *pListStrings, qint32 nIndex, qint32 nNumberOfStrings = -1);

    static bool isStringUnicode(const QString &sString, qint32 nMaxCheckSize = -1);

    static quint32 elfHash(const quint8 *pData);

    static QString getVersionString(const QString &sString);
    static qint64 getVersionIntValue(const QString &sString);
    static bool checkStringNumber(const QString &sString, quint32 nMin, quint32 nMax);

    enum DT_TYPE {
        DT_TYPE_UNKNOWN = 0,
        DT_TYPE_POSIX,
        DT_TYPE_DOSTIME,
        DT_TYPE_DOSDATE,
        DT_TYPE_UNIXTIME,
        DT_TYPE_FILETIME
    };

    static QDateTime valueToTime(quint64 nValue, DT_TYPE type);
    static QString valueToTimeString(quint64 nValue, DT_TYPE type);
    static QString msecToDate(quint64 nValue);

    // DOS date/time helpers (used by ZIP, RAR, etc.)
    // Converts MS-DOS packed date (nDosDate) and time (nDosTime) into QDateTime (local time).
    // If the values are invalid (e.g., zero month/day), returns an invalid QDateTime.
    static QDateTime dosDateTimeToQDateTime(quint16 nDosDate, quint16 nDosTime);
    // Converts QDateTime to MS-DOS packed date and time format.
    // Returns a pair where first is nDosDate and second is nDosTime.
    // If date/time is invalid or out of DOS range (1980-2107), returns {0, 0}.
    static QPair<quint16, quint16> qDateTimeToDosDateTime(const QDateTime &dateTime);

    static QString valueToFlagsString(quint64 nValue, const QMap<quint64, QString> &mapFlags, VL_TYPE vlType);

    static bool isX86asm(const QString &sArch);  // TODO remove use getDisasmMode
    static QString disasmIdToString(DM disasmMode);
    static QString disasmIdToArch(DM disasmMode);
    static QString syntaxIdToString(SYNTAX syntax);
    static SYNTAX stringToSyntaxId(const QString &sString);
    static QString osNameIdToString(OSNAME osName);
    DM getDisasmMode();
    static DM getDisasmMode(_MEMORY_MAP *pMemoryMap);
    static DM getDisasmMode(const QString &sArch, bool bIsBigEndian = false, MODE mode = MODE_UNKNOWN);
    static DM getDisasmMode(FILEFORMATINFO *pFileFormatInfo);
    static DMFAMILY getDisasmFamily(DM disasmMode);
    static DMFAMILY getDisasmFamily(_MEMORY_MAP *pMemoryMap);
    static QList<SYNTAX> getDisasmSyntax(DM disasmMode);
    static MODE getModeFromDisasmMode(DM disasmMode);

    static bool checkFileType(XBinary::FT fileTypeMain, XBinary::FT fileTypeOptional);

    static void filterFileTypes(QSet<XBinary::FT> *pStFileTypes);
    static void filterFileTypes(QSet<XBinary::FT> *pStFileTypes, XBinary::FT fileType);
    static void removeFileTypes(QSet<XBinary::FT> *pStFileTypes);

    static bool isFileTypePresent(QSet<XBinary::FT> *pStFileTypes, QSet<XBinary::FT> *pStAvailableFileTypes);

    struct PERCENTAGE {
        qint64 nCurrentValue;
        qint64 nMaxValue;
        qint32 nCurrentPercentage;
        qint32 nMaxPercentage;
        bool bTimer;
        QElapsedTimer timer;
    };

    static PERCENTAGE percentageInit(qint64 nMaxValue, bool bTimer = false);
    static bool percentageSetCurrentValue(PERCENTAGE *pPercentage, qint64 nCurrentValue);

    static qint64 getTotalOSSize(QList<OFFSETSIZE> *pListOffsetSize);

    static MODE getWidthModeFromSize(quint64 nSize);  // TODO rename check
    static MODE getWidthModeFromSize_32_64(quint64 nSize);
    static MODE getWidthModeFromMemoryMap(_MEMORY_MAP *pMemoryMap);

    static MODE getWidthModeFromByteSize(quint32 nByteSize);
    static quint32 getByteSizeFromWidthMode(MODE mode);

    static bool isAnsiSymbol(quint8 cCode, bool bExtra = false);
    static bool isUTF8Symbol(quint8 cCode, qint32 *pnWidth);
    static bool isUnicodeSymbol(quint16 nCode, bool bExtra = false);
    QString getStringFromIndex(qint64 nOffset, qint64 nSize, qint32 nIndex);

    static QList<QString> getAllFilesFromDirectory(const QString &sDirectory, const QString &sExtension);

    enum OPCODE_STATUS {
        OPCODE_STATUS_SUCCESS = 0,
        OPCODE_STATUS_END
    };

    QList<OPCODE> getOpcodes(qint64 nOffset, XADDR nStartAddress, qint64 nSize, quint32 nType, PDSTRUCT *pPdStruct = nullptr);                            // TODO remove
    virtual XADDR readOpcodes(quint32 nType, char *pData, XADDR nStartAddress, qint64 nSize, QList<OPCODE> *pListOpcodes, OPCODE_STATUS *pOpcodeStatus);  // TODO remove

    bool _read_opcode_uleb128(OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix);     // TODO remove
    bool _read_opcode_ansiString(OPCODE *pOpcode, char **ppData, qint64 *pnSize, XADDR *pnAddress, XADDR *pnResult, const QString &sPrefix);  // TODO remove

    QList<quint32> get_uint32_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian = false);
    QList<quint64> get_uint64_list(qint64 nOffset, qint32 nNumberOfRecords, bool bIsBigEndian = false);

    static bool _isOffsetsCrossed(qint64 nOffset1, qint64 nSize1, qint64 nOffset2, qint64 nSize2);
    static bool _isAddressCrossed(XADDR nAddress1, qint64 nSize1, XADDR nAddress2, qint64 nSize2);
    static bool _isReplaced(qint64 nOffset, qint64 nSize, QList<MEMORY_REPLACE> *pListMemoryReplace);
    static bool _replaceMemory(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<MEMORY_REPLACE> *pListMemoryReplace);
    static bool _updateReplaces(qint64 nDataOffset, char *pData, qint64 nDataSize, QList<MEMORY_REPLACE> *pListMemoryReplace);

    virtual QList<SYMBOL_RECORD> getSymbolRecords(XBinary::_MEMORY_MAP *pMemoryMap, SYMBOL_TYPE symbolType = SYMBOL_TYPE_ALL);
    static SYMBOL_RECORD findSymbolByAddress(QList<SYMBOL_RECORD> *pListSymbolRecords, XADDR nAddress);     // Check InfoDB
    static SYMBOL_RECORD findSymbolByName(QList<SYMBOL_RECORD> *pListSymbolRecords, const QString &sName);  // Check InfoDB
    static SYMBOL_RECORD findSymbolByOrdinal(QList<SYMBOL_RECORD> *pListSymbolRecords, qint32 nOrdinal);    // Check InfoDB

    static QString generateUUID();

    static QString appendText(const QString &sResult, const QString &sString, const QString &sSeparate);
    static QString appendComma(const QString &sResult, const QString &sString);
    static QString bytesCountToString(quint64 nValue, quint64 nBase = 1024);
    static QString numberToString(quint64 nValue);
    static QString fullVersionByteToString(quint8 nValue);
    static QString fullVersionWordToString(quint16 nValue);
    static QString fullVersionDwordToString(quint32 nValue);
    static QString fullVersionQwordToString(quint64 nValue);
    static QString versionWordToString(quint16 nValue);
    static QString versionDwordToString(quint32 nValue);
    static QString versionDivModByteToString(quint8 nValue);
    static QString versionDivModWordToString(quint16 nValue);
    static QString formatXML(const QString &sXML);

    struct XDWORD {
        quint16 nValue1;
        quint16 nValue2;
    };

    static quint32 make_dword(XDWORD xdword);
    static quint32 make_dword(quint16 nValue1, quint16 nValue2);
    static XDWORD make_xdword(quint32 nValue);

    static QString recordFilePartIdToString(FILEPART id);
    static QString recordFilePartIdToFtString(FILEPART id);

    static bool checkVersionString(const QString &sVersion);
    static QString cleanString(const QString &sString);

    static XVARIANT getXVariant(bool bValue);
    static XVARIANT getXVariant(quint8 nValue);
    static XVARIANT getXVariant(quint16 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint32 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nValue, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nValue[2], bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nLow, quint64 nHigh, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint64 nLow1, quint64 nLow2, quint64 nHigh1, quint64 nHigh2, bool bIsBigEndian = false);
    static XVARIANT getXVariant(quint8 nValue[10], bool bIsBigEndian = false);

    static quint64 xVariantToQword(const XVARIANT &xvariant);

    static quint32 getDwordFromQword(quint64 nValue, qint32 nIndex);
    static quint16 getWordFromQword(quint64 nValue, qint32 nIndex);
    static quint8 getByteFromQword(quint64 nValue, qint32 nIndex);
    static quint16 getWordFromDword(quint32 nValue, qint32 nIndex);
    static quint8 getByteFromDword(quint32 nValue, qint32 nIndex);
    static quint8 getByteFromWord(quint16 nValue, qint32 nIndex);
    static bool getBitFromByte(quint8 nValue, qint32 nIndex);
    static bool getBitFromWord(quint16 nValue, qint32 nIndex);
    static bool getBitFromDword(quint32 nValue, qint32 nIndex);
    static bool getBitFromQword(quint64 nValue, qint32 nIndex);

    static quint64 setDwordToQword(quint64 nInit, quint32 nValue, qint32 nIndex);
    static quint64 setWordToQword(quint64 nInit, quint16 nValue, qint32 nIndex);
    static quint64 setByteToQword(quint64 nInit, quint8 nValue, qint32 nIndex);
    static quint32 setWordToDword(quint32 nInit, quint16 nValue, qint32 nIndex);
    static quint32 setByteToDword(quint32 nInit, quint8 nValue, qint32 nIndex);
    static quint16 setByteToWord(quint16 nInit, quint8 nValue, qint32 nIndex);
    static quint8 setBitToByte(quint8 nInit, bool bValue, qint32 nIndex);
    static quint16 setBitToWord(quint16 nInit, bool bValue, qint32 nIndex);
    static quint32 setBitToDword(quint32 nInit, bool bValue, qint32 nIndex);
    static quint64 setBitToQword(quint64 nInit, bool bValue, qint32 nIndex);

    static bool isXVariantEqual(XVARIANT value1, XVARIANT value2);
    static void clearXVariant(XVARIANT *pVar);

    static MODE getModeOS();

    static PDSTRUCT createPdStruct();
    static void setPdStructInit(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nTotal);
    static void setPdStructTotal(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue);
    static void setPdStructCurrent(PDSTRUCT *pPdStruct, qint32 nIndex, qint64 nValue);
    static void setPdStructCurrentIncrement(PDSTRUCT *pPdStruct, qint32 nIndex);
    static void setPdStructStatus(PDSTRUCT *pPdStruct, qint32 nIndex, const QString &sStatus);
    static void setPdStructFinished(PDSTRUCT *pPdStruct, qint32 nIndex);
    static void setPdStructInfoString(PDSTRUCT *pPdStruct, const QString &sInfoString);
    static void setPdStructErrorString(PDSTRUCT *pPdStruct, const QString &sErrorString);
    static void clearPdStructInfoString(PDSTRUCT *pPdStruct);
    static void clearPdStructErrorString(PDSTRUCT *pPdStruct);
    static QString getPdStructInfoString(PDSTRUCT *pPdStruct);
    static QString getPdStructErrorString(PDSTRUCT *pPdStruct);
    static qint32 getFreeIndex(PDSTRUCT *pPdStruct);
    static bool isPdStructFinished(PDSTRUCT *pPdStruct);
    static bool isPdStructNotCanceled(PDSTRUCT *pPdStruct);
    static bool isPdStructSuccess(PDSTRUCT *pPdStruct);
    static bool isPdStructStopped(PDSTRUCT *pPdStruct);
    static void setPdStructStopped(PDSTRUCT *pPdStruct);
    static qint32 getPdStructPercentage(PDSTRUCT *pPdStruct);  // 0-100
    static void invokePdStructCallback(PDSTRUCT *pPdStruct, qint32 nMinIntervalMs = 100);

    struct REGION_FILL {
        quint64 nSize;
        quint8 nByte;
    };

    REGION_FILL getRegionFill(qint64 nOffset, qint64 nSize, qint32 nAlignment);
    static QString getDataString(char *pData, qint32 nDataSize, const QString &sBaseType, bool bIsBigEndian);

    QList<FPART> getHighlights(HLTYPE hlType, PDSTRUCT *pPdStruct = nullptr);
    virtual quint32 hlTypeToFParts(HLTYPE hlType);

    static qint64 align_up(qint64 nValue, qint64 nAlignment);
    static qint64 align_down(qint64 nValue, qint64 nAlignment);

    static char *strCopy(char *dest, const char *src, size_t destSize);
    static QString getAndroidVersionFromApi(quint32 nAPI);
    static QString getiOSVersionFromDarwin(quint32 nDarwin);

    static QString _fromWCharArray(const wchar_t *pWString, qint32 size = -1);
    static qint32 _toWCharArray(const QString &sString, wchar_t *pWString);

    enum DSMODE {
        DSMODE_NONE = 0,
        DSMODE_NOPRINT_TO_DOT
    };

    static QString dataToString(const QByteArray &baData, DSMODE dsmode);
    static QString convertSignature(const QString &sSignature);

    struct STRINGTABLE_RECORD {
        qint64 nOffsetFromStart;
        QString sString;
        qint32 nSizeInBytes;
    };

    QList<STRINGTABLE_RECORD> getStringTable_ANSI(qint64 nOffset, qint64 nSize, PDSTRUCT *pPdStruct);

    virtual bool isDebugBuild();
    virtual bool isReleaseBuild();

    enum FMT_MSG_TYPE {
        FMT_MSG_TYPE_UNKNOWN = 0,
        FMT_MSG_TYPE_INFO,
        FMT_MSG_TYPE_ERROR,
        FMT_MSG_TYPE_WARNING
    };

    enum FMT_MSG_CODE {
        FMT_MSG_CODE_UNKNOWN = 0,
        FMT_MSG_CODE_INVALID_ENTRYPOINT,
        FMT_MSG_CODE_INVALID_CHECKSUM,
        FMT_MSG_CODE_INVALID_HEADER,
        FMT_MSG_CODE_INVALID_SECTIONALIGNMENT,
        FMT_MSG_CODE_INVALID_FILEALIGNMENT,
        FMT_MSG_CODE_INVALID_RELOCSTABLE,
        FMT_MSG_CODE_INVALID_IMPORTTABLE,
        FMT_MSG_CODE_INVALID_EXPORTTABLE,
        FMT_MSG_CODE_INVALID_RESOURCESTABLE,
        FMT_MSG_CODE_INVALID_SECTIONSTABLE,
    };

    struct FMT_MSG {
        FMT_MSG_TYPE type;
        FMT_MSG_CODE code;
        QVariant value;
        QString sString;
    };

    virtual QList<FMT_MSG> checkFileFormat(bool bDeep, PDSTRUCT *pPdStruct);
    bool isFileFormatValid(bool bDeep, PDSTRUCT *pPdStruct);

    static QList<QString> getFileFormatMessages(const QList<FMT_MSG> *pListFmtMsg);

    static bool isFmtMsgCodePresent(const QList<FMT_MSG> *pListFmtMsgs, FMT_MSG_CODE code, FMT_MSG_TYPE type, PDSTRUCT *pPdStruct);
    static bool _addCheckFormatTest(QList<FMT_MSG> *pListFmtMsgs, bool *pbContinue, FMT_MSG_CODE code, FMT_MSG_TYPE type, const QString &sString, QVariant value,
                                    QString sInfo, bool bFailCase);

    void dumpMemoryMap();
    void dumpHeaders();

    enum CRC_TYPE {
        CRC_TYPE_UNKNOWN = 0,
        CRC_TYPE_EDB88320,
        CRC_TYPE_ADLER32,
        CRC_TYPE_FFFFFFFF_EDB88320_00000000,
        CRC_TYPE_FFFFFFFF_EDB88320_FFFFFFFFF
    };

    static bool checkCRC(QIODevice *pDevice, CRC_TYPE crcType, QVariant value, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr);
    static FPART getFPART(FILEPART filePart, const QString &sOriginalName, qint64 nFileOffset, qint64 nFileSize, XADDR nVirtualAddress, qint64 nVirtualSize);

    qint64 getNumberOfArchiveRecords(PDSTRUCT *pPdStruct);
    QList<ARCHIVERECORD> getArchiveRecords(qint32 nLimit, PDSTRUCT *pPdStruct);
    bool packFolderToDevice(QIODevice *pDevice, const QMap<PACK_PROP, QVariant> &mapProperties, const QString &sFolderName, PDSTRUCT *pPdStruct);
    virtual bool unpack(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<QString> getListOfArchiveRecordNames(const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct = nullptr);

    // Streaming unpacking API
    virtual bool initUnpack(UNPACK_STATE *pState, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct = nullptr);
    virtual ARCHIVERECORD infoCurrent(UNPACK_STATE *pState, PDSTRUCT *pPdStruct = nullptr);
    virtual bool unpackCurrent(UNPACK_STATE *pState, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual bool moveToNext(UNPACK_STATE *pState, PDSTRUCT *pPdStruct = nullptr);
    virtual bool finishUnpack(UNPACK_STATE *pState, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<FPART_PROP> getAvailableFPARTProperties();

    // Streaming packing API
    virtual bool initPack(PACK_STATE *pState, QIODevice *pDevice, const QMap<PACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct = nullptr);
    virtual bool addDevice(PACK_STATE *pState, QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual bool addFile(PACK_STATE *pState, const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);
    virtual bool addFolder(PACK_STATE *pState, const QString &sDirectoryPath, PDSTRUCT *pPdStruct = nullptr);
    virtual bool finishPack(PACK_STATE *pState, PDSTRUCT *pPdStruct = nullptr);

    virtual QVariant calculateHash(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    static QVariant calculateHash(const QString &sFileName, PDSTRUCT *pPdStruct = nullptr);

    bool unpackToFolder(const QString &sFolderName, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct = nullptr);
    bool unpackSingleStream(QIODevice *pOutDevice, const QMap<UNPACK_PROP, QVariant> &mapProperties, PDSTRUCT *pPdStruct = nullptr);

    struct FFSEARCH_STATE {
        QIODevice *pDevice;     // Input device
        qint64 nStartOffset;    // Start offset for search
        qint64 nSize;           // Size for search
        qint64 nCurrentOffset;  // Current position
        void *pContext;         // Format-specific context (cast to format's context struct)
    };

    struct FFSEARCH_INFO {
        bool bIsValid;
        FT fileTYPE;
        qint64 nOffset;
        qint64 nSize;
        QString sString;
        QString sExt;
    };

    virtual bool initFFSearch(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct = nullptr);
    virtual FFSEARCH_INFO searchFFNext(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct = nullptr);
    virtual bool finishFFSearch(FFSEARCH_STATE *pState, PDSTRUCT *pPdStruct = nullptr);

    virtual QList<QString> getSearchSignatures();
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1);

    static qint32 getBufferSize(PDSTRUCT *pPdStruct);
    static qint32 getFileBufferSize(PDSTRUCT *pPdStruct);
    static QIODevice *createFileBuffer(qint64 nSize, PDSTRUCT *pPdStruct);
    static void freeFileBuffer(QIODevice **ppBuffer);

    static QString getArchiveRecordComment(const ARCHIVERECORD &record);
    static QString getHandleMethods(const QMap<FPART_PROP, QVariant> &mapProperties);

private:
    static QString qcharToHex(QChar cSymbol);

    static QList<SIGNATURE_RECORD> getSignatureRecords(const QString &sSignature, bool *pbValid, PDSTRUCT *pPdStruct);
    bool _compareSignature(_MEMORY_MAP *pMemoryMap, QList<SIGNATURE_RECORD> *pListSignatureRecords, qint64 nOffset, PDSTRUCT *pPdStruct);

    static qint32 _getSignatureSkip(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotANSI(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureNotANSIAndNull(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureANSIAndNumber(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureDelta(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid, PDSTRUCT *pPdStruct);
    static qint32 _getSignatureRelOffset(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureAddress(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex);
    static qint32 _getSignatureBytes(QList<SIGNATURE_RECORD> *pListSignatureRecords, const QString &sSignature, qint32 nStartIndex, bool *pbValid, PDSTRUCT *pPdStruct);

protected:
    bool _isOffsetValid(qint64 nOffset);
    void _errorMessage(const QString &sErrorMessage, PDSTRUCT *pPdStruct = nullptr);
    void _infoMessage(const QString &sInfoMessage, PDSTRUCT *pPdStruct = nullptr);
    qint64 _calculateRawSize(PDSTRUCT *pPdStruct);
    static qint64 _calculateRawSize(_MEMORY_MAP *pMemoryMap, PDSTRUCT *pPdStruct);

signals:
    void errorMessage(const QString &sErrorMessage);
    void infoMessage(const QString &sInfoMessage);

private:
    QIODevice *m_pDevice;
    const char *m_pConstMemory;
    QString m_sFileName;
    QFile *m_pFile;
    QMutex *m_pReadWriteMutex;
    bool m_bIsImage;
    XADDR m_nBaseAddress;
    qint64 m_nEntryPointOffset;
    XADDR m_nModuleAddress;
    QString m_sArch;
    QString m_sFileFormatExt;
    qint64 m_nFileFormatSize;
    OSNAME m_osName;
    QString m_sOsVersion;
    ENDIAN m_endian;
    MODE m_mode;
    QString m_sVersion;
    QString m_sOptions;
    qint32 m_nType;
    FT m_fileType;
    bool m_bMultiSearchCallback;
    qint64 m_nSize;
    bool m_bIsExecutable;
    bool m_bIsArchive;
    QString m_sFileFormatExts;
};

bool compareMemoryMapRecord(const XBinary::_MEMORY_RECORD &a, const XBinary::_MEMORY_RECORD &b);

#endif  // XBINARY_H
