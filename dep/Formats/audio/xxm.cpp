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
#include "xxm.h"

static XBinary::XCONVERT _TABLE_XXM_STRUCTID[] = {
    {XXM::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
    {XXM::STRUCTID_HEADER, "HEADER", QString("HEADER")},
    {XXM::STRUCTID_PATTERN_HEADER, "PATTERN_HEADER", QString("PATTERN_HEADER")},
    {XXM::STRUCTID_PATTERN_DATA, "PATTERN_DATA", QString("PATTERN_DATA")},
    {XXM::STRUCTID_INSTRUMENT_HEADER, "INSTRUMENT_HEADER", QString("INSTRUMENT_HEADER")},
    {XXM::STRUCTID_INSTRUMENT_EXTRA_HEADER, "INSTRUMENT_EXTRA_HEADER", QString("INSTRUMENT_EXTRA_HEADER")},
    {XXM::STRUCTID_SAMPLE_HEADER, "SAMPLE_HEADER", QString("SAMPLE_HEADER")},
    {XXM::STRUCTID_SAMPLE_DATA, "SAMPLE_DATA", QString("SAMPLE_DATA")}
    // Add more as needed
};

XXM::XXM(QIODevice *pDevice) : XBinary(pDevice)
{
}

XXM::~XXM()
{
}

bool XXM::isValid(PDSTRUCT *pPdStruct)
{
    bool bResult = false;

    if (getSize() >= 60) {
        QByteArray id = read_array_process(0, 17, pPdStruct);
        if (id == QByteArray("Extended Module: ")) {
            bResult = true;
        }
    }

    return bResult;
}

XBinary::FT XXM::getFileType()
{
    return XBinary::FT_XM;
}

bool XXM::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XXM xxm(pDevice);

    return xxm.isValid(pPdStruct);
}

XBinary::MODE XXM::getMode()
{
    return XBinary::MODE_8;
}

QString XXM::getMIMEString()
{
    return "audio/x-xm";
}

qint32 XXM::getType()
{
    return 0;
}

QString XXM::typeIdToString(qint32 nType)
{
    Q_UNUSED(nType)
    return "XM";
}

XBinary::ENDIAN XXM::getEndian()
{
    // XM is little-endian
    return XBinary::ENDIAN_LITTLE;
}

QString XXM::getArch()
{
    return QString();
}

QString XXM::getFileFormatExt()
{
    return "xm";
}

qint64 XXM::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)
    return getSize();
}

bool XXM::isSigned()
{
    return false;
}

XXM::OSNAME XXM::getOsName()
{
    return XBinary::OSNAME_UNKNOWN;
}

QString XXM::getOsVersion()
{
    return QString();
}

QString XXM::getVersion()
{
    HEADER header = _read_HEADER(0);
    return QString("%1.%2").arg(header.version >> 8).arg(header.version & 0xFF);
}

bool XXM::isEncrypted()
{
    return false;
}

QList<XXM::MAPMODE> XXM::getMapModesList()
{
    QList<MAPMODE> listResult;
    listResult.append(MAPMODE_REGIONS);
    return listResult;
}

XXM::_MEMORY_MAP XXM::getMemoryMap(MAPMODE mapMode, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(mapMode)
    return _getMemoryMap(FILEPART_HEADER | FILEPART_TABLE | FILEPART_DATA | FILEPART_OVERLAY, pPdStruct);
}

