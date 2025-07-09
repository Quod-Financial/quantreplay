#ifndef SIMULATOR_GENERATOR_IH_UTILS_VALIDATOR_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_VALIDATOR_HPP_

#include "data_layer/api/models/listing.hpp"
#include "data_layer/api/models/price_seed.hpp"

namespace simulator::generator {

class Validator {
 public:
  static auto is_acceptable(const data_layer::Listing& listing) -> bool;

  static auto is_acceptable_for_random_generation(
      const data_layer::Listing& listing) -> bool;

  static auto is_acceptable(const data_layer::PriceSeed& price_seed) -> bool;
};

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_UTILS_VALIDATOR_HPP_
