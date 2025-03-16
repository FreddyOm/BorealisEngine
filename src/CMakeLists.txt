# CMake version
cmake_minimum_required(VERSION 3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project information
project(
    BorealisEngineLib
    VERSION 0.1.0
    LANGUAGES CXX C
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

    find_package(X11 REQUIRED)
    link_libraries(${X11_LIBRARIES})
    include_directories(${X11_INCLUDE_DIR})
endif()

message("Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION} ...")

# Project source files
set(SOURCES 
    ./core/window/window.cpp
    ./core/window/window_internal.cpp
)

# Project include files
set(HEADERS
    config.h
    borealis_engine.h
    core/types/types.h
    core/window/window.h
    core/window/window_internal.h
    helpers/helpers.h
)

# Create library
add_library(BorealisEngineLib SHARED ${SOURCES} ${HEADERS})