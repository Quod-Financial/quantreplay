add_target_tests(
  TARGET ${COMPONENT_NAME}
  UNIT_TESTS
    unit_tests/chained_mux_test.cpp
    unit_tests/one_second_rate_loop_test.cpp
    unit_tests/simple_thread_pool_test.cpp
  DEPENDENCIES
    fmt::fmt)