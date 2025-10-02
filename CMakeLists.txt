cmake_minimum_required(VERSION 3.20)
project(tmxparser VERSION 1.0.0)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 26)

option(BUILD_TMX_EXAMPLES "Enable build tmxparser examples" OFF)
option(BUILD_TMX_TESTS "Enable build tmxparser tests" OFF)

include(CheckModules)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_subdirectory(src)

if (BUILD_TMX_EXAMPLES)
    add_subdirectory(examples)
endif ()

if (BUILD_TMX_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

# Install public headers
install(DIRECTORY include/tmx
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(TARGETS tmxparser
    EXPORT tmxparserTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install export targets
install(EXPORT tmxparserTargets
    FILE tmxparserTargets.cmake
    NAMESPACE tmxparser::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tmxparser
)

# Generate config file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/tmxparserConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/tmxparserConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tmxparser
)

# Generate version file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/tmxparserConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install config and version files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/tmxparserConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/tmxparserConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tmxparser
)

# Install cmake modules that dependencies might need
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/tmxparser
    FILES_MATCHING PATTERN "*.cmake"
)