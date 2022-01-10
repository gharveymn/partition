cmake_minimum_required (VERSION 3.15)

project (
  partition
  VERSION
    0.0.2
  LANGUAGES
    CXX
)

# Options
string (COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" _EXTRAS_DEFAULT)

option (
  GCH_PARTITION_BUILD_TESTS
  "Set to ON to build tests for gch::partition."
  ${_EXTRAS_DEFAULT}
)

include (CMakeDependentOption)
cmake_dependent_option (
  GCH_USE_LIBCXX_WITH_CLANG
  "Set to ON to use libc++ when compiling with Clang."
  OFF
  "CMAKE_CXX_COMPILER_ID MATCHES Clang"
  OFF
)

if (GCH_PARTITION_BUILD_TESTS)
  enable_testing ()
endif ()

add_subdirectory (source)

export (
  EXPORT
    partition-targets
  NAMESPACE
    gch::
)
