#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_CACHE_MANAGER_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_CACHE_MANAGER_HPP_

#include "common/market_state/snapshot.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/cache/depth_cache.hpp"
#include "ih/market_data/cache/instrument_info_cache.hpp"
#include "ih/market_data/cache/market_data_provider.hpp"
#include "ih/market_data/cache/trade_cache.hpp"
#include "ih/market_data/tools/market_entry_id_generator.hpp"
#include "matching_engine/configuration.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class CacheManager final : public MarketDataProvider {
 public:
  explicit CacheManager(const Configuration& configuration);

  auto compose_initial(const StreamingSettings& settings) const
      -> std::vector<MarketDataEntry> override;

  auto compose_update(const StreamingSettings& settings) const
      -> std::vector<MarketDataEntry> override;

  auto capture(protocol::InstrumentState& state) const -> void;

  auto store_state(std::optional<Trade>& last_trade,
                   std::optional<market_state::InstrumentInfo>& info) -> void;

  auto push(OrderBookNotification notification) -> void;

  auto apply_pending_changes() -> void;

  auto was_updated() const -> bool;

 private:
  std::unique_ptr<MarketEntryIdGenerator> entry_id_generator_;
  std::vector<OrderBookNotification> pending_notifications_;
  DepthCache depth_cache_;
  TradeCache trade_cache_;
  InstrumentInfoCache instrument_info_cache_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_CACHE_MANAGER_HPP_
