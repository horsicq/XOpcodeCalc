@echo off

:: ---- Check for admin rights ----
net session >nul 2>&1
if %errorLevel% neq 0 (
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

:menu
echo ===============================================
echo        Windows Boot Debugging Toggle by horsicq
echo ===============================================
echo.
echo 1. Enable Windows Kernel Debugging Mode (Reboot required)
echo 2. Disable Windows Kernel Debugging Mode (Reboot required)
echo 3. Exit
echo.

set /p choice=Select an option [1-3]: 

if "%choice%"=="1" goto enable
if "%choice%"=="2" goto disable
if "%choice%"=="3" exit
goto menu

:enable
echo.
echo Enabling Windows Kernel Debug... (Please restart your machine)
bcdedit /bootdebug on
echo Done.
pause
exit

:disable
echo.
echo Disabling Windows Kernel Debug... (Please restart your machine)
bcdedit /bootdebug off
echo Done.
pause
exit