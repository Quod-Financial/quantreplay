#include "ih/historical/mapping/params.hpp"

#include "core/common/unreachable.hpp"
#include "data_layer/api/converters/column_mapping.hpp"
#include "data_layer/api/validations/datasource.hpp"
#include "ih/historical/mapping/column_mapping_filter.hpp"
#include "ih/historical/mapping/configurator.hpp"
#include "ih/historical/mapping/csv_no_header_configurator.hpp"
#include "log/logging.hpp"

namespace simulator::generator::historical {

MappingParams::MappingParams(ColumnMappings mapping_configs) noexcept
    : column_mappings_{std::move(mapping_configs)} {}

auto MappingParams::initialize(std::uint32_t column_count,
                               std::uint32_t max_depth_levels) -> void {
  const mapping::CsvNoHeaderConfigurator configurator{column_count,
                                                      max_depth_levels};
  const auto column_configs = mapping::filter(column_mappings_);
  if (auto result = configurator.configure(column_configs);
      result.has_value()) {
    spec_ = std::move(*result);
  } else {
    throw std::invalid_argument{
        fmt::format("Invalid column mapping: {}", result.error())};
  }

  const auto datasource_id =
      column_mappings_.empty() ? 0 : column_mappings_.front().datasource_id();
  log::debug(
      "Historic column mapping has been configured for data_source_id `{}': {}",
      datasource_id,
      spec_);
}

auto MappingParams::initialize(ColumnNames column_names,
                               std::uint32_t max_depth_levels) -> void {
  mapping::Configurator configurator{std::move(column_names), max_depth_levels};

  const auto column_configs = mapping::filter(column_mappings_);
  if (auto result = configurator.configure(column_configs);
      result.has_value()) {
    spec_ = std::move(*result);
  } else {
    throw std::invalid_argument{
        fmt::format("Invalid column mapping: {}", result.error())};
  }

  const auto datasource_id =
      column_mappings_.empty() ? 0 : column_mappings_.front().datasource_id();
  log::debug(
      "Historic column mapping has been configured for data_source_id `{}': {}",
      datasource_id,
      spec_);
}

auto MappingParams::column_idx(data_layer::converter::ColumnFrom column) const
    -> std::optional<std::uint32_t> {
  const auto source_column = spec_.resolve_by(column);
  return source_column.has_value()
             ? std::make_optional<std::uint32_t>(source_column->index())
             : std::nullopt;
}

auto MappingParams::max_depth() const -> std::uint32_t {
  return spec_.max_depth();
}

auto make_mapping_params(const data_layer::Datasource& datasource)
    -> MappingParams {
  if (const auto datasource_valid = data_layer::validation::valid(datasource);
      !datasource_valid.has_value()) {
    throw std::invalid_argument(
        fmt::format("the datasource `{}' is invalid: {}",
                    datasource.datasource_id(),
                    datasource_valid.error()));
  }
  return MappingParams{datasource.columns_mapping()};
}

}  // namespace simulator::generator::historical