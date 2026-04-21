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
#ifndef XIODEVICE_H
#define XIODEVICE_H

#include <QIODevice>

class XIODevice : public QIODevice {
    Q_OBJECT

public:
    XIODevice(QObject *pParent);

    void setSize(qint64 nSize);
    void setInitLocation(quint64 nLocation);
    quint64 getInitLocation();

    static quint64 getInitLocation(QIODevice *pDevice);

    virtual qint64 size() const;
    virtual bool isSequential() const;
    virtual bool seek(qint64 nPos);
    virtual bool reset();
    virtual bool open(OpenMode mode);
    virtual bool atEnd() const;
    virtual void close();
    virtual qint64 pos() const;

protected:
    virtual qint64 readData(char *pData, qint64 nMaxSize);
    virtual qint64 writeData(const char *pData, qint64 nMaxSize);
    virtual void setErrorString(const QString &sString);

signals:
    void readDataSignal(quint64 nOffset, char *pData, qint64 nSize);
    void writeDataSignal(quint64 nOffset, char *pData, qint64 nSize);

private:
    qint64 m_nSize;
    quint64 m_nInitLocation;
};

#endif  // XIODEVICE_H
