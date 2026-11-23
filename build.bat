@echo off
REM ========================================================
REM  SMART GPS STUDENT ATTENDANCE SYSTEM - BUILD SCRIPT
REM  MinGW-w64 GCC Compiler (C++17 Standard)
REM ========================================================

echo ========================================================
echo  Compiling Smart GPS Attendance System (C++17 MinGW)
echo ========================================================

REM [1/2] Compiling Unit Test Suite
echo [1/2] Compiling Automated Unit Test Suite (tests.exe)...
g++ -std=c++17 -O2 -Isrc -Itests -Ithird_party/googletest/include -Ithird_party/googletest -pthread ^
    tests/test_suite.cpp third_party/googletest/src/gtest-all.cc -o tests.exe -lws2_32
if %errorlevel% neq 0 (
    echo [FAIL] Compilation failed for tests.exe
    exit /b %errorlevel%
)
echo [PASS] tests.exe built successfully.

REM [2/2] Compiling Main Server Application
echo [2/2] Compiling Main Server Application (server.exe)...
g++ -std=c++17 -O2 -Isrc src/main.cpp -o server.exe -lws2_32
if %errorlevel% neq 0 (
    echo [FAIL] Compilation failed for server.exe
    exit /b %errorlevel%
)
echo [PASS] server.exe built successfully.

echo ========================================================
echo  Build Completed Successfully!
echo  Run '.\tests.exe' to execute automated test cases.
echo  Run '.\run_server.bat' to launch the live Web Application.
echo ========================================================
