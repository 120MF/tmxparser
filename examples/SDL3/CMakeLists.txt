# Find or fetch SDL3
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
include(FindSDL3)
include(FindSTB)

add_executable(tmxparser_sdl3_example
    main.cpp
)

target_link_libraries(tmxparser_sdl3_example
    PRIVATE
    tmxparser
    SDL3::SDL3
    stb::image
)

target_compile_definitions(tmxparser_sdl3_example
    PRIVATE
    ASSET_DIR="${PROJECT_SOURCE_DIR}/assets/"
)
