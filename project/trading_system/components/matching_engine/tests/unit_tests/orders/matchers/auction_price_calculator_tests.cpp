#include <gmock/gmock.h>

#include <algorithm>
#include <cstdint>
#include <optional>

#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_book_update.hpp"
#include "ih/orders/matchers/auction_price_calculator.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

constexpr auto MoreBuyers = TradeCondition::Option::ImbalanceMoreBuyers;
constexpr auto MoreSellers = TradeCondition::Option::ImbalanceMoreSellers;

[[nodiscard]]
auto IsAuctionResult(double price,
                     double quantity,
                     double imbalance,
                     TradeCondition imbalance_side) {
  return Optional(
      AllOf(Field(&AuctionResult::price, Eq(Price{price})),
            Field(&AuctionResult::quantity, Eq(Quantity{quantity})),
            Field(&AuctionResult::imbalance, Eq(Quantity{imbalance})),
            Field(&AuctionResult::imbalance_side, Eq(imbalance_side))));
}

constexpr auto Added = OrderBookUpdate::Action::Add;
constexpr auto Removed = OrderBookUpdate::Action::Remove;

[[nodiscard]]
auto update_of(const LimitOrder& order, OrderBookUpdate::Action action)
    -> OrderBookUpdate {
  return OrderBookUpdate{.side = order.side(),
                         .action = action,
                         .price = order.price(),
                         .quantity = order.leaves_quantity()};
}

[[nodiscard]]
auto update_of(const MarketOrder& order, OrderBookUpdate::Action action)
    -> OrderBookUpdate {
  return OrderBookUpdate{.side = order.side(),
                         .action = action,
                         .price = std::nullopt,
                         .quantity = order.leaves_quantity()};
}

struct MatchingEngineAuctionPriceCalculator : public Test {
  OrderBook order_book;
  std::optional<Price> reference_price;
  std::optional<AuctionPriceCalculator> calculator;

  auto create_calculator() -> void {
    calculator.emplace(order_book, reference_price);
  }

  [[nodiscard]]
  auto auction_result() const -> std::optional<AuctionResult> {
    return calculator->auction_result();
  }

  // The result a calculator constructed from scratch on the current book gives.
  [[nodiscard]]
  auto rebuilt_result() const -> std::optional<AuctionResult> {
    return AuctionPriceCalculator{order_book, reference_price}.auction_result();
  }

  auto add_buy_limit(OrderPrice price, OrderQuantity quantity) -> LimitOrder {
    return add_limit(Side::Option::Buy, price, quantity);
  }

  auto add_sell_limit(OrderPrice price, OrderQuantity quantity) -> LimitOrder {
    return add_limit(Side::Option::Sell, price, quantity);
  }

  auto add_buy_market(OrderQuantity quantity) -> MarketOrder {
    return add_market(Side::Option::Buy, quantity);
  }

  auto add_sell_market(OrderQuantity quantity) -> MarketOrder {
    return add_market(Side::Option::Sell, quantity);
  }

  auto place_limit(Side side, OrderPrice price, OrderQuantity quantity)
      -> LimitOrder {
    const auto order = add_limit(side, price, quantity);
    calculator->process({update_of(order, Added)});
    return order;
  }

  auto place_market(Side side, OrderQuantity quantity) -> MarketOrder {
    const auto order = add_market(side, quantity);
    calculator->process({update_of(order, Added)});
    return order;
  }

  template <typename Container>
  static auto find_order(Container& orders, OrderId identifier) {
    return std::ranges::find_if(orders, [identifier](const auto& order) {
      return order.id() == identifier;
    });
  }

  auto cancel(const LimitOrder& order) -> void {
    auto& orders = order_book.take_page(order.side()).limit_orders();
    orders.erase(find_order(orders, order.id()));

    calculator->process({update_of(order, Removed)});
  }

  auto cancel(const MarketOrder& order) -> void {
    auto& orders = order_book.take_page(order.side()).market_orders();
    orders.erase(find_order(orders, order.id()));

    calculator->process({update_of(order, Removed)});
  }

