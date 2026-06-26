@echo off
REM ============================================================
REM Runs all Phase 4 test scenes one by one
REM Close each window to advance to the next test
REM ============================================================

setlocal
set ENG=build\src\engine\Release\engine.exe
set SCN=src\scenes

if not exist "%ENG%" (
    echo Engine not found at %ENG%
    echo Build the project first with: build.bat
    exit /b 1
)

echo Running Phase 4 tests. Close each window to advance.
echo.

for %%T in (test_4_1 test_4_2 test_4_3 test_4_4 test_4_5 test_4_6) do (
    echo === %%T ===
    start /WAIT "%%T" "%ENG%" "%SCN%\%%T.xml"
    taskkill /F /IM engine.exe >nul 2>&1
)

echo.
echo === solar_system4 ===
start /WAIT "solar_system4" "%ENG%" "%SCN%\solar_system4.xml"
taskkill /F /IM engine.exe >nul 2>&1

echo.
echo All tests done.
endlocal