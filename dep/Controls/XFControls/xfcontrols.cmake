include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XFMODEL_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../../Formats/xfmodel.cmake)
    set(XFCONTROLS_SOURCES ${XFCONTROLS_SOURCES} ${XFMODEL_SOURCES})
endif()

if (NOT DEFINED XTABLEVIEW_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../xtableview.cmake)
    set(XFCONTROLS_SOURCES ${XFCONTROLS_SOURCES} ${XTABLEVIEW_SOURCES})
endif()

set(XFCONTROLS_SOURCES
    ${XFCONTROLS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xftableview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xftableview.h
    ${CMAKE_CURRENT_LIST_DIR}/xftreeview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xftreeview.h
    ${CMAKE_CURRENT_LIST_DIR}/xfstringdialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfstringdialog.h
    ${CMAKE_CURRENT_LIST_DIR}/xfvaluedialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfvaluedialog.h
    ${CMAKE_CURRENT_LIST_DIR}/xfsizedialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfsizedialog.h
    ${CMAKE_CURRENT_LIST_DIR}/xfflagsdialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xfflagsdialog.h
    ${CMAKE_CURRENT_LIST_DIR}/xflistdialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xflistdialog.h
)
