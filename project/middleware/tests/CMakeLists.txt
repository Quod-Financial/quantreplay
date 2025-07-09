add_target_tests(
  TARGET ${PROJECT_NAME}
  HEADERS
    mocks/generator_admin_receiver_mock.hpp
    mocks/trading_admin_receiver_mock.hpp
    mocks/trading_reply_receiver_mock.hpp
    mocks/trading_request_receiver_mock.hpp
    mocks/trading_session_event_listener_mock.hpp
    test_utils/protocol_utils.hpp
  UNIT_TESTS
    unit_tests/generator_admin_channel_tests.cpp
    unit_tests/trading_admin_channel_tests.cpp
    unit_tests/trading_reply_channel_tests.cpp
    unit_tests/trading_request_channel_tests.cpp
    unit_tests/trading_session_event_channel_tests.cpp)