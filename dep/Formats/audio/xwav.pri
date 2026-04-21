INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xwav.h

SOURCES += \
    $$PWD/xwav.cpp

!contains(XCONFIG, xriff) {
    XCONFIG += xriff
    include($$PWD/../formats/xriff.pri)
}
