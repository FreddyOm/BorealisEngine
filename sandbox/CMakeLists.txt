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
    BorealisSandbox
    VERSION 0.1.0
    LANGUAGES CXX C
)

# Project source files
set(SOURCES 
    main.cpp
)

if(WIN32)
    message("Configuring sandbox for Windows platform")
    add_compile_definitions(BOREALIS_WIN)
elseif(APPLE)
    message("Configuring sandbox for MacOS platform")
    add_compile_definitions(BOREALIS_OSX)
elseif(UNIX)
    message("Configuring sandbox for Linux platform")
    add_compile_definitions(BOREALIS_LINUX)
endif()


message("Building engine sandbox...")
add_executable(BorealisSandbox ${SOURCES})

# Define preprocessor macros for build configurations
target_compile_definitions(BorealisSandbox PRIVATE
        $<$<CONFIG:Debug>:BOREALIS_DEBUG>
        $<$<CONFIG:Release>:BOREALIS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BOREALIS_RELWITHDEBINFO>
        $<$<CONFIG:MinSizeRel>:BOREALIS_MINSIZEREL>
        -DBOREALIS_VERSION=${CMAKE_PROJECT_VERSION}
)

if(WIN32)

    # Copy necessary dll to output directory
    add_custom_command(
        TARGET BorealisSandbox
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:BorealisSandbox>
            $<TARGET_FILE_DIR:BorealisSandbox>
        COMMENT "Copying runtime DLLs to output directory..."
        COMMAND_EXPAND_LISTS
    )

elseif(UNIX OR APPLE)

    # Set path for unix and osx to point to lib output dir
    set_target_properties(BorealisSandbox PROPERTIES
        BUILD_RPATH "${CMAKE_CURRENT_BINARY_DIR}/../src"
    )

endif()

# Include directories
target_include_directories(BorealisSandbox PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/
)


# Link libraries
target_link_libraries(BorealisSandbox 
    BorealisEngineLib
)