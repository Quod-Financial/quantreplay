#include <gmock/gmock.h>

#include <optional>
#include <utility>

#include "core/tools/price_tick_ranges.hpp"
#include "ih/orders/actions/auction_placement.hpp"
#include "ih/orders/actions/order_type_conversion.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "tests/mocks/event_listener_mock.hpp"
#include "tools/matchers.hpp"
#include "tools/order_builder.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct MatchingEngineOrderTypeConversion : public Test {
  MatchingEngineOrderTypeConversion() {
    EXPECT_CALL(event_listener, on(_)).Times(AnyNumber());
  }

  NiceMock<EventListenerMock> event_listener;
  OrderBook order_book;
  const core::PriceTickRanges no_price_tick_ranges;
  OrderTypeConversion conversion{event_listener, no_price_tick_ranges};

  auto resting_limit_orders() -> LimitOrdersContainer& {
    return order_book.buy_page().limit_orders();
  }

  auto resting_market_orders() -> MarketOrdersContainer& {
    return order_book.buy_page().market_orders();
  }
};

struct MatchingEngineOrderTypeConversionToMarket
    : public MatchingEngineOrderTypeConversion {
  auto rest_limit_order(OrderQuantity quantity,
                        CumExecutedQuantity executed = CumExecutedQuantity{0})
      -> void {
    auto order = OrderBuilder{}
                     .with_order_id(OrderId{42})
                     .with_side(Side::Option::Buy)
                     .with_client_order_id(ClientOrderId{"original"})
                     .with_order_price(OrderPrice{10})
                     .with_order_quantity(quantity)
                     .with_time_in_force(TimeInForce::Option::GoodTillCancel)
                     .build_limit_order();
    if (static_cast<double>(executed) > 0.0) {
      order.execute(ExecutedQuantity{static_cast<double>(executed)},
                    ExecutionPrice{10});
    }
    resting_limit_orders().emplace(order);
  }

  auto convert(MarketUpdate update)
      -> std::optional<ConvertedOrder<MarketOrder>> {
    return conversion.to_market_order(std::move(update),
                                      resting_limit_orders());
  }

  static auto amend_to_market(OrderQuantity quantity,
                              OrderId order_id = OrderId{42}) -> MarketUpdate {
    OrderAttributes attributes;
    attributes.set_client_order_id(ClientOrderId{"amended"});
    attributes.set_time_in_force(TimeInForce::Option::ImmediateOrCancel);
    MarketUpdate update{
        protocol::Session{protocol::generator::Session{}},
        Side::Option::Buy,
        MarketOrder::Update{.quantity = quantity,
                            .attributes = std::move(attributes)}};
    update.order_id = order_id;
    update.orig_client_order_id = OrigClientOrderId{"original"};
    return update;
  }

  static auto amend_unknown_order_to_market() -> MarketUpdate {
    return amend_to_market(OrderQuantity{100}, OrderId{99});
  }

  static auto amend_to_market_with_time_in_force(OrderQuantity quantity)
      -> MarketUpdate {
    auto update = amend_to_market(quantity);
    update.time_in_force_specified = true;
    return update;
  }
};

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       RejectsAmendmentOfUnknownOrderAsOrderNotFound) {
  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  "order not found", OrderStatus::Option::Rejected)));

  convert(amend_unknown_order_to_market());
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       RejectsAmendmentWithQuantityNotAboveExecutedQuantity) {
  rest_limit_order(OrderQuantity{100}, CumExecutedQuantity{50});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  "invalid quantity", OrderStatus::Option::PartiallyFilled)));

  convert(amend_to_market(OrderQuantity{50}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       KeepsLimitOrderRestingUntouchedWhenQuantityIsInvalid) {
  rest_limit_order(OrderQuantity{100}, CumExecutedQuantity{50});

  convert(amend_to_market(OrderQuantity{50}));

  EXPECT_THAT(resting_limit_orders(),
              ElementsAre(AllOf(
                  Property(&LimitOrder::price, Eq(OrderPrice{10})),
                  Property(&LimitOrder::total_quantity, Eq(OrderQuantity{100})),
                  Property(&LimitOrder::status,
                           Eq(OrderStatus::Option::PartiallyFilled)))));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       RejectsAmendmentSpecifyingTimeInForce) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  TimeInForceNotAllowedReject, OrderStatus::Option::New)));

  convert(amend_to_market_with_time_in_force(OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       KeepsLimitOrderRestingUntouchedWhenTimeInForceIsSpecified) {
  rest_limit_order(OrderQuantity{100});

  convert(amend_to_market_with_time_in_force(OrderQuantity{100}));

  EXPECT_THAT(resting_limit_orders(),
              ElementsAre(AllOf(
                  Property(&LimitOrder::price, Eq(OrderPrice{10})),
                  Property(&LimitOrder::total_quantity, Eq(OrderQuantity{100})),
                  Property(&LimitOrder::status, Eq(OrderStatus::Option::New)),
                  Property(&LimitOrder::time_in_force,
                           Eq(TimeInForce::Option::GoodTillCancel)))));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       RejectsInvalidQuantityWhenTimeInForceIsAlsoSpecified) {
  rest_limit_order(OrderQuantity{100}, CumExecutedQuantity{50});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  "invalid quantity", OrderStatus::Option::PartiallyFilled)));

  convert(amend_to_market_with_time_in_force(OrderQuantity{50}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       ConfirmsAmendmentAsMarketOrder) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderModificationConfirmation>(AllOf(
              Field(&protocol::OrderModificationConfirmation::order_type,
                    Optional(Eq(OrderType::Option::Market))),
              Field(&protocol::OrderModificationConfirmation::order_price,
                    Eq(std::nullopt)),
              Field(&protocol::OrderModificationConfirmation::order_status,
                    Optional(Eq(OrderStatus::Option::Modified))))))));

  convert(amend_to_market(OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       ConfirmsAmendmentWithNewClientOrderId) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<
                              protocol::OrderModificationConfirmation>(AllOf(
          Field(&protocol::OrderModificationConfirmation::client_order_id,
                Optional(Eq(ClientOrderId{"amended"}))),
          Field(&protocol::OrderModificationConfirmation::orig_client_order_id,
                Optional(Eq(OrigClientOrderId{"original"}))))))));

  convert(amend_to_market(OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       ConfirmsAmendmentWithExecutionsOfPartiallyFilledLimitOrder) {
  rest_limit_order(OrderQuantity{100}, CumExecutedQuantity{40});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(VariantWith<
                              protocol::OrderModificationConfirmation>(AllOf(
          Field(&protocol::OrderModificationConfirmation::cum_executed_quantity,
                Optional(Eq(CumExecutedQuantity{40}))),
          Field(&protocol::OrderModificationConfirmation::average_price,
                Optional(Eq(AveragePrice{10}))),
          Field(&protocol::OrderModificationConfirmation::leaving_quantity,
                Optional(Eq(LeavesQuantity{60}))))))));

  convert(amend_to_market(OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderTypeConversionToMarket,
       EmitsOrderRemovedNotificationForLimitOrder) {
  rest_limit_order(OrderQuantity{100});

  EXPECT_CALL(
      event_listener,
      on(IsOrderBookNotification(VariantWith<OrderRemoved>(
          AllOf(Field(&OrderRemoved::order_id, Eq(OrderId{42})),
                Field(&OrderRemoved::order_price, Optional(Eq(Price{10}))))))));

  convert(amend_to_market(OrderQuantity{100}));
}

struct MatchingEngineOrderTypeConversionToLimit
    : public MatchingEngineOrderTypeConversion {
  auto rest_market_order(OrderQuantity quantity,
                         CumExecutedQuantity executed = CumExecutedQuantity{0})
      -> void {
    auto order = OrderBuilder{}
                     .with_order_id(OrderId{42})
                     .with_side(Side::Option::Buy)
                     .with_client_order_id(ClientOrderId{"original"})
                     .with_order_quantity(quantity)
                     .with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
                     .build_market_order();
    if (static_cast<double>(executed) > 0.0) {
      order.execute(ExecutedQuantity{static_cast<double>(executed)},
                    ExecutionPrice{10});
    }
    resting_market_orders().emplace(order);
  }

  auto convert(LimitUpdate update)
      -> std::optional<ConvertedOrder<LimitOrder>> {
    return conversion.to_auction_limit_order(std::move(update),
                                             resting_market_orders());
  }

  static auto amend_to_limit(OrderQuantity quantity,
                             std::optional<TimeInForce> time_in_force)
      -> LimitUpdate {
    OrderAttributes attributes;
    attributes.set_client_order_id(ClientOrderId{"amended"});
    attributes.set_time_in_force(
        time_in_force.value_or(TimeInForce::Option::Day));
    LimitUpdate update{protocol::Session{protocol::generator::Session{}},
                       Side::Option::Buy,
                       LimitOrder::Update{.price = OrderPrice{20},
                                          .quantity = quantity,
                                          .attributes = std::move(attributes)}};
    update.order_id = OrderId{42};
    update.orig_client_order_id = OrigClientOrderId{"original"};
    update.time_in_force_specified = time_in_force.has_value();
    return update;
  }

  static auto amend_to_limit(TimeInForce time_in_force) -> LimitUpdate {
    return amend_to_limit(OrderQuantity{100}, time_in_force);
  }

  static auto amend_to_limit_without_time_in_force(OrderQuantity quantity)
      -> LimitUpdate {
    return amend_to_limit(quantity, std::nullopt);
  }
};

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       RejectsAmendmentWithQuantityNotAboveExecutedQuantity) {
  rest_market_order(OrderQuantity{100}, CumExecutedQuantity{50});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  "invalid quantity", OrderStatus::Option::PartiallyFilled)));

  convert(amend_to_limit(OrderQuantity{50}, TimeInForce::Option::Day));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       KeepsMarketOrderRestingUntouchedWhenQuantityIsInvalid) {
  rest_market_order(OrderQuantity{100}, CumExecutedQuantity{50});

  convert(amend_to_limit(OrderQuantity{50}, TimeInForce::Option::Day));

  EXPECT_THAT(
      resting_market_orders(),
      ElementsAre(
          AllOf(Property(&MarketOrder::total_quantity, Eq(OrderQuantity{100})),
                Property(&MarketOrder::status,
                         Eq(OrderStatus::Option::PartiallyFilled)))));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       RejectsAmendmentWithoutTimeInForce) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  TimeInForceMissingReject, OrderStatus::Option::New)));

  convert(amend_to_limit_without_time_in_force(OrderQuantity{100}));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       RejectsImmediateOrCancelAmendment) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  auction::ImmediateOrdersNotAcceptedReject,
                  OrderStatus::Option::New)));

  convert(amend_to_limit(TimeInForce::Option::ImmediateOrCancel));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit, RejectsFillOrKillAmendment) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  auction::ImmediateOrdersNotAcceptedReject,
                  OrderStatus::Option::New)));

  convert(amend_to_limit(TimeInForce::Option::FillOrKill));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       RejectsInvalidQuantityWhenTimeInForceIsAlsoMissing) {
  rest_market_order(OrderQuantity{100}, CumExecutedQuantity{50});

  EXPECT_CALL(event_listener,
              on(IsOrderModificationRejectWithStatus(
                  "invalid quantity", OrderStatus::Option::PartiallyFilled)));

  convert(amend_to_limit_without_time_in_force(OrderQuantity{50}));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       ConfirmsAmendmentAsLimitOrderAtAmendedPrice) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(
      event_listener,
      on(IsClientNotification(
          VariantWith<protocol::OrderModificationConfirmation>(AllOf(
              Field(&protocol::OrderModificationConfirmation::order_type,
                    Optional(Eq(OrderType::Option::Limit))),
              Field(&protocol::OrderModificationConfirmation::order_price,
                    Optional(Eq(OrderPrice{20}))),
              Field(&protocol::OrderModificationConfirmation::order_status,
                    Optional(Eq(OrderStatus::Option::Modified))))))));

  convert(amend_to_limit(TimeInForce::Option::Day));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       ConfirmsAmendmentWithRequestedTimeInForce) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsClientNotification(
                  VariantWith<protocol::OrderModificationConfirmation>(Field(
                      &protocol::OrderModificationConfirmation::time_in_force,
                      Optional(Eq(TimeInForce::Option::GoodTillCancel)))))));

  convert(amend_to_limit(TimeInForce::Option::GoodTillCancel));
}

