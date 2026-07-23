set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

# Test harness only; never installed or packaged.
