INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

include($$PWD/../XArchive/xarchive.pri)
include($$PWD/../XGithub/xgithub.pri)
include($$PWD/../XOptions/xoptions.pri)

HEADERS += \
    $$PWD/xupdate.h

SOURCES += \
    $$PWD/xupdate.cpp

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xupdate.cmake