TEST_F(MatchingEngineOrderTypeConversionToLimit,
       EmitsOrderRemovedNotificationWithoutPriceForMarketOrder) {
  rest_market_order(OrderQuantity{100});

  EXPECT_CALL(event_listener,
              on(IsOrderBookNotification(VariantWith<OrderRemoved>(AllOf(
                  Field(&OrderRemoved::order_id, Eq(OrderId{42})),
                  Field(&OrderRemoved::order_price, Eq(std::nullopt)))))));

  convert(amend_to_limit(TimeInForce::Option::Day));
}

struct MatchingEngineOrderTypeConversionToLimitWithRejectedTimeInForce
    : public MatchingEngineOrderTypeConversionToLimit,
      public WithParamInterface<std::optional<TimeInForce>> {};

TEST_P(MatchingEngineOrderTypeConversionToLimitWithRejectedTimeInForce,
       KeepsMarketOrderRestingUntouched) {
  rest_market_order(OrderQuantity{100});

  convert(amend_to_limit(OrderQuantity{100}, GetParam()));

  EXPECT_THAT(
      resting_market_orders(),
      ElementsAre(
          AllOf(Property(&MarketOrder::total_quantity, Eq(OrderQuantity{100})),
                Property(&MarketOrder::status, Eq(OrderStatus::Option::New)),
                Property(&MarketOrder::time_in_force,
                         Eq(TimeInForce::Option::ImmediateOrCancel)))));
}

INSTANTIATE_TEST_SUITE_P(
    RejectedTimeInForce,
    MatchingEngineOrderTypeConversionToLimitWithRejectedTimeInForce,
    Values(
        std::nullopt,
        std::make_optional<TimeInForce>(TimeInForce::Option::ImmediateOrCancel),
        std::make_optional<TimeInForce>(TimeInForce::Option::FillOrKill)));

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test
