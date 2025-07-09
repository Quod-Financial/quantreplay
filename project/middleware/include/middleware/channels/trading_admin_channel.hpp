#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_PHASE_ADMIN_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_PHASE_ADMIN_CHANNEL_HPP_

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/admin/market_state.hpp"
#include "protocol/admin/trading_phase.hpp"

namespace simulator::middleware {

struct TradingAdminRequestReceiver : public detail::Receiver {
  virtual auto process(const protocol::HaltPhaseRequest& request,
                       protocol::HaltPhaseReply& reply) -> void = 0;

  virtual auto process(const protocol::ResumePhaseRequest& request,
                       protocol::ResumePhaseReply& reply) -> void = 0;

  virtual auto process(const protocol::StoreMarketStateRequest& request,
                       protocol::StoreMarketStateReply& reply) -> void = 0;

  virtual auto process(const protocol::RecoverMarketStateRequest& request,
                       protocol::RecoverMarketStateReply& reply) -> void = 0;
};

auto bind_trading_admin_channel(
    std::shared_ptr<TradingAdminRequestReceiver> receiver) -> void;

auto release_trading_admin_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_PHASE_ADMIN_CHANNEL_HPP_
