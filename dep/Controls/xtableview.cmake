include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XMODEL_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/xmodel.cmake)
    set(XTABLEVIEW_SOURCES ${XTABLEVIEW_SOURCES} ${XMODEL_SOURCES})
endif()

set(XTABLEVIEW_SOURCES
    ${XTABLEVIEW_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xtableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xtableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xheaderview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xheaderview.h
    ${CMAKE_CURRENT_LIST_DIR}/xsortfilterproxymodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xsortfilterproxymodel.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel.h
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_msrecord.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xmodel_msrecord.h
    )
