cmake_minimum_required(VERSION 3.16)
project(SmartGPSAttendanceSystem VERSION 2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/services
    ${CMAKE_CURRENT_SOURCE_DIR}/src/repository
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Google Test (vendored, no internet / no system package needed)
add_library(gtest_all STATIC third_party/googletest/src/gtest-all.cc)
target_include_directories(gtest_all PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/googletest)
find_package(Threads REQUIRED)
target_link_libraries(gtest_all PUBLIC Threads::Threads)

# Automated Unit Tests (Google Test)
add_executable(tests tests/test_suite.cpp)
if(WIN32)
    target_link_libraries(tests gtest_all ws2_32)
else()
    target_link_libraries(tests gtest_all pthread)
endif()

# Main Web & REST API Server
add_executable(server src/main.cpp)
if(WIN32)
    target_link_libraries(server ws2_32)
else()
    target_link_libraries(server pthread)
endif()
