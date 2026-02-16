add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    test_utils/matchers.hpp
  UNIT_TESTS
    unit_tests/events_tests.cpp
    unit_tests/instrument_state_tests.cpp
    unit_tests/instrument_tests.cpp
    unit_tests/phase_tests.cpp
    unit_tests/trade_tests.cpp)