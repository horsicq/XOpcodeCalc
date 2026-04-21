function(_deploy_qt_windows_install_files _qt_prefix _destination)
    foreach(_deploy_qt_windows_relpath ${ARGN})
        install(FILES "${_qt_prefix}/${_deploy_qt_windows_relpath}" DESTINATION "${_destination}" OPTIONAL)
    endforeach()
endfunction()

function(_deploy_qt_windows_install_glob _qt_prefix _pattern _destination)
    file(GLOB _deploy_qt_windows_matches LIST_DIRECTORIES FALSE "${_qt_prefix}/${_pattern}")
    if(_deploy_qt_windows_matches)
        install(FILES ${_deploy_qt_windows_matches} DESTINATION "${_destination}" OPTIONAL)
    endif()
endfunction()

message(STATUS qt_version_${QT_VERSION_MAJOR})
if(WIN32)
    set(_deploy_qt_windows_prefix_paths ${CMAKE_PREFIX_PATH})
    if(_deploy_qt_windows_prefix_paths)
        list(GET _deploy_qt_windows_prefix_paths 0 _deploy_qt_windows_prefix)
    else()
        set(_deploy_qt_windows_prefix "${CMAKE_PREFIX_PATH}")
    endif()

    string(REPLACE "\\" "/" _deploy_qt_windows_prefix "${_deploy_qt_windows_prefix}")

    if(NOT "${Qt5Core_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Core.dll"
        )
    endif()

    if(NOT "${Qt5Gui_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Gui.dll"
            "bin/Qt5Svg.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "platforms"
            "plugins/platforms/qwindows.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "imageformats"
            "plugins/imageformats/qgif.dll"
            "plugins/imageformats/qicns.dll"
            "plugins/imageformats/qico.dll"
            "plugins/imageformats/qjpeg.dll"
            "plugins/imageformats/qsvg.dll"
            "plugins/imageformats/qtga.dll"
            "plugins/imageformats/qtiff.dll"
            "plugins/imageformats/qwbmp.dll"
            "plugins/imageformats/qwebp.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "iconengines"
            "plugins/iconengines/qsvgicon.dll"
        )
    endif()

    if(NOT "${Qt5Widgets_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Widgets.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "styles"
            "plugins/styles/qwindowsvistastyle.dll"
        )
    endif()

    if(NOT "${Qt5OpenGL_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5OpenGL.dll"
        )
    endif()

    if(NOT "${Qt5Svg_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Svg.dll"
        )
    endif()

    if(NOT "${Qt5Sql_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Sql.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "sqldrivers"
            "plugins/sqldrivers/qsqlite.dll"
        )
    endif()

    if(NOT "${Qt5Network_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Network.dll"
        )
    endif()

    if(NOT "${Qt5Script_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5Script.dll"
        )
    endif()

    if(NOT "${Qt5ScriptTools_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt5ScriptTools.dll"
        )
    endif()

    if(NOT "${Qt6Core_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Core.dll"
        )
    endif()

    if(NOT "${Qt6Gui_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Gui.dll"
            "bin/Qt6Svg.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "platforms"
            "plugins/platforms/qwindows.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "imageformats"
            "plugins/imageformats/qgif.dll"
            "plugins/imageformats/qicns.dll"
            "plugins/imageformats/qico.dll"
            "plugins/imageformats/qjpeg.dll"
            "plugins/imageformats/qsvg.dll"
            "plugins/imageformats/qtga.dll"
            "plugins/imageformats/qtiff.dll"
            "plugins/imageformats/qwbmp.dll"
            "plugins/imageformats/qwebp.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "iconengines"
            "plugins/iconengines/qsvgicon.dll"
        )
    endif()

    if(NOT "${Qt6Widgets_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Widgets.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "styles"
            "plugins/styles/qwindowsvistastyle.dll"
        )
    endif()

    if(NOT "${Qt6OpenGL_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6OpenGL.dll"
        )
    endif()

    if(NOT "${Qt6Svg_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Svg.dll"
        )
    endif()

    if(NOT "${Qt6Sql_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Sql.dll"
        )
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "sqldrivers"
            "plugins/sqldrivers/qsqlite.dll"
        )
    endif()

    if(NOT "${Qt6Network_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Network.dll"
        )
    endif()

    if(NOT "${Qt6Qml_VERSION}" STREQUAL "")
        _deploy_qt_windows_install_files("${_deploy_qt_windows_prefix}" "."
            "bin/Qt6Qml.dll"
        )
    endif()
endif()
