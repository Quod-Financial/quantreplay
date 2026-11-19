#include "ih/market_data/validation/checkers.hpp"

#include <functional>

#include "common/checker_utils.hpp"

namespace simulator::trading_system::matching_engine::mdata {

LowPriceRespectsTick::LowPriceRespectsTick(
    const core::PriceTickRanges& tick_ranges) noexcept
    : tick_ranges_{&tick_ranges} {}

auto LowPriceRespectsTick::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  return price_respects_tick(instrument_info.low_price,
                             *tick_ranges_,
                             ValidationError::LowPriceTickViolated);
}

HighPriceRespectsTick::HighPriceRespectsTick(
    const core::PriceTickRanges& tick_ranges) noexcept
    : tick_ranges_{&tick_ranges} {}

auto HighPriceRespectsTick::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  return price_respects_tick(instrument_info.high_price,
                             *tick_ranges_,
                             ValidationError::HighPriceTickViolated);
}

auto LowPriceIsLessThanOrEqualToHighPrice::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  if (!instrument_info.low_price.has_value() ||
      !instrument_info.high_price.has_value()) {
    return std::nullopt;
  }
  return fields_respect_order(
      *instrument_info.low_price,
      *instrument_info.high_price,
      std::less_equal{},
      ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated);
};

TradePriceRespectsTick::TradePriceRespectsTick(
    const core::PriceTickRanges& tick_ranges) noexcept
    : tick_ranges_{&tick_ranges} {}

auto TradePriceRespectsTick::operator()(const Trade& trade) const
    -> std::optional<ValidationError> {
  return price_respects_tick(trade.trade_price,
                             *tick_ranges_,
                             ValidationError::TradePriceTickViolated);
}

TradedQuantityRespectsTick::TradedQuantityRespectsTick(
    std::optional<QuantityTick> tick) noexcept
    : tick_{tick} {}

auto TradedQuantityRespectsTick::operator()(const Trade& trade) const
    -> std::optional<ValidationError> {
  return field_respects_tick(trade.traded_quantity,
                             tick_,
                             ValidationError::TradedQuantityTickViolated);
}

TradedQuantityRespectsMinimum::TradedQuantityRespectsMinimum(
    std::optional<MinQuantity> min) noexcept
    : min_{min} {}

auto TradedQuantityRespectsMinimum::operator()(const Trade& trade) const
    -> std::optional<ValidationError> {
  return field_respects_minimum(
      trade.traded_quantity, min_, ValidationError::TradedQuantityMinViolated);
}

TradedQuantityRespectsMaximum::TradedQuantityRespectsMaximum(
    std::optional<MaxQuantity> max) noexcept
    : max_{max} {}

auto TradedQuantityRespectsMaximum::operator()(const Trade& trade) const
    -> std::optional<ValidationError> {
  return field_respects_maximum(
      trade.traded_quantity, max_, ValidationError::TradedQuantityMaxViolated);
}

}  // namespace simulator::trading_system::matching_engine::mdata
