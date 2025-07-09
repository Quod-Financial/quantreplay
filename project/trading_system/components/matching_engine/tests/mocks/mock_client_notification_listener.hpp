#ifndef SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_CLIENT_NOTIFICATION_LISTENER_HPP_
#define SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_CLIENT_NOTIFICATION_LISTENER_HPP_

#include <gmock/gmock.h>

#include "ih/common/abstractions/event_listener.hpp"
#include "ih/common/events/event.hpp"
#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/security_status.hpp"

namespace simulator::trading_system::matching_engine {

class MockClientNotificationListener : public EventListener {
 public:
  // clang-format off
  MOCK_METHOD(void, on_business_message_reject, (protocol::BusinessMessageReject));
  MOCK_METHOD(void, on_placement_confirmation, (protocol::OrderPlacementConfirmation));
  MOCK_METHOD(void, on_placement_reject, (protocol::OrderPlacementReject));
  MOCK_METHOD(void, on_modification_confirmation, (protocol::OrderModificationConfirmation));
  MOCK_METHOD(void, on_modification_reject, (protocol::OrderModificationReject));
  MOCK_METHOD(void, on_cancellation_confirmation, (protocol::OrderCancellationConfirmation));
  MOCK_METHOD(void, on_cancellation_reject, (protocol::OrderCancellationReject));
  MOCK_METHOD(void, on_execution_report, (protocol::ExecutionReport));
  MOCK_METHOD(void, on_market_data_reject, (protocol::MarketDataReject));
  MOCK_METHOD(void, on_market_data_snapshot, (protocol::MarketDataSnapshot));
  MOCK_METHOD(void, on_market_data_update, (protocol::MarketDataUpdate));
  MOCK_METHOD(void, on_security_status, (protocol::SecurityStatus));
  // clang-format on

 private:
  auto on(Event event) -> void override;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_TESTS_MOCKS_MOCK_CLIENT_NOTIFICATION_LISTENER_HPP_
