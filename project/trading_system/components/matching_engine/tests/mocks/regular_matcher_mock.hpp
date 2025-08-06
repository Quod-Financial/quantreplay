#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_REGULAR_MATCHER_MOCK_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_REGULAR_MATCHER_MOCK_HPP_

#include <gmock/gmock.h>

#include "ih/orders/matchers/regular_order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

class RegularMatcherMock : public RegularMatcher {
 public:
  MOCK_METHOD(void, match, (LimitOrder&), (override));
  MOCK_METHOD(void, match, (MarketOrder&), (override));
  MOCK_METHOD(bool, has_facing_orders, (const LimitOrder&), (override));
  MOCK_METHOD(bool, has_facing_orders, (const MarketOrder&), (override));
  MOCK_METHOD(bool, can_fully_trade, (const LimitOrder&), (override));
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_REGULAR_MATCHER_MOCK_HPP_