QList<XBinary::FPART> XXM::getFileParts(quint32 nFileParts, qint32 nLimit, PDSTRUCT *pPdStruct)
{
    Q_UNUSED(nLimit)
    QList<FPART> list;
    HEADER header = _read_HEADER(0);
    if (getSize() < 60) return list;

    qint64 offset = 60 + header.header_size;

    if (nFileParts & FILEPART_HEADER) {
        FPART f = {};
        f.filePart = FILEPART_HEADER;
        f.nFileOffset = 0;
        f.nFileSize = offset;
        f.nVirtualAddress = -1;
        f.sName = tr("Header");
        list.append(f);
    }

    // Patterns
    for (qint32 i = 0; (i < header.num_patterns) && isPdStructNotCanceled(pPdStruct); i++) {
        PATTERN_HEADER ph = _read_PATTERN_HEADER(offset);
        qint64 patternHeaderSize = 9;
        if (nFileParts & FILEPART_TABLE) {
            FPART t = {};
            t.filePart = FILEPART_TABLE;
            t.nFileOffset = offset;
            t.nFileSize = patternHeaderSize;
            t.nVirtualAddress = -1;
            t.sName = QString("Pattern %1 header").arg(i);
            list.append(t);
        }
        offset += patternHeaderSize;
        if ((nFileParts & FILEPART_DATA) && ph.packed_data_size > 0) {
            FPART d = {};
            d.filePart = FILEPART_DATA;
            d.nFileOffset = offset;
            d.nFileSize = ph.packed_data_size;
            d.nVirtualAddress = -1;
            d.sName = QString("Pattern %1 data").arg(i);
            list.append(d);
        }
        offset += ph.packed_data_size;
    }

    // Instruments
    for (qint32 i = 0; (i < header.num_instruments) && isPdStructNotCanceled(pPdStruct); i++) {
        INSTRUMENT_HEADER ih = _read_INSTRUMENT_HEADER(offset);
        qint64 ihFixed = 4 + 22 + 1 + 2;
        if (nFileParts & FILEPART_TABLE) {
            FPART t = {};
            t.filePart = FILEPART_TABLE;
            t.nFileOffset = offset;
            t.nFileSize = ihFixed;
            t.nVirtualAddress = -1;
            t.sName = QString("Instrument %1 header").arg(i);
            list.append(t);
        }
        qint64 extraOffset = offset + ihFixed;
        if (ih.num_samples > 0) {
            INSTRUMENT_EXTRA_HEADER ieh = _read_INSTRUMENT_EXTRA_HEADER(extraOffset);
            qint64 extraSize = 212;  // fixed size of extra header
            if (nFileParts & FILEPART_TABLE) {
                FPART t2 = {};
                t2.filePart = FILEPART_TABLE;
                t2.nFileOffset = extraOffset;
                t2.nFileSize = extraSize;
                t2.nVirtualAddress = -1;
                t2.sName = QString("Instrument %1 extra").arg(i);
                list.append(t2);
            }
            Q_UNUSED(ieh)
            qint64 sampleHeadersOffset = extraOffset + extraSize;
            for (qint32 s = 0; s < ih.num_samples && isPdStructNotCanceled(pPdStruct); s++) {
                SAMPLE_HEADER sh = _read_SAMPLE_HEADER(sampleHeadersOffset);
                if (nFileParts & FILEPART_TABLE) {
                    FPART shf = {};
                    shf.filePart = FILEPART_TABLE;
                    shf.nFileOffset = sampleHeadersOffset;
                    shf.nFileSize = 40;
                    shf.nVirtualAddress = -1;
                    shf.sName = QString("Instrument %1 sample %2 header").arg(i).arg(s);
                    list.append(shf);
                }
                sampleHeadersOffset += 40;
            }
            // Sample data blocks follow sample headers; sizes not trivially derivable here without decoding ADPCM/packed — skip safe mapping.
        }
        offset = qMax(offset, extraOffset + qMax<qint64>(0, ih.instrument_header_size - ihFixed));
    }

    if (nFileParts & FILEPART_OVERLAY) {
        qint64 maxEnd = 0;
        for (int i = 0; i < list.size(); ++i) {
            const FPART &p = list.at(i);
            maxEnd = qMax(maxEnd, p.nFileOffset + p.nFileSize);
        }
        if (maxEnd < getSize()) {
            FPART ov = {};
            ov.filePart = FILEPART_OVERLAY;
            ov.nFileOffset = maxEnd;
            ov.nFileSize = getSize() - maxEnd;
            ov.nVirtualAddress = -1;
            ov.sName = tr("Overlay");
            list.append(ov);
        }
    }

    return list;
}

QString XXM::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XXM_STRUCTID, sizeof(_TABLE_XXM_STRUCTID) / sizeof(XBinary::XCONVERT));
}

