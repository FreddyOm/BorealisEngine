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

message("Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION}")

# Project source files
set(SOURCES 
    src/main.cpp
)

set(HEADER_FILES
    
)

# Create executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
