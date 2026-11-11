#include <gmock/gmock.h>

#include <chrono>
#include <stdexcept>

#include "common/attributes.hpp"
#include "core/tools/time.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/market_order.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {

using namespace ::testing;  // NOLINT
using namespace std::chrono_literals;

// NOLINTBEGIN(*-magic-numbers)

struct LimitOrderEntry : public Test {
  OrderBuilder builder;

  auto make_update(OrderPrice price, OrderQuantity quantity) const
      -> LimitOrder::Update {
    return LimitOrder::Update{.price = price,
                              .quantity = quantity,
                              .attributes = builder.build_order_attributes()};
  }

  auto make_order(OrderQuantity quantity) -> LimitOrder {
    return builder.with_order_quantity(quantity).build_limit_order();
  }

  static auto make_test_order_time() -> OrderTime {
    const auto sys_time = core::get_current_system_time();
    return OrderTime(sys_time - 1s);
  }
};

TEST_F(LimitOrderEntry, KeepsPriceAndQuantity) {
  const LimitOrder order{
      OrderPrice{1}, OrderQuantity{1}, builder.build_order_record()};

  EXPECT_THAT(order.price(), Eq(Price{1}));
  EXPECT_THAT(order.total_quantity(), Eq(Quantity{1}));
}

TEST_F(LimitOrderEntry, HasNewOrderStatusAfterCreation) {
  const auto order = builder.build_limit_order();

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::New));
}

TEST_F(LimitOrderEntry, SetsOrderTimeOnCreation) {
  const OrderTime min_order_time{core::get_current_system_time()};

  const auto order = builder.build_limit_order();

  ASSERT_THAT(order.time(), Ge(min_order_time));
}

TEST_F(LimitOrderEntry, StoresPriceWhenConstructedWithFillState) {
  const LimitOrder order{OrderPrice{7},
                         OrderQuantity{123},
                         builder.build_order_record(),
                         CumExecutedQuantity{50},
                         2500.0};

  EXPECT_THAT(order.price(), Eq(OrderPrice{7}));
}

TEST_F(LimitOrderEntry, StoresTotalQuantityWhenConstructedWithFillState) {
  const LimitOrder order{OrderPrice{7},
                         OrderQuantity{123},
                         builder.build_order_record(),
                         CumExecutedQuantity{50},
                         2500.0};

  EXPECT_THAT(order.total_quantity(), Eq(OrderQuantity{123}));
}

TEST_F(LimitOrderEntry, StoresRecordStatusWhenConstructedWithFillState) {
  OrderRecord record = builder.build_order_record();
  record.set_order_status(OrderStatus::Option::PartiallyFilled);

  const LimitOrder order{OrderPrice{1},
                         OrderQuantity{100},
                         std::move(record),
                         CumExecutedQuantity{50},
                         2500.0};

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::PartiallyFilled));
}

TEST_F(LimitOrderEntry, StoresCumExecutedQuantityWhenConstructedWithFillState) {
  const LimitOrder order{OrderPrice{7},
                         OrderQuantity{123},
                         builder.build_order_record(),
                         CumExecutedQuantity{50},
                         2500.0};

  EXPECT_THAT(order.cum_executed_quantity(), Eq(CumExecutedQuantity{50}));
}

TEST_F(LimitOrderEntry, StoresCumPriceQuantityWhenConstructedWithFillState) {
  const LimitOrder order{OrderPrice{7},
                         OrderQuantity{123},
                         builder.build_order_record(),
                         CumExecutedQuantity{50},
                         2500.0};

  ASSERT_THAT(order.cum_px_qty(), DoubleEq(2500.0));
}

TEST_F(LimitOrderEntry,
       CalculatesAveragePriceFromCumPxQtyAndCumExecutedQuantity) {
  const LimitOrder order{OrderPrice{1},
                         OrderQuantity{100},
                         builder.build_order_record(),
                         CumExecutedQuantity{50},
                         2500.0};

  EXPECT_THAT(order.average_price(), Optional(AveragePrice{50.0}));
}

TEST_F(LimitOrderEntry, ReturnsNoAveragePriceIfCumExecutedQuantityIsZero) {
  const LimitOrder order{OrderPrice{1},
                         OrderQuantity{100},
                         builder.build_order_record(),
                         CumExecutedQuantity{0},
                         2500.0};

  EXPECT_THAT(order.average_price(), Eq(std::nullopt));
}

