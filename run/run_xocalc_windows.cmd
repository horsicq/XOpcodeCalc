@echo off
setlocal
rem Repo convention:
rem - run builds always happen under %TEMP%
rem - repository-local build trees are not used by run helpers

if "%~3"=="" (
    echo Usage: %~nx0 ^<qt-root^> ^<cmake-platform^> ^<run-suffix^>
    exit /b 1
)

for %%I in ("%~dp0..") do set "PROJECT_ROOT=%%~fI"

set "QT_ROOT=%~1"
set "CMAKE_PLATFORM=%~2"
set "RUN_SUFFIX=%~3"

set "WORK_ROOT=%TEMP%\xopcodecalc_run_%RUN_SUFFIX%"
set "BUILD_DIR=%WORK_ROOT%\build"
set "QT_ENV=%QT_ROOT%\bin\qtenv2.bat"
set "APP_EXE=%BUILD_DIR%\src\gui\Release\xocalc.exe"

if not exist "%QT_ROOT%" (
    echo Qt root not found:
    echo   %QT_ROOT%
    exit /b 1
)

if not exist "%QT_ENV%" (
    echo Qt environment script not found:
    echo   %QT_ENV%
    exit /b 1
)

if not exist "%WORK_ROOT%" mkdir "%WORK_ROOT%"
if errorlevel 1 exit /b 1

echo Configuring %RUN_SUFFIX% build in:
echo   %BUILD_DIR%
cmake -S "%PROJECT_ROOT%" -B "%BUILD_DIR%" -G "Visual Studio 17 2022" -A %CMAKE_PLATFORM% -DCMAKE_PREFIX_PATH="%QT_ROOT%"
if errorlevel 1 exit /b 1

echo Building %RUN_SUFFIX% Release...
cmake --build "%BUILD_DIR%" --config Release
if errorlevel 1 exit /b 1

if not exist "%APP_EXE%" (
    echo Application executable not found after build:
    echo   %APP_EXE%
    exit /b 1
)

call "%QT_ENV%"
start "" /D "%BUILD_DIR%\src\gui\Release" "%APP_EXE%"

endlocal
