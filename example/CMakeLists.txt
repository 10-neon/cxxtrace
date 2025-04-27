set(EXAMPLE_TARGET_NAME "cxxtrace_example")
add_executable(${EXAMPLE_TARGET_NAME} example.cpp)
target_link_libraries(${EXAMPLE_TARGET_NAME} PRIVATE cxxtrace)
