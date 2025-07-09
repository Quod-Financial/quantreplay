#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <optional>

#include "common/attributes.hpp"
#include "core/domain/party.hpp"
#include "ih/orders/book/limit_order.hpp"
#include "ih/orders/book/order_updates.hpp"
#include "ih/orders/requests/interpretation.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::matching_engine {

using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

namespace detail::test {

// region SideInterpretation

TEST(SideInterpretation, ReportsMissingSide) {
  const auto result = interpret_side(std::nullopt);

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::SideMissing));
}

TEST(SideInterpretation, ReportsInvalidSide) {
  const auto result = interpret_side(static_cast<Side::Option>(0xFF));

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::SideInvalid));
}

TEST(SideInterpretation, InterpretsValidSideValue) {
  const auto result = interpret_side(Side::Option::Buy);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(Side::Option::Buy));
}

// endregion SideInterpretation

// region OrderTypeInterpretation

TEST(OrderTypeInterpretation, ReportsMissingOrderType) {
  const auto result = interpret_order_type(std::nullopt);

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::OrderTypeMissing));
}

TEST(OrderTypeInterpretation, ReportsInvalidOrderType) {
  const auto result =
      interpret_order_type(static_cast<OrderType::Option>(0xFF));

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::OrderTypeInvalid));
}

TEST(OrderTypeInterpretation, InterpretsMarketOrderType) {
  const auto result = interpret_order_type(OrderType::Option::Market);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(OrderType::Option::Market));
}

TEST(OrderTypeInterpretation, InterpretsLimitOrderType) {
  const auto result = interpret_order_type(OrderType::Option::Limit);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(OrderType::Option::Limit));
}

// endregion OrderTypeInterpretation

// region TimeInForceInterpretation

TEST(TimeInForceInterpretation, ReportsInvalidTimeInForce) {
  const auto result =
      interpret_time_in_force(static_cast<TimeInForce::Option>(0xFF));

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::TimeInForceInvalid));
}

TEST(TimeInForceInterpretation, InterpretsDayTimeInForce) {
  const auto result = interpret_time_in_force(TimeInForce::Option::Day);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::Day));
}

TEST(TimeInForceInterpretation, InterpretsImmediateOrCancelTimeInForce) {
  const auto result =
      interpret_time_in_force(TimeInForce::Option::ImmediateOrCancel);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::ImmediateOrCancel));
}

TEST(TimeInForceInterpretation, InterpretsFillOrKillTimeInForce) {
  const auto result = interpret_time_in_force(TimeInForce::Option::FillOrKill);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::FillOrKill));
}

TEST(TimeInForceInterpretation, InterpretsGoodTillDateTimeInForce) {
  const auto result =
      interpret_time_in_force(TimeInForce::Option::GoodTillDate);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::GoodTillDate));
}

TEST(TimeInForceInterpretation, InterpretsGoodTillCancelTimeInForce) {
  const auto result =
      interpret_time_in_force(TimeInForce::Option::GoodTillCancel);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::GoodTillCancel));
}

TEST(TimeInForceInterpretation, InterpretsMissingTimeInForceAsDay) {
  const auto result = interpret_time_in_force(std::nullopt);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(TimeInForce::Option::Day));
}

// endregion TimeInForceInterpretation

// region OrderIdInterpretation

TEST(OrderIdIntepretation, InterpretsValidVenueOrderId) {
  const VenueOrderId venue_order_id{"4221"};

  const auto result = interpret_order_id(venue_order_id);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Optional(Eq(OrderId{4221})));
}

TEST(OrderIdIntepretation, ReportsInvalidVenueOrderId) {
  const VenueOrderId venue_order_id{"4221-I"};

  const auto result = interpret_order_id(venue_order_id);

  ASSERT_THAT(result.has_value(), IsFalse());
  ASSERT_THAT(result.error(), Eq(OrderRequestError::OrderIdInvalid));
}

TEST(OrderIdIntepretation, InterpretsMissingVenueOrderId) {
  const auto result = interpret_order_id(std::nullopt);

  ASSERT_THAT(result.has_value(), IsTrue());
  ASSERT_THAT(result.value(), Eq(std::nullopt));
}

// endregion OrderIdInterpretation

// region OrderAttributesCreation

template <typename RequestType>
struct OrderAttributesCreation : public Test {
  protocol::Session session{protocol::generator::Session{}};
  RequestType request{session};
};

using OrderAttributesRequests =
    Types<protocol::OrderPlacementRequest, protocol::OrderModificationRequest>;
