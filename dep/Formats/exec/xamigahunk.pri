INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xamigahunk.h \
    $$PWD/xamigahunk_def.h

SOURCES += \
    $$PWD/xamigahunk.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/xbinary.pri)
}
