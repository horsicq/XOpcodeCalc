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

if(NOT XOPCODECALC_IS_X86)
    message(FATAL_ERROR
        "XOpcodeCalc can only be built for x86/x86_64 targets because it links x86 assembly.")
endif()

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

add_library(xopcodecalc_asm STATIC
    asm.h
    ${XOPCODECALC_ASM_SOURCE}
)

target_include_directories(xopcodecalc_asm PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_target_properties(xopcodecalc_asm PROPERTIES
    AUTOMOC OFF
    AUTOUIC OFF
    AUTORCC OFF
)
