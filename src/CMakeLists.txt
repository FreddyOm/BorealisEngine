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

# Platform-specific configurations
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
endif()

# Add GLFW as a subdirectory
message("Adding GLFW via ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/")

message("Building ${CMAKE_PROJECT_NAME} v${CMAKE_PROJECT_VERSION} ...")

# Project source files
set(SOURCES 
    core/types/string_id.cpp
    core/window/window.cpp
    core/debug/logger.cpp
    core/debug/logger_internal.cpp
    core/debug/runtime-debug/runtime_debug.cpp
    core/debug/runtime-debug/IGUIDrawable.cpp
    core/debug/runtime-debug/imgui/imgui.cpp
    core/debug/runtime-debug/imgui/imgui_demo.cpp
    core/debug/runtime-debug/imgui/imgui_draw.cpp
    core/debug/runtime-debug/imgui/imgui_impl_dx11.cpp
    core/debug/runtime-debug/imgui/imgui_impl_dx12.cpp
    core/debug/runtime-debug/imgui/imgui_impl_vulkan.cpp
    core/debug/runtime-debug/imgui/imgui_impl_win32.cpp
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
    core/graphics/d3d11/borealis_d3d11.cpp
    core/graphics/d3d12/borealis_d3d12.cpp
    core/graphics/vulkan/borealis_vulkan.cpp
    core/input/input.cpp
    core/input/keyboard.cpp
)

# Project include files
set(HEADERS
    config.h
    borealis_engine.h
    core/types/types.h
    core/types/string_id.h
    core/window/window.h
    core/debug/logger.h
    core/debug/logger_internal.h
    core/debug/runtime-debug/runtime_debug.h
    core/debug/runtime-debug/runtime_debug_internal.h
    core/debug/runtime-debug/IGUIDrawable.h
    core/debug/runtime-debug/imgui/imconfig.h
    core/debug/runtime-debug/imgui/imgui.h
    core/debug/runtime-debug/imgui/imgui_impl_dx11.h
    core/debug/runtime-debug/imgui/imgui_impl_dx12.h
    core/debug/runtime-debug/imgui/imgui_impl_vulkan.h
    core/debug/runtime-debug/imgui/imgui_impl_win32.h
    core/debug/runtime-debug/imgui/imgui_impl_glfw.h
    core/debug/runtime-debug/imgui/imgui_internal.h
    core/debug/runtime-debug/imgui/imstb_textedit.h
    core/debug/runtime-debug/imgui/imstb_truetype.h
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
    core/graphics/d3d11/borealis_d3d11.h
    core/graphics/d3d11/d3d11_common.h
    core/graphics/d3d12/borealis_d3d12.h
    core/graphics/d3d12/d3d12_common.h
    core/graphics/vulkan/borealis_vulkan.h
    core/graphics/helpers/helpers.h
    core/graphics/helpers/d3d12_helpers.h
    core/graphics/helpers/gbuffer.h
    core/helpers/events.h
    core/input/input.h
    core/input/keyboard.h
)

# Specify lib directory for GLFW
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/lib)


# Create library
if(WIN_32)
add_library(BorealisEngineLib SHARED ${SOURCES} ${HEADERS}) #${CMAKE_CURRENT_SOURCE_DIR}/core/graphics/d3d12/DirectX-Headers/include)
else()
add_library(BorealisEngineLib SHARED ${SOURCES} ${HEADERS})
endif(WIN_32)

# Define preprocessor macros for build configurations
target_compile_definitions(BorealisEngineLib PRIVATE
        $<$<CONFIG:Debug>:BOREALIS_DEBUG>
        $<$<CONFIG:Release>:BOREALIS_RELEASE>
        $<$<CONFIG:RelWithDebInfo>:BOREALIS_RELWITHDEBINFO>
        $<$<CONFIG:MinSizeRel>:BOREALIS_MINSIZEREL>
        -DBOREALIS_VERSION=${CMAKE_PROJECT_VERSION}
)

if(WIN32)
    target_link_libraries(BorealisEngineLib PUBLIC 
        d3d12
        dxgi
        dxguid
        d3dcompiler
        "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/lib/glfw3.lib"
    )
endif()

target_include_directories(BorealisEngineLib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/GLFW/include/)