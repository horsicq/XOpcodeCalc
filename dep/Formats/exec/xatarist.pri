INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xatarist.h \
    $$PWD/xatarist_def.h

SOURCES += \
    $$PWD/xatarist.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/xbinary.pri)
}
