add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    mocks/trading_reply_receiver_mock.hpp
    test_utils/inline_service.hpp
  UNIT_TESTS
    unit_tests/market_data/quote_streamer_tests.cpp
    unit_tests/orders/order_rejecter_tests.cpp
    unit_tests/quotes/quote_book_tests.cpp
    unit_tests/quotes/quote_validator_tests.cpp
    unit_tests/quoting_engine_tests.cpp)
