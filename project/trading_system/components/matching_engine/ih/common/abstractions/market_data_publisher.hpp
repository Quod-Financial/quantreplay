#ifndef SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_PUBLISHER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_PUBLISHER_HPP_

namespace simulator::trading_system::matching_engine {

class MarketDataPublisher {
 public:
  MarketDataPublisher() = default;
  MarketDataPublisher(const MarketDataPublisher&) = default;
  MarketDataPublisher(MarketDataPublisher&&) = default;
  virtual ~MarketDataPublisher() = default;

  auto operator=(const MarketDataPublisher&) -> MarketDataPublisher& = default;
  auto operator=(MarketDataPublisher&&) -> MarketDataPublisher& = default;

  virtual auto publish() -> void = 0;
};

}  // namespace simulator::trading_system::matching_engine

#endif  // SIMULATOR_MATCHING_ENGINE_IH_COMMON_ABSTRACTIONS_MARKET_DATA_PUBLISHER_HPP_