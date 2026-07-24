#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_

#include <optional>

#include "common/instrument_state.hpp"
#include "core/domain/attributes.hpp"
#include "core/domain/market_data_entry.hpp"
#include "core/tools/time.hpp"
#include "ih/common/events/order_book_notification.hpp"
#include "ih/market_data/streaming_settings.hpp"
#include "ih/market_data/tools/md_entry_value.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class InstrumentInfoCache {
  struct PriceQuantity {
    Price price;
    Quantity quantity;

    auto operator==(const PriceQuantity&) const -> bool = default;
  };

  struct CachedData {
    MdEntryValue<MdEntryType::Option::LowPrice, Price> low_price;
    MdEntryValue<MdEntryType::Option::MidPrice, Price> mid_price;
    MdEntryValue<MdEntryType::Option::HighPrice, Price> high_price;
    MdEntryValue<MdEntryType::Option::OpeningPrice, Price> opening_price;
    MdEntryValue<MdEntryType::Option::ClosingPrice, Price> closing_price;
    MdEntryValue<MdEntryType::Option::AuctionClearingPrice, PriceQuantity>
        auction_clearing_price;
    MdEntryValue<MdEntryType::Option::EarlyPrice, PriceQuantity> early_price;
    MdEntryValue<MdEntryType::Option::PreviousClosingPrice, Price>
        previous_closing_price;
    MdEntryValue<MdEntryType::Option::TradeVolume, Quantity>
        trade_volume;
    std::optional<core::sys_us> opening_price_time;
    std::optional<core::sys_us> closing_price_time;
  };

 public:
  struct Config {
    core::TzClock clock;

    bool opening_auction_scheduled = false;
    bool closing_auction_scheduled = false;
  };

  auto configure(Config config) -> void;

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

  auto update_on_first_trade(const Trade& trade) -> bool;

  auto update_closing_price(const Trade& trade) -> void;

  auto update_closing_price(const TzDayPassed& day_passed) -> void;

  auto set_low_price(Price price) -> bool;

  auto set_high_price(Price price) -> bool;

  auto recalculate_mid_price() -> void;

  auto add_to_trade_volume(Quantity quantity) -> void;

  auto apply_auction_prices(const AuctionPricesUpdate& prices) -> void;

  auto apply_early_price(const EarlyPriceUpdate& early) -> void;

  auto reset_session_high_low(Price opening_price) -> void;

  auto mark_deleted() -> void;

  auto compose(const StreamingSettings& settings,
               std::vector<MarketDataEntry>& destination,
               const CachedData& data,
               bool with_action) const -> void;

  auto less_tz_date(core::sys_us lh, core::sys_us rh) const -> bool;

  Config config_;
  CachedData actual_data_;
  CachedData last_update_;

  std::optional<Trade> last_trade_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_CACHE_INSTRUMENT_INFO_CACHE_HPP_
