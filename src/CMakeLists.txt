if(TARGET Qt6::Core OR TARGET Qt6::Widgets)
    set(XOPCODECALC_QT_MAJOR 6)
elseif(TARGET Qt5::Core OR TARGET Qt5::Widgets)
    set(XOPCODECALC_QT_MAJOR 5)
else()
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    set(XOPCODECALC_QT_MAJOR ${QT_VERSION_MAJOR})
endif()

if(XOPCODECALC_QT_MAJOR EQUAL 5)
    if(NOT TARGET Qt5::Widgets)
        if(DEFINED Qt5Core_VERSION_STRING AND Qt5Core_VERSION_STRING VERSION_GREATER_EQUAL 5.6.0)
            find_package(Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
        else()
            find_package(Qt5 REQUIRED COMPONENTS Widgets)
        endif()
    endif()
elseif(XOPCODECALC_QT_MAJOR GREATER_EQUAL 6)
    find_package(Qt${XOPCODECALC_QT_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

    if(NOT TARGET Qt5::Widgets)
        add_library(Qt5::Widgets INTERFACE IMPORTED)
        set_property(TARGET Qt5::Widgets PROPERTY
            INTERFACE_LINK_LIBRARIES Qt${XOPCODECALC_QT_MAJOR}::Widgets
        )
    endif()
endif()

file (STRINGS ${CMAKE_CURRENT_LIST_DIR}/../release_version.txt X_PROJECT_VERSION)
project(XOpcodeCalc VERSION ${X_PROJECT_VERSION} LANGUAGES CXX)

set(X_COMPANYNAME "ntinfo")
set(X_PROJECTNAME "XOpcodeCalc")
set(X_MAINTAINER "horsicq@gmail.com")
set(X_DESCRIPTION "XOpcodeCalc is an opcode calculator")
set(X_HOMEPAGE "http://ntinfo.biz")
set(X_ORIGINAL_FILENAME "xocalc")

if(NOT DEFINED MACOSX_BUNDLE_GUI_IDENTIFIER OR "${MACOSX_BUNDLE_GUI_IDENTIFIER}" STREQUAL "")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.xopcodecalc")
endif()
set(BUNDLE_ID_OPTION "${MACOSX_BUNDLE_GUI_IDENTIFIER}")

set(X_RESOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../res")
set(X_WINDOWS_ICON "${X_RESOURCE_DIR}/windows/main.ico")
set(X_WINDOWS_RESOURCE_RC_IN "${X_RESOURCE_DIR}/windows/resource.rc.in")
set(X_WINDOWS_RESOURCE_ICON_RC_IN "${X_RESOURCE_DIR}/windows/resource_icon.rc.in")
set(X_WINDOWS_RESOURCE_RC "${X_RESOURCE_DIR}/windows/resource.rc")
set(X_WINDOWS_RESOURCE_ICON_RC "${X_RESOURCE_DIR}/windows/resource_icon.rc")
set(X_MACOS_ICON "${X_RESOURCE_DIR}/macos/main.icns")
set(X_MACOS_INFO_PLIST_IN "${X_RESOURCE_DIR}/macos/Info.plist.in")
set(X_MACOS_INFO_PLIST "${X_RESOURCE_DIR}/macos/Info.plist")

set(XOPCODECALC_DEPLOY_INIT "${CMAKE_CURRENT_LIST_DIR}/../dep/build_tools/cmake/deploy_init.cmake")

if(EXISTS "${XOPCODECALC_DEPLOY_INIT}"
   AND EXISTS "${X_WINDOWS_RESOURCE_RC_IN}"
   AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC_IN}"
   AND EXISTS "${X_MACOS_ICON}"
   AND EXISTS "${X_MACOS_INFO_PLIST_IN}")
    include("${XOPCODECALC_DEPLOY_INIT}")
else()
    message(STATUS "Skipping deploy_init.cmake because packaging resource templates are missing")
endif()

add_subdirectory(asm)
add_subdirectory(gui)
