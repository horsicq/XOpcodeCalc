INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(XCONFIG, xfmodel) {
    XCONFIG += xfmodel
    include($$PWD/../../Formats/xfmodel.pri)
}

!contains(XCONFIG, xtableview) {
    XCONFIG += xtableview
    include($$PWD/../xtableview.pri)
}

HEADERS += \
    $$PWD/xftableview.h \
    $$PWD/xftreeview.h \
    $$PWD/xfstringdialog.h \
    $$PWD/xfvaluedialog.h \
    $$PWD/xfsizedialog.h \
    $$PWD/xfflagsdialog.h \
    $$PWD/xflistdialog.h

SOURCES += \
    $$PWD/xftableview.cpp \
    $$PWD/xftreeview.cpp \
    $$PWD/xfstringdialog.cpp \
    $$PWD/xfvaluedialog.cpp \
    $$PWD/xfsizedialog.cpp \
    $$PWD/xfflagsdialog.cpp \
    $$PWD/xflistdialog.cpp

DISTFILES += \
    $$PWD/xfcontrols.cmake