TYPED_TEST_SUITE(OrderAttributesCreation, OrderAttributesRequests);

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithParties) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm},
                            Party{PartyId{"XTX"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ContraFirm}};
  this->request.parties = parties;

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->order_parties(), ElementsAreArray(parties));
}

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithoutClientOrderId) {
  this->request.client_order_id = std::nullopt;

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->client_order_id(), Eq(std::nullopt));
}

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithClientOrderId) {
  const ClientOrderId client_order_id{"CL-1"};
  this->request.client_order_id = client_order_id;

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->client_order_id(), Optional(Eq(client_order_id)));
}

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithTimeInForceValue) {
  this->request.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->time_in_force(),
              Eq(TimeInForce::Option::ImmediateOrCancel));
}

TYPED_TEST(OrderAttributesCreation,
           CreatesAttributesWithShortSaleExemptionReason) {
  this->request.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->short_sale_exemption_reason(),
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithExpireTime) {
  using namespace std::chrono;  // NOLINT
  const auto now = system_clock::now();
  this->request.expire_time = ExpireTime(time_point_cast<microseconds>(now));

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->expire_time(),
              Optional(Eq(this->request.expire_time)));
}

TYPED_TEST(OrderAttributesCreation, CreatesAttributesWithExpireDate) {
  using namespace std::chrono;  // NOLINT
  const auto now = system_clock::now();
  this->request.expire_date =
      ExpireDate(local_days(time_point_cast<days>(now).time_since_epoch()));

  const auto attributes = OrderAttributesCreator::create_from(this->request);

  ASSERT_THAT(attributes.has_value(), IsTrue());
  ASSERT_THAT(attributes->expire_date(),
              Optional(Eq(this->request.expire_date)));
}

// endregion OrderAttributesCreation

}  // namespace detail::test

namespace test {

// region OrderErrorFormatting

TEST(OrderErrorFormatting, ThrowsErrorForUnsupportedErrorEnumValue) {
  ASSERT_THROW(
      (void)convert_to_reason_text(static_cast<OrderRequestError>(0xFF)),
      std::invalid_argument);
}

TEST(OrderErrorFormatting, FormatsOrderTypeInvalid) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::OrderTypeInvalid),
              Eq(RejectText{"unknown order type"}));
}

TEST(OrderErrorFormatting, FormatsOrderTypeMissing) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::OrderTypeMissing),
              Eq(RejectText{"order type missing"}));
}

TEST(OrderErrorFormatting, FormatsSideInvalid) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::SideInvalid),
              Eq(RejectText{"unknown order side"}));
}

TEST(OrderErrorFormatting, FormatsSideMissing) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::SideMissing),
              Eq(RejectText{"order side missing"}));
}

TEST(OrderErrorFormatting, FormatsTimeInForceInvalid) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::TimeInForceInvalid),
              Eq(RejectText{"unknown time in force"}));
}

TEST(OrderErrorFormatting, FormatsPriceMissing) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::PriceMissing),
              Eq(RejectText{"order price missing"}));
}

TEST(OrderErrorFormatting, FormatsQuantityMissing) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::QuantityMissing),
              Eq(RejectText{"order quantity missing"}));
}

TEST(OrderErrorFormatting, FormatsOrderIdInvalid) {
  ASSERT_THAT(convert_to_reason_text(OrderRequestError::OrderIdInvalid),
              Eq(RejectText{"invalid order identifier format"}));
}

// endregion OrderErrorFormatting

// region PlacementInterpretation

struct PlacementInterpretation : public Test {
  const protocol::Session session{protocol::generator::Session{}};
  protocol::OrderPlacementRequest raw_request{session};
  protocol::OrderPlacementRequest limit_request{session};
  protocol::OrderPlacementRequest market_request{session};

  const OrderId order_id{4221};
  PlacementInterpreter interpreter{order_id};

 private:
  auto SetUp() -> void override {
    // Minimal set of required fields for a limit order
    limit_request.order_type = OrderType::Option::Limit;
    limit_request.side = Side::Option::Buy;
    limit_request.order_price = OrderPrice{100.0};
    limit_request.order_quantity = OrderQuantity{100};

    // Minimal set of required fields for a market order
    market_request.order_type = OrderType::Option::Market;
    market_request.side = Side::Option::Buy;
    market_request.order_quantity = OrderQuantity{100};
  }
};

TEST_F(PlacementInterpretation, ReportsOrderTypeIntepreationError) {
  raw_request.order_type = std::nullopt;

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::OrderTypeMissing)));
}

