add_target_tests(
  TARGET ${COMPONENT_NAME}
  API_TESTS
    api_tests/api_test.cpp
  UNIT_TESTS
    unit_tests/generation/execution_id_generation_tests.cpp
    unit_tests/generation/instrument_id_generation_tests.cpp
    unit_tests/generation/market_entry_id_generation_tests.cpp
    unit_tests/generation/order_id_generation_tests.cpp
    unit_tests/sequences/chrono_sequence_tests.cpp
    unit_tests/sequences/numeric_sequence_tests.cpp
  DEPENDENCIES
    simulator::cfg)