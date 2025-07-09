#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_TRADE_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_TRADE_CACHE_HPP_

#include <optional>
#include <vector>

#include "common/trade.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "ih/market_data/tools/market_entry_id_generator.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class TradeCache {
 public:
  struct Config {
    bool report_trade_volume = true;
    bool report_trade_parties = true;
    bool report_trade_aggressor_side = true;
  };

  explicit TradeCache(MarketEntryIdGenerator& id_generator);

  auto configure(Config config) -> void;

  auto compose_initial(const StreamingSettings& settings,
                       std::vector<MarketDataEntry>& destination) const -> void;

  auto compose_update(const StreamingSettings& settings,
                      std::vector<MarketDataEntry>& destination) const -> void;

  auto update(const std::vector<OrderBookNotification>& updates) -> void;

  auto store_state(std::optional<Trade>& last_trade) const -> void;

 private:
  auto compose_market_entry(const Trade& trade) const -> MarketDataEntry;

  std::optional<Trade> last_trade_;
  std::vector<Trade> cached_trades_;
  MarketEntryIdGenerator& id_generator_;
  Config config_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_TRADE_CACHE_HPP_