TEST_F(PlacementInterpretation, ReportsSideInterpretationError) {
  raw_request.order_type = OrderType::Option::Market;
  raw_request.side = std::nullopt;

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::SideMissing)));
}

TEST_F(PlacementInterpretation, ReportsTimeInForceInterpretationError) {
  limit_request.time_in_force = static_cast<TimeInForce::Option>(0xFF);

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(error,
              VariantWith<OrderRequestError>(
                  Eq(OrderRequestError::TimeInForceInvalid)));
}

TEST_F(PlacementInterpretation, ReportsPriceMissingForLimitOrder) {
  limit_request.order_price = std::nullopt;

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::PriceMissing)));
}

TEST_F(PlacementInterpretation, ReportsQuantityMissingForLimitOrder) {
  limit_request.order_quantity = std::nullopt;

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::QuantityMissing)));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithSpecifiedId) {
  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order,
              VariantWith<LimitOrder>(Property(&LimitOrder::id, Eq(order_id))));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenPrice) {
  limit_request.order_price = OrderPrice{42.0};

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order,
              VariantWith<LimitOrder>(
                  Property(&LimitOrder::price, Eq(OrderPrice{42.0}))));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenQuantity) {
  limit_request.order_quantity = OrderQuantity{42};

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order,
              VariantWith<LimitOrder>(Property(&LimitOrder::total_quantity,
                                               Eq(OrderQuantity{42}))));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenSide) {
  limit_request.side = Side::Option::Sell;

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order,
              VariantWith<LimitOrder>(
                  Property(&LimitOrder::side, Eq(Side::Option::Sell))));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenSession) {
  limit_request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"SENDER"},
                             protocol::fix::TargetCompId{"TARGET"}}};

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order,
              VariantWith<LimitOrder>(Property(&LimitOrder::client_session,
                                               Eq(limit_request.session))));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenInstrument) {
  const Symbol symbol{"AAPL"};
  limit_request.instrument.symbol = symbol;

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order, VariantWith<LimitOrder>(_));
  ASSERT_THAT(std::get<LimitOrder>(order).instrument().symbol, Eq(symbol));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenParties) {
  const Party party{PartyId{"QUOD"},
                    PartyIdSource::Option::Proprietary,
                    PartyRole::Option::ExecutingFirm};
  limit_request.parties = {party};

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order, VariantWith<LimitOrder>(_));
  ASSERT_THAT(std::get<LimitOrder>(order).attributes().order_parties(),
              ElementsAre(Eq(party)));
}

TEST_F(PlacementInterpretation, CreatesLimitOrderWithGivenClientOrderId) {
  const ClientOrderId client_order_id{"CL-1"};
  limit_request.client_order_id = client_order_id;

  const auto order = interpreter.interpret(limit_request);

  ASSERT_THAT(order, VariantWith<LimitOrder>(_));
  ASSERT_THAT(std::get<LimitOrder>(order).attributes().client_order_id(),
              Eq(client_order_id));
}

TEST_F(PlacementInterpretation, ReportsQuantityMissingForMarketOrder) {
  market_request.order_quantity = std::nullopt;

  const auto error = interpreter.interpret(market_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::QuantityMissing)));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithSpecifiedId) {
  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(
      order,
      VariantWith<MarketOrder>(Property(&MarketOrder::id, Eq(order_id))));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenQuantity) {
  market_request.order_quantity = OrderQuantity{42};

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order,
              VariantWith<MarketOrder>(Property(&MarketOrder::total_quantity,
                                                Eq(OrderQuantity{42}))));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenSide) {
  market_request.side = Side::Option::Sell;

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order,
              VariantWith<MarketOrder>(
                  Property(&MarketOrder::side, Eq(Side::Option::Sell))));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenSession) {
  market_request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"SENDER"},
                             protocol::fix::TargetCompId{"TARGET"}}};

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order,
              VariantWith<MarketOrder>(Property(&MarketOrder::client_session,
                                                Eq(market_request.session))));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenInstrument) {
  const Symbol symbol{"AAPL"};
  market_request.instrument.symbol = symbol;

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order, VariantWith<MarketOrder>(_));
  ASSERT_THAT(std::get<MarketOrder>(order).instrument().symbol, Eq(symbol));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenParties) {
  const Party party{PartyId{"QUOD"},
                    PartyIdSource::Option::Proprietary,
                    PartyRole::Option::ExecutingFirm};
  market_request.parties = {party};

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order, VariantWith<MarketOrder>(_));
  ASSERT_THAT(std::get<MarketOrder>(order).attributes().order_parties(),
              ElementsAre(Eq(party)));
}

