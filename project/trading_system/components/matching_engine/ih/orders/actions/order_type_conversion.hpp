#ifndef SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_TYPE_CONVERSION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_TYPE_CONVERSION_HPP_

#include <optional>
#include <string_view>

#include "common/attributes.hpp"
#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event_reporter.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/book/order_updates.hpp"

namespace simulator::trading_system::matching_engine {

constexpr std::string_view TimeInForceNotAllowedReject =
    "time in force is not allowed";

constexpr std::string_view TimeInForceMissingReject = "time in force missing";

template <typename Order>
struct ConvertedOrder {
  Order order;
  OrderBookUpdate removal;
};

// Takes the resting order an amendment targets out of the book and returns it
// as an order of the amended type.
class OrderTypeConversion : private EventReporter {
 public:
  OrderTypeConversion(EventListener& event_listener,
                      std::optional<PriceTick> price_tick);

  OrderTypeConversion(const OrderTypeConversion&) = default;
  OrderTypeConversion(OrderTypeConversion&&) = default;
  ~OrderTypeConversion() override = default;

  auto operator=(const OrderTypeConversion&) -> OrderTypeConversion& = delete;
  auto operator=(OrderTypeConversion&&) -> OrderTypeConversion& = delete;

  [[nodiscard]]
  auto to_market_order(MarketUpdate update, LimitOrdersContainer& orders)
      -> std::optional<ConvertedOrder<MarketOrder>>;

  [[nodiscard]]
  auto to_auction_limit_order(LimitUpdate update, MarketOrdersContainer& orders)
      -> std::optional<ConvertedOrder<LimitOrder>>;

 private:
  template <typename ConvertedType, typename UpdateType, typename ContainerType>
  auto convert(UpdateType update, ContainerType& orders)
      -> std::optional<ConvertedOrder<ConvertedType>>;

  std::optional<PriceTick> price_tick_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_ORDERS_ACTIONS_ORDER_TYPE_CONVERSION_HPP_
