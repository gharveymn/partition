add_unit_test (test_list_partition main.cpp)
set_target_properties (
  test_list_partition
  PROPERTIES
  CXX_STANDARD
    11
  CXX_STANDARD_REQUIRED
    NO
  CXX_EXTENSIONS
    NO
)
