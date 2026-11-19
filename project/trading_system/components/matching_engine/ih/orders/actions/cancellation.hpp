#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_

#include <gsl/pointers>
#include <optional>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

class Cancellation : private EventReporter {
 public:
  Cancellation(EventListener& event_listener,
               OrderBook& order_book,
               const core::PriceTickRanges& price_tick_ranges,
               LimitOrderQueue queue);

  Cancellation(const Cancellation&) = default;
  Cancellation(Cancellation&&) = default;
  ~Cancellation() override = default;

  auto operator=(const Cancellation&) -> Cancellation& = delete;
  auto operator=(Cancellation&&) -> Cancellation& = delete;

  auto operator()(const OrderCancel& cancel) -> OrderBookUpdates;

 private:
  auto cancel_order(const OrderCancel& cancel, OrderPage& page)
      -> OrderBookUpdates;

  template <typename Container>
  auto try_cancel(const OrderCancel& cancel, Container& orders)
      -> std::optional<OrderBookUpdate>;

  OrderBook& order_book_;
  gsl::not_null<const core::PriceTickRanges*> price_tick_ranges_;
  LimitOrderQueue queue_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_CANCELLATION_HPP_
