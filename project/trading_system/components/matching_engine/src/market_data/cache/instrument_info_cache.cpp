#include "ih/market_data/cache/instrument_info_cache.hpp"

#include <cassert>
#include <optional>

#include "common/trade.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace {

constexpr auto low_price_changed(const auto& data, Price trade_price) -> bool {
  const auto trade = static_cast<double>(trade_price);
  const auto low = static_cast<std::optional<double>>(data.low_price);
  return low.has_value() ? *low > trade : true;
}

constexpr auto high_price_changed(const auto& data, Price trade_price) -> bool {
  const auto trade = static_cast<double>(trade_price);
  const auto high = static_cast<std::optional<double>>(data.high_price);
  return high.has_value() ? *high < trade : true;
}

constexpr auto get_mid_price(const auto& data) -> std::optional<Price> {
  const auto low = static_cast<std::optional<double>>(data.low_price);
  const auto high = static_cast<std::optional<double>>(data.high_price);

  if (!low.has_value() || !high.has_value()) {
    return std::nullopt;
  }

  // I'm not sure if it is valid.
  // This calculation migrated from an old implementation.
  // And it seems that the mid-price may not respect price tick restrictions.
  return std::make_optional<Price>((*low + *high) / 2.);
}

auto assign(auto& actual, auto& last_update, auto value) -> void {
  if (actual.update(value)) {
    last_update = actual;
  }
}

auto assign(auto& actual,
            auto& last_update,
            std::optional<Price> price,
            std::optional<Quantity> quantity) -> void {
  if (actual.update(price, quantity)) {
    last_update = actual;
  }
}

template <typename Px>
auto clear_slot(Px& actual, Px& last_update) -> void {
  last_update = actual;
  last_update.mark_deleted();
  actual = Px{};
}

auto for_each_px(const auto& data, const auto& visit) -> void {
  visit(data.low_price);
  visit(data.high_price);
  visit(data.mid_price);
  visit(data.opening_price);
  visit(data.closing_price);
  visit(data.auction_clearing_price);
  visit(data.early_price);
  visit(data.previous_closing_price);
}

}  // namespace

auto InstrumentInfoCache::compose_initial(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  compose(settings, destination, actual_data_, false);
}

auto InstrumentInfoCache::compose_update(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  compose(settings, destination, last_update_, true);
}

auto InstrumentInfoCache::has_update(const StreamingSettings& settings) const
    -> bool {
  bool reportable = false;
  for_each_px(last_update_, [&](const auto& px) {
    reportable = reportable || (px.price().has_value() &&
                                settings.is_data_type_requested(px.type()));
  });
  return reportable;
}

auto InstrumentInfoCache::compose(const StreamingSettings& settings,
                                  std::vector<MarketDataEntry>& destination,
                                  const CachedData& data,
                                  bool with_action) const -> void {
  const auto emit = [&](const auto& px) {
    const auto price = px.price();
    if (!price || !settings.is_data_type_requested(px.type())) {
      return;
    }
    MarketDataEntry entry;
    entry.price = price;
    if constexpr (requires { px.quantity(); }) {
      entry.quantity = px.quantity();
    }
    entry.type = px.type();
    if (with_action) {
      entry.action = px.action();
    }
    destination.emplace_back(std::move(entry));
  };

  for_each_px(data, emit);
}

auto InstrumentInfoCache::update(
    const std::vector<OrderBookNotification>& updates) -> void {
  last_update_ = {};  // reset state of the last update
  for (const auto& update : updates) {
    if (const auto* trade = std::get_if<Trade>(&update.value)) {
      update_low_price(trade->trade_price);
      update_high_price(trade->trade_price);
    }
    if (const auto* recover =
            std::get_if<InstrumentInfoRecover>(&update.value)) {
      const auto& info = recover->info;
      if (info.has_value()) {
        const bool low_recovered =
            info->low_price && set_low_price(*info->low_price);
        const bool high_recovered =
            info->high_price && set_high_price(*info->high_price);
        if (low_recovered || high_recovered) {
          recalculate_mid_price();
        }
        assign(actual_data_.opening_price,
               last_update_.opening_price,
               info->opening_price);
        assign(actual_data_.closing_price,
               last_update_.closing_price,
               info->closing_price);
        assign(actual_data_.auction_clearing_price,
               last_update_.auction_clearing_price,
               info->auction_clearing_price,
               info->auction_clearing_quantity);
        assign(actual_data_.previous_closing_price,
               last_update_.previous_closing_price,
               info->previous_closing_price);
      } else {
        mark_prices_deleted();
      }
    }
    if (const auto* auction = std::get_if<AuctionPricesUpdate>(&update.value)) {
      apply_auction_prices(*auction);
    }
    if (const auto* early = std::get_if<EarlyPriceUpdate>(&update.value)) {
      apply_early_price(*early);
    }
  }
}

