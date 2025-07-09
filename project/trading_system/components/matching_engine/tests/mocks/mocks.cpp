#include <variant>

#include "core/tools/overload.hpp"
#include "mocks/mock_client_notification_listener.hpp"
#include "mocks/mock_execution_reports_listener.hpp"
#include "mocks/mock_order_id_generator.hpp"
#include "mocks/trading_reply_receiver_mock.hpp"

namespace simulator::trading_system::matching_engine {

auto MockClientNotificationListener::on(Event event) -> void {
  const auto dispatcher = core::overload(
      [this](protocol::BusinessMessageReject message) {
        on_business_message_reject(std::move(message));
      },
      [this](protocol::OrderPlacementReject message) {
        on_placement_reject(std::move(message));
      },
      [this](protocol::OrderPlacementConfirmation message) {
        on_placement_confirmation(std::move(message));
      },
      [this](protocol::OrderModificationReject message) {
        on_modification_reject(std::move(message));
      },
      [this](protocol::OrderModificationConfirmation message) {
        on_modification_confirmation(std::move(message));
      },
      [this](protocol::OrderCancellationReject message) {
        on_cancellation_reject(std::move(message));
      },
      [this](protocol::OrderCancellationConfirmation message) {
        on_cancellation_confirmation(std::move(message));
      },
      [this](protocol::ExecutionReport message) {
        on_execution_report(std::move(message));
      },
      [this](protocol::MarketDataReject message) {
        on_market_data_reject(std::move(message));
      },
      [this](protocol::MarketDataSnapshot message) {
        on_market_data_snapshot(std::move(message));
      },
      [this](protocol::MarketDataUpdate message) {
        on_market_data_update(std::move(message));
      },
      [this](protocol::SecurityStatus message) {
        on_security_status(std::move(message));
      });

  if (auto* notification = std::get_if<ClientNotification>(&event.value)) {
    std::visit(dispatcher, std::move(notification->value));
  }
}

auto MockExecutionReportsListener::on(Event event) -> void {
  if (!std::holds_alternative<ClientNotification>(event.value)) {
    return;
  }

  auto& notification = std::get<ClientNotification>(event.value);
  if (!std::holds_alternative<protocol::ExecutionReport>(notification.value)) {
    return;
  }

  reports.emplace_back(
      std::get<protocol::ExecutionReport>(std::move(notification.value)));
}

auto MockOrderIdGenerator::operator()() -> OrderId { return generate(); }

TradingReplyReceiverMock::TradingReplyReceiverMock() = default;
TradingReplyReceiverMock::~TradingReplyReceiverMock() = default;

}  // namespace simulator::trading_system::matching_engine