@echo off
setlocal

call "%~dp0run_xocalc_windows.cmd" "C:\Qt\5.15.2\msvc2019" Win32 qt5_msvc2019_32
set "RESULT=%ERRORLEVEL%"

endlocal & exit /b %RESULT%
