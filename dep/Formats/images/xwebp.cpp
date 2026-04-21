#include "xwebp.h"

XBinary::XCONVERT _TABLE_XWEBP_STRUCTID[] = {
    {XWEBP::STRUCTID_UNKNOWN, "Unknown", QObject::tr("Unknown")},
};

XWEBP::XWEBP(QIODevice *pDevice) : XRiff(pDevice)
{
}
XWEBP::~XWEBP()
{
}

bool XWEBP::isValid(PDSTRUCT *pPdStruct)
{
    if (!XRiff::isValid(pPdStruct)) return false;
    // WebP uses RIFF container with fourcc "WEBP" at bytes 8..11 (chunk type)
    // RIFF header: 0..3 = 'RIFF'/'RIFX', 4..7 = size, 8..11 = form type
    QString form = read_ansiString(8, 4);
    return (form == "WEBP");
}

bool XWEBP::isValid(QIODevice *pDevice, PDSTRUCT *pPdStruct)
{
    XWEBP x(pDevice);
    return x.isValid();
}

QString XWEBP::getFileFormatExt()
{
    return "webp";
}
QString XWEBP::getFileFormatExtsString()
{
    return "WebP";
}
qint64 XWEBP::getFileFormatSize(PDSTRUCT *pPdStruct)
{
    return _calculateRawSize(pPdStruct);
}
XBinary::FT XWEBP::getFileType()
{
    return FT_WEBP;
}
QString XWEBP::getMIMEString()
{
    return "image/webp";
}

QString XWEBP::structIDToString(quint32 nID)
{
    return XBinary::XCONVERT_idToTransString(nID, _TABLE_XWEBP_STRUCTID, sizeof(_TABLE_XWEBP_STRUCTID) / sizeof(XBinary::XCONVERT));
}

