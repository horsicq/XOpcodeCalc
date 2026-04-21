INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

# Enable AVX2 for optimized binary operations on x86/x64 architectures
contains(QT_ARCH, x86_64)|contains(QT_ARCH, i386) {
    linux|macx|unix {
        #QMAKE_CXXFLAGS += -mavx2
        #DEFINES += XBINARY_USE_AVX2
    }
    win32-msvc* {
        #QMAKE_CXXFLAGS += /arch:AVX2
        #DEFINES += XBINARY_USE_AVX2
    }
    win32-g++ {
        #QMAKE_CXXFLAGS += -mavx2
        #DEFINES += XBINARY_USE_AVX2
    }
}

HEADERS += \
    $$PWD/xformats.h

SOURCES += \
    $$PWD/xformats.cpp

!contains(XCONFIG, xbinary) {
    XCONFIG += xbinary
    include($$PWD/xbinary.pri)
}

!contains(XCONFIG, xcom) {
    XCONFIG += xcom
    include($$PWD/exec/xcom.pri)
}

!contains(XCONFIG, xmsdos) {
    XCONFIG += xmsdos
    include($$PWD/exec/xmsdos.pri)
}
!contains(XCONFIG, xavi) {
    XCONFIG += xavi
    include($$PWD/video/xavi.pri)
}

!contains(XCONFIG, xne) {
    XCONFIG += xne
    include($$PWD/exec/xne.pri)
}

!contains(XCONFIG, xle) {
    XCONFIG += xle
!contains(XCONFIG, xwebp) {
    XCONFIG += xwebp
    include($$PWD/images/xwebp.pri)
}
    include($$PWD/exec/xle.pri)
}

!contains(XCONFIG, xpe) {
    XCONFIG += xpe
    include($$PWD/exec/xpe.pri)
}

!contains(XCONFIG, xmach) {
    XCONFIG += xmach
    include($$PWD/exec/xmach.pri)
}

!contains(XCONFIG, xelf) {
    XCONFIG += xelf
    include($$PWD/exec/xelf.pri)
}

!contains(XCONFIG, xamigahunk) {
    XCONFIG += xamigahunk
    include($$PWD/exec/xamigahunk.pri)
}

!contains(XCONFIG, xatarist) {
    XCONFIG += xatarist
    include($$PWD/exec/xatarist.pri)
}

contains(XCONFIG, use_dex) {
    DEFINES += USE_DEX
    !contains(XCONFIG, xdex) {
        XCONFIG += xdex
        include($$PWD/../XDEX/xdex.pri)
    }
}

contains(XCONFIG, use_pdf) {
    DEFINES += USE_PDF
    !contains(XCONFIG, xpdf) {
        XCONFIG += xpdf
        include($$PWD/../XPDF/xpdf.pri)
    }
}

contains(XCONFIG, use_archive) {
    DEFINES += USE_ARCHIVE
    !contains(XCONFIG, xarchives) {
        XCONFIG += xarchives
        include($$PWD/../XArchive/xarchives.pri)
    }
}

!contains(XCONFIG, xicon) {
    XCONFIG += xicon
    include($$PWD/images/xicon.pri)
}

!contains(XCONFIG, xgif) {
    XCONFIG += xgif
    include($$PWD/images/xgif.pri)
}

!contains(XCONFIG, xicc) {
    XCONFIG += xicc
    include($$PWD/images/xicc.pri)
}

!contains(XCONFIG, xbmp) {
    XCONFIG += xbmp
    include($$PWD/images/xbmp.pri)
}

!contains(XCONFIG, xpng) {
    XCONFIG += xpng
    include($$PWD/images/xpng.pri)
}

!contains(XCONFIG, xjpeg) {
    XCONFIG += xjpeg
    include($$PWD/images/xjpeg.pri)
}

!contains(XCONFIG, xtiff) {
    XCONFIG += xtiff
    include($$PWD/images/xtiff.pri)
}

!contains(XCONFIG, xmp4) {
    XCONFIG += xmp4
    include($$PWD/video/xmp4.pri)
}

!contains(XCONFIG, xriff) {
    XCONFIG += xriff
    include($$PWD/formats/xriff.pri)
}

!contains(XCONFIG, xder) {
    XCONFIG += xder
    include($$PWD/formats/xder.pri)
}

!contains(XCONFIG, xmp3) {
    XCONFIG += xmp3
    include($$PWD/audio/xmp3.pri)
}

!contains(XCONFIG, xwav) {
    XCONFIG += xwav
    include($$PWD/audio/xwav.pri)
}

!contains(XCONFIG, xxm) {
    XCONFIG += xxm
    include($$PWD/audio/xxm.pri)
}

!contains(XCONFIG, xjavaclass) {
    XCONFIG += xjavaclass
    include($$PWD/formats/xjavaclass.pri)
}

!contains(XCONFIG, xpyc) {
    XCONFIG += xpyc
    include($$PWD/formats/xpyc.pri)
}

!contains(XCONFIG, xttf) {
    XCONFIG += xttf
    include($$PWD/formats/xttf.pri)
}

!contains(XCONFIG, xdjvu) {
    XCONFIG += xdjvu
    include($$PWD/formats/xdjvu.pri)
}

!contains(XCONFIG, xtext) {
    XCONFIG += xtext
    include($$PWD/texts/xtext.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xformats.cmake
