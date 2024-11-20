@echo off
setlocal

:: Check if a parameter was provided
if "%~1"=="" (
    echo Please drag a .wara file to this window.
    pause
    exit /b 1
)

:: Get the dragged .wara file path
set "WARA_FILE=%~1"

:: Execute waratahcmd.exe
waratahcmd.exe -s "%WARA_FILE%" -d "Descriptor"

:: Pause to prevent the window from closing
echo.
echo Press any key to exit...
pause

endlocal
