#include <gmock/gmock.h>

#include <stdexcept>

#include "common/attributes.hpp"
#include "ih/orders/book/market_order.hpp"
#include "tools/order_test_tools.hpp"

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
  order.execute(ExecutedQuantity{5});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::PartiallyFilled));
}

TEST_F(MarketOrderEntry, HasFilledStatusOnceFullyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(MarketOrderEntry, HasFilledStatusOnceOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15});

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
  order.execute(ExecutedQuantity{3});
  order.execute(ExecutedQuantity{2});

  ASSERT_THAT(order.cum_executed_quantity(), Eq(CumExecutedQuantity{5}));
}

TEST_F(MarketOrderEntry, HasLeavesQtyEqualToTotalQtyWhenNotExecuted) {
  const auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.leaves_quantity(), Eq(LeavesQuantity{10}));
}

TEST_F(MarketOrderEntry, HasLeavesQtyEqualToTotalQtyMinusTotalExecutedQty) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{3});
  order.execute(ExecutedQuantity{3});

  ASSERT_THAT(order.leaves_quantity(), Eq(LeavesQuantity{4}));
}

TEST_F(MarketOrderEntry, HasZeroLeavesQtyWhenFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10});

  ASSERT_THAT(order.leaves_quantity(), LeavesQuantity{0});
}

TEST_F(MarketOrderEntry, HasZeroLeavesQtyWhenOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15});

  ASSERT_THAT(order.leaves_quantity(), LeavesQuantity{0});
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedOnceCreated) {
  auto order = make_order(OrderQuantity{10});

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedWhenPartiallyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{5});

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsNotExecutedWhenCancelled) {
  auto order = make_order(OrderQuantity{10});
  order.cancel();

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(MarketOrderEntry, ReportsThatIsExecutedWhenFullyFilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{10});

  ASSERT_THAT(order.executed(), IsTrue());
}

TEST_F(MarketOrderEntry, ReportsThatIsExecutedWhenOverfilled) {
  auto order = make_order(OrderQuantity{10});
  order.execute(ExecutedQuantity{15});

  ASSERT_THAT(order.executed(), IsTrue());
}

// NOLINTEND(*-magic-numbers)

}  // namespace simulator::trading_system::matching_engine::test