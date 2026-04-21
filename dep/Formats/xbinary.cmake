include_directories(${CMAKE_CURRENT_LIST_DIR})

set(XBINARY_SOURCES
    ${XBINARY_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/subdevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/subdevice.h
    ${CMAKE_CURRENT_LIST_DIR}/xbinary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xbinary.h
    ${CMAKE_CURRENT_LIST_DIR}/xbinary_def.h
    ${CMAKE_CURRENT_LIST_DIR}/xiodevice.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xiodevice.h
)
