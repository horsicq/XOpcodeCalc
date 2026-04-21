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
#include "xdataconvertor.h"

XDataConvertor::XDataConvertor(QObject *pParent) : XThreadObject(pParent)
{
    m_options = {};
}

void XDataConvertor::setData(QIODevice *pDeviceIn, DATA *pData, CMETHOD method, const OPTIONS &options, XBinary::PDSTRUCT *pPdStruct)
{
    m_pDeviceIn = pDeviceIn;
    m_pData = pData;
    m_method = method;
    m_options = options;
    m_pPdStruct = pPdStruct;
}

void XDataConvertor::process()
{
    // mb TODO set selected processed

    XBinary::PDSTRUCT pdStructEmpty = XBinary::createPdStruct();
    XBinary::PDSTRUCT *pPdStruct = m_pPdStruct;

    if (!pPdStruct) {
        pPdStruct = &pdStructEmpty;
    }

    qint32 _nFreeIndex = XBinary::getFreeIndex(pPdStruct);
    XBinary::setPdStructInit(pPdStruct, _nFreeIndex, 0);

    if (m_method == CMETHOD_NONE) {
        m_pData->pTmpFile = nullptr;
        m_pData->dEntropy = XBinary::getEntropy(m_pDeviceIn, pPdStruct);
    } else {
        m_pData->pTmpFile = new QTemporaryFile;

        if (m_pData->pTmpFile->open()) {
            qint64 nOutSize = 0;
            qint32 nBufferSize = XBinary::getBufferSize(pPdStruct);
            qint64 nInSize = m_pDeviceIn->size();
            quint64 nKey = 0;

            // mb TODO adjust
            if ((m_method == CMETHOD_XOR_BYTE) || (m_method == CMETHOD_ADD_BYTE) || (m_method == CMETHOD_SUB_BYTE)) {
                nOutSize = nInSize;
                nKey = (quint8)m_options.varKey.toULongLong();
            } else if ((m_method == CMETHOD_XOR_WORD) || (m_method == CMETHOD_ADD_WORD) || (m_method == CMETHOD_SUB_WORD)) {
                nOutSize = S_ALIGN_DOWN64(nInSize, 2);
                nKey = (quint16)m_options.varKey.toULongLong();
            } else if ((m_method == CMETHOD_XOR_DWORD) || (m_method == CMETHOD_ADD_DWORD) || (m_method == CMETHOD_SUB_DWORD)) {
                nOutSize = S_ALIGN_DOWN64(nInSize, 4);
                nKey = (quint32)m_options.varKey.toULongLong();
            } else if ((m_method == CMETHOD_XOR_QWORD) || (m_method == CMETHOD_ADD_QWORD) || (m_method == CMETHOD_SUB_QWORD)) {
                nOutSize = S_ALIGN_DOWN64(nInSize, 8);
                nKey = (quint64)m_options.varKey.toULongLong();
            } else if (m_method == CMETHOD_BASE64_ENCODE) {
                nOutSize = (nInSize / 3) * 4;
                if (nInSize % 3) {
                    nOutSize += 4;
                }
            } else if (m_method == CMETHOD_BASE64_DECODE) {
                nOutSize = (nInSize / 4) * 3;
            }

            char *pBuffer = new char[nBufferSize];

            if (XBinary::resize(m_pData->pTmpFile, nOutSize)) {
                for (qint32 i = 0; i < nInSize;) {
                    qint64 _nBufferSize = qMin((qint64)nBufferSize, nInSize - i);
                    qint64 nProcessedSize = 0;
                    QByteArray baOut;

                    bool bInvalidSize = false;

                    if ((m_method == CMETHOD_XOR_WORD) || (m_method == CMETHOD_ADD_WORD) || (m_method == CMETHOD_SUB_WORD)) {
                        if (_nBufferSize % 2) {
                            bInvalidSize = true;
                        }
                    } else if ((m_method == CMETHOD_XOR_DWORD) || (m_method == CMETHOD_ADD_DWORD) || (m_method == CMETHOD_SUB_DWORD)) {
                        if (_nBufferSize % 4) {
                            bInvalidSize = true;
                        }
                    } else if ((m_method == CMETHOD_XOR_QWORD) || (m_method == CMETHOD_ADD_QWORD) || (m_method == CMETHOD_SUB_QWORD)) {
                        if (_nBufferSize % 8) {
                            bInvalidSize = true;
                        }
                    } else if (m_method == CMETHOD_BASE64_DECODE) {
                        if (_nBufferSize % 4) {
                            bInvalidSize = true;
                        }
                    }

                    m_pDeviceIn->seek(i);
                    m_pData->pTmpFile->seek(i);

                    if ((m_pDeviceIn->read(pBuffer, _nBufferSize) != _nBufferSize) || (_nBufferSize == 0)) {
                        XBinary::setPdStructInfoString(pPdStruct, ("Read error"));
                        break;
                    }

                    if (m_method == CMETHOD_XOR_BYTE) {
                        for (qint32 j = 0; j < _nBufferSize; j++) {
                            pBuffer[j] = pBuffer[j] ^ (quint8)nKey;
                            nProcessedSize++;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_XOR_WORD) {
                        for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                            ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] ^ (quint16)nKey;
                            nProcessedSize += 2;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_XOR_DWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                            ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] ^ (quint32)nKey;
                            nProcessedSize += 4;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_XOR_QWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                            ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] ^ (quint64)nKey;
                            nProcessedSize += 8;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_ADD_BYTE) {
                        for (qint32 j = 0; j < _nBufferSize; j++) {
                            pBuffer[j] = pBuffer[j] + (quint8)nKey;
                            nProcessedSize++;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_ADD_WORD) {
                        for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                            ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] + (quint16)nKey;
                            nProcessedSize += 2;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_ADD_DWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                            ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] + (quint32)nKey;
                            nProcessedSize += 4;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_ADD_QWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                            ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] + (quint64)nKey;
                            nProcessedSize += 8;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_SUB_BYTE) {
                        for (qint32 j = 0; j < _nBufferSize; j++) {
                            pBuffer[j] = pBuffer[j] - (quint8)nKey;
                            nProcessedSize++;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_SUB_WORD) {
                        for (qint32 j = 0; j < _nBufferSize / 2; j++) {
                            ((quint16 *)pBuffer)[j] = ((quint16 *)pBuffer)[j] - (quint16)nKey;
                            nProcessedSize += 2;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_SUB_DWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 4; j++) {
                            ((quint32 *)pBuffer)[j] = ((quint32 *)pBuffer)[j] - (quint32)nKey;
                            nProcessedSize += 4;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_SUB_QWORD) {
                        for (qint32 j = 0; j < _nBufferSize / 8; j++) {
                            ((quint64 *)pBuffer)[j] = ((quint64 *)pBuffer)[j] - (quint64)nKey;
                            nProcessedSize += 8;
                        }
                        baOut.append(pBuffer, nProcessedSize);
                    } else if (m_method == CMETHOD_BASE64_ENCODE) {
                        QByteArray baArray(pBuffer, _nBufferSize);
                        baOut = baArray.toBase64();
                        nProcessedSize = baOut.size();
                    } else if (m_method == CMETHOD_BASE64_DECODE) {
                        QByteArray baArray(pBuffer, _nBufferSize);
                        baOut = QByteArray::fromBase64(baArray);
                        // baOut = QByteArray::fromBase64(baArray, QByteArray::AbortOnBase64DecodingErrors);
                        nProcessedSize = baOut.size();
                    }

                    if (m_pData->pTmpFile->write(baOut.data(), nProcessedSize) != nProcessedSize) {
                        XBinary::setPdStructInfoString(pPdStruct, tr("Write error"));
                        break;
                    }

                    if (bInvalidSize) {
                        XBinary::setPdStructInfoString(pPdStruct, tr("Invalid size"));
                        break;
                    }

                    i += _nBufferSize;
                }
            } else {
                XBinary::setPdStructInfoString(pPdStruct, tr("Cannot resize"));
            }

            delete[] pBuffer;
        }

        m_pData->dEntropy = XBinary::getEntropy(m_pData->pTmpFile, pPdStruct);
    }

    m_pData->bValid = (!pPdStruct->bIsStop);

    XBinary::setPdStructFinished(pPdStruct, _nFreeIndex);
}
