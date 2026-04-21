include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XOPTIONS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XOptions/xoptions.cmake)
    set(XSHORTCUTS_SOURCES ${XSHORTCUTS_SOURCES} ${XOPTIONS_SOURCES})
endif()

set(XSHORTCUTS_SOURCES
    ${XSHORTCUTS_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogshortcuts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogshortcuts.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogshortcuts.ui
    ${CMAKE_CURRENT_LIST_DIR}/xshortcuts.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xshortcuts.h
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutsdialog.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutsdialog.h
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutstscrollarea.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutstscrollarea.h
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutswidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutswidget.h
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutsobject.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xshortcutsobject.h
    )
