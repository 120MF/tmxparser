find_package(ZSTD QUIET)

if (NOT ZSTD_FOUND)
    FetchContent_Declare(
            ZSTD_fetched
            GIT_REPOSITORY https://github.com/facebook/zstd.git
            GIT_TAG "dev"
            GIT_SHALLOW TRUE
            SOURCE_SUBDIR build/cmake
    )
    FetchContent_MakeAvailable(ZSTD_fetched)
endif ()