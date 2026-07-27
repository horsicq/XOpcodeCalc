string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" XOPCODECALC_SYSTEM_PROCESSOR)
string(TOLOWER "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}" XOPCODECALC_COMPILER_ARCH)
string(TOLOWER "${CMAKE_VS_PLATFORM_NAME}" XOPCODECALC_VS_PLATFORM)

set(XOPCODECALC_IS_X86 FALSE)
foreach(candidate IN ITEMS
    "${XOPCODECALC_SYSTEM_PROCESSOR}"
    "${XOPCODECALC_COMPILER_ARCH}"
    "${XOPCODECALC_VS_PLATFORM}")
    if(candidate MATCHES "^(x86_64|amd64|x64|i[3-6]86|x86|win32)$")
        set(XOPCODECALC_IS_X86 TRUE)
    endif()
endforeach()

set(XOPCODECALC_ASM_LIB_SOURCES
    asm.h
    asm_c.h
    asm_c.cpp
)

if(XOPCODECALC_IS_X86)
    if(MSVC)
        enable_language(ASM_MASM)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(XOPCODECALC_ASM_SOURCE msvc64.asm)
        else()
            set(XOPCODECALC_ASM_SOURCE msvc32.asm)
        endif()
    else()
        enable_language(ASM)
        if(CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(XOPCODECALC_ASM_SOURCE gcc64.s)
        else()
            set(XOPCODECALC_ASM_SOURCE gcc32.s)
        endif()
    endif()
    list(APPEND XOPCODECALC_ASM_LIB_SOURCES ${XOPCODECALC_ASM_SOURCE})
else()
    message(STATUS "XOpcodeCalc: non-x86 target, opcodes run on the portable C backend")
endif()

add_library(xopcodecalc_asm STATIC
    ${XOPCODECALC_ASM_LIB_SOURCES}
)

if(NOT XOPCODECALC_IS_X86)
    target_compile_definitions(xopcodecalc_asm PRIVATE XOPCODECALC_C_BACKEND)
endif()

target_include_directories(xopcodecalc_asm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(xopcodecalc_asm PRIVATE
    Qt${XOPCODECALC_QT_MAJOR}::Core
)

set_target_properties(xopcodecalc_asm PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
