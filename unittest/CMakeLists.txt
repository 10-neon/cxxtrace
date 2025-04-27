
add_executable(unittest
    cxx_project_name_unittest.cpp
)

target_link_libraries(unittest gtest_main gmock)

gtest_discover_tests(unittest)
