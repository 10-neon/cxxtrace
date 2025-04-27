set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${PROJECT_SOURCE_DIR}/cmake/CPM.cmake)
include(${PROJECT_SOURCE_DIR}/cmake/platform.cmake)

add_custom_target(version
        COMMAND ${CMAKE_COMMAND} -D SRC=${PROJECT_SOURCE_DIR}/cmake/version.h.in
                         -D DST=${PROJECT_BINARY_DIR}/include/version.h
                         -P ${PROJECT_SOURCE_DIR}/cmake/version.cmake
        COMMENT "Generate version.h")
include_directories(${PROJECT_BINARY_DIR}/include)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
info("finish post project cmake")
