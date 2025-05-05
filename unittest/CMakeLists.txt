add_executable(unittest ${CMAKE_CURRENT_SOURCE_DIR}/cxx_project_name_unittest.cpp)

target_link_libraries(unittest PRIVATE gtest_main gmock)

# gtest_discover_tests(unittest)
