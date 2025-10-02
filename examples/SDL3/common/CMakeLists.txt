# Common SDL3 utilities library
add_library(tmxparser_sdl3_common STATIC
    sdl3_utils.cpp
    sdl3_utils.hpp
)

target_link_libraries(tmxparser_sdl3_common
    PUBLIC
    tmxparser
    SDL3::SDL3
    stb::image
)

target_include_directories(tmxparser_sdl3_common
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
