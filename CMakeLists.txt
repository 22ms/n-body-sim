cmake_minimum_required(VERSION 3.0.0)
project(n-body-sim VERSION 0.1.0 LANGUAGES C CXX)

find_package(glfw3 CONFIG REQUIRED)

set(SOURCES
    src/main.cpp
    # Add more source files here as needed
)

add_executable(n-body ${SOURCES})

target_link_libraries(n-body glfw)