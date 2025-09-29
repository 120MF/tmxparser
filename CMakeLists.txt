cmake_minimum_required(VERSION 3.20)
project(tmxparser)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

include(CheckModules)

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)