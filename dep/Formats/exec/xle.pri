INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xle.h \
    $$PWD/xle_def.h

SOURCES += \
    $$PWD/xle.cpp

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include($$PWD/xmsdos.pri)
}
