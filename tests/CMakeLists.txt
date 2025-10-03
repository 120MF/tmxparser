# Enable CTest
enable_testing()

# Create test executable
add_executable(test_parser test_parser.cpp)

target_link_libraries(test_parser
    PRIVATE
    tmxparser
)

# Create test executable for infinite maps
add_executable(test_infinite test_infinite.cpp)

target_link_libraries(test_infinite
    PRIVATE
    tmxparser
)

# Add tests for each TMX file with different compression methods
add_test(NAME test_csv
    COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(NAME test_base64
    COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test_b64.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(NAME test_base64_gzip
    COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test_b64_gzip.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(NAME test_base64_zlib
    COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test_b64_zlib.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_test(NAME test_base64_zstd
    COMMAND test_parser "${PROJECT_SOURCE_DIR}/assets/test_b64_zstd.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Add test for infinite map
add_test(NAME test_infinite
    COMMAND test_infinite "${PROJECT_SOURCE_DIR}/assets/infinite/Interior1.tmx"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# Set test properties
set_tests_properties(
    test_csv
    test_base64
    test_base64_gzip
    test_base64_zlib
    test_base64_zstd
    test_infinite
    PROPERTIES
    TIMEOUT 10
)
