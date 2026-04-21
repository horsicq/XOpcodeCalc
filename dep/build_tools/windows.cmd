call:%~1 %~2 %~3
goto exit

:check_file
    echo "Check file"
    echo %~1
    IF EXIST %~1 (
        echo "Check file:  %~1: TRUE"
    ) ELSE (
        set X_ERROR="TRUE"
        echo "Cannot find file: %~1"
    )
    goto:eof

:make_init
    echo "init"
    set X_ARCHITECTURE=x86
:msvc_env
    IF [%VSVARS_PATH%] == [] goto mingw_env
    call %VSVARS_PATH%
    set X_MAKE=nmake
    IF [%Platform%] == [] goto qmake_env
    set X_ARCHITECTURE=%Platform%
    goto qmake_env
:mingw_env
    rem TODO platform
    IF [%MINGW_PATH%] == [] goto qmake_env
    set PATH=%MINGW_PATH%
    set X_MAKE=mingw32-make
    goto qmake_env
    set X_ERROR="TRUE"
    echo "Please set MSVC or MinGW"
:qmake_env
    %QMAKE_PATH% -query QT_VERSION > qt_tmp.txt
    set /p X_QT_VERSION=<qt_tmp.txt
    %QMAKE_PATH% -query QT_INSTALL_BINS > qt_tmp.txt
    set /p X_QT_INSTALL_BINS=<qt_tmp.txt
    %QMAKE_PATH% -query QT_INSTALL_PLUGINS > qt_tmp.txt
    set /p X_QT_INSTALL_PLUGINS=<qt_tmp.txt
    %QMAKE_PATH% -query QMAKE_SPEC > qt_tmp.txt
    set /p X_QMAKE_SPEC=<qt_tmp.txt
    del qt_tmp.txt
    
    set X_QT_INSTALL_BINS=%X_QT_INSTALL_BINS:/=\%
    set X_QT_INSTALL_PLUGINS=%X_QT_INSTALL_PLUGINS:/=\%
    
    mkdir %X_SOURCE_PATH%\release
    mkdir %X_SOURCE_PATH%\release\%X_BUILD_NAME%
    mkdir %X_SOURCE_PATH%\build\release
    IF "%X_PORTABLE%"=="1" (
        type NUL > "%X_SOURCE_PATH%\release\%X_BUILD_NAME%\portable"
    )
    
    if exist %X_SOURCE_PATH%\gui_source\ (
        xcopy %X_SOURCE_PATH%\build_tools\windows.manifest.xml %X_SOURCE_PATH%\gui_source\ /Y
    )
    
    if exist %X_SOURCE_PATH%\console_source\ (
        xcopy %X_SOURCE_PATH%\build_tools\windows.manifest.xml %X_SOURCE_PATH%\console_source\ /Y
    )
    
    if exist %X_SOURCE_PATH%\lite_source\ (
        xcopy %X_SOURCE_PATH%\build_tools\windows.manifest.xml %X_SOURCE_PATH%\lite_source\ /Y
    )
    
    xcopy %X_SOURCE_PATH%\build_tools\build.pri %X_SOURCE_PATH%\ /Y
    
    goto:eof
    
:make_build
    IF EXIST "Makefile" (
        if defined JOM_PATH (
            %JOM_PATH% -j %BUILD_JOBS% clean
        ) else (
            %X_MAKE% clean
        )
    )
    %QMAKE_PATH% "%~1" -r -spec %X_QMAKE_SPEC% "CONFIG+=release"
    if defined JOM_PATH (
        echo Using jom with %BUILD_JOBS% parallel jobs
        %JOM_PATH% -j %BUILD_JOBS%
    ) else (
        echo Using nmake (single-threaded)
        %X_MAKE%
    )
    goto:eof
    
:make_build_pdb
    IF EXIST "Makefile" (
        if defined JOM_PATH (
            %JOM_PATH% -j %BUILD_JOBS% clean
        ) else (
            %X_MAKE% clean
        )
    )
    %QMAKE_PATH% "%~1" -r -spec %X_QMAKE_SPEC% "CONFIG+=release" "DEFINES+=CREATE_PDB"
    if defined JOM_PATH (
        %JOM_PATH% -j %BUILD_JOBS%
    ) else (
        %X_MAKE%
    )
    goto:eof
    
:make_translate
    %X_QT_INSTALL_BINS%\lupdate.exe "%~1"
    %X_QT_INSTALL_BINS%\lrelease.exe "%~1"
    mkdir %X_SOURCE_PATH%\release\%X_BUILD_NAME%\lang
    xcopy translation\*.qm  %X_SOURCE_PATH%\release\%X_BUILD_NAME%\lang\  /Y
    goto:eof
    
:deploy_qt
    %X_QT_INSTALL_BINS%\windeployqt.exe %X_SOURCE_PATH%\release\%X_BUILD_NAME%\%~1
    del %X_SOURCE_PATH%\release\%X_BUILD_NAME%\vc_redist.x64.exe
    del %X_SOURCE_PATH%\release\%X_BUILD_NAME%\vc_redist.x86.exe

    goto:eof
   
:deploy_qt_library
    copy %X_QT_INSTALL_BINS%\%~1.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    goto:eof
    
