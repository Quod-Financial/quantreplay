#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_CHECKERS_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_CHECKERS_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "ih/market_data/validation/errors.hpp"

namespace simulator::trading_system::matching_engine::mdata {

using ValidationResult = std::optional<ValidationError>;

struct LowPriceRespectsTick {
  explicit LowPriceRespectsTick(std::optional<PriceTick> tick) noexcept;

  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;

 private:
  std::optional<PriceTick> tick_;
};

struct HighPriceRespectsTick {
  explicit HighPriceRespectsTick(std::optional<PriceTick> tick) noexcept;

  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;

 private:
  std::optional<PriceTick> tick_;
};

struct LowPriceIsLessThanOrEqualToHighPrice {
  auto operator()(const market_state::InstrumentInfo& instrument_info) const
      -> std::optional<ValidationError>;
};

struct TradePriceRespectsTick {
  explicit TradePriceRespectsTick(std::optional<PriceTick> tick) noexcept;

  auto operator()(const Trade& trade) const -> std::optional<ValidationError>;

 private:
  std::optional<PriceTick> tick_;
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
