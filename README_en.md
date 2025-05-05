Language: [English](README_en.md) | [中文](README.md)

# CXXTrace - Intrusive, Lightweight C++ Performance Tracing

## Features
- [x] Intrusive: Can correlate business information with performance overhead
- [x] Lightweight: Can be enabled in production with much lower overhead than normal profiling
- [x] Visualization: Provides an HTML file as visualization UI with no other dependencies
- [x] Easy Integration: Statically link this library to take effect. Useful in scenarios where LD_PRELOAD cannot be used
- [x] Supports memory and CPU metrics: task-clock, alloc-bytes, dealloc-bytes, duration
- [x] Supports multiple platforms: Linux, Android, MacOS, iOS (Windows support planned but not yet completed)
- [x] Provides both object and scope tracing: One line of code to trace performance overhead of all calls on a C++ object. Also supports scope-based overhead statistics

## TODO
- [ ] Count memory allocations in dynamically linked libraries that don't statically link this library
- [ ] Optimize trace format: 1. Switch to flatbuffer 2. Use optimization techniques similar to [neonlog](https://github.com/PlatformLab/NanoLog)
- [ ] Implement disk writing: Plan to reference java fqueue, batch writing, double buffering etc.
- [ ] Optimize existing code
- [ ] Add cmake installation, support cpm installation
- [ ] Add unit tests
- [ ] Add documentation

## Status
Written during May Day holiday with limited time, so implementation is rough.

Currently supports Linux, Android, MacOS, iOS, but implementation is still crude.

Memory metrics on Linux/Android can only count overhead in binaries that statically link this library.

Most features not supported on Windows.
