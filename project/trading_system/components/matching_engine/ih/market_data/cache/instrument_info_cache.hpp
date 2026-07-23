#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_

#include "common/instrument_state.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "ih/market_data/tools/instrument_px.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class InstrumentInfoCache {
  struct CachedData {
    InstrumentPx<MdEntryType::Option::LowPrice> low_price;
    InstrumentPx<MdEntryType::Option::MidPrice> mid_price;
    InstrumentPx<MdEntryType::Option::HighPrice> high_price;
    InstrumentPx<MdEntryType::Option::OpeningPrice> opening_price;
    InstrumentPx<MdEntryType::Option::ClosingPrice> closing_price;
    InstrumentPx<MdEntryType::Option::AuctionClearingPrice, true>
        auction_clearing_price;
    InstrumentPx<MdEntryType::Option::EarlyPrice, true> early_price;
    InstrumentPx<MdEntryType::Option::PreviousClosingPrice>
        previous_closing_price;
  };

 public:
  auto compose_initial(const StreamingSettings& settings,
                       std::vector<MarketDataEntry>& destination) const -> void;

  auto compose_update(const StreamingSettings& settings,
                      std::vector<MarketDataEntry>& destination) const -> void;

  [[nodiscard]]
  auto has_update(const StreamingSettings& settings) const -> bool;

  auto update(const std::vector<OrderBookNotification>& updates) -> void;

  auto store_state(std::optional<market_state::InstrumentInfo>& info) const
      -> void;

 private:
  auto update_low_price(Price trade_price) -> void;

  auto update_high_price(Price trade_price) -> void;

  auto update_mid_price() -> void;

  auto set_low_price(Price price) -> bool;

  auto set_high_price(Price price) -> bool;

  auto recalculate_mid_price() -> void;

  auto apply_auction_prices(const AuctionPricesUpdate& prices) -> void;

  auto apply_early_price(const EarlyPriceUpdate& early) -> void;

  auto reset_session_high_low(Price opening_price) -> void;

  auto mark_prices_deleted() -> void;

  auto compose(const StreamingSettings& settings,
               std::vector<MarketDataEntry>& destination,
               const CachedData& data,
               bool with_action) const -> void;

  CachedData actual_data_;
  CachedData last_update_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
