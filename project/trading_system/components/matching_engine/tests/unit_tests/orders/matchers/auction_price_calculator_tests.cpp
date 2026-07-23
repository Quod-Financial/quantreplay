#include <gmock/gmock.h>

#include <cstdint>
#include <optional>

#include "ih/orders/book/order_book.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

struct MatchingEngineAuctionPriceCalculator : public Test {
  OrderBook order_book;

  auto add_buy_limit(OrderPrice price, OrderQuantity quantity) -> void {
    order_book.buy_page().limit_orders().emplace(
        make_limit(Side::Option::Buy, price, quantity));
  }

  auto add_sell_limit(OrderPrice price, OrderQuantity quantity) -> void {
    order_book.sell_page().limit_orders().emplace(
        make_limit(Side::Option::Sell, price, quantity));
  }

  auto add_buy_market(OrderQuantity quantity) -> void {
    order_book.buy_page().market_orders().emplace(
        make_market(Side::Option::Buy, quantity));
  }

  auto add_sell_market(OrderQuantity quantity) -> void {
    order_book.sell_page().market_orders().emplace(
        make_market(Side::Option::Sell, quantity));
  }

  auto compute(std::optional<PriceTick> tick = std::nullopt)
      -> std::optional<AuctionResult> {
    return AuctionPriceCalculator{tick}.compute(order_book);
  }

 private:
  auto make_limit(Side side,
                  OrderPrice price,
                  OrderQuantity quantity) -> LimitOrder {
    return builder_.with_order_id(OrderId{next_id_++})
        .with_side(side)
        .with_order_price(price)
        .with_order_quantity(quantity)
        .build_limit_order();
  }

  auto make_market(Side side, OrderQuantity quantity) -> MarketOrder {
    return builder_.with_order_id(OrderId{next_id_++})
        .with_side(side)
        .with_order_quantity(quantity)
        .build_market_order();
  }

  OrderBuilder builder_;
  std::uint64_t next_id_{1};
};

TEST_F(MatchingEngineAuctionPriceCalculator, YieldsNoResultForEmptyBook) {
  ASSERT_THAT(compute(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       YieldsNoResultWithoutBuyLimitOrders) {
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  ASSERT_THAT(compute(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       YieldsNoResultWithoutSellLimitOrders) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{100});

  ASSERT_THAT(compute(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       YieldsNoResultWhenOnlyMarketOrdersArePresent) {
  add_buy_market(OrderQuantity{100});
  add_sell_market(OrderQuantity{100});

  ASSERT_THAT(compute(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       YieldsNoResultWhenLimitPricesDoNotCross) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{100});
  add_sell_limit(OrderPrice{101}, OrderQuantity{100});

  ASSERT_THAT(compute(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       DerivesClearingPriceAsMidOfBestBidAndOffer) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  ASSERT_THAT(compute(),
              Optional(Field(&AuctionResult::price, Eq(Price{105}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       DerivesClearingPriceWhenBestBidEqualsBestOffer) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  ASSERT_THAT(compute(),
              Optional(Field(&AuctionResult::price, Eq(Price{100}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator, RoundsClearingPriceToTickSize) {
  add_buy_limit(OrderPrice{130}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});

  // Raw mid is 115; rounded to the nearest multiple of 10 it becomes 120.
  ASSERT_THAT(compute(PriceTick{10}),
              Optional(Field(&AuctionResult::price, Eq(Price{120}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ClampsRoundedClearingPriceUpToBestOffer) {
  add_buy_limit(OrderPrice{104}, OrderQuantity{100});
  add_sell_limit(OrderPrice{101}, OrderQuantity{100});

  // Raw mid 102.5 rounds to 100, below the best offer; it is clamped back up.
  ASSERT_THAT(compute(PriceTick{10}),
              Optional(Field(&AuctionResult::price, Eq(Price{101}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ClampsRoundedClearingPriceDownToBestBid) {
  add_buy_limit(OrderPrice{109}, OrderQuantity{100});
  add_sell_limit(OrderPrice{106}, OrderQuantity{100});

  // Raw mid 107.5 rounds to 110, above the best bid; it is clamped back down.
  ASSERT_THAT(compute(PriceTick{10}),
              Optional(Field(&AuctionResult::price, Eq(Price{109}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       VolumeIsMinimumOfCrossableQuantities) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_sell_limit(OrderPrice{100}, OrderQuantity{40});

  ASSERT_THAT(compute(),
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{40}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       VolumeIncludesRestingMarketOrders) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{10});
  add_sell_limit(OrderPrice{100}, OrderQuantity{10});
  add_buy_market(OrderQuantity{30});
  add_sell_market(OrderQuantity{5});

  ASSERT_THAT(compute(),
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{15}))));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       VolumeExcludesLimitOrdersPricedBeyondClearingPrice) {
  add_buy_limit(OrderPrice{110}, OrderQuantity{100});
  add_buy_limit(OrderPrice{90}, OrderQuantity{999});
  add_sell_limit(OrderPrice{100}, OrderQuantity{100});
  add_sell_limit(OrderPrice{130}, OrderQuantity{999});

  ASSERT_THAT(compute(),
              Optional(Field(&AuctionResult::quantity, Eq(Quantity{100}))));
}

TEST(MatchingEngineLimitCrossesClearingPrice,
     BuyOrderCrossesWhenPricedAtOrAboveClearingPrice) {
  EXPECT_TRUE(limit_crosses_clearing_price(
      OrderPrice{105}, Price{105}, Side::Option::Buy));
  EXPECT_TRUE(limit_crosses_clearing_price(
      OrderPrice{110}, Price{105}, Side::Option::Buy));
  EXPECT_FALSE(limit_crosses_clearing_price(
      OrderPrice{100}, Price{105}, Side::Option::Buy));
}

TEST(MatchingEngineLimitCrossesClearingPrice,
     SellOrderCrossesWhenPricedAtOrBelowClearingPrice) {
  EXPECT_TRUE(limit_crosses_clearing_price(
      OrderPrice{105}, Price{105}, Side::Option::Sell));
  EXPECT_TRUE(limit_crosses_clearing_price(
      OrderPrice{100}, Price{105}, Side::Option::Sell));
  EXPECT_FALSE(limit_crosses_clearing_price(
      OrderPrice{110}, Price{105}, Side::Option::Sell));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
