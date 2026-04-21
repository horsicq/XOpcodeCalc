INCLUDEPATH += $$PWD/src
DEPENDPATH += $$PWD/src

# Link all SIMD libraries
win32-g++ {
    LIBS += $$PWD/libs/libxsimd-win-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_sse2-win-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_avx2-win-$${QT_ARCH}.a
}
win32-msvc* {
    LIBS += $$PWD/libs/xsimd-win-$${QT_ARCH}.lib
    LIBS += $$PWD/libs/xsimd_sse2-win-$${QT_ARCH}.lib
    LIBS += $$PWD/libs/xsimd_avx2-win-$${QT_ARCH}.lib
}
unix:!macx {
    LIBS += $$PWD/libs/libxsimd-unix-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_sse2-unix-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_avx2-unix-$${QT_ARCH}.a
}
unix:macx {
    LIBS += $$PWD/libs/libxsimd-macos-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_sse2-macos-$${QT_ARCH}.a
    LIBS += $$PWD/libs/libxsimd_avx2-macos-$${QT_ARCH}.a
}

