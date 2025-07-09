#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_

#include <gmock/gmock.h>

#include "ih/common/events/event.hpp"
#include "ih/common/events/order_book_notification.hpp"

namespace simulator::trading_system::matching_engine {

MATCHER_P(IsOrderBookNotification, matcher, "") {
  return ExplainMatchResult(Field(&Event::value,
                                  VariantWith<OrderBookNotification>(Field(
                                      &OrderBookNotification::value, matcher))),
                            arg,
                            result_listener);
}

MATCHER_P(IsClientNotification, matcher, "") {
  return ExplainMatchResult(Field(&Event::value,
                                  VariantWith<ClientNotification>(Field(
                                      &ClientNotification::value, matcher))),
                            arg,
                            result_listener);
}

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_MATCHERS_HPP_
