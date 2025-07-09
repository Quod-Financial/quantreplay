#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_

#include <functional>
#include <string_view>
#include <vector>

#include "data_layer/api/converters/column_mapping.hpp"
#include "ih/historical/mapping/datasource_params.hpp"
#include "ih/historical/mapping/depth_config.hpp"
#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical::mapping {

class Configurator {
 public:
  using SourceColumnNames = std::vector<std::string>;

  Configurator(Specification& target_spec,
               SourceColumnNames source_columns,
               const DepthConfig& depth_config,
               const DatasourceParams& datasource_params);

  auto configure(const data_layer::converter::ColumnConfig& column_config)
      -> void;

 private:
  auto validate_depth_config(const DepthConfig& depth_config) -> void;

  auto associate(const data_layer::converter::ColumnConfig& columns_config)
      -> bool;

  [[nodiscard]]
  auto resolve_source(std::string_view column) const
      -> std::optional<SourceColumn>;

  [[nodiscard]]
  static auto resolve_by_name(const SourceColumnNames& columns,
                              std::string_view column_name)
      -> std::optional<SourceColumn>;

  [[nodiscard]]
  static auto resolve_by_number(std::string_view column_number)
      -> std::optional<SourceColumn>;

  auto configure_default_associations(std::uint32_t datasource_depth) -> void;

  SourceColumnNames source_columns_;
  std::reference_wrapper<Specification> spec_;
  std::uint32_t depth_;
  DatasourceParams datasource_params_;
};

}  // namespace simulator::generator::historical::mapping

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_
