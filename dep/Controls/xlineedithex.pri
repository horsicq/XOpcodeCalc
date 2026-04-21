INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogbits8.h \
    $$PWD/xlineedithex.h \
    $$PWD/xlineedit_num.h \
    $$PWD/xlineedit_num_validator.h \
    $$PWD/xlineeditvalidator.h

SOURCES += \
    $$PWD/dialogbits8.cpp \
    $$PWD/xlineedithex.cpp \
    $$PWD/xlineedit_num.cpp \
    $$PWD/xlineedit_num_validator.cpp \
    $$PWD/xlineeditvalidator.cpp

FORMS += \
    $$PWD/dialogbits8.ui

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xlineedithex.cmake
