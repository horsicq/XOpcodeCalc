include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XMODEL_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xmodel.cmake)
    set(XFMODEL_SOURCES ${XFMODEL_SOURCES} ${XMODEL_SOURCES})
endif()

set(XFMODEL_SOURCES
    ${XFMODEL_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel_header.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel_header.h
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel_table.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfmodel_table.h
    ${CMAKE_CURRENT_LIST_DIR}/xftree_model.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xftree_model.h
)
