find_package(pugixml QUIET)

if (NOT pugixml_FOUND)
    FetchContent_Declare(
            pugixml_fetched
            GIT_REPOSITORY https://github.com/zeux/pugixml.git
            GIT_TAG "master"
            GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(pugixml_fetched)
endif ()