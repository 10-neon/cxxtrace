add_library(thread_info STATIC)
target_include_directories(thread_info PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set(LINUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/thread_info.h
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/linux/thread_info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/linux/perf_event.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/linux/perf_event.h
)

set(APPLE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/thread_info.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/impl/mac/thread_info.cpp
)

# 使用生成表达式为不同平台添加源文件
target_sources(
    thread_info
    PRIVATE $<$<BOOL:${PLATFORM_ANDROID}>:${LINUX_SOURCES}>
            $<$<BOOL:${PLATFORM_IOS}>:${APPLE_SOURCES}>
            $<$<BOOL:${PLATFORM_MAC}>:${APPLE_SOURCES}>
            $<$<BOOL:${PLATFORM_WIN}>:impl/win/win.cpp>
            $<$<BOOL:${PLATFORM_LINUX}>:${LINUX_SOURCES}>
)
target_link_libraries(thread_info PRIVATE mallochook)