:deploy_qt_plugin
    mkdir %X_SOURCE_PATH%\release\%X_BUILD_NAME%\%~1
    echo "Copying plugin: %X_QT_INSTALL_PLUGINS%\%~1\%~2.dll to %X_SOURCE_PATH%\release\%X_BUILD_NAME%\%~1\"
    copy %X_QT_INSTALL_PLUGINS%\%~1\%~2.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\%~1\
    goto:eof
    
:deploy_vc_redist
    echo %VisualStudioVersion%
    if "%VisualStudioVersion%" == "12.0" (
        copy "%VCINSTALLDIR%\redist\x86\Microsoft.VC120.CRT\msvcp120.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCINSTALLDIR%\redist\x86\Microsoft.VC120.CRT\msvcr120.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    if "%VisualStudioVersion%" == "16.0" (
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\msvcp140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\vcruntime140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\msvcp140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\vcruntime140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    if "%VisualStudioVersion%" == "17.0" (
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\vcruntime140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140_2.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    
    goto:eof
    
:deploy_redist
    if "%VisualStudioVersion%" == "12.0" (
        copy "%VCINSTALLDIR%\redist\x86\Microsoft.VC120.CRT\msvcp120.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCINSTALLDIR%\redist\x86\Microsoft.VC120.CRT\msvcr120.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    if "%VisualStudioVersion%" == "16.0" (
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\msvcp140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\vcruntime140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\msvcp140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC142.CRT\vcruntime140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
	if "%VisualStudioVersion%" == "17.0" (
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\vcruntime140.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140_1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%VCToolsRedistDir%\%Platform%\Microsoft.VC143.CRT\msvcp140_2.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    if "%X_QMAKE_SPEC%" == "win32-g++" (
        copy "%X_QT_INSTALL_BINS%\libgcc_s_dw2-1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%X_QT_INSTALL_BINS%\libstdc++-6.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
        copy "%X_QT_INSTALL_BINS%\libwinpthread-1.dll" %X_SOURCE_PATH%\release\%X_BUILD_NAME%\
    )
    rem TODO more
    
    goto:eof
    
:deploy_openssl
    IF "%VisualStudioVersion%" == "12.0" goto deploy_openssl_winxp
    IF "%VisualStudioVersion%" == "16.0" goto deploy_openssl_win
    goto deploy_openssl_exit
:deploy_openssl_winxp
    xcopy %X_SOURCE_PATH%\build_tools\openssl\winxp_x86\libeay32.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    xcopy %X_SOURCE_PATH%\build_tools\openssl\winxp_x86\ssleay32.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    goto deploy_openssl_exit
:deploy_openssl_win
    if "%VSCMD_ARG_TGT_ARCH%" == "x86" goto deploy_openssl_win_x86
    if "%VSCMD_ARG_TGT_ARCH%" == "x64" goto deploy_openssl_win_x64
    goto deploy_openssl_exit
:deploy_openssl_win_x86
    xcopy %X_SOURCE_PATH%\build_tools\openssl\win_x86\libcrypto-1_1.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    xcopy %X_SOURCE_PATH%\build_tools\openssl\win_x86\libssl-1_1.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    goto deploy_openssl_exit
:deploy_openssl_win_x64
    xcopy %X_SOURCE_PATH%\build_tools\openssl\win_x64\libcrypto-1_1-x64.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    xcopy %X_SOURCE_PATH%\build_tools\openssl\win_x64\libssl-1_1-x64.dll %X_SOURCE_PATH%\release\%X_BUILD_NAME%\ /Y
    goto deploy_openssl_exit
:deploy_openssl_exit
    goto:eof
    
:make_release
    cd %X_SOURCE_PATH%\release
    set X_ZIP_NAME=%X_BUILD_NAME%_%X_BUILD_PREFIX%_portable_%X_RELEASE_VERSION%_%X_ARCHITECTURE%
    if exist %X_ZIP_NAME%.zip del %X_ZIP_NAME%.zip

    %SEVENZIP_PATH% a %X_SOURCE_PATH%\release\%X_ZIP_NAME%.zip %X_BUILD_NAME%\
    set X_ZIP_NAME=
    cd %X_SOURCE_PATH%
    IF [%INNOSETUP_PATH%] == [] goto make_release_end
    if not exist "%INNOSETUP_PATH%\install.iss" goto make_release_end
    %INNOSETUP_PATH% install.iss
    if exist "%X_SOURCE_PATH%\release\%X_BUILD_NAME%_%X_BUILD_PREFIX%_install_%X_RELEASE_VERSION%.exe" del "%X_SOURCE_PATH%\release\%X_BUILD_NAME%_%X_BUILD_PREFIX%_install_%X_RELEASE_VERSION%.exe"
    ren "%X_SOURCE_PATH%\release\install.exe" "%X_BUILD_NAME%_%X_BUILD_PREFIX%_install_%X_RELEASE_VERSION%.exe"
:make_release_end
    rem TODO
    goto:eof 
    
:make_clear
    set X_ERROR=
    set X_QT_VERSION=
    set X_QT_INSTALL_BINS=
    set X_QT_INSTALL_PLUGINS=
    set X_QMAKE_SPEC=
    
    rmdir /s /q %X_SOURCE_PATH%\release\%X_BUILD_NAME%
    
    goto:eof
    
:exit
    exit /b