  auto amend(const LimitOrder& order, OrderPrice price, OrderQuantity quantity)
      -> void {
    auto& orders = order_book.take_page(order.side()).limit_orders();
    orders.erase(find_order(orders, order.id()));
    const auto amended = make_limit(order.side(), price, quantity, order.id());
    orders.emplace(amended);

    calculator->process({update_of(order, Removed), update_of(amended, Added)});
  }

  auto amend(const MarketOrder& order, OrderQuantity quantity) -> void {
    auto& orders = order_book.take_page(order.side()).market_orders();
    const auto order_it = find_order(orders, order.id());
    order_it->amend(MarketOrder::Update{quantity});

    calculator->process(
        {update_of(order, Removed), update_of(*order_it, Added)});
  }

 private:
  auto add_limit(Side side, OrderPrice price, OrderQuantity quantity)
      -> LimitOrder {
    const auto order = make_limit(side, price, quantity, OrderId{next_id_++});
    order_book.take_page(side).limit_orders().emplace(order);
    return order;
  }

  auto add_market(Side side, OrderQuantity quantity) -> MarketOrder {
    const auto order = make_market(side, quantity, OrderId{next_id_++});
    order_book.take_page(side).market_orders().emplace(order);
    return order;
  }

  auto make_limit(Side side,
                  OrderPrice price,
                  OrderQuantity quantity,
                  OrderId identifier) -> LimitOrder {
    return builder_.with_order_id(identifier)
        .with_side(side)
        .with_order_price(price)
        .with_order_quantity(quantity)
        .build_limit_order();
  }

  auto make_market(Side side, OrderQuantity quantity, OrderId identifier)
      -> MarketOrder {
    return builder_.with_order_id(identifier)
        .with_side(side)
        .with_order_quantity(quantity)
        .build_market_order();
  }

  OrderBuilder builder_;
  std::uint64_t next_id_{1};
};

