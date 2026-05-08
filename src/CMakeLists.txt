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
    VERSION 0.1.1
    LANGUAGES CXX C
)

# Platform-specific configurations
if(WIN32)
    message(STATUS "Configuring engine lib for Windows platform")
    add_compile_definitions(BOREALIS_WIN)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(APPLE)
    message(STATUS "Configuring engine lib for MacOS platform")
    add_compile_definitions(BOREALIS_OSX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Configuring engine lib for Linux platform")
    add_compile_definitions(BOREALIS_LINUX)
    add_compile_definitions(BOREALIS_BUILD_DLL)
endif()


message(STATUS "Adding dependencies...")
# GLFW
include(FetchContent)

# Fetch and build GLFW 
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4  # oder eine andere stabile Version
)
FetchContent_MakeAvailable(glfw)

# Vulkan
if(UNIX AND NOT APPLE)
    FetchContent_Declare(
        vulkan_headers
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
        GIT_TAG        v1.4.345
    )
endif()

message(STATUS "Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION} ...")

# Cross-platform sources & headers
set(COMMON_SOURCES 
    core/types/string_id.cpp
    core/window/window.cpp
    core/debug/logger.cpp
    core/debug/logger_internal.cpp
    core/debug/runtime-debug/runtime_debug.cpp
    core/debug/runtime-debug/IGUI_drawable.cpp
    core/debug/runtime-debug/editor_window.cpp
    core/debug/runtime-debug/debug_category_button.cpp
    core/debug/runtime-debug/imgui/imgui.cpp
    core/debug/runtime-debug/imgui/imgui_demo.cpp
    core/debug/runtime-debug/imgui/imgui_draw.cpp
    core/debug/runtime-debug/imgui/imgui_impl_vulkan.cpp
    core/debug/runtime-debug/imgui/imgui_impl_glfw.cpp
    core/debug/runtime-debug/imgui/imgui_tables.cpp
    core/debug/runtime-debug/imgui/imgui_widgets.cpp
    core/memory/stack_allocator.cpp
    core/memory/pool_allocator.cpp
    core/memory/heap_allocator.cpp
    core/memory/ref_cnt_auto_ptr.cpp
    core/memory/memory.cpp
    core/math/math.cpp
    core/math/random.cpp
    core/math/crc_hash.cpp
    core/graphics/vulkan/borealis_vulkan.cpp
    core/graphics/helpers/helpers.cpp
    core/input/input.cpp
    core/input/keyboard.cpp
    core/io/file_io.cpp
)

# Project include files
set(COMMON_HEADERS
    config.h
    borealis_engine.h
    core/types/types.h
    core/types/string_id.h
    core/window/window.h
    core/debug/logger.h
    core/debug/logger_internal.h
    core/debug/runtime-debug/runtime_debug.h
    core/debug/runtime-debug/debug_info_label.h
    core/debug/runtime-debug/debug_category_button.h
    core/debug/runtime-debug/IGUI_drawable.h
    core/debug/runtime-debug/editor_window.h
    core/debug/runtime-debug/imgui/imconfig.h
    core/debug/runtime-debug/imgui/imgui.h
    core/debug/runtime-debug/imgui/imgui_impl_vulkan.h
    core/debug/runtime-debug/imgui/imgui_impl_glfw.h
    core/debug/runtime-debug/imgui/imgui_internal.h
    core/debug/runtime-debug/imgui/imstb_textedit.h
    core/debug/runtime-debug/imgui/imstb_truetype.h
    core/debug/runtime-debug/input_debugger.h
    core/debug/runtime-debug/memory_debugger.h
    core/memory/allocator.h
    core/memory/stack_allocator.h
    core/memory/pool_allocator.h
    core/memory/heap_allocator.h
    core/memory/ref_cnt_auto_ptr.h
    core/memory/memory.h
    core/math/math.h
    core/math/random.h
    core/math/crc_hash.h   
    core/graphics/graphics.h
    core/graphics/pipeline_config.h
    core/graphics/vulkan/borealis_vulkan.h
    core/graphics/helpers/helpers.h
    core/graphics/helpers/gbuffer.h
    core/helpers/events.h
    core/helpers/macros.h
    core/helpers/object_pool.h
    core/input/input.h
    core/input/keyboard.h
    core/input/borealis_devices.h
    core/input/input_device.h
    core/io/file_io.h
)

