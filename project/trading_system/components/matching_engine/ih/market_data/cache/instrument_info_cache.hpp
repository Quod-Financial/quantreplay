#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_

#include "common/market_state/snapshot.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "ih/market_data/tools/instrument_px.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class InstrumentInfoCache {
  template <typename>
  struct CachedData {
    InstrumentPx<MdEntryType::Option::LowPrice> low_price;
    InstrumentPx<MdEntryType::Option::MidPrice> mid_price;
    InstrumentPx<MdEntryType::Option::HighPrice> high_price;
  };

 public:
  auto compose_initial(const StreamingSettings& settings,
                       std::vector<MarketDataEntry>& destination) const -> void;

  auto compose_update(const StreamingSettings& settings,
                      std::vector<MarketDataEntry>& destination) const -> void;

  auto update(const std::vector<OrderBookNotification>& updates) -> void;

  auto store_state(std::optional<market_state::InstrumentInfo>& info) const
      -> void;

 private:
  auto update_low_price(Price trade_price) -> void;

  auto update_high_price(Price trade_price) -> void;

  auto update_mid_price() -> void;

  auto recover_low_price(Price trade_price) -> bool;

  auto recover_high_price(Price trade_price) -> bool;

  auto recover_mid_price() -> void;

  auto mark_prices_deleted() -> void;

  static auto push_initial_entry(const auto& px,
                                 std::vector<MarketDataEntry>& destination)
      -> void;

  static auto push_update_entry(const auto& px,
                                std::vector<MarketDataEntry>& destination)
      -> void;

  CachedData<struct Actual> actual_data_;
  CachedData<struct LastUpdate> last_update_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