qint32 XXM::readTableRow(qint32 nRow, LT locType, XADDR nLocation, const DATA_RECORDS_OPTIONS &dataRecordsOptions, QList<QVariant> *pListValues, void *pUserData,
                         PDSTRUCT *pPdStruct)
{
    Q_UNUSED(pPdStruct)

    qint32 nResult = 0;
    if (dataRecordsOptions.dataHeaderFirst.dsID.nID == STRUCTID_HEADER) {
        HEADER header = _read_HEADER(locationToOffset(dataRecordsOptions.pMemoryMap, locType, nLocation));
        if (nRow == 0) {
            pListValues->append(QByteArray(header.id_text, 17));
            pListValues->append(QByteArray(header.module_name, 20));
            pListValues->append(header.ctrl_byte_1a);
            pListValues->append(QByteArray(header.tracker_name, 20));
            pListValues->append(header.version);
            pListValues->append(header.header_size);
            pListValues->append(header.song_length);
            pListValues->append(header.song_restart);
            pListValues->append(header.channels);
            pListValues->append(header.num_patterns);
            pListValues->append(header.num_instruments);
            pListValues->append(header.flags);
            pListValues->append(header.default_tempo);
            pListValues->append(header.default_bpm);
            pListValues->append(QByteArray(reinterpret_cast<const char *>(header.pattern_order), 256));
            nResult = 1;
        }
    }
    // Add for other structs as needed

    return nResult;
}

QList<XXM::DATA_HEADER> XXM::getDataHeaders(const DATA_HEADERS_OPTIONS &dataHeadersOptions, PDSTRUCT *pPdStruct)
{
    QList<DATA_HEADER> listResult;

    if (dataHeadersOptions.nID == STRUCTID_UNKNOWN) {
        DATA_HEADERS_OPTIONS _dataHeadersOptions = dataHeadersOptions;
        _dataHeadersOptions.bChildren = true;
        _dataHeadersOptions.dsID_parent = _addDefaultHeaders(&listResult, pPdStruct);
        _dataHeadersOptions.dhMode = XBinary::DHMODE_HEADER;

        _dataHeadersOptions.nID = STRUCTID_HEADER;
        _dataHeadersOptions.nLocation = 0;
        _dataHeadersOptions.locType = XBinary::LT_OFFSET;

        listResult.append(getDataHeaders(_dataHeadersOptions, pPdStruct));
    } else {
        qint64 nStartOffset = locationToOffset(dataHeadersOptions.pMemoryMap, dataHeadersOptions.locType, dataHeadersOptions.nLocation);

        if (nStartOffset != -1) {
            DATA_HEADER dataHeader = _initDataHeader(dataHeadersOptions, XXM::structIDToString(dataHeadersOptions.nID));

            if (dataHeadersOptions.nID == STRUCTID_HEADER) {
                dataHeader.nSize = sizeof(HEADER);
                dataHeader.listRecords.append(getDataRecord(offsetof(HEADER, id_text), 17, "id_text", VT_CHAR_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, module_name), 20, "module_name", VT_CHAR_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, ctrl_byte_1a), 1, "ctrl_byte_1a", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, tracker_name), 20, "tracker_name", VT_CHAR_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(HEADER, version), 2, "version", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, header_size), 4, "header_size", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, song_length), 2, "song_length", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, song_restart), 2, "song_restart", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(HEADER, channels), 2, "channels", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, num_patterns), 2, "num_patterns", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, num_instruments), 2, "num_instruments", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(HEADER, flags), 2, "flags", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, default_tempo), 2, "default_tempo", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, default_bpm), 2, "default_bpm", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(HEADER, pattern_order), 256, "pattern_order", VT_BYTE_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            } else if (dataHeadersOptions.nID == STRUCTID_PATTERN_HEADER) {
                dataHeader.nSize = sizeof(PATTERN_HEADER);
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(PATTERN_HEADER, header_length), 4, "header_length", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(PATTERN_HEADER, packing_type), 1, "packing_type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(PATTERN_HEADER, num_rows), 2, "num_rows", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(PATTERN_HEADER, packed_data_size), 2, "packed_data_size", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            } else if (dataHeadersOptions.nID == STRUCTID_INSTRUMENT_HEADER) {
                dataHeader.nSize = sizeof(INSTRUMENT_HEADER);
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_HEADER, instrument_header_size), 4, "instrument_header_size", VT_DWORD, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_HEADER, instrument_name), 22, "instrument_name", VT_CHAR_ARRAY, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_HEADER, instrument_type), 1, "instrument_type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_HEADER, num_samples), 2, "num_samples", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            } else if (dataHeadersOptions.nID == STRUCTID_INSTRUMENT_EXTRA_HEADER) {
                dataHeader.nSize = sizeof(INSTRUMENT_EXTRA_HEADER);
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, sample_header_size), 4, "sample_header_size", VT_DWORD, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, sample_number_for_notes), 96, "sample_number_for_notes", VT_BYTE_ARRAY,
                                                            DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_envelope_points), 48, "volume_envelope_points", VT_BYTE_ARRAY,
                                                            DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, panning_envelope_points), 48, "panning_envelope_points", VT_BYTE_ARRAY,
                                                            DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, num_volume_points), 1, "num_volume_points", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, num_panning_points), 1, "num_panning_points", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_sustain_point), 1, "volume_sustain_point", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_loop_start_point), 1, "volume_loop_start_point", VT_BYTE,
                                                            DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_loop_end_point), 1, "volume_loop_end_point", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, panning_sustain_point), 1, "panning_sustain_point", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, panning_loop_start_point), 1, "panning_loop_start_point", VT_BYTE,
                                                            DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, panning_loop_end_point), 1, "panning_loop_end_point", VT_BYTE, DRF_UNKNOWN,
                                                            dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_type), 1, "volume_type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, panning_type), 1, "panning_type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, vibrato_type), 1, "vibrato_type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, vibrato_sweep), 1, "vibrato_sweep", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, vibrato_depth), 1, "vibrato_depth", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, vibrato_rate), 1, "vibrato_rate", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, volume_fadeout), 2, "volume_fadeout", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(INSTRUMENT_EXTRA_HEADER, reserved), 2, "reserved", VT_WORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            } else if (dataHeadersOptions.nID == STRUCTID_SAMPLE_HEADER) {
                dataHeader.nSize = sizeof(SAMPLE_HEADER);
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, sample_length), 4, "sample_length", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, loop_start), 4, "loop_start", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, loop_length), 4, "loop_length", VT_DWORD, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(SAMPLE_HEADER, volume), 1, "volume", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, finetune), 1, "finetune", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(SAMPLE_HEADER, type), 1, "type", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(getDataRecord(offsetof(SAMPLE_HEADER, panning), 1, "panning", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, relative_note_number), 1, "relative_note_number", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, reserved), 1, "reserved", VT_BYTE, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
                dataHeader.listRecords.append(
                    getDataRecord(offsetof(SAMPLE_HEADER, sample_name), 22, "sample_name", VT_CHAR_ARRAY, DRF_UNKNOWN, dataHeadersOptions.pMemoryMap->endian));
            }

            listResult.append(dataHeader);
        }
    }

    return listResult;
}

