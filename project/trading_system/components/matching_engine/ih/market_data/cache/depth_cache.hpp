#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_DEPTH_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_DEPTH_CACHE_HPP_

#include "core/domain/market_data_entry.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/depth/depth_sheet.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "ih/market_data/tools/market_entry_id_generator.hpp"
#include "protocol/app/instrument_state_request.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class DepthCache {
 public:
  struct Config {
    bool allow_orders_exclusion = false;
  };

  explicit DepthCache(MarketEntryIdGenerator& id_generator);

  auto configure(Config config) -> void;

  auto compose_initial(const StreamingSettings& settings,
                       std::vector<MarketDataEntry>& destination) const -> void;

  auto compose_update(const StreamingSettings& settings,
                      std::vector<MarketDataEntry>& destination) const -> void;

  auto capture(protocol::InstrumentState& state) const -> void;

  auto update(const std::vector<OrderBookNotification>& updates) -> void;

 private:
  auto apply(const auto& update) -> void;

  auto build_full_update(const auto& sheet,
                         const StreamingSettings& settings,
                         std::vector<MarketDataEntry>& destination) const
      -> void;

  auto build_incremental_update(const auto& sheet,
                                const StreamingSettings& settings,
                                std::vector<MarketDataEntry>& destination) const
      -> void;

  DepthSheet bid_depth_;
  DepthSheet offer_depth_;
  Config config_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_DEPTH_CACHE_HPP_
