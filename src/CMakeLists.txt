# CMake version
cmake_minimum_required(VERSION 3.19)

if(${CMAKE_VERSION} VERSION_LESS 3.19)
    cmake_policy(VERSION ${CMAKE_VERSION})
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

    #include(FetchContent)
    #FetchContent_Declare(
    #    libX11
    #    GIT_REPOSITORY https://github.com/mirror/libX11.git
    #    GIT_TAG libX11-1.8.4
    #    GIT_SHALLOW TRUE
    #)
    #FetchContent_MakeAvailable(libX11)
    #
    #find_package(X11 REQUIRED)
    #link_libraries(${X11_LIBRARIES})
    #include_directories(${X11_INCLUDE_DIR})
endif()

message("Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION} ...")

# Project source files
set(SOURCES 
    core/types/string_id.cpp
    core/window/window.cpp
    core/window/window_internal.cpp
    core/debug/logger.cpp
    core/debug/logger_internal.cpp
    core/memory/stack_allocator.cpp
    core/memory/pool_allocator.cpp
    core/memory/heap_allocator.cpp
    core/memory/ref_cnt_auto_ptr.cpp
    core/memory/memory.cpp
    core/math/math.cpp
    core/math/random.cpp
    core/math/crc_hash.cpp
)

# Project include files
set(HEADERS
    config.h
    borealis_engine.h
    core/types/types.h
    core/types/string_id.h
    core/window/window.h
    core/window/window_internal.h
    core/debug/logger.h
    core/debug/logger_internal.h
    core/memory/allocator.h
    core/memory/stack_allocator.h
    core/memory/pool_allocator.h
    core/memory/heap_allocator.h
    core/memory/ref_cnt_auto_ptr.h
    core/memory/memory.h
    core/math/math.h
    core/math/random.h
    core/math/crc_hash.h    
)

# Create library
add_library(BorealisEngineLib SHARED ${SOURCES} ${HEADERS})

# Define preprocessor macros for build configurations
target_compile_definitions(BorealisEngineLib PRIVATE
        $<$<CONFIG:Debug>:BOREALIS_DEBUG>
        $<$<CONFIG:Release>:BOREALIS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BOREALIS_RELWITHDEBINFO>
        $<$<CONFIG:MinSizeRel>:BOREALIS_MINSIZEREL>
        -DBOREALIS_VERSION=${CMAKE_PROJECT_VERSION}
)
