#ifndef SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REQUEST_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REQUEST_CHANNEL_HPP_

#include "middleware/routing/errors.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::middleware {

auto send_trading_request(protocol::OrderPlacementRequest request) -> void;

auto send_trading_request(protocol::OrderModificationRequest request) -> void;

auto send_trading_request(protocol::OrderCancellationRequest request) -> void;

auto send_trading_request(protocol::MarketDataRequest request) -> void;

auto send_trading_request(protocol::SecurityStatusRequest request) -> void;

auto send_trading_request(const protocol::InstrumentStateRequest& request,
                          protocol::InstrumentState& reply) -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_TRADING_REQUEST_CHANNEL_HPP_