/* Copyright (c) 2025-2026 hors<horsicq@gmail.com>
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
#ifndef XXM_H
#define XXM_H

#include <QObject>
#include "xbinary.h"

class XXM : public XBinary {
    Q_OBJECT
public:
    virtual QList<QString> getSearchSignatures() override;
    virtual XBinary *createInstance(QIODevice *pDevice, bool bIsImage = false, XADDR nModuleAddress = -1) override;
    enum STRUCTID {
        STRUCTID_UNKNOWN = 0,
        STRUCTID_HEADER,
        STRUCTID_PATTERN_HEADER,
        STRUCTID_PATTERN_DATA,
        STRUCTID_INSTRUMENT_HEADER,
        STRUCTID_INSTRUMENT_EXTRA_HEADER,
        STRUCTID_SAMPLE_HEADER,
        STRUCTID_SAMPLE_DATA,
        // Add more as needed
    };

#pragma pack(push, 1)
    struct HEADER {
        char id_text[17];           // 'Extended Module: '
        char module_name[20];       // Module name, zero padded
        quint8 ctrl_byte_1a;        // Always $1A
        char tracker_name[20];      // Tracker name
        quint16 version;            // Version number (hi-byte major, low-byte minor)
        quint32 header_size;        // Header size (from this offset)
        quint16 song_length;        // Song length (in pattern order table)
        quint16 song_restart;       // Song restart position
        quint16 channels;           // Number of channels
        quint16 num_patterns;       // Number of patterns
        quint16 num_instruments;    // Number of instruments
        quint16 flags;              // Flags (bit0: 0=Amiga freq table, 1=Linear freq table)
        quint16 default_tempo;      // Default tempo
        quint16 default_bpm;        // Default BPM
        quint8 pattern_order[256];  // Pattern order table
    };

    struct PATTERN_HEADER {
        quint32 header_length;
        quint8 packing_type;  // always 0
        quint16 num_rows;     // 1..256
        quint16 packed_data_size;
        // ... pattern data follows
    };

    struct INSTRUMENT_HEADER {
        quint32 instrument_header_size;
        char instrument_name[22];
        quint8 instrument_type;  // always 0, but may be random
        quint16 num_samples;
        // If num_samples > 0, extra header follows
    };

    struct INSTRUMENT_EXTRA_HEADER {
        quint32 sample_header_size;
        quint8 sample_number_for_notes[96];
        quint8 volume_envelope_points[48];
        quint8 panning_envelope_points[48];
        quint8 num_volume_points;
        quint8 num_panning_points;
        quint8 volume_sustain_point;
        quint8 volume_loop_start_point;
        quint8 volume_loop_end_point;
        quint8 panning_sustain_point;
        quint8 panning_loop_start_point;
        quint8 panning_loop_end_point;
        quint8 volume_type;
        quint8 panning_type;
        quint8 vibrato_type;
        quint8 vibrato_sweep;
        quint8 vibrato_depth;
        quint8 vibrato_rate;
        quint16 volume_fadeout;
        quint16 reserved;
    };

    struct SAMPLE_HEADER {
        quint32 sample_length;
        quint32 loop_start;
        quint32 loop_length;
        quint8 volume;
        qint8 finetune;
        quint8 type;  // bits: 0-1 loop type, 4 = 16bit sample data
        quint8 panning;
        qint8 relative_note_number;
        quint8 reserved;
        char sample_name[22];
    };
#pragma pack(pop)

    XXM(QIODevice *pDevice = nullptr);
    ~XXM();

    virtual bool isValid(PDSTRUCT *pPdStruct = nullptr) override;
    static bool isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct = nullptr);
    virtual FT getFileType() override;
    virtual MODE getMode() override;
    virtual QString getMIMEString() override;
    virtual qint32 getType() override;
    virtual QString typeIdToString(qint32 nType) override;
    virtual ENDIAN getEndian() override;
    virtual QString getArch() override;
    virtual QString getFileFormatExt() override;
    virtual qint64 getFileFormatSize(PDSTRUCT *pPdStruct) override;
    virtual bool isSigned() override;
    virtual OSNAME getOsName() override;
    virtual QString getOsVersion() override;
    virtual QString getVersion() override;
    virtual bool isEncrypted() override;
    virtual QList<MAPMODE> getMapModesList() override;
    virtual _MEMORY_MAP getMemoryMap(MAPMODE mapMode = MAPMODE_UNKNOWN, PDSTRUCT *pPdStruct = nullptr) override;
    virtual QList<FPART> getFileParts(quint32 nFileParts, qint32 nLimit = -1, PDSTRUCT *pPdStruct = nullptr) override;

    virtual QString structIDToString(quint32 nID) override;
    virtual QList<DATA_HEADER> getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct) override;
    virtual qint32 readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<QVariant> *pListValues, void *pUserData,
                                PDSTRUCT *pPdStruct);

    HEADER _read_HEADER(qint64 nOffset);
    PATTERN_HEADER _read_PATTERN_HEADER(qint64 nOffset);
    INSTRUMENT_HEADER _read_INSTRUMENT_HEADER(qint64 nOffset);
    INSTRUMENT_EXTRA_HEADER _read_INSTRUMENT_EXTRA_HEADER(qint64 nOffset);
    SAMPLE_HEADER _read_SAMPLE_HEADER(qint64 nOffset);

    // Add more as needed
};

#endif  // XXM_H

