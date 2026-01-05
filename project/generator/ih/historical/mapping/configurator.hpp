#ifndef SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_
#define SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_

#include <functional>
#include <map>
#include <string>
#include <string_view>
#include <tl/expected.hpp>
#include <vector>

#include "data_layer/api/converters/column_mapping.hpp"
#include "ih/historical/mapping/specification.hpp"

namespace simulator::generator::historical::mapping {

class Configurator {
 public:
  using SourceColumnNames = std::vector<std::string>;

  Configurator(SourceColumnNames source_columns, std::uint32_t depth_config);

  auto configure(
      std::map<data_layer::converter::ColumnFrom, std::string> columns_config)
      -> tl::expected<Specification, std::string>;

 private:
  using ColumnToPrefix = std::string;

  [[nodiscard]]
  static auto resolve_by_name(const SourceColumnNames& columns,
                              std::string_view column_name)
      -> std::optional<SourceColumn>;

  [[nodiscard]]
  static auto extract_variable_depth_mapping(
      std::map<data_layer::converter::ColumnFrom, std::string>& columns_config)
      -> tl::expected<
          std::map<data_layer::converter::ColumnFrom, ColumnToPrefix>,
          std::string>;

  static auto unfold_variable_depth_mappings(
      const std::map<data_layer::converter::ColumnFrom, ColumnToPrefix>&
          variable_depth_mappings,
      std::map<data_layer::converter::ColumnFrom, std::string>& columns_config,
      std::uint32_t depth) -> void;

  [[nodiscard]]
  auto transform_variable_depth_mapping(
      std::map<data_layer::converter::ColumnFrom, std::string>& columns_config)
      -> tl::expected<void, std::string>;

  SourceColumnNames source_columns_;
  Specification spec_;
  std::uint32_t max_depth_levels_;
};

}  // namespace simulator::generator::historical::mapping

#endif  // SIMULATOR_GENERATOR_IH_HISTORICAL_MAPPING_CONFIGURATOR_HPP_
