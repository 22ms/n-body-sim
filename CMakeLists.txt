cmake_minimum_required(VERSION 3.10)
project(n-body-sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    if(CMAKE_COMPILER_IS_GNUCC)
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
    endif()
endif()

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/world_gens/*.cpp"
)
add_executable(n-body ${SOURCES})
set_target_properties(n-body PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}
)

include(FetchContent)

FetchContent_Declare(
    glfw3
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw3)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad
    GIT_TAG v0.1.27
)
FetchContent_MakeAvailable(glad)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

find_package(OpenCL)
if (OpenCL_FOUND)
    target_link_libraries(n-body ${OpenCL_LIBRARY})
    target_include_directories(n-body PRIVATE
        ${OpenCL_INCLUDE_DIRS}
    )
else()
    FetchContent_Declare(
        OpenCLHeaders
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-Headers
        GIT_TAG v2024.05.08
    )
    FetchContent_MakeAvailable(OpenCLHeaders)

    FetchContent_Declare(
        OpenCL
        GIT_REPOSITORY https://github.com/KhronosGroup/OpenCL-ICD-Loader
        GIT_TAG v2024.05.08
    )
    set(OPENCL_ICD_LOADER_HEADERS_DIR ${OpenCLHeaders_SOURCE_DIR})
    FetchContent_MakeAvailable(OpenCL)
    target_include_directories(n-body PRIVATE
        ${OPENCL_ICD_LOADER_HEADERS_DIR}
        ${OpenCLHeaders_SOURCE_DIR}
    )
    target_link_libraries(n-body OpenCL)
endif()

find_package(OpenGL REQUIRED)
if (NOT OpenGL_FOUND)
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()

target_include_directories(n-body PRIVATE
    ${glad_BINARY_DIR}/include
)

target_link_libraries(n-body
    glfw
    glad
    glm::glm
    ${OPENGL_LIBRARIES}
)

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
    DEPENDS ${SOURCES}
)

add_custom_target(copy_kernels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/kernels ${CMAKE_CURRENT_BINARY_DIR}/kernels
    DEPENDS ${SOURCES}
)

add_dependencies(n-body copy_shaders copy_kernels)

target_compile_definitions(n-body PRIVATE CL_TARGET_OPENCL_VERSION=120)
target_compile_definitions(n-body PRIVATE _USE_MATH_DEFINES)