call %X_SOURCE_PATH%\build_tools\windows.cmd make_init

IF NOT [%X_ERROR%] == [] goto exit

call %X_SOURCE_PATH%\build_tools\windows.cmd make_build %X_SOURCE_PATH%\xopcodecalc_source.pro

call %X_SOURCE_PATH%\build_tools\windows.cmd check_file %X_SOURCE_PATH%\build\release\xocalc.exe

IF NOT [%X_ERROR%] == [] goto exit

copy %X_SOURCE_PATH%\build\release\xocalc.exe %X_SOURCE_PATH%\release\%X_BUILD_NAME%\

call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_library Qt5Widgets
call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_library Qt5Gui
call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_library Qt5Core
call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_qt_plugin platforms qwindows
call %X_SOURCE_PATH%\build_tools\windows.cmd deploy_redist

call %X_SOURCE_PATH%\build_tools\windows.cmd make_release
:exit
call %X_SOURCE_PATH%\build_tools\windows.cmd make_clear