#include "ih/market_data/cache/instrument_info_cache.hpp"

#include <cassert>
#include <concepts>
#include <optional>
#include <type_traits>

#include "common/trade.hpp"
#include "core/tools/time.hpp"
#include "ih/market_data/tools/algorithms.hpp"

namespace simulator::trading_system::matching_engine::mdata {

namespace {

constexpr auto low_price_changed(const auto& data, Price trade_price) -> bool {
  const auto& low = data.low_price.value();
  return low.has_value() ? *low > trade_price : true;
}

constexpr auto high_price_changed(const auto& data, Price trade_price) -> bool {
  const auto& high = data.high_price.value();
  return high.has_value() ? *high < trade_price : true;
}

constexpr auto get_mid_price(const auto& data) -> std::optional<Price> {
  const auto& low = data.low_price.value();
  const auto& high = data.high_price.value();

  if (!low.has_value() || !high.has_value()) {
    return std::nullopt;
  }

  // I'm not sure if it is valid.
  // This calculation migrated from an old implementation.
  // And it seems that the mid-price may not respect price tick restrictions.
  return std::make_optional<Price>(
      (static_cast<double>(*low) + static_cast<double>(*high)) / 2.);
}

auto assign(auto& actual, auto& last_update, auto value) -> void {
  if (actual.update(value)) {
    last_update = actual;
  }
}

auto assign_with_time(auto& actual,
                      auto& last_update,
                      std::optional<Price> price,
                      std::optional<core::sys_us>& update_time,
                      std::optional<core::sys_us> time) -> void {
  if (actual.update(std::move(price))) {
    last_update = actual;
  }
  update_time = time;
}

auto roll_closing_price(
    MdEntryValue<MdEntryType::Option::PreviousClosingPrice, Price>&
        previous_actual,
    MdEntryValue<MdEntryType::Option::PreviousClosingPrice, Price>&
        previous_last_update,
    MdEntryValue<MdEntryType::Option::ClosingPrice, Price>& actual,
    MdEntryValue<MdEntryType::Option::ClosingPrice, Price>& last_update,
    std::optional<Price> price,
    std::optional<core::sys_us>& update_time,
    std::optional<core::sys_us> time) -> void {
  if (previous_actual.update(actual.value())) {
    previous_last_update = previous_actual;
  }
  if (actual.update(std::move(price))) {
    last_update = actual;
  }

  update_time = time;
}

template <typename T>
auto clear_slot(T& actual, T& last_update) -> void {
  last_update = actual;
  last_update.mark_deleted();
  actual = T{};
}

auto for_each_md_entry(const auto& data, const auto& visit) -> void {
  visit(data.low_price);
  visit(data.high_price);
  visit(data.mid_price);
  visit(data.opening_price);
  visit(data.closing_price);
  visit(data.auction_clearing_price);
  visit(data.early_price);
  visit(data.previous_closing_price);
  visit(data.trade_volume);
}

template <typename T>
auto clear_slot(T& actual,
                T& last_update,
                std::optional<core::sys_us>& update_time) -> void {
  clear_slot(actual, last_update);
  update_time = core::get_current_system_time();
}

}  // namespace

auto InstrumentInfoCache::configure(Config config) -> void { config_ = config; }

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
  for_each_md_entry(last_update_, [&](const auto& md_entry) {
    reportable =
        reportable || (md_entry.value().has_value() &&
                       settings.is_data_type_requested(md_entry.type()));
  });
  return reportable;
}

auto InstrumentInfoCache::compose(const StreamingSettings& settings,
                                  std::vector<MarketDataEntry>& destination,
                                  const CachedData& data,
                                  bool with_action) const -> void {
  const auto emit = [&](const auto& md_entry_value) {
    const auto& value = md_entry_value.value();
    if (!value || !settings.is_data_type_requested(md_entry_value.type())) {
      return;
    }
    MarketDataEntry entry;
    using ValueType = std::remove_cvref_t<decltype(*value)>;
    if constexpr (std::same_as<ValueType, Price>) {
      entry.price = *value;
    } else if constexpr (std::same_as<ValueType, Quantity>) {
      entry.quantity = *value;
    } else if constexpr (std::same_as<ValueType, PriceQuantity>) {
      entry.price = value->price;
      entry.quantity = value->quantity;
    } else {
      static_assert(core::always_false_v<ValueType>, "unhandled ValueType");
    }
    entry.type = md_entry_value.type();
    if (with_action) {
      entry.action = md_entry_value.action();
    }
    destination.emplace_back(std::move(entry));
  };

  for_each_md_entry(data, emit);
}

