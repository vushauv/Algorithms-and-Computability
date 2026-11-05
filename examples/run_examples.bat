@echo off
rem Runs dist\partition.exe on every sample input, valid ones first.
set EXE=%~dp0..\dist\partition.exe
for %%f in ("%~dp0*.txt") do (
    echo ===== %%~nxf
    "%EXE%" "%%f"
    echo.
)
for %%f in ("%~dp0errors\*.txt") do (
    echo ===== errors\%%~nxf
    "%EXE%" "%%f"
)
pause
