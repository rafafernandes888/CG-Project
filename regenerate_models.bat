@echo off
REM ============================================================
REM Regenerates all .3d model files for Phase 4
REM Run from project root (folder containing src/ and build/)
REM ============================================================

setlocal
set GEN=build\src\generator\Release\generator.exe
set OUT=src\models

if not exist "%GEN%" (
    echo Generator not found at %GEN%
    echo Build the project first with: cmake --build build --config Release
    exit /b 1
)

if not exist "%OUT%" mkdir "%OUT%"

echo Generating models...
echo.

"%GEN%" sphere 1 20 20 "%OUT%\sphere_1_20_20.3d"
"%GEN%" sphere 1 10 10 "%OUT%\sphere_1_10_10.3d"
"%GEN%" sphere 1 8 8 "%OUT%\sphere_1_8_8.3d"

"%GEN%" plane 2 3 "%OUT%\plane_2_3.3d"

"%GEN%" box 2 3 "%OUT%\box_2_3.3d"
"%GEN%" box 1 1 "%OUT%\box_1_1.3d"

"%GEN%" cone 1 2 4 3 "%OUT%\cone_1_2_4_3.3d"

if exist "%OUT%\teapot.patch" (
    "%GEN%" bezier "%OUT%\teapot.patch" 10 "%OUT%\bezier_10.3d"
) else (
    echo Warning: teapot.patch not found in %OUT%, skipping bezier
)

echo.
echo Done. Models in %OUT%
endlocal
