INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogshortcuts.h \
    $$PWD/xshortcuts.h \
    $$PWD/xshortcutsdialog.h \
    $$PWD/xshortcutstscrollarea.h \
    $$PWD/xshortcutsobject.h \
    $$PWD/xshortcutswidget.h

SOURCES += \
    $$PWD/dialogshortcuts.cpp \
    $$PWD/xshortcuts.cpp \
    $$PWD/xshortcutsdialog.cpp \
    $$PWD/xshortcutstscrollarea.cpp \
    $$PWD/xshortcutsobject.cpp \
    $$PWD/xshortcutswidget.cpp

FORMS += \
    $$PWD/dialogshortcuts.ui

!contains(XCONFIG, xoptions) {
    XCONFIG += xoptions
    include($$PWD/../XOptions/xoptions.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xshortcuts.cmake
