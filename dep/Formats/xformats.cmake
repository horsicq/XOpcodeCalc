# Enable AVX2 for optimized binary operations on x86/x64 architectures
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|i686|i386")
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2")
        #add_definitions(-DXBINARY_USE_AVX2)
    elseif(MSVC)
        #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
        #add_definitions(-DXBINARY_USE_AVX2)
    endif()
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_CURRENT_LIST_DIR}/images)
include_directories(${CMAKE_CURRENT_LIST_DIR}/video)
include_directories(${CMAKE_CURRENT_LIST_DIR}/audio)
include_directories(${CMAKE_CURRENT_LIST_DIR}/exec)
include_directories(${CMAKE_CURRENT_LIST_DIR}/texts)
include_directories(${CMAKE_CURRENT_LIST_DIR}/formats)
include_directories(${CMAKE_CURRENT_LIST_DIR}/xsimd/src)

if (NOT DEFINED XDEX_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XDEX/xdex.cmake)
    set(XFORMATS_SOURCES ${XFORMATS_SOURCES} ${XDEX_SOURCES})
endif()
if (NOT DEFINED XPDF_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XPDF/xpdf.cmake)
    set(XFORMATS_SOURCES ${XFORMATS_SOURCES} ${XPDF_SOURCES})
endif()
if (NOT DEFINED XARCHIVES_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XArchive/xarchives.cmake)
    set(XFORMATS_SOURCES ${XFORMATS_SOURCES} ${XARCHIVES_SOURCES})
endif()

# TODO

set(XFORMATS_SOURCES
    ${XFORMATS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xbinary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xbinary.h
    ${CMAKE_CURRENT_LIST_DIR}/xbinary_def.h
    ${CMAKE_CURRENT_LIST_DIR}/subdevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/subdevice.h
    ${CMAKE_CURRENT_LIST_DIR}/xformats.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xformats.h
    ${CMAKE_CURRENT_LIST_DIR}/audio/xmp3.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio/xmp3.h
    ${CMAKE_CURRENT_LIST_DIR}/audio/xwav.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio/xwav.h
    ${CMAKE_CURRENT_LIST_DIR}/audio/xxm.cpp
    ${CMAKE_CURRENT_LIST_DIR}/audio/xxm.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xbmp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xbmp.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xgif.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xgif.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xicc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xicc.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xicon.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xicon.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xjpeg.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xjpeg.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xpng.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xpng.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xtiff.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xtiff.h
    ${CMAKE_CURRENT_LIST_DIR}/images/xwebp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/images/xwebp.h
    ${CMAKE_CURRENT_LIST_DIR}/video/xmp4.cpp
    ${CMAKE_CURRENT_LIST_DIR}/video/xmp4.h
    ${CMAKE_CURRENT_LIST_DIR}/video/xavi.cpp
    ${CMAKE_CURRENT_LIST_DIR}/video/xavi.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xriff.cpp
    ${CMAKE_CURRENT_LIST_DIR}/formats/xriff.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xder.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/formats/xpyc.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xpyc.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xamigahunk.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xamigahunk.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xamigahunk_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xatarist.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xatarist.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xatarist_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xcom.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xcom.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xcom_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xelf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xelf.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xelf_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xle.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xle.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xle_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmach.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmach.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmach_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmsdos.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmsdos.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xmsdos_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xne.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xne.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xne_def.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xpe.cpp
    ${CMAKE_CURRENT_LIST_DIR}/exec/xpe.h
    ${CMAKE_CURRENT_LIST_DIR}/exec/xpe_def.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xjavaclass.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xjavaclass.cpp
    ${CMAKE_CURRENT_LIST_DIR}/formats/xttf.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xttf.cpp
    ${CMAKE_CURRENT_LIST_DIR}/formats/xdjvu.h
    ${CMAKE_CURRENT_LIST_DIR}/formats/xdjvu.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texts/xtext.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texts/xtext.h
)
