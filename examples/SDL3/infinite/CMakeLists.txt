add_executable(tmxparser_sdl3_infinite
    main.cpp
)

target_link_libraries(tmxparser_sdl3_infinite
    PRIVATE
    tmxparser_sdl3_common
)

target_compile_definitions(tmxparser_sdl3_infinite
    PRIVATE
    ASSET_DIR="${PROJECT_SOURCE_DIR}/assets/"
)
