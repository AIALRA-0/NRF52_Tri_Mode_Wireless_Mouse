@echo off
setlocal enabledelayedexpansion

REM Prompt the user to enter a project name
set /p projectName=Please enter the new project name:

REM Find .uvoptx files in the current directory and rename them
for %%f in (*.uvoptx) do (
    set "oldName=%%~nf"
    ren "%%f" "!projectName!.uvoptx"
)

REM Find .uvprojx files in the current directory and rename them
for %%f in (*.uvprojx) do (
    set "oldName=%%~nf"
    ren "%%f" "!projectName!.uvprojx"
)

REM Find and delete files that contain .uvguix in the filename
for %%f in (*.*) do (
    if "%%f" neq "rename_project.bat" (
        echo %%f | find ".uvguix" >nul
        if not errorlevel 1 (
            del "%%f"
        )
    )
)

echo Renaming and deletion completed!
pause
