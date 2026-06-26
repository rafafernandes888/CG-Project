@echo off
REM ============================================================
REM Builds the project (generator + engine)
REM Toolkits are 32-bit, so we force Win32 platform
REM ============================================================
 
setlocal
 
if not exist build (
    echo Creating build folder...
    mkdir build
    cd build
    cmake -A Win32 ..
    cd ..
)
 
cmake --build build --config Release
 
echo.
echo Build done.
endlocal