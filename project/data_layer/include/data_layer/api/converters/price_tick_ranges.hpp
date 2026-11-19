#ifndef SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_PRICE_TICK_RANGES_HPP_
#define SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_PRICE_TICK_RANGES_HPP_

#include <vector>

#include "core/tools/price_tick_ranges.hpp"
#include "data_layer/api/models/price_tick_range.hpp"

namespace simulator::data_layer::converter {

[[nodiscard]]
auto to_price_tick_ranges(const std::vector<PriceTickRange>& rows)
    -> core::PriceTickRanges;

}  // namespace simulator::data_layer::converter

#endif  // SIMULATOR_DATA_LAYER_INCLUDE_DATA_LAYER_API_CONVERTERS_PRICE_TICK_RANGES_HPP_