auto InstrumentInfoCache::update(
    const std::vector<OrderBookNotification>& updates) -> void {
  last_update_ = {};  // reset state of the last update
  for (const auto& update : updates) {
    if (const auto* trade = std::get_if<Trade>(&update.value)) {
      update_low_price(trade->trade_price);
      update_high_price(trade->trade_price);

      if (!update_on_first_trade(*trade) &&
          trade->market_phase.trading_phase() == TradingPhase::Option::Open) {
        add_to_trade_volume(trade->traded_quantity);
      }
      update_closing_price(*trade);

      last_trade_ = *trade;
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
        assign_with_time(actual_data_.opening_price,
                         last_update_.opening_price,
                         info->opening_price,
                         actual_data_.opening_price_time,
                         info->opening_price_time);
        assign_with_time(actual_data_.closing_price,
                         last_update_.closing_price,
                         info->closing_price,
                         actual_data_.closing_price_time,
                         info->closing_price_time);

        const auto& clearing_price = info->auction_clearing_price;
        const auto& clearing_quantity = info->auction_clearing_quantity;
        std::optional<PriceQuantity> clearing;
        if (clearing_price && clearing_quantity) {
          clearing = PriceQuantity{*clearing_price, *clearing_quantity};
        }
        assign(actual_data_.auction_clearing_price,
               last_update_.auction_clearing_price,
               clearing);

        assign(actual_data_.previous_closing_price,
               last_update_.previous_closing_price,
               info->previous_closing_price);

        assign(actual_data_.trade_volume,
               last_update_.trade_volume,
               info->trade_volume);
      } else {
        mark_deleted();
      }
    }
    if (const auto* auction = std::get_if<AuctionPricesUpdate>(&update.value)) {
      apply_auction_prices(*auction);
    }
    if (const auto* early = std::get_if<EarlyPriceUpdate>(&update.value)) {
      apply_early_price(*early);
    }
    if (const auto* day_passed = std::get_if<TzDayPassed>(&update.value)) {
      update_closing_price(*day_passed);
    }
  }
}

