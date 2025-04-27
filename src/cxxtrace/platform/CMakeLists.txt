add_library(platform_impl STATIC)
target_include_directories(platform_impl
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
# 使用生成表达式为不同平台添加源文件
target_sources(platform_impl PRIVATE
    $<$<PLATFORM_ID:Android>:impl/android/android.cpp>
    $<$<PLATFORM_ID:iOS>:impl/ios/ios.cpp>
    $<$<PLATFORM_ID:Darwin>:impl/mac/mac.cpp>
    $<$<PLATFORM_ID:Windows>:impl/win/win.cpp>
    $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<PLATFORM_ID:Android>>>:impl/linux/linux.cpp>
)

if (BUILD_TESTING)
    add_subdirectory(unittest)
endif()
