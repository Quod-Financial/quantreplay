#include <gmock/gmock.h>

#include "ih/orders/actions/elimination.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;             // NOLINT
using namespace std::chrono_literals;  // NOLINT

struct MatchingEngineClosedPhaseElimination : public Test {
  inline static const core::TzClock Timezone{"Europe/Kyiv"};
  static constexpr auto PhaseStartDate = core::sys_days{2025y / 12 / 30};

  MatchingEngineClosedPhaseElimination() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  auto make_eliminator(core::sys_days phase_start_date)
      -> ClosedPhaseElimination {
    return ClosedPhaseElimination{event_listener,
                                  core::as_tz_time(phase_start_date, Timezone),
                                  std::nullopt};
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;
};

struct MatchingEngineClosedPhaseEliminationDay
    : public MatchingEngineClosedPhaseElimination {
  auto make_eliminator() -> ClosedPhaseElimination {
    return MatchingEngineClosedPhaseElimination::make_eliminator(
        PhaseStartDate);
  }
};

TEST_F(MatchingEngineClosedPhaseEliminationDay, EliminatesAllBuyOrders) {
  order_book.buy_page().limit_orders().emplace(
      builder.with_order_id(OrderId{1})
          .with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order());
  order_book.buy_page().limit_orders().emplace(
      builder.with_order_id(OrderId{2})
          .with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationDay, EliminatesAllSellOrders) {
  order_book.sell_page().limit_orders().emplace(
      builder.with_order_id(OrderId{1})
          .with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order());
  order_book.sell_page().limit_orders().emplace(
      builder.with_order_id(OrderId{2})
          .with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order());

  const auto eliminator = make_eliminator();

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationDay,
       EmitsOrderRemovedNotificationOnEliminationBuyOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationDay,
       EmitsOrderRemovedNotificationOnEliminationSellOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationDay,
       EmitsOrderCancellationConfirmationOnEliminationBuyOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .with_client_order_id(ClientOrderId{"client-123"})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationDay,
       EmitsOrderCancellationConfirmationOnEliminationSellOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .with_client_order_id(ClientOrderId{"client-123"})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator();

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

struct MatchingEngineClosedPhaseEliminationGoodTillDate
    : public MatchingEngineClosedPhaseElimination {};

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       DoesNotEliminateBuyOrderWhenOrderExpireDateIsNullopt) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       DoesNotEliminateSellOrderWhenOrderExpireDateIsNullopt) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       DoesNotEliminateBuyOrderWhenPhaseStartDateIsLessThanOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator =
      make_eliminator(PhaseStartDate - std::chrono::days{1});

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       DoesNotEliminateSellOrderWhenPhaseStartDateIsLessThanOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator =
      make_eliminator(PhaseStartDate - std::chrono::days{1});

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EliminatesBuyOrderWhenPhaseStartDateIsEqualToOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EliminatesSellOrderWhenPhaseStartDateIsEqualToOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EliminatesBuyOrderWhenPhaseStartDateIsGreaterThanOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator =
      make_eliminator(PhaseStartDate + std::chrono::days{1});

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EliminatesSellOrderWhenPhaseStartDateIsGreaterThanOrderExpireDate) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator =
      make_eliminator(PhaseStartDate + std::chrono::days{1});

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EmitsOrderRemovedNotificationOnEliminationBuyOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EmitsOrderRemovedNotificationOnEliminationSellOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EmitsOrderCancellationConfirmationOnEliminationBuyOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .with_client_order_id(ClientOrderId{"client-123"})
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

TEST_F(MatchingEngineClosedPhaseEliminationGoodTillDate,
       EmitsOrderCancellationConfirmationOnEliminationSellOrder) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate{PhaseStartDate})
                         .with_client_order_id(ClientOrderId{"client-123"})
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(PhaseStartDate);

  EXPECT_CALL(event_listener,
              on(IsClientNotification(IsOrderCancellationConfirmation(
                  VenueOrderId{"123"},
                  OrderStatus::Option::Cancelled,
                  LeavesQuantity{420},
                  ClientOrderId{"client-123"}))));

  eliminator(order_book);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
