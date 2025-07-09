#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REQUEST_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REQUEST_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/app/instrument_state_request.hpp"
#include "protocol/app/market_data_request.hpp"
#include "protocol/app/order_cancellation_request.hpp"
#include "protocol/app/order_modification_request.hpp"
#include "protocol/app/order_placement_request.hpp"
#include "protocol/app/security_status_request.hpp"

namespace simulator::middleware {

struct TradingRequestReceiver : public detail::Receiver {
  virtual auto process(protocol::OrderPlacementRequest request) -> void = 0;
  virtual auto process(protocol::OrderModificationRequest request) -> void = 0;
  virtual auto process(protocol::OrderCancellationRequest request) -> void = 0;
  virtual auto process(protocol::MarketDataRequest request) -> void = 0;
  virtual auto process(protocol::SecurityStatusRequest request) -> void = 0;

  virtual auto process(const protocol::InstrumentStateRequest& request,
                       protocol::InstrumentState& reply) -> void = 0;
};

// Allows the receiver receiving messages sent via the channel,
// unbinds previous receiver from the channel if any.
auto bind_trading_request_channel(
    std::shared_ptr<TradingRequestReceiver> receiver) -> void;

// Unbinds previous receiver from the channel if any.
auto release_trading_request_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_TRADING_REQUEST_CHANNEL_HPP_