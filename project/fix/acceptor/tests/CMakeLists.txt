add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    mocks/event_processor_mock.hpp
    mocks/from_fix_mapper_stub.hpp
    mocks/reply_sender_mock.hpp
    mocks/request_processor_mock.hpp
    mocks/to_fix_mapper_stub.hpp
    mocks/trading_request_receiver_mock.hpp
  UNIT_TESTS
    unit_tests/communicators/application_tests.cpp
    unit_tests/mapping/from_fix_mapper_tests.cpp
    unit_tests/mapping/to_fix_mapper_tests.cpp
    unit_tests/processors/app_reply_processor_tests.cpp
    unit_tests/processors/app_request_processor_tests.cpp)