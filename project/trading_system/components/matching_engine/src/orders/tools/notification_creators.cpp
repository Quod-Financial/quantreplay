#include "ih/orders/tools/notification_creators.hpp"

#include "common/trade.hpp"

namespace simulator::trading_system::matching_engine::order {

namespace {

template <typename TradeActorType>
auto get_trade_actor(std::optional<Party> party)
    -> std::optional<TradeActorType> {
  return party.has_value()
             ? std::make_optional<TradeActorType>(
                   static_cast<const std::string&>(party->party_id()))
             : std::nullopt;
}

template <typename TakerOrder, typename MakerOrder>
auto get_buyer_seller(const TakerOrder& taker, const MakerOrder& maker)
    -> std::pair<std::optional<BuyerId>, std::optional<SellerId>> {
  if (taker.side() == Side::Option::Buy) {
    return std::make_pair(get_trade_actor<BuyerId>(taker.owner()),
                          get_trade_actor<SellerId>(maker.owner()));
  }
  return std::make_pair(get_trade_actor<BuyerId>(maker.owner()),
                        get_trade_actor<SellerId>(taker.owner()));
}

auto get_owner_id(const std::optional<Party>& party) -> std::optional<PartyId> {
  std::optional<PartyId> owner_id;
  if (party.has_value()) {
    owner_id = party->party_id();
  }
  return owner_id;
}

}  // namespace

auto make_making_order_added_to_book_notification(const LimitOrder& order)
    -> OrderBookNotification {
  return OrderBookNotification(OrderAdded{
      .order_owner = get_owner_id(order.owner()),
      .order_price = static_cast<Price>(order.price()),
      .order_quantity = static_cast<Quantity>(order.leaves_quantity()),
      .order_id = order.id(),
      .order_side = order.side()});
}

auto make_making_order_removed_from_book_notification(const LimitOrder& order)
    -> OrderBookNotification {
  return OrderBookNotification(
      OrderRemoved{.order_price = static_cast<Price>(order.price()),
                   .order_id = order.id(),
                   .order_side = order.side()});
}

auto make_making_order_reduced_notification(const LimitOrder& maker)
    -> OrderBookNotification {
  return OrderBookNotification(OrderReduced{
      .order_price = static_cast<Price>(maker.price()),
      .order_quantity = static_cast<Quantity>(maker.leaves_quantity()),
      .order_id = maker.id(),
      .order_side = maker.side()});
}

auto make_trade_notification(const LimitOrder& taker,
                             const LimitOrder& maker,
                             ExecutionPrice trade_price,
                             ExecutedQuantity traded_quantity)
    -> OrderBookNotification {
  auto [buyer, seller] = get_buyer_seller(taker, maker);
  return OrderBookNotification(Trade{
      .buyer = std::move(buyer),
      .seller = std::move(seller),
      .trade_price = static_cast<Price>(trade_price),
      .traded_quantity = static_cast<Quantity>(traded_quantity),
      .aggressor_side = AggressorSide{taker.side()},
      .trade_time = core::get_current_system_time(),
      .market_phase =
          MarketPhase::open()});  // The "Open" phase is hardcoded for now.
}

auto make_trade_notification(const MarketOrder& taker,
                             const LimitOrder& maker,
                             ExecutionPrice trade_price,
                             ExecutedQuantity traded_quantity)
    -> OrderBookNotification {
  auto [buyer, seller] = get_buyer_seller(taker, maker);
  return OrderBookNotification(Trade{
      .buyer = std::move(buyer),
      .seller = std::move(seller),
      .trade_price = static_cast<Price>(trade_price),
      .traded_quantity = static_cast<Quantity>(traded_quantity),
      .aggressor_side = AggressorSide{taker.side()},
      .trade_time = core::get_current_system_time(),
      .market_phase =
          MarketPhase::open()});  // The "Open" phase is hardcoded for now.
}

}  // namespace simulator::trading_system::matching_engine::order