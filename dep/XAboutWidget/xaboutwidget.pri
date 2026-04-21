INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/xaboutwidget.cpp

HEADERS += \
    $$PWD/xaboutwidget.h

FORMS += \
    $$PWD/xaboutwidget.ui

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xaboutwidget.cmake
