INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogviewcolors.h \
    $$PWD/xoptionswidget.h

SOURCES += \
    $$PWD/dialogviewcolors.cpp \
    $$PWD/xoptionswidget.cpp

FORMS += \
    $$PWD/dialogviewcolors.ui \
    $$PWD/xoptionswidget.ui

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include($$PWD/xoptions.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xoptionswidget.cmake
