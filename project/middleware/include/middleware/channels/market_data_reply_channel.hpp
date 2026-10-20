#ifndef SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REPLY_CHANNEL_HPP_
#define SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REPLY_CHANNEL_HPP_

#include <memory>

#include "middleware/channels/detail/receiver.hpp"
#include "protocol/app/market_data_reject.hpp"
#include "protocol/app/market_data_snapshot.hpp"
#include "protocol/app/market_data_update.hpp"

namespace simulator::middleware {

struct MarketDataReplyReceiver : public detail::Receiver {
  virtual auto process(protocol::MarketDataSnapshot snapshot) -> void = 0;

  virtual auto process(protocol::MarketDataUpdate update) -> void = 0;

  virtual auto process(protocol::MarketDataReject reject) -> void = 0;
};

auto bind_market_data_reply_channel(
    std::shared_ptr<MarketDataReplyReceiver> receiver) -> void;

auto release_market_data_reply_channel() noexcept -> void;

}  // namespace simulator::middleware

#endif  // SIMULATOR_MIDDLEWARE_CHANNELS_MARKET_DATA_REPLY_CHANNEL_HPP_