auto InstrumentInfoCache::store_state(
    std::optional<market_state::InstrumentInfo>& info) const -> void {
  // The mid-price is derived and intentionally not persisted; it is recomputed
  // from low/high on recovery.
  const auto& clearing = actual_data_.auction_clearing_price.value();
  market_state::InstrumentInfo stored{
      .low_price = actual_data_.low_price.value(),
      .high_price = actual_data_.high_price.value(),
      .opening_price = actual_data_.opening_price.value(),
      .opening_price_time = actual_data_.opening_price_time,
      .closing_price = actual_data_.closing_price.value(),
      .closing_price_time = actual_data_.closing_price_time,
      .auction_clearing_price =
          clearing ? std::make_optional(clearing->price) : std::nullopt,
      .auction_clearing_quantity =
          clearing ? std::make_optional(clearing->quantity) : std::nullopt,
      .previous_closing_price = actual_data_.previous_closing_price.value(),
      .trade_volume = actual_data_.trade_volume.value()};

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

auto InstrumentInfoCache::update_on_first_trade(const Trade& trade) -> bool {
  if (config_.opening_auction_scheduled) {
    return false;
  }

  const bool first_trade_today = [&]() -> bool {
    if (actual_data_.opening_price_time.has_value()) {
      return core::to_tz_date(*actual_data_.opening_price_time, config_.clock) <
             core::to_tz_date(trade.trade_time, config_.clock);
    }
    return true;
  }();

  if (!first_trade_today) {
    return false;
  }

  assign_with_time(actual_data_.opening_price,
                   last_update_.opening_price,
                   trade.trade_price,
                   actual_data_.opening_price_time,
                   trade.trade_time);
  if (trade.market_phase.trading_phase() == TradingPhase::Option::Open) {
    assign(actual_data_.trade_volume,
           last_update_.trade_volume,
           trade.traded_quantity);
  }
  reset_session_high_low(trade.trade_price);
  return true;
}

auto InstrumentInfoCache::update_closing_price(const Trade& trade) -> void {
  if (config_.closing_auction_scheduled) {
    return;
  }

  if (!last_trade_.has_value()) {
    return;
  }

  // New Trade comes after midnight but before TzDayPassed
  const bool first_trade_today =
      core::to_tz_date(last_trade_->trade_time, config_.clock) <
      core::to_tz_date(trade.trade_time, config_.clock);

  // TzDayPassed may have already applied the last trade to the closing price
  const bool already_rolled_today =
      actual_data_.closing_price_time.has_value() &&
      core::to_tz_date(*actual_data_.closing_price_time, config_.clock) >=
          core::to_tz_date(trade.trade_time, config_.clock);

  if (first_trade_today && !already_rolled_today) {
    roll_closing_price(actual_data_.previous_closing_price,
                       last_update_.previous_closing_price,
                       actual_data_.closing_price,
                       last_update_.closing_price,
                       last_trade_->trade_price,
                       actual_data_.closing_price_time,
                       trade.trade_time);
  }
}

auto InstrumentInfoCache::update_closing_price(const TzDayPassed& day_passed)
    -> void {
  if (config_.closing_auction_scheduled) {
    return;
  }

  if (!last_trade_.has_value()) {
    return;
  }

  // TzDayPassed comes before the "Trade after midnight"
  const bool trade_before_midnight =
      core::to_tz_date(last_trade_->trade_time, config_.clock) <
      core::to_tz_date(day_passed.sys_tick_time, config_.clock);

  // The first trade after midnight may have already rolled the closing price
  const bool already_rolled_today =
      actual_data_.closing_price_time.has_value() &&
      core::to_tz_date(*actual_data_.closing_price_time, config_.clock) >=
          core::to_tz_date(day_passed.sys_tick_time, config_.clock);

  if (trade_before_midnight && !already_rolled_today) {
    roll_closing_price(actual_data_.previous_closing_price,
                       last_update_.previous_closing_price,
                       actual_data_.closing_price,
                       last_update_.closing_price,
                       last_trade_->trade_price,
                       actual_data_.closing_price_time,
                       day_passed.sys_tick_time);
  }
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

auto InstrumentInfoCache::add_to_trade_volume(Quantity qty) -> void {
  const auto& current = actual_data_.trade_volume.value();
  const double accumulated = (current ? current->value() : 0.0) + qty.value();
  assign(actual_data_.trade_volume,
         last_update_.trade_volume,
         Quantity{accumulated});
}

auto InstrumentInfoCache::apply_auction_prices(
    const AuctionPricesUpdate& prices) -> void {
  using Phase = TradingPhase::Option;
  const bool crossed = prices.clearing_value.has_value();

  if (crossed) {
    assign(actual_data_.auction_clearing_price,
           last_update_.auction_clearing_price,
           std::make_optional(PriceQuantity{prices.clearing_value->price,
                                            prices.clearing_value->quantity}));
  } else {
    clear_slot(actual_data_.auction_clearing_price,
               last_update_.auction_clearing_price);
  }

  switch (prices.auction_phase) {
    case Phase::OpeningAuction:
      if (crossed) {
        assign_with_time(actual_data_.opening_price,
                         last_update_.opening_price,
                         std::make_optional(prices.clearing_value->price),
                         actual_data_.opening_price_time,
                         core::get_current_system_time());
        assign(actual_data_.trade_volume,
               last_update_.trade_volume,
               prices.clearing_value->quantity);
        reset_session_high_low(prices.clearing_value->price);
      } else {
        clear_slot(actual_data_.opening_price,
                   last_update_.opening_price,
                   actual_data_.opening_price_time);
        clear_slot(actual_data_.trade_volume, last_update_.trade_volume);
      }
      break;
    case Phase::ClosingAuction:
      assign(actual_data_.previous_closing_price,
             last_update_.previous_closing_price,
             actual_data_.closing_price.value());
      if (crossed) {
        assign_with_time(actual_data_.closing_price,
                         last_update_.closing_price,
                         std::make_optional(prices.clearing_value->price),
                         actual_data_.closing_price_time,
                         core::get_current_system_time());
        add_to_trade_volume(prices.clearing_value->quantity);
      } else {
        clear_slot(actual_data_.closing_price,
                   last_update_.closing_price,
                   actual_data_.closing_price_time);
      }
      break;
    case Phase::IntradayAuction:
      if (crossed) {
        add_to_trade_volume(prices.clearing_value->quantity);
      }
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
  if (early.early_value.has_value()) {
    // The 30-second indicative tick republishes 269=P even when the value is
    // unchanged, so the change dedup of assign() must not swallow repeats.
    actual_data_.early_price.force_update(
        PriceQuantity{early.early_value->price, early.early_value->quantity});
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

auto InstrumentInfoCache::mark_deleted() -> void {
  clear_slot(actual_data_.low_price, last_update_.low_price);
  clear_slot(actual_data_.high_price, last_update_.high_price);
  clear_slot(actual_data_.mid_price, last_update_.mid_price);
  clear_slot(actual_data_.opening_price,
             last_update_.opening_price,
             actual_data_.opening_price_time);
  clear_slot(actual_data_.closing_price,
             last_update_.closing_price,
             actual_data_.closing_price_time);
  clear_slot(actual_data_.auction_clearing_price,
             last_update_.auction_clearing_price);
  clear_slot(actual_data_.previous_closing_price,
             last_update_.previous_closing_price);
  clear_slot(actual_data_.trade_volume, last_update_.trade_volume);
}

}  // namespace simulator::trading_system::matching_engine::mdata