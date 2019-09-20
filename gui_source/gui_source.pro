QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xocalc
TEMPLATE = app


SOURCES +=\
    guimainwindow.cpp \
    main_gui.cpp \
    dialogabout.cpp \
    dialogoptions.cpp \
    modevalidator.cpp

HEADERS  += guimainwindow.h \
    dialogabout.h \
    dialogoptions.h \
    modevalidator.h \
    ../asm/asm.h \
    ../asm_defs.h \
    ../global.h \

FORMS    += guimainwindow.ui \
    dialogabout.ui \
    dialogoptions.ui

include(../build.pri)

win32 {
    RC_ICONS = ../icons/main.ico
}

macx {
    ICON = ../icons/main.icns
}

RESOURCES += \
    resources.qrc


preprocess.name = assembler
preprocess.input = ASM_FILES

win32-msvc*{
    preprocess.output = ${QMAKE_FILE_BASE}.obj
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("Win x86 build")
        ASM_FILES = ../asm/win32_msvc.asm
        preprocess.commands = ml /c /coff ${QMAKE_FILE_IN}
    } else {
        message("Win x86_64 build")
        ASM_FILES = ../asm/win64_msvc.asm
        preprocess.commands = ml64 /c /coff ${QMAKE_FILE_IN}
    }
}

linux-g++{
    BITSIZE = $$system(getconf LONG_BIT)
    !contains(BITSIZE, 64) {
        message("Linux x32 build")
        ASM_FILES = ../asm/unix32_gcc.s
    } else {
        message("Linux x64 build")
        ASM_FILES = ../asm/unix64_gcc.s
    }

    preprocess.output = ${QMAKE_FILE_BASE}.o
    preprocess.commands = gcc -c ${QMAKE_FILE_IN}
}

osx{
    message("OSX x64 build")
    ASM_FILES = ../asm/unix64_gcc.s
    preprocess.output = ${QMAKE_FILE_BASE}.o
    preprocess.commands = gcc -arch x86_64 -c ${QMAKE_FILE_IN}
}

QMAKE_EXTRA_COMPILERS += preprocess
