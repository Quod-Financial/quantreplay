#include "ih/market_data/cache/instrument_info_cache.hpp"

#include <ranges>

#include "common/trade.hpp"
#include "ih/market_data/tools/algorithms.hpp"

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

}  // namespace

auto InstrumentInfoCache::compose_initial(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  if (settings.is_data_type_requested(MdEntryType::Option::LowPrice)) {
    push_initial_entry(actual_data_.low_price, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::HighPrice)) {
    push_initial_entry(actual_data_.high_price, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::MidPrice)) {
    push_initial_entry(actual_data_.mid_price, destination);
  }
}

auto InstrumentInfoCache::compose_update(
    const StreamingSettings& settings,
    std::vector<MarketDataEntry>& destination) const -> void {
  if (settings.is_full_update_requested()) {
    compose_initial(settings, destination);
    return;
  }

  if (settings.is_data_type_requested(MdEntryType::Option::LowPrice)) {
    push_update_entry(last_update_.low_price, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::HighPrice)) {
    push_update_entry(last_update_.high_price, destination);
  }
  if (settings.is_data_type_requested(MdEntryType::Option::MidPrice)) {
    push_update_entry(last_update_.mid_price, destination);
  }
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
        const bool low_recovered = recover_low_price(info->low_price);
        const bool high_recovered = recover_high_price(info->high_price);
        if (low_recovered || high_recovered) {
          recover_mid_price();
        }
      } else {
        mark_prices_deleted();
      }
    }
  }
}

auto InstrumentInfoCache::store_state(
    std::optional<market_state::InstrumentInfo>& info) const -> void {
  const auto low_price = actual_data_.low_price.price();
  const auto high_price = actual_data_.high_price.price();
  if (low_price.has_value() && high_price.has_value()) {
    info = std::make_optional(market_state::InstrumentInfo{
        .low_price = *low_price, .high_price = *high_price});
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

auto InstrumentInfoCache::recover_low_price(Price trade_price) -> bool {
  if (actual_data_.low_price.update(trade_price)) {
    last_update_.low_price = actual_data_.low_price;
    return true;
  }
  return false;
}

auto InstrumentInfoCache::recover_high_price(Price trade_price) -> bool {
  if (actual_data_.high_price.update(trade_price)) {
    last_update_.high_price = actual_data_.high_price;
    return true;
  }
  return false;
}

auto InstrumentInfoCache::recover_mid_price() -> void {
  if (actual_data_.mid_price.update(get_mid_price(actual_data_))) {
    last_update_.mid_price = actual_data_.mid_price;
  }
}

auto InstrumentInfoCache::mark_prices_deleted() -> void {
  last_update_.low_price = actual_data_.low_price;
  last_update_.mid_price = actual_data_.mid_price;
  last_update_.high_price = actual_data_.high_price;

  actual_data_ = {};
  last_update_.low_price.mark_deleted();
  last_update_.mid_price.mark_deleted();
  last_update_.high_price.mark_deleted();
}

auto InstrumentInfoCache::push_initial_entry(
    const auto& px, std::vector<MarketDataEntry>& destination) -> void {
  if (auto price = px.price()) {
    MarketDataEntry entry;
    entry.price = price;
    entry.type = px.type();
    destination.emplace_back(std::move(entry));
  }
}

auto InstrumentInfoCache::push_update_entry(
    const auto& px, std::vector<MarketDataEntry>& destination) -> void {
  if (auto price = px.price()) {
    MarketDataEntry entry;
    entry.price = price;
    entry.type = px.type();
    entry.action = px.action();
    destination.emplace_back(std::move(entry));
  }
}

}  // namespace simulator::trading_system::matching_engine::mdata