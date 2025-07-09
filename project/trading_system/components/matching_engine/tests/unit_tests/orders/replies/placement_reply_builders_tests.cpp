#include <gmock/gmock.h>

#include <vector>

#include "ih/orders/replies/placement_reply_builders.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace std::chrono_literals;
using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

// region PlacementConfirmationBuilder

class PlacementConfirmationBuilder : public ::testing::Test {
 public:
  const protocol::Session test_session{protocol::generator::Session{}};
  OrderBuilder order_builder;
  matching_engine::PlacementConfirmationBuilder builder{test_session};
};

TEST_F(PlacementConfirmationBuilder, BuildsConfirmationForSession) {
  const auto confirmation = builder.build();

  ASSERT_THAT(confirmation.session, Eq(test_session));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderParties) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.parties, ElementsAreArray(parties));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderId) {
  const auto limit_order =
      order_builder.with_order_id(OrderId{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderClientOrderId) {
  const auto limit_order =
      order_builder.with_client_order_id(ClientOrderId{"CL-123"})
          .build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.client_order_id,
              Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderPrice) {
  const auto limit_order =
      order_builder.with_order_price(OrderPrice{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderQuantity) {
  const auto limit_order =
      order_builder.with_order_quantity(OrderQuantity{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_quantity, Optional(Eq(OrderQuantity{123})));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderSide) {
  const auto limit_order =
      order_builder.with_side(Side::Option::Buy).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderType) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_type, Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderTimeInForce) {
  const auto limit_order =
      order_builder.with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.time_in_force,
              Optional(Eq(TimeInForce::Option::Day)));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderShortSellExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(PlacementConfirmationBuilder, SetsLimitOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto market_order =
      order_builder.with_instrument(instrument).build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderParties) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};
  const auto market_order =
      order_builder.with_order_parties(parties).build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.parties, ElementsAreArray(parties));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderId) {
  const auto market_order =
      order_builder.with_order_id(OrderId{123}).build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderClientOrderId) {
  const auto market_order =
      order_builder.with_client_order_id(ClientOrderId{"CL-123"})
          .build_limit_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.client_order_id,
              Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderType) {
  const auto market_order = order_builder.build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.order_type, Optional(Eq(OrderType::Option::Market)));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderTimeInForce) {
  const auto market_order =
      order_builder.with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
          .build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.time_in_force,
              Optional(Eq(TimeInForce::Option::ImmediateOrCancel)));
}

TEST_F(PlacementConfirmationBuilder, DoesNotSetPriceForMarketOrder) {
  const auto market_order = order_builder.build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.order_price, Eq(std::nullopt));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderQuantity) {
  const auto market_order =
      order_builder.with_order_quantity(OrderQuantity{123})
          .build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.order_quantity, Optional(Eq(OrderQuantity{123})));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderSide) {
  const auto market_order =
      order_builder.with_side(Side::Option::Buy).build_market_order();

  const auto confirmation = builder.for_order(market_order).build();

  ASSERT_THAT(confirmation.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(PlacementConfirmationBuilder, SetsExecutionId) {
  const auto confirmation =
      builder.with_execution_id(ExecutionId{"E-123"}).build();

  ASSERT_THAT(confirmation.execution_id, Optional(Eq(ExecutionId{"E-123"})));
}

TEST_F(PlacementConfirmationBuilder, SetsMarketOrderShortSellExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

// endregion PlacementConfirmationBuilder

// region PlacementRejectBuilder

struct PlacementRejectBuilder : public ::testing::Test {
  protocol::Session test_session{protocol::generator::Session{}};
  protocol::OrderPlacementRequest request{test_session};
  OrderBuilder order_builder;

  matching_engine::PlacementRejectBuilder builder{test_session};
};

TEST_F(PlacementRejectBuilder, BuildsRejectForSession) {
  const auto reject = builder.build();

  ASSERT_THAT(reject.session, Eq(test_session));
}

TEST_F(PlacementRejectBuilder, SetsInstrumentFromRequest) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  request.instrument = instrument;

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.instrument.symbol, Eq(instrument.symbol));
}

TEST_F(PlacementRejectBuilder, SetsOrderPartiesFromRequest) {
  request.parties = {Party{PartyId{"QUOD"},
                           PartyIdSource::Option::Proprietary,
                           PartyRole::Option::ExecutingFirm}};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.parties, ElementsAreArray(request.parties));
}

TEST_F(PlacementRejectBuilder, SetsOrderPriceFromRequest) {
  request.order_price = OrderPrice{123};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(PlacementRejectBuilder, SetsOrderQuantityFromRequest) {
  request.order_quantity = OrderQuantity{123};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.order_quantity, Optional(Eq(OrderQuantity{123})));
}

