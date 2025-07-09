#include "ih/orders/matchers/regular_order_matcher.hpp"

#include <numeric>
#include <ranges>

#include "core/common/unreachable.hpp"
#include "core/domain/party.hpp"
#include "ih/common/events/client_notification.hpp"
#include "ih/orders/book/order_algorithms.hpp"
#include "ih/orders/replies/execution_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine {

RegularOrderMatcher::RegularOrderMatcher(EventListener& event_listener,
                                         OrderBook& order_book)
    : EventReporter(event_listener), order_book_(order_book) {}

auto RegularOrderMatcher::match(LimitOrder& taker) -> void {
  log::debug("matching: {}", taker);

  if (taker.time_in_force() == TimeInForce::Option::ImmediateOrCancel) {
    trade_ioc_taker(taker, find_matching_orders(taker));
  } else {
    trade_taker(taker, find_matching_orders(taker));
  }

  remove_filled_orders(take_opposite_limit_orders(taker.side()));
}

auto RegularOrderMatcher::match(MarketOrder& taker) -> void {
  log::debug("matching: {}", taker);

  trade_market_taker(taker, find_matching_orders(taker));
  remove_filled_orders(take_opposite_limit_orders(taker.side()));
}

auto RegularOrderMatcher::has_facing_orders(const LimitOrder& taker) -> bool {
  return std::ranges::any_of(take_opposite_limit_orders(taker.side()),
                             make_price_criteria(taker));
}

auto RegularOrderMatcher::has_facing_orders(const MarketOrder& taker) -> bool {
  return !take_opposite_limit_orders(taker.side()).empty();
}

auto RegularOrderMatcher::can_fully_trade(const LimitOrder& taker) -> bool {
  const auto makers = find_matching_orders(taker);
  const LeavesQuantity making_qty = std::accumulate(
      makers.begin(),
      makers.end(),
      LeavesQuantity{0.0},
      [](LeavesQuantity leaves_qty, const LimitOrder* maker) {
        return LeavesQuantity{static_cast<double>(leaves_qty) +
                              static_cast<double>(maker->leaves_quantity())};
      });

  return making_qty >= taker.leaves_quantity();
}

auto RegularOrderMatcher::find_matching_orders(const LimitOrder& taker)
    -> std::vector<LimitOrder*> {
  std::vector<LimitOrder*> matched_makers;

  auto& opposite_resting_orders = take_opposite_limit_orders(taker.side());
  const auto match_criteria = make_price_criteria(taker);
  std::ranges::transform(
      opposite_resting_orders | std::views::take_while(match_criteria),
      std::back_inserter(matched_makers),
      [](LimitOrder& maker) { return &maker; });

  return matched_makers;
}

auto RegularOrderMatcher::find_matching_orders(const MarketOrder& taker)
    -> std::vector<LimitOrder*> {
  std::vector<LimitOrder*> matched_makers;

  auto& opposite_resting_orders = take_opposite_limit_orders(taker.side());
  std::ranges::transform(opposite_resting_orders,
                         std::back_inserter(matched_makers),
                         [](LimitOrder& maker) { return &maker; });

  return matched_makers;
}

auto RegularOrderMatcher::trade_taker(LimitOrder& taker,
                                      const std::vector<LimitOrder*>& makers)
    -> void {
  for (LimitOrder* maker : makers) {
    if (taker.executed()) {
      break;
    }

    const auto [trade_px, trade_qty] = compute_trade(taker, *maker);
    log::debug("trading {}@{}: taker: {}; maker: {}",
               trade_qty,
               trade_px,
               *maker,
               taker);
    taker.execute(trade_qty);
    maker->execute(trade_qty);

    emit(ClientNotification(
        prepare_execution_report(taker)
            .with_execution_id(taker.make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(maker->owner()))
            .build()));

    emit(ClientNotification(
        prepare_execution_report(*maker)
            .with_execution_id(maker->make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(taker.owner()))
            .build()));

    emit(order::make_making_order_reduced_notification(*maker));
    emit(order::make_trade_notification(taker, *maker, trade_px, trade_qty));
  }
}