TEST_F(MatchingEngineAuctionPriceCalculator, ReturnsNulloptForEmptyBook) {
  create_calculator();

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReturnsNulloptWhenOnlyMarketOrdersArePresent) {
  add_buy_market(OrderQuantity{10});
  add_sell_market(OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReturnsNulloptWhenBestBuyLessThanBestSell) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{10});
  add_sell_limit(OrderPrice{101}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReturnsNulloptWhenNoBuyLimitOrders) {
  add_buy_market(OrderQuantity{10});
  add_sell_limit(OrderPrice{100}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReturnsNulloptWhenNoSellLimitOrders) {
  add_sell_market(OrderQuantity{10});
  add_buy_limit(OrderPrice{100}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       DerivesPriceWhenBestBidEqualsBestOffer) {
  add_buy_market(OrderQuantity{5});
  add_buy_limit(OrderPrice{100}, OrderQuantity{30});
  add_buy_limit(OrderPrice{98}, OrderQuantity{10});
  add_sell_market(OrderQuantity{10});
  add_sell_limit(OrderPrice{100}, OrderQuantity{20});
  add_sell_limit(OrderPrice{103}, OrderQuantity{15});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 30, 5, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       SumsQuantitiesOfOrdersSharingSamePriceLevel) {
  add_buy_limit(OrderPrice{101}, OrderQuantity{10});
  add_buy_limit(OrderPrice{101}, OrderQuantity{15});
  add_buy_limit(OrderPrice{101}, OrderQuantity{25});
  add_sell_limit(OrderPrice{100}, OrderQuantity{12});
  add_sell_limit(OrderPrice{100}, OrderQuantity{13});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 25, 25, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       RecalculatesToResultWhenPlacementCrossesBook) {
  add_buy_market(OrderQuantity{5});
  add_buy_limit(OrderPrice{99}, OrderQuantity{30});
  add_buy_limit(OrderPrice{95}, OrderQuantity{10});
  add_sell_market(OrderQuantity{4});
  add_sell_limit(OrderPrice{101}, OrderQuantity{20});
  add_sell_limit(OrderPrice{106}, OrderQuantity{10});
  create_calculator();
  ASSERT_THAT(auction_result(), Eq(std::nullopt));

  place_limit(Side::Option::Buy, OrderPrice{101}, OrderQuantity{25});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 24, 6, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ChoosesResultByMinimumAbsoluteImbalance) {
  add_buy_market(OrderQuantity{30});
  add_buy_limit(OrderPrice{103}, OrderQuantity{30});
  add_buy_limit(OrderPrice{101}, OrderQuantity{20});
  add_buy_limit(OrderPrice{99}, OrderQuantity{10});
  add_sell_market(OrderQuantity{20});
  add_sell_limit(OrderPrice{101}, OrderQuantity{40});
  add_sell_limit(OrderPrice{103}, OrderQuantity{15});
  add_sell_limit(OrderPrice{105}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(103, 60, 15, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ChoosesResultByMaxPriceWhichImbalanceIsGreaterThanZero) {
  add_buy_market(OrderQuantity{50});
  add_buy_limit(OrderPrice{105}, OrderQuantity{40});
  add_buy_limit(OrderPrice{98}, OrderQuantity{10});
  add_sell_market(OrderQuantity{10});
  add_sell_limit(OrderPrice{100}, OrderQuantity{20});
  add_sell_limit(OrderPrice{103}, OrderQuantity{15});
  add_sell_limit(OrderPrice{107}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(105, 45, 45, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ChoosesResultByMinPriceWhichImbalanceIsLessThanZero) {
  add_buy_market(OrderQuantity{10});
  add_buy_limit(OrderPrice{105}, OrderQuantity{20});
  add_buy_limit(OrderPrice{102}, OrderQuantity{15});
  add_buy_limit(OrderPrice{99}, OrderQuantity{10});
  add_sell_market(OrderQuantity{50});
  add_sell_limit(OrderPrice{100}, OrderQuantity{40});
  add_sell_limit(OrderPrice{107}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 45, 45, MoreSellers));
}

TEST_F(
    MatchingEngineAuctionPriceCalculator,
    ChoosesResultByLowestPriceWhenTiedImbalanceSignsAreMixedAndReferencePriceNotDefined) {
  add_buy_limit(OrderPrice{100}, OrderQuantity{5});
  add_buy_limit(OrderPrice{102}, OrderQuantity{50});
  add_sell_limit(OrderPrice{100}, OrderQuantity{50});
  add_sell_limit(OrderPrice{101}, OrderQuantity{5});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 50, 5, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ChoosesResultByClosestPriceToReferencePriceWhenImbalanceSignsAreMixed) {
  reference_price = Price{102.5};
  add_buy_market(OrderQuantity{20});
  add_buy_limit(OrderPrice{103}, OrderQuantity{30});
  add_buy_limit(OrderPrice{101}, OrderQuantity{10});
  add_buy_limit(OrderPrice{99}, OrderQuantity{10});
  add_sell_market(OrderQuantity{20});
  add_sell_limit(OrderPrice{101}, OrderQuantity{30});
  add_sell_limit(OrderPrice{103}, OrderQuantity{10});
  add_sell_limit(OrderPrice{106}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(103, 50, 10, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ChoosesResultByClosestPriceToReferencePriceWhenTiedImbalancesAreZero) {
  reference_price = Price{101.4};
  add_buy_market(OrderQuantity{1000});
  add_buy_limit(OrderPrice{103}, OrderQuantity{100});
  add_buy_limit(OrderPrice{102}, OrderQuantity{200});
  add_buy_limit(OrderPrice{100}, OrderQuantity{300});
  add_sell_market(OrderQuantity{500});
  add_sell_limit(OrderPrice{100}, OrderQuantity{300});
  add_sell_limit(OrderPrice{101}, OrderQuantity{500});
  add_sell_limit(OrderPrice{105}, OrderQuantity{300});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 1300, 0, MoreBuyers));
}

TEST_F(
    MatchingEngineAuctionPriceCalculator,
    ChoosesResultByLowestPriceWhenTiedImbalancesAreZeroAndReferencePriceNotDefined) {
  add_buy_market(OrderQuantity{1000});
  add_buy_limit(OrderPrice{103}, OrderQuantity{100});
  add_buy_limit(OrderPrice{102}, OrderQuantity{200});
  add_buy_limit(OrderPrice{100}, OrderQuantity{300});
  add_sell_market(OrderQuantity{500});
  add_sell_limit(OrderPrice{100}, OrderQuantity{300});
  add_sell_limit(OrderPrice{101}, OrderQuantity{500});
  add_sell_limit(OrderPrice{105}, OrderQuantity{300});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 1300, 0, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReportsMoreBuyersSideWhenChosenImbalanceIsZeroAndBuySurplusIsHeavier) {
  add_buy_limit(OrderPrice{102}, OrderQuantity{40});
  add_buy_limit(OrderPrice{101}, OrderQuantity{10});
  add_buy_limit(OrderPrice{100}, OrderQuantity{30});
  add_sell_limit(OrderPrice{100}, OrderQuantity{30});
  add_sell_limit(OrderPrice{101}, OrderQuantity{20});
  add_sell_limit(OrderPrice{102}, OrderQuantity{10});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 50, 0, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       ReportsMoreSellersSideWhenChosenImbalanceIsZeroAndSellSurplusIsHeavier) {
  add_buy_limit(OrderPrice{102}, OrderQuantity{40});
  add_buy_limit(OrderPrice{101}, OrderQuantity{10});
  add_buy_limit(OrderPrice{100}, OrderQuantity{10});
  add_sell_limit(OrderPrice{100}, OrderQuantity{40});
  add_sell_limit(OrderPrice{101}, OrderQuantity{10});
  add_sell_limit(OrderPrice{102}, OrderQuantity{40});

  create_calculator();

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 50, 0, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       RecalculatesResultWhenBookCrossesAgainAfterUncrossing) {
  const auto buy_101 = add_buy_limit(OrderPrice{101}, OrderQuantity{50});
  add_sell_limit(OrderPrice{100}, OrderQuantity{50});
  create_calculator();

  cancel(buy_101);
  ASSERT_THAT(auction_result(), Eq(std::nullopt));

  place_limit(Side::Option::Buy, OrderPrice{102}, OrderQuantity{20});

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 20, 30, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       KeepsCandidatePriceWhenQuantityOnlyAmendLeavesOrderAtSamePrice) {
  const auto buy_100 = add_buy_limit(OrderPrice{100}, OrderQuantity{50});
  add_sell_limit(OrderPrice{100}, OrderQuantity{50});
  create_calculator();

  amend(buy_100, OrderPrice{100}, OrderQuantity{30});

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 30, 20, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculator,
       KeepsIncrementalResultEqualToFullRecalculation) {
  // The candidate range spans 100..103 with a gap at 101 and 102, so the
  // placements below force new candidate prices to be interpolated.
  const auto market_buy = add_buy_market(OrderQuantity{20});
  const auto buy_103 = add_buy_limit(OrderPrice{103}, OrderQuantity{10});
  add_buy_limit(OrderPrice{100}, OrderQuantity{30});
  const auto buy_97 = add_buy_limit(OrderPrice{97}, OrderQuantity{50});
  add_sell_market(OrderQuantity{10});
  const auto sell_100 = add_sell_limit(OrderPrice{100}, OrderQuantity{30});
  const auto sell_103 = add_sell_limit(OrderPrice{103}, OrderQuantity{40});
  add_sell_limit(OrderPrice{106}, OrderQuantity{25});
  create_calculator();

  const auto buy_102 =
      place_limit(Side::Option::Buy, OrderPrice{102}, OrderQuantity{25});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "buy at a price with no candidate yet";

  place_limit(Side::Option::Sell, OrderPrice{101}, OrderQuantity{20});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "sell at a price with no candidate yet";

  place_limit(Side::Option::Buy, OrderPrice{104}, OrderQuantity{15});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result())) << "buy above the range";

  place_market(Side::Option::Buy, OrderQuantity{25});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result())) << "market buy placement";

  amend(buy_102, OrderPrice{102}, OrderQuantity{10});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result())) << "quantity-only amend";

  amend(sell_100, OrderPrice{104}, OrderQuantity{15});
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "amend moving a price across the range";

  cancel(buy_102);
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "cancel of the only order at a candidate price";

  cancel(market_buy);
  EXPECT_THAT(auction_result(), Eq(rebuilt_result())) << "market order cancel";

  cancel(buy_97);
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "cancel below the range";

  cancel(sell_103);
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "cancel inside the range";

  cancel(buy_103);
  EXPECT_THAT(auction_result(), Eq(rebuilt_result()))
      << "cancel of the best bid";
}

