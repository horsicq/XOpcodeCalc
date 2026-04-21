INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xavi.h

SOURCES += \
    $$PWD/xavi.cpp

!contains(XCONFIG, xriff) {
    XCONFIG += xriff
    include($$PWD/../formats/xriff.pri)
}
