#ifndef SIMULATOR_GENERATOR_IH_UTILS_PRICE_SEED_SELECTION_HPP_
#define SIMULATOR_GENERATOR_IH_UTILS_PRICE_SEED_SELECTION_HPP_

#include <optional>
#include <span>
#include <string_view>

#include "data_layer/api/models/price_seed.hpp"

namespace simulator::generator {

[[nodiscard]]
auto select_price_seed(std::string_view instrument_symbol,
                       std::optional<std::string_view> price_currency,
                       std::span<const data_layer::PriceSeed> price_seeds)
    -> std::optional<data_layer::PriceSeed>;

}  // namespace simulator::generator

#endif  // SIMULATOR_GENERATOR_IH_UTILS_PRICE_SEED_SELECTION_HPP_
