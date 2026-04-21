@echo off
setlocal

call "%~dp0run_xocalc_windows.cmd" "C:\Qt\5.15.2\msvc2019_64" x64 qt5_msvc2019_64
set "RESULT=%ERRORLEVEL%"

endlocal & exit /b %RESULT%
