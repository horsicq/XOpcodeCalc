QT -= core gui

TARGET = xsimd_sse2
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

include(../../../build.pri)

# Enable SSE2 instructions only
win32-msvc* {
    QMAKE_CFLAGS += /arch:SSE2
    QMAKE_CXXFLAGS += /arch:SSE2
}
else:gcc|clang {
    contains(QT_ARCH, x86_64)|contains(QT_ARCH, i386) {
        QMAKE_CFLAGS += -msse -msse2
        QMAKE_CXXFLAGS += -msse -msse2
    }
}

win32{
    TARGET = xsimd_sse2-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = xsimd_sse2-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = xsimd_sse2-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/../src/xsimd_sse2.c

HEADERS += \
    $$PWD/../src/xsimd_sse2.h \
    $$PWD/../src/xsimd.h
	
TARGETLIB_PATH = $$PWD/..

DESTDIR=$${TARGETLIB_PATH}/libs
