#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "common/market_state/snapshot.hpp"
#include "ih/orders/book/order_book.hpp"
#include "ih/orders/validation/client_request_validator.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "tools/protocol_test_tools.hpp"

namespace simulator::trading_system::matching_engine::order::test {
namespace {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct ValidationFixture {
  template <typename RequestType>
  auto validate(const RequestType& request) -> Conclusion {
    return validator.validate(request);
  }

  auto set_min_quantity(MinQuantity quantity) -> void {
    config.min_quantity = quantity;
    validator.configure(config);
  }

  auto set_max_quantity(MaxQuantity quantity) -> void {
    config.max_quantity = quantity;
    validator.configure(config);
  }

  auto set_quantity_tick(QuantityTick tick) -> void {
    config.quantity_tick = tick;
    validator.configure(config);
  }

  auto set_price_tick(PriceTick tick) -> void {
    config.price_tick = tick;
    validator.configure(config);
  }

 protected:
  ClientRequestValidator validator;

 private:
  ClientRequestValidator::Config config;
};

MATCHER_P(IsError, error_message, "") {
  return ExplainMatchResult(IsTrue(), arg.failed(), result_listener) &&
         ExplainMatchResult(Eq(error_message), arg.error(), result_listener);
}

/*----------------------------------------------------------------------------*/

struct OrderPlacementRequestValidation : public Test, public ValidationFixture {
  protocol::OrderPlacementRequest request =
      make_message<protocol::OrderPlacementRequest>();

