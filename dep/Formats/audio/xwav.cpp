#include "xwav.h"

XWAV::XWAV(QIODevice *pDevice) : XRiff(pDevice)
{
}
XWAV::~XWAV()
{
}

bool XWAV::isValid(PDSTRUCT *pPdStruct)
{
    if (!XRiff::isValid(pPdStruct)) return false;
    // WAV uses RIFF container with form type "WAVE" at bytes 8..11
    QString form = read_ansiString(8, 4);
    return (form == "WAVE");
}

bool XWAV::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XWAV x(pDevice);
    return x.isValid();
}

QString XWAV::getFileFormatExt()
{
    return "wav";
}
QString XWAV::getFileFormatExtsString()
{
    return "WAV";
}
qint64 XWAV::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}
XBinary::FT XWAV::getFileType()
{
    return FT_WAV;
}
QString XWAV::getMIMEString()
{
    return "audio/x-wav";
}