TEST_F(LimitOrderEntry, ExecutesSpecifiedQuantity) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.cum_executed_quantity(), Eq(Quantity{0}));

  order.execute(ExecutedQuantity{50}, ExecutionPrice{10.0});

  ASSERT_THAT(order.cum_executed_quantity(), Eq(Quantity{50}));
}

TEST_F(LimitOrderEntry, UpdatesLeavesQuantityAfterExecution) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.leaves_quantity(), Eq(Quantity{100}));

  order.execute(ExecutedQuantity{50}, ExecutionPrice{10.0});

  ASSERT_THAT(order.leaves_quantity(), Eq(Quantity{50}));
}

TEST_F(LimitOrderEntry, KeepsTotalQuantityAfterExecution) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.total_quantity(), Eq(OrderQuantity{100}));

  order.execute(ExecutedQuantity{50}, ExecutionPrice{10.0});

  ASSERT_THAT(order.total_quantity(), Eq(OrderQuantity{100}));
}

TEST_F(LimitOrderEntry, AccumulatesCumQuantityWhenOverfilled) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.cum_executed_quantity(), Eq(Quantity{0}));

  order.execute(ExecutedQuantity{150}, ExecutionPrice{10.0});

  ASSERT_THAT(order.cum_executed_quantity(), Eq(Quantity{150}));
}

TEST_F(LimitOrderEntry, ReturnsZeroLeavesQuantityWhenOverfilled) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.leaves_quantity(), Eq(Quantity{100}));

  order.execute(ExecutedQuantity{150}, ExecutionPrice{10.0});

  ASSERT_THAT(order.leaves_quantity(), Eq(Quantity{0}));
}

TEST_F(LimitOrderEntry, ReportsThatNotExecutedWhenCumQuantityLessThanTotal) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{99}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsFalse());
}

TEST_F(LimitOrderEntry, ReportsThatExecutedWhenCumQuantityEqualsTotal) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{100}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsTrue());
}

TEST_F(LimitOrderEntry, ReportsThatExecutedWhenOverfilled) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{150}, ExecutionPrice{10.0});

  ASSERT_THAT(order.executed(), IsTrue());
}

TEST_F(LimitOrderEntry, SetsPartiallyFilledStatusWhenExecuted) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{50}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::PartiallyFilled));
}

TEST_F(LimitOrderEntry, SetsFilledStatusWhenFullyExecuted) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{100}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}

TEST_F(LimitOrderEntry, SetsFilledStatusWhenOverfilled) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{150}, ExecutionPrice{10.0});

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Filled));
}
TEST_F(LimitOrderEntry, HasNoAveragePriceWhenNoFills) {
  const auto order = make_order(OrderQuantity{100});

  ASSERT_THAT(order.average_price(), Eq(std::nullopt));
}

TEST_F(LimitOrderEntry, SetsAveragePriceAfterSingleFillWithPrice) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{50}, ExecutionPrice{42.5});

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{42.5})));
}

TEST_F(LimitOrderEntry, SetsWeightedAveragePriceAcrossMultipleFills) {
  auto order = make_order(OrderQuantity{100});

  order.execute(ExecutedQuantity{40}, ExecutionPrice{10.0});
  order.execute(ExecutedQuantity{60}, ExecutionPrice{15.0});

  ASSERT_THAT(order.average_price(), Optional(AveragePrice{13.0}));
}

TEST_F(LimitOrderEntry, AccumulatesCumPxQtyAcrossFills) {
  auto order = make_order(OrderQuantity{100});
  ASSERT_THAT(order.cum_px_qty(), DoubleEq(0.0));

  order.execute(ExecutedQuantity{40}, ExecutionPrice{10.0});
  order.execute(ExecutedQuantity{60}, ExecutionPrice{15.0});

  ASSERT_THAT(order.cum_px_qty(), DoubleEq(1300.0));
}

TEST_F(LimitOrderEntry, SetsModifiedStatusWhenAmended) {
  auto order = builder.build_limit_order();
  ASSERT_THAT(order.status(), Ne(OrderStatus::Option::Modified));

  order.amend(make_update(OrderPrice{2}, OrderQuantity{200}));

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Modified));
}

TEST_F(LimitOrderEntry, UpdatesOrderPriceAndQuantityWhenAmended) {
  auto order = builder.with_order_price(OrderPrice{5})
                   .with_order_quantity(OrderQuantity{500})
                   .build_limit_order();

  order.amend(make_update(OrderPrice{2}, OrderQuantity{200}));

  EXPECT_THAT(order.price(), Eq(Price{2}));
  EXPECT_THAT(order.total_quantity(), Eq(Quantity{200}));
}

