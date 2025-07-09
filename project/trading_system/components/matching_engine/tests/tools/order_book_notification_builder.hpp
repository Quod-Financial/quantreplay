#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_BOOK_NOTIFICATION_BUILDER_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_BOOK_NOTIFICATION_BUILDER_HPP_

#include <optional>

#include "common/trade.hpp"
#include "core/domain/attributes.hpp"
#include "ih/common/data/market_data_updates.hpp"

namespace simulator::trading_system::matching_engine {

struct NewOrderAdded {
  operator OrderAdded() const { return create(); }

  auto with_order_owner(std::optional<PartyId> owner) -> NewOrderAdded& {
    owner_ = std::move(owner);
    return *this;
  }

  auto with_order_price(std::optional<Price> price) -> NewOrderAdded& {
    price_ = price;
    return *this;
  }

  auto with_order_quantity(Quantity quantity) -> NewOrderAdded& {
    quantity_ = quantity;
    return *this;
  }

  auto with_order_id(OrderId id) -> NewOrderAdded& {
    id_ = id;
    return *this;
  }

  auto with_order_side(Side side) -> NewOrderAdded& {
    side_ = side;
    return *this;
  }

  auto create() const -> OrderAdded {
    return {owner_, price_, quantity_.value(), id_.value(), side_.value()};
  }

  static auto init() -> NewOrderAdded {
    return NewOrderAdded()
        .with_order_owner(std::nullopt)
        .with_order_price(std::nullopt)
        .with_order_quantity(Quantity(1))
        .with_order_id(OrderId(1))
        .with_order_side(Side::Option::Buy);
  }

 private:
  std::optional<PartyId> owner_;
  std::optional<Price> price_;
  std::optional<Quantity> quantity_;
  std::optional<OrderId> id_;
  std::optional<Side> side_;
};

struct NewOrderReduced {
  operator OrderReduced() const { return create(); }

  auto with_order_price(std::optional<Price> price) -> NewOrderReduced& {
    price_ = price;
    return *this;
  }

  auto with_order_quantity(Quantity quantity) -> NewOrderReduced& {
    quantity_ = quantity;
    return *this;
  }

  auto with_order_id(OrderId id) -> NewOrderReduced& {
    id_ = id;
    return *this;
  }

  auto with_order_side(Side side) -> NewOrderReduced& {
    side_ = side;
    return *this;
  }

  auto create() const -> OrderReduced {
    return {price_, quantity_.value(), id_.value(), side_.value()};
  }

  static auto init() -> NewOrderReduced {
    return NewOrderReduced()
        .with_order_quantity(Quantity(1))
        .with_order_id(OrderId(1))
        .with_order_side(Side::Option::Buy);
  }

 private:
  std::optional<Price> price_;
  std::optional<Quantity> quantity_;
  std::optional<OrderId> id_;
  std::optional<Side> side_;
};

struct NewOrderRemoved {
  operator OrderRemoved() const { return create(); }

  auto with_order_price(std::optional<Price> price) -> NewOrderRemoved& {
    price_ = price;
    return *this;
  }

  auto with_order_id(OrderId id) -> NewOrderRemoved& {
    id_ = id;
    return *this;
  }

  auto with_order_side(Side side) -> NewOrderRemoved& {
    side_ = side;
    return *this;
  }

  auto create() const -> OrderRemoved {
    return {price_, id_.value(), side_.value()};
  }

  static auto init() -> NewOrderRemoved {
    return NewOrderRemoved()
        .with_order_id(OrderId(1))
        .with_order_side(Side::Option::Buy);
  }

 private:
  std::optional<Price> price_;
  std::optional<OrderId> id_;
  std::optional<Side> side_;
};

struct NewTrade {
  operator Trade() const { return create(); }

  auto create() const -> Trade {
    return {
        .buyer = buyer_,
        .seller = seller_,
        .trade_price = trade_price_.value_or(Price(1)),
        .traded_quantity = traded_quantity_.value_or(Quantity(1)),
        .aggressor_side = aggressor_side_.value_or(AggressorSide::Option::Buy),
        .trade_time = trade_time_.value_or(core::get_current_system_time()),
        .market_phase = market_phase_.value_or(MarketPhase::open())};
  }

  auto with_buyer(BuyerId buyer) -> NewTrade& {
    buyer_ = std::move(buyer);
    return *this;
  }

  auto with_seller(SellerId seller) -> NewTrade& {
    seller_ = std::move(seller);
    return *this;
  }

  auto with_trade_price(Price price) -> NewTrade& {
    trade_price_ = price;
    return *this;
  }

  auto with_traded_quantity(Quantity quantity) -> NewTrade& {
    traded_quantity_ = quantity;
    return *this;
  }

  auto with_aggressor_side(AggressorSide side) -> NewTrade& {
    aggressor_side_ = side;
    return *this;
  }

  auto with_trade_time(core::sys_us time) -> NewTrade& {
    trade_time_ = time;
    return *this;
  }

  auto with_market_phase(MarketPhase phase) -> NewTrade& {
    market_phase_ = phase;
    return *this;
  }

 private:
  std::optional<BuyerId> buyer_;
  std::optional<SellerId> seller_;
  std::optional<Price> trade_price_;
  std::optional<Quantity> traded_quantity_;
  std::optional<AggressorSide> aggressor_side_;
  std::optional<core::sys_us> trade_time_;
  std::optional<MarketPhase> market_phase_;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_TOOLS_ORDER_BOOK_NOTIFICATION_BUILDER_HPP_
