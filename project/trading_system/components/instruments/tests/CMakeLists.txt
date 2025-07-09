add_target_tests(
  TARGET ${COMPONENT_NAME}
  HEADERS
    test_utils/utils.hpp
  API_TESTS
    api_tests/cache_tests.cpp
    api_tests/matcher_tests.cpp
  UNIT_TESTS
    unit_tests/currency_category_tests.cpp
    unit_tests/instruments_cache_test.cpp
    unit_tests/instruments_container_test.cpp
    unit_tests/instruments_matcher_tests.cpp
    unit_tests/lookup_tests.cpp
    unit_tests/match_rate_tests.cpp
    unit_tests/matchers_tests.cpp
    unit_tests/sources_test.cpp
    unit_tests/strategies_tests.cpp)