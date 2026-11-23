@echo off
title Smart GPS Student Attendance System (C++ Server)
cd /d "%~dp0"
echo ===========================================================
echo  Starting Smart GPS Student Attendance Server on Port 8080...
echo ===========================================================
REM Open the browser 2 seconds after the server has started.
start "" /B cmd /c "timeout /t 2 >nul && start http://127.0.0.1:8080"
echo.
if "%~1"=="" (
    .\server.exe
) else (
    .\server.exe %~1
)
pause
