#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REQUEST_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REQUEST_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/app/market_data_request.hpp"

namespace simulator::middleware {

struct MarketDataRequestReceiver : public detail::Receiver {
  virtual auto process(protocol::MarketDataRequest request) -> void = 0;
};

auto bind_market_data_request_channel(
    std::shared_ptr<MarketDataRequestReceiver> receiver) -> void;

auto release_market_data_request_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REQUEST_CHANNEL_HPP_
