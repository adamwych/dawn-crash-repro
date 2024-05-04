cmake_minimum_required(VERSION 3.29)
project(dawntest)

if (APPLE)
    enable_language(OBJC)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET 14.0)

set(DAWN_FETCH_DEPENDENCIES OFF)
set(DAWN_ENABLE_D3D11 OFF)
set(DAWN_ENABLE_VULKAN OFF)
set(DAWN_ENABLE_OPENGLES OFF)
set(DAWN_ENABLE_DESKTOP_GL OFF)
set(DAWN_ENABLE_NULL OFF)
set(DAWN_ENABLE_METAL ON)
set(DAWN_USE_GLFW ON)
set(DAWN_BUILD_SAMPLES ON)
set(TINT_BUILD_SPV_READER ON)
set(TINT_BUILD_TINT ON)
set(TINT_BUILD_SAMPLES OFF)
set(TINT_BUILD_DOCS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_FUZZERS OFF)
set(TINT_BUILD_SPIRV_TOOLS_FUZZER OFF)
set(TINT_BUILD_AST_FUZZER OFF)
set(TINT_BUILD_REGEX_FUZZER OFF)
set(TINT_BUILD_BENCHMARKS OFF)
set(TINT_BUILD_TESTS OFF)
set(TINT_BUILD_AS_OTHER_OS OFF)
set(TINT_BUILD_REMOTE_COMPILE OFF)
set(TINT_BUILD_CMD_TOOLS OFF)

add_subdirectory(../Dawn Dawn)

add_executable(dawntest main.cpp)

target_include_directories(dawntest INTERFACE ../Dawn/include)

target_link_libraries(dawntest
    dawncpp
    dawn_common
    dawn_glfw
    dawn_native
    dawn_utils
    glfw
    webgpu_dawn
)