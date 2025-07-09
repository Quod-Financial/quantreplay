#ifndef SIMULATOR_MIDDLEWARE_ROUTING_TRADING_PHASE_ADMIN_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_ROUTING_TRADING_PHASE_ADMIN_CHANNEL_HPP_

#include "middleware/routing/errors.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::middleware {

auto send_admin_request(const protocol::HaltPhaseRequest& request,
                        protocol::HaltPhaseReply& reply) -> void;

auto send_admin_request(const protocol::ResumePhaseRequest& request,
                        protocol::ResumePhaseReply& reply) -> void;

auto send_admin_request(const protocol::StoreMarketStateRequest& request,
                        protocol::StoreMarketStateReply& reply) -> void;

auto send_admin_request(const protocol::RecoverMarketStateRequest& request,
                        protocol::RecoverMarketStateReply& reply) -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_ROUTING_TRADING_PHASE_ADMIN_CHANNEL_HPP_