XXM::HEADER XXM::_read_HEADER(qint64 nOffset)
{
    HEADER header = {};
    read_array(nOffset + offsetof(HEADER, id_text), header.id_text, 17);
    read_array(nOffset + offsetof(HEADER, module_name), header.module_name, 20);
    header.ctrl_byte_1a = read_uint8(nOffset + offsetof(HEADER, ctrl_byte_1a));
    read_array(nOffset + offsetof(HEADER, tracker_name), header.tracker_name, 20);
    header.version = read_uint16(nOffset + offsetof(HEADER, version));
    header.header_size = read_uint32(nOffset + offsetof(HEADER, header_size));
    header.song_length = read_uint16(nOffset + offsetof(HEADER, song_length));
    header.song_restart = read_uint16(nOffset + offsetof(HEADER, song_restart));
    header.channels = read_uint16(nOffset + offsetof(HEADER, channels));
    header.num_patterns = read_uint16(nOffset + offsetof(HEADER, num_patterns));
    header.num_instruments = read_uint16(nOffset + offsetof(HEADER, num_instruments));
    header.flags = read_uint16(nOffset + offsetof(HEADER, flags));
    header.default_tempo = read_uint16(nOffset + offsetof(HEADER, default_tempo));
    header.default_bpm = read_uint16(nOffset + offsetof(HEADER, default_bpm));
    read_array(nOffset + offsetof(HEADER, pattern_order), (char *)header.pattern_order, 256);

    return header;
}

