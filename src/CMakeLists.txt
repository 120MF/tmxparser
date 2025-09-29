add_library(tmxparser STATIC
    Map.cpp
    Parser/Parser.cpp
    Render/Render.cpp
)

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
