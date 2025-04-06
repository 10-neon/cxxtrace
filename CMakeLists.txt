cmake_minimum_required(VERSION 3.20)

# Set a name and a version number for your project:
project(cxxtrace VERSION 0.0.1 LANGUAGES CXX)

# Initialize some default paths
include(GNUInstallDirs)

# Define the minimum C++ standard that is required
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Compilation options
option(BUILD_DOCS "Enable building of documentation" ON)
option(BUILD_TESTING "Enable building of unittest" ON)

# compile the library
add_subdirectory(src)

# compile the example
add_subdirectory(example)

# compile the tests
include(CTest)
if(BUILD_TESTING)
  add_subdirectory(ext/Catch2)
  include(./ext/Catch2/contrib/Catch.cmake)
  add_subdirectory(tests)
endif()

if(BUILD_DOCS)
  # Add the documentation
  add_subdirectory(doc)
endif()

# Add an alias target for use if this project is included as a subproject in another project
add_library(cxxtrace::cxxtrace ALIAS cxxtrace)

# Install targets and configuration
install(
  TARGETS cxxtrace
  EXPORT cxxtrace-config
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  EXPORT cxxtrace-config
  NAMESPACE cxxtrace::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cxxtrace
)

install(
  DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# This prints a summary of found dependencies
include(FeatureSummary)
feature_summary(WHAT ALL)
