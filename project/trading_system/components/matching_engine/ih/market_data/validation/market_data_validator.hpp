#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_MARKET_DATA_VALIDATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_MARKET_DATA_VALIDATOR_HPP_

#include <optional>

#include "common/attributes.hpp"
#include "common/market_state/snapshot.hpp"
#include "ih/common/validation/validation.hpp"
#include "ih/market_data/validation/errors.hpp"
#include "ih/market_data/validation/validator.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class MarketDataValidation : public Validator {
  template <typename T>
  using Validation = matching_engine::Validation<T, ValidationError>;

 public:
  struct Config {
    std::optional<MinQuantity> min_quantity;
    std::optional<MaxQuantity> max_quantity;
    std::optional<QuantityTick> quantity_tick;
    std::optional<PriceTick> price_tick;
  };

  auto configure(Config config) -> void;

  [[nodiscard]]
  auto validate(const market_state::InstrumentInfo& instrument_info) const
      -> Conclusion override;

  [[nodiscard]]
  auto validate(const Trade& trade) const -> Conclusion override;

 private:
  auto run(Validation<market_state::InstrumentInfo>& validation) const -> void;
  auto run(Validation<Trade>& validation) const -> void;

  Config config_;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_MARKET_DATA_VALIDATOR_HPP_