TEST_F(LimitOrderEntry, ThrowsExceptionWhenNewQuantityLessThanExecuted) {
  auto order = make_order(OrderQuantity{100});
  order.execute(ExecutedQuantity{50}, ExecutionPrice{10.0});

  ASSERT_THROW(order.amend(make_update(OrderPrice{2}, OrderQuantity{40})),
               std::logic_error);
}

TEST_F(LimitOrderEntry, UpdatesOrderTimeWhenPriceChanged) {
  const auto orig_order_time = make_test_order_time();
  auto order = builder.with_order_price(OrderPrice{5})
                   .with_order_quantity(OrderQuantity{200})
                   .with_order_time(orig_order_time)
                   .build_limit_order();

  order.amend(make_update(OrderPrice{2}, OrderQuantity{200}));

  ASSERT_THAT(order.time(), Gt(orig_order_time));
}

TEST_F(LimitOrderEntry, UpdatesOrderTimeWhenQuantityIncreased) {
  const auto orig_order_time = make_test_order_time();
  auto order = builder.with_order_price(OrderPrice{5})
                   .with_order_quantity(OrderQuantity{200})
                   .with_order_time(orig_order_time)
                   .build_limit_order();

  order.amend(make_update(OrderPrice{5}, OrderQuantity{300}));

  ASSERT_THAT(order.time(), Gt(orig_order_time));
}

TEST_F(LimitOrderEntry, KeepsOrderTimeWhenQuantityDecreased) {
  const auto orig_order_time = make_test_order_time();
  auto order = builder.with_order_price(OrderPrice{5})
                   .with_order_quantity(OrderQuantity{200})
                   .with_order_time(orig_order_time)
                   .build_limit_order();

  order.amend(make_update(OrderPrice{5}, OrderQuantity{100}));

  ASSERT_THAT(order.time(), Eq(orig_order_time));
}

TEST_F(LimitOrderEntry, KeepsOrderTimeWhenPriceAndQuantityUnchanged) {
  const auto orig_order_time = make_test_order_time();
  auto order = builder.with_order_price(OrderPrice{5})
                   .with_order_quantity(OrderQuantity{200})
                   .with_order_time(orig_order_time)
                   .build_limit_order();

  order.amend(make_update(OrderPrice{5}, OrderQuantity{200}));

  ASSERT_THAT(order.time(), Eq(orig_order_time));
}

TEST_F(LimitOrderEntry, SetsCancelledStatusWhenCancelled) {
  auto order = builder.build_limit_order();
  ASSERT_THAT(order.status(), Ne(OrderStatus::Option::Cancelled));

  order.cancel();

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Cancelled));
}

TEST_F(LimitOrderEntry, DetectsOrderOwnerFromAttributes) {
  const Party owner{PartyId{"QUOD"},
                    PartyIdSource::Option::Proprietary,
                    PartyRole::Option::ExecutingFirm};

  const auto order = builder.with_order_parties({owner}).build_limit_order();

  ASSERT_THAT(order.owner(), Optional(Eq(owner)));
}

TEST_F(LimitOrderEntry, DetectsFirstExecutingFirmPartyAsOrderOwner) {
  const Party executing_firm_1{PartyId{"QUOD"},
                               PartyIdSource::Option::Proprietary,
                               PartyRole::Option::ExecutingFirm};
  const Party executing_firm_2{PartyId{"XTX"},
                               PartyIdSource::Option::Proprietary,
                               PartyRole::Option::ExecutingFirm};

  const auto order =
      builder.with_order_parties({executing_firm_1, executing_firm_2})
          .build_limit_order();

  ASSERT_THAT(order.owner(), Optional(Eq(executing_firm_1)));
}

TEST_F(LimitOrderEntry, DoesNotReturnOwnerWhenNoExecutingFirmIsSpecified) {
  const Party exchange{
      PartyId{"XLON"}, PartyIdSource::Option::MIC, PartyRole::Option::Exchange};

  const auto order = builder.with_order_parties({exchange}).build_limit_order();

  ASSERT_THAT(order.owner(), std::nullopt);
}

struct LimitOrderConversion : public Test {
  static constexpr OrderTime Arrival{
      core::sys_us{core::sys_days{2025y / 1 / 25} + 13h + 14min + 1s}};

