INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xpe_def.h \
    $$PWD/xpe.h

SOURCES += \
    $$PWD/xpe.cpp

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include($$PWD/xmsdos.pri)
}

win32-msvc* {
    # TODO Check MinGW
    LIBS += wintrust.lib    # for WinTrust
    LIBS += crypt32.lib     # for WinTrust
}
