cmake_minimum_required(VERSION 3.11)

project(flat_map LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}")

enable_testing()

add_library(flat_map INTERFACE)
target_include_directories(flat_map INTERFACE .)

add_subdirectory(test)
add_subdirectory(bench)
