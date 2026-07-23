#include <gmock/gmock.h>

#include <stdexcept>

#include "core/tools/time.hpp"
#include "ih/orders/book/market_order.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

// NOLINTBEGIN(*-magic-numbers)

struct MarketOrderEntry : public Test {
  OrderBuilder builder;

  auto make_order(OrderQuantity quantity) -> MarketOrder {
    return builder.with_order_quantity(quantity).build_market_order();
  }
};

TEST_F(MarketOrderEntry, HasNewStatusOnceCreated) {
  const auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::New));
}

TEST_F(MarketOrderEntry, HasPartiallyFilledStatusOncePartiallyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{5}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::PartiallyFilled));
}

TEST_F(MarketOrderEntry, HasFilledStatusOnceFullyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(MarketOrderEntry, HasFilledStatusOnceOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(MarketOrderEntry, HasCancelledStatusOnceCancelled) {
  auto order = make_order(OrderQuantity{10});
  order.cancel();

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
}

TEST_F(MarketOrderEntry, KeepsEnteredTotalQty) {
  const auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.total_quantity(), Eq(OrderQuantity{10}));
}

TEST_F(MarketOrderEntry, KeepsCumExecutedQty) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{3}, ExecutionPrice{10.0});
  order.execute(ExecutedQuantity{2}, ExecutionPrice{10.0});

  ASSERT_THAT(order.cum_executed_quantity(), Eq(CumExecutedQuantity{5}));
}

TEST_F(MarketOrderEntry, HasLeavesQtyEqualToTotalQtyWhenNotExecuted) {
  const auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.leaves_quantity(), Eq(LeavesQuantity{10}));
}

TEST_F(MarketOrderEntry, HasLeavesQtyEqualToTotalQtyMinusTotalExecutedQty) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{3}, ExecutionPrice{10.0});
  order.execute(ExecutedQuantity{3}, ExecutionPrice{10.0});

  ASSERT_THAT(order.leaves_quantity(), Eq(LeavesQuantity{4}));
}

TEST_F(MarketOrderEntry, HasZeroLeavesQtyWhenFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10}, ExecutionPrice{10.0});

  ASSERT_THAT(order.leaves_quantity(), LeavesQuantity{0});
}

TEST_F(MarketOrderEntry, HasZeroLeavesQtyWhenOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15}, ExecutionPrice{10.0});

  ASSERT_THAT(order.leaves_quantity(), LeavesQuantity{0});
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedOnceCreated) {
  auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedWhenPartiallyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{5}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedWhenCancelled) {
  auto order = make_order(OrderQuantity{10});
  order.cancel();

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsExecutedWhenFullyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsTrue());
}

TEST_F(MarketOrderEntry, ReportsThatIsExecutedWhenOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsTrue());
}

TEST_F(MarketOrderEntry, HasNoAveragePriceWhenNoFills) {
  const auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.average_price(), Eq(std::nullopt));
}

TEST_F(MarketOrderEntry, SetsAveragePriceAfterSingleFillWithPrice) {
  auto order = make_order(OrderQuantity{10});

  order.execute(ExecutedQuantity{5}, ExecutionPrice{42.5});

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{42.5})));
}

TEST_F(MarketOrderEntry, SetsWeightedAveragePriceAcrossMultipleFills) {
  auto order = make_order(OrderQuantity{10});

  order.execute(ExecutedQuantity{4}, ExecutionPrice{10.0});
  order.execute(ExecutedQuantity{6}, ExecutionPrice{15.0});

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{13.0})));
}

TEST_F(MarketOrderEntry, KeepsArrivalTime) {
  const OrderTime arrival{core::sys_us{core::sys_days{2025y / 1 / 25} + 13h +
                                       14min + 1s + 123456us}};
  const auto order = builder.with_order_time(arrival).build_market_order();

  ASSERT_THAT(order.time(), Eq(arrival));
}

struct MarketOrderAmendment : public Test {
  static constexpr OrderTime Arrival{
      core::sys_us{core::sys_days{2025y / 1 / 25} + 13h + 14min + 1s}};

  OrderBuilder builder;

  auto make_order(OrderQuantity quantity) -> MarketOrder {
    return builder.with_order_quantity(quantity)
        .with_order_time(Arrival)
        .build_market_order();
  }
};

TEST_F(MarketOrderAmendment, UpdatesTotalQuantity) {
  auto order = make_order(OrderQuantity{10});

  order.amend(MarketOrder::Update{.quantity = OrderQuantity{20}});

  ASSERT_THAT(order.total_quantity(), Eq(OrderQuantity{20}));
}

TEST_F(MarketOrderAmendment, SetsModifiedStatus) {
  auto order = make_order(OrderQuantity{10});

  order.amend(MarketOrder::Update{.quantity = OrderQuantity{20}});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Modified));
}

TEST_F(MarketOrderAmendment, ResetsArrivalTimeWhenQuantityIncreases) {
  auto order = make_order(OrderQuantity{10});

  order.amend(MarketOrder::Update{.quantity = OrderQuantity{20}});

  ASSERT_THAT(order.time(), Ne(Arrival));
}

TEST_F(MarketOrderAmendment, KeepsArrivalTimeWhenQuantityDecreases) {
  auto order = make_order(OrderQuantity{10});

  order.amend(MarketOrder::Update{.quantity = OrderQuantity{5}});

  ASSERT_THAT(order.time(), Eq(Arrival));
}

TEST_F(MarketOrderAmendment, ThrowsWhenQuantityNotAboveExecutedQuantity) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{5}, ExecutionPrice{10.0});

  ASSERT_THROW(order.amend(MarketOrder::Update{.quantity = OrderQuantity{5}}),
               std::logic_error);
}

// NOLINTEND(*-magic-numbers)

}  // namespace simulator::trading_system::matching_engine::test