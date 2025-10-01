cmake_minimum_required(VERSION 3.20)
project(tmxparser)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

option(BUILD_TMX_EXAMPLES "Enable build tmxparser examples" OFF)
option(BUILD_TMX_TESTS "Enable build tmxparser tests" OFF)

include(CheckModules)

add_subdirectory(src)

if (BUILD_TMX_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (BUILD_TMX_TESTS)
    add_subdirectory(tests)
endif ()