#include <gmock/gmock.h>

#include "common/order_rejects.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/party.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/types/session.hpp"

namespace simulator::trading_system::test {
namespace {

// NOLINTBEGIN(*magic-numbers*)

using namespace ::testing;  // NOLINT

[[nodiscard]]
auto make_session() -> protocol::Session {
  return protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT.1.1"},
                             protocol::fix::SenderCompId{"CLIENT"},
                             protocol::fix::TargetCompId{"SIM"}}};
}

struct TradingSystemOrderPlacementReject : public Test {
  [[nodiscard]]
  auto make_reject_of_request() const -> protocol::OrderPlacementReject {
    return make_reject(
        request, VenueOrderId{"venue-id"}, ExecutionId{"exec-id"}, "reason");
  }

  protocol::OrderPlacementRequest request{make_session()};
};

TEST_F(TradingSystemOrderPlacementReject, RepliesToRequestingSession) {
  ASSERT_EQ(make_reject_of_request().session, make_session());
}

TEST_F(TradingSystemOrderPlacementReject, CopiesInstrument) {
  request.instrument.symbol = Symbol{"AAPL"};

  ASSERT_EQ(make_reject_of_request().instrument, request.instrument);
}

TEST_F(TradingSystemOrderPlacementReject, CopiesParties) {
  request.parties = {Party{PartyId{"P"},
                           PartyIdSource::Option::Proprietary,
                           PartyRole::Option::ExecutingFirm}};

  ASSERT_THAT(make_reject_of_request().parties,
              ElementsAreArray(request.parties));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  ASSERT_THAT(make_reject_of_request().client_order_id,
              Optional(Eq(ClientOrderId{"client-id"})));
}

TEST_F(TradingSystemOrderPlacementReject, SetsGivenVenueOrderId) {
  ASSERT_THAT(make_reject_of_request().venue_order_id,
              Optional(Eq(VenueOrderId{"venue-id"})));
}

TEST_F(TradingSystemOrderPlacementReject, SetsGivenExecutionId) {
  ASSERT_THAT(make_reject_of_request().execution_id,
              Optional(Eq(ExecutionId{"exec-id"})));
}

TEST_F(TradingSystemOrderPlacementReject, ExplainsRejectWithGivenReason) {
  ASSERT_THAT(make_reject_of_request().reject_text,
              Optional(Eq(RejectText{"reason"})));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesOrderPrice) {
  request.order_price = OrderPrice{100};

  ASSERT_THAT(make_reject_of_request().order_price,
              Optional(Eq(OrderPrice{100})));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesOrderQuantity) {
  request.order_quantity = OrderQuantity{100};

  ASSERT_THAT(make_reject_of_request().order_quantity,
              Optional(Eq(OrderQuantity{100})));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesOrderType) {
  request.order_type = OrderType::Option::Limit;

  ASSERT_THAT(make_reject_of_request().order_type,
              Optional(Eq(OrderType::Option::Limit)));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesSide) {
  request.side = Side::Option::Buy;

  ASSERT_THAT(make_reject_of_request().side, Optional(Eq(Side::Option::Buy)));
}

TEST_F(TradingSystemOrderPlacementReject, CopiesTimeInForce) {
  request.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  ASSERT_THAT(make_reject_of_request().time_in_force,
              Optional(Eq(TimeInForce::Option::ImmediateOrCancel)));
}

struct TradingSystemOrderModificationReject : public Test {
  protocol::OrderModificationRequest request{make_session()};
};

TEST_F(TradingSystemOrderModificationReject, RepliesToRequestingSession) {
  ASSERT_EQ(make_reject(request, "reason").session, make_session());
}

TEST_F(TradingSystemOrderModificationReject, CopiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  ASSERT_THAT(make_reject(request, "reason").client_order_id,
              Optional(Eq(ClientOrderId{"client-id"})));
}

TEST_F(TradingSystemOrderModificationReject, CopiesOrigClientOrderId) {
  request.orig_client_order_id = OrigClientOrderId{"orig-client-id"};

  ASSERT_THAT(make_reject(request, "reason").orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"orig-client-id"})));
}

TEST_F(TradingSystemOrderModificationReject, CopiesVenueOrderId) {
  request.venue_order_id = VenueOrderId{"venue-id"};

  ASSERT_THAT(make_reject(request, "reason").venue_order_id,
              Optional(Eq(VenueOrderId{"venue-id"})));
}

TEST_F(TradingSystemOrderModificationReject, ExplainsRejectWithGivenReason) {
  ASSERT_THAT(make_reject(request, "reason").reject_text,
              Optional(Eq(RejectText{"reason"})));
}

TEST_F(TradingSystemOrderModificationReject, SetsRejectedOrderStatus) {
  ASSERT_THAT(make_reject(request, "reason").order_status,
              Optional(Eq(OrderStatus::Option::Rejected)));
}

struct TradingSystemOrderCancellationReject : public Test {
  protocol::OrderCancellationRequest request{make_session()};
};

TEST_F(TradingSystemOrderCancellationReject, RepliesToRequestingSession) {
  ASSERT_EQ(make_reject(request, "reason").session, make_session());
}

TEST_F(TradingSystemOrderCancellationReject, CopiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  ASSERT_THAT(make_reject(request, "reason").client_order_id,
              Optional(Eq(ClientOrderId{"client-id"})));
}

TEST_F(TradingSystemOrderCancellationReject, CopiesOrigClientOrderId) {
  request.orig_client_order_id = OrigClientOrderId{"orig-client-id"};

  ASSERT_THAT(make_reject(request, "reason").orig_client_order_id,
              Optional(Eq(OrigClientOrderId{"orig-client-id"})));
}

TEST_F(TradingSystemOrderCancellationReject, CopiesVenueOrderId) {
  request.venue_order_id = VenueOrderId{"venue-id"};

  ASSERT_THAT(make_reject(request, "reason").venue_order_id,
              Optional(Eq(VenueOrderId{"venue-id"})));
}

TEST_F(TradingSystemOrderCancellationReject, ExplainsRejectWithGivenReason) {
  ASSERT_THAT(make_reject(request, "reason").reject_text,
              Optional(Eq(RejectText{"reason"})));
}

TEST_F(TradingSystemOrderCancellationReject, SetsRejectedOrderStatus) {
  ASSERT_THAT(make_reject(request, "reason").order_status,
              Optional(Eq(OrderStatus::Option::Rejected)));
}

// NOLINTEND(*magic-numbers*)

}  // namespace
}  // namespace simulator::trading_system::test
