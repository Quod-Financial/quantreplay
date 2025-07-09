#include <gmock/gmock.h>

#include <vector>

#include "ih/orders/replies/cancellation_reply_builders.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/types/session.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace std::chrono_literals;
using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

// region CancellationConfirmationBuilder

struct CancellationConfirmationBuilder : public Test {
  const protocol::Session test_session{protocol::generator::Session{}};
  OrderBuilder order_builder;
  matching_engine::CancellationConfirmationBuilder builder{test_session};
};

TEST_F(CancellationConfirmationBuilder, BuildsConfirmationForSession) {
  const auto confirmation = builder.build();

  ASSERT_THAT(confirmation.session, Eq(test_session));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderParties) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};

  const auto order =
      order_builder.with_order_parties(parties).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.parties, ElementsAreArray(parties));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderId) {
  const auto limit_order =
      order_builder.with_order_id(OrderId{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderLeavesQuantity) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.leaving_quantity, Ne(std::nullopt));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderCumExecutedQuantity) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.cum_executed_quantity, Ne(std::nullopt));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderPrice) {
  const auto limit_order =
      order_builder.with_order_price(OrderPrice{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderStatus) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_status, Ne(std::nullopt));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderSide) {
  const auto limit_order =
      order_builder.with_side(Side::Option::Buy).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(CancellationConfirmationBuilder,
       SetsLimitOrderShortSellExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(CancellationConfirmationBuilder, SetsLimitOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(CancellationConfirmationBuilder, SetsExecutionId) {
  const auto confirmation =
      builder.with_execution_id(ExecutionId{"E-123"}).build();

  ASSERT_THAT(confirmation.execution_id, Optional(Eq(ExecutionId{"E-123"})));
}

TEST_F(CancellationConfirmationBuilder, SetsClientOrderId) {
  const auto confirmation =
      builder.with_client_order_id(ClientOrderId{"CL-123"}).build();

  ASSERT_THAT(confirmation.client_order_id,
              Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(CancellationConfirmationBuilder, SetsOrigClientOrderId) {
  const auto confirmation =
      builder.with_orig_client_order_id(OrigClientOrderId{"ORIG-123"}).build();

  ASSERT_THAT(confirmation.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

// endregion CancellationConfirmationBuilder

// region CancellationRejectBuilder

struct CancellationRejectBuilder : public Test {
  const protocol::Session test_session{protocol::generator::Session{}};
  protocol::OrderCancellationRequest request{test_session};
  matching_engine::CancellationRejectBuilder builder{test_session};
};

TEST_F(CancellationRejectBuilder, BuildsRejectForSession) {
  const auto reject = builder.build();

  ASSERT_THAT(reject.session, Eq(test_session));
}

TEST_F(CancellationRejectBuilder, SetsOrderStatusToRejectedByDefault) {
  const auto reject = builder.build();

  ASSERT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Rejected)));
}

TEST_F(CancellationRejectBuilder, SetsVenueOrderIdFromRequest) {
  request.venue_order_id = VenueOrderId{"VENUE-123"};
  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"VENUE-123"})));
}

TEST_F(CancellationRejectBuilder, SetsClientOrderIdFromRequest) {
  request.client_order_id = ClientOrderId{"CL-123"};
  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(CancellationRejectBuilder, SetsOrigClientOrderIdFromRequest) {
  request.orig_client_order_id = OrigClientOrderId{"ORIG-123"};
  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

TEST_F(CancellationRejectBuilder, SetsRejectReason) {
  const auto reject = builder.with_reason(RejectText{"Some reason"}).build();

  ASSERT_THAT(reject.reject_text, Optional(Eq(RejectText{"Some reason"})));
}

TEST_F(CancellationRejectBuilder, SetsOrderStatus) {
  const auto reject =
      builder.with_order_status(OrderStatus::Option::Filled).build();

  ASSERT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Filled)));
}

TEST_F(CancellationRejectBuilder, SetsVenueOrderIdFromOrderId) {
  const auto reject = builder.with_order_id(OrderId{123}).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(CancellationRejectBuilder, SetsClientOrderId) {
  const auto reject =
      builder.with_client_order_id(ClientOrderId{"CL-123"}).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(CancellationRejectBuilder, SetsOrigClientOrderId) {
  const auto reject =
      builder.with_orig_client_order_id(OrigClientOrderId{"ORIG-123"}).build();

  ASSERT_THAT(reject.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

// endregion CancellationRejectBuilder

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test