#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_AMENDMENT_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_AMENDMENT_HPP_

#include <gsl/pointers>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/matchers/order_matcher.hpp"

namespace simulator::trading_system::matching_engine {

class RegularAmendment : private EventReporter {
 public:
  RegularAmendment(EventListener& event_listener,
                   OrderBook& order_book,
                   RegularMatcher& matcher,
                   const core::PriceTickRanges& price_tick_ranges,
                   LimitOrderQueue queue);

  RegularAmendment(const RegularAmendment&) = default;
  RegularAmendment(RegularAmendment&&) = default;
  ~RegularAmendment() override = default;

  auto operator=(const RegularAmendment&) -> RegularAmendment& = delete;
  auto operator=(RegularAmendment&&) -> RegularAmendment& = delete;

  auto operator()(LimitUpdate update) -> OrderBookUpdates;

 private:
  auto amend_order(LimitUpdate update, LimitOrdersContainer& orders)
      -> OrderBookUpdates;

  OrderBook& order_book_;
  RegularMatcher& matcher_;
  gsl::not_null<const core::PriceTickRanges*> price_tick_ranges_;
  LimitOrderQueue queue_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_REGULAR_AMENDMENT_HPP_
