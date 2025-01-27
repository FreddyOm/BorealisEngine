# CMake version
cmake_minimum_required(VERSION 3.19...3.29)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

# Project information
project(
    BorealisEngine 
    VERSION 0.1.0
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(WIN32)
    message("Configuring for Windows platform")
    add_compile_definitions(BOREALIS_WIN)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(APPLE)
    message("Configuring for MacOS platform")
    add_compile_definitions(BOREALIS_OSX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(UNIX)
    message("Configuring for Linux platform")
    add_compile_definitions(BOREALIS_LINUX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
endif()

message("Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION} ...")

# Project source files
set(SOURCES 
    src/main.cpp
    src/core/window/window.cpp
    src/core/window/window_internal.cpp
)

# Project include files
set(HEADERS
    src/config.h
    src/borealis_engine.h
    src/core/types/types.h
    src/core/window/window.h
    src/core/window/window_internal.h
    src/helpers/helpers.h
)

# Create test
enable_testing()
add_subdirectory(tests)

# Create executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})

