cmake_minimum_required(VERSION 3.10)

project(GGGS)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(main ${SRC_FILES})