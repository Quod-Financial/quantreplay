#include "ih/market_data/validation/checkers.hpp"

#include <functional>

#include "ih/common/validation/checker_utils.hpp"

namespace simulator::trading_system::matching_engine::mdata {

LowPriceRespectsTick::LowPriceRespectsTick(
    std::optional<PriceTick> tick) noexcept
    : tick_{tick} {}

auto LowPriceRespectsTick::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  return field_respects_tick(
      instrument_info.low_price, tick_, ValidationError::LowPriceTickViolated);
}

HighPriceRespectsTick::HighPriceRespectsTick(
    std::optional<PriceTick> tick) noexcept
    : tick_{tick} {}

auto HighPriceRespectsTick::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  return field_respects_tick(instrument_info.high_price,
                             tick_,
                             ValidationError::HighPriceTickViolated);
}

auto LowPriceIsLessThanOrEqualToHighPrice::operator()(
    const market_state::InstrumentInfo& instrument_info) const
    -> std::optional<ValidationError> {
  return fields_respect_order(
      instrument_info.low_price,
      instrument_info.high_price,
      std::less_equal{},
      ValidationError::LowPriceIsLessThanOrEqualToHighPriceViolated);
};

TradePriceRespectsTick::TradePriceRespectsTick(
    std::optional<PriceTick> tick) noexcept
    : tick_{tick} {}

auto TradePriceRespectsTick::operator()(const Trade& trade) const
    -> std::optional<ValidationError> {
  return field_respects_tick(
      trade.trade_price, tick_, ValidationError::TradePriceTickViolated);
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
