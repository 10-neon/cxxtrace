if(ANDROID)
    set(TARGET_PLATFORM "android")
    set(PLATFORM_ANDROID 1)
elseif(IOS)
    set(TARGET_PLATFORM "ios")
    set(PLATFORM_IOS 1)
elseif(APPLE)
    set(TARGET_PLATFORM "macos")
    set(PLATFORM_MAC 1)
elseif(WIN32)
    set(TARGET_PLATFORM "windows")
    set(PLATFORM_WIN 1)
elseif(UNIX)
    set(TARGET_PLATFORM "linux")
    set(PLATFORM_LINUX 1)
else()
    warn("Unknown platform")
endif()

add_compile_definitions(
    $<$<BOOL:${PLATFORM_ANDROID}>:PLATFORM_ANDROID>
    $<$<BOOL:${PLATFORM_IOS}>:PLATFORM_IOS>
    $<$<BOOL:${PLATFORM_MAC}>:PLATFORM_MAC>
    $<$<BOOL:${PLATFORM_WIN}>:PLATFORM_WIN>
    $<$<BOOL:${PLATFORM_LINUX}>:PLATFORM_LINUX>
)

info("Target platform: ${TARGET_PLATFORM}")
