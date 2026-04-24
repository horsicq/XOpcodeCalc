include(../../dep/build_tools/cmake/cpp_standart_setup.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XAboutWidget/xaboutwidget.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/Controls/xlineedithex.cmake)
if(NOT DEFINED XOPTIONS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XOptions/xoptions.cmake)
endif()
include(${CMAKE_CURRENT_LIST_DIR}/../../dep/XStyles/xstyles.cmake)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

set(XOPCODECALC_GUI_SOURCES
    ${XABOUTWIDGET_SOURCES}
    ${XLINEEDITHEX_SOURCES}
    ${XOPTIONS_SOURCES}
    ${XSTYLES_SOURCES}
    main_gui.cpp
    bigint.h
    dialogabout.cpp
    dialogabout.h
    dialogabout.ui
    dialogoptions.cpp
    dialogoptions.h
    dialogoptions.ui
    guimainwindow.cpp
    guimainwindow.h
    guimainwindow.ui
    resources.qrc
)

if(WIN32 AND EXISTS "${X_WINDOWS_RESOURCE_ICON_RC}")
    list(APPEND XOPCODECALC_GUI_SOURCES "${X_WINDOWS_RESOURCE_ICON_RC}")
endif()

if(APPLE AND EXISTS "${X_MACOS_ICON}")
    set_source_files_properties("${X_MACOS_ICON}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    list(APPEND XOPCODECALC_GUI_SOURCES "${X_MACOS_ICON}")
endif()

add_executable(XOpcodeCalc WIN32
    ${XOPCODECALC_GUI_SOURCES}
)

set_target_properties(XOpcodeCalc PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_GUI_IDENTIFIER "${MACOSX_BUNDLE_GUI_IDENTIFIER}"
    OUTPUT_NAME xocalc
)

target_include_directories(XOpcodeCalc PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(XOpcodeCalc PRIVATE
    Qt${XOPCODECALC_QT_MAJOR}::Widgets
    xopcodecalc_asm
)

if(TARGET xopcodecalc_translations)
    add_dependencies(XOpcodeCalc xopcodecalc_translations)
endif()

if(UNIX AND NOT APPLE)
    set(XOPCODECALC_RUNTIME_INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")
    set(XOPCODECALC_DOC_INSTALL_DESTINATION "${CMAKE_INSTALL_DOCDIR}/xopcodecalc")
    set(XOPCODECALC_DESKTOP_FILE "${CMAKE_CURRENT_BINARY_DIR}/xocalc.desktop")

    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/xocalc.desktop.in"
        "${XOPCODECALC_DESKTOP_FILE}"
        @ONLY
    )
else()
    set(XOPCODECALC_RUNTIME_INSTALL_DESTINATION ".")
    set(XOPCODECALC_DOC_INSTALL_DESTINATION ".")
endif()

install(TARGETS XOpcodeCalc
    RUNTIME DESTINATION ${XOPCODECALC_RUNTIME_INSTALL_DESTINATION}
    BUNDLE DESTINATION .
)

if(UNIX AND NOT APPLE)
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../README.md
        DESTINATION ${XOPCODECALC_DOC_INSTALL_DESTINATION}
        OPTIONAL
    )

    install(FILES
        "${XOPCODECALC_DESKTOP_FILE}"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )

    install(DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/../../res/linux/hicolor/"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor"
    )
else()
    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../LICENSE
        DESTINATION .
        RENAME LICENSE.txt
        OPTIONAL
    )

    install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/../../README.md
        ${CMAKE_CURRENT_SOURCE_DIR}/../../res/license.txt
        ${CMAKE_CURRENT_SOURCE_DIR}/../../res/readme.txt
        ${X_WINDOWS_ICON}
        DESTINATION .
        OPTIONAL
    )

    install(CODE [[
file(WRITE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/portable" "")
]])
endif()

if(APPLE)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/macdeployqt.cmake)
endif()

if(WIN32)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_qt_windows.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/../../dep/build_tools/cmake/deploy_msvc.cmake)
endif()
