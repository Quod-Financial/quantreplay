#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_PARAMS_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_PARAMS_HPP_

#include <string>
#include <vector>

#include "data_layer/api/models/column_mapping.hpp"
#include "data_layer/api/models/datasource.hpp"
#include "ih/historical/mapping/datasource_params.hpp"
#include "ih/historical/mapping/depth_config.hpp"
#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical {

class MappingParams {
 public:
  using ColumnNames = std::vector<std::string>;
  using ColumnMappings = std::vector<data_layer::ColumnMapping>;

  explicit MappingParams(ColumnMappings mapping_configs,
                         DatasourceParams datasource_params) noexcept;

  auto initialize(mapping::DepthConfig depth_config) -> void;

  auto initialize(ColumnNames column_names, mapping::DepthConfig depth_config)
      -> void;

  [[nodiscard]]
  auto column_idx(data_layer::converter::ColumnFrom column) const
      -> std::optional<std::uint32_t>;

 private:
  mapping::Specification spec_;
  ColumnMappings column_mappings_;
  DatasourceParams datasource_params_;
};

auto make_mapping_params(const data_layer::Datasource& datasource)
    -> MappingParams;

}  // namespace simulator::generator::historical

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_PARAMS_HPP_