auto InstrumentInfoCache::store_state(
    std::optional<market_state::InstrumentInfo>& info) const -> void {
  // The mid-price is derived and intentionally not persisted; it is recomputed
  // from low/high on recovery.
  market_state::InstrumentInfo stored{
      .low_price = actual_data_.low_price.price(),
      .high_price = actual_data_.high_price.price(),
      .opening_price = actual_data_.opening_price.price(),
      .closing_price = actual_data_.closing_price.price(),
      .auction_clearing_price = actual_data_.auction_clearing_price.price(),
      .auction_clearing_quantity =
          actual_data_.auction_clearing_price.quantity(),
      .previous_closing_price = actual_data_.previous_closing_price.price()};

  if (stored != market_state::InstrumentInfo{}) {
    info = std::move(stored);
  }
}

auto InstrumentInfoCache::update_low_price(Price trade_price) -> void {
  if (low_price_changed(actual_data_, trade_price)) {
    actual_data_.low_price.update(trade_price);
    last_update_.low_price = actual_data_.low_price;
    update_mid_price();
  }
}

auto InstrumentInfoCache::update_high_price(Price trade_price) -> void {
  if (high_price_changed(actual_data_, trade_price)) {
    actual_data_.high_price.update(trade_price);
    last_update_.high_price = actual_data_.high_price;
    update_mid_price();
  }
}

auto InstrumentInfoCache::update_mid_price() -> void {
  actual_data_.mid_price.update(get_mid_price(actual_data_));
  last_update_.mid_price = actual_data_.mid_price;
}

auto InstrumentInfoCache::set_low_price(Price price) -> bool {
  if (actual_data_.low_price.update(price)) {
    last_update_.low_price = actual_data_.low_price;
    return true;
  }
  return false;
}

auto InstrumentInfoCache::set_high_price(Price price) -> bool {
  if (actual_data_.high_price.update(price)) {
    last_update_.high_price = actual_data_.high_price;
    return true;
  }
  return false;
}

auto InstrumentInfoCache::recalculate_mid_price() -> void {
  if (actual_data_.mid_price.update(get_mid_price(actual_data_))) {
    last_update_.mid_price = actual_data_.mid_price;
  }
}

auto InstrumentInfoCache::apply_auction_prices(
    const AuctionPricesUpdate& prices) -> void {
  using Phase = TradingPhase::Option;
  const bool crossed = prices.clearing_price.has_value();

  if (crossed) {
    assign(actual_data_.auction_clearing_price,
           last_update_.auction_clearing_price,
           prices.clearing_price,
           prices.clearing_quantity);
  } else {
    clear_slot(actual_data_.auction_clearing_price,
               last_update_.auction_clearing_price);
  }

  switch (prices.auction_phase) {
    case Phase::OpeningAuction:
      if (crossed) {
        assign(actual_data_.opening_price,
               last_update_.opening_price,
               prices.clearing_price);
        reset_session_high_low(*prices.clearing_price);
      } else {
        clear_slot(actual_data_.opening_price, last_update_.opening_price);
      }
      break;
    case Phase::ClosingAuction:
      assign(actual_data_.previous_closing_price,
             last_update_.previous_closing_price,
             actual_data_.closing_price.price());
      if (crossed) {
        assign(actual_data_.closing_price,
               last_update_.closing_price,
               prices.clearing_price);
      } else {
        clear_slot(actual_data_.closing_price, last_update_.closing_price);
      }
      break;
    case Phase::IntradayAuction:
      break;
    case Phase::Open:
    case Phase::Closed:
    case Phase::PostTrading:
      assert(false && "apply_auction_prices requires an auction phase");
      break;
  }
}

auto InstrumentInfoCache::apply_early_price(const EarlyPriceUpdate& early)
    -> void {
  if (early.early_price.has_value()) {
    // The 30-second indicative tick republishes 269=P even when the value is
    // unchanged, so the change dedup of assign() must not swallow repeats.
    actual_data_.early_price.force_update(*early.early_price,
                                          early.early_quantity);
    last_update_.early_price = actual_data_.early_price;
  } else {
    clear_slot(actual_data_.early_price, last_update_.early_price);
  }
}

auto InstrumentInfoCache::reset_session_high_low(Price opening_price) -> void {
  const bool low_changed = set_low_price(opening_price);
  const bool high_changed = set_high_price(opening_price);
  if (low_changed || high_changed) {
    recalculate_mid_price();
  }
}

auto InstrumentInfoCache::mark_prices_deleted() -> void {
  clear_slot(actual_data_.low_price, last_update_.low_price);
  clear_slot(actual_data_.high_price, last_update_.high_price);
  clear_slot(actual_data_.mid_price, last_update_.mid_price);
  clear_slot(actual_data_.opening_price, last_update_.opening_price);
  clear_slot(actual_data_.closing_price, last_update_.closing_price);
  clear_slot(actual_data_.auction_clearing_price,
             last_update_.auction_clearing_price);
  clear_slot(actual_data_.previous_closing_price,
             last_update_.previous_closing_price);
}

}  // namespace simulator::trading_system::matching_engine::mdata