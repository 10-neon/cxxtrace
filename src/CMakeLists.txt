add_library(cxxtrace 
  cxxtrace.cpp
  detail/process_info.cpp
  detail/thread_info.cpp
  detail/mac/process_info.cpp
  detail/mac/thread_info.cpp
  detail/mac/malloc_hook.cpp
  detail/mac/malloc_hook.h
  )

target_include_directories(cxxtrace PUBLIC
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