TEST_F(PlacementRejectBuilder, SetsOrderSideFromRequest) {
  request.side = Side::Option::Buy;

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(PlacementRejectBuilder, SetsClientOrderIdFromRequest) {
  request.client_order_id = ClientOrderId{"CL-123"};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(PlacementRejectBuilder, SetsShortSellExemptionReasonFromRequest) {
  request.short_sale_exempt_reason = ShortSaleExemptionReason(0);

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(PlacementRejectBuilder, SetsExpireTimeFromRequest) {
  request.expire_time = ExpireTime(std::chrono::system_clock::now());

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.expire_time, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsExpireDateFromRequest) {
  request.expire_date = ExpireDate(2020y / 12 / 31);

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.expire_date, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsInstrumentFromLimitOrder) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(PlacementRejectBuilder, SetsOrderPartiesFromLimitOrder) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.parties, ElementsAreArray(parties));
}

TEST_F(PlacementRejectBuilder, SetsOrderPriceFromLimitOrder) {
  const auto order =
      order_builder.with_order_price(OrderPrice{123}).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(PlacementRejectBuilder, SetsOrderQuantityFromLimitOrder) {
  const auto order =
      order_builder.with_order_quantity(OrderQuantity{123}).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.order_quantity, Optional(Eq(OrderQuantity{123})));
}

TEST_F(PlacementRejectBuilder, SetsOrderSideFromLimitOrder) {
  const auto order =
      order_builder.with_side(Side::Option::Buy).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(PlacementRejectBuilder, SetsClientOrderIdFromLimitOrder) {
  const auto order = order_builder.with_client_order_id(ClientOrderId{"CL-123"})
                         .build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(PlacementRejectBuilder, SetsVenueOrderIdFromLimitOrderId) {
  const auto order =
      order_builder.with_order_id(OrderId{4221}).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"4221"})));
}

TEST_F(PlacementRejectBuilder, SetsOrderTypeForLimitOrder) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_type, Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(PlacementRejectBuilder, SetsTimeInForceForLimitOrder) {
  const auto limit_order =
      order_builder.with_time_in_force(TimeInForce::Option::Day)
          .build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.time_in_force,
              Optional(Eq(TimeInForce::Option::Day)));
}

TEST_F(PlacementRejectBuilder, SetsShortSellExemptionReasonForLimitOrder) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(PlacementRejectBuilder, SetsExpireTimeForLimitOrder) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.expire_time, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsExpireDateForLimitOrder) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.expire_date, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsInstrumentFromMarketOrder) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(PlacementRejectBuilder, SetsOrderPartiesFromMarketOrder) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.parties, ElementsAreArray(parties));
}

TEST_F(PlacementRejectBuilder, DoesNotSetPriceForMarketOrder) {
  const auto order =
      order_builder.with_order_price(OrderPrice{123}).build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.order_price, Eq(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsOrderQuantityFromMarketOrder) {
  const auto order =
      order_builder.with_order_quantity(OrderQuantity{123}).build_limit_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.order_quantity, Optional(Eq(OrderQuantity{123})));
}

TEST_F(PlacementRejectBuilder, SetsOrderSideFromMarketOrder) {
  const auto order =
      order_builder.with_side(Side::Option::Buy).build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(PlacementRejectBuilder, SetsClientOrderIdFromMarketOrder) {
  const auto order = order_builder.with_client_order_id(ClientOrderId{"CL-123"})
                         .build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(PlacementRejectBuilder, SetsVenueOrderIdFromMarketOrderId) {
  const auto order =
      order_builder.with_order_id(OrderId{4221}).build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"4221"})));
}

TEST_F(PlacementRejectBuilder, SetsOrderTypeForMarketOrder) {
  const auto order = order_builder.build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.order_type, Optional(Eq(OrderType::Option::Market)));
}

TEST_F(PlacementRejectBuilder, SetsTimeInForceForMarketOrder) {
  const auto order =
      order_builder.with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
          .build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.time_in_force,
              Optional(Eq(TimeInForce::Option::ImmediateOrCancel)));
}

TEST_F(PlacementRejectBuilder, SetsShortSellExemptionReasonForMarketOrder) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(PlacementRejectBuilder, SetsExpireTimeForMarketOrder) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_market_order();

  const auto reject = builder.for_order(order).build();

  ASSERT_THAT(reject.expire_time, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsExpireDateForMarketOrder) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(PlacementRejectBuilder, SetsRejectReason) {
  const auto reject = builder.with_reason(RejectText{"Some reason"}).build();

  ASSERT_THAT(reject.reject_text, Optional(Eq(RejectText{"Some reason"})));
}

TEST_F(PlacementRejectBuilder, SetsVenueOrderIdFromOrderId) {
  const auto reject = builder.with_order_id(OrderId{123}).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(PlacementRejectBuilder, SetsExecutionId) {
  const auto reject = builder.with_execution_id(ExecutionId{"4221-1"}).build();

  ASSERT_THAT(reject.execution_id, Optional(Eq(ExecutionId{"4221-1"})));
}

// endregion PlacementRejectBuilder

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test