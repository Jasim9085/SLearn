cmake_minimum_required(VERSION 3.16)
project(T760_Engine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Add Third-Party Dependencies ---
# This makes Eigen's headers available to our entire project.
include_directories(third_party/eigen)


# --- Shader Compilation Setup ---
# Find the glslc compiler from the Vulkan SDK
find_program(GLSLC_COMPILER glslc)
if(NOT GLSLC_COMPILER)
    message(FATAL_ERROR "glslc compiler not found. Please install the Vulkan SDK and ensure it is in your PATH.")
endif()

# Create a build target that will represent all compiled shaders.
add_custom_target(Shaders)

# Find all shader source files (.comp) and define a rule to compile each one to .spv
file(GLOB SHADER_SOURCES "src/shaders/*.comp")
foreach(SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SRC} NAME_WE)
    set(SPIRV_OUT "${CMAKE_BINARY_DIR}/shaders/${SHADER_NAME}.spv")

    # This command compiles one shader.
    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND ${GLSLC_COMPILER} -o ${SPIRV_OUT} ${SHADER_SRC}
        DEPENDS ${SHADER_SRC}
        COMMENT "Compiling ${SHADER_SRC}"
    )
    # Create a target for the individual shader and add it as a dependency to the main "Shaders" target.
    add_custom_target(Shader_${SHADER_NAME} DEPENDS ${SPIRV_OUT})
    add_dependencies(Shaders Shader_${SHADER_NAME})
endforeach()


# --- Define the Engine Core Library ---
# This creates a single static library containing all our engine's logic.
add_library(t760_engine_core STATIC)

# CRITICAL: Ensure shaders are compiled before the engine library is built.
add_dependencies(t760_engine_core Shaders)

# Gather all our .cpp source files recursively from the src/ directory.
file(GLOB_RECURSE ENGINE_SOURCES CONFIGURE_DEPENDS "src/*.cpp")
target_sources(t760_engine_core PRIVATE ${ENGINE_SOURCES})

# Make sure the compiler can find our public headers in the "include" directory.
target_include_directories(t760_engine_core PUBLIC include)

# Link necessary system libraries (e.g., for std::mutex).
find_package(Threads REQUIRED)
target_link_libraries(t760_engine_core PRIVATE Threads::Threads)


# --- ANDROID NDK SPECIFIC SECTION ---
# This block is only active when cross-compiling for Android.
# It links the required NDK libraries for GPU, NPU, and logging.
if(ANDROID)
    find_library(log-lib log)
    find_library(android-lib android)
    find_library(nativewindow-lib nativewindow)
    find_library(vulkan-lib vulkan)
    find_library(neuralnetworks-lib neuralnetworks)

    target_link_libraries(t760_engine_core PRIVATE
        ${log-lib}
        ${android-lib}
        ${nativewindow-lib}
        ${vulkan-lib}
        ${neuralnetworks-lib}
    )
endif()


# --- REFERENCE EXECUTABLE (for desktop/host testing) ---
# This creates a simple main.cpp runner to test our library on the host machine.
add_executable(engine_runner main.cpp)

# Link our engine core library to the test runner so it can use the Engine class.
target_link_libraries(engine_runner PRIVATE t760_engine_core)