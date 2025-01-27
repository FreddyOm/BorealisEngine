# CMake version
cmake_minimum_required(VERSION 3.19...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

# Project information
project(
    BorealisTests
    VERSION 0.1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.15.2
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(googletest)

# Project source files
set(SOURCES 
    test_helpers.cpp
    test_types.cpp
    test_window.cpp
)

add_executable(BorealisEngineTests ${SOURCES} "test_window.cpp")
target_link_libraries(BorealisEngineTests GTest::gtest_main)


# Tests
include(GoogleTest)
gtest_discover_tests(BorealisEngineTests)