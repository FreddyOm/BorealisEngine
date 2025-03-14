# CMake version
cmake_minimum_required(VERSION 3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project information
project(
    BorealisEngineTests
    VERSION 0.1.0
    LANGUAGES CXX C
)

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

if(WIN32)
    message("Configuring engine lib for Windows platform")
    add_compile_definitions(BOREALIS_WIN)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(APPLE)
    message("Configuring engine lib for MacOS platform")
    add_compile_definitions(BOREALIS_OSX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(UNIX)
    message("Configuring engine lib for Linux platform")
    add_compile_definitions(BOREALIS_LINUX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
endif()

add_executable(BorealisEngineTests ${SOURCES})

add_custom_command(
    TARGET BorealisEngineTests
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/../src/BorealisEngineLib.dll ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Copy engine lib to output directory..."
)

# Link libraries
target_link_libraries(BorealisEngineTests 
GTest::gtest_main 
BorealisEngineLib
)

# Include directories
target_include_directories(BorealisEngineTests PRIVATE 
${CMAKE_SOURCE_DIR}/src
${CMAKE_SOURCE_DIR}/src/core/types
)

# Tests
include(GoogleTest)
gtest_discover_tests(BorealisEngineTests)

# ${CMAKE_SOURCE_DIR} -> D:\Repos\BorealisEngine
# ${CMAKE_CURRENT_BINARY_DIR} -> D:\Repos\BorealisEngine\out\build\x64-Debug\tests