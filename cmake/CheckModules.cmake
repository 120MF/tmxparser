include(FetchContent)

include(Modules/FindPugixml)
include(Modules/FindTlExpected)
include(Modules/FindBase64)
include(Modules/Findlibzstd)
include(Modules/FindZLIB)

if (BUILD_TMX_EXAMPLES)
    include(Modules/FindSDL3)
    include(Modules/FindSTB)
endif ()