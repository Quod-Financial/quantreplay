#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_NOTIFICATION_CREATORS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_TOOLS_NOTIFICATION_CREATORS_HPP_

#include <string>
#include <string_view>
#include <utility>

#include "ih/common/events/client_notification.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace detail {

[[nodiscard]]
constexpr auto describe(MdRejectReason reason) -> std::string_view {
  switch (reason.value()) {
    case MdRejectReason::Option::UnknownSymbol:
      return "unknown instrument";
    case MdRejectReason::Option::DuplicateMdReqId:
      return "request identifier is used already";
  }
  return "unknown error";
}

}  // namespace detail

[[nodiscard]]
inline auto make_request_rejected_notification(
    const protocol::MarketDataRequest& request,
    std::string_view reason) -> ClientNotification {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_text = RejectText{std::string(reason)};
  return ClientNotification{std::move(reject)};
}

[[nodiscard]]
inline auto make_request_rejected_notification(
    const protocol::MarketDataRequest& request,
    MdRejectReason reason) -> ClientNotification {
  protocol::MarketDataReject reject{request.session};
  reject.request_id = request.request_id;
  reject.reject_reason = reason;
  reject.reject_text = RejectText{std::string(detail::describe(reason))};
  return ClientNotification{std::move(reject)};
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