XXM::PATTERN_HEADER XXM::_read_PATTERN_HEADER(qint64 nOffset)
{
    PATTERN_HEADER patternHeader = {};
    patternHeader.header_length = read_uint32(nOffset);
    patternHeader.packing_type = read_uint8(nOffset + 4);
    patternHeader.num_rows = read_uint16(nOffset + 5);
    patternHeader.packed_data_size = read_uint16(nOffset + 7);
    return patternHeader;
}

XXM::INSTRUMENT_HEADER XXM::_read_INSTRUMENT_HEADER(qint64 nOffset)
{
    INSTRUMENT_HEADER hdr = {};
    hdr.instrument_header_size = read_uint32(nOffset);
    QByteArray arrName = read_array(nOffset + 4, 22);
    memcpy(hdr.instrument_name, arrName.constData(), 22);
    hdr.instrument_type = read_uint8(nOffset + 26);
    hdr.num_samples = read_uint16(nOffset + 27);
    return hdr;
}

XXM::INSTRUMENT_EXTRA_HEADER XXM::_read_INSTRUMENT_EXTRA_HEADER(qint64 nOffset)
{
    INSTRUMENT_EXTRA_HEADER hdr = {};
    hdr.sample_header_size = read_uint32(nOffset);
    QByteArray samples = read_array(nOffset + 4, 96);
    memcpy(hdr.sample_number_for_notes, samples.constData(), 96);
    QByteArray volEnv = read_array(nOffset + 100, 48);
    memcpy(hdr.volume_envelope_points, volEnv.constData(), 48);
    QByteArray panEnv = read_array(nOffset + 148, 48);
    memcpy(hdr.panning_envelope_points, panEnv.constData(), 48);

    hdr.num_volume_points = read_uint8(nOffset + 196);
    hdr.num_panning_points = read_uint8(nOffset + 197);
    hdr.volume_sustain_point = read_uint8(nOffset + 198);
    hdr.volume_loop_start_point = read_uint8(nOffset + 199);
    hdr.volume_loop_end_point = read_uint8(nOffset + 200);
    hdr.panning_sustain_point = read_uint8(nOffset + 201);
    hdr.panning_loop_start_point = read_uint8(nOffset + 202);
    hdr.panning_loop_end_point = read_uint8(nOffset + 203);
    hdr.volume_type = read_uint8(nOffset + 204);
    hdr.panning_type = read_uint8(nOffset + 205);
    hdr.vibrato_type = read_uint8(nOffset + 206);
    hdr.vibrato_sweep = read_uint8(nOffset + 207);
    hdr.vibrato_depth = read_uint8(nOffset + 208);
    hdr.vibrato_rate = read_uint8(nOffset + 209);
    hdr.volume_fadeout = read_uint16(nOffset + 210);
    hdr.reserved = read_uint16(nOffset + 212);

    return hdr;
}

QList<QString> XXM::getSearchSignatures()
{
    QList<QString> listResult;

    listResult.append("'Extended Module: '");

    return listResult;
}

XBinary *XXM::createInstance(QIODevice *pDevice, bool bIsImage, XADDR nModuleAddress)
{
    Q_UNUSED(bIsImage)
    Q_UNUSED(nModuleAddress)

    return new XXM(pDevice);
}

XXM::SAMPLE_HEADER XXM::_read_SAMPLE_HEADER(qint64 nOffset)
{
    SAMPLE_HEADER hdr = {};
    hdr.sample_length = read_uint32(nOffset);
    hdr.loop_start = read_uint32(nOffset + 4);
    hdr.loop_length = read_uint32(nOffset + 8);
    hdr.volume = read_uint8(nOffset + 12);
    hdr.finetune = read_int8(nOffset + 13);
    hdr.type = read_uint8(nOffset + 14);
    hdr.panning = read_uint8(nOffset + 15);
    hdr.relative_note_number = read_int8(nOffset + 16);
    hdr.reserved = read_uint8(nOffset + 17);
    QByteArray arrName = read_array(nOffset + 18, 22);
    memcpy(hdr.sample_name, arrName.constData(), 22);

    return hdr;
}

