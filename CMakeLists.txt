# CMake version
cmake_minimum_required(VERSION 3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

# Project information
project(
    BorealisEngine
    VERSION 0.1.0
    LANGUAGES CXX C
)

# Create dll
message(STATUS "BUILDING ENGINE LIBRARY...")
add_subdirectory(src)


# Create sandbox
message(STATUS "BUILDING ENGINE SANDBOX...")
add_subdirectory(sandbox)

# Create test
message(STATUS "BUILDING TESTS ...")
enable_testing()
add_subdirectory(tests)
