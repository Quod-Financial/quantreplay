#include "ih/orders/actions/order_type_conversion.hpp"

#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include "ih/common/events/client_notification.hpp"
#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/replies/modification_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "ih/orders/tools/order_lookup.hpp"

namespace simulator::trading_system::matching_engine {
namespace {

constexpr std::string_view InvalidQuantityReject = "invalid quantity";

[[nodiscard]]
auto conversion_error(const MarketUpdate& update, const LimitOrder& order)
    -> std::optional<std::string_view> {
  if (!is_amendable_quantity(update.order_diff.quantity,
                             order.cum_executed_quantity())) {
    return InvalidQuantityReject;
  }
  if (update.time_in_force_specified) {
    return TimeInForceNotAllowedReject;
  }
  return std::nullopt;
}

[[nodiscard]]
auto conversion_error(const LimitUpdate& update, const MarketOrder& order)
    -> std::optional<std::string_view> {
  if (!is_amendable_quantity(update.order_diff.quantity,
                             order.cum_executed_quantity())) {
    return InvalidQuantityReject;
  }
  if (!update.time_in_force_specified) {
    return TimeInForceMissingReject;
  }
  return auction::acceptance_error(
      update.order_diff.attributes.time_in_force());
}

[[nodiscard]]
auto find_target(LimitOrdersContainer& orders, const MarketUpdate& update)
    -> LimitOrdersContainer::iterator {
  return find_target_limit_order(orders, update);
}

[[nodiscard]]
auto find_target(MarketOrdersContainer& orders, const LimitUpdate& update)
    -> MarketOrdersContainer::iterator {
  return find_target_market_order(orders, update);
}

[[nodiscard]]
auto book_price(const LimitOrder& order) -> std::optional<OrderPrice> {
  return order.price();
}

[[nodiscard]]
auto book_price(const MarketOrder& /*order*/) -> std::optional<OrderPrice> {
  return std::nullopt;
}

}  // namespace

OrderTypeConversion::OrderTypeConversion(
    EventListener& event_listener,
    const core::PriceTickRanges& price_tick_ranges)
    : EventReporter{event_listener}, price_tick_ranges_{&price_tick_ranges} {}

template <typename ConvertedType, typename UpdateType, typename ContainerType>
auto OrderTypeConversion::convert(UpdateType update, ContainerType& orders)
    -> std::optional<ConvertedOrder<ConvertedType>> {
  const auto order_it = find_target(orders, update);
  if (order_it == orders.end()) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_reason(RejectText{"order not found"})
                                .build()));
    return std::nullopt;
  }

  if (const auto error = conversion_error(update, *order_it)) {
    emit(ClientNotification(prepare_modification_reject(update)
                                .with_order_status(order_it->status())
                                .with_reason(RejectText{std::string{*error}})
                                .build()));
    return std::nullopt;
  }

  const OrderBookUpdate removal{.side = order_it->side(),
                                .action = OrderBookUpdate::Action::Remove,
                                .price = book_price(*order_it),
                                .quantity = order_it->leaves_quantity()};

  auto resting_order = *order_it;
  orders.erase(order_it);
  emit(order::make_making_order_removed_from_book_notification(resting_order));

  ConvertedType order{std::move(resting_order), std::move(update.order_diff)};
  emit(ClientNotification(
      prepare_modification_confirmation(order, *price_tick_ranges_)
          .with_execution_id(order.make_execution_id())
          .with_orig_client_order_id(update.orig_client_order_id)
          .build()));

  return std::make_optional<ConvertedOrder<ConvertedType>>(std::move(order),
                                                           removal);
}

auto OrderTypeConversion::to_market_order(MarketUpdate update,
                                          LimitOrdersContainer& orders)
    -> std::optional<ConvertedOrder<MarketOrder>> {
  return convert<MarketOrder>(std::move(update), orders);
}

auto OrderTypeConversion::to_auction_limit_order(LimitUpdate update,
                                                 MarketOrdersContainer& orders)
    -> std::optional<ConvertedOrder<LimitOrder>> {
  return convert<LimitOrder>(std::move(update), orders);
}

}  // namespace simulator::trading_system::matching_engine
