#include <gmock/gmock.h>

#include "common/attributes.hpp"
#include "ih/orders/replies/client_reject_reporter.hpp"
#include "mocks/mock_client_notification_listener.hpp"
#include "mocks/mock_order_id_generator.hpp"
#include "tools/protocol_test_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

using namespace ::testing;  // NOLINT

/*----------------------------------------------------------------------------*/

class OrderPlacementRejectReporting : public Test {
 public:
  auto listener() -> decltype(auto) { return (listener_); }
  auto order_id_generator() -> decltype(auto) { return (order_id_generator_); }

  auto report_request_rejected(auto&&... args) -> void {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
    reporter_.notify_rejected(std::forward<decltype(args)>(args)...);
  }

  protocol::OrderPlacementRequest request =
      make_message<protocol::OrderPlacementRequest>();

  protocol::OrderPlacementReject reject =
      make_message<protocol::OrderPlacementReject>();

 private:
  auto SetUp() -> void override {
    ON_CALL(listener_, on_placement_reject).WillByDefault(SaveArg<0>(&reject));
    ON_CALL(order_id_generator_, generate).WillByDefault(Return(OrderId{1}));
  }

  StrictMock<MockClientNotificationListener> listener_;
  NiceMock<MockOrderIdGenerator> order_id_generator_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderPlacementRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_placement_reject);

  report_request_rejected(request, "");
}

TEST_F(OrderPlacementRejectReporting, SpecifiesSession) {
  request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT1.1"},
                             protocol::fix::SenderCompId{"Sender"},
                             protocol::fix::TargetCompId{"Target"}}};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.session, Eq(request.session));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesInstrument) {
  request.instrument = {.security_id = std::nullopt,
                        .symbol = Symbol{"test symbol"},
                        .currency = std::nullopt,
                        .security_exchange = std::nullopt,
                        .parties = {},
                        .requester_instrument_id = std::nullopt,
                        .security_type = std::nullopt,
                        .security_id_source = std::nullopt};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.instrument.symbol, Eq(request.instrument.symbol));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesParties) {
  request.parties = {Party{PartyId{"P"},
                           PartyIdSource::Option::Proprietary,
                           PartyRole::Option::ExecutingFirm}};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.parties, ElementsAreArray(request.parties));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.client_order_id, Eq(request.client_order_id));
}

TEST_F(OrderPlacementRejectReporting, GeneratesRejectedVenueOrderId) {
  EXPECT_CALL(order_id_generator(), generate).WillOnce(Return(OrderId{10000}));
  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.venue_order_id, Eq(VenueOrderId{"10000"}));
}

TEST_F(OrderPlacementRejectReporting, GeneratesRejectExecutionId) {
  EXPECT_CALL(order_id_generator(), generate).WillOnce(Return(OrderId{10000}));
  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.execution_id, Eq(ExecutionId{"10000-1"}));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesRejectReason) {
  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject.reject_text, Eq(RejectText("test reject reason")));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesOrderPrice) {
  request.order_price = OrderPrice{100};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.order_price, Eq(request.order_price));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesOrderQuantity) {
  request.order_quantity = OrderQuantity{100};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.order_quantity, Eq(request.order_quantity));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesOrderType) {
  request.order_type = OrderType::Option::Limit;

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.order_type, Eq(request.order_type));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesSide) {
  request.side = Side::Option::Buy;

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.side, Eq(request.side));
}

TEST_F(OrderPlacementRejectReporting, SpecifiesTimeInForce) {
  request.time_in_force = TimeInForce::Option::ImmediateOrCancel;

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.time_in_force, Eq(request.time_in_force));
}

/*----------------------------------------------------------------------------*/

class OrderModificationRejectReporting : public Test {
 public:
  auto listener() -> decltype(auto) { return (listener_); }

  auto report_request_rejected(auto&&... args) -> void {
    // NOLINTNEXTLINE(*array-to-pointer-decay*)
    reporter_.notify_rejected(std::forward<decltype(args)>(args)...);
  }

