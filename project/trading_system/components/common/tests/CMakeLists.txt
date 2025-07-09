add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    test_utils/matchers.hpp
  UNIT_TESTS
    unit_tests/market_state/instrument_info_tests.cpp
    unit_tests/market_state/instrument_state_tests.cpp
    unit_tests/market_state/limit_order_tests.cpp
    unit_tests/market_state/order_book_tests.cpp
    unit_tests/market_state/session_tests.cpp
    unit_tests/market_state/session_type_tests.cpp
    unit_tests/market_state/snapshot_tests.cpp
    unit_tests/events_tests.cpp
    unit_tests/instrument_test.cpp
    unit_tests/phase_test.cpp
    unit_tests/trade_tests.cpp)