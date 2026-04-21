@echo off
setlocal

REM Locate Visual Studio 2013 environment script
set "VCVARSALL="
if exist "%VS120COMNTOOLS%..\..\VC\vcvarsall.bat" set "VCVARSALL=%VS120COMNTOOLS%..\..\VC\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
if not defined VCVARSALL (
	echo ERROR: Visual Studio 2013 vcvarsall.bat not found.
	echo Install VS 2013 (v120) with C++ tools or set VS120COMNTOOLS.
	exit /b 1
)

call "%VCVARSALL%" x86 || goto :error

REM Compute source and build directories
set "SCRIPT_DIR=%~dp0"
set "SOURCE_DIR=%SCRIPT_DIR%..\.."

pushd "%SOURCE_DIR%"
mkdir tmp_build 2>nul
pushd tmp_build

REM Detect Qt 5.6.x MSVC2013
set "QT_PREFIX_PATH=%QT_PREFIX_PATH%"
if not defined QT_PREFIX_PATH if exist "C:\Qt\5.6.3\msvc2013" set "QT_PREFIX_PATH=C:\Qt\5.6.3\msvc2013"
if not defined QT_PREFIX_PATH (
	for /D %%D in ("C:\Qt\5.*") do (
		if exist "%%D\msvc2013" (
			set "QT_PREFIX_PATH=%%D\msvc2013"
			goto :qt_found
		)
	)
)
:qt_found

if defined QT_PREFIX_PATH (
	echo Using Qt: "%QT_PREFIX_PATH%"
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH="%QT_PREFIX_PATH%" .. || goto :error
) else (
	echo WARNING: Qt MSVC2013 not found. Proceeding without CMAKE_PREFIX_PATH.
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel .. || goto :error
)

nmake || goto :error
cpack || echo WARNING: Packaging failed, continuing.

popd
if exist "tmp_build\packages" xcopy /y /E "tmp_build\packages\" "packages\" >nul
popd

endlocal
exit /b 0

:error
echo Build failed with errorlevel %errorlevel%.
popd
popd
endlocal
exit /b %errorlevel%
