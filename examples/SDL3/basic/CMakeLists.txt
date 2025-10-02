add_executable(tmxparser_sdl3_basic
    main.cpp
)

target_link_libraries(tmxparser_sdl3_basic
    PRIVATE
    tmxparser_sdl3_common
)

target_compile_definitions(tmxparser_sdl3_basic
    PRIVATE
    ASSET_DIR="${PROJECT_SOURCE_DIR}/assets/"
)
