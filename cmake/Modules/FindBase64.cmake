FetchContent_Declare(
        base64_fetched
        GIT_REPOSITORY https://github.com/aklomp/base64.git
        GIT_TAG "master"
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(base64_fetched)