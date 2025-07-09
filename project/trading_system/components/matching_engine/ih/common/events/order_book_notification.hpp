#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_ORDER_BOOK_NOTIFICATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_ORDER_BOOK_NOTIFICATION_HPP_

#include <fmt/base.h>

#include <concepts>
#include <utility>
#include <variant>

#include "common/trade.hpp"
#include "ih/common/data/market_data_updates.hpp"

namespace simulator::trading_system::matching_engine {

struct OrderBookNotification {
  using Value = std::variant<OrderAdded,
                             OrderReduced,
                             OrderRemoved,
                             Trade,
                             LastTradeRecover,
                             InstrumentInfoRecover>;

  template <typename NotificationType>
    requires std::constructible_from<Value, NotificationType>
  explicit OrderBookNotification(NotificationType&& notification)
      : value(std::forward<NotificationType>(notification)) {}

  Value value;
};

}  // namespace simulator::trading_system::matching_engine

template <>
struct fmt::formatter<
    simulator::trading_system::matching_engine::OrderBookNotification> {
  using formattable =
      simulator::trading_system::matching_engine::OrderBookNotification;

  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const formattable& notification, FormatContext& ctx) const {
    return std::visit(
        [&ctx](const auto& value) { return format_to(ctx.out(), "{}", value); },
        notification.value);
  }
};

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_ORDER_BOOK_NOTIFICATION_HPP_
