add_library(mallochook STATIC)
target_include_directories(mallochook PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set(MALLOCHOOK_LINUX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/plthook_elf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/plthook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook_linux.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook_disable_guard_posix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook_disable_guard.h
)

set(MALLOCHOOK_OSX_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook_osx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook_disable_guard_posix.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook_disable_guard.h
)

set(MALLOCHOOK_WIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/plthook_win32.c ${CMAKE_CURRENT_SOURCE_DIR}/impl/plthook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/impl/malloc_hook.cpp ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook.h
    ${CMAKE_CURRENT_SOURCE_DIR}/malloc_hook_disable_guard.h
)

# 使用生成表达式为不同平台添加源文件
target_sources(
    mallochook
    PRIVATE $<$<BOOL:${PLATFORM_ANDROID}>:${MALLOCHOOK_LINUX_SOURCES}>
            $<$<BOOL:${PLATFORM_LINUX}>:${MALLOCHOOK_LINUX_SOURCES}>
            $<$<BOOL:${PLATFORM_IOS}>:${MALLOCHOOK_OSX_SOURCES}>
            $<$<BOOL:${PLATFORM_MAC}>:${MALLOCHOOK_OSX_SOURCES}>
            $<$<BOOL:${PLATFORM_WIN}>:${MALLOCHOOK_WIN_SOURCES}>
)
