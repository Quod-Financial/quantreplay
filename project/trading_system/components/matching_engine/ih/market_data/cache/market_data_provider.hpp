#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_MARKET_DATA_PROVIDER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_MARKET_DATA_PROVIDER_HPP_

#include "core/domain/market_data_entry.hpp"
#include "ih/market_data/streaming_settings.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class MarketDataProvider {
 public:
  MarketDataProvider() = default;
  MarketDataProvider(const MarketDataProvider&) = default;
  MarketDataProvider(MarketDataProvider&&) noexcept = default;
  virtual ~MarketDataProvider() = default;

  // clang-format off
  auto operator=(const MarketDataProvider&) -> MarketDataProvider& = default;
  auto operator=(MarketDataProvider&&) noexcept
      -> MarketDataProvider& = default;
  // clang-format on

  virtual auto compose_initial(const StreamingSettings& settings) const
      -> std::vector<MarketDataEntry> = 0;

  virtual auto compose_update(const StreamingSettings& settings) const
      -> std::vector<MarketDataEntry> = 0;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_MARKET_DATA_PROVIDER_HPP_
