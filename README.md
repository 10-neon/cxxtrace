Language: [English](README_en.md) | [中文](README.md)


# C++ Cross Platform Template (WIP)
It's not ready for use yet.

cmake >= 3.25
## Feature
- [x] pre-commit-check
- [x] commit-msg-check
- [x] clang-format
- [x] git tag version (https://semver.org/)
- [x] ccache
- [x] multi platform [cmake-preset](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)
- [x] dependence manage ([cpm](https://github.com/cpm-cmake/CPM.cmake))
- [ ] unittest (gtest)
- [ ] documentation (doxygen? mkdocs? sphinx?)
- [ ] module ut and all in one ut
- [ ] cmake module.cmake
- [ ] cmake target.cmake
- [ ] Docker (dev container)
- [ ] cookiecutter

[cpm](https://github.com/cpm-cmake/CPM.cmake)
[cmake-preset](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html)


需要env{ANDROID_NDK}
## Installation

```bash
setup.sh
```

Linux
brew install llvm uncrustify cppcheck include-what-you-use
MacOS
brew install llvm uncrustify cppcheck include-what-you-use
Windows
choco install llvm uncrustify cppcheck inlcude-what-you-use
Cross-platform
pipx install clang-format


cmake --preset macos -G Ninja --fresh
cmake --build --preset macos
ctest --preset macos --test-dir build/macos/unittest
