include_directories(${CMAKE_CURRENT_LIST_DIR})

set(XMODEL_BINARY_SOURCES
    ${XMODEL_BINARY_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_archiverecords.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_archiverecords.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_binary.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_binary.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_fparts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_fparts.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_hex.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_hex.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_streams.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_streams.h
)