TEST_F(PlacementInterpretation, CreatesMarketOrderWithGivenClientOrderId) {
  const ClientOrderId client_order_id{"CL-1"};
  market_request.client_order_id = client_order_id;

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order, VariantWith<MarketOrder>(_));
  ASSERT_THAT(std::get<MarketOrder>(order).attributes().client_order_id(),
              Eq(client_order_id));
}

TEST_F(PlacementInterpretation, ImplicitlySetsIocTimeInForceForMarketOrder) {
  market_request.time_in_force = TimeInForce::Option::Day;

  const auto order = interpreter.interpret(market_request);

  ASSERT_THAT(order, VariantWith<MarketOrder>(_));
  ASSERT_THAT(std::get<MarketOrder>(order).time_in_force(),
              Eq(TimeInForce::Option::ImmediateOrCancel));
}

// endregion PlacementInterpretation

// region ModificationInterpretation

struct ModificationInterpretation : public Test {
  const protocol::Session session{protocol::generator::Session{}};
  protocol::OrderModificationRequest raw_request{session};
  protocol::OrderModificationRequest limit_request{session};

  ModificationInterpreter interpreter;

 private:
  auto SetUp() -> void override {
    // Minimal set of required fields for a limit order update
    limit_request.order_type = OrderType::Option::Limit;
    limit_request.side = Side::Option::Buy;
    limit_request.order_price = OrderPrice{100.0};
    limit_request.order_quantity = OrderQuantity{100};
    limit_request.venue_order_id = VenueOrderId{"4221"};
  }
};

TEST_F(ModificationInterpretation, ReportsOrderTypeIntepreationError) {
  raw_request.order_type = std::nullopt;

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::OrderTypeMissing)));
}

TEST_F(ModificationInterpretation, ReportsSideInterpretationError) {
  raw_request.order_type = OrderType::Option::Limit;
  raw_request.side = std::nullopt;

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::SideMissing)));
}

TEST_F(ModificationInterpretation, ReportsTimeInForceInterpretationError) {
  limit_request.time_in_force = static_cast<TimeInForce::Option>(0xFF);

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(error,
              VariantWith<OrderRequestError>(
                  Eq(OrderRequestError::TimeInForceInvalid)));
}

TEST_F(ModificationInterpretation, ReportsOrderIdInterpretationError) {
  raw_request.order_type = OrderType::Option::Limit;
  raw_request.side = Side::Option::Sell;
  raw_request.venue_order_id = VenueOrderId{"4221-INVALID"};

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::OrderIdInvalid)));
}

TEST_F(ModificationInterpretation, ReportsInvalidOrderTypeForMarketUpdate) {
  raw_request.order_type = OrderType::Option::Market;
  raw_request.side = Side::Option::Buy;
  raw_request.order_quantity = OrderQuantity{100};

  const auto error = interpreter.interpret(raw_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::OrderTypeInvalid)));
}

TEST_F(ModificationInterpretation, ReportsPriceMissingForLimitUpdate) {
  limit_request.order_price = std::nullopt;

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::PriceMissing)));
}

TEST_F(ModificationInterpretation, ReportsQuantityMissingForLimitUpdate) {
  limit_request.order_quantity = std::nullopt;

  const auto error = interpreter.interpret(limit_request);

  ASSERT_THAT(
      error,
      VariantWith<OrderRequestError>(Eq(OrderRequestError::QuantityMissing)));
}

TEST_F(ModificationInterpretation, InterpretsLimitUpdate) {
  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
}

TEST_F(ModificationInterpretation, SetsSessionForLimitUpdate) {
  limit_request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"SENDER"},
                             protocol::fix::TargetCompId{"TARGET"}}};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).client_session,
              Eq(limit_request.session));
}

TEST_F(ModificationInterpretation, SetsOrderSideForLimitUpdate) {
  limit_request.side = Side::Option::Sell;

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).order_side, Eq(Side::Option::Sell));
}

TEST_F(ModificationInterpretation, SetsOrderPriceForLimitUpdate) {
  limit_request.order_price = OrderPrice{200.0};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).order_diff.price,
              Eq(OrderPrice{200.0}));
}

TEST_F(ModificationInterpretation, SetsOrderQuantityForLimitUpdate) {
  limit_request.order_quantity = OrderQuantity{200};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).order_diff.quantity,
              Eq(OrderQuantity{200}));
}

