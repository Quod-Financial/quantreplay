#ifndef SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_VALIDATOR_HPP_
#define SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_VALIDATOR_HPP_

#include "common/market_state/snapshot.hpp"
#include "ih/common/validation/conclusion.hpp"

namespace simulator::trading_system::matching_engine::mdata {

class Validator {
 public:
  Validator() = default;
  Validator(const Validator&) = default;
  Validator(Validator&&) = default;
  virtual ~Validator() = default;

  auto operator=(const Validator&) -> Validator& = default;
  auto operator=(Validator&&) -> Validator& = default;

  virtual auto validate(const market_state::InstrumentInfo& instrument_info)
      const -> Conclusion = 0;

  virtual auto validate(const Trade& trade) const -> Conclusion = 0;
};

}  // namespace simulator::trading_system::matching_engine::mdata

#endif  // SIMULATOR_MATCHING_ENGINE_IH_MARKET_DATA_VALIDATION_VALIDATOR_HPP_