struct MatchingEngineAuctionPriceCalculatorRecalculation
    : public MatchingEngineAuctionPriceCalculator {
  MarketOrder market_buy = add_buy_market(OrderQuantity{30});
  LimitOrder buy_limit_102 = add_buy_limit(OrderPrice{102}, OrderQuantity{10});
  LimitOrder buy_limit_101 = add_buy_limit(OrderPrice{101}, OrderQuantity{45});
  LimitOrder buy_limit_100 = add_buy_limit(OrderPrice{100}, OrderQuantity{55});
  LimitOrder buy_limit_98 = add_buy_limit(OrderPrice{98}, OrderQuantity{10});
  MarketOrder market_sell = add_sell_market(OrderQuantity{15});
  LimitOrder sell_limit_100 =
      add_sell_limit(OrderPrice{100}, OrderQuantity{45});
  LimitOrder sell_limit_101 =
      add_sell_limit(OrderPrice{101}, OrderQuantity{45});
  LimitOrder sell_limit_102 =
      add_sell_limit(OrderPrice{102}, OrderQuantity{20});
  LimitOrder sell_limit_105 =
      add_sell_limit(OrderPrice{105}, OrderQuantity{30});

  MatchingEngineAuctionPriceCalculatorRecalculation() { create_calculator(); }
};

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       ChoosesResultByMaxTradableQuantity) {
  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 20, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketBuyPlacement) {
  place_market(Side::Option::Buy, OrderQuantity{30});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 105, 10, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketSellPlacement) {
  place_market(Side::Option::Sell, OrderQuantity{60});

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 120, 20, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitBuyPlacement) {
  place_limit(Side::Option::Buy, OrderPrice{103}, OrderQuantity{55});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 105, 35, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitSellPlacement) {
  place_limit(Side::Option::Sell, OrderPrice{99}, OrderQuantity{15});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 35, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketBuyCancellation) {
  cancel(market_buy);

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 60, 50, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketSellCancellation) {
  cancel(market_sell);

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 5, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitBuyCancellation) {
  cancel(buy_limit_102);

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 75, 30, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitSellCancellation) {
  cancel(sell_limit_100);

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 60, 25, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketBuyQtyChange) {
  amend(market_buy, OrderQuantity{15});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 70, 35, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterMarketSellQtyChange) {
  amend(market_sell, OrderQuantity{25});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 30, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitBuyQtyChange) {
  amend(buy_limit_102, OrderPrice{102}, OrderQuantity{20});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 95, 10, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitSellQtyChange) {
  amend(sell_limit_100, OrderPrice{100}, OrderQuantity{50});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 25, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitBuyPriceChange) {
  amend(buy_limit_101, OrderPrice{100}, OrderQuantity{45});

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 60, 80, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitSellPriceChange) {
  amend(sell_limit_101, OrderPrice{99}, OrderQuantity{45});

  ASSERT_THAT(auction_result(), IsAuctionResult(100, 105, 35, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitBuyPriceQtyChange) {
  amend(buy_limit_100, OrderPrice{103}, OrderQuantity{40});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 105, 20, MoreBuyers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesAfterLimitSellPriceQtyChange) {
  amend(sell_limit_102, OrderPrice{101}, OrderQuantity{55});

  ASSERT_THAT(auction_result(), IsAuctionResult(101, 85, 75, MoreSellers));
}

TEST_F(MatchingEngineAuctionPriceCalculatorRecalculation,
       RecalculatesToNulloptWhenCancellationUncrossesBook) {
  cancel(buy_limit_102);
  cancel(buy_limit_101);
  cancel(buy_limit_100);

  ASSERT_THAT(auction_result(), Eq(std::nullopt));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
