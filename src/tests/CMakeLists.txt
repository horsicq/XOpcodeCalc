set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    add_definitions(-DNOMINMAX)
endif()

# 1. Opcode backend harness: the assembly backends vs. the portable C reference.
add_executable(xocalc_test
    main_test.cpp
)

target_link_libraries(xocalc_test PRIVATE
    Qt${XOPCODECALC_QT_MAJOR}::Core
    xopcodecalc_asm
)

set_target_properties(xocalc_test PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)

# 2. Opcode-table smoke test: the invariants the GUI relies on.
add_executable(xocalc_table_test
    table_test.cpp
)

target_link_libraries(xocalc_table_test PRIVATE
    Qt${XOPCODECALC_QT_MAJOR}::Core
    xopcodecalc_asm
)

set_target_properties(xocalc_table_test PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)

# 3. GUI smoke test: the real GuiMainWindow, driven headless on the offscreen
#    platform. It links the xopcodecalc_gui object library, i.e. the exact
#    translation units the shipped application is built from.
if(TARGET xopcodecalc_gui)
    add_executable(xocalc_gui_test
        gui_test.cpp
    )

    target_link_libraries(xocalc_gui_test PRIVATE
        xopcodecalc_gui
    )

    set_target_properties(xocalc_gui_test PROPERTIES
        AUTOMOC OFF
        AUTOUIC OFF
        AUTORCC OFF
    )
else()
    message(STATUS "XOpcodeCalc: GUI smoke test skipped, the xopcodecalc_gui object library is missing")
endif()

# Test harnesses only; never installed or packaged.
