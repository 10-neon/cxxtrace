include(${SRC_ROOT}/cmake/message.cmake)
info("current cmake version: ${CMAKE_VERSION}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set CMAKE_BUILD_TYPE
set(CMAKE_CONFIGURATION_TYPES
    "Debug;Release"
    CACHE STRING "Configs" FORCE
)

if (NOT CMAKE_BUILD_TYPE)
    info("'Debug' build type is used by default. Use CMAKE_BUILD_TYPE to specify build type (Release or Debug)")
    set(CMAKE_BUILD_TYPE Debug)
endif ()
