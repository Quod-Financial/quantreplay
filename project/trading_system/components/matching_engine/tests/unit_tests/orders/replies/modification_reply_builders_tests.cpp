#include <gmock/gmock.h>

#include <vector>

#include "ih/orders/replies/modification_reply_builders.hpp"
#include "tools/order_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

using namespace std::chrono_literals;
using namespace ::testing;  // NOLINT

// NOLINTBEGIN(*magic-numbers*)

// region ModificationConfirmationBuilder

struct ModificationConfirmationBuilder : public Test {
  const protocol::Session test_session{protocol::generator::Session{}};
  OrderBuilder order_builder;
  matching_engine::ModificationConfirmationBuilder builder{test_session};
};

TEST_F(ModificationConfirmationBuilder, BuildsConfirmationForSession) {
  const auto confirmation = builder.build();

  ASSERT_THAT(confirmation.session, Eq(test_session));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderInstrumentDescriptor) {
  InstrumentDescriptor instrument;
  instrument.symbol = Symbol{"AAPL"};
  const auto order =
      order_builder.with_instrument(instrument).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.instrument.symbol, Optional(Eq(instrument.symbol)));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderParties) {
  const std::vector parties{Party{PartyId{"QUOD"},
                                  PartyIdSource::Option::Proprietary,
                                  PartyRole::Option::ExecutingFirm}};
  const auto order =
      order_builder.with_order_parties(parties).build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.parties, ElementsAreArray(parties));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderId) {
  const auto limit_order =
      order_builder.with_order_id(OrderId{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderClientOrderId) {
  const auto limit_order =
      order_builder.with_client_order_id(ClientOrderId{"CL-123"})
          .build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.client_order_id,
              Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderPrice) {
  const auto limit_order =
      order_builder.with_order_price(OrderPrice{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_price, Optional(Eq(OrderPrice{123})));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderLeavesQuantity) {
  const auto limit_order =
      order_builder.with_order_quantity(OrderQuantity{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.leaving_quantity, Ne(std::nullopt));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderCumExecutedQuantity) {
  const auto limit_order =
      order_builder.with_order_quantity(OrderQuantity{123}).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.cum_executed_quantity, Ne(std::nullopt));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderStatus) {
  const auto limit_order = order_builder.build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.order_status, Ne(std::nullopt));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderSide) {
  const auto limit_order =
      order_builder.with_side(Side::Option::Buy).build_limit_order();

  const auto confirmation = builder.for_order(limit_order).build();

  ASSERT_THAT(confirmation.side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(ModificationConfirmationBuilder,
       SetsLimitOrderShortSellExemptionReason) {
  const auto order =
      order_builder
          .with_short_sell_exemption_reason(ShortSaleExemptionReason(0))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.short_sale_exempt_reason,
              Optional(Eq(ShortSaleExemptionReason(0))));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderExpireTime) {
  const auto order =
      order_builder
          .with_expire_time(ExpireTime(std::chrono::system_clock::now()))
          .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_time, Ne(std::nullopt));
}

TEST_F(ModificationConfirmationBuilder, SetsLimitOrderExpireDate) {
  const auto order = order_builder.with_expire_date(ExpireDate(2020y / 12 / 31))
                         .build_limit_order();

  const auto confirmation = builder.for_order(order).build();

  ASSERT_THAT(confirmation.expire_date, Ne(std::nullopt));
}

TEST_F(ModificationConfirmationBuilder, SetsExecutionId) {
  const auto confirmation =
      builder.with_execution_id(ExecutionId{"E-123"}).build();

  ASSERT_THAT(confirmation.execution_id, Optional(Eq(ExecutionId{"E-123"})));
}

TEST_F(ModificationConfirmationBuilder, SetsOrigClientOrderId) {
  const auto confirmation =
      builder.with_orig_client_order_id(OrigClientOrderId{"ORIG-123"}).build();

  ASSERT_THAT(confirmation.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

// endregion ModificationConfirmationBuilder

// region ModificationRejectBuilder

struct ModificationRejectBuilder : public Test {
  protocol::Session test_session{protocol::generator::Session{}};
  protocol::OrderModificationRequest request{test_session};

  matching_engine::ModificationRejectBuilder builder{test_session};
};

TEST_F(ModificationRejectBuilder, BuildsRejectForSession) {
  const auto reject = builder.build();

  ASSERT_THAT(reject.session, Eq(test_session));
}

TEST_F(ModificationRejectBuilder, SetsOrderStatusToRejectedByDefault) {
  const auto reject = builder.build();

  ASSERT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Rejected)));
}

TEST_F(ModificationRejectBuilder, SetsVenueOrderIdFromRequest) {
  request.venue_order_id = VenueOrderId{"ORD-123"};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"ORD-123"})));
}

TEST_F(ModificationRejectBuilder, SetsClientOrderIdFromRequest) {
  request.client_order_id = ClientOrderId{"CL-123"};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(ModificationRejectBuilder, SetsOrigClientOrderIdFromRequest) {
  request.orig_client_order_id = OrigClientOrderId{"ORIG-123"};

  const auto reject = builder.for_request(request).build();

  ASSERT_THAT(reject.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

TEST_F(ModificationRejectBuilder, SetsRejectReason) {
  const auto reject = builder.with_reason(RejectText{"test"}).build();

  ASSERT_THAT(reject.reject_text, Optional(Eq(RejectText{"test"})));
}

TEST_F(ModificationRejectBuilder, SetsOrderStatus) {
  const auto reject =
      builder.with_order_status(OrderStatus::Option::Filled).build();

  ASSERT_THAT(reject.order_status, Optional(Eq(OrderStatus::Option::Filled)));
}

TEST_F(ModificationRejectBuilder, SetsVenueOrderIdFromOrderId) {
  const auto reject = builder.with_order_id(OrderId{123}).build();

  ASSERT_THAT(reject.venue_order_id, Optional(Eq(VenueOrderId{"123"})));
}

TEST_F(ModificationRejectBuilder, SetsClientOrderId) {
  const auto reject =
      builder.with_client_order_id(ClientOrderId{"CL-123"}).build();

  ASSERT_THAT(reject.client_order_id, Optional(Eq(ClientOrderId{"CL-123"})));
}

TEST_F(ModificationRejectBuilder, SetsOrigClientOrderId) {
  const auto reject =
      builder.with_orig_client_order_id(OrigClientOrderId{"ORIG-123"}).build();

  ASSERT_THAT(reject.orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"ORIG-123"})));
}

// endregion ModificationRejectBuilder

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test