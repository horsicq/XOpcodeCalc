INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/xabstracttableview.h \
    $$PWD/xdevicetableeditview.h \
    $$PWD/xdevicetableview.h

SOURCES += \
    $$PWD/xabstracttableview.cpp \
    $$PWD/xdevicetableeditview.cpp \
    $$PWD/xdevicetableview.cpp

!contains(XCONFIG, dialogtextinfo) {
    XCONFIG += dialogtextinfo
    include($$PWD/../FormatDialogs/dialogtextinfo.pri)
}

!contains(XCONFIG, dialogresize) {
    XCONFIG += dialogresize
    include($$PWD/../FormatDialogs/dialogresize.pri)
}

!contains(XCONFIG, dialogremove) {
    XCONFIG += dialogremove
    include($$PWD/../FormatDialogs/dialogremove.pri)
}

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

!contains(XCONFIG, xinfodb) {
    XCONFIG += xinfodb
    include($$PWD/../XInfoDB/xinfodb.pri)
}

!contains(XCONFIG, xbinaryview) {
    XCONFIG += xbinaryview
    include($$PWD/xbinaryview.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xabstracttableview.cmake
