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
    test_graphics.cpp
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

add_executable(BorealisEngineTests ${SOURCES})

# Define preprocessor macros for build configurations
target_compile_definitions(BorealisEngineTests PRIVATE
        $<$<CONFIG:Debug>:BOREALIS_DEBUG>
        $<$<CONFIG:Release>:BOREALIS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BOREALIS_RELWITHDEBINFO>
        $<$<CONFIG:MinSizeRel>:BOREALIS_MINSIZEREL>
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
    ${CMAKE_SOURCE_DIR}/src/
)

# Tests
include(GoogleTest)
gtest_discover_tests(BorealisEngineTests)