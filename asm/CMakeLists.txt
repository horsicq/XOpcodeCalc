cmake_minimum_required(VERSION 3.10)

if(MSVC)
    project(asm_lib LANGUAGES ASM_MASM)
else()
    project(asm_lib LANGUAGES ASM)
endif()
#project(asm_lib LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_library(asm_lib STATIC
            ${PROJECT_SOURCE_DIR}/msvc64.asm
        )
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_library(asm_lib STATIC
            ${PROJECT_SOURCE_DIR}/msvc32.asm
        )
    endif()
else()
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_library(asm_lib STATIC
            ${PROJECT_SOURCE_DIR}/gcc64.s
        )
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        add_library(asm_lib STATIC
            ${PROJECT_SOURCE_DIR}/gcc32.s
        )
    endif()
endif()

if(MSVC)
    set (CMAKE_PREFIX_PATH "C:/Qt/5.15.2/msvc2019/")
endif()

if(APPLE)
    set (CMAKE_PREFIX_PATH $ENV{HOME}/Qt/5.15.2/clang_64/)
    set (CMAKE_OSX_ARCHITECTURES x86_64)
    add_compile_options(-Wno-deprecated-declarations)
    add_compile_options(-Wno-switch)
endif()

set_target_properties(asm_lib PROPERTIES LINKER_LANGUAGE C)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
