cmake_minimum_required(VERSION 3.14)

project(xsimd LANGUAGES C)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include_directories(${PROJECT_SOURCE_DIR}/src/)

# xsimd_sse2 library - SSE2 specific implementations
add_library(xsimd_sse2 STATIC
    ${PROJECT_SOURCE_DIR}/src/xsimd_sse2.c
)
set_target_properties(xsimd_sse2 PROPERTIES LINKER_LANGUAGE C)

# xsimd_avx2 library - AVX2 specific implementations
add_library(xsimd_avx2 STATIC
    ${PROJECT_SOURCE_DIR}/src/xsimd_avx2.c
)
set_target_properties(xsimd_avx2 PROPERTIES LINKER_LANGUAGE C)

# xsimd main library - core functionality and dispatching
add_library(xsimd STATIC
    ${PROJECT_SOURCE_DIR}/src/xsimd.c
)
set_target_properties(xsimd PROPERTIES LINKER_LANGUAGE C)

# Link SIMD libraries to main library
target_link_libraries(xsimd PUBLIC xsimd_sse2 xsimd_avx2)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # Enable SSE2 for xsimd_sse2
    target_compile_options(xsimd_sse2 PRIVATE /arch:SSE2)
    # Enable AVX2 for xsimd_avx2
    target_compile_options(xsimd_avx2 PRIVATE /arch:AVX2)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    # Enable SSE2 for xsimd_sse2
    target_compile_options(xsimd_sse2 PRIVATE -msse -msse2)
    # Enable AVX2 for xsimd_avx2
    target_compile_options(xsimd_avx2 PRIVATE -msse -msse2 -mavx -mavx2)
endif()