  static auto resting_market_order(OrderQuantity quantity) -> MarketOrder {
    return OrderBuilder{}
        .with_order_id(OrderId{42})
        .with_order_quantity(quantity)
        .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
        .with_order_time(Arrival)
        .build_market_order();
  }

  static auto update_to(OrderPrice price, OrderQuantity quantity)
      -> LimitOrder::Update {
    OrderAttributes attributes;
    attributes.set_client_order_id(ClientOrderId{"amended"});
    attributes.set_time_in_force(TimeInForce::Option::Day);
    return LimitOrder::Update{
        .price = price, .quantity = quantity, .attributes = attributes};
  }
};

TEST_F(LimitOrderConversion, ThrowsWhenQuantityNotAboveExecutedQuantity) {
  auto market_order = resting_market_order(OrderQuantity{100});
  market_order.execute(ExecutedQuantity{50}, ExecutionPrice{10});

  ASSERT_THROW(
      (LimitOrder{market_order, update_to(OrderPrice{10}, OrderQuantity{50})}),
      std::logic_error);
}

TEST_F(LimitOrderConversion, KeepsOrderIdOfMarketOrder) {
  const LimitOrder order{resting_market_order(OrderQuantity{100}),
                         update_to(OrderPrice{10}, OrderQuantity{100})};

  ASSERT_THAT(order.id(), Eq(OrderId{42}));
}

TEST_F(LimitOrderConversion, ContinuesExecutionIdSequenceOfMarketOrder) {
  auto market_order = resting_market_order(OrderQuantity{100});
  const auto market_execution_id = market_order.make_execution_id();

  LimitOrder order{market_order, update_to(OrderPrice{10}, OrderQuantity{100})};

  ASSERT_THAT(order.make_execution_id(), Ne(market_execution_id));
}

TEST_F(LimitOrderConversion, KeepsExecutedQuantityOfMarketOrder) {
  auto market_order = resting_market_order(OrderQuantity{100});
  market_order.execute(ExecutedQuantity{30}, ExecutionPrice{10});

  const LimitOrder order{market_order,
                         update_to(OrderPrice{10}, OrderQuantity{100})};

  ASSERT_THAT(order.cum_executed_quantity(), Eq(CumExecutedQuantity{30}));
}

TEST_F(LimitOrderConversion, KeepsAveragePriceOfMarketOrder) {
  auto market_order = resting_market_order(OrderQuantity{100});
  market_order.execute(ExecutedQuantity{20}, ExecutionPrice{10});
  market_order.execute(ExecutedQuantity{20}, ExecutionPrice{12});

  const LimitOrder order{market_order,
                         update_to(OrderPrice{10}, OrderQuantity{100})};

  ASSERT_THAT(order.average_price(), Optional(Eq(AveragePrice{11})));
}

TEST_F(LimitOrderConversion, TakesAmendedPriceAndQuantity) {
  const LimitOrder order{resting_market_order(OrderQuantity{100}),
                         update_to(OrderPrice{7}, OrderQuantity{50})};

  EXPECT_THAT(order.price(), Eq(OrderPrice{7}));
  EXPECT_THAT(order.total_quantity(), Eq(OrderQuantity{50}));
}

TEST_F(LimitOrderConversion, TakesAmendedAttributes) {
  const LimitOrder order{resting_market_order(OrderQuantity{100}),
                         update_to(OrderPrice{10}, OrderQuantity{100})};

  EXPECT_THAT(order.client_order_id(), Optional(Eq(ClientOrderId{"amended"})));
  EXPECT_THAT(order.time_in_force(), Eq(TimeInForce::Option::Day));
}

TEST_F(LimitOrderConversion, SetsModifiedStatus) {
  const LimitOrder order{resting_market_order(OrderQuantity{100}),
                         update_to(OrderPrice{10}, OrderQuantity{100})};

  ASSERT_THAT(order.status(), Eq(OrderStatus::Option::Modified));
}

TEST_F(LimitOrderConversion, LosesTimePriorityEvenWhenQuantityDecreases) {
  const LimitOrder order{resting_market_order(OrderQuantity{100}),
                         update_to(OrderPrice{10}, OrderQuantity{50})};

  ASSERT_THAT(order.time(), Gt(Arrival));
}

// NOLINTEND(*-magic-numbers)

}  // namespace simulator::trading_system::matching_engine::test