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
    test_allocator.cpp
    test_debug.cpp
    test_math.cpp
    test_types.cpp
    test_string.cpp
    test_window.cpp
    test_ref_cnt_auto_ptr.cpp
)

if(WIN32)
    message("Configuring tests for Windows platform")
    add_compile_definitions(BOREALIS_WIN)
elseif(APPLE)
    message("Configuring tests for MacOS platform")
    add_compile_definitions(BOREALIS_OSX)
elseif(UNIX)
    message("Configuring tests for Linux platform")
    add_compile_definitions(BOREALIS_LINUX)
endif()

#if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#    add_compile_definitions(BOREALIS_DEBUG)
#elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#    add_compile_definitions(BOREALIS_RELEASE)
#elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
#    add_compile_definitions(BOREALIS_RELWITHDEBINFO)
#elseif(CMAKE_BUILD_TYPE STREQUAL "MinSizeRel")
#    add_compile_definitions(BOREALIS_MINSIZEREL)
#endif()

#set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel")

add_executable(BorealisEngineTests ${SOURCES} "test_string.cpp")

# Define preprocessor macros
target_compile_definitions(BorealisEngineTests PRIVATE
    -DBOREALIS_DEBUG
    -DBOREALIS_VERSION=${CMAKE_PROJECT_VERSION}
)

if(WIN32)

    # Copy necessary dll to output directory
    add_custom_command(
        TARGET BorealisEngineTests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:BorealisEngineTests>
            $<TARGET_FILE_DIR:BorealisEngineTests>
        COMMENT "Copying runtime DLLs to output directory..."
        COMMAND_EXPAND_LISTS
    )

elseif(UNIX OR APPLE)

    # Set path for unix and osx to point to lib output dir
    set_target_properties(BorealisEngineTests PROPERTIES
        BUILD_RPATH "${CMAKE_CURRENT_BINARY_DIR}/../src"
    )

endif()


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