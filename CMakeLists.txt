cmake_minimum_required(VERSION 3.0.0)
project(n-body-sim VERSION 0.1.0 LANGUAGES C CXX)

find_package(GLFW3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenCL REQUIRED)

set(SOURCES
    src/main.cpp
    # Add more source files here as needed
)

add_executable(n-body ${SOURCES})

target_link_libraries(n-body glfw)
target_link_libraries(n-body OpenGL::GL)
target_link_libraries(n-body OpenCL::OpenCL)