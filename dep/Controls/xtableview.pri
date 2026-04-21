INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(XCONFIG, xmodel) {
    XCONFIG += xmodel
    include($$PWD/xmodel.pri)
}

HEADERS += \
    $$PWD/xheaderview.h \
    $$PWD/xsortfilterproxymodel.h \
    $$PWD/xtableview.h \
    $$PWD/xmodel_msrecord.h

SOURCES += \
    $$PWD/xheaderview.cpp \
    $$PWD/xsortfilterproxymodel.cpp \
    $$PWD/xtableview.cpp \
    $$PWD/xmodel_msrecord.cpp

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xtableview.cmake
