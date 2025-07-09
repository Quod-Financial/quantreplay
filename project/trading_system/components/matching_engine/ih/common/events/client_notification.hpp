#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_CLIENT_NOTIFICATION_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_CLIENT_NOTIFICATION_HPP_

#include <concepts>
#include <utility>
#include <variant>

#include "protocol/app/business_message_reject.hpp"
#include "protocol/app/execution_report.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"
#include "protocol/app/order_cancellation_confirmation.hpp"
#include "protocol/app/order_cancellation_reject.hpp"
#include "protocol/app/order_modification_confirmation.hpp"
#include "protocol/app/order_modification_reject.hpp"
#include "protocol/app/order_placement_confirmation.hpp"
#include "protocol/app/order_placement_reject.hpp"
#include "protocol/app/security_status.hpp"

namespace simulator::trading_system::matching_engine {

struct ClientNotification {
  using Value = std::variant<protocol::OrderPlacementConfirmation,
                             protocol::OrderPlacementReject,
                             protocol::OrderModificationConfirmation,
                             protocol::OrderModificationReject,
                             protocol::OrderCancellationConfirmation,
                             protocol::OrderCancellationReject,
                             protocol::ExecutionReport,
                             protocol::MarketDataReject,
                             protocol::MarketDataSnapshot,
                             protocol::MarketDataUpdate,
                             protocol::SecurityStatus,
                             protocol::BusinessMessageReject>;

  template <typename NotificationType>
    requires std::constructible_from<Value, NotificationType>
  explicit ClientNotification(NotificationType&& notification)
      : value(std::forward<NotificationType>(notification)) {}

  Value value;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_EVENTS_CLIENT_NOTIFICATION_HPP_
