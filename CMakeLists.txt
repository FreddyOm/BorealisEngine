# CMake version
cmake_minimum_required(VERSION 3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project information
project(
    BorealisEngine 
    VERSION 0.1.0
    LANGUAGES CXX C
)

# Create dll
message("Building engine library...")
add_subdirectory(src)

# Create test
message("Building tests...")
enable_testing()
add_subdirectory(tests)
