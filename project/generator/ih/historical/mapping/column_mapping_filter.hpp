#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_COLUMN_MAPPING_FILTER_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_COLUMN_MAPPING_FILTER_HPP_

#include <cstdint>
#include <vector>

#include "data_layer/api/converters/column_mapping.hpp"
#include "data_layer/api/models/column_mapping.hpp"

namespace simulator::generator::historical::mapping {

auto filter(const std::vector<data_layer::ColumnMapping>& mapping_configs,
            std::uint32_t depth_to_parse)
    -> std::vector<data_layer::converter::ColumnConfig>;

}  // namespace simulator::generator::historical::mapping

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_COLUMN_MAPPING_FILTER_HPP_
