add_library(tmxparser STATIC
    Map.cpp
    Parser.cpp
    RenderData.cpp
)

# Create alias for consistent usage in both build and install
add_library(tmxparser::tmxparser ALIAS tmxparser)

target_include_directories(tmxparser 
    PUBLIC 
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(tmxparser 
    PUBLIC
        tl::expected
        pugixml
        base64
        ZLIB::ZLIB
        libzstd_static
)

target_compile_features(tmxparser PUBLIC cxx_std_23)