  protocol::OrderModificationRequest request =
      make_message<protocol::OrderModificationRequest>();

  protocol::OrderModificationReject reject =
      make_message<protocol::OrderModificationReject>();

 private:
  auto SetUp() -> void override {
    ON_CALL(listener_, on_modification_reject)
        .WillByDefault(SaveArg<0>(&reject));
  }

  StrictMock<MockOrderIdGenerator> order_id_generator_;
  StrictMock<MockClientNotificationListener> listener_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderModificationRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_modification_reject);

  report_request_rejected(request, "");
}

TEST_F(OrderModificationRejectReporting, SpecifiesSession) {
  request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT1.1"},
                             protocol::fix::SenderCompId{"Sender"},
                             protocol::fix::TargetCompId{"Target"}}};

  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.session, Eq(request.session));
}

TEST_F(OrderModificationRejectReporting, SpecifiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.client_order_id, Eq(request.client_order_id));
}

TEST_F(OrderModificationRejectReporting, SpecifiesOrigClientOrderId) {
  request.orig_client_order_id = OrigClientOrderId{"orig-client-id"};

  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.orig_client_order_id, Eq(request.orig_client_order_id));
}

TEST_F(OrderModificationRejectReporting, SpecifiesVenueOrderId) {
  request.venue_order_id = VenueOrderId{"venue-id"};

  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.venue_order_id, Eq(request.venue_order_id));
}

TEST_F(OrderModificationRejectReporting, SpecifiesRejectReason) {
  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject.reject_text, Eq(RejectText("test reject reason")));
}

TEST_F(OrderModificationRejectReporting, HardcodesRejectedOrderStatus) {
  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.order_status, Eq(OrderStatus::Option::Rejected));
}

/*----------------------------------------------------------------------------*/

class OrderCancellationRejectReporting : public Test {
 public:
  auto listener() -> decltype(auto) { return (listener_); }

  auto report_request_rejected(auto&&... args) -> void {
    // NOLINTNEXTLINE(*array-to-pointer-decay*)
    reporter_.notify_rejected(std::forward<decltype(args)>(args)...);
  }

  protocol::OrderCancellationRequest request =
      make_message<protocol::OrderCancellationRequest>();

  protocol::OrderCancellationReject reject =
      make_message<protocol::OrderCancellationReject>();

 private:
  auto SetUp() -> void override {
    ON_CALL(listener_, on_cancellation_reject)
        .WillByDefault(SaveArg<0>(&reject));
  }

  StrictMock<MockOrderIdGenerator> order_id_generator_;
  StrictMock<MockClientNotificationListener> listener_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderCancellationRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_cancellation_reject);

  report_request_rejected(request, "");
}

TEST_F(OrderCancellationRejectReporting, SpecifiesSession) {
  request.session = protocol::Session{
      protocol::fix::Session{protocol::fix::BeginString{"FIXT1.1"},
                             protocol::fix::SenderCompId{"Sender"},
                             protocol::fix::TargetCompId{"Target"}}};

  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.session, Eq(request.session));
}

TEST_F(OrderCancellationRejectReporting, SpecifiesOrigClientOrderId) {
  request.orig_client_order_id = OrigClientOrderId{"orig-client-id"};

  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.orig_client_order_id, Eq(request.orig_client_order_id));
}

TEST_F(OrderCancellationRejectReporting, SpecifiesVenueOrderId) {
  request.venue_order_id = VenueOrderId{"venue-id"};

  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.venue_order_id, Eq(request.venue_order_id));
}

TEST_F(OrderCancellationRejectReporting, SpecifiesClientOrderId) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.client_order_id, Eq(request.client_order_id));
}

TEST_F(OrderCancellationRejectReporting, SpecifiesRejectReason) {
  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject.reject_text, Eq(RejectText("test reject reason")));
}

TEST_F(OrderCancellationRejectReporting, HardcodesRejectedOrderStatus) {
  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "");

  ASSERT_THAT(reject.order_status, Eq(OrderStatus::Option::Rejected));
}

/*----------------------------------------------------------------------------*/

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test