QT -= core gui

TARGET = xsimd_avx2
TEMPLATE = lib
CONFIG += staticlib

INCLUDEPATH += $$PWD/../src
DEPENDPATH += $$PWD/../src

include(../../../build.pri)

# Enable AVX2 instructions
win32-msvc* {
    QMAKE_CFLAGS += /arch:AVX2
    QMAKE_CXXFLAGS += /arch:AVX2
}
else:gcc|clang {
    contains(QT_ARCH, x86_64)|contains(QT_ARCH, i386) {
        QMAKE_CFLAGS += -msse -msse2 -mavx -mavx2
        QMAKE_CXXFLAGS += -msse -msse2 -mavx -mavx2
    }
}

win32{
    TARGET = xsimd_avx2-win-$${QT_ARCH}
}
unix:!macx {
    TARGET = xsimd_avx2-unix-$${QT_ARCH}
}
unix:macx {
    TARGET = xsimd_avx2-macos-$${QT_ARCH}
}

SOURCES += \
    $$PWD/../src/xsimd_avx2.c

HEADERS += \
    $$PWD/../src/xsimd_avx2.h \
    $$PWD/../src/xsimd.h
	
TARGETLIB_PATH = $$PWD/..

DESTDIR=$${TARGETLIB_PATH}/libs