# Windows platform specific sources & headers
set(WINDOWS_SOURCES
    core/debug/runtime-debug/imgui/imgui_impl_dx11.cpp
    core/debug/runtime-debug/imgui/imgui_impl_dx12.cpp
    core/debug/runtime-debug/imgui/imgui_impl_win32.cpp
    core/graphics/d3d11/borealis_d3d11.cpp
    core/graphics/d3d12/borealis_d3d12.cpp
)

set(WINDOWS_HEADER
    thirdparty/ds5w/include/ds5w.h
    thirdparty/directxtk12/include/directxtk12/WICTextureLoader.h
    core/debug/runtime-debug/imgui/imgui_impl_dx11.h
    core/debug/runtime-debug/imgui/imgui_impl_dx12.h
    core/debug/runtime-debug/imgui/imgui_impl_win32.h
    core/graphics/helpers/d3d12_helpers.h
    core/graphics/d3d11/borealis_d3d11.h
    core/graphics/d3d11/d3d11_common.h
    core/graphics/d3d12/borealis_d3d12.h
    core/graphics/d3d12/d3d12_common.h
)

# Create library
add_library(BorealisEngineLib SHARED ${COMMON_SOURCES} ${COMMON_HEADERS}) #${CMAKE_CURRENT_SOURCE_DIR}/core/graphics/d3d12/DirectX-Headers/include)

target_sources(BorealisEngineLib PRIVATE
    $<$<PLATFORM_ID:Windows>:${WINDOWS_SOURCES}>
    $<$<PLATFORM_ID:Windows>:${WINDOWS_HEADERS}>
)


# Define preprocessor macros for build configurations
target_compile_definitions(BorealisEngineLib PRIVATE
        $<$<CONFIG:Debug>:BOREALIS_DEBUG>
        $<$<CONFIG:Release>:BOREALIS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BOREALIS_RELWITHDEBINFO>
        $<$<CONFIG:MinSizeRel>:BOREALIS_MINSIZEREL>
        -DBOREALIS_VERSION=${CMAKE_PROJECT_VERSION}
)

# Include directories
#message(STATUS "GLFW Include Dir: ${glfw_SOURCE_DIR}/include")

target_include_directories(BorealisEngineLib SYSTEM PRIVATE 
    ${glfw_SOURCE_DIR}/include
    $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<PLATFORM_ID:Darwin>>>:${vulkan_headers_SOURCE_DIR}/include>
    ${CMAKE_SOURCE_DIR}/src/thirdparty/ds5w/include
if(WIN32)
    ${CMAKE_SOURCE_DIR}/src/thirdparty/directxtk12/include
endif()
)

# Link libraries for each target platform
if(WIN32)
    target_link_libraries(BorealisEngineLib PRIVATE 
        d3d12
        dxgi
        dxguid
        uuid
        kernel32
        user32
        comdlg32
        advapi32
        shell32
        ole32
        oleaut32
        runtimeobject
        d3dcompiler
        glfw
        ${CMAKE_SOURCE_DIR}/src/thirdparty/ds5w/ds5w_x64.lib
        ${CMAKE_SOURCE_DIR}/src/thirdparty/directxtk12/lib/DirectXTK12.lib
    )
elseif(APPLE)
    target_link_libraries(BorealisEngineLib PRIVATE
    glfw
    ${Vulkan_LIBRARIES}
    "-framework Cocoa"
    "-framework IOKit"
    "-framework QuartzCore"
    )
elseif(UNIX)

    target_link_libraries(BorealisEngineLib PRIVATE
        glfw
        ${Vulkan_LIBRARIES}
        X11
        Xrandr
        Xinerama
        Xcursor
        Xi
    )
endif()