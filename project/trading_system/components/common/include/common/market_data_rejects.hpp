#ifndef SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_DATA_REJECTS_HPP_
#define SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_DATA_REJECTS_HPP_

#include <optional>
#include <string_view>

#include "core/domain/attributes.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::trading_system {

[[nodiscard]]
auto find_market_data_request_violation(
    const protocol::MarketDataRequest& request)
    -> std::optional<std::string_view>;

[[nodiscard]]
auto make_reject(const protocol::MarketDataRequest& request,
                 std::string_view reason) -> protocol::MarketDataReject;

[[nodiscard]]
auto make_reject(const protocol::MarketDataRequest& request,
                 MdRejectReason reason) -> protocol::MarketDataReject;

[[nodiscard]]
auto make_reject(const protocol::MarketDataRequest& request,
                 MdRejectReason reason,
                 std::string_view text) -> protocol::MarketDataReject;

[[nodiscard]]
auto make_unknown_subscription_reject(
    const protocol::MarketDataRequest& request) -> protocol::MarketDataReject;

}  // namespace simulator::trading_system

#endif  // SIMULATOR_TRADING_SYSTEM_COMPONENTS_COMMON_MARKET_DATA_REJECTS_HPP_
