INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xwebp.h

SOURCES += \
    $$PWD/xwebp.cpp

!contains(XCONFIG, xriff) {
    XCONFIG += xriff
    include($$PWD/../formats/xriff.pri)
}
