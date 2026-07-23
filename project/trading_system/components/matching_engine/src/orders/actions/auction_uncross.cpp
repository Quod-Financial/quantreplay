#include "ih/orders/actions/auction_uncross.hpp"

#include <algorithm>
#include <type_traits>

#include "core/domain/party.hpp"
#include "ih/common/events/client_notification.hpp"
#include "ih/orders/book/order_algorithms.hpp"
#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "ih/orders/replies/execution_reply_builders.hpp"
#include "ih/orders/tools/notification_creators.hpp"
#include "log/logging.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

auto eligible_limit_end(LimitOrdersContainer& limits,
                        Price clearing_price,
                        Side side) -> LimitOrdersContainer::iterator {
  return find_limit_order(limits, [&](const LimitOrder& order) {
    return !limit_crosses_clearing_price(order.price(), clearing_price, side);
  });
}

}  // namespace

AuctionUncross::AuctionUncross(EventListener& event_listener,
                               MarketPhase auction_phase,
                               std::optional<PriceTick> price_tick)
    : EventReporter{event_listener},
      auction_phase_{auction_phase},
      price_tick_{price_tick} {}

auto AuctionUncross::operator()(OrderBook& book) const
    -> std::optional<AuctionResult> {
  // TODO: REMOVE THIS IMPLEMENTATION ONCE VOLUME-MAXIMISING EQUILIBRIUM PRICE
  // IS IMPLEMENTED, UNCOMMENT THE IMPLEMENTATION BELLOW
  std::optional<AuctionResult> clearing;
  double total_quantity = 0.0;
  while (const auto result =
             AuctionPriceCalculator{price_tick_}.compute(book)) {
    if (!clearing.has_value()) {
      clearing = result;
    }
    total_quantity += static_cast<double>(result->quantity);
    log::debug("uncrossing auction book at price {}, volume {}",
               result->price,
               result->quantity);
    cross_book(book, *result);
    remove_filled_limit_orders(book);
  }

  if (clearing.has_value()) {
    clearing->quantity = Quantity{total_quantity};
  } else {
    log::debug("auction book did not cross, cancelling all market orders");
  }

  clear_market_orders(book.buy_page().market_orders());
  clear_market_orders(book.sell_page().market_orders());

  return clearing;

  // const auto result = AuctionPriceCalculator{price_tick_}.compute(book);
  // if (result.has_value()) {
  //   log::debug("uncrossing auction book at price {}, volume {}",
  //              result->price,
  //              result->quantity);
  //   cross_book(book, *result);
  // } else {
  //   log::debug("auction book did not cross, cancelling all market orders");
  // }
  //
  // clear_market_orders(book.buy_page().market_orders());
  // clear_market_orders(book.sell_page().market_orders());
  // remove_filled_limit_orders(book);
  //
  // return result;
}

auto AuctionUncross::cross_book(OrderBook& book,
                                const AuctionResult& result) const -> void {
  const ExecutionPrice price{result.price};
  OrderPage& buy = book.buy_page();
  OrderPage& sell = book.sell_page();

  // Market orders cross against opposing market orders first, by time priority.
  cross_sequences(buy.market_orders().begin(),
                  buy.market_orders().end(),
                  sell.market_orders().begin(),
                  sell.market_orders().end(),
                  price);

  // Market orders left on one side cross against the opposing limit orders
  // priced at or better than the clearing price. At most one of the two calls
  // does any work, since market orders can only remain on a single side.
  cross_sequences(
      buy.market_orders().begin(),
      buy.market_orders().end(),
      sell.limit_orders().begin(),
      eligible_limit_end(sell.limit_orders(), result.price, Side::Option::Sell),
      price);
  cross_sequences(
      buy.limit_orders().begin(),
      eligible_limit_end(buy.limit_orders(), result.price, Side::Option::Buy),
      sell.market_orders().begin(),
      sell.market_orders().end(),
      price);

  // Whatever eligible limit orders remain cross against one another.
  cross_sequences(
      buy.limit_orders().begin(),
      eligible_limit_end(buy.limit_orders(), result.price, Side::Option::Buy),
      sell.limit_orders().begin(),
      eligible_limit_end(sell.limit_orders(), result.price, Side::Option::Sell),
      price);
}

template <typename BuyOrderIt, typename SellOrderIt>
auto AuctionUncross::cross_sequences(BuyOrderIt buy,
                                     BuyOrderIt buy_end,
                                     SellOrderIt sell,
                                     SellOrderIt sell_end,
                                     ExecutionPrice price) const -> void {
  while (buy != buy_end && sell != sell_end) {
    if (buy->executed()) {
      ++buy;
    } else if (sell->executed()) {
      ++sell;
    } else {
      cross(*buy, *sell, price);
    }
  }
}

template <typename BuyOrder, typename SellOrder>
auto AuctionUncross::cross(BuyOrder& buy_order,
                           SellOrder& sell_order,
                           ExecutionPrice price) const -> void {
  const ExecutedQuantity quantity{static_cast<Quantity>(
      std::min(buy_order.leaves_quantity(), sell_order.leaves_quantity()))};

  buy_order.execute(quantity, price);
  sell_order.execute(quantity, price);

  log::debug("auction cross {}@{}: buy: {}; sell: {}",
             quantity,
             price,
             buy_order,
             sell_order);

  emit(ClientNotification(
      prepare_execution_report(buy_order, price_tick_)
          .with_execution_id(buy_order.make_execution_id())
          .with_execution_price(price)
          .with_executed_quantity(quantity)
          .with_counterparty(make_counterparty(sell_order.owner()))
          .build()));
  emit(ClientNotification(
      prepare_execution_report(sell_order, price_tick_)
          .with_execution_id(sell_order.make_execution_id())
          .with_execution_price(price)
          .with_executed_quantity(quantity)
          .with_counterparty(make_counterparty(buy_order.owner()))
          .build()));

  // Both orders rest in the book, so each priced order needs a depth update;
  // market orders carry no depth level and are dropped wholesale on removal.
  if constexpr (std::is_same_v<BuyOrder, LimitOrder>) {
    emit(make_making_order_reduced_notification(buy_order));
  }
  if constexpr (std::is_same_v<SellOrder, LimitOrder>) {
    emit(make_making_order_reduced_notification(sell_order));
  }

  emit(make_auction_trade_notification(
      buy_order, sell_order, price, quantity, auction_phase_));
}

auto AuctionUncross::clear_market_orders(MarketOrdersContainer& orders) const
    -> void {
  for (MarketOrder& order : orders) {
    const bool cancelled = !order.executed();
    if (cancelled) {
      order.cancel();
    }
    // Emit the book removal before the client cancellation, as the elimination
    // actions do.
    emit(make_making_order_removed_from_book_notification(order));
    if (cancelled) {
      emit(ClientNotification(
          prepare_cancellation_confirmation(order, price_tick_)
              .with_leaving_quantity(LeavesQuantity{0})
              .with_execution_id(order.make_execution_id())
              .with_client_order_id(order.client_order_id())
              .build()));
    }
  }
  orders.erase(orders.begin(), orders.end());
}

auto AuctionUncross::remove_filled_limit_orders(OrderBook& book) -> void {
  erase_filled_limit_orders(book.buy_page().limit_orders());
  erase_filled_limit_orders(book.sell_page().limit_orders());
}

}  // namespace simulator::trading_system::matching_engine::order
