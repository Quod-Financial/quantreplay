#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_NOTIFICATION_CREATORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_NOTIFICATION_CREATORS_HPP_

#include <string_view>
#include <utility>

#include "common/market_data_rejects.hpp"
#include "ih/common/events/client_notification.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::trading_system::matching_engine::mdata {

[[nodiscard]]
inline auto make_request_rejected_notification(
    const protocol::MarketDataRequest& request, std::string_view reason)
    -> ClientNotification {
  return ClientNotification{make_reject(request, reason)};
}

[[nodiscard]]
inline auto make_request_rejected_notification(
    const protocol::MarketDataRequest& request, MdRejectReason reason)
    -> ClientNotification {
  return ClientNotification{make_reject(request, reason)};
}

[[nodiscard]]
inline auto make_snapshot_published_notification(
    protocol::MarketDataSnapshot snapshot) -> ClientNotification {
  return ClientNotification{std::move(snapshot)};
}

[[nodiscard]]
inline auto make_update_published_notification(
    protocol::MarketDataUpdate update) -> ClientNotification {
  return ClientNotification{std::move(update)};
}

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_NOTIFICATION_CREATORS_HPP_
