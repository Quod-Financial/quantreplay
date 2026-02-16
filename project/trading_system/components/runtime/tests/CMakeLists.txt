add_target_tests(
  TARGET ${COMPONENT_NAME}
  UNIT_TESTS
    unit_tests/chained_mux_tests.cpp
    unit_tests/one_second_rate_loop_tests.cpp
    unit_tests/simple_thread_pool_tests.cpp
  DEPENDENCIES
    fmt::fmt)