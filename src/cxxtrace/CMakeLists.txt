set(TARGET_NAME cxxtrace)
add_subdirectory(platform)

add_library(
    ${TARGET_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/cxxtrace.cpp
)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC
    ${SRC_ROOT}/include
    PRIVATE
    ${SRC_ROOT}/src
)

CPMAddPackage(
    NAME expected
    GITHUB_REPOSITORY TartanLlama/expected
    GIT_TAG v1.1.0
    OPTIONS "EXPECTED_BUILD_TESTS OFF"
)

CPMAddPackage(
    NAME optional
    GITHUB_REPOSITORY TartanLlama/optional
    GIT_TAG v1.1.0
    OPTIONS "OPTIONAL_BUILD_TESTS OFF"
)

target_link_libraries(${TARGET_NAME} PUBLIC tl::expected tl::optional)

target_link_libraries(${TARGET_NAME} PRIVATE platform_impl)
add_dependencies(${TARGET_NAME} version)
