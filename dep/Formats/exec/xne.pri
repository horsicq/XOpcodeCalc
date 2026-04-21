INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xne.h \
    $$PWD/xne_def.h

SOURCES += \
    $$PWD/xne.cpp

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include($$PWD/xmsdos.pri)
}
