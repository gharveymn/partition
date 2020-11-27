SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_EXECUTABLE(test_list_partition main.cpp)
TARGET_LINK_LIBRARIES(test_list_partition partition)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  TARGET_COMPILE_OPTIONS(test_list_partition PUBLIC "-stdlib=libc++" "-ftime-trace")
  TARGET_LINK_OPTIONS(test_list_partition PUBLIC "-Wl,--allow-multiple-definition")
  TARGET_LINK_LIBRARIES(test_list_partition -lc++ -lc++abi)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

SET_TARGET_PROPERTIES(test_list_partition PROPERTIES
                      CXX_STANDARD          11
                      CXX_STANDARD_REQUIRED NO
                      CXX_EXTENSIONS        NO)
