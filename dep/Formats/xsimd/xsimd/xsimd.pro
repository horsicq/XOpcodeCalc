QT -= core gui

TARGET = xsimd
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

include(../../../build.pri)

win32{
    TARGET = xsimd-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = xsimd-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = xsimd-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/../src/xsimd.c

HEADERS += \
    $$PWD/../src/xsimd.h \
    $$PWD/../src/xsimd_sse2.h \
    $$PWD/../src/xsimd_avx2.h
	
TARGETLIB_PATH = $$PWD/..

DESTDIR=$${TARGETLIB_PATH}/libs

# Link SIMD libraries
win32-g++ {
    LIBS += $$PWD/../libs/libxsimd_sse2-win-$${QT_ARCH}.a
    LIBS += $$PWD/../libs/libxsimd_avx2-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/../libs/xsimd_sse2-win-$${QT_ARCH}.lib
    LIBS += $$PWD/../libs/xsimd_avx2-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/../libs/libxsimd_sse2-unix-$${QT_ARCH}.a
    LIBS += $$PWD/../libs/libxsimd_avx2-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/../libs/libxsimd_sse2-macos-$${QT_ARCH}.a
    LIBS += $$PWD/../libs/libxsimd_avx2-macos-$${QT_ARCH}.a
}
