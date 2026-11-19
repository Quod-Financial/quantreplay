#include <gmock/gmock.h>

#include "common/attributes.hpp"
#include "ih/orders/replies/client_reject_reporter.hpp"
#include "mocks/client_notification_listener_mock.hpp"
#include "mocks/order_id_generator_mock.hpp"
#include "tools/protocol_tools.hpp"

namespace simulator::trading_system::matching_engine::test {
namespace {

// NOLINTBEGIN(*magic-numbers*,*non-private-member*)

using namespace ::testing;  // NOLINT

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

  StrictMock<ClientNotificationListenerMock> listener_;
  NiceMock<OrderIdGeneratorMock> order_id_generator_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderPlacementRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_placement_reject);

  report_request_rejected(request, "");
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

TEST_F(OrderPlacementRejectReporting, RejectsGivenRequestWithGivenReason) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_placement_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject,
              AllOf(Field(&protocol::OrderPlacementReject::client_order_id,
                          Eq(ClientOrderId{"client-id"})),
                    Field(&protocol::OrderPlacementReject::reject_text,
                          Eq(RejectText{"test reject reason"}))));
}

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

  StrictMock<OrderIdGeneratorMock> order_id_generator_;
  StrictMock<ClientNotificationListenerMock> listener_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderModificationRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_modification_reject);

  report_request_rejected(request, "");
}

TEST_F(OrderModificationRejectReporting, RejectsGivenRequestWithGivenReason) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_modification_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject,
              AllOf(Field(&protocol::OrderModificationReject::client_order_id,
                          Eq(ClientOrderId{"client-id"})),
                    Field(&protocol::OrderModificationReject::reject_text,
                          Eq(RejectText{"test reject reason"}))));
}

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

  StrictMock<OrderIdGeneratorMock> order_id_generator_;
  StrictMock<ClientNotificationListenerMock> listener_;
  order::ClientRejectReporter reporter_{listener_, order_id_generator_};
};

TEST_F(OrderCancellationRejectReporting, EmitsRejectNotification) {
  EXPECT_CALL(listener(), on_cancellation_reject);

  report_request_rejected(request, "");
}

TEST_F(OrderCancellationRejectReporting, RejectsGivenRequestWithGivenReason) {
  request.client_order_id = ClientOrderId{"client-id"};

  EXPECT_CALL(listener(), on_cancellation_reject);
  report_request_rejected(request, "test reject reason");

  ASSERT_THAT(reject,
              AllOf(Field(&protocol::OrderCancellationReject::client_order_id,
                          Eq(ClientOrderId{"client-id"})),
                    Field(&protocol::OrderCancellationReject::reject_text,
                          Eq(RejectText{"test reject reason"}))));
}

// NOLINTEND(*magic-numbers*,*non-private-member*)

}  // namespace
}  // namespace simulator::trading_system::matching_engine::test