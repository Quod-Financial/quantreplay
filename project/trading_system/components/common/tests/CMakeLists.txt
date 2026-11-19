add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    mocks/security_status_reply_sender_mock.hpp
    test_utils/matchers.hpp
  UNIT_TESTS
    unit_tests/checker_utils_tests.cpp
    unit_tests/events_tests.cpp
    unit_tests/instrument_state_tests.cpp
    unit_tests/instrument_tests.cpp
    unit_tests/market_data_rejects_tests.cpp
    unit_tests/order_rejects_tests.cpp
    unit_tests/phase_tests.cpp
    unit_tests/security_status_publisher_tests.cpp
    unit_tests/trade_tests.cpp
    unit_tests/validation_tests.cpp)