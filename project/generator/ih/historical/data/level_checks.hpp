#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_LEVEL_CHECKS_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_LEVEL_CHECKS_HPP_

#include "ih/historical/data/record.hpp"

namespace simulator::generator::historical {

[[nodiscard]]
auto has_valid_bid(const historical::Level& level) noexcept -> bool;

[[nodiscard]]
auto has_valid_offer(const historical::Level& level) noexcept -> bool;

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_DATA_LEVEL_CHECKS_HPP_
