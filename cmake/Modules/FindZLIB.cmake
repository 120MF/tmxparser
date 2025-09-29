find_package(ZLIB QUIET)

if (NOT ZLIB_FOUND)
    FetchContent_Declare(
            ZLIB_fetched
            GIT_REPOSITORY https://github.com/madler/zlib.git
            GIT_TAG "dev"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(ZLIB_fetched)
endif ()
