# Object rendering example
add_executable(tmxparser_sdl3_object main.cpp)

target_link_libraries(tmxparser_sdl3_object
    PRIVATE
        tmxparser
        tmxparser_sdl3_common
        SDL3::SDL3
)

# Define the asset directory macro
target_compile_definitions(tmxparser_sdl3_object
    PRIVATE
        ASSET_DIR="${CMAKE_SOURCE_DIR}/assets/object"
)
