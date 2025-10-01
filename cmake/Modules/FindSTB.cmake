# Find or fetch stb single-file libraries
include(FetchContent)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(stb)

# Create interface library for stb_image
if (NOT TARGET stb::image)
    add_library(stb_image INTERFACE)
    target_include_directories(stb_image INTERFACE ${stb_SOURCE_DIR})
    add_library(stb::image ALIAS stb_image)
endif()
