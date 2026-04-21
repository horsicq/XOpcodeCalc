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
#include "subdevice.h"

SubDevice::SubDevice(QIODevice *pDevice, qint64 nOffset, qint64 nSize, QObject *pParent) : XIODevice(pParent)
{
    if (nOffset > pDevice->size()) {
        nOffset = pDevice->size();
    }

    if (nOffset < 0) {
        nOffset = 0;
    }

    if ((nSize + nOffset > pDevice->size()) || (nSize == -1))  // TODO Check
    {
        nSize = pDevice->size() - nOffset;
    }

    if (nSize + nOffset < 0) {
        nSize = 0;
    }

    this->m_pDevice = pDevice;

    setInitLocation(nOffset);
    setSize(nSize);

    //    reset();
    pDevice->seek(nOffset);

    QVariant varBackupDevice = (quint64)pDevice;

    setProperty("BACKUPDEVICE", varBackupDevice);
    const char *pMemory = (const char *)(pDevice->property("Memory").toULongLong());

    if (pMemory) {
        setProperty("Memory", (quint64)pMemory + nOffset);
    }
}

SubDevice::~SubDevice()
{
    if (isOpen()) {
        setOpenMode(NotOpen);
    }
}

QIODevice *SubDevice::getOrigDevice()
{
    return m_pDevice;
}

bool SubDevice::seek(qint64 nPos)
{
    bool bResult = false;

    if ((nPos < size()) && (nPos >= 0)) {
        if (m_pDevice->seek(getInitLocation() + nPos)) {
            bResult = QIODevice::seek(nPos);
        }
    }

    return bResult;
}

bool SubDevice::reset()
{
    return seek(0);
}

qint64 SubDevice::readData(char *pData, qint64 nMaxSize)
{
    nMaxSize = qMin(nMaxSize, size() - pos());

    qint64 nLen = m_pDevice->read(pData, nMaxSize);

    return nLen;
}

qint64 SubDevice::writeData(const char *pData, qint64 nMaxSize)
{
    nMaxSize = qMin(nMaxSize, size() - pos());

    qint64 nLen = m_pDevice->write(pData, nMaxSize);

    return nLen;
}
