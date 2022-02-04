add_library (partition INTERFACE)

target_sources (
  partition
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/partition/dependent_partition.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/partition/list_partition.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/partition/partition.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include/gch/partition/vector_partition.hpp>
)

target_include_directories (
  partition
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_include_directories (
  partition
  SYSTEM INTERFACE
    $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>
)

add_library (gch::partition ALIAS partition)

install (
  TARGETS
    partition
  EXPORT
    partition-targets
)

install (
  DIRECTORY
    include/gch
  DESTINATION
    include
)

file (WRITE ${CMAKE_CURRENT_BINARY_DIR}/partition-config.cmake [=[
get_filename_component (PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)
include (CMakeFindDependencyMacro)
find_dependency (plf_list)
include ("${CMAKE_CURRENT_LIST_DIR}/partition-targets.cmake")
]=])

include (CMakePackageConfigHelpers)
write_basic_package_version_file (
  partition-version.cmake
  COMPATIBILITY
    SameMinorVersion
)

install (
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/partition-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/partition-version.cmake
  DESTINATION
    lib/cmake/partition
)

install (
  EXPORT
    partition-targets
  DESTINATION
    lib/cmake/partition
  NAMESPACE
    gch::
)

if (GCH_PARTITION_ENABLE_TESTS)
  add_subdirectory (test)
endif ()
