# Welcome to CXXTrace

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation Status](https://readthedocs.org/projects/None/badge/)](https://None.readthedocs.io/)


# Prerequisites

Building CXXTrace requires the following software installed:

* A C++14-compliant compiler
* CMake `>= 3.9`
* Doxygen (optional, documentation building is skipped if missing)

# Building CXXTrace

The following sequence of commands builds CXXTrace.
It assumes that your current working directory is the top-level directory
of the freshly cloned repository:

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

The build process can be customized with the following CMake variables,
which can be set by adding `-D<var>={ON, OFF}` to the `cmake` call:

* `BUILD_TESTING`: Enable building of the test suite (default: `ON`)
* `BUILD_DOCS`: Enable building the documentation (default: `ON`)



# Testing CXXTrace

When built according to the above explanation (with `-DBUILD_TESTING=ON`),
the C++ test suite of `CXXTrace` can be run using
`ctest` from the build directory:

```
cd build
ctest
```


# Documentation

CXXTrace provides a Sphinx-based documentation, that can
be browsed [online at readthedocs.org](https://cxxtrace.readthedocs.io).
To build it locally, first ensure the requirements are installed by running this command from the top-level source directory:

```
pip install -r doc/requirements.txt
```

Then build the sphinx documentation from the top-level build directory:

```
cmake --build . --target sphinx-doc
```

The web documentation can then be browsed by opening `doc/sphinx/index.html` in your browser.