  OrderPlacementRequestValidation() {
    request.side = Side::Option::Buy;
    request.order_type = OrderType::Option::Market;
    request.order_quantity = OrderQuantity{100};
  }
};

TEST_F(OrderPlacementRequestValidation, FailsWhenSideUnspecified) {
  request.side = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("side missing"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenSideInvalid) {
  request.side = static_cast<Side::Option>(0xFF);

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("unsupported side value"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderTypeUnspecified) {
  request.order_type = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order type missing"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderTypeInvalid) {
  request.order_type = static_cast<OrderType::Option>(0xFF);

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("unsupported order type value"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderQuantityUnspecified) {
  request.order_quantity = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order quantity missing"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderQuantityZero) {
  request.order_quantity = OrderQuantity{0};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("minimal order quantity constraint violated"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderQuantityLessThanMin) {
  set_min_quantity(MinQuantity{100});
  request.order_quantity = OrderQuantity{99};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("minimal order quantity constraint violated"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenOrderQuantityGreaterThanMax) {
  set_max_quantity(MaxQuantity{100});
  request.order_quantity = OrderQuantity{101};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("maximal order quantity constraint violated"));
}

TEST_F(OrderPlacementRequestValidation,
       FailsWhenOrderQuantityNotMultipleOfTick) {
  set_quantity_tick(QuantityTick{0.02});
  request.order_quantity = OrderQuantity{9.45};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("order quantity multiple constraint violated"));
}

TEST_F(OrderPlacementRequestValidation,
       FailsWhenPriceUnspecifiedForLimitOrder) {
  request.order_type = OrderType::Option::Limit;
  request.order_price = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price missing"));
}

TEST_F(OrderPlacementRequestValidation,
       FailsWhenLimitOrderPriceIsNotMultipleOfTick) {
  set_price_tick(PriceTick{0.002});
  request.order_type = OrderType::Option::Limit;
  request.order_price = OrderPrice{945.105};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price tick constraint violated"));
}

TEST_F(OrderPlacementRequestValidation, FailsWhenPriceSpecifiedForMarketOrder) {
  request.order_type = OrderType::Option::Market;
  request.order_price = OrderPrice{100};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price is not allowed"));
}

TEST_F(OrderPlacementRequestValidation,
       SucceedsWhenAllConstraintsMetForLimitOrder) {
  set_min_quantity(MinQuantity{100});
  set_max_quantity(MaxQuantity{300});
  set_quantity_tick(QuantityTick{0.2});
  set_price_tick(PriceTick{0.05});
  request.side = Side::Option::Sell;
  request.order_type = OrderType::Option::Limit;
  request.order_quantity = OrderQuantity{200};
  request.order_price = OrderPrice{100.25};

  const auto conclusion = validate(request);

  ASSERT_FALSE(conclusion.failed());
}

TEST_F(OrderPlacementRequestValidation,
       FailsWhenGtdOrderSpecifiedWithoutExpireInforimation) {
  request.time_in_force = TimeInForce::Option::GoodTillDate;
  request.expire_date = std::nullopt;
  request.expire_time = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("neither expire date nor expire time specified"));
}

TEST_F(OrderPlacementRequestValidation,
       SucceedsWhenAllConstraintsMetForMarketOrder) {
  set_min_quantity(MinQuantity{100});
  set_max_quantity(MaxQuantity{300});
  set_quantity_tick(QuantityTick{0.2});
  request.side = Side::Option::Sell;
  request.order_type = OrderType::Option::Market;
  request.order_quantity = OrderQuantity{200};

  const auto conclusion = validate(request);

  ASSERT_FALSE(conclusion.failed());
}

/*----------------------------------------------------------------------------*/

struct OrderModificationRequestValidation : public Test,
                                            public ValidationFixture {
  protocol::OrderModificationRequest request =
      make_message<protocol::OrderModificationRequest>();

  OrderModificationRequestValidation() {
    request.side = Side::Option::Buy;
    request.order_type = OrderType::Option::Market;
    request.order_quantity = OrderQuantity{100};
  }
};

TEST_F(OrderModificationRequestValidation, FailsWhenSideUnspecified) {
  request.side = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("side missing"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenSideInvalid) {
  request.side = static_cast<Side::Option>(0xFF);

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("unsupported side value"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenOrderTypeUnspecified) {
  request.order_type = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order type missing"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenOrderTypeInvalid) {
  request.order_type = static_cast<OrderType::Option>(0xFF);

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("unsupported order type value"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenOrderQuantityUnspecified) {
  request.order_quantity = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order quantity missing"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenOrderQuantityZero) {
  request.order_quantity = OrderQuantity{0};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("minimal order quantity constraint violated"));
}

TEST_F(OrderModificationRequestValidation, FailsWhenOrderQuantityLessThanMin) {
  set_min_quantity(MinQuantity{100});
  request.order_quantity = OrderQuantity{99};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("minimal order quantity constraint violated"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenOrderQuantityGreaterThanMax) {
  set_max_quantity(MaxQuantity{100});
  request.order_quantity = OrderQuantity{101};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("maximal order quantity constraint violated"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenOrderQuantityNotMultipleOfTick) {
  set_quantity_tick(QuantityTick{0.02});
  request.order_quantity = OrderQuantity{9.45};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("order quantity multiple constraint violated"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenPriceUnspecifiedForLimitOrder) {
  request.order_type = OrderType::Option::Limit;
  request.order_price = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price missing"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenLimitOrderPriceIsNotMultipleOfTick) {
  set_price_tick(PriceTick{0.002});
  request.order_type = OrderType::Option::Limit;
  request.order_price = OrderPrice{945.105};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price tick constraint violated"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenGtdOrderSpecifiedWithoutExpireInforimation) {
  request.time_in_force = TimeInForce::Option::GoodTillDate;
  request.expire_date = std::nullopt;
  request.expire_time = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion,
              IsError("neither expire date nor expire time specified"));
}

TEST_F(OrderModificationRequestValidation,
       FailsWhenPriceSpecifiedForMarketOrder) {
  request.order_type = OrderType::Option::Market;
  request.order_price = OrderPrice{100};

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("order price is not allowed"));
}

TEST_F(OrderModificationRequestValidation,
       SucceedsWhenAllConstraintsMetForLimitOrder) {
  set_min_quantity(MinQuantity{100});
  set_max_quantity(MaxQuantity{300});
  set_quantity_tick(QuantityTick{0.2});
  set_price_tick(PriceTick{0.05});
  request.side = Side::Option::Sell;
  request.order_type = OrderType::Option::Limit;
  request.order_quantity = OrderQuantity{200};
  request.order_price = OrderPrice{100.25};

  const auto conclusion = validate(request);

  ASSERT_FALSE(conclusion.failed());
}

TEST_F(OrderModificationRequestValidation,
       SucceedsWhenAllConstraintsMetForMarketOrder) {
  set_min_quantity(MinQuantity{100});
  set_max_quantity(MaxQuantity{300});
  set_quantity_tick(QuantityTick{0.2});
  request.side = Side::Option::Sell;
  request.order_type = OrderType::Option::Market;
  request.order_quantity = OrderQuantity{200};

  const auto conclusion = validate(request);

  ASSERT_FALSE(conclusion.failed());
}

/*----------------------------------------------------------------------------*/

struct OrderCancellationRequestValidation : public Test,
                                            public ValidationFixture {
  protocol::OrderCancellationRequest request =
      make_message<protocol::OrderCancellationRequest>();
};

TEST_F(OrderCancellationRequestValidation, FailsWhenSideUnspecified) {
  request.side = std::nullopt;

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("side missing"));
}

TEST_F(OrderCancellationRequestValidation, FailsWhenSideInvalid) {
  request.side = static_cast<Side::Option>(0xFF);

  const auto conclusion = validate(request);

  ASSERT_THAT(conclusion, IsError("unsupported side value"));
}

TEST_F(OrderCancellationRequestValidation, SucceedsWhenAllConstraintsMet) {
  request.side = Side::Option::Sell;

  const auto conclusion = validate(request);

  ASSERT_FALSE(conclusion.failed());
}

/*----------------------------------------------------------------------------*/

struct MarketStateLimitOrderValidation : public Test, public ValidationFixture {
  auto validate(const market_state::LimitOrder& limit_order,
                OrderBookSide order_book_side) -> Conclusion {
    return validator.validate(limit_order, order_book_side);
  }

  MarketStateLimitOrderValidation() {
    order.time_in_force = TimeInForce::Option::GoodTillCancel;
    order.order_id = OrderId{1};
    order.order_time = OrderTime{core::sys_us{std::chrono::microseconds{0}}};
    order.side = Side::Option::Buy;
    order.order_status = OrderStatus::Option::New;
    order.order_price = OrderPrice{0.0};
    order.total_quantity = OrderQuantity{100.0};
    order.cum_executed_quantity = CumExecutedQuantity{50.0};
  }

  market_state::LimitOrder order;
};

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenSideIsNotAllowedForOrderBookPage) {
  order.side = Side::Option::Buy;

  const auto conclusion = validate(order, OrderBookSide::Sell);

  ASSERT_THAT(conclusion, IsError("invalid side value"));
}

TEST_F(MarketStateLimitOrderValidation, FailsWhenTotalQuantityIsLessThanMin) {
  set_min_quantity(MinQuantity{50});
  order.total_quantity = OrderQuantity{49};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion,
              IsError("total quantity minimal constraint violated"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenTotalQuantityIsGreaterThanMax) {
  set_max_quantity(MaxQuantity{100});
  order.total_quantity = OrderQuantity{101};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion,
              IsError("total quantity maximal constraint violated"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenTotalQuantityIsNotMultipleOfTick) {
  set_quantity_tick(QuantityTick{0.02});
  order.total_quantity = OrderQuantity{9.45};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion,
              IsError("total quantity multiple constraint violated"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenCumExecutedQuantityIsLessThanZero) {
  order.cum_executed_quantity = CumExecutedQuantity{-1};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion,
              IsError("cumulative executed quantity is less than zero"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenCumExecutedQuantityIsNotMultipleOfTick) {
  set_quantity_tick(QuantityTick{0.02});
  order.cum_executed_quantity = CumExecutedQuantity{9.45};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(
      conclusion,
      IsError("cumulative executed quantity multiple constraint violated"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenCumExecutedQuantityIsGreaterThanTotalQuantity) {
  order.cum_executed_quantity = CumExecutedQuantity{101};
  order.total_quantity = OrderQuantity{100};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(
      conclusion,
      IsError("cumulative executed quantity is not less than total quantity"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenOrderPriceIsNotMultipleOfTick) {
  set_price_tick(PriceTick{0.002});
  order.order_price = OrderPrice{945.105};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion, IsError("order price tick constraint violated"));
}

TEST_F(MarketStateLimitOrderValidation, FailsWhenOrderStatusIsFilled) {
  order.order_status = OrderStatus::Option::Filled;

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion, IsError("unsupported order status value"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenTimeInForceIsImmediateOrCancel) {
  order.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion, IsError("time in force value is invalid"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenGoodTillDateOrderSpecifiedWithoutExpireInforimation) {
  order.time_in_force = TimeInForce::Option::GoodTillDate;
  order.expire_date = std::nullopt;
  order.expire_time = std::nullopt;

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion,
              IsError("neither expire date nor expire time specified"));
}

TEST_F(MarketStateLimitOrderValidation, FailsWhenOrderIsExpired) {
  using namespace std::chrono_literals;
  order.time_in_force = TimeInForce::Option::GoodTillDate;
  order.expire_date = ExpireDate{core::local_days{2025y / 1 / 1}};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion, IsError("order already expired"));
}

TEST_F(MarketStateLimitOrderValidation,
       FailsWhenOrderTimeIsNotTodayForDayOrder) {
  order.time_in_force = TimeInForce::Option::Day;
  order.order_time =
      OrderTime{core::get_current_system_time() - std::chrono::days(1)};

  const auto conclusion = validate(order, OrderBookSide::Buy);

  ASSERT_THAT(conclusion, IsError("order already expired"));
}

TEST_F(MarketStateLimitOrderValidation, SuccessWhenAllConstraintsMet) {
  set_min_quantity(MinQuantity{50});
  set_max_quantity(MaxQuantity{300});
  set_quantity_tick(QuantityTick{0.2});
  set_price_tick(PriceTick{0.05});

  order.side = Side::Option::Sell;
  order.total_quantity = OrderQuantity{200};
  order.cum_executed_quantity = CumExecutedQuantity{100};
  order.order_price = OrderPrice{100.25};
  order.order_status = OrderStatus::Option::New;
  order.time_in_force = TimeInForce::Option::GoodTillCancel;

  const auto conclusion = validate(order, OrderBookSide::Sell);

  ASSERT_FALSE(conclusion.failed());
}

/*----------------------------------------------------------------------------*/

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::order::test
