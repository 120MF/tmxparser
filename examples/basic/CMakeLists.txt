add_executable(tmxparser_basic_example
        main.cpp
)

target_link_libraries(tmxparser_basic_example
        PRIVATE
        tmxparser
)

target_compile_definitions(tmxparser_basic_example
        PRIVATE
        ASSET_DIR="${PROJECT_SOURCE_DIR}/assets/"
)


