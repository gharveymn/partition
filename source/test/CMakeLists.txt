SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

SET(TEST_NAME partition)
SET(SOURCE_FILES main.cpp)

SET(TARGET_VERSIONS 11 14 17 20)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(COMPILE_OPTS -stdlib=libc++ -ftime-trace --coverage -Wall -Wextra)
  SET(LINK_OPTS -Wl,--allow-multiple-definition --coverage)
  SET(LINK_LIBS c++ c++abi)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  SET(COMPILE_OPTS -Wall -Wextra)
  SET(LINK_OPTS )
  SET(LINK_LIBS )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  SET(COMPILE_OPTS /W4)
  SET(LINK_OPTS )
  SET(LINK_LIBS )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  SET(COMPILE_OPTS )
  SET(LINK_OPTS )
  SET(LINK_LIBS )
endif()

FOREACH(version ${TARGET_VERSIONS})
  SET(target_name ${TEST_NAME}.test.c++${version})
  ADD_EXECUTABLE(${target_name} ${SOURCE_FILES})

  TARGET_COMPILE_OPTIONS(${target_name} PUBLIC ${COMPILE_OPTS})
  TARGET_LINK_OPTIONS   (${target_name} PUBLIC ${LINK_OPTS})
  TARGET_LINK_LIBRARIES (${target_name} ${PACKAGE_NAME} ${LINK_LIBS})

  SET_TARGET_PROPERTIES(${target_name} PROPERTIES
                        CXX_STANDARD          ${version}
                        CXX_STANDARD_REQUIRED NO
                        CXX_EXTENSIONS        NO)
ENDFOREACH()

ADD_SUBDIRECTORY(list)
