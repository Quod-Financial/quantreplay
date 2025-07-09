#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

#include "common/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/book/order_book.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

TEST(BetterOrderComparator, CreatesForBuySide) {
  ASSERT_NO_THROW(BetterOrderComparator{Side::Option::Buy});
}

TEST(BetterOrderComparator, CreatesForSellSide) {
  ASSERT_NO_THROW(BetterOrderComparator{Side::Option::Sell});
}

TEST(BetterOrderComparator, ThrowsExceptionWhenCreatedForUnknownSide) {
  ASSERT_THROW(BetterOrderComparator{static_cast<Side::Option>(0xFF)},
               std::invalid_argument);
}

class BetterBuyOrderComparator : public Test {
  constexpr static auto DefaultPrice = OrderPrice{42};

 public:
  auto make_order_time(std::chrono::seconds increment) const -> OrderTime {
    return OrderTime{test_order_time.value() + increment};
  }

  auto make_limit_order(OrderPrice price) -> LimitOrder {
    return order_builder.with_order_price(price).build_limit_order();
  }

  auto make_limit_order(OrderTime time) -> LimitOrder {
    return order_builder.with_order_price(DefaultPrice)
        .with_order_time(time)
        .build_limit_order();
  }

  BetterOrderComparator comparator{Side::Option::Buy};

 private:
  OrderBuilder order_builder;
  OrderTime test_order_time{core::get_current_system_time()};
};

TEST_F(BetterBuyOrderComparator, LimitOrderWithHigherPriceIsBetter) {
  const auto left_order = make_limit_order(OrderPrice{42});
  const auto righ_order = make_limit_order(OrderPrice{41});

  ASSERT_TRUE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterBuyOrderComparator, LimitOrderWithLowerPriceIsWorse) {
  const auto left_order = make_limit_order(OrderPrice{41});
  const auto righ_order = make_limit_order(OrderPrice{42});

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterBuyOrderComparator, LimitOrderWithSamePriceAndLowerTimeIsBetter) {
  const auto left_order = make_limit_order(make_order_time(1s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_TRUE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterBuyOrderComparator, LimitOrderWithSamePriceAndHigherTimeIsWorse) {
  const auto left_order = make_limit_order(make_order_time(3s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterBuyOrderComparator, LimitOrderWithSamePriceAndSameTimeIsWorse) {
  const auto left_order = make_limit_order(make_order_time(2s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

class BetterSellOrderComparator : public Test {
  constexpr static auto DefaultPrice = OrderPrice{42};

 public:
  auto make_order_time(std::chrono::seconds increment) const -> OrderTime {
    return OrderTime{test_order_time.value() + increment};
  }

  auto make_limit_order(OrderPrice price) -> LimitOrder {
    return order_builder.with_order_price(price).build_limit_order();
  }

  auto make_limit_order(OrderTime time) -> LimitOrder {
    return order_builder.with_order_price(DefaultPrice)
        .with_order_time(time)
        .build_limit_order();
  }

  BetterOrderComparator comparator{Side::Option::Sell};

 private:
  OrderBuilder order_builder;
  OrderTime test_order_time{core::get_current_system_time()};
};

TEST_F(BetterSellOrderComparator, LimitOrderWithLowerPriceIsBetter) {
  const auto left_order = make_limit_order(OrderPrice{41});
  const auto righ_order = make_limit_order(OrderPrice{42});

  ASSERT_TRUE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterSellOrderComparator, LimitOrderWithHigherPriceIsWorse) {
  const auto left_order = make_limit_order(OrderPrice{42});
  const auto righ_order = make_limit_order(OrderPrice{41});

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterSellOrderComparator, LimitOrderWithSamePriceAndLowerTimeIsBetter) {
  const auto left_order = make_limit_order(make_order_time(1s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_TRUE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterSellOrderComparator, LimitOrderWithSamePriceAndHigherTimeIsWorse) {
  const auto left_order = make_limit_order(make_order_time(3s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

TEST_F(BetterSellOrderComparator, LimitOrderWithSamePriceAndSameTimeIsWorse) {
  const auto left_order = make_limit_order(make_order_time(3s));
  const auto righ_order = make_limit_order(make_order_time(2s));

  ASSERT_FALSE(comparator.is_better(left_order, righ_order));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test