#include <gmock/gmock.h>

#include <vector>

#include "ih/orders/replies/execution_reply_builders.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace std::chrono_literals;
using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

struct ExecutionReportBuilder : public Test {
  const protocol::Session test_session{protocol::generator::Session{}};
  OrderBuilder order_builder;
  matching_engine::ExecutionReportBuilder builder{test_session};
};

TEST_F(ExecutionReportBuilder, BuildsReportForSession) {
  const auto report = builder.build();

  ASSERT_THAT(report.session, Eq(test_session));
}

TEST_F(ExecutionReportBuilder, SetsExecutionTypeOrderTradedByDefault) {
  const auto report = builder.build();

  ASSERT_THAT(report.execution_type,
              Optional(Eq(ExecutionType::Option::OrderTraded)));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_limit_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderParties) {
  std::vector parties{Party{PartyId{"QUOD"},
                            PartyIdSource::Option::Proprietary,
                            PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_limit_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.parties, ElementsAreArray(parties));
}

TEST_F(ExecutionReportBuilder, SetsVenueOrderIdFromLimitOrderId) {
  const auto limit_order =
      order_builder.with_order_id(OrderId{123}).build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderClientOrderId) {
  const auto limit_order =
      order_builder.with_client_order_id(ClientOrderId{"CL-123"})
          .build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderPrice) {
  const auto limit_order =
      order_builder.with_order_price(OrderPrice{123}).build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderLeavesQuantity) {
  const auto limit_order = order_builder.build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.leaves_quantity, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderCumExecutedQuantity) {
  const auto limit_order = order_builder.build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.cum_executed_quantity, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderStatus) {
  const auto limit_order = order_builder.build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.order_status, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderSide) {
  const auto limit_order =
      order_builder.with_side(Side::Option::Buy).build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderTypeForLimitOrder) {
  const auto limit_order = order_builder.build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.order_type, Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderTimeInForce) {
  const auto limit_order =
      order_builder.with_time_in_force(TimeInForce::Option::FillOrKill)
          .build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.time_in_force,
              Optional(Eq(TimeInForce::Option::FillOrKill)));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderShortSaleExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_limit_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsLimitOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderParties) {
  std::vector parties{Party{PartyId{"QUOD"},
                            PartyIdSource::Option::Proprietary,
                            PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.parties, ElementsAreArray(parties));
}

TEST_F(ExecutionReportBuilder, SetsVenueOrderIdFromMarketOrderId) {
  const auto order =
      order_builder.with_order_id(OrderId{123}).build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderClientOrderId) {
  const auto order = order_builder.with_client_order_id(ClientOrderId{"CL-123"})
                         .build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(ExecutionReportBuilder, DoesNotSetPriceForMarketOrder) {
  const auto order = order_builder.build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.order_price, Eq(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderLeavesQuantity) {
  const auto order = order_builder.build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.leaves_quantity, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderCumExecutedQuantity) {
  const auto order = order_builder.build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.cum_executed_quantity, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderStatus) {
  const auto limit_order = order_builder.build_limit_order();

  const auto report = builder.for_order(limit_order).build();

  ASSERT_THAT(report.order_status, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderSide) {
  const auto order =
      order_builder.with_side(Side::Option::Buy).build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderTypeForMarketOrder) {
  const auto order = order_builder.build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.order_type, Optional(Eq(OrderType::Option::Market)));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderTimeInForce) {
  const auto order =
      order_builder.with_time_in_force(TimeInForce::Option::ImmediateOrCancel)
          .build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.time_in_force,
              Optional(Eq(TimeInForce::Option::ImmediateOrCancel)));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderShortSaleExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_market_order();

  const auto report = builder.for_order(order).build();

  ASSERT_THAT(report.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsMarketOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_market_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(ExecutionReportBuilder, SetsExecutionId) {
  const auto report = builder.with_execution_id(ExecutionId{"E-123"}).build();

  ASSERT_THAT(report.execution_id, Optional(Eq(ExecutionId{"E-123"})));
}

TEST_F(ExecutionReportBuilder, SetsExecutionPrice) {
  const auto report = builder.with_execution_price(ExecutionPrice{123}).build();

  ASSERT_THAT(report.execution_price, Optional(Eq(ExecutionPrice{123})));
}

TEST_F(ExecutionReportBuilder, SetsExecutedQuantity) {
  const auto report =
      builder.with_executed_quantity(ExecutedQuantity{123}).build();

  ASSERT_THAT(report.executed_quantity, Optional(Eq(ExecutedQuantity{123})));
}

TEST_F(ExecutionReportBuilder, SetsValidCounterparty) {
  const auto counterparty = Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ContraFirm};

  const auto report = builder.with_counterparty(counterparty).build();

  ASSERT_THAT(report.parties, ElementsAre(Eq(counterparty)));
}

TEST_F(ExecutionReportBuilder, DoesNotSetNullCounterparty) {
  const auto report = builder.with_counterparty(std::nullopt).build();

  ASSERT_THAT(report.parties, IsEmpty());
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test