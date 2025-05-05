set(TARGET_NAME cxxtrace)
add_subdirectory(hook)
add_subdirectory(platform)

add_library(
    ${TARGET_NAME} STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/cxxtrace.cpp ${CMAKE_CURRENT_SOURCE_DIR}/location.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/structlog.cpp ${CMAKE_CURRENT_SOURCE_DIR}/structlog.h
)

target_include_directories(${TARGET_NAME} PUBLIC ${SRC_ROOT}/include PRIVATE ${SRC_ROOT}/src)

cpmaddpackage(
    NAME expected GITHUB_REPOSITORY TartanLlama/expected GIT_TAG v1.1.0 OPTIONS
    "EXPECTED_BUILD_TESTS OFF"
)

cpmaddpackage(
    NAME optional GITHUB_REPOSITORY TartanLlama/optional GIT_TAG v1.1.0 OPTIONS
    "OPTIONAL_BUILD_TESTS OFF"
)

cpmaddpackage(
    NAME nlohmann_json GIT_TAG v3.12.0 GITHUB_REPOSITORY nlohmann/json OPTIONS
    "JSON_BuildTests OFF"
)

cpmaddpackage(GITHUB_REPOSITORY fmtlib/fmt GIT_TAG 11.1.4 OPTIONS "FMT_TEST OFF" "FMT_FUZZ OFF")
cpmaddpackage(
    GITHUB_REPOSITORY gabime/spdlog GIT_TAG v1.15.2 OPTIONS "SPDLOG_FMT_EXTERNAL 1"
    "SPDLOG_NO_EXCEPTIONS 1"
)

target_link_libraries(${TARGET_NAME} PUBLIC tl::expected tl::optional)

target_link_libraries(${TARGET_NAME} PUBLIC thread_info fmt spdlog nlohmann_json::nlohmann_json)
add_dependencies(${TARGET_NAME} version)
