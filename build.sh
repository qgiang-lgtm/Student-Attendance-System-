#!/usr/bin/env bash
set -e

echo "========================================================"
echo "Compiling Smart GPS Attendance System (C++17 Linux/GCC)"
echo "========================================================"

mkdir -p build

echo "[1/2] Compiling Unit Test Suite (tests.out)..."
g++ -std=c++17 -O2 -Isrc -Itests -Ithird_party/googletest/include -Ithird_party/googletest -pthread \
    tests/test_suite.cpp third_party/googletest/src/gtest-all.cc -o tests.out
echo "[PASS] tests.out built successfully."

echo "[2/2] Compiling Main Server Application (server.out)..."
g++ -std=c++17 -O2 -Isrc -pthread src/main.cpp -o server.out
echo "[PASS] server.out built successfully."

echo "========================================================"
echo "Build Completed Successfully!"
echo "Run './tests.out' to execute automated test cases."
echo "Run './server.out' to launch live Web Application."
echo "========================================================"
