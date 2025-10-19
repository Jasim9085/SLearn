cmake_minimum_required(VERSION 3.16)
project(T760_Engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(third_party/eigen)

# --- Shader Compilation Setup (Resilient Method) ---
# First, try to get the path from the environment variable set by the CI.
set(GLSLC_COMPILER "$ENV{GLSLC_PATH}")

# If the environment variable is not set (e.g., local build), fall back to searching.
if(NOT GLSLC_COMPILER)
    find_program(GLSLC_COMPILER_FOUND
        glslc
        HINTS ${ANDROID_NDK}/shader-tools
    )
    if(GLSLC_COMPILER_FOUND)
        set(GLSLC_COMPILER ${GLSLC_COMPILER_FOUND})
    endif()
endif()

# Final check. If we still don't have a compiler, fail loudly.
if(NOT GLSLC_COMPILER OR NOT EXISTS ${GLSLC_COMPILER})
    message(FATAL_ERROR "glslc compiler not found. Ensure GLSL_PATH env var is set or NDK is correct.")
endif()

message(STATUS "Using glslc compiler: ${GLSLC_COMPILER}")

add_custom_target(Shaders)

file(GLOB SHADER_SOURCES "src/shaders/*.comp")
foreach(SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME_WE)
    set(SPIRV_OUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND ${GLSLC_COMPILER} -o ${SPIRV_OUT} ${SHADER_SRC}
        DEPENDS ${SHADER_SRC}
        COMMENT "Compiling ${SHADER_SRC}"
    )
    add_custom_target(Shader_${SHADER_NAME} DEPENDS ${SPIRV_OUT})
    add_dependencies(Shaders Shader_${SHADER_NAME})
endforeach()


# --- Define the Engine Core Library ---
add_library(t760_engine_core STATIC)
add_dependencies(t760_engine_core Shaders)
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
target_sources(t760_engine_core PRIVATE ${ENGINE_SOURCES})
target_include_directories(t760_engine_core PUBLIC include)
find_package(Threads REQUIRED)
target_link_libraries(t760_engine_core PRIVATE Threads::Threads)


# --- ANDROID NDK SPECIFIC SECTION ---
if(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(nativewindow-lib nativewindow)
    find_library(vulkan-lib vulkan)
    find_library(neuralnetworks-lib neuralnetworks)
    target_link_libraries(t760_engine_core PRIVATE
        ${log-lib} ${android-lib} ${nativewindow-lib} ${vulkan-lib} ${neuralnetworks-lib}
    )
endif()


# --- REFERENCE EXECUTABLE (will be ignored by Gradle) ---
add_executable(engine_runner main.cpp)
target_link_libraries(engine_runner PRIVATE t760_engine_core)