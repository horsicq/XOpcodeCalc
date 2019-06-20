QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xopcodecalc
TEMPLATE = app


SOURCES +=\
        guimainwindow.cpp \
    main_gui.cpp \
    dialogabout.cpp \
    dialogoptions.cpp

HEADERS  += guimainwindow.h \
    dialogabout.h \
    dialogoptions.h \
    ../asm_defs.h

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
        ASM_FILES = ../asm/msvc32.asm
        preprocess.commands = ml /c /coff ${QMAKE_FILE_IN}
    } else {
        message("Win x86_64 build")
        ASM_FILES = ../asm/msvc64.asm
        preprocess.commands = ml64 /c /coff ${QMAKE_FILE_IN}
    }
}

message($(QMAKE_TARGET.arch))

linux-g++{
    BITSIZE = $$system(getconf LONG_BIT)
    !contains(BITSIZE, 64) {
        message("Linux 32 build")
        ASM_FILES = ../asm/gcc32.s
    } else {
        message("Linux 64 build")
        ASM_FILES = ../asm/gcc64.s
    }

    preprocess.output = ${QMAKE_FILE_BASE}.o
    preprocess.commands = gcc -c ${QMAKE_FILE_IN}
}

QMAKE_EXTRA_COMPILERS += preprocess