TEST_F(ModificationInterpretation, SetsPartiesForLimitUpdate) {
  const Party party{PartyId{"QUOD"},
                    PartyIdSource::Option::Proprietary,
                    PartyRole::Option::ExecutingFirm};
  limit_request.parties = {party};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(
      std::get<LimitUpdate>(update).order_diff.attributes.order_parties(),
      ElementsAre(Eq(party)));
}

TEST_F(ModificationInterpretation, SetsOrderIdForLimitUpdate) {
  limit_request.venue_order_id = VenueOrderId{"4221"};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).order_id, Eq(OrderId{4221}));
}

TEST_F(ModificationInterpretation, SetsOrigClientOrderIdForLimitUpdate) {
  limit_request.orig_client_order_id = OrigClientOrderId{"ORIG-1"};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(std::get<LimitUpdate>(update).orig_client_order_id,
              Eq(OrigClientOrderId{"ORIG-1"}));
}

TEST_F(ModificationInterpretation, SetsClientOrderIdForLimitUpdate) {
  limit_request.client_order_id = ClientOrderId{"CLIENT-ORDER-ID"};

  const auto update = interpreter.interpret(limit_request);

  ASSERT_THAT(update, VariantWith<LimitUpdate>(_));
  ASSERT_THAT(
      std::get<LimitUpdate>(update).order_diff.attributes.client_order_id(),
      Eq(ClientOrderId{"CLIENT-ORDER-ID"}));
}

// endregion ModificationInterpretation

// region CancellationInterpretation

struct CancellationInterpretation : public Test {
  const protocol::Session session{protocol::generator::Session{}};
  protocol::OrderCancellationRequest request{session};

  CancellationInterpreter interpreter;

 private:
  auto SetUp() -> void override {
    // Minimal set of required fields for a cancellation request
    request.side = Side::Option::Buy;
  }
};

TEST_F(CancellationInterpretation, ReportsSideInterpretationError) {
  request.side = std::nullopt;

  const auto error = interpreter.interpret(request);

  ASSERT_THAT(error,
              VariantWith<OrderRequestError>(OrderRequestError::SideMissing));
}

TEST_F(CancellationInterpretation, ReportsOrderIdInterpretationError) {
  request.venue_order_id = VenueOrderId{"4221-INVALID"};

  const auto error = interpreter.interpret(request);

  ASSERT_THAT(error, VariantWith<OrderRequestError>(_));
}

TEST_F(CancellationInterpretation, SetsRequestSession) {
  const auto cancellation = interpreter.interpret(request);

  ASSERT_THAT(cancellation, VariantWith<OrderCancel>(_));
  ASSERT_THAT(std::get<OrderCancel>(cancellation).client_session,
              Eq(request.session));
}

TEST_F(CancellationInterpretation, SetsRequestSide) {
  request.side = Side::Option::Sell;

  const auto cancellation = interpreter.interpret(request);

  ASSERT_THAT(cancellation, VariantWith<OrderCancel>(_));
  ASSERT_THAT(std::get<OrderCancel>(cancellation).order_side,
              Eq(Side::Option::Sell));
}

TEST_F(CancellationInterpretation, SetsRequestVenueOrderId) {
  request.venue_order_id = VenueOrderId{"4221"};

  const auto cancellation = interpreter.interpret(request);

  ASSERT_THAT(cancellation, VariantWith<OrderCancel>(_));
  ASSERT_THAT(std::get<OrderCancel>(cancellation).order_id, Eq(OrderId{4221}));
}

TEST_F(CancellationInterpretation, SetsRequestClientOrderId) {
  request.client_order_id = ClientOrderId{"CLIENT-ORDER-ID"};

  const auto cancellation = interpreter.interpret(request);

  ASSERT_THAT(cancellation, VariantWith<OrderCancel>(_));
  ASSERT_THAT(std::get<OrderCancel>(cancellation).client_order_id,
              Optional(Eq(ClientOrderId{"CLIENT-ORDER-ID"})));
}

TEST_F(CancellationInterpretation, SetsRequestOrigClientOrderId) {
  request.orig_client_order_id = OrigClientOrderId{"ORIG-CLIENT-ORDER-ID"};

  const auto cancellation = interpreter.interpret(request);

  ASSERT_THAT(cancellation, VariantWith<OrderCancel>(_));
  ASSERT_THAT(std::get<OrderCancel>(cancellation).orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-CLIENT-ORDER-ID"})));
}

// endregion CancellationInterpretation

}  // namespace test

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace simulator::trading_system::matching_engine