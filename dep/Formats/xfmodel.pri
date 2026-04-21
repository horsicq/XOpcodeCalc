INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

!contains(XCONFIG, xmodel) {
    XCONFIG += xmodel
    include($$PWD/../Controls/xmodel.pri)
}

HEADERS += \
    $$PWD/xfmodel.h \
    $$PWD/xfmodel_header.h \
    $$PWD/xfmodel_table.h \
    $$PWD/xftree_model.h

SOURCES += \
    $$PWD/xfmodel.cpp \
    $$PWD/xfmodel_header.cpp \
    $$PWD/xfmodel_table.cpp \
    $$PWD/xftree_model.cpp

DISTFILES += \
    $$PWD/xfmodel.cmake
