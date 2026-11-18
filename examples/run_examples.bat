@echo off
setlocal enabledelayedexpansion
rem Runs dist\partition.exe on every sample input, valid ones first. Each run
rem saves NAME_output.txt next to its input; those files are skipped here.
set EXE=%~dp0..\dist\partition.exe
for %%f in ("%~dp0*.txt") do (
    set "name=%%~nf"
    if /i not "!name:~-7!"=="_output" (
        echo ===== %%~nxf
        "%EXE%" "%%f" 2
        echo.
    )
)
for %%f in ("%~dp0errors\*.txt") do (
    echo ===== errors\%%~nxf
    "%EXE%" "%%f" 2
)
pause
