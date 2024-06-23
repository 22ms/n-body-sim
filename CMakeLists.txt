cmake_minimum_required(VERSION 3.10)
project(n-body-sim VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_BUILD_TYPE Debug)

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(OpenCL 1.2 REQUIRED)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(n-body ${SOURCES})

target_link_libraries(n-body glfw)
target_link_libraries(n-body GLEW::GLEW)
target_link_libraries(n-body OpenGL::GL)
target_link_libraries(n-body OpenCL::OpenCL)

file(COPY shaders DESTINATION ${CMAKE_BINARY_DIR})