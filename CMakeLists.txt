cmake_minimum_required(VERSION 3.10)
project(n-body-sim VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE SOURCES
    "src/*.cpp"
    "src/world_gens/*.cpp"
)
add_executable(n-body ${SOURCES})

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
set(OPENCL_ICD_LOADER_HEADERS_DIR ${openclheaders_SOURCE_DIR})
FetchContent_MakeAvailable(OpenCL)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

message(glad_BINARY_DIR: ${glad_BINARY_DIR})
target_include_directories(n-body PRIVATE ${glad_BINARY_DIR}/include ${OPENGL_INCLDUE_DIRS})

target_link_libraries(n-body glfw)
target_link_libraries(n-body glad)
target_link_libraries(n-body OpenCL)
target_link_libraries(n-body glm::glm)

find_package(OpenGL REQUIRED)
if (OpenGL_FOUND)
    target_link_libraries(n-body ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL was not found on the system")
endif()

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders
    DEPENDS ${SOURCES}
)

add_custom_target(copy_kernels ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/kernels ${CMAKE_BINARY_DIR}/kernels
    DEPENDS ${SOURCES}
)

add_dependencies(n-body copy_shaders)
add_dependencies(n-body copy_kernels)

add_compile_definitions(CL_TARGET_OPENCL_VERSION=120)
add_compile_definitions(GLEW_BUILD)