SET(REQUIRED_CREATE_PACKAGE_VERSION 0.0.1)
IF(NOT HAS_CREATE_PACKAGE OR
   NOT CREATE_PACKAGE_VERSION VERSION_GREATER_EQUAL REQUIRED_CREATE_PACKAGE_VERSION)
  MESSAGE(STATUS "Initializing create-package-${REQUIRED_CREATE_PACKAGE_VERSION} in git submodule.")
  EXECUTE_PROCESS(COMMAND git submodule --quiet update --init -- "external/create-package"
                  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}")
  INCLUDE("${PROJECT_SOURCE_DIR}/external/create-package/cmake/create-package.cmake")
ENDIF()

CREATE_PACKAGE(AUTHOR         gharveymn
               NAMESPACE      gch
               NAME           partition
               HEADERS        partition.hpp
                              list_partition.hpp)

# Only include tests if we are at the top level
IF(PARTITION_BUILD_TESTS)
  MESSAGE(STATUS "Testing is enabled.")
  ADD_SUBDIRECTORY(test)
ELSE()
  MESSAGE(STATUS "Testing is disabled.")
ENDIF()