auto RegularOrderMatcher::trade_ioc_taker(
    LimitOrder& taker, const std::vector<LimitOrder*>& makers) -> void {
  if (makers.empty()) {
    log::err(
        "[BUG] the matcher can not trade an order, no orders were matched with "
        "IoC order: {}",
        taker);
    throw std::logic_error("no orders can be traded with IoC order");
  }

  const auto last_maker_it = std::prev(makers.end());
  for (auto maker_iter = makers.begin(); maker_iter != makers.end();
       ++maker_iter) {
    if (taker.executed()) {
      break;
    }

    LimitOrder* maker = *maker_iter;
    const auto [trade_px, trade_qty] = compute_trade(taker, *maker);
    log::debug("trading {}@{}: taker: {}; maker: {}",
               trade_qty,
               trade_px,
               taker,
               *maker);
    taker.execute(trade_qty);
    maker->execute(trade_qty);

    if (maker_iter == last_maker_it && !taker.executed()) {
      taker.cancel();
    }

    emit(ClientNotification(
        prepare_execution_report(taker)
            .with_execution_id(taker.make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(maker->owner()))
            .build()));

    emit(ClientNotification(
        prepare_execution_report(*maker)
            .with_execution_id(maker->make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(taker.owner()))
            .build()));

    emit(order::make_making_order_reduced_notification(*maker));
    emit(order::make_trade_notification(taker, *maker, trade_px, trade_qty));
  }
}

auto RegularOrderMatcher::trade_market_taker(
    MarketOrder& taker, const std::vector<LimitOrder*>& makers) -> void {
  if (makers.empty()) {
    log::err(
        "[BUG] the matcher can not trade an order, no orders were matched with "
        "a market order: {}",
        taker);
    throw std::logic_error("no orders can be traded with market order");
  }

  const auto last_maker_it = std::prev(makers.end());
  for (auto maker_iter = makers.begin(); maker_iter != makers.end();
       ++maker_iter) {
    if (taker.executed()) {
      break;
    }

    LimitOrder* maker = *maker_iter;
    const auto [trade_px, trade_qty] = compute_trade(taker, *maker);
    log::debug("trading {}@{}: taker: {}; maker: {}",
               trade_qty,
               trade_px,
               taker,
               *maker);
    taker.execute(trade_qty);
    maker->execute(trade_qty);

    if (maker_iter == last_maker_it && !taker.executed()) {
      taker.cancel();
    }

    emit(ClientNotification(
        prepare_execution_report(taker)
            .with_execution_id(taker.make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(maker->owner()))
            .build()));

    emit(ClientNotification(
        prepare_execution_report(*maker)
            .with_execution_id(maker->make_execution_id())
            .with_execution_price(trade_px)
            .with_executed_quantity(trade_qty)
            .with_counterparty(make_counterparty(taker.owner()))
            .build()));

    emit(order::make_making_order_reduced_notification(*maker));
    emit(order::make_trade_notification(taker, *maker, trade_px, trade_qty));
  }
}

auto RegularOrderMatcher::take_opposite_limit_orders(Side aggressor_side)
    -> LimitOrdersContainer& {
  switch (static_cast<Side::Option>(aggressor_side)) {
    case Side::Option::Buy:
      return order_book_.sell_page().limit_orders();
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt:
      return order_book_.buy_page().limit_orders();
  }

  core::unreachable();
}

auto RegularOrderMatcher::make_price_criteria(const LimitOrder& aggressor)
    -> PriceMatchCriteria {
  const auto aggressor_price = aggressor.price();
  switch (static_cast<Side::Option>(aggressor.side())) {
    case Side::Option::Buy: {
      return [aggressor_price](const LimitOrder& resting_sell_order) {
        return resting_sell_order.price() <= aggressor_price;
      };
    }
    case Side::Option::Sell:
    case Side::Option::SellShort:
    case Side::Option::SellShortExempt: {
      return [aggressor_price](const LimitOrder& resting_buy_order) {
        return resting_buy_order.price() >= aggressor_price;
      };
    }
  }

  core::unreachable();
}

auto RegularOrderMatcher::remove_filled_orders(LimitOrdersContainer& side)
    -> void {
  constexpr auto non_filled_order = [](const LimitOrder& order) {
    return !order.executed();
  };

  side.erase(side.begin(), find_limit_order(side, non_filled_order));
}

template <typename TakerOrderType>
auto RegularOrderMatcher::compute_trade(const TakerOrderType& taker,
                                        const LimitOrder& maker)
    -> std::pair<ExecutionPrice, ExecutedQuantity> {
  const ExecutionPrice trade_px{static_cast<Price>(maker.price())};
  const ExecutedQuantity trade_qty{static_cast<Quantity>(
      std::min(taker.leaves_quantity(), maker.leaves_quantity()))};
  return std::make_pair(trade_px, trade_qty);
}

}  // namespace simulator::trading_system::matching_engine
