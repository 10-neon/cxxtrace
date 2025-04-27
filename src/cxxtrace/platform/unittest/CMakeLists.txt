add_executable(platform_unittest
    platform_unittest.cpp
)

target_link_libraries(platform_unittest
    PRIVATE
    gtest_main
    platform_impl
)

target_include_directories(platform_unittest
    PRIVATE
    ${PROJECT_SOURCE_DIR}/src/cxxtrace/platform
)


gtest_discover_tests(platform_unittest)
info("platform_unittest")
