#include <gmock/gmock.h>

#include "ih/orders/actions/elimination.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tests/tools/matchers.hpp"
#include "tests/tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

struct MatchingEngineSystemElimination : public Test {
  inline static const core::TzClock Timezone{"Europe/Kyiv"};

  auto make_eliminator(event::Tick tick) -> SystemElimination {
    return SystemElimination{event_listener, std::move(tick)};
  }

  static auto default_tick() -> event::Tick {
    return event::Tick{.sys_tick_time = core::get_current_system_time(),
                       .tz_tick_time = core::get_current_tz_time(Timezone),
                       .is_new_sys_day = false,
                       .is_new_tz_day = false};
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBuilder builder;
  OrderBook order_book;
};

struct MatchingEngineSystemEliminationDay
    : public MatchingEngineSystemElimination {};

TEST_F(MatchingEngineSystemEliminationDay,
       DoesNotEliminateBuyOrderWhenTimezoneDayIsNotChanged) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = false;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineSystemEliminationDay,
       DoesNotEliminateSellOrderWhenTimezoneDayIsNotChanged) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = false;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineSystemEliminationDay,
       EliminatesBuyOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderRemovedNotificationOnEliminationBuyOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderCancellationConfirmationOnEliminationBuyOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineSystemEliminationDay,
       EliminatesSellOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderRemovedNotificationOnEliminationSellOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderCancellationConfirmationOnEliminationSellOrderWhenTimezoneDayIsChanged) {
  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::Day)
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

struct MatchingEngineSystemEliminationGoodTillDateExpireTime
    : public MatchingEngineSystemElimination {};

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       DoesNotEliminateBuyOrderWhenExpireTimeIsGreaterThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time + 1h))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       DoesNotEliminateSellOrderWhenExpireTimeIsGreaterThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time + 1h))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       EliminatesBuyOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderRemovedNotificationOnEliminationBuyOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderCancellationConfirmationOnEliminationBuyOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       EliminatesSellOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderRemovedNotificationOnEliminationSellOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderCancellationConfirmationOnEliminationSellOrderWhenExpireTimeIsEqualToUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       EliminatesBuyOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderRemovedNotificationOnEliminationBuyOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderCancellationConfirmationOnEliminationBuyOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireTime,
       EliminatesSellOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderRemovedNotificationOnEliminationSellOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireTime,
    EmitsOrderCancellationConfirmationOnEliminationSellOrderWhenExpireTimeIsLessThanUtcTime) {
  using namespace std::chrono_literals;
  auto tick = default_tick();
  tick.sys_tick_time = core::sys_us{core::sys_days{2025y / 12 / 30} + 13h +
                                    30min + 59s + 123456us};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_time(ExpireTime(tick.sys_tick_time - 1s))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

struct MatchingEngineSystemEliminationGoodTillDateExpireDate
    : public MatchingEngineSystemElimination {};

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireDate,
       DoesNotEliminateBuyOrderWhenTimezoneDayIsNotChanged) {
  using namespace std::chrono_literals;

  const auto order =
      builder.with_side(Side{Side::Option::Buy})
          .with_time_in_force(TimeInForce::Option::GoodTillDate)
          .with_expire_date(ExpireDate(core::sys_days{2025y / 12 / 30}))
          .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = false;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(MatchingEngineSystemEliminationGoodTillDateExpireDate,
       DoesNotEliminateSellOrderWhenTimezoneDayIsNotChanged) {
  using namespace std::chrono_literals;

  const auto order =
      builder.with_side(Side{Side::Option::Sell})
          .with_time_in_force(TimeInForce::Option::GoodTillDate)
          .with_expire_date(ExpireDate(core::sys_days{2025y / 12 / 30}))
          .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = false;
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    DoesNotEliminateBuyOrderWhenTimezoneDayIsChangedAndExpireDateIsGreaterThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date - std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    DoesNotEliminateSellOrderWhenTimezoneDayIsChangedAndExpireDateIsGreaterThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date - std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    DoesNotEliminateBuyOrderWhenTimezoneDayIsChangedAndExpireDateIsEqualToTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time = core::as_tz_time(expire_date, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    DoesNotEliminateSellOrderWhenTimezoneDayIsChangedAndExpireDateIsEqualToTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time = core::as_tz_time(expire_date, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_FALSE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    EliminatesBuyOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.buy_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    EmitsOrderRemovedNotificationOnEliminationBuyOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderCancellationConfirmationOnEliminationBuyOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Buy})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.buy_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    EliminatesSellOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  eliminator(order_book);

  ASSERT_TRUE(order_book.sell_page().limit_orders().empty());
}

TEST_F(
    MatchingEngineSystemEliminationGoodTillDateExpireDate,
    EmitsOrderRemovedNotificationOnEliminationSellOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(
                  Field(&OrderRemoved::order_id, Eq(OrderId{123}))))));

  eliminator(order_book);
}

TEST_F(
    MatchingEngineSystemEliminationDay,
    EmitsOrderCancellationConfirmationOnEliminationSellOrderWhenTimezoneDayIsChangedAndExpireDateIsLessThanTimezoneDate) {
  using namespace std::chrono_literals;
  constexpr auto expire_date = core::sys_days{2025y / 12 / 30};

  const auto order = builder.with_order_id(OrderId{123})
                         .with_side(Side{Side::Option::Sell})
                         .with_time_in_force(TimeInForce::Option::GoodTillDate)
                         .with_expire_date(ExpireDate(expire_date))
                         .build_limit_order();
  order_book.sell_page().limit_orders().emplace(order);

  auto tick = default_tick();
  tick.is_new_tz_day = true;
  tick.tz_tick_time =
      core::as_tz_time(expire_date + std::chrono::days{1}, Timezone);
  const auto eliminator = make_eliminator(tick);

  EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderCancellationConfirmation>(Field(
                      &protocol::OrderCancellationConfirmation::venue_order_id,
                      Eq(VenueOrderId{"123"}))))));

  eliminator(order_book);
}

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
