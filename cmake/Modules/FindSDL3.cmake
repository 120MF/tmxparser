# Try to find SDL3 from system first
find_package(PkgConfig QUIET)
pkg_check_modules(SDL3 QUIET sdl3)

if (NOT SDL3_FOUND)
    # If not found on system, fetch from git
    message(STATUS "SDL3 not found on system, fetching from Git...")
    
    include(FetchContent)
    FetchContent_Declare(
        SDL3
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    
    # Configure SDL3 options
    set(SDL_SHARED OFF CACHE BOOL "Build SDL as a shared library")
    set(SDL_STATIC ON CACHE BOOL "Build SDL as a static library")
    set(SDL_TEST OFF CACHE BOOL "Build SDL test programs")
    
    FetchContent_MakeAvailable(SDL3)
    
    # Create an alias for consistency
    if (NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 ALIAS SDL3-static)
    endif()
else()
    # If found on system, create imported target
    if (NOT TARGET SDL3::SDL3)
        add_library(SDL3::SDL3 INTERFACE IMPORTED)
        set_target_properties(SDL3::SDL3 PROPERTIES
            INTERFACE_LINK_LIBRARIES "${SDL3_LIBRARIES}"
            INTERFACE_INCLUDE_DIRECTORIES "${SDL3_INCLUDE_DIRS}"
            INTERFACE_COMPILE_OPTIONS "${SDL3_CFLAGS_OTHER}"
        )
    endif()
endif()