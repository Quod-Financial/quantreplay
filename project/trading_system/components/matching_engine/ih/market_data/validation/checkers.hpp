#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_CHECKERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_CHECKERS_HPP_

#include <gsl/pointers>
#include <optional>

#include "common/attributes.hpp"
#include "common/instrument_state.hpp"
#include "core/tools/price_tick_ranges.hpp"
#include "ih/market_data/validation/errors.hpp"

namespace simulator::trading_system::matching_engine::mdata {

using ValidationResult = std::optional<ValidationError>;

struct LowPriceRespectsTick {
  explicit LowPriceRespectsTick(
      const core::PriceTickRanges& tick_ranges) noexcept;

  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;

 private:
  gsl::not_null<const core::PriceTickRanges*> tick_ranges_;
};

struct HighPriceRespectsTick {
  explicit HighPriceRespectsTick(
      const core::PriceTickRanges& tick_ranges) noexcept;

  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;

 private:
  gsl::not_null<const core::PriceTickRanges*> tick_ranges_;
};

struct LowPriceIsLessThanOrEqualToHighPrice {
  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;
};

struct TradePriceRespectsTick {
  explicit TradePriceRespectsTick(
      const core::PriceTickRanges& tick_ranges) noexcept;

  auto operator()(const Trade& trade) const -> std::optional<ValidationError>;

 private:
  gsl::not_null<const core::PriceTickRanges*> tick_ranges_;
};

struct TradedQuantityRespectsTick {
  explicit TradedQuantityRespectsTick(
      std::optional<QuantityTick> tick) noexcept;

  auto operator()(const Trade& trade) const -> std::optional<ValidationError>;

 private:
  std::optional<QuantityTick> tick_;
};

struct TradedQuantityRespectsMinimum {
  explicit TradedQuantityRespectsMinimum(
      std::optional<MinQuantity> min) noexcept;

  auto operator()(const Trade& trade) const -> std::optional<ValidationError>;

 private:
  std::optional<MinQuantity> min_;
};

struct TradedQuantityRespectsMaximum {
  explicit TradedQuantityRespectsMaximum(
      std::optional<MaxQuantity> max) noexcept;

  auto operator()(const Trade& trade) const -> std::optional<ValidationError>;

 private:
  std::optional<MaxQuantity> max_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_CHECKERS_HPP_
