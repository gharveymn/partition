SET(CMAKE_EXPORT_COMPILE_COMMANDS ON)

ADD_EXECUTABLE(test_partition main.cpp)
TARGET_LINK_LIBRARIES(test_partition partition)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  TARGET_COMPILE_OPTIONS(test_partition PUBLIC -stdlib=libc++ -ftime-trace --coverage -Wall -Wextra)
  TARGET_LINK_OPTIONS(test_partition PUBLIC -Wl,--allow-multiple-definition --coverage)
  TARGET_LINK_LIBRARIES(test_partition c++ c++abi)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  TARGET_COMPILE_OPTIONS(test_partition PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  TARGET_COMPILE_OPTIONS(test_partition PUBLIC /W4)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

SET_TARGET_PROPERTIES(test_partition PROPERTIES
                      CXX_STANDARD          11
                      CXX_STANDARD_REQUIRED NO
                      CXX_EXTENSIONS        NO)

ADD_SUBDIRECTORY(list)
