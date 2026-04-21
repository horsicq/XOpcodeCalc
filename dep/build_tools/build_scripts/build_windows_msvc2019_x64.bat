@echo off
setlocal

REM Locate Visual Studio environment (prefer VS 2022, fallback to 2019)
set "VCVARSALL="
if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" set "VCVARSALL=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat"
if not defined VCVARSALL (
	echo ERROR: Visual Studio vcvarsall.bat not found (VS 2022/2019).
	exit /b 1
)

call "%VCVARSALL%" x64 || goto :error

REM Compute project root from script location: project_root = build_scripts\..
set "SCRIPT_DIR=%~dp0"
set "SOURCE_DIR=%SCRIPT_DIR%.."

pushd "%SOURCE_DIR%"
mkdir tmp_build 2>nul
pushd tmp_build

REM Detect Qt 5.15.x for MSVC2019 x64
set "QT_PREFIX_PATH=%QT_PREFIX_PATH%"
if not defined QT_PREFIX_PATH if exist "C:\Qt\5.15.2\msvc2019_64" set "QT_PREFIX_PATH=C:\Qt\5.15.2\msvc2019_64"
if not defined QT_PREFIX_PATH if exist "C:\Qt\5.15.1\msvc2019_64" set "QT_PREFIX_PATH=C:\Qt\5.15.1\msvc2019_64"
if not defined QT_PREFIX_PATH if exist "C:\Qt\5.15.0\msvc2019_64" set "QT_PREFIX_PATH=C:\Qt\5.15.0\msvc2019_64"
if not defined QT_PREFIX_PATH (
	for /D %%D in ("C:\Qt\5.*") do (
		if exist "%%D\msvc2019_64" (
			set "QT_PREFIX_PATH=%%D\msvc2019_64"
			goto :qt_found
		)
	)
)
:qt_found

if defined QT_PREFIX_PATH (
	echo Using Qt: "%QT_PREFIX_PATH%"
	cmake -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=MinSizeRel -DCMAKE_PREFIX_PATH="%QT_PREFIX_PATH%" .. || goto :error
) else (
	echo WARNING: Qt MSVC2019_64 not found. Proceeding without CMAKE_PREFIX_PATH.
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
