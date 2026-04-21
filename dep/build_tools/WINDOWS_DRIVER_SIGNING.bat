@echo off

:: ---- Check for admin rights ----
net session >nul 2>&1
if %errorLevel% neq 0 (
    powershell -Command "Start-Process '%~f0' -Verb RunAs"
    exit /b
)

:menu
echo =======================================
echo        Driver Signing Toggle by horsicq
echo =======================================
echo.
echo (Before setting BCDEdit options you might need to disable or suspend BitLocker and Secure Boot on the computer.)
echo.
echo 1. Enable Driver Signing (Reboot required)
echo 2. Disable Driver Signing (Reboot required)
echo 3. Exit
echo.

set /p choice=Select an option [1-3]: 

if "%choice%"=="1" goto enable
if "%choice%"=="2" goto disable
if "%choice%"=="3" exit
goto menu

:enable
echo.
echo Enabling Driver Signing... (Please restart your machine)
bcdedit /set nointegritychecks OFF
bcdedit /set loadoptions DISABLE_INTEGRITY_CHECKS
bcdedit /set TESTSIGNING OFF
echo Done.
pause
exit

:disable
echo.
echo Disabling Driver Signing... (Please restart your machine)
bcdedit /set nointegritychecks ON
bcdedit /set loadoptions DISABLE_INTEGRITY_CHECKS
bcdedit /set TESTSIGNING ON
echo Done.
pause
exit