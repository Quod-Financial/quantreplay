add_target_tests(
  TARGET ${PROJECT_NAME}
  HEADERS
    mocks/market_data_request_receiver_mock.hpp
  UNIT_TESTS
    unit_tests/command_options_tests.cpp
    unit_tests/datasource_settings_files_tests.cpp
    unit_tests/dispatchers/market_data_request_dispatcher_tests.cpp)